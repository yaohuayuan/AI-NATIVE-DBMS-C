# 目录职责

目录存在只表示工程为某类能力预留了位置，不代表模块已经 implemented。仓库允许少量带 README 的架构占位目录，但必须明确标记为 planned；不得为尚未确定的设计批量创建空 `.c/.h` 文件。

## 当前目录边界

### `include/aidb`

- 项目 public API 的唯一命名空间目录。
- 当前公开 error、context、memory、arena、binary、vector、list、string_utils 等已实现模块的接口。
- 公开头文件不能依赖 `src` 下的 internal header。
- 不稳定的 node、manager、provider 或平台结构不应直接暴露；复杂模块未来优先使用 opaque public type。
- 当前未跟踪 `rbt.h` 仍是 in-progress 工作，不因位于此目录就视为稳定 public API。

### `src/core`

- 通用核心实现，不依赖 DBMS 或 AI 业务。
- 当前实现 error、context、memory、arena、binary、vector、list、string_utils。
- 当前未跟踪 `rbt.c` 未接入 CMake，不属于已完成 core。
- 项目当前不规划独立 generic result 模块；函数统一使用 `enum aidb_status` 和领域 out 参数。

### `src/platform`

- internal 平台与编译器差异封装。
- 当前包含 arena 使用的 alignment abstraction。
- 未来只有出现真实跨平台需求时才增加 path、filesystem、time、thread、socket 等适配，不提前创建空实现。

### `src/cli`

- 当前只有 minimal CLI/bootstrap。
- 负责组合、入口和人类可读输出，不承载 DBMS、AI runtime 或存储逻辑。
- 当前不是 SQL CLI。

### `tests`

- 当前包含 core 基础模块测试，并由根 `CMakeLists.txt` 注册到 CTest。
- 默认测试离线、可重复、无费用，不依赖 API Key。
- 未来真实 API 或特殊环境测试放入明确的 manual 路径，不进入默认 CTest。

## Planned architecture directories

以下目录当前主要是架构占位或设计方向，不代表实现：

- `src/dbms`：planned storage、buffer、record、transaction、metadata、parser、planner、executor、optimizer、explain。
- `src/ai`：designed/planned provider、runtime、operators。
- `src/json`：planned 项目所需 JSON 子集。
- `src/net`：planned 网络适配；真实网络不进入默认测试假设。
- `src/shell`：planned REPL/command dispatch，与当前 minimal CLI 区分。
- `examples`：planned 可运行示例；不得展示尚未实现的功能。
- `benchmark`：planned 性能和研究实验入口。
- `tools`：planned 辅助工具，不混入核心库。
- `data`：本地运行数据位置；不提交运行产物。

占位 README 必须明确写出 planned/design 状态。目录一旦开始承载实现，应同步更新 [状态矩阵](README.md)、ROADMAP、CMake 和测试。

## Supporting directories

- `docs`：项目概览、工程规范、模块设计、状态入口和未来迁移/研究文档。
- `scripts`：可重复的 build/test/run 辅助脚本；脚本不能隐藏失败。
- `.github/workflows`：CI 配置，不作为模块已经通过 CI 的唯一证据；还需要成功运行记录。
- `build`、`bin`：生成产物，不属于源码或文档事实来源，不应提交。

## 创建规则

- 允许少量用于说明目标分层的目录和 README 占位。
- 占位目录必须清楚标记 planned，不得让读者误以为能力可用。
- 不为尚未确定的模块批量创建头文件、实现文件、测试 target 或伪 API。
- 模块开始实现时，public/internal/platform 边界必须先明确。
- 模块状态以源码、CMake、CTest 和 CI 为准，而不是以目录名或文件名为准。
