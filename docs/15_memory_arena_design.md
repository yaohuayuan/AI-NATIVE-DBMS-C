# v0.1.4 memory + arena 设计文档

本文对应 `v0.1.4`，只设计 `memory wrapper` 和 `arena allocator`。本文不创建 `memory.c`、`arena.c`、`memory.h`、`arena.h`，也不实现任何新功能代码。

本项目遵循“必要复杂，而非过度设计”的原则。奥卡姆剃刀不是拒绝复杂，而是拒绝无意义复杂。`memory/arena` 属于 C 系统基础能力，应该认真处理边界、生命周期、对齐、失败路径和测试覆盖；但不应在第一版提前做 pool allocator、leak tracker、allocator vtable、线程安全 allocator 或虚拟内存管理。

## 1. 阶段目标

`v0.1.4` 只做两个模块：

- memory wrapper
- arena allocator

本阶段不做：

- vector
- string_utils
- json
- net
- ai
- dbms
- pool allocator
- leak tracker
- allocator vtable
- 线程安全 allocator
- 引用计数
- GC

第一版必须考虑 `size_t`、0 字节分配、`NULL` 参数、arena `reset/deinit`、失败路径、对齐和测试覆盖。

## 2. memory 模块职责

memory 模块用于统一堆内存入口，避免 `malloc/free` 散落在业务模块。

建议接口草案如下，仅作为文档设计，不创建头文件：

```c
void *aidb_malloc(size_t size);
void *aidb_calloc(size_t count, size_t size);
void *aidb_realloc(void *ptr, size_t size);
void aidb_free(void *ptr);
```

第一版内部只是 thin wrapper：

- `aidb_malloc` -> `malloc`
- `aidb_calloc` -> `calloc`
- `aidb_realloc` -> `realloc`
- `aidb_free` -> `free`

统一入口方便后续加入：

- 统计
- debug 检查
- 故障注入
- leak 检测
- 自定义 allocator

## 3. memory 模块边界规则

- `aidb_free(NULL)` 应安全。
- `aidb_malloc(0)` 的规则需要明确，建议第一版直接遵循 C 标准库行为，但测试不依赖具体返回值。
- `aidb_calloc` 需要考虑 `count * size` 溢出风险，第一版可以先用标准 `calloc`，后续再加显式溢出检查。
- `aidb_realloc` 失败时不能丢失旧指针，调用方必须用临时指针接收。
- memory wrapper 不负责记录 context 错误，调用方自己把 `NULL` 转成 `AIDB_ERROR_OUT_OF_MEMORY`。

## 4. arena 模块职责

arena 是短生命周期批量分配器，适合：

- parser token
- AST 节点
- plan 临时节点
- predicate/expression
- JSON 解析临时节点
- AI prompt 构造临时字符串
- 单次 query 生命周期对象

arena 的核心特点：

- 多次 alloc
- 不逐个 free
- `reset` 一次性复用
- `deinit` 一次性释放所有 block

## 5. arena 第一版接口草案

以下只写在文档中，不创建头文件：

```c
struct aidb_arena_block;

struct aidb_arena {
    struct aidb_arena_block *first_block;
    struct aidb_arena_block *current_block;
    size_t default_block_size;
};

enum aidb_status aidb_arena_init(
    struct aidb_arena *arena,
    size_t default_block_size
);

void aidb_arena_deinit(struct aidb_arena *arena);

void aidb_arena_reset(struct aidb_arena *arena);

void *aidb_arena_alloc(
    struct aidb_arena *arena,
    size_t size
);
```

第一版不做 `alloc_aligned` 公开接口，内部先按 `sizeof(void *)` 做最小对齐。后续如果需要，再增加 `aidb_arena_alloc_aligned`。

## 6. arena 内部 block 设计

内部结构建议如下，仅作为文档说明：

```c
struct aidb_arena_block {
    struct aidb_arena_block *next;
    size_t capacity;
    size_t used;
    unsigned char data[];
};
```

- `capacity` 表示 `data` 可用容量。
- `used` 表示当前已使用字节数。
- `next` 连接多个 block。
- 如果当前 block 空间不足，则创建新 block。
- 新 block 大小应至少能容纳本次申请。

## 7. 对齐规则

- 第一版按 `sizeof(void *)` 对齐。
- 对齐函数概念为 `align_up(value, alignment)`。
- `alignment` 必须是 2 的幂。
- `v0.1.4` 第一版不开放复杂对齐接口，避免过早设计。

## 8. 0 字节规则

建议规则：

- `aidb_arena_alloc(arena, 0)` 返回 `NULL`。
- `aidb_arena_alloc(NULL, size)` 返回 `NULL`。
- `default_block_size == 0` 时，`aidb_arena_init` 返回 `AIDB_ERROR_INVALID_ARGUMENT`。

## 9. reset 和 deinit 区别

- `reset`：不释放 block，只把 `used` 清零，便于复用。
- `deinit`：释放所有 block，并把 arena 字段清空。
- `deinit(NULL)` 安全。
- `reset(NULL)` 安全。

## 10. 失败路径

- 初始化失败时 arena 应处于可安全 `deinit` 状态。
- block 分配失败时 `aidb_arena_alloc` 返回 `NULL`。
- 创建新 block 失败时不能破坏已有 block 链。
- 后续调用方需要把 `NULL` 转成 `AIDB_ERROR_OUT_OF_MEMORY`。

## 11. 测试计划

后续规划测试：

- `tests/core/memory_test.c`
- `tests/core/arena_test.c`

`memory_test` 覆盖：

- `aidb_malloc` 正常分配
- `aidb_free(NULL)`
- `aidb_calloc` 初始化为 0
- `aidb_realloc` 扩容
- `aidb_malloc(0)` 不依赖具体返回值，只保证可安全 free 返回值

`arena_test` 覆盖：

- `aidb_arena_init(NULL, size)` 返回 invalid argument
- `aidb_arena_init(&arena, 0)` 返回 invalid argument
- `aidb_arena_init(&arena, valid_size)` 返回 OK
- `aidb_arena_alloc` 小对象
- 连续分配多个对象
- 分配超过默认 block 的大对象
- `aidb_arena_alloc(NULL, size)` 返回 `NULL`
- `aidb_arena_alloc(&arena, 0)` 返回 `NULL`
- `reset` 后可以重新分配
- `deinit(NULL)` 不崩溃
- `deinit(&arena)` 不崩溃

## 12. 和后续模块关系

- `vector/string_utils` 将依赖 memory wrapper。
- `parser/json/plan/AI prompt` 构造会使用 arena。
- DBMS Core 后续模块不得直接散落 `malloc/free`。
- memory + arena 是后续复杂模块的基础。
