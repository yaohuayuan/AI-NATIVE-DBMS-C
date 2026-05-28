# ROADMAP

本路线图使用 `vX.Y.Z` 版本格式。`X` 是主版本号，`Y` 是阶段版本号，`Z` 是当前阶段内的小里程碑。详细规则见 [docs/14_versioning.md](docs/14_versioning.md)。

## 已完成

- [x] `v0.1.0` 工程骨架初始化
- [x] `v0.1.1` 项目规范冻结

## 当前完成

- [x] `v0.1.2` Core Foundation 设计与版本号规范

`v0.1.2` 是文档和设计阶段，不包含 `error/result/context` 的代码实现。

## 后续

- [ ] `v0.1.3` `error/result/context` 最小实现
- [ ] `v0.1.4` `memory + arena`
- [ ] `v0.1.5` `vector + string_utils`
- [ ] `v0.2.0` DBMS Core baseline
- [ ] `v0.3.0` Explain Plan / Scan
- [ ] `v0.4.0` Handmade JSON + MockModel + AiClient 接口
- [ ] `v0.5.0` `AI_MATCH + AiCache + AiCallLog`
- [ ] `v1.0.0` 论文级原型机

## 阶段说明

`v0.1.x` 聚焦工程骨架、项目规范和 Core Foundation。这个阶段只建立最低限度的工程纪律和基础抽象，不提前实现 DBMS、JSON、net、AI 或优化器。

`v0.2.0` 开始进入 DBMS Core baseline，包括 storage、page、file manager、buffer、record、parser、plan/scan。

`v0.3.0` 聚焦 Explain Plan / Scan，为后续 AI 算子的可解释执行路径打基础。

`v0.4.0` 引入 Handmade JSON、确定性的 MockModel 和 AiClient 接口。默认测试仍然不调用真实 API。

`v0.5.0` 引入 `AI_MATCH`、`AiCache`、`AiCallLog` 和最小谓词重排。

`v1.0.0` 目标是论文级原型机：AI Operator Runtime、AI-aware Optimizer、Benchmark 和技术报告基本完成。
