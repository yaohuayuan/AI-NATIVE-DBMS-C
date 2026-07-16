# AI-NATIVE-DBMS-C 完整开发路线

本路线包含此前提出的全部能力。阶段顺序只控制依赖和风险，不表示后期能力可以取消。

---

## Phase 0：Core Containers 完成

### 必须做出

- 修复 `memory.h` header guard；
- 完成 `rbt`；
- 实现 `map`；
- 实现 `byte_buffer`；
- public header 独立/重复 include 测试；
- allocator failure 注入；
- Core microbench；
- Windows/Linux/macOS CI。

### Demo

```text
aidb-core-test
aidb-core-bench
```

### DoD

实现接入 CMake/CTest，ownership/error 清楚，三平台通过，README/ROADMAP 与代码一致。

---

## Phase 1：Storage 与磁盘格式

### 必须做出

- BlockId；
- Page；
- FileManager；
- Storage Format v1；
- fixed-width little-endian 编码；
- page magic/version/type；
- short read/write；
- reopen；
- corruption detection；
- platform file backend；
- mmap 作为后续 backend。

### Demo

创建文件、追加页、写入、关闭、重开并验证。

---

## Phase 2：Record、Schema、Layout、RID 与 TableScan

### 必须做出

- Value types；
- Schema；
- Layout；
- RID；
- RecordPage；
- slot management；
- 定长字段；
- 受限变长字符串；
- insert/read/update/delete；
- TableScan；
- reopen；
- corrupt slot/header tests。

### Demo

插入 100 行，关闭重开，扫描、更新、删除并验证。

---

## Phase 3：Buffer Pool

### 必须做出

- Frame/Buffer；
- BufferManager；
- pin/unpin；
- dirty/flush/flush-all；
- deterministic LRU 或 Clock；
- buffer exhaustion；
- timeout/cancel boundary；
- hit/miss/read/write/eviction metrics；
- shutdown flush。

### Demo

固定三帧访问五页，显示命中、淘汰、脏页写回和 pinned frame 保护。

---

## Phase 4：Log、WAL、Transaction 与 Rollback

### 必须做出

- LSN；
- LogManager；
- versioned LogRecord；
- START/COMMIT/ROLLBACK；
- SET_INT/SET_STRING；
- WAL-before-data；
- Transaction state machine；
- BufferList；
- commit/rollback；
- transaction trace。

### Demo

一个事务提交，一个事务更新后回滚，重开后验证。

---

## Phase 5：Restart Recovery

### 必须做出

- crash harness；
- process kill/crash injection；
- recovery scan；
- redo/undo；
- idempotent recovery；
- 最小 checkpoint；
- truncated/corrupt log handling；
- recovery metrics/event。

### DoD

必须有跨进程 kill/restart test，不能只测同进程 rollback。

---

## Phase 6：Concurrency

### 第一层

- 单进程多线程；
- S/X Lock；
- lock upgrade；
- timeout；
- mutex/condition；
- conflict tests；
- deterministic thread fixtures。

### 第二层

- wait-for graph；
- deadlock detection/victim；
- two-phase locking；
- isolation level；
- fairness/cancellation；
- lock metrics。

### 第三层

- MVCC 分支；
- snapshot isolation；
- optimistic concurrency 实验。

三层都进入总路线，但 MVCC 不阻塞第一研究原型。

---

## Phase 7：Typed Value、SQL NULL 与 Expression

### 必须做出

- int/string/bool/float 等最小类型；
- owned/borrowed Value；
- comparison/arithmetic 最小集；
- NULL 与 SQL three-valued logic；
- Expression/Term/Predicate；
- ordinary predicate；
- AI predicate AST 占位；
- type checking；
- structured evaluation errors。

---

## Phase 8：Tokenizer、Lexer、Parser 与 AST

### 第一版 SQL

- CREATE TABLE；
- INSERT；
- SELECT/WHERE；
- UPDATE；
- DELETE；
- CREATE VIEW；
- CREATE INDEX；
- transaction statements；
- AI_MATCH。

### 后续 SQL

- JOIN；
- ORDER BY/LIMIT；
- GROUP BY/aggregate；
- 其他 AI Operator 语法。

### 工程要求

- source span；
- structured parse errors；
- parser recovery；
- fuzz tests；
- 底层不 `exit()`。

---

## Phase 9：Scan、Plan、Planner 与 Executor

### 必须做出

- Scan interface；
- Table/Select/Project/Product Scan；
- NestedLoopJoin；
- Logical/Physical Plan；
- Binder；
- Basic Planner；
- Executor；
- cancellation；
- operator metrics；
- cleanup；
- result iterator。

