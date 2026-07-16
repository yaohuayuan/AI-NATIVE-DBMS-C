# Core Containers 设计

## Status

| 模块 | 状态 |
|---|---|
| vector | implemented、tested、registered in CMake/CTest、cross-platform verified |
| list | implemented、tested、registered in CMake/CTest、cross-platform verified |
| string_utils | implemented、tested、registered in CMake/CTest、cross-platform verified |
| rbt | implemented、tested、registered in CMake/CTest、cross-platform verified |
| map | planned |
| byte_buffer | planned |

本表只记录本设计文档覆盖的模块；项目唯一完整状态矩阵见 [docs/README](README.md)。

## 1. 设计目标

Core Containers 为 Parser、Metadata、Plan、Record、File Manager 和 AI Runtime 提供统一动态集合与查找能力，避免业务模块各自手写扩容、节点、ownership 和错误路径。

统一规则：

- public symbol 使用 `aidb_` 前缀和 `snake_case`。
- 允许非指针 typedef，例如 `typedef struct aidb_vector aidb_vector;`。
- 禁止隐藏指针 typedef和自定义 `_t` 后缀类型。
- stack/embedded container 使用 `init/deinit`。
- 内部内存使用 `aidb_malloc`、`aidb_calloc`、`aidb_realloc`、`aidb_free`。
- status API 使用 `enum aidb_status` 和领域相关 out 参数。
- 每个容器明确元素、payload、key 和 value 的 ownership。

旧 DBMS_C 的容器只作为算法和行为参考；新项目不复制旧 API、命名或所有权模型。

## 2. vector

`aidb_vector` 是保存固定大小元素的连续动态数组：

```c
typedef struct aidb_vector aidb_vector;

struct aidb_vector {
    void *data;
    size_t size;
    size_t capacity;
    size_t element_size;
};
```

当前能力：

- `aidb_vector_init` / `aidb_vector_deinit`
- `aidb_vector_size` / `aidb_vector_capacity`
- `aidb_vector_reserve` / `aidb_vector_push_back`
- `aidb_vector_at` / `aidb_vector_at_const`
- `aidb_vector_clear`

vector 按值复制 `element_size` 字节，不自动析构元素内部指针。越界或无效访问通过 pointer `NULL` 表达；会失败的变更操作返回 `enum aidb_status`。

`vector_test` 已覆盖初始化、扩容、插入、访问、清空和错误路径，并注册到默认 CTest。

## 3. list

`aidb_list` 用于节点地址稳定、两端插入/删除或不需要连续存储的场景：

```c
typedef struct aidb_list_node aidb_list_node;
typedef struct aidb_list aidb_list;

struct aidb_list {
    aidb_list_node *head;
    aidb_list_node *tail;
    size_t size;
    size_t element_size;
};
```

当前能力：

- `aidb_list_init` / `aidb_list_deinit`
- `aidb_list_size` / `aidb_list_is_empty`
- `aidb_list_push_front` / `aidb_list_push_back`
- `aidb_list_pop_front` / `aidb_list_pop_back`
- `aidb_list_front` / `aidb_list_back` 及 const 版本
- `aidb_list_clear`

节点布局保持 internal。list 按值保存元素，不自动释放元素内部指针；intrusive list 如未来需要，应单独设计。

`list_test` 已覆盖两端操作、访问、清空、空列表和错误路径，并注册到默认 CTest。

## 4. string_utils

当前字符串工具保持小而明确，不引入复杂字符串对象或完整 Unicode 层：

- `aidb_string_duplicate` / `aidb_string_duplicate_n`
- `aidb_string_equal`
- `aidb_string_starts_with` / `aidb_string_ends_with`
- `aidb_string_to_lower_ascii` / `aidb_string_to_upper_ascii`
- `aidb_string_concat`

返回 `char *` 的复制函数产生 owned string，调用方用 `aidb_free` 释放；分配或非法输入失败时返回 `NULL`。`aidb_string_concat` 使用 `enum aidb_status` 和 `char **out_text`，失败时输出保持为文档规定的安全状态。大小写转换只处理 ASCII。

`string_utils_test` 已覆盖复制、比较、前后缀、拼接、ASCII 大小写和 NULL/失败语义，并注册到默认 CTest。

## 5. rbt（implemented and locally tested）

`rbt.h` 和 `rbt.c` 已进入 `aidb_core`。`rbt_test` 与 `rbt_oom_test` 已注册到默认 CTest，并通过本地 MinGW Debug 及 Windows/MSVC、Ubuntu/GCC、macOS/AppleClang 验证。

当前契约是：

- caller-owned tree 使用 `init/deinit`；
- tree 拥有内部 sentinel 和 node；
- stored payload 是 borrowed，tree 不复制或释放 payload；
- compare callback 提供严格排序；
- duplicate 不重复插入，并通过领域 out 参数报告；
- pointer-returning find 在未找到或失败时返回 `NULL`；
- validate 检查根节点、sentinel、父子链接、排序、红色节点、black height 和 size 不变量；删除返回 borrowed payload。

正式测试覆盖参数错误、空树、插入、查找、重复、不同删除形态、排序压力、固定种子的 10,000 次随机操作，以及初始化和插入的确定性 OOM 路径。

## 6. map（planned）

map 计划建立在已验证的 rbt 之上，为业务模块提供 key-value 语义。设计必须先明确：

- key/value 是按值复制、borrowed 还是由回调管理；
- duplicate key 的替换规则；
- compare callback 和字符串 key helper；
- get 返回值的 borrowed/owned 语义；
- clear/deinit 的析构责任。

rbt 已满足 map 的前置实现与验证要求；map public API 仍须在独立 B005 批次中明确契约后实现。

## 7. byte_buffer（planned）

byte_buffer 负责 data、capacity、position/limit 和 bounds error，并依赖已实现的 binary encoding。它不属于当前已完成容器；详细边界见 [Binary Encoding](16_binary_encoding_design.md)。

## 8. 测试与后续依赖

vector/list/string_utils/rbt 的测试均已接入当前三平台 workflow。每个 public header 也由独立翻译单元执行重复 include 测试；该项当前已在本地通过，三平台 CI 尚待当前改动推送后验证。map、byte_buffer 也必须分别具备独立测试，不能仅由未来业务模块间接覆盖。

后续依赖建议：

- Parser/Plan 使用 vector 组织 token、表达式和节点列表。
- queue/等待或稳定节点场景按需要使用 list。
- Metadata/File Manager 在 map 完成后使用明确的 key-value 抽象。
- Page/File/Log/Record 使用 byte_buffer 和 binary，不能各自散落编码逻辑。
- AI Runtime 使用 containers 组织 prompt 片段、候选结果和调用上下文。
