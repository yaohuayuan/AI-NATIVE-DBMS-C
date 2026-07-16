# AI-Native DBMS 研究原型定义

## Status

**Designed / planned，尚未形成可运行的研究原型。** 当前已实现、测试并完成三平台验证的是 error、context、memory、arena、platform alignment、binary、vector、list、string_utils 和 rbt；map、byte_buffer、DBMS 主链、AI runtime、AI operator、实验 runner 和研究发布均尚未完成。

本文定义最终研究原型的目标边界和完成标准，不表示所列能力已经实现。状态词统一解释为：

- `implemented`：已有新项目源码实现；
- `designed`：已有具体设计，但没有可用实现；
- `planned`：目标已列入路线，尚未开始或尚无具体实现；
- `experimental`：只在受控实验路径中验证，尚不构成稳定产品能力；
- `future work`：不属于当前研究原型的必要交付。

## 1. 原型定位

AI-NATIVE-DBMS-C 的最终目标是一个单机、教学与研究性质的 AI-native DBMS 原型，而不是完整工业数据库。它需要做到：

- 具备从存储、记录、事务到查询执行的核心 DBMS 链路；
- 支持足以完成研究查询和基本写入验证的有限 SQL 子集；
- 把 AI 调用作为真实物理算子纳入 logical/physical plan 与 executor；
- 记录、解释并比较 AI 调用的 latency、token、估算费用和结果质量；
- 使用固定配置、数据集版本和命令重复运行实验；
- 支撑论文实验、复试展示和开源工程演示。

原型的价值来自“可执行 DBMS 基线 + 可度量 AI 算子 + 成本感知执行”的闭环，而不是功能数量或 SQL 兼容度。

## 2. 非目标

当前研究原型不追求：

- 分布式数据库或大规模集群；
- 完整 SQL 标准；
- PostgreSQL、MySQL 的协议、语法或行为兼容；
- 工业级高可用、在线扩缩容或灾备体系；
- 全套 B+Tree、LSM 或多种工业索引结构；
- 完整 cost-based optimizer；
- 多租户、账号、角色与权限体系；
- GUI IDE 或数据库管理平台；
- 一次性实现所有 AI 功能、provider 和展示入口；
- 成为生产级 PostgreSQL/MySQL 替代品。

这些方向只有在核心研究问题完成后才可列为 `future work`，不得反向扩大当前实现范围。

## 3. 必需 DBMS 基线

AI operator 必须运行在真实、可测试的 DBMS 执行链上。以下能力是最终原型的最低基线；除已明确标注外，目前均为 `planned`。

