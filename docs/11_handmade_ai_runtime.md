# Handmade AI Runtime

## Status

**Designed / planned，尚未实现。** 当前没有 AI runtime public header、source、CMake target、CTest 或真实 provider。本文描述未来模块边界，不代表接口已经冻结。

## 1. 目标

Handmade AI Runtime 为 DBMS executor 中的 AI operator 提供可替换、可测试和可度量的模型调用边界：

- 默认使用 deterministic mock provider；
- 隔离真实 provider、网络、认证和模型差异；
- 统一 request/result、错误、timeout、metrics、cache 和 batching 的协作方式；
- 让 executor 看到稳定的 operator 结果，而不是 provider SDK 细节。

NL2SQL、SQL repair 和自然语言 CLI 不是 AI runtime 核心。它们未来可以复用 runtime，但不能决定 runtime 的底层设计。

## 2. Provider abstraction

Provider abstraction 负责把统一 request 转换为具体模型调用，并返回统一 result/error。设计要求：

- executor 和 AI operator 不直接依赖厂商 SDK、HTTP 或认证格式；
- mock provider 与真实 provider 使用相同调用边界；
- provider capabilities、model identifier 和配置显式传入，不依赖隐藏全局状态；
- 真实 provider adapter 与默认离线测试隔离。

具体 C public API、opaque type 和 ownership 在实现任务中再冻结，本阶段不捏造函数签名。

## 3. Request and result

统一 request 至少需要表达：

- provider/model 选择；
- prompt 或结构化输入引用；
- timeout/budget 等执行约束；
- 可选的 deterministic/test metadata。

统一 result 至少需要容纳：

- provider 返回内容或结构化 operator 结果；
- 使用量和延迟等可度量信息；
- 是否来自 cache/batch；
- 可分类错误和 provider diagnostic。

request/result 的 owned、borrowed、arena-backed 或 heap-owned 语义尚未冻结，必须在实现前与 executor 生命周期共同设计。

## 4. Deterministic mock provider

mock provider 是 runtime 的第一条实现路径：

- 相同输入产生可预测输出；
- 不联网、不收费、不读取 API Key；
- 可以注入 timeout、provider error、malformed result 和固定延迟；
- 进入默认 CTest，支持 executor/operator 的稳定回归测试。

真实 provider 不能替代 mock 作为默认测试基线。

## 5. Metrics

runtime 计划记录可用于执行分析和实验的指标：

- 请求数量、成功/失败/timeout 数；
- latency；
- token/usage 或 provider 可提供的等价用量；
- cache hit/miss；
- batch size 与等待时间；
- operator 层可关联的执行标识。

指标收集不能改变 operator 结果语义，也不能在默认测试中依赖外部监控服务。

## 6. Batching

batching 是后续优化能力，不是 runtime 第一版的前置条件。未来设计需明确：

- 哪些 request 可以合并；
- 顺序、结果映射和部分失败；
- 最大 batch、等待窗口和 timeout 传播；
- cache lookup 与 batch formation 的顺序；
- executor cancellation 对等待队列的影响。

在单请求 mock/provider 路径稳定前，不实现 batching。

## 7. Cache

cache 面向确定输入和模型配置的结果复用。未来 key 至少要考虑 provider、model、prompt/structured input、参数和相关 schema/operator version。

cache 必须明确：

- 命中结果与实时结果的语义一致性；
- 失效和版本规则；
- 错误是否缓存；
- metrics 如何区分 hit/miss；
- 内存或持久化 ownership。

当前没有实现 AI result cache。

## 8. Timeout and error

runtime 需要把 invalid request、timeout、provider/network failure、rate limit、malformed response 和 cancellation 映射为稳定的领域错误，同时保留可诊断信息。

- 底层 provider 不直接向用户界面打印。
- executor 不解析 provider 私有错误格式。
- retry 只有在幂等性、预算和 timeout 规则明确后才能加入。
- 默认 mock 应能覆盖每类错误路径。

## 9. Executor boundary

executor 负责关系执行、row/tuple 生命周期和 operator 调度；AI runtime 只负责模型调用及其运行期支持。边界要求：

- executor 通过 AI operator 调用 runtime，不直接调用 provider；
- runtime 不读取 page、record 或 catalog 的内部结构；
- AI operator 负责把 DBMS 值转换为 request，并把 result 转换为 predicate/value；
- explain/metrics 通过明确结果或事件接口获取信息，不读取 runtime 私有结构；
- 成本感知计划依赖稳定 metrics，属于更后阶段。

## 10. 实现顺序

```text
stable schema/scan/plan/executor
  -> provider abstraction + request/result
  -> deterministic mock provider
  -> minimal AI operator integration
  -> metrics/cache
  -> batching/timeout/retry refinements
  -> optional real provider adapters
```

每一步都需要 public/internal 边界、ownership、错误路径、独立测试、CMake/CTest 和要求的平台验证。
