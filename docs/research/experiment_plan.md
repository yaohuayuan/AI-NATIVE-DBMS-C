# AI-Native DBMS 实验计划

## Status

**Planned experiment design，尚无实验结果。** 当前 DBMS execution baseline 与 AI runtime 尚未实现，数据集、provider 版本和成本模型也尚未冻结。本文只定义可验证问题、基线、指标与复现要求；所有假设均为 `planned hypothesis`，不得被引用为已证实结论。

## 1. 研究问题与假设

| ID | 研究问题 | Planned hypothesis | 判定所需证据 |
|---|---|---|---|
| RQ1 | AI 算子批处理是否降低总延迟和估算费用？ | H1：在输入规模足够且 provider 存在固定调用开销时，batching 将减少调用次数并提高吞吐；batch 过大可能增加 time to first result。 | 多种 batch size 下的 latency、TTFR、throughput、call count、token/费用与质量 |
| RQ2 | AI 结果 cache 如何影响吞吐和调用次数？ | H2：重复输入比例越高，cache 越能减少调用次数和费用；低重复率或高失效成本时收益下降。 | 可控重复率下的 hit rate、throughput、call count、memory 和结果一致性 |
| RQ3 | 普通谓词先执行能否减少 AI 调用？ | H3：当普通谓词选择性较高且与 AI 谓词可安全重排时，ordinary-filter-first 将减少进入 AI operator 的行数。 | 两种顺序的 operator row counts、AI calls、latency、cost 和结果等价性 |
| RQ4 | 成本感知计划能否优于固定算子顺序？ | H4：在选择性、cache、batch 和 provider latency 变化时，经过校准的 cost-aware plan 将比单一 fixed plan 获得更低实际成本；估算误差会限制收益。 | 固定/成本感知计划的 actual cost、estimated cost、误差、latency、费用和质量 |
| RQ5 | confidence 阈值如何影响质量与成本？ | H5：提高阈值并复核低 confidence 结果可能改善 precision 或降低 false positive，但会增加调用和 latency；不存在对所有数据集都最优的固定阈值。 | threshold sweep 的 precision、recall、F1、FP/FN、calibration、calls 和费用 |
| RQ6 | materialize AI 结果何时有收益？ | H6：当同一 AI 结果被重复查询且失效频率低时，materialization 的复用收益将超过构建和维护成本。 | 重用次数/更新率矩阵下的 build、storage、refresh、query latency 和总成本 |

这些假设描述预期方向，不构成结果。实验必须允许假设不成立，并保留负结果和异常运行。

## 2. 实验变量与控制

### 独立变量

- execution mode：row-by-row、fixed batch、cache、materialization；
- operator order：ordinary filter first、AI filter first、cost-aware order；
- batch size 与 batch waiting window；
- cache capacity、命中率/重复率和失效策略；
- ordinary predicate selectivity；
- confidence threshold 与低置信度复核/降级策略；
- provider mode、latency/error profile 和模型版本；
- dataset size、label distribution 和文本长度。

### 控制变量

- 相同 dataset version、split 和 query workload；
- 相同 prompt/model/provider 配置；
- 相同机器、OS、编译器、build type 和 commit；
- 相同 warm-up、repetition count 和统计方法；
- 对需要结果等价的计划使用相同语义与错误策略。

每次比较只改变声明的独立变量。无法固定的真实 provider 波动必须单独记录，不与 deterministic mock/replay 结果混合汇总。

## 3. 实验基线

### 执行与 cache 基线

1. **Row-by-row, no cache**：每行一次 AI 调用，作为直接执行基线。
2. **Fixed batch, no cache**：使用固定 batch size，不复用结果。
3. **Fixed batch + cache**：固定 batch，batch formation 前查 cache。
4. **Ordinary filter first**：先执行可安全重排的普通谓词，再送入 AI operator。
5. **AI filter first**：先执行 AI predicate，用于比较顺序影响。
6. **Fixed physical plan**：对所有输入使用预先固定的 operator 顺序与 batch/cache 策略。
7. **Cost-aware physical plan**：根据选择性、调用成本、cache 与 batch 估算选择执行策略。

### Provider 基线

- **Deterministic mock provider**：固定输入、输出、latency、usage 和错误注入，是 CI 与正确性实验的默认基线。
- **Recorded/replay provider**：回放经过版本化、脱敏且许可明确的响应，用于离线重现真实响应形态；它不能伪装成实时模型结果。
- **Optional real provider**：仅用于明确标注的外部有效性或演示实验，必须记录模型、provider、参数、时间和费用。

计划对比必须同时报告结果质量。若 ordinary-first 与 AI-first 因错误、unknown 或 confidence 策略产生不同语义，不能只比较性能。

## 4. 指标

### 4.1 性能

- total latency；
- time to first result；
- throughput；
- CPU time；
- memory usage。

### 4.2 AI 调用

- model call count；
- batch count；
- average batch size；
- token usage 或 provider 提供的等价 usage；
- estimated monetary cost；
- retries；
- timeout/error rate；
- cache hit rate。

### 4.3 结果质量

- accuracy；
- precision；
- recall；
- F1；
- false positive / false negative；
- confidence calibration。

质量指标必须说明正类定义、unknown/timeout 的计分方式、类别不平衡和标签来源。confidence calibration 可采用 reliability bins、Brier score 或后续冻结的等价方法，但不得在数据集和输出语义确定前假设某一种指标已经实现。

### 4.4 计划质量

