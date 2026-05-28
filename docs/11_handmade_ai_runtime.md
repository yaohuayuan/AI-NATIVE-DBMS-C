# Handmade AI Runtime

Handmade AI Runtime 是后续阶段的 AI 调用抽象层，当前 `v0.1.1` 不实现任何功能代码。

## 目标

- 让模型调用成为 DBMS 执行链中的可控组件。
- 先支持 deterministic mock，再接入真实 API。
- 为 `AI_MATCH`、缓存、调用日志、预算控制和 explain 输出提供统一入口。

## 当前不做

- 不实现真实模型调用。
- 不实现 batching。
- 不实现 retry。
- 不实现 metrics。
- 不实现 budget。
- 不读取 API Key。
- 不让默认测试联网。

## 后续方向

`v0.4` 会引入 Handmade JSON、`MockModel` 和 `AiClient` 接口。真实 API 集成必须被隔离在默认测试之外，优先进入 `tests/manual` 或显式启用的实验路径。
