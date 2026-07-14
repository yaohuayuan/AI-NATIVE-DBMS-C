# memory + arena 设计

## Status

- `memory`：implemented、tested、registered in CMake/CTest、cross-platform verified。
- `arena`：implemented、tested、registered in CMake/CTest、cross-platform verified。
- platform alignment：internal implementation，随 arena 构建并由 `arena_test` 覆盖，cross-platform verified。

本文记录当前实现所依据的设计边界。项目坚持“必要复杂，而非过度设计”：处理生命周期、对齐、失败路径和测试，但不提前加入 pool allocator、leak tracker、allocator vtable、线程安全 allocator、引用计数或 GC。

## 1. memory 模块

memory wrapper 统一项目堆内存入口，避免标准库分配函数散落在业务模块。

```c
void *aidb_malloc(size_t size);
void *aidb_calloc(size_t count, size_t size);
void *aidb_realloc(void *ptr, size_t size);
void aidb_free(void *ptr);
```

当前实现是标准库的 thin wrapper。统一入口为后续故障注入、统计、debug 检查或 allocator 策略留下边界，但这些能力现在没有实现。

### 行为规则

- `aidb_free(NULL)` 安全。
- `aidb_malloc(0)` 遵循 C 标准库实现行为；调用方和测试不得依赖返回值一定为 `NULL` 或非 `NULL`。
- `aidb_realloc` 失败时不会替调用方保存新指针，调用方必须通过临时变量避免丢失原指针。
- pointer-returning allocation API 失败时返回 `NULL`。
- wrapper 不直接写入 context；领域调用方负责把分配失败转为 `AIDB_ERROR_OUT_OF_MEMORY`。

## 2. arena 模块

arena 是短生命周期批量分配器，适合 parser token、AST、plan 临时节点、predicate/expression、JSON 解析节点和 AI prompt 临时数据。

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
void *aidb_arena_alloc(struct aidb_arena *arena, size_t size);
```

arena 多次分配但不逐对象释放：`reset` 复用全部 block，`deinit` 释放整个 block 链。

## 3. 内部 block 与增长

内部 block 记录 `next`、`capacity`、`used` 和柔性数组数据区。当前 block 不足时，新 block 容量取默认 block 大小与本次请求大小中的较大者。

关键约束：

- 创建新 block 失败时返回 `NULL`，已有 block 链保持有效。
- 初始化失败或部分使用后的 arena 必须可以安全清理。
- arena 不跟踪单个分配对象的析构函数，也不负责对象内部资源。

## 4. 对齐

arena 通过 internal `src/platform/align.h` 使用 `AIDB_PLATFORM_MAX_ALIGN`，为当前普通标量和指针类型提供跨编译器对齐基线。该实现不是 public API；只有出现明确的更高对齐需求时，才设计公开 aligned allocation 接口。

## 5. NULL 与零长度

- `aidb_arena_init(NULL, size)` 或 `default_block_size == 0` 返回 `AIDB_ERROR_INVALID_ARGUMENT`。
- `aidb_arena_alloc(NULL, size)` 和 `aidb_arena_alloc(arena, 0)` 返回 `NULL`。
- `aidb_arena_reset(NULL)`、`aidb_arena_deinit(NULL)` 安全。
- `deinit` 释放所有 block 并清空 arena 字段；`reset` 清零各 block 的使用量并回到第一块。

## 6. 测试

`memory_test` 覆盖基础分配、calloc 清零、realloc 和 `free(NULL)`。`arena_test` 覆盖非法初始化、小对象、连续分配、大对象、对齐、reset、deinit 及 NULL/零长度路径。

两个测试均注册到默认 CTest，并由当前 Windows/MSVC、Ubuntu/GCC、macOS/AppleClang workflow 验证。

## 7. 后续边界

- containers 和后续 DBMS 模块使用 memory wrapper，不直接散落 `malloc/free`。
- parser、plan、JSON 和 AI prompt 可以使用 arena 管理单次操作生命周期。
- pool、leak tracking、allocator vtable 和线程安全只在有可测试需求时单独设计。