---

## Phase 10：Catalog、Metadata、Statistics 与 Index

### Catalog

- tables/columns/types；
- views；
- index metadata；
- schema version；
- reopen；
- transactional DDL；
- system tables。

### Statistics

- row count；
- distinct estimate；
- min/max；
- null fraction；
- sampling；
- histogram 后续。

### Index 第一版

- Hash Index insert/find/delete；
- collision/duplicate/reopen；
- WAL/DML maintenance；
- IndexScan；
- Planner 使用；
- Explain 显示。

### 后续 Index

- B+Tree/range/unique/composite；
- vector index；
- semantic materialized index。

---

## Phase 11：Explain、Trace、Metrics 与 Event 基础

### 必须做出

- logical/physical plan 结构化表示；
- text/JSON Explain；
- estimated/actual rows；
- elapsed time；
- buffer/index/tx/log metrics；
- operator events；
- trace ID/run ID。

这是 AI、GUI、Agent 和 Benchmark 的共同底座。

---

## Phase 12：AI Runtime 基线

### 必须做出

- AI Request/Result；
- Provider/capability；
- model id/revision；
- structured output；
- timeout/cancellation；
- malformed/rate-limit/provider errors；
- retry policy；
- metrics/budget/health；
- sync API 与 async 边界；
- deterministic Mock；
- recorded/replay Provider。

### DoD

默认 CTest 不联网、不读 API Key、不收费，并可注入固定 latency、timeout、error 和 malformed。

---

## Phase 13：第一 AI Operator——AI_MATCH

### 必须做出

- SQL syntax；
- AST；
- logical/physical node；
- executor；
- true/false/unknown；
- NULL；
- strict error/timeout；
- Explain/Metrics；
- Mock tests；
- 与普通关系谓词组合。

### Demo

```sql
SELECT id, text
FROM documents
WHERE category = 'support'
  AND AI_MATCH(text, '用户表达强烈退款意图');
```

---

## Phase 14：完整 AI Operator 家族

每个算子都必须有 Parser、AST、Plan、Executor、Metrics、Cache、Batch 和测试。

推进顺序：

```text
AI_MATCH / AI_FILTER
 -> AI_CLASSIFY
 -> AI_EXTRACT
 -> AI_MAP
 -> AI_RANK
 -> AI_SUMMARIZE
 -> AI_JOIN
 -> AI_AGGREGATE
 -> Multimodal Operators
```

### 最终要做出

- AI_FILTER：布尔语义筛选；
- AI_CLASSIFY：固定标签；
- AI_EXTRACT：结构化字段；
- AI_MAP：语义值变换；
- AI_SUMMARIZE：组/文档摘要；
- AI_RANK：语义评分排序；
- AI_JOIN：语义关系连接；
- AI_AGGREGATE：语义聚合；
- image/audio/document/multimodal filter/join。

---

## Phase 15：Cache 全路线

### 第一层

- process-local exact cache；
- versioned key；
- bounded LRU；
- metrics；
- unknown/error policy；
- invalidation。

### 第二层

- persistent exact cache；
- cross-query cache；
- operator materialization；
- cache catalog；
- schema/model/prompt version invalidation。

### 第三层

- vector semantic cache；
- approximate lookup；
- quality threshold；
- false-reuse evaluation；
- remote/distributed cache research。

---

## Phase 16：Batching 与 Runtime Scheduling

### 必须做出

- request queue；
- batch formation/wait window/max batch；
- token/context limit；
- result mapping；
- partial failure；
- timeout/cancel；
- cache-before-batch；
- backpressure；
- workers；
- rate limit；
- priority；
- metrics。

### 后续

- cross-query batching；
- operator fusion；
- continuous batching；
- GPU utilization scheduling。

---

## Phase 17：Local / Cloud Provider 全家族

### Provider

```text
mock_provider
recorded_provider
openai_compatible_provider
ollama_provider
llama_cpp_provider
cloud_provider
raspberry_pi_provider
native_c_provider
```

### 能力

- structured output/tool calling/embeddings/streaming；
- context/model switching；
- health/fallback；
- load/unload；
- local model registry；
- quantization metadata；
- memory estimate。

### Demo

同一 AI_MATCH 在 Mock、Ollama、llama.cpp、云端、树莓派和 aidb_infer 之间切换。

---

## Phase 18：Model Routing、Fallback 与 Cascade

### 第一层

