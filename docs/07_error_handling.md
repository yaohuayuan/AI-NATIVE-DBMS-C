# 错误处理

错误处理必须让调用者知道失败原因。不要到处 `return -1` / `NULL` 后不说明原因。

## 状态类型

后续统一使用 `aidb_status` 或类似结果类型表达成功/失败。当前文档只冻结方向，具体类型在 `v0.1.2` 实现。

常见错误类别包括：

- `AIDB_OK`
- `AIDB_ERROR_INVALID_ARGUMENT`
- `AIDB_ERROR_OUT_OF_MEMORY`
- `AIDB_ERROR_IO`
- `AIDB_ERROR_PARSE`
- `AIDB_ERROR_NOT_FOUND`
- `AIDB_ERROR_INTERNAL`

## 函数返回值规则

- 简单状态函数返回 `aidb_status`。
- 输出对象通过 `out` 参数返回。
- 返回指针时必须明确 `owned/borrowed/view`。
- 入参非法时返回明确错误，不依赖崩溃暴露问题。

## 错误信息

- 后续可由 `aidb_context` 记录最近错误。
- 不在底层库随意 `printf/fprintf`。
- CLI 或 shell 层可以负责把错误转成人类可读输出。

## 测试要求

- 测试应覆盖错误路径。
- 内存分配失败、非法参数、解析失败、文件不存在等路径都应逐步补齐。
- 不允许只测试 happy path 后宣称模块完成。
