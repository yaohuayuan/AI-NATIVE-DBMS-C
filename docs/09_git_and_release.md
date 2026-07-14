# Git 与发布

本文维护提交、推送和发布纪律。版本号语义与推进条件统一见 [版本号规范](14_versioning.md)，当前阶段和模块状态见 [ROADMAP](../ROADMAP.md) 与 [状态矩阵](README.md)。

## 1. Commit scope

- 一个 commit 对应一个可说明的逻辑变化。
- 文档、实现、测试和构建变更可以在同一功能 commit 中出现，但必须服务于同一结果。
- 不把无关格式化、临时调试、生成产物或其他工作区改动混入提交。
- 未完成代码可以使用名称和说明明确的 WIP commit/branch；稳定 `main` 应优先保持可构建、可测试和文档一致。

## 2. 选择性暂存

提交前先检查：

```powershell
git status --short
git diff
```

只暂存本次允许的文件，例如：

```powershell
git add docs/02_architecture.md docs/05_directory_layout.md
```

不要使用 `git add .` 盲目纳入整个工作区，尤其是存在未跟踪实验文件、构建产物或用户其他改动时。

暂存后必须检查：

```powershell
git diff --cached --check
git diff --cached
```

确认 staged diff 只包含预期逻辑变化，再创建 commit。

## 3. Commit 前验证

根据变更风险运行适当验证：

- 文档变更至少运行 `git diff --check` 并检查链接、命令和状态事实。
- C/CMake 变更运行 configure、build 和 CTest。
- public API 变更同步检查调用方、测试和模块文档。
- 版本或发布声明必须和源码、CMake、CTest、CI 证据一致。

禁止删除测试、弱化警告或掩盖失败来制造绿色状态。

## 4. Push 与 CI

- push 是显式外部操作，不由 Codex/AI 自动执行。
- push 后必须检查 GitHub Actions，而不是把本地通过等同于远端三平台通过。
- Windows/MSVC、Ubuntu/GCC、macOS/AppleClang job 的失败需要查看具体日志并修复根因。
- CI 未完成或失败时，不得把相关模块标记为 cross-platform verified。

## 5. Tag 与 release

tag/release 必须建立在以下基础上：

- 目标 commit 已明确提交且工作区范围可解释；
- clean clone 能按文档完成 configure、build 和 test；
- 对应 GitHub Actions 成功；
- README、ROADMAP、状态矩阵和模块文档一致；
- 不包含 build/bin、运行数据、`.env`、API Key 或其他敏感信息；
- 发布说明准确区分 implemented、experimental、designed 和 planned。

当前项目没有正式 research prototype release。现有基础模块、CI 和文档进展不能被描述为完整 DBMS、AI-native runtime 或论文级研究发布。

## 6. Release artifacts

安装、打包、版本化数据格式、benchmark 数据、实验报告和可复现研究制品仍是 planned。未来 release 任务必须单独定义：

- source/archive 内容；
- 支持平台与编译器；
- clean-clone 验证步骤；
- 默认测试和手动实验边界；
- 已知限制；
- 数据集、模型、预算和指标版本。

在这些规则未实现和验证前，不创建暗示正式稳定性的 release。
