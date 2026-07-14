# Legacy DBMS_C 迁移文档

本目录记录从旧项目 `DBMS_C` 提取设计经验、行为用例和风险的过程。Legacy DBMS_C 只作为只读参考实现：它不进入 AI-NATIVE-DBMS-C 的构建，不作为 Git submodule，也不通过复制整套源码的方式成为新项目的一部分。

旧项目中存在某个目录、源文件或测试，只能证明有可供考古的材料，不能证明该能力完整，也不能证明新项目已经实现对应模块。新项目状态以自身的源码、public header、CMake、CTest 和当前 GitHub Actions 为准。

## 迁移方法

每个模块按以下路径推进：

1. 代码考古：只读检查旧实现、测试、调用路径和失败路径。
2. 新接口设计：先定义新项目的职责、边界、所有权和错误契约。
3. 手写核心实现：依据新设计实现，不机械翻译旧 API 或复制旧文件。
4. 重写测试：把旧测试转化为行为用例，并补充边界、故障和所有权测试。
5. 三平台验证：本机 CTest 通过后，由 GitHub Actions 在 Windows/MSVC、Ubuntu/GCC、macOS/AppleClang 上验证。

详细流程和禁止事项见 [迁移规则](migration_rules.md)。

## 当前文档

- [旧项目事实清单](legacy_inventory.md)：按模块记录文件、职责、接口、依赖、所有权、错误方式、测试证据与已知问题。
- [Legacy 到 AIDB 映射](legacy_to_aidb_mapping.md)：说明旧能力在新架构中的目标位置、当前状态、保留思想、必须重设计内容和测试来源。
- [迁移规则](migration_rules.md)：规定以后每个 DBMS 模块统一采用的迁移流程和完成标准。
- 本文件：迁移文档入口及边界说明。

## 未来 decisions 文档

真正开始迁移某个 DBMS 模块后，再为该模块新增 decision 文档。以下仅为 planned 的目录示例，不代表文件已经存在：

```text
docs/migration/decisions/
  block_page_file.md
  buffer_manager.md
  transaction_recovery.md
```

decision 应记录采用、修改和放弃的旧设计，以及新接口、所有权、错误契约、测试证据和三平台验证结果。当前不创建空白 decision 文件。

## 仓库边界

- Legacy DBMS_C 始终只读，不修改、不清理、不暂存。
- 两个仓库之间不通过 `#include` 或构建路径互相引用。
- 新项目不链接旧项目目标，不依赖旧项目构建产物。
- 默认不使用 Git submodule；若未来需要保留历史快照，必须另行评审目的、许可和维护成本。
- 旧项目的 SQL CLI、DBMS 模块和测试不得被描述为新项目当前能力。
