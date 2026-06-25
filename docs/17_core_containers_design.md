# v0.1.6 Core Containers 设计

## 1. 阶段目标

`v0.1.6` 只做 Core Containers 的设计文档，不实现任何容器代码，也不创建新的头文件或源文件。

Core Containers 是后续 Parser、Metadata、Plan、Record、File Manager、AI Runtime 的基础设施。DBMS 不能长期只依赖 `malloc/free` 和裸数组：裸数组很难统一处理扩容、元素拷贝、所有权、错误返回和测试边界；业务模块各自手写容器逻辑，也会让内存语义分散到多个地方。

本阶段先明确容器边界、命名规范、所有权规则和实现顺序，让后续每个容器都能小步实现、小步测试。

## 2. 为什么需要 Core Containers

后续模块会频繁处理动态集合和 key-value 查询：

- `struct aidb_vector` 用于动态数组、字段列表、token 列表、plan 列表、表达式子节点列表。
- `string_utils` 用于字符串复制、比较、拼接、大小写处理，以及稳定的字符串所有权转移。
- `struct aidb_list` 用于链式结构、等待队列、回收列表、日志链等不适合频繁移动元素的场景。
- `struct aidb_rbt` 和 `struct aidb_map` 用于 key-value 映射、字段 offset 查找、文件句柄缓存、元数据缓存等需要稳定查找语义的场景。

这些能力应集中在 core 层，而不是散落在 parser、storage、metadata 或 ai runtime 内部。

## 3. 和旧项目数据结构的关系

旧项目中已经有过动态数组、链表、映射和红黑树等数据结构经验。新项目不直接照搬旧命名、旧接口或旧所有权模型，而是按当前 C 项目规范重新设计：

- 所有公开符号使用 `aidb_` 前缀。
- 函数名使用 `snake_case`。
- 失败路径优先返回 `enum aidb_status`。
- 公开结构体保留 `struct aidb_vector`、`struct aidb_list`、`struct aidb_map`、`struct aidb_rbt` tag。
- 公开结构体允许增加非指针 typedef，例如 `typedef struct aidb_vector aidb_vector;`。
- 生命周期使用 `init/deinit` 语义。
- 堆内存统一通过 `aidb_malloc`、`aidb_calloc`、`aidb_realloc`、`aidb_free`。
- API 文档必须明确元素、key、value、字符串和节点的所有权。

本项目允许非指针 typedef，但不使用隐藏结构体指针的 typedef，也不定义自定义 `_t` 后缀类型名。函数声明和定义中应把指针写清楚，例如 `aidb_vector *vector`，而不是把指针藏进类型别名。

## 4. 分阶段实现顺序

建议实现节奏如下：

- `v0.1.6`：Core Containers 设计文档。
- `v0.1.7`：`struct aidb_vector`。
- `v0.1.8`：`string_utils`。
- `v0.1.9`：`struct aidb_list`。
- `v0.1.10`：`struct aidb_rbt` + `struct aidb_map`。
- `v0.1.11`：`byte_buffer`。
- `v0.1.12`：`block_id` + `page` + `file_manager` 最小存储层。

不一次性实现所有容器，原因是：

- `struct aidb_vector` 最简单，适合先建立泛型元素存储规则、扩容规则和析构规则。
- `string_utils` 简单但高频，应该尽早确定字符串复制、比较和释放的所有权语义。
- `struct aidb_list` 适合链式场景，但不是所有列表都应该使用链表；先有 vector 能避免过早依赖链表。
- `struct aidb_rbt` 和 `struct aidb_map` 最复杂，放在 vector/list 之后更稳，也便于复用已验证的内存和测试模式。

## 5. aidb_vector 设计草案

`struct aidb_vector` 是连续内存动态数组，保存固定大小元素。它不理解元素内部结构，只按 `element_size` 拷贝字节。

```c
typedef struct aidb_vector aidb_vector;

struct aidb_vector {
    void *data;
    size_t size;
    size_t capacity;
    size_t element_size;
};

enum aidb_status aidb_vector_init(aidb_vector *vector, size_t element_size);
void aidb_vector_deinit(aidb_vector *vector);

enum aidb_status aidb_vector_reserve(aidb_vector *vector, size_t capacity);
enum aidb_status aidb_vector_push_back(aidb_vector *vector, const void *element);

void *aidb_vector_at(aidb_vector *vector, size_t index);
const void *aidb_vector_at_const(const aidb_vector *vector, size_t index);

void aidb_vector_clear(aidb_vector *vector);
```

初始规则：

- `aidb_vector_init` 不预分配或只做最小初始化，具体策略在实现阶段决定。
- `aidb_vector_push_back` 按值拷贝 `element_size` 字节。
- vector 不自动析构元素内部指针；如元素拥有额外资源，调用方负责在 `clear/deinit` 前处理。
- `aidb_vector_at` 越界时第一版可返回 `NULL`，后续也可以提供返回 `enum aidb_status` 的安全访问接口。

## 6. string_utils 设计草案

字符串工具层不做复杂字符串对象，第一版只提供高频、低风险函数：

