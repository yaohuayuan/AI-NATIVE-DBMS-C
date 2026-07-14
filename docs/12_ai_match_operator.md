# AI_MATCH / AI_FILTER 算子

## Status

**Designed，尚未 implemented。** 当前没有 AI_MATCH/AI_FILTER SQL syntax、plan node、scan/executor implementation、AI runtime 或正式测试。本文记录研究方向和依赖顺序，不冻结具体 API。

## 1. 核心定位

AI_MATCH/AI_FILTER 是计划中的核心执行算子：把模型语义判断放入 DBMS plan/executor，使调用可以被解释、缓存、度量和后续优化。

它们与展示能力不同：

- **核心执行算子**：AI_MATCH、AI_FILTER，参与 plan/executor 并对 row/value 产生 predicate 或结构化结果。
- **可选展示能力**：NL2SQL、SQL repair、自然语言 CLI、对话入口。这些能力可以调用 DBMS/AI runtime，但不属于核心 operator 执行机制。

## 2. 必要依赖顺序

实现顺序必须保持：

```text
schema / value representation
  -> scan / predicate
  -> plan / executor
  -> AI runtime + deterministic mock provider
  -> AI_MATCH / AI_FILTER operator
  -> metrics / cache / batching
  -> cost-aware execution or optimization
```

没有稳定 schema/scan/plan/executor 时，AI operator 无法定义输入生命周期、结果类型、错误传播或可解释计划。没有 mock-first AI runtime 时，operator 测试会错误依赖网络、费用和真实模型波动。

## 3. Planned semantics

AI_MATCH/AI_FILTER 未来至少需要明确：

- 输入来自哪个 column/value/expression；
- prompt/template 和模型配置怎样成为计划的一部分；
- 输出是 boolean、score、label 还是结构化结果；
- NULL、unknown、timeout 和 provider error 如何传播；
- row/value 在异步、batch 或 cache 路径中的生命周期；
- 相同输入的确定性、cache key 和重复执行语义；
- explain 输出怎样显示 provider、cache、batch 和成本信息。

这些规则尚未冻结，不能从概念文档推断已经存在 SQL 语法或 public API。

## 4. Planner boundary

planner 负责识别 AI predicate、建立 operator/plan node 并记录必要配置，但不直接执行模型调用。初始阶段优先保持计划可解释，不提前实现复杂谓词重排。

未来 planner 需要知道：

- AI operator 的输入表达式和输出类型；
- 是否可 batch/cache；
- 基础 cost/latency estimate 是否可用；
- operator 与普通 filter/project/join 的依赖关系。

在 metrics 和稳定执行语义出现前，不把估算写成成熟 AI-aware optimizer。

## 5. Executor and runtime boundary

- executor 调度 operator，并提供 row/value 和取消/错误上下文。
- AI operator 把 DBMS 输入转换为 runtime request，再把 runtime result 转换为 DBMS predicate/value。
- runtime 管理 provider、mock、timeout、metrics、cache 和未来 batching，不了解 page/record 内部布局。
- provider adapter 不直接操作 plan、scan 或 catalog。
- explain 从稳定的 plan/operator metadata 与执行指标生成，不读取厂商私有对象。

## 6. Test strategy

第一版测试必须使用 deterministic mock provider，至少覆盖：

- match/non-match；
- NULL/empty input；
- provider error 和 timeout；
- malformed result；
- cache hit/miss（cache 实现后）；
- batch 内部分失败（batching 实现后）；
- explain 中 operator 可见性；
- 普通关系算子与 AI operator 的组合。

默认 CTest 不联网、不依赖 API Key，也不使用真实模型结果作为断言。

## 7. Cost-aware execution

成本感知执行是后续阶段，而不是 AI_MATCH 第一版的一部分。只有 runtime 能稳定提供 latency、usage、cache 和 batch metrics 后，才可以设计：

- 普通 predicate 与 AI predicate 的安全重排；
- batch size 和调用次数估算；
- cache-aware cost；
- budget/timeout 约束；
- 结果质量与调用成本的实验比较。

在此之前，文档只能把 AI-aware optimization 标记为 planned。

## 8. Non-goals for the first implementation

- 不同时实现 NL2SQL、SQL repair 和自然语言 CLI。
- 不直接接入真实 provider 作为默认路径。
- 不绕过 DBMS planner/executor 建立独立脚本调用。
- 不宣称已有 AI-aware optimizer。
- 不把目录或设计文档存在写成 operator 已实现。
