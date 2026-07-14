# Core Foundation 设计

## Status

- `error`：implemented、tested、registered in CMake/CTest、cross-platform verified。
- `context`：implemented、tested、registered in CMake/CTest、cross-platform verified。
- 独立 generic `aidb_result` 模块：当前不规划；统一使用 `enum aidb_status` 和领域相关 out 参数。

本文保留 Core Foundation 的契约和设计理由。详细项目状态以 [文档索引与模块状态矩阵](README.md) 为准。

## 1. 设计目标

Core Foundation 先统一三类跨模块约定：

- 稳定的错误码和可读错误信息；
- 函数成功/失败及结果返回方式；
- 可由上层携带的运行上下文和最近错误。

这些约定供 memory、containers、DBMS、JSON 和 AI runtime 复用，避免各模块自行定义 `return -1`、含糊的 `NULL` 或隐藏的所有权规则。

## 2. error 模块

`enum aidb_status` 是当前统一状态类型：

```c
enum aidb_status {
    AIDB_OK = 0,
    AIDB_ERROR_INVALID_ARGUMENT,
    AIDB_ERROR_OUT_OF_MEMORY,
    AIDB_ERROR_IO,
    AIDB_ERROR_PARSE,
    AIDB_ERROR_NOT_FOUND,
    AIDB_ERROR_INTERNAL
};
```

当前 public API：

```c
const char *aidb_status_name(enum aidb_status status);
const char *aidb_status_message(enum aidb_status status);
bool aidb_status_is_ok(enum aidb_status status);
bool aidb_status_is_error(enum aidb_status status);
```

设计约束：

- `AIDB_OK` 表示成功，其他值表示失败。
- 错误码不能依赖模块内部临时数字值。
- 底层库不直接打印错误；CLI 或调用者决定怎样展示。
- `AIDB_ERROR_INTERNAL` 只用于不应发生或无法进一步分类的内部错误。

## 3. 结果返回约定

C 语言函数不需要通过独立 generic result 容器才能统一错误处理。当前规则是：

- 状态 API 返回 `enum aidb_status`。
- 领域结果通过明确类型的 out 参数返回。
- pointer-returning lookup/allocation API 失败时返回 `NULL`。
- `deinit/destroy` 等 cleanup API 返回 `void`。
- 指针结果必须说明 `owned`、`borrowed` 或 `view`。

示例：

```c
enum aidb_status aidb_x_init(aidb_x *value);
void aidb_x_deinit(aidb_x *value);

enum aidb_status aidb_x_lookup(
    const aidb_x *value,
    const void *key,
    const void **out_item
);
```

required argument 为 `NULL` 时，status API 返回 `AIDB_ERROR_INVALID_ARGUMENT`。out 参数在失败后的状态应由具体接口记录；返回 owned object 的函数通常在失败时把输出置为 `NULL`。

## 4. context 模块

当前 `aidb_context` 是可放在 stack 或嵌入其他对象的结构，保存最近状态和固定容量错误消息：

```c
struct aidb_context {
    enum aidb_status last_status;
    char last_error_message[AIDB_CONTEXT_ERROR_MESSAGE_CAPACITY];
};
```

当前 public API：

```c
enum aidb_status aidb_context_init(struct aidb_context *context);
void aidb_context_deinit(struct aidb_context *context);
void aidb_context_clear_error(struct aidb_context *context);

enum aidb_status aidb_context_set_error(
    struct aidb_context *context,
    enum aidb_status status,
    const char *message
);

enum aidb_status aidb_context_last_status(const struct aidb_context *context);
const char *aidb_context_error_message(const struct aidb_context *context);
```

当前 context 不承担 allocator、logger、trace、config 或 AI runtime 的全部职责。未来只有出现明确跨模块需求时才扩展，避免把 context 变成无边界的全局对象。

## 5. 生命周期与所有权

- stack/embedded object 使用 `init/deinit`。
- heap-owned object 只有确有必要时才使用 `create/destroy`。
- `owned` 由接收方负责释放；`borrowed` 不得释放；`view` 是不拥有内存的只读视图。
- 清理函数是否接受 `NULL` 必须由模块文档和测试共同确认。
- 初始化失败不能泄漏资源；对象必须处于可安全清理状态，或明确说明无需清理。

## 6. 测试与完成标准

`error_test` 和 `context_test` 已注册到默认 CTest，并随 `aidb_core` 在当前三平台 workflow 中验证。测试覆盖状态映射、状态判断、context 初始化、设置、读取、清空和非法参数。

后续模块必须复用这些契约，不得重新建立冲突的错误码体系或独立 generic result foundation。

## 7. 后续依赖

- memory 和 containers 将分配失败映射为 `AIDB_ERROR_OUT_OF_MEMORY` 或 pointer `NULL`。
- storage、parser、transaction 和 executor 使用领域相关状态与 out 参数。
- AI runtime 的 provider、cache 和 operator 错误也通过相同基础状态向上传递，并由上层记录更具体的上下文。
