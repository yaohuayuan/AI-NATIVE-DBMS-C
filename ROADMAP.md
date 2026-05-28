# ROADMAP

本路线图描述 AI-NATIVE-DBMS-C 的阶段边界。早期版本优先保证工程规范、可构建、可测试和可解释的演进节奏，不追求一次性完成终局架构。

## v0.1：工程骨架

- `CMakePresets.json`。
- `aidb_core` 基础库目标。
- `aidb` 最小可执行文件。
- `basic_core_test`。
- 基础目录结构。

## v0.1.1：规范冻结

- 文档语言规范。
- 命名规范。
- 目录职责规范。
- 内存和所有权规范。
- 错误处理规范。
- 测试规范。
- Git 和提交规范。
- AI 辅助开发规范。

## v0.1.2：core/error/result/context

- 统一错误码。
- result 风格。
- context 设计。
- `basic_core_test` 扩展。

## v0.1.3：memory + arena

- `aidb_malloc` / `aidb_free`。
- arena。
- 不让 `malloc/free` 散落在业务模块。

## v0.1.4：vector + string_utils

- vector。
- `string_utils`。
- 对应 core 测试。

## v0.2：DBMS Core baseline

- storage。
- page。
- file manager。
- buffer。
- record。
- parser。
- plan/scan。

## v0.3：Explain Plan/Scan

- 输出可读的 `EXPLAIN PLAN`。
- 输出可读的 `EXPLAIN SCAN`。
- 为后续 AI 算子解释打基础。

## v0.4：Handmade JSON + MockModel + AiClient 接口

- 手搓项目需要的 JSON 子集。
- 提供确定性的 `MockModel`。
- 定义 `AiClient` 接口。
- 默认测试仍然不调用真实 API。

## v0.5：AI_MATCH + AiCache + AiCallLog + 最小谓词重排

- 引入 `AI_MATCH` 语义算子。
- 接入计划、执行和 explain 路径。
- 增加 `AiCache`。
- 增加 `AiCallLog`。
- 做最小谓词重排实验。

## v1.0：AI Operator Runtime + AI-aware Optimizer + Benchmark + 技术报告

- 完成可演示的 AI Operator Runtime。
- 引入 AI-aware Optimizer。
- 建立 benchmark。
- 输出技术报告。