| 能力 | 最低交付 | 可简化内容 | 不能缺失的契约 |
|---|---|---|---|
| file / block / page | 单机块文件读写和固定页访问 | 单一数据库目录、固定页大小 | 短读写/越界错误、重开一致性、平台隔离 |
| byte_buffer | 有边界检查的顺序读写 buffer | 只支持当前磁盘格式需要的 primitive | position/limit、端序、失败后状态；当前为 `planned` |
| record / schema / layout / RID | 固定 schema 下的记录编码、定位、插入和扫描 | 先支持少量标量类型和定长/受限变长字段 | 稳定页格式、slot/RID 有效性、owned/borrowed 规则 |
| buffer pool | 固定帧数的 pin/unpin、dirty flush 和一种替换策略 | 只实现一种 deterministic replacement policy | pin count、WAL-before-data、flush 失败与 shutdown |
| minimal catalog | 表、字段和基础索引元数据 | 暂不实现完整统计、视图和权限 catalog | schema 可重开、事务一致性和 not-found 错误 |
| parser | 有限 SQL 子集到 AST/statement | 只覆盖研究演示需要的 DDL/DML/SELECT | 结构化语法错误，不在底层 `exit` |
| typed value / expression / predicate | 类型化值、比较、普通谓词和 AI 谓词表示 | 少量数据类型、受限表达式组合 | 类型错误、NULL/unknown 策略和结果所有权 |
| scan / operator | table scan、filter、project 及必要组合 | join 可从最小实现开始 | iterator 生命周期、错误传播、operator 统计 |
| logical / physical plan | 可解释的计划树和基础 logical-to-physical 映射 | 不要求成熟全局优化器 | 普通与 AI 节点均能进入计划并被 explain |
| executor | 执行 physical plan 并传播 row/value/error | 初期可单线程、同步执行 | 资源清理、取消/timeout 边界和运行指标 |
| insert / update / delete | 各一条可验证的最小写入链路 | SQL 语法和更新表达式可受限 | 写入可被事务、日志和恢复测试覆盖 |
| transaction | begin、commit、rollback | 初期限制并发事务数量和隔离级别 | 明确状态机、失败清理、提交/回滚结果 |
| WAL | 关键更新与事务边界日志 | 受限日志记录集合和 checkpoint 策略 | 日志格式、持久化顺序和损坏输入检测 |
| recovery | 可重复验证的最小 crash recovery | 只覆盖原型支持的更新类型 | 重启测试、幂等性和确定的恢复结果 |
| concurrency / lock | 简化锁管理 | 可先采用单机、少量事务和简化锁模式 | 不能以数据竞争或不受控锁冲突代替简化设计 |
| basic hash index | 一个等值查询索引路径 | 固定/简单 bucket 策略 | 碰撞、重复键、事务/WAL 与重开行为 |
| CLI | 组合 public API 的 SQL/实验入口 | 无 GUI、有限命令 | 不访问内部结构，错误与计划可读 |
| explain | 展示 logical/physical plan 与运行指标 | 文本或结构化格式先实现一种 | AI operator、顺序、batch/cache 和 cost 可见 |
| benchmark / experiment runner | 固定配置运行查询并保存结果 | 初期只支持预定义实验矩阵 | JSON/CSV、配置快照、失败状态和可重复命令 |

可简化的是规模、SQL 覆盖、数据类型、优化搜索空间和并发级别；不能缺失的是执行闭环、事务/恢复可测试性、AI 算子进入物理计划、指标可追溯和实验可复现。

## 4. AI-native 核心主线

研究原型固定围绕两条相互依赖的主线。主线 A 建立可执行、可测试、可度量的 AI 算子；主线 B 使用这些稳定语义和指标研究执行策略。

### 4.1 主线 A：AI Operator Runtime + AI_MATCH / AI_FILTER

AI 调用必须以 physical operator 身份进入 executor，而不是绕过 DBMS 的独立脚本。最低设计包括：

- **输入/输出 schema**：输入来自明确的 column/value/expression；输出定义为 boolean、score、label 或结构化值，并规定 NULL/unknown。
- **provider abstraction**：executor 和 operator 不依赖厂商 SDK、网络或认证细节。
- **deterministic mock provider**：相同输入得到可预测结果，可注入 timeout、error、malformed result 和固定 latency，作为默认 CTest 基线。
- **batching**：定义可合并条件、最大 batch、等待窗口、结果映射和部分失败。
- **cache**：cache key 至少包含 provider/model、prompt 或 structured input、采样参数及相关版本；命中语义与失效规则必须明确。
- **timeout/error**：invalid request、timeout、provider failure、rate limit、malformed result 和 cancellation 映射为稳定领域错误。
- **confidence**：operator 明确如何表示、解释和使用 confidence，不把 provider 私有字段直接泄漏给 planner/executor。
- **metrics**：记录 model call count、batch、token/usage、estimated cost、latency、retry、timeout/error 和 cache hit/miss。
- **混合执行**：AI filter 能与 table scan、普通 filter、project 及后续其他关系算子组合，并在 explain 中可见。

第一条实现路径必须是 mock-first。真实 provider 是可选演示适配器，不得成为默认构建、CTest 或研究正确性断言的前提。

### 4.2 主线 B：AI 调用成本感知执行

