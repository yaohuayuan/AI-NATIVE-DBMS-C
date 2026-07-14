# 内存与所有权

## Status

`memory`、`arena` 和 internal platform alignment 已实现；memory/arena 具有正式测试、接入 CMake/CTest，并由当前 main 分支三平台 CI 验证。本文统一 ownership 语言，不替尚未实现的 DBMS/AI 模块捏造具体生命周期。

## 1. 内存入口

除 `src/core/memory.c` 外，项目代码统一使用：

```c
void *aidb_malloc(size_t size);
void *aidb_calloc(size_t count, size_t size);
void *aidb_realloc(void *ptr, size_t size);
void aidb_free(void *ptr);
```

规则：

- `aidb_free(NULL)` 安全。
- pointer-returning allocation API 失败时返回 `NULL`。
- `aidb_realloc` 应通过临时指针接收，避免失败时丢失原指针。
- 业务模块把分配失败映射为 `AIDB_ERROR_OUT_OF_MEMORY`，memory wrapper 不直接打印或写入 context。
- 标准库 `malloc/calloc/realloc/free` 不得散落到业务模块。

## 2. Ownership 术语

- `owned`：接收方拥有资源，并负责使用对应 cleanup API 释放。
- `borrowed`：仅借用，调用方不得释放；生命周期不能超过 owner。
- `view`：不拥有底层内存的只读或受限视图；owner 变化后可能失效。

public API 返回指针或接收 out 参数时，必须注明属于哪种语义。不能用“返回对象”掩盖释放责任。

## 3. 生命周期命名

- stack 或 embedded object 使用 `init/deinit`。
- `init` 初始化调用方提供的对象；`deinit` 释放内部资源，但不释放对象本身。
- heap-owned object 只有在确有必要时使用 `create/destroy`。
- `create` 成功后返回 owned object；`destroy` 释放对象及其内部资源。
- 不为统一外观而强制给所有模块同时提供两套生命周期 API。

未来 DBMS manager、AI provider/runtime 等内部结构可能频繁变化的复杂对象，优先考虑 opaque public type；具体 create/destroy、引用关系和线程语义必须在实现前单独设计。

## 4. 容器元素

当前 vector 和 list 按值复制固定大小元素，但不会深拷贝元素内部指针，也不会自动释放这些内部资源。

因此：

- 保存普通标量或无内部资源的结构体最直接。
- 保存指针时，容器只复制指针值；指针目标的 owner 不自动改变。
- 元素包含 owned pointer 时，调用方必须在 clear/deinit 前按模块契约完成清理。
- 未来如需 copy/destroy callback，应作为明确扩展设计，不能悄悄改变现有容器语义。

当前 in-progress rbt 的 payload ownership 尚未冻结为稳定 public contract；只有正式实现和测试完成后才能写入状态矩阵。

## 5. Arena ownership

`aidb_arena` 拥有它创建的全部 block：

- `aidb_arena_alloc` 返回的单次 allocation 是 arena-owned/borrowed allocation，调用方不得单独 free。
- allocation 的有效期不超过所属 arena 的下一次 reset 或 deinit。
- `aidb_arena_reset` 复用 block，并使此前 allocation 失效。
- `aidb_arena_deinit` 释放全部 block，并清空 arena 状态。
- arena 不调用 allocation 中对象的析构函数；含外部资源的对象仍需调用方先处理。

## 6. Cleanup 与 NULL

- required argument 为 `NULL` 时，status API 返回 `AIDB_ERROR_INVALID_ARGUMENT`。
- pointer-returning lookup/allocation API 遇到非法输入或失败时返回 `NULL`。
- cleanup API 只有在对应头文件/模块文档明确并有测试时才承诺接受 `NULL`。
- 当前 `aidb_free(NULL)`、`aidb_arena_reset(NULL)`、`aidb_arena_deinit(NULL)` 是安全路径。
- cleanup 应尽量支持部分初始化对象，但具体保证必须由模块实现和测试确认。

## 7. Out 参数失败规则

- required out 参数为 `NULL` 时返回 `AIDB_ERROR_INVALID_ARGUMENT`。
- 输出 owned pointer 的函数应在开始时将 `*out` 初始化为 `NULL`，成功后再写入有效对象。
- 输出布尔值、计数或结构体时，失败后的值必须由具体接口定义；不得让调用者误读未初始化内存。
- 函数在写入多个 out 参数前应先完成参数验证，并保证任何失败路径都不泄漏资源。
- 领域 API 可以采用更严格规则，但必须记录并测试，不能假设全项目存在尚未定义的统一对象语义。

## 8. 未来模块边界

DBMS storage、transaction、metadata、query 和 AI runtime 尚未实现。其 buffer ownership、page pin、transaction lifetime、catalog cache、provider request/result 等规则必须在对应模块设计时确定；本文只提供共同词汇和最低约束。
