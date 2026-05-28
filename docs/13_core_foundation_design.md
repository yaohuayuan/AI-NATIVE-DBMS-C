# Core Foundation 设计文档

本文对应 `v0.1.2`。它是 Core Foundation 的设计文档，不包含实现代码，不创建任何 `.c/.h` 功能实现文件，也不修改 CMake 或现有测试。

下一步实现版本为 `v0.1.3 error/result/context` 最小实现。

## 一、v0.1.2 目标

`v0.1.2` 只处理 Core Foundation 的第一批模块设计：

- `error`
- `result`
- `context`

这三个模块的目标是先把错误表达、函数返回约定和运行上下文边界统一起来，为后续 core、DBMS、JSON、AI Runtime 等模块提供稳定基础。

本阶段暂不处理：

- `memory`
- `arena`
- `vector`
- `string_utils`
- `json`
- `net`
- `ai`
- `dbms`

这些模块会依赖本阶段形成的错误处理和上下文约定，但不应在本阶段提前实现。

## 二、error 模块设计

`error` 模块负责统一项目错误码，避免各业务模块随意 `return -1`、返回 `NULL` 后不说明原因，或在底层库中直接 `printf/fprintf`。它也为后续 `context` 记录最近错误信息做准备。

建议错误码草案：

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

未来可能需要的辅助接口：

```c
const char *aidb_status_to_string(enum aidb_status status);
bool aidb_status_is_ok(enum aidb_status status);
bool aidb_status_is_error(enum aidb_status status);
```

设计约束：

- `AIDB_OK` 表示成功，其他值表示失败。
- 错误码应足够稳定，避免业务模块依赖临时数字值。
- 底层库不直接输出错误文本，错误展示由 CLI、shell 或上层调用者负责。
- 未知或不应发生的错误归入 `AIDB_ERROR_INTERNAL`，但测试和日志应尽量保留定位信息。

## 三、result 模块设计

C 语言没有异常机制，函数需要统一表达成功/失败。`result` 模块不一定要在第一阶段引入复杂泛型结构，重点是明确返回规则：状态通过 `aidb_status` 返回，输出对象通过 `out` 参数返回。

建议规则：

- 简单状态函数返回 `aidb_status`。
- 创建对象函数返回 `aidb_status`，并通过 `out` 参数返回对象指针。
- `destroy/deinit` 函数返回 `void`。
- 返回指针时必须说明 `owned/borrowed/view`。

接口草案示例：

```c
enum aidb_status aidb_xxx_init(struct aidb_xxx *xxx);
void aidb_xxx_deinit(struct aidb_xxx *xxx);

enum aidb_status aidb_xxx_create(struct aidb_context *ctx,
                                 struct aidb_xxx **out_xxx);
void aidb_xxx_destroy(struct aidb_xxx *xxx);

enum aidb_status aidb_xxx_find(struct aidb_xxx *xxx,
                               const char *name,
                               const struct aidb_item **out_item);
```

返回语义：

- `aidb_xxx_init`：调用者提供对象内存，函数只初始化内部状态。
- `aidb_xxx_create`：函数分配对象，成功后 `*out_xxx` 是 `owned`。
- `aidb_xxx_find`：如果返回内部对象指针，必须声明 `*out_item` 是 `borrowed` 或 `view`。
- 失败时应尽量把 `out` 参数置为 `NULL` 或保持调用前约定状态，具体规则在实现文档中进一步细化。

## 四、context 模块设计

`context` 模块负责记录运行上下文，并作为后续跨模块基础设施的承载点。它至少需要支持最近错误信息，未来可以继续承载 allocator、logger、trace、config、AI runtime 配置等。

第一阶段 `context` 只做最小设计：

- `create`
- `destroy`
- `set_error`
- `get_error`
- `clear_error`

接口草案示例：

