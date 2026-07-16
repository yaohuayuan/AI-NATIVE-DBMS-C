# 完整能力矩阵

| 能力 | 最小交付 | 最终交付 | 前置依赖 | 完成证据 |
|---|---|---|---|---|
| Core | rbt/map/byte_buffer | allocator、microbench、platform foundation | 当前 Core | CTest+CI |
| Storage | Page/File v1 | mmap、version migration、corruption tooling | Core | reopen/corrupt tests |
| Record | Schema/Layout/TableScan | richer types、varlen、compression research | Storage | round-trip tests |
| Buffer | fixed pool+LRU | policies、prefetch、async I/O research | Storage | pressure tests |
| Transaction | commit/rollback | isolation、savepoint | Buffer/Log | state tests |
| Recovery | crash redo/undo | checkpoint、ARIES-like research | WAL | kill/restart tests |
| Concurrency | S/X timeout | deadlock、MVCC branch | Tx | deterministic threads |
| Parser | minimal SQL | extended SQL+AI syntax | Value | parser/fuzz |
| Planner | basic plans | cost/adaptive/learned | Query/Metrics | plan comparison |
| Catalog | tables/columns | system catalogs/statistics | Storage/Tx | reopen tests |
| Index | hash equality | B+Tree/vector/semantic | Catalog/Tx | planner+DML integration |
| Explain | text/json plan | GUI/LLM explanation | Query/Metrics | fact consistency |
| AI Runtime | mock request/result | multi-provider/routing/async | Core/Net | offline CTest |
| AI_MATCH | boolean semantic filter | confidence/fallback | AI Runtime/Query | E2E SQL |
| AI Operators | classify/extract/map | join/agg/multimodal | AI_MATCH | per-op benchmark |
| Cache | exact process cache | persistent/vector/materialized | AI Runtime | hit/invalidation tests |
| Batching | fixed provider batch | cross-query/continuous | Runtime Metrics | throughput tests |
| Routing | rules/fallback | learned adaptive router | Providers/Quality | routing benchmark |
| AI Optimizer | ordinary-first | multi-objective/adaptive/learned | Metrics | plan-quality tests |
| Terminal | SQL/DB commands | integrated AI/Skill/Agent | Public APIs | E2E CLI |
| File Runtime | safe read/list | full controlled workspace FS | Tool Runtime | sandbox tests |
| Process Runtime | safe spawn | pipe/jobs/resource limits | Platform | timeout/cancel tests |
| Tool Runtime | registry/schema/permission | SDK/plugin/remote tools | DB/AI/File/Process | fake-tool tests |
| Skill Runtime | built-in Skills | file Skills/version/marketplace | Tools | fixture/replay |
| Agent Runtime | single controlled loop | long tasks/multi-agent | Skills/Tools | deterministic model |
| Event Runtime | metrics/trace events | async bus/replay/triggers | Core systems | ordering tests |
| Security | permission/audit | policy engine/threat model | Tools/Agent | adversarial tests |
| NL2SQL | read-only validated | AST/plan generation | Parser/AI | benchmark |
| SQL Repair | parser-error repair | specialized model | Parser/AI | repair benchmark |
| Natural Explain | plan fact translation | diagnosis Agent | Explain/AI | no-hallucination tests |
| MCP | stdio tools/resources | HTTP/auth/streaming | Tool Runtime | protocol tests |
| GUI | DB workspace | full DB/AI/Agent/Model/Experiment UI | Stable API | packaged app |
| Ollama | compatible provider | model management | AI Runtime | integration tests |
| llama.cpp | server provider | performance/reference harness | AI Runtime | integration tests |
| Raspberry Pi | provider endpoint | native infer/gateway/full aidb | Local Provider | edge benchmark |
| Tensor Engine | basic ops | graph/fusion/backends | Core | Python numeric compare |
| GGUF | inspector | full loader/quant support | Tensor | spec fixtures |
| Tokenizer | encode/decode | multiple families/templates | GGUF/model | reference IDs |
| Transformer | one model forward | multiple architectures/MoE | Tensor/GGUF | logits compare |
| KV/Generation | greedy | sampling/stream/batch/speculative | Transformer | token compare |
| Quantization | Q8/Q4 | full mixed precision | Tensor/Model | quality+speed |
| SIMD/GPU | AVX2/NEON | CUDA/Vulkan/Metal | Stable inference | perf tests |
| Model Lab | CLI inspect/benchmark | GUI/train/deploy registry | Inference/Providers | reproducible runs |
| Fine-tuning | LoRA task model | preference/distillation | Data/Benchmark | held-out eval |
| DB-aware Model | specialized head | encoders/attention/decoding | Training/Inference | ablation study |
| Benchmark | AI_MATCH matrix | all operators/inference/edge/model | Metrics | raw reproducible data |
| Research Release | fixed first report | papers/datasets/models/tools | Benchmark | independent reproduce |
