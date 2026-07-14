# Legacy DBMS_C 迁移规则

## 1. 适用范围

本规则适用于以后从 Legacy DBMS_C 提取任何 DBMS 能力的任务。旧仓库始终只读；“迁移”指重新确认需求并在 AI-NATIVE-DBMS-C 中设计、手写、测试，而不是搬运旧文件。

新项目状态的事实来源依次是自身源码和 public header、CMake/CTest、GitHub Actions；旧实现、旧测试和旧文档仅提供考古证据。

## 2. 标准迁移流程

### 1. 旧代码考古

只读检查对应旧源码、header、测试、CMake 注册、调用者和设计文档。记录实际观察与旧文档声明之间的差异，不构建“目录存在即功能完整”的结论。

### 2. 列出依赖和行为

列出直接/间接依赖、初始化与销毁顺序、成功路径、失败路径、持久化格式、并发假设和平台调用。为每个行为标记证据来自实现、测试还是文档。

### 3. 明确采用、修改和放弃项

逐项写明：

- `adopt`：概念与行为可进入新设计；
- `modify`：目标保留，但接口、算法或契约必须改变；
- `reject`：不适合新架构或首个原型；
- `unknown`：证据不足，必须先实验或补测试。

不得把 `unknown` 默认为采用。

### 4. 新项目先设计 public header

在实现前定义最小 public surface：类型是否 opaque、函数职责、参数有效性、返回状态、out 参数、生命周期和线程安全假设。核心 public API 由用户主导确认。

### 5. 明确 public/internal/platform 边界

- `include/aidb/` 只放稳定 public API，不暴露 OS header 或内部容器布局。
- 通用内部实现进入 `src/core/`，DBMS 实现进入后续 `src/dbms/`。
- Windows/POSIX/编译器差异封装在 `src/platform/`。
- CLI 只组合 public API，不跨层访问 manager 内部。

### 6. 明确 owned / borrowed / view

对每个指针、buffer、字符串、iterator 和 out 参数标明所有权：谁创建、谁销毁、有效期多长、是否可为空、是否允许别名。容器按值复制元素，但不深拷贝元素内部指针；需要深拷贝时必须由领域 API 明确提供。

### 7. 统一 `aidb_status + out` 参数

可失败且返回领域结果的函数使用 `aidb_status` 加领域 out 参数。required argument 为 `NULL` 时返回 `AIDB_ERROR_INVALID_ARGUMENT`；pointer-returning lookup/allocation API 可按已定义契约在失败时返回 `NULL`。失败时 out 参数必须进入文档约定的安全初始状态。cleanup API 只在文档明确时接受 `NULL`。

不创建独立 generic `aidb_result` 基础模块。

### 8. 统一 `aidb_malloc` / `aidb_free`

新实现只使用新项目内存接口，包括与设计一致的 `aidb_calloc`、`aidb_realloc`。不得把旧项目的裸 `malloc/free` 习惯带入。分配失败必须可测试，部分构造必须可回滚。

### 9. 核心实现由用户手写

核心数据结构、DBMS 关键路径和 public API 由用户主导并手写。自动生成只可用于明确批准的机械辅助，不能代替算法推导、状态机设计和所有权审查。

### 10. Codex 的职责

Codex 适合承担旧仓库只读分析、接口与实现审查、测试设计/实现、CMake/CTest 接入、CI 日志分析和文档同步。Codex 不根据旧文档自行宣布完成，不自动 commit/push，也不在未授权时生成整个 DBMS 模块。

### 11. 旧 API 不直接复制

不得保留旧 PascalCase 名称、公开内部结构、函数指针 union、旧 `Error`、底层 `exit` 或隐式 ownership 作为兼容层。新 API 使用 C17、snake_case、`aidb_` 前缀，custom type 不使用 `_t` 后缀。

### 12. 旧测试只作为行为用例来源

提取旧测试的输入、操作序列、预期结果和 bug regression，不复制其 fixture、旧 API 绑定或平台假设。新测试必须覆盖正常、边界、invalid argument、OOM/I/O 故障、清理和必要的不变量。

### 13. 本机 CTest

实现与测试接入 CMake 后，从受支持的干净构建目录运行配置、构建和 `ctest --output-on-failure`。测试必须默认注册，不能依赖偶然存在的预构建测试库才出现。

### 14. `git diff --check`

提交前运行 `git diff --check`，同时检查 `git status --short` 和实际 diff，确认没有越界文件、生成物或旧仓库变化。

### 15. 逻辑 commit

一个 commit 对应一个逻辑变化。只暂存允许文件，检查 `git diff --cached` 后再由用户明确执行提交；不得使用 `git add .` 盲目纳入工作树内容。

### 16. GitHub 三平台 CI

push 后检查 GitHub Actions，至少验证 Windows/MSVC、Ubuntu/GCC、macOS/AppleClang。任一平台失败都不能把模块标记为 cross-platform verified。

### 17. 记录模块迁移 decision

模块满足完成标准后，在 `docs/migration/decisions/` 新增对应 decision，记录旧证据、adopt/modify/reject、最终 API/ownership/error 契约、测试列表、已知限制和三平台 CI 证据。decision 与 ROADMAP/docs 状态必须同步。

## 3. 完成标准

一个旧模块只有在新项目中同时满足以下条件，才可标记为完成：

- public API 与 public/internal/platform 边界经过确认；
- 实现不引用旧仓库，也不依赖旧构建产物；
- 所有权、失败原子性和 cleanup 行为已有测试；
- CMake/CTest 在 clean build 中默认包含该模块与测试；
- 本机 CTest 通过；
- GitHub Actions 三平台通过；
- 模块设计文档、migration decision、ROADMAP 与文档状态矩阵一致。

仅有旧实现、复制后的文件、未跟踪代码、单平台编译或未注册测试，都不满足完成标准。

## 4. 明确禁止

- 禁止整仓复制 Legacy DBMS_C。
- 禁止按文件机械搬运或逐行翻译旧实现。
- 禁止保留旧 PascalCase public API。
- 禁止将旧 `malloc/free` 混入新项目。
- 禁止把“旧项目已实现”写成“新项目已完成”。
- 禁止未经设计直接生成整个 DBMS 模块。
- 禁止让旧项目参与新项目 CMake。
- 禁止通过 `#include` 或路径配置让两个仓库相互引用。
- 默认禁止使用 Git submodule；任何例外必须单独评审并明确授权。
- 禁止从库或底层模块调用 `exit`/`abort` 处理可报告错误。
- 禁止自动 commit 或 push。

## 5. 单模块迁移检查表

```text
[ ] 旧源码、header、测试、CMake 和调用路径已只读盘点
[ ] 依赖、行为、磁盘格式、平台调用和失败路径已列出
[ ] adopt / modify / reject / unknown 已确认
[ ] public header 和最小职责已由用户确认
[ ] public / internal / platform 边界已确认
[ ] owned / borrowed / view 和 cleanup 已确认
[ ] aidb_status、out 参数与失败后状态已确认
[ ] 仅使用 aidb 内存接口
[ ] 核心实现由用户主导手写
[ ] 新测试已重写并注册默认 CTest
[ ] 本机 clean build 与 CTest 通过
[ ] git diff --check 和变更范围检查通过
[ ] 逻辑 commit 已由用户明确执行
[ ] GitHub 三平台 CI 通过
[ ] migration decision 与状态文档已同步
```