- estimated cost；
- actual cost；
- estimation error；
- rows entering each operator；
- rows filtered before AI operator。

`actual cost` 必须在实验配置中定义是 wall-clock、AI usage/费用、资源加权值还是多目标向量。不同定义不得混用。estimated monetary cost 只根据冻结的 provider 计价快照计算，不声称等于长期真实账单。

## 5. 数据集计划

本批不创建独立 `datasets.md`。候选数据集必须满足：

- 同时包含可供普通 predicate 使用的结构化字段和供 AI operator 判断的文本字段；
- 有人工标注或来源可靠、可审计的标签；
- train/development/test 或 experiment split 可重复生成或直接版本化；
- 数据与标签具有固定版本和校验信息；
- 隐私、许可、再分发和模型使用条件明确。

计划中的数据集层级如下，当前均不存在已冻结交付物：

| 候选数据集 | 用途 | 计划规模/性质 | 约束 |
|---|---|---|---|
| synthetic support-ticket dataset | 控制选择性、重复率、文本长度和标签分布 | 可参数化生成 | 生成器版本、seed 和标签规则固定 |
| fixed labeled document dataset | 质量与成本联合评估 | 中小规模、人工或可靠标签 | 许可、隐私、标签指南和版本明确 |
| small deterministic CI dataset | operator、plan、metrics 回归 | 很小、完全离线 | 仓库可安全分发，结果确定且运行快速 |
| larger local experiment dataset | latency、throughput、cache/materialization 实验 | 大于 CI 数据集 | 不默认进入仓库；记录获取、校验和本地路径配置 |

在数据集冻结前可以用生成的微型 fixture 开发测试，但不能据此报告最终研究质量结论。

## 6. 实验可复现要求

每次可报告的实验运行必须保存：

- fixed random seed；
- dataset name、version、split 和 checksum；
- model/provider name 与 version；
- prompt/template version；
- temperature、top-p 或其他 sampling settings；
- 机器、CPU/内存、OS、编译器和 build type；
- git commit hash 及工作树是否 clean；
- 完整 configuration file 或其内容快照；
- warm-up policy；
- repetition count；
- mean、median 和约定 percentile；
- 每次运行的 raw result、错误与退出状态。

另外需要记录 batch/cache/materialization 配置、成本表版本、timeout/retry 策略和 plan。汇总结果必须能追溯到原始运行，不只保存截图或手工表格。

真实 provider 实验还需记录运行时间窗口、region/endpoint（若适用）、rate limit 事件和不可控服务变化。无法固定的 provider 行为应作为限制报告。

## 7. 实验执行接口规划

本节只规划未来接口，不创建目录或代码。可能的目录结构为：

```text
benchmark/
experiments/
data/
outputs/
```

这些目录均为 `planned`。未来 runner 应支持：

- 通过实验 ID 和配置运行单个实验；
- 运行预定义完整实验矩阵，并允许 dry-run 查看组合；
- 输出版本化 JSON/CSV；
- 保存 logical/physical plan、运行配置和环境信息；
- 自动对齐并对比多个基线；
- 区分成功、超时、provider error、无效配置和部分完成；
- 不覆盖原始结果，生成可追溯 run ID。

概念上的命令能力可以是：

```text
run one <experiment-id> --config <file>
run matrix <matrix-id> --config <file>
compare <baseline-run> <candidate-run> --format csv|json
```

以上不是已存在的 CLI 语法；真正实现时应服从项目 public/internal 边界和目录规范。

## 8. 实验阶段

| 阶段 | 开始条件 | 可建立的实验 | 不可声称的内容 |
|---|---|---|---|
| Core/DBMS component microbench | 对应容器、binary、page/record 或 buffer 模块实现并有正确性测试 | allocation、encoding、scan、buffer policy 等微基准 | 不能外推 AI 查询收益 |
| Executor baseline benchmark | schema/scan/plan/executor 与最小数据集稳定 | operator row counts、普通 filter 顺序、TTFR/throughput | 不能声称 AI-aware execution 已实现 |
| AI runtime microbench | deterministic mock provider 与 metrics 稳定 | per-call overhead、batch、cache、timeout/error | 不能以 mock latency 代表真实模型 |
| AI operator integration | AI_MATCH/AI_FILTER 进入 physical plan | ordinary-first/AI-first、batch/cache、explain | 不能在固定策略下声称成熟 optimizer |
| Cost-aware experiments | 成本模型、数据集和 fixed baselines 冻结 | fixed vs cost-aware、threshold、materialization | 假设未统计验证前不能写成结论 |
| Research release | 实验矩阵、原始结果和复现文档完整 | 可报告实验与可选真实 provider 演示 | 不得描述为工业数据库发布 |

微基准应随对应模块稳定逐步建立，不必等待整个 DBMS 完成；但跨层研究实验必须等待其前置执行链和指标契约稳定。

## 9. 当前限制与待冻结项

当前研究实验尚不能正式开始，原因是：

- DBMS execution baseline 尚未完成；
- AI runtime 与 deterministic mock provider 尚未实现；
- AI_MATCH/AI_FILTER 尚未进入 physical plan；
- datasets 尚未冻结；
- provider、prompt、usage 与成本模型尚未冻结；
- experiment runner、JSON/CSV schema 和统计脚本尚未设计实现。

因此 RQ1-RQ6 及 H1-H6 均无法在当前仓库状态下验证。后续每个阶段只能报告其已满足前置条件的微基准或实验，不能用计划、设计文档或旧 DBMS_C 的能力代替新项目实验结果。