当主线 A 的语义和 metrics 稳定后，研究以下执行决策：

- 普通谓词与 AI 谓词的安全执行顺序；
- batch size 对 latency、throughput、token/费用和首条结果时间的影响；
- cache 命中率及重复调用避免；
- 低 confidence 结果的复核、降级或 unknown 策略；
- AI 结果 materialization 的构建成本、复用收益和失效成本；
- fixed physical plan 与 cost-aware physical plan 的对比；
- estimated cost 与 actual cost 的误差及模型校准。

成本感知策略不得改变查询语义。涉及近似判断或低置信度降级时，必须同时报告质量指标和成本指标，不能只以更低 latency 宣称更优。

## 5. 展示能力与研究贡献区分

### 核心研究能力

- AI runtime 与 provider abstraction；
- AI physical operator；
- batching 与 cache；
- cost-aware execution；
- metrics、cost attribution 与 explain。

### 可选展示能力

- NL2SQL；
- SQL repair；
- natural-language CLI；
- plan natural-language explanation；
- 多个真实模型 provider。

展示能力可以提升演示效果，但不会自动成为论文创新。只有提出明确研究问题、可比较基线、可验证假设和可重复证据后，某项能力才可能构成研究贡献。NL2SQL 不作为本项目默认核心创新。

## 6. 端到端演示

目标演示可以采用以下受限查询：

```sql
SELECT id, text
FROM documents
WHERE category = 'support'
  AND AI_MATCH(text, '用户表达强烈退款意图');
```

演示应依次展示：

1. 原始 SQL 与固定数据集版本；
2. logical plan；
3. physical plan；
4. 普通 `category` filter 的位置和输入/输出行数；
5. AI filter 的输入 schema、输出语义和 provider 配置；
6. batch size、cache 策略及是否 materialize；
7. model call count、batch count、token usage、latency 和 estimated monetary cost；
8. estimated cost、actual cost 和估算误差；
9. confidence、低置信度处理方式和 final result。

同一演示至少应能切换 fixed plan 与一种 cost-aware plan，并使用 deterministic mock/replay 路径得到可重复结果。真实 provider 演示必须明确模型版本、配置、费用和不可重复因素。

## 7. 推进顺序

```text
implemented core foundation and containers
  -> complete map / byte_buffer
  -> storage + record baseline
  -> buffer + transaction + WAL + recovery
  -> parser + plan + executor + minimal DML
  -> deterministic AI runtime
  -> AI_MATCH / AI_FILTER physical operator
  -> metrics + explain + batch/cache
  -> cost-aware execution experiments
  -> reproducible research prototype release
```

每个箭头表示前置契约和测试必须稳定，不表示所有能力一次性交付。

## 8. 最终完成标准

研究原型只有同时满足以下条件，才能形成正式 research prototype release：

- clean clone 可以配置、构建和运行默认测试；
- GitHub Actions 在 Windows/MSVC、Ubuntu/GCC、macOS/AppleClang 上通过；
- 有可运行的核心 SQL 读取与最小 insert/update/delete 链路；
- file/page/record/buffer 的写入、读取和重开行为可测试；
- transaction、commit、rollback、WAL 和 recovery 有确定测试；
- AI_MATCH/AI_FILTER 进入 physical plan 和 executor，而非独立脚本；
- deterministic mock provider 进入默认测试；
- 至少一个真实 provider 可作为可选演示，但不影响离线构建和测试；
- 数据集具有固定版本、标签、划分、许可与隐私说明；
- 有固定实验配置和固定实验命令；
- 指标可输出为机器可读 JSON/CSV，并保存 plan/configuration；
- 有架构图、实验说明、基线定义和限制说明；
- 重复运行能保存原始结果，并按预定统计方法汇总；
- release/tag 建立在 clean clone、测试通过、文档一致和实验可复现基础上。

当前上述 DBMS、AI 和实验条件尚未满足，因此本文只能标记为 designed/planned，不能描述为已经发布的研究原型。