- rule-based router；
- capability matching；
- small-model-first；
- confidence/timeout fallback；
- cost/quality budget；
- health routing；
- route trace；
- fallback rate。

### 后续

- learned router；
- contextual bandit；
- adaptive routing；
- per-operator model selection。

---

## Phase 19：AI-aware Execution 与 Optimizer

### 安全规则

- ordinary predicate first；
- projection pushdown；
- relational index first；
- duplicate input elimination；
- cache-aware placement；
- materialized result reuse。

### 成本模型

- rows/calls/latency/token/cost；
- cache hit probability；
- batch efficiency；
- quality/error；
- warm model state。

### 计划选择

- fixed/rule-based；
- dynamic programming；
- multi-objective/Pareto；
- runtime feedback；
- adaptive reoptimization。

### 近似与学习型

- sampling/clustering/early exit；
- confidence fallback；
- approximate cache；
- user quality constraints；
- learned selectivity/cost/routing/order；
- Agentic plan rewrite。

所有层级都做，但按数据和 Metrics 成熟度推进。

---

## Phase 20：AIDB Terminal

### 模式

- SQL Mode；
- Command Mode；
- AI `/ask` Mode；
- Skill Mode；
- Agent Trace Mode。

### DBMS 命令

```text
.tables .schema .catalog .index .plan .explain
.metrics .buffer .transaction .history
```

### 终端能力

- REPL/parser/history/completion/help；
- structured output；
- script 最小能力；
- color/TTY abstraction；
- Windows/Linux/macOS。

---

## Phase 21：File Runtime

### 命令

```text
pwd cd ls mkdir touch cat write append cp mv rm stat find history clear
```

### 必须做出

- workspace root；
- canonical path；
- `..` escape protection；
- symlink/junction；
- file size limit；
- atomic write/backup；
- audit/permission；
- streaming read；
- binary/text distinction；
- cancellation。

---

## Phase 22：Process Runtime

### 必须做出

- program/argv/cwd/env allowlist；
- stdin/stdout/stderr；
- exit code；
- timeout/cancel；
- output limit；
- process-tree kill；
- Windows `CreateProcessW`；
- POSIX `posix_spawn`；
- structured result。

### 后续

- pipe/redirect/background jobs；
- process list/signals/job control；
- resource limits；
- sandbox research。

---

## Phase 23：Tool Runtime

### 必须做出

- Registry/version；
- input/output schema；
- validation；
- permissions/side effects；
- timeout/cancel；
- audit/metrics/trace；
- discovery/aliases；
- built-in adapters。

### 工具家族

```text
fs.* process.* db.* ai.* model.* benchmark.* experiment.* system.*
```

---

## Phase 24：Skill Runtime

### 必须做出

- built-in C Skills；
- file-defined Skills；
- manifest/instructions；
- allowed tools/permissions；
- input/output；
- max steps/budget/stop；
- error/trace/fixtures/versioning。

### Skills

- NL2SQL；
- SQL Repair；
- Plan Explain；
- Query/Recovery Diagnosis；
- Benchmark Analysis；
- Migration Audit；
- Test Runner；
- Documentation Review；
- Model Evaluation；
- Dataset/Experiment Runner。

---

## Phase 25：Agent Runtime

### 第一层

- single Agent；
- structured tool call；
- validation/observation；
- step/token/cost/time limits；
- confirmation/cancel；
- loop detection；
- trace/replay。

### 第二层

- plan-and-execute；
- state/memory；
- checkpoint；
- long-running task；
- human approval gates。

### 第三层

- specialized Agents；
- coordinator/reviewer；
- DB Diagnosis/Research/Model Lab Agents。

多 Agent 必须在单 Agent 稳定后实现。

---

## Phase 26：Event Runtime

### 必须做出

- Event schema/bus；
- sync/async subscribers；
- persistent event log；
- query/tx/recovery/buffer/AI/model/Agent events；
- filtering/correlation/trace ID；
- replay；
- event-triggered Skill；
- notification。

---

## Phase 27：Permission、安全与审计

### 权限

```text
READ_ONLY
WORKSPACE_WRITE
PROCESS_EXECUTE
DATABASE_WRITE
NETWORK_ACCESS
MODEL_MANAGE
SYSTEM_ADMIN
```

### 必须做出

- policy engine/file；
- workspace sandbox；
- path/program allowlist；
- confirmation；
- DB read/write distinction；
- secret redaction；
- audit log；
- backup/rollback；
- limits/budget；
- secure defaults；
- threat model。

---

## Phase 28：NL2SQL

### 必须做出

