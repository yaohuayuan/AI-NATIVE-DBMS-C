# 内存与所有权

本文冻结后续内存管理和所有权表达方式。当前阶段只定义规范，不创建 `memory.c`、`arena.c` 或相关实现。

## 内存总原则

- 不禁止 `malloc/free`。
- 但 `malloc/free` 不允许散落到各业务模块。
- 原则上只有 `src/core/memory.c` 直接调用 `malloc/calloc/realloc/free`。
- 其他模块使用 `aidb_malloc`、`aidb_calloc`、`aidb_realloc`、`aidb_free`。

## 分配策略

- 长生命周期对象：`aidb_malloc` / `aidb_free`。
- 短生命周期批量对象：`aidb_arena_alloc`。
- 高频固定大小对象：后期考虑 pool allocator。

## 适合 arena 的场景

- Parser token。
- AST。
- Plan 临时节点。
- Predicate/Expression。
- JSON 解析临时节点。
- AI Prompt 构造临时字符串。
- 单次查询生命周期对象。

## 所有权语义

- `owned`：调用者拥有，需要释放。
- `borrowed`：借用，不允许释放。
- `view`：只读视图，不拥有内存。

## 命名语义

- `create/destroy`：堆对象创建和销毁。
- `init/deinit`：调用者提供对象内存，只初始化和释放内部资源。
- `clone`：深拷贝，返回 `owned` 对象。
- `borrow/view`：返回借用或只读视图。

## 错误路径

- 函数失败时不能泄漏已分配资源。
- 失败时需要保持对象处于可销毁状态。
- `destroy/deinit` 应尽量允许 `NULL` 或半初始化对象安全处理，具体规则以后实现时确定。
- 涉及多个资源的函数应优先设计清晰的清理路径，避免正常路径和失败路径行为不一致。
