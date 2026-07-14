# 编码风格

本文约束 AI-NATIVE-DBMS-C 的 C 代码和公开接口。命名细节见 [命名规范](04_naming_convention.md)，ownership 细节见 [内存与所有权](06_memory_and_ownership.md)。

## C 语言标准

- 统一使用 C17；CMake 设置 `CMAKE_C_STANDARD 17`、`CMAKE_C_STANDARD_REQUIRED ON`、`CMAKE_C_EXTENSIONS OFF`。
- 保持代码可读、可测试、可移植。
- 编译器或平台扩展不能成为未封装的核心依赖；平台差异收敛在 `src/platform`。

## 命名和类型

- 文件、函数、变量和 CMake target 使用 `snake_case`。
- public symbol 使用 `aidb_` 前缀；宏使用 `AIDB_UPPER_SNAKE_CASE`。
- 自定义类型不使用 `_t` 后缀。
- 允许非指针 typedef，例如 `typedef struct aidb_vector aidb_vector;`。
- 禁止隐藏指针 typedef，例如 `typedef struct aidb_x *aidb_x;`。
- 函数签名必须显式写出指针，例如 `aidb_vector *vector`。

## 文件和 API 边界

- 公开头文件放在 `include/aidb`，不得依赖 internal header。
- 内部头文件放在 `src` 的对应模块；平台封装放在 `src/platform`。
- 实现文件放在 `src`，测试放在 `tests`。
- 每个头文件必须有 include guard。
- public API 变更必须同步更新测试和对应模块文档。
- 不因路线图存在就提前创建未来模块的空 `.c/.h` 文件。

## 内存和生命周期

- 除 `src/core/memory.c` 外，项目代码统一使用 `aidb_malloc`、`aidb_calloc`、`aidb_realloc`、`aidb_free`。
- stack 或 embedded object 使用 `init/deinit`。
- heap-owned object 只有在确有需要时才使用 `create/destroy`。
- API 文档必须明确 `owned`、`borrowed` 或 `view`。
- 初始化、清理和失败路径应比正常路径更容易读懂。

## 返回值和 NULL

- 一般状态 API 返回 `enum aidb_status`，并用领域相关 out 参数返回结果。
- required argument 为 `NULL` 时，status API 返回 `AIDB_ERROR_INVALID_ARGUMENT`。
- pointer-returning lookup/allocation API 失败时返回 `NULL`。
- cleanup API 在文档明确时接受 `NULL`，以简化失败路径。
- 当前不规划独立的通用 `aidb_result` 基础模块。

## 函数和注释

- 单个函数只承担一个清晰职责，复杂流程拆成可测试的小函数。
- 不在 `main.c` 或 CLI 层堆积 DBMS/AI 业务逻辑。
- 注释解释设计意图、ownership、错误路径和危险边界，不重复代码表面行为。
- 文档以中文为主，代码标识符使用英文。

## 禁止事项

- 不在业务模块散落标准库 `malloc/free`。
- 不在默认测试中联网、调用真实模型或依赖 API Key。
- 不通过删除测试维持绿色构建。
- 不提交 `build/`、`bin/`、运行数据、`.env` 或密钥。
- 不为了统一格式而改变未授权文件或源码逻辑。