- schema grounding；
- dialect；
- structured SQL；
- Parser/Binder validation；
- ambiguity/clarification；
- read-only mode；
- write confirmation；
- retry；
- benchmark；
- small/large model comparison；
- Skill/Agent integration。

### 后续

- multi-turn；
- direct AST；
- semantic/hybrid plan generation；
- plan-aware NL2SQL。

---

## Phase 29：SQL Repair

### 流程

```text
SQL -> Structured Parse/Bind Error -> Model Repair
    -> Validate Again -> Confirmation -> Execute
```

### 必须做出

- limited retries；
- repair diff；
- DDL/DML safety；
- benchmark/error taxonomy；
- specialized repair model later。

---

## Phase 30：Natural-language Explain 与 Diagnosis

### 必须做出

- DBMS structured facts；
- plan explanation；
- slow-query/buffer/index/recovery diagnosis；
- AI call explanation；
- fact trace IDs；
- no fabricated facts；
- confirmation before changes。

---

## Phase 31：MCP

### 必须做出

- MCP Server；
- stdio/HTTP transport；
- tools/resources/prompts；
- schema/error mapping；
- authentication/permission；
- session/audit/cancel/streaming；
- compatibility tests。

### Resources

schema、catalog、plans、metrics、benchmark、docs、model registry。

---

## Phase 32：GUI

### Database Workspace

schema browser、SQL editor、result grid、transaction、errors、history。

### Explain Workspace

logical/physical plan、operator tree、rows、latency、buffer、index、AI calls、cache、batch、cost。

### AI Workspace

provider/model/health/usage/cost/latency/trace/routing/fallback。

### Agent Workspace

task、steps、tools、confirmation、trace、replay。

### Terminal Workspace

AIDB Terminal、file/process、DB commands、Skills、Agent trace。

### Model Lab Workspace

model/tokenizer/GGUF/quantization/benchmark/memory/deploy/fine-tune records。

### Experiment Workspace

dataset/workload/model/operator/cache/batch/optimizer/run/chart/export/manifest。

推荐 C17 `aidb_server` + 本地 HTTP/IPC + TypeScript Web GUI，并保留 GTK/libui/ImGui/Qt/WebView 技术评估记录。

---

## Phase 33：Raspberry Pi 全路线

### A 整个系统

运行完整 aidb 做跨平台低资源演示。

### B 模型节点

PC DBMS 调用 Pi 模型。

### C 远程 Provider

Pi 作为标准 Provider endpoint。

### D Gateway/Router/Cache

路由、缓存、故障转移。

### E aidb_infer

ARM64/NEON 原生推理。

### 必须做出

部署脚本、service、auth、health、reconnect、latency/power metrics、model matrix、failure injection、edge benchmark。

---

## Phase 34：Native C Inference——Tensor 与数值基础

### 必须做出

- Tensor/shape/stride/dtype；
- ownership/view/contiguous/alignment；
- allocator/graph or sequential executor；
- add/mul/matmul/embedding；
- RMSNorm/LayerNorm/Softmax/SiLU/GELU/RoPE；
- attention/linear/residual；
- Python numeric reference tests。

---

## Phase 35：GGUF 与 Model Inspector

### 必须做出

- header/version/metadata；
- tensor metadata/alignment/dtype/offset；
- mmap/read；
- validation/unknown fields；
- memory estimate；
- `aidb-model-info`；
- conversion/inspection scripts。

---

## Phase 36：Tokenizer

### 必须做出

- vocab/BPE/SentencePiece；
- special tokens；
- encode/decode；
- UTF-8；
- chat template；
- streaming decode；
- official tokenizer comparison；
- fuzz tests。

---

## Phase 37：Transformer Forward

### 必须做出

- embedding；
- Q/K/V；
- GQA/MQA；
- RoPE/causal mask/attention；
- projection/residual/norm/MLP；
- final norm/LM head/logits；
- layer-by-layer comparison；
- MoE 作为后续架构。

---

## Phase 38：Generation 与 KV Cache

### 必须做出

- prefill/decode；
- greedy/temperature/top-k/top-p；
- repetition penalty/seed/stop；
- streaming/context limit；
- KV layout/reset/reuse/overflow；
- memory estimator；
- continuous batching 后续。

---

## Phase 39：Quantization

### 顺序

```text
FP32 -> FP16/BF16 -> Q8 -> Q6/Q5 -> Q4
```

### 必须做出

- dequant kernels；
- quant format support；
- calibration；
- quality/speed/memory comparison；
- GGUF compatibility；
- mixed precision。

