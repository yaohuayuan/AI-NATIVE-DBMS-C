# AI_MATCH 算子

`AI_MATCH` 是项目计划中的第一个 LLM 语义算子。当前阶段只定义方向，不创建 `ai_match.c` 或相关实现文件。

## 设计目标

- 把语义判断表达为 DBMS 查询执行链中的一等算子。
- 让 `AI_MATCH` 可以进入 plan/scan/explain 流程。
- 让模型调用可以被缓存、记录和优化。
- 让 mock-first 测试覆盖默认行为。

## 当前不做

- 不实现 SQL 语法。
- 不实现 planner 接入。
- 不实现 executor 接入。
- 不实现真实 API 调用。
- 不实现谓词重排。

## 后续方向

`v0.5` 计划引入 `AI_MATCH`、`AiCache`、`AiCallLog` 和最小谓词重排。进入该阶段前，DBMS Core、Explain、Handmade JSON、MockModel 和 `AiClient` 接口应先稳定。