```c
char *aidb_string_duplicate(const char *text);
int aidb_string_equal(const char *left, const char *right);
enum aidb_status aidb_string_concat(const char *left, const char *right, char **out);
void aidb_string_to_lower_ascii(char *text);
void aidb_string_to_upper_ascii(char *text);
```

设计规则：

- `aidb_string_duplicate` 返回由 `aidb_malloc` 分配的新字符串，调用方用 `aidb_free` 释放。
- `aidb_string_concat` 通过 `out` 返回新分配字符串，失败时不泄漏内存。
- 大小写转换第一版只处理 ASCII，避免提前引入 Unicode 复杂度。
- NULL 参数策略在每个函数文档中明确；返回 `enum aidb_status` 的函数负责给出可测试的错误路径。

## 7. aidb_list 设计草案

`struct aidb_list` 用于节点稳定、插入删除频繁、元素不需要连续存储的场景。

```c
struct aidb_list_node;

typedef struct aidb_list aidb_list;

struct aidb_list {
    struct aidb_list_node *head;
    struct aidb_list_node *tail;
    size_t size;
    size_t element_size;
};

enum aidb_status aidb_list_init(aidb_list *list, size_t element_size);
void aidb_list_deinit(aidb_list *list);

enum aidb_status aidb_list_append(aidb_list *list, const void *element);
enum aidb_status aidb_list_prepend(aidb_list *list, const void *element);
void aidb_list_clear(aidb_list *list);
```

初始规则：

- list 节点内部结构不对外暴露。
- list 按值保存元素，不自动释放元素内部指针。
- 如未来需要 intrusive list，应作为单独设计，不混入第一版 `struct aidb_list`。

## 8. aidb_rbt 与 aidb_map 设计草案

`struct aidb_rbt` 是有序树基础结构，`struct aidb_map` 是面向业务的 key-value 映射。第一版可以先让 map 使用 rbt 实现，但对外只暴露 map 语义。

```c
typedef struct aidb_rbt aidb_rbt;
typedef struct aidb_map aidb_map;

struct aidb_rbt;

enum aidb_status aidb_rbt_init(
    aidb_rbt *tree,
    int (*compare)(const void *left, const void *right)
);
void aidb_rbt_deinit(aidb_rbt *tree);

struct aidb_map {
    aidb_rbt *tree;
    size_t key_size;
    size_t value_size;
    int (*compare)(const void *left, const void *right);
};

enum aidb_status aidb_map_init(
    aidb_map *map,
    size_t key_size,
    size_t value_size,
    int (*compare)(const void *left, const void *right)
);
void aidb_map_deinit(aidb_map *map);

enum aidb_status aidb_map_put(aidb_map *map, const void *key, const void *value);
void *aidb_map_get(aidb_map *map, const void *key);
int aidb_map_contains(const aidb_map *map, const void *key);
```

说明：

- compare 参数使用显式函数指针声明，不通过 typedef 隐藏指针。
- `struct aidb_map` 按值拷贝 key 和 value。
- 对字符串 key 的 map 可以后续单独提供 helper，但第一版 map 不隐式复制字符串内容。
- 删除、迭代器和范围查询可以后续扩展，不进入第一版最小接口。

## 9. 所有权与错误处理

Core Containers 统一遵守以下规则：

- 所有容器自身由调用方提供存储，使用 `init/deinit` 管理内部资源。
- 容器内部堆内存统一使用 memory wrapper。
- 返回 `enum aidb_status` 的函数必须能表达 `AIDB_OK`、`AIDB_ERROR_INVALID_ARGUMENT`、`AIDB_ERROR_OUT_OF_MEMORY` 等基础错误。
- `deinit(NULL)` 和 `clear(NULL)` 是否安全由各模块文档明确，测试跟随文档。
- 容器默认按值保存元素，不拥有元素内部指针。
- 需要深拷贝或析构回调时，后续另行设计，不塞进第一版基础容器。

## 10. 测试计划

每个容器都应有独立测试文件：

- `tests/core/vector_test.c`
- `tests/core/string_utils_test.c`
- `tests/core/list_test.c`
- `tests/core/rbt_test.c`
- `tests/core/map_test.c`

测试原则：

- 不访问文件。
- 不联网。
- 不依赖真实 AI API。
- 不测试实现内部节点布局。
- 优先测试公开 API 的行为、边界和失败路径。
- 所有测试进入默认 CTest，保持可重复、离线、无费用。

## 11. 和后续模块的关系

Core Containers 会成为后续模块的共同基础：

- Parser 使用 vector 保存 token、表达式节点列表和解析结果集合。
- Metadata 使用 map 保存表、字段、索引和约束信息。
- Plan 使用 vector/list 组织计划节点和候选计划。
- Record 使用 string_utils 处理字段名、文本值和序列化辅助字符串。
- File Manager 使用 map 缓存文件句柄和页元数据。
- AI Runtime 使用 vector/list 管理 prompt 片段、候选结果和推理上下文。

后续 Page、File、Log、Record 不应各自手写动态数组、链表或映射。容器逻辑集中在 core 层，业务模块只表达自己的领域语义。