---

## Phase 40：Inference 性能与硬件后端

### CPU

thread pool、tiling、cache blocking、fusion、AVX2、AVX512、ARM NEON、NUMA、mmap、prefetch。

### GPU

CUDA、Vulkan、Metal、OpenCL/SYCL 研究、backend interface、device memory、graph execution、offload。

### Runtime

batch、continuous batching、scheduler、model load/unload、multi-model、prefix cache、speculative decoding。

---

## Phase 41：模型支持

### 第一顺序

1. synthetic tiny Transformer；
2. Qwen 小模型非思考路径；
3. GLM 小模型；
4. Llama；
5. MoE；
6. embedding/reranker；
7. multimodal encoder；
8. specialized database model。

每个模型都需要 architecture descriptor、tokenizer、GGUF mapping、reference logits、generation tests、license 和 performance profile。

---

## Phase 42：Model Lab

### 必须做出

- model registry/metadata/license/checksum；
- download/import；
- GGUF inspect/convert/quantize；
- tokenizer/logits compare；
- benchmark/memory estimate；
- provider/Pi deploy；
- task evaluation；
- experiment history；
- CLI 与 GUI Workspace。

---

## Phase 43：Fine-tuning 与训练

### 数据闭环

任务、schema、input/output、validation/execution、human correction、metrics、model/prompt/split/privacy/lineage。

### 方法

- SFT；
- LoRA/QLoRA；
- preference optimization；
- rejection sampling；
- hard-example mining；
- synthetic data；
- teacher-student；
- distillation；
- tool-use/structured-output training。

### 任务

AI_MATCH、classification、extraction、SQL Repair、NL2SQL、Tool Selection、Plan Explain、Router。

---

## Phase 44：数据库专用模型结构

### 输入结构

- Schema Encoder；
- Table/Column Embedding；
- SQL AST Encoder；
- Query Plan Encoder；
- Operator Embedding；
- Catalog-aware attention；
- Statistics-aware input；
- structured tokens。

### 输出结构

- SQL AST/Plan Node decoder；
- Tool Call tokens；
- typed output/Predicate decoder；
- confidence/cost/quality heads。

### Runtime 协同

- DB cache 与 model cache；
- result/semantic cache；
- plan-aware compression；
- early exit/layer skipping；
- speculative/DBMS-guided decoding；
- model cascade/operator-specific model。

### 研究层级

```text
Prompt -> LoRA -> Adapter -> 专用头 -> 专用 Encoder
       -> Attention/内部结构 -> Distillation -> 从零预训练
```

每一级都必须有 baseline、ablation 和独立 test split。

---

## Phase 45：Benchmark 与 Research System

### Dataset

- synthetic support tickets；
- labeled documents；
- SQL workloads；
- semantic join/extraction/multimodal；
- tool-use；
- Pi；
- inference microbench；
- training data registry。

### Runner

- config/run ID/matrix/dry-run；
- JSON/CSV/Parquet/raw output；
- environment/commit/seed/model/provider/hardware；
- cost table/failure/replay。

### Metrics

latency、throughput、TTFR、tokens/s、calls、usage、cost、cache、batch、accuracy、precision、recall、F1、structured validity、SQL correctness、failure、memory、CPU/GPU、power、buffer、plan estimation error。

### Research Release

fixed commit、datasets/generator、models、configs、scripts、raw results、report、demo、limitations、reproducibility guide。

---

## Phase 46：最终全系统集成 Demo

用户在 GUI 或 Terminal 输入：

```text
找出最近一个月客户支持记录中，已经多次投诉、表达强烈退款意图，
且金额超过 1000 元的记录；解释查询计划，并用小模型优先，
低置信度时回退到强模型。
```

系统完成：

1. NL2SQL/semantic plan；
2. Parser/Binder validation；
3. relational filter/index；
4. AI_MATCH/CLASSIFY/EXTRACT；
5. cache/batch；
6. model routing；
7. local/cloud/native/Pi Provider；
8. Explain；
9. Agent/Tool trace；
10. GUI；
11. benchmark record；
12. reproducibility manifest。

---

## Phase 47：工程化与最终发布

### 必须做出

- stable C API；
- versioned storage/log/cache/model formats；
- migration/import tools；
- packaging/installers/service mode；
- docs/tutorials/samples；
- security guide；
- Provider/Tool/Skill SDK；
- MCP docs；
- GUI package；
- model package；
- benchmark/research release；
- long-term maintenance policy。

项目仍然是研究原型，不宣称工业生产级。
