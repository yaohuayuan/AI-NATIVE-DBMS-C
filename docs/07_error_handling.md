# 错误处理

## Status

`error` 和 `context` 已实现、具有独立测试、接入 CMake/CTest，并由当前 GitHub Actions workflow 在 Windows/MSVC、Ubuntu/GCC、macOS/AppleClang 上验证。

## 状态类型

项目统一使用 `enum aidb_status`：

- `AIDB_OK`
- `AIDB_ERROR_INVALID_ARGUMENT`
- `AIDB_ERROR_OUT_OF_MEMORY`
- `AIDB_ERROR_IO`
- `AIDB_ERROR_PARSE`
- `AIDB_ERROR_NOT_FOUND`
- `AIDB_ERROR_INTERNAL`

`aidb_status_name` 和 `aidb_status_message` 提供稳定名称与可读说明；`aidb_status_is_ok`、`aidb_status_is_error` 用于状态判断。

## 返回规则

- 一般状态函数返回 `enum aidb_status`。
- 结果通过领域相关 out 参数返回；当前不规划独立的通用 `aidb_result` 模块。
- required argument 为 `NULL` 时，status API 返回 `AIDB_ERROR_INVALID_ARGUMENT`。
- pointer-returning lookup/allocation API 失败时返回 `NULL`。
- 返回指针时必须明确 `owned`、`borrowed` 或 `view`。
- 失败时不能泄漏资源，out 参数的失败后状态必须在接口文档中定义。

## Context

`aidb_context` 保存最近一次 `aidb_status` 和固定容量的错误消息。当前接口使用 stack/embedded 生命周期：

- `aidb_context_init` / `aidb_context_deinit`
- `aidb_context_set_error` / `aidb_context_clear_error`
- `aidb_context_last_status`
- `aidb_context_error_message`

底层库不随意 `printf/fprintf`；CLI 或更高层负责把状态和 context 信息转换为人类可读输出。

## 清理 API

- stack 或 embedded object 使用 `init/deinit`。
- heap-owned object 只有在确有需要时使用 `create/destroy`。
- cleanup API 是否接受 `NULL` 必须由对应模块记录并测试；已记录为安全的清理函数可以接受 `NULL`。

## 测试要求

- `error_test` 覆盖状态名称、消息和状态判断。
- `context_test` 覆盖初始化、设置、读取、清空和非法参数路径。
- 后续模块必须覆盖 invalid argument、out of memory、not found 及其领域错误路径，不能只验证 happy path。
