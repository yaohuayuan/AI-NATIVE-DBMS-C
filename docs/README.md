# 文档索引与模块状态

本文是项目文档的统一入口，也是唯一维护完整模块状态矩阵的文件。README 只提供能力摘要，ROADMAP 只维护阶段与里程碑，模块文档只记录对应设计和完成情况。

## 状态定义

- `implemented`：已有实际源代码实现。
- `tested`：已有正式测试并注册到默认 CTest。
- `cross-platform verified`：当前 GitHub Actions workflow 已在 Windows/MSVC、Ubuntu/GCC、macOS/AppleClang 上验证。
- `designed`：已有具体设计，但尚无可用实现。
- `in progress`：存在未完成或尚未接入工程的实现。
- `planned`：尚未开始实现或仅有高层方向。
- `superseded`：已由新的工程约定替代。

## 当前模块状态矩阵

### Core foundation and containers

| 模块 | 代码与接口 | 测试/CMake | 当前状态 | 对应文档 |
|---|---|---|---|---|
| error | `error.h`、`error.c` | `error_test` 已注册 | `tested`, `cross-platform verified` | [错误处理](07_error_handling.md)、[Core Foundation](13_core_foundation_design.md) |
| context | `context.h`、`context.c` | `context_test` 已注册 | `tested`, `cross-platform verified` | [Core Foundation](13_core_foundation_design.md) |
| memory | `memory.h`、`memory.c` | `memory_test` 已注册 | `tested`, `cross-platform verified` | [memory + arena](15_memory_arena_design.md) |
| arena | `arena.h`、`arena.c` | `arena_test` 已注册 | `tested`, `cross-platform verified` | [memory + arena](15_memory_arena_design.md) |
| platform alignment | internal `src/platform/align.h` | 由 `arena_test` 覆盖并随 `aidb_core` 构建 | `implemented`, `cross-platform verified` | [memory + arena](15_memory_arena_design.md) |
| binary | `binary.h`、`binary.c` | `binary_test` 已注册 | `tested`, `cross-platform verified` | [Binary Encoding](16_binary_encoding_design.md) |
| vector | `vector.h`、`vector.c` | `vector_test` 已注册 | `tested`, `cross-platform verified` | [Core Containers](17_core_containers_design.md) |
| list | `list.h`、`list.c` | `list_test` 已注册 | `tested`, `cross-platform verified` | [Core Containers](17_core_containers_design.md) |
| string_utils | `string_utils.h`、`string_utils.c` | `string_utils_test` 已注册 | `tested`, `cross-platform verified` | [Core Containers](17_core_containers_design.md) |
| rbt | `rbt.h`、`rbt.c`，已进入 `aidb_core` | `rbt_test`、`rbt_oom_test` 已注册并在本地通过；当前改动三平台 CI 待验证 | `implemented`, `locally tested` | [Core Containers](17_core_containers_design.md) |
| map | 无实现 | 无 | `planned` | [Core Containers](17_core_containers_design.md) |
| byte_buffer | 无实现 | 无 | `planned` | [Binary Encoding](16_binary_encoding_design.md)、[Core Containers](17_core_containers_design.md) |
| generic result module | 不计划建立独立模块 | 不适用 | `superseded` | 使用 `aidb_status` 与领域 out 参数 |

### DBMS, AI, product and research

| 领域 | 模块 | 当前状态 | 说明 |
|---|---|---|---|
| Storage | block_id、page、file_manager、record_id、schema、layout、record_page、table_scan、buffer、buffer_manager | `planned` | 目录或概念文档不代表实现 |
| Transaction | log、WAL、transaction、recovery、concurrency、lock_manager | `planned` | 尚无公共实现或正式测试 |
| Query | tokenizer、lexer、parser、constant、expression、predicate、scan、plan、executor、update planner | `planned` | 尚无端到端 SQL 执行链 |
| Metadata/index | metadata、table/view/index manager、hash index、index scan | `planned` | 尚无实现 |
| AI-native | provider、AI runtime、AI operator、AI match/filter、cache、batching、AI-aware optimizer | `designed` / `planned` | 现有文档只定义研究方向 |
| 展示能力 | NL2SQL、SQL repair、plan explanation | `planned` | 不自动等同于研究创新 |
| CLI | minimal CLI/bootstrap | `implemented` | 不是 SQL CLI |
| Examples/benchmark/research/release | 示例、实验、数据集、报告、发布制品 | `planned` | 尚无可复现实验发布链路 |

## 推荐阅读顺序

1. [项目概览](00_project_overview.md)
2. [构建与运行](01_build_and_run.md)
3. [架构草图](02_architecture.md)
4. [编码风格](03_coding_style.md) 与 [命名规范](04_naming_convention.md)
5. [目录职责](05_directory_layout.md)、[内存与所有权](06_memory_and_ownership.md)、[错误处理](07_error_handling.md)、[测试规范](08_testing.md)
6. [Git 与发布](09_git_and_release.md)、[AI 辅助开发](10_ai_assisted_development.md)
7. 模块设计文档 13、15、16、17
8. AI 方向文档 11、12

## 工程规范入口

- [编码风格](03_coding_style.md)
- [命名规范](04_naming_convention.md)
- [目录职责](05_directory_layout.md)
- [内存与所有权](06_memory_and_ownership.md)
- [错误处理](07_error_handling.md)
- [测试规范](08_testing.md)
- [版本号规范](14_versioning.md)

## 模块设计入口

- [Core Foundation](13_core_foundation_design.md)
- [memory + arena](15_memory_arena_design.md)
- [Binary Encoding](16_binary_encoding_design.md)
- [Core Containers](17_core_containers_design.md)
- [Handmade AI Runtime](11_handmade_ai_runtime.md)
- [AI_MATCH](12_ai_match_operator.md)

## Migration 文档

- [迁移文档入口](migration/README.md)
- [旧项目事实清单](migration/legacy_inventory.md)
- [Legacy 到 AIDB 映射](migration/legacy_to_aidb_mapping.md)
- [迁移规则](migration/migration_rules.md)

模块级 migration decisions 将在真正开始迁移对应 DBMS 模块后按需创建。旧 DBMS_C 始终保持只读参考，旧模块存在不代表新项目已经完成对应能力。

## Research 文档

- [AI-Native DBMS 研究原型定义](research/research_prototype.md)
- [AI-Native DBMS 实验计划](research/experiment_plan.md)

独立的 research questions、metrics 和 datasets 文档尚未创建；当前定义分别保存在实验计划的对应章节中，不提供不存在的链接。