```c
enum aidb_status aidb_context_create(struct aidb_context **out_ctx);
void aidb_context_destroy(struct aidb_context *ctx);

void aidb_context_set_error(struct aidb_context *ctx,
                            enum aidb_status status,
                            const char *message);

enum aidb_status aidb_context_get_error(const struct aidb_context *ctx,
                                        const char **out_message);

void aidb_context_clear_error(struct aidb_context *ctx);
```

设计约束：

- `aidb_context_create` 成功后返回 `owned` context。
- `aidb_context_destroy` 负责释放 context 及其内部资源。
- `set_error` 记录最近一次错误状态和可读信息。
- `get_error` 返回最近错误码，并通过 `out_message` 返回错误文本视图。
- `clear_error` 把 context 恢复为无错误状态。
- 第一阶段不把 logger、trace、config、AI runtime 配置塞进实现，只保留后续扩展方向。

## 五、所有权规则

Core Foundation 必须延续项目统一所有权语义。

`create/destroy`：

- `create` 创建堆对象。
- 成功后调用者拥有对象，即 `owned`。
- 调用者最终必须调用对应 `destroy`。

`init/deinit`：

- `init` 初始化调用者提供的对象内存。
- `deinit` 释放对象内部资源，但不释放对象本身内存。

`owned`：

- 调用者拥有资源。
- 调用者负责释放。

`borrowed`：

- 借用资源。
- 调用者不能释放。
- 生命周期不能超过被借用对象。

`view`：

- 只读视图。
- 不拥有内存。
- 通常用于字符串、切片或内部状态快照。

`out` 参数：

- 用于返回对象、借用指针或视图。
- 函数应检查必要的 `out` 参数是否为 `NULL`。
- 创建类函数失败时不能让调用者误以为拿到了有效对象。

`NULL` 参数处理：

- 对必要输入参数传入 `NULL`，应返回 `AIDB_ERROR_INVALID_ARGUMENT`。
- `destroy/deinit` 是否接受 `NULL` 由具体实现约定，但应优先让清理路径简单可靠。
- `clear_error(NULL)`、`set_error(NULL, ...)` 等行为需要在实现前明确，不依赖未定义行为。

失败路径：

- 失败路径不能泄漏已分配资源。
- 对象失败后应处于可销毁状态，或明确说明调用者不需要销毁。
- 多资源函数应有清晰清理顺序，避免半初始化状态逃逸。

## 六、测试计划

后续规划测试文件：

- `tests/core/error_test.c`
- `tests/core/result_test.c`
- `tests/core/context_test.c`

测试应覆盖：

- 错误码转字符串。
- `NULL` 参数。
- context 创建和销毁。
- 设置错误和清空错误。
- 重复 `clear`。
- 基础成功路径和失败路径。

测试原则：

- 默认测试不联网、不调用真实 API、不依赖 API Key。
- 测试文件命名使用 `xxx_test.c`。
- 测试函数命名使用 `test_xxx`。
- 新模块实现时同步加入 basic test，不通过删除测试来维持绿色构建。

## 七、和后续模块的关系

`memory/arena` 会依赖 `error/result/context`，用于统一分配失败、上下文错误记录和返回状态。

`vector/string_utils` 会依赖 `memory` 和 `error`，并遵循 `owned/borrowed/view` 语义。

`json/net/ai/dbms` 都会依赖统一错误处理。解析失败、IO 失败、网络失败、AI 调用失败、DBMS 执行失败都应通过清晰错误码和 context 信息向上传递。

现在先不实现这些模块。`v0.1.2` 的价值是把最小核心契约定清楚，让后续实现不在错误处理和所有权规则上反复返工。

## 八、验证

本文档变更完成后仍需运行：

```powershell
cmake --preset mingw-debug
cmake --build --preset mingw-debug-build
ctest --test-dir build/mingw-debug --output-on-failure
```

验证通过后提交：

```powershell
git add .
git commit -m "docs: add core foundation design and versioning policy"
```

本次不 push，push 由用户确认后执行。
