# AI-NATIVE-DBMS-C Master Specification

## Status and purpose

This document defines the complete long-term scope of AI-NATIVE-DBMS-C.

It is not a single Codex task.

Codex must execute only the phase explicitly selected by the current prompt. Unselected sections are long-term context and constraints, not current deliverables.

---

# 1. Project definition

AI-NATIVE-DBMS-C (`aidb`) is intended to become:

> A cross-platform C17 research DBMS that first establishes a real, testable relational execution chain and then makes model invocation a plannable, explainable, cacheable, measurable and optimizable database capability.

It is not intended to be:

- an industrial replacement for PostgreSQL or MySQL;
- a chat UI wrapped around an external database;
- only an NL2SQL demo;
- a collection of unrelated AI scripts;
- a full operating-system shell replacement;
- a complete reimplementation of llama.cpp in its first release;
- a foundation model trained from scratch in the near term.

The project may eventually contain several cooperating systems:

```text
C17 DBMS kernel
+ AI operator runtime
+ Tool / Skill / Agent runtime
+ AIDB Terminal
+ local and cloud model providers
+ native C LLM inference runtime
+ database-aware model research
+ MCP and GUI clients
+ model laboratory
+ reproducible benchmark suite
```

These systems must remain separated by explicit interfaces and staged dependencies.

---

# 2. Governing principles

## 2.1 DBMS first

The project must establish a real execution chain:

```text
SQL
  -> tokenizer / lexer
  -> parser / AST
  -> expression / predicate
  -> catalog / metadata
  -> planner / optimizer
  -> executor / scan
  -> record / schema / layout
  -> buffer manager
  -> page / file manager
  -> transaction / WAL / recovery
  -> concurrency
```

AI features must not bypass this chain.

## 2.2 AI thinks; DBMS verifies and executes

Models may:

- interpret intent;
- classify or match semantic content;
- propose SQL;
- repair SQL candidates;
- explain structured DBMS facts;
- select tools;
- propose plans or optimizations.

The DBMS and Tool Runtime must:

- validate syntax and types;
- enforce permissions;
- execute operations;
- produce structured errors;
- record trace and metrics;
- require confirmation for dangerous side effects.

## 2.3 Mock-first and offline tests

Default CTest must:

- not require network access;
- not require API keys;
- not incur model cost;
- use deterministic mock providers;
- be reproducible across supported platforms.

## 2.4 Measurements before optimizer claims

Do not claim a mature AI-aware optimizer until the runtime can reliably report:

- request count;
- latency;
- token or equivalent usage;
- cache hit/miss;
- batch behavior;
- errors and timeouts;
- estimated financial cost where applicable;
- result-quality metrics where meaningful.

## 2.5 Migration is evidence-based

The legacy DBMS contains valuable implementations, but each module must be classified as:

- `REUSE_DIRECTLY`;
- `PORT_WITH_REFACTOR`;
- `REFERENCE_ONLY`;
- `REWRITE`;
- `DROP_OR_DEFER`.

Do not copy the whole legacy source tree blindly.

---

# 3. Repository context

## 3.1 Current repository

Expected path:

```text
D:\code\AI-NATIVE-DBMS-C
```

Expected characteristics:

- C17;
- CMake and CTest;
- cross-platform ambitions;
- Core Foundation and Core Containers already underway;
- DBMS and AI directories may still contain placeholders;
- untracked work must not be deleted.

## 3.2 Legacy repository

Expected path:

```text
D:\code\DBMS\NewDBMS\DBMS_C
```

Expected historical capabilities include some or all of:

- file/page;
- buffer pool;
- log and recovery;
- transaction and concurrency;
- record/schema/layout;
- metadata;
- parser;
- plan/scan;
- update execution;
- hash index;
- SQL CLI and demo;
- CMocka tests.

The exact state must be verified from the current local working tree, not assumed from memory.

---

# 4. DBMS development line

The intended dependency order is:

```text
Core Foundation
  -> Core Containers
  -> Storage
  -> Buffer
  -> Transaction / WAL / Recovery
  -> controlled Concurrency
  -> Query System
  -> Catalog / Metadata
  -> one Index path
  -> Explain / Trace / Metrics
```

## 4.1 Core Foundation and Containers

Potential components:

- error;
- context;
- memory;
- arena;
- binary encoding;
- string utilities;
- vector;
- list;
- red-black tree;
- map;
- byte buffer;
- platform abstractions.

## 4.2 Storage

Potential components:

- BlockId;
- Page;
- FileManager;
- RecordId;
- Schema;
- Layout;
- RecordPage;
- TableScan;
- stable or explicitly unstable disk format;
- reopen and corruption/error-path tests.

## 4.3 Buffer

Potential components:

- frame;
- buffer;
- buffer manager;
- pin/unpin;
- dirty/flush;
- replacement policy;
- buffer exhaustion;
- hit/miss/read/write/eviction metrics.

## 4.4 Transaction and recovery

Potential components:

- log records;
- LogManager;
- WAL;
- transaction state;
- commit;
- rollback;
- restart recovery;
- explicit crash tests;
- controlled S/X locking later.

## 4.5 Query system

Initial SQL scope should remain intentionally small:

- `CREATE TABLE`;
- `INSERT`;
- `SELECT ... FROM ... WHERE ...`;
- `UPDATE`;
- `DELETE`;
- basic comparison and `AND`;
- simple product or nested-loop join only when the basic chain is stable.

Avoid prematurely implementing the entire SQL standard.

## 4.6 Catalog, index and Explain

A research-ready DBMS baseline should expose structured facts:

- logical and physical plan nodes;
- predicate and projection;
- estimated and actual rows where available;
- table/index choice;
- buffer metrics;
- elapsed time;
- later, AI operator metrics.

---

# 5. AI operator line

The first AI-native milestone should place model-backed semantics inside planner/executor rather than only outside SQL generation.

Candidate operators include:

- `AI_MATCH` / `AI_FILTER`;
- `AI_CLASSIFY`;
- `AI_EXTRACT`;
- `AI_MAP`;
- `AI_SUMMARIZE`;
- `AI_RANK`;
- `AI_JOIN`;
- `AI_AGGREGATE`;
- later multimodal operators.

The first implementation should normally select one narrow operator, likely:

```text
AI_MATCH(value, instruction) -> BOOLEAN / UNKNOWN / ERROR
```

The design must define:

- parser and AST representation;
- plan node;
- executor boundary;
- input and output types;
- NULL/UNKNOWN/error semantics;
- provider error and timeout propagation;
- cacheability;
- batchability;
- determinism assumptions;
- Explain representation;
- metrics;
- tests combining relational and AI predicates.

---

# 6. AI Runtime line

The AI Runtime must be C-native, mock-first and provider-agnostic.

Conceptual boundary:

```text
AI Operator
  -> AI Runtime
      -> Provider abstraction
          -> deterministic mock
          -> cloud provider
          -> OpenAI-compatible provider
          -> Ollama
          -> llama.cpp server
          -> Raspberry Pi provider
          -> native C inference provider
```

Required or candidate runtime capabilities:

- request/result;
- structured output;
- provider capabilities;
- model identifier;
- explicit configuration;
- timeout;
- cancellation;
- classified errors;
- retry only with explicit idempotency and budget rules;
- metrics and trace;
- exact result cache;
- budget;
- batching;
- model routing;
- fallback and cascade;
- provider health.

Default tests must stay offline.

---

# 7. Cache, batching and AI-aware execution

## 7.1 Cache levels

Distinguish:

1. exact request/result cache;
2. provider-side prompt cache;
3. operator intermediate-result cache;
4. cross-query cache;
5. persistent cache;
6. vector-similarity semantic cache.

Do not label exact-input caching as a mature semantic cache.

Cache keys may need:

- provider;
- model and version;
- operator;
- prompt/template version;
- normalized input;
- generation parameters;
- schema version;
- operator version;
- quantization/dtype where relevant.

## 7.2 Batching

Batching is not a first-runtime prerequisite.

It requires explicit handling of:

- batch formation;
- waiting window;
- maximum batch;
- input/output mapping;
- partial failure;
- timeout and cancellation propagation;
- cache-before-batch ordering;
- provider token limits;
- queues and backpressure.

## 7.3 AI-aware execution and optimizer

Start with safe, explainable rules:

- execute selective deterministic relational predicates before expensive AI predicates when semantics permit;
- push projection before AI requests;
- use relational indexes first;
- deduplicate identical AI inputs;
- account for cache state;
- later account for batching and model routing.

Possible cost dimensions:

```text
relational cost
estimated rows
AI call count
latency
token usage
financial cost
cache hit rate
batch efficiency
quality
error rate
```

Separate exact-semantics rules from approximate execution that requires user quality constraints.

---

# 8. AIDB Terminal and command runtime

The project should eventually provide a minimal but capable cross-platform terminal.

It is not merely a SQL prompt and not a complete Bash/PowerShell clone.

Conceptual relation:

```text
DBMS <-> Tool / Skill / Event Runtime <-> AI
                       ^
                  AIDB Terminal
```

The terminal is the first client of the runtime. Later clients may include GUI, MCP and HTTP/IPC.

## 8.1 Terminal modes

- direct SQL mode;
- deterministic command mode;
- AI `/ask` mode;
- Skill mode;
- Agent trace mode.

## 8.2 File runtime

Candidate commands/tools:

- `pwd`;
- `cd`;
- `ls`;
- `mkdir`;
- `touch`;
- `cat`;
- `write`;
- `append`;
- `cp`;
- `mv`;
- `rm`;
- `stat`;
- `find`;
- history and clear.

All operations require:

- normalized paths;
- workspace root;
- permission checks;
- size limits;
- audit trace;
- symlink/junction awareness.

## 8.3 Process runtime

Avoid uncontrolled `system(command)` as the primary design.

Represent:

- executable;
- argument vector;
- working directory;
- environment;
- stdin;
- stdout;
- stderr;
- exit code;
- timeout;
- cancellation;
- elapsed time.

Potential platform backends:

- Windows `CreateProcessW`;
- POSIX `posix_spawn` or controlled `fork/exec`.

Pipes, redirection and background jobs are later extensions.

## 8.4 Tool Runtime

A tool definition should include:

- name;
- description;
- input schema;
- output schema;
- permissions;
- side effects;
- timeout;
- execution callback.

Results should be structured for terminal, AI, GUI, MCP and tests.

Candidate tools:

- `fs.*`;
- `process.*`;
- `db.list_tables`;
- `db.describe_table`;
- `db.validate_sql`;
- `db.execute_query`;
- `db.execute_update`;
- `db.explain`;
- `db.metrics`;
- `ai.generate`;
- `ai.match`;
- `ai.list_models`;
- `ai.select_model`;
- `benchmark.run`.

AI must request structured tool calls; it must not directly touch storage, files or processes.

## 8.5 Skill Runtime

A Skill is a reusable workflow containing:

- description;
- instructions;
- allowed tools;
- permissions;
- input/output;
- step and budget limits;
- stop conditions;
- error handling;
- trace.

Candidate Skills:

- NL2SQL;
- SQL Repair;
- Plan Explanation;
- Query Diagnosis;
- Recovery Diagnosis;
- Benchmark Analysis;
- Migration Audit;
- Test Runner;
- Model Evaluation.

## 8.6 Agent Runtime

A controlled loop:

```text
User task
  -> model response
  -> structured tool call
  -> validation and permission check
  -> execution
  -> structured result
  -> model continuation or final answer
```

Guardrails:

- maximum steps;
- token/cost/time budget;
- allowed tools;
- read/write distinction;
- confirmation for dangerous effects;
- cancellation;
- loop detection;
- trace and replay.

Avoid meaningless multi-Agent role proliferation.

## 8.7 Event Runtime

Potential events:

- query started/completed/failed/slow;
- transaction started/committed/rolled back;
- recovery started/completed;
- buffer pressure;
- AI operator started/completed.

Events may trigger display, logging, Skills or AI analysis. AI should normally propose rather than automatically perform destructive actions.

---

# 9. Local model providers

Early local-model integration may use:

- Ollama;
- llama.cpp server;
- OpenAI-compatible local HTTP;
- Qwen;
- GLM;
- other suitable GGUF text models.

The DBMS should not hard-code each application deeply. Prefer a stable provider abstraction with explicit capabilities such as:

- structured output;
- tool use;
- embeddings;
- context length;
- streaming;
- model size and quantization;
- hardware requirements;
- health and latency profile.

A local-model validation matrix should compare:

- deterministic mock;
- local CPU model;
- local GPU model;
- cloud model;
- Raspberry Pi remote provider.

---

# 10. Native C inference runtime

A long-term project goal is not merely to call llama.cpp forever, but to learn from llama.cpp, ggml, GGUF and Transformer inference, then implement a smaller C17 inference runtime owned by this project.

This must not begin as a wholesale source copy.

Upstream licenses and attribution must be respected.

A possible module family:

```text
aidb_infer
├── tensor
├── dtype
├── allocator
├── execution graph
├── CPU backend
├── GGUF reader
├── tokenizer
├── model loader
├── Transformer blocks
├── attention / RoPE
├── KV cache
├── sampler
├── quantization
└── generation
```

## 10.1 First implementation boundary

The first prototype should normally support only:

- one text model architecture;
- CPU;
- batch size 1;
- one request;
- fixed or small context;
- greedy decoding;
- FP32/FP16 or one simple supported dtype;
- no training;
- no claim of llama.cpp replacement.

## 10.2 Staged implementation

Possible order:

1. tensor representation, shape, stride, dtype and ownership;
2. elementary operators and numerical tests;
3. GGUF metadata/tensor inspection tool;
4. tokenizer with reference comparisons;
5. one Transformer layer with intermediate-value comparisons;
6. full forward pass and logits;
7. greedy multi-token generation;
8. KV cache;
9. sampling;
10. basic quantization;
11. threading and SIMD;
12. optional accelerator backends.

Reference validation should compare with llama.cpp, official tokenizers or a Python reference.

## 10.3 First model selection

Research should evaluate a small Qwen, GLM, Llama-family or tiny educational model and select one architecture based on:

- implementation simplicity;
- GGUF availability;
- tokenizer complexity;
- hardware requirements;
- license;
- relevance to future database tasks.

---

# 11. Database-aware model structure research

This is a long-term research line beyond simply running an existing model.

Distinguish levels:

1. prompt and constrained output only;
2. LoRA/QLoRA without structural change;
3. adapters;
4. dedicated encoders or output heads;
5. attention/input/internal architecture changes;
6. distillation into a new smaller model;
7. training a foundation model from scratch.

Candidate structural research:

- schema/table/column embeddings;
- SQL AST encoder;
- query-plan encoder;
- operator embeddings;
- catalog-aware attention;
- statistics-aware input;
- direct AST or plan-node decoding;
- typed tool-call tokens;
- constrained predicate/expression output;
- confidence/cost/quality heads;
- model/DBMS cache coordination;
- plan-aware context compression;
- dynamic layer skipping;
- speculative decoding;
- DBMS-guided decoding.

Near-term work should not claim these are already implemented.

Fine-tuning and distillation may happen later, after data and evaluation pipelines exist.

---

# 12. Raspberry Pi route

Evaluate:

- full DBMS on Raspberry Pi;
- DBMS on PC and model on Raspberry Pi;
- Raspberry Pi as remote provider;
- Raspberry Pi as gateway/router/cache;
- native `aidb_infer` on Raspberry Pi.

Compare:

- latency;
- memory;
- power;
- network dependence;
- deployment;
- reliability;
- security;
- research value.

The likely early use is remote-provider or edge-validation, not blocking the main PC development line.

---

# 13. NL2SQL, repair and explanation branch

These are valuable product and demonstration capabilities, but they do not define the core AI operator architecture.

## 13.1 NL2SQL

Should include:

- schema grounding;
- explicit SQL dialect;
- structured output;
- parser validation;
- read-only mode;
- ambiguity handling;
- retry/repair limits;
- independent evaluation.

## 13.2 SQL Repair

Preferred flow:

```text
candidate SQL
  -> real parser/planner error
  -> structured error
  -> model repair
  -> parser validation again
  -> confirmation when required
  -> execution
```

No unlimited retry or automatic dangerous write execution.

## 13.3 Explanation

The DBMS first produces facts:

- plan nodes;
- rows;
- elapsed time;
- buffer metrics;
- index use;
- AI calls;
- cache;
- token/usage;
- cost.

The model translates those facts; it must not invent an execution path.

---

# 14. MCP and GUI

## 14.1 MCP

MCP is an external protocol adapter over the Tool Runtime, not the terminal itself.

Possible clients:

```text
CLI Terminal
GUI
MCP Server
local HTTP/IPC API
        -> shared Tool Runtime
```

MCP design must cover:

- tool/resource/prompt exposure;
- schema mapping;
- stdio or HTTP transport;
- authentication;
- permission boundaries;
- error mapping.

## 14.2 GUI

GUI must not make the DBMS kernel depend on a UI framework.

Preferred layering:

```text
GUI
  -> stable C API / IPC / local service / Tool Runtime
      -> DBMS and AI systems
```

Candidate workspaces:

- database browser and SQL editor;
- results grid;
- logical/physical plan tree;
- buffer and execution metrics;
- AI provider/model/cost/cache trace;
- Agent/Skill steps and confirmation;
- embedded AIDB Terminal;
- benchmark and experiment dashboard.

Technology selection should compare native C UI, GTK/libui/ImGui and local web UI without forcing the kernel away from C17.

---

# 15. Benchmark and research release

A research prototype requires reproducible experiments.

Candidate workloads:

- structured and text tables;
- high/low selectivity relational predicates;
- repeated and unique semantic inputs;
- different cache hit rates;
- different batch sizes;
- different model sizes;
- different operator placements;
- timeout and malformed responses;
- cloud/local/native inference;
- Raspberry Pi.

Metrics may include:

- end-to-end latency;
- throughput;
- model calls;
- token/usage;
- financial cost;
- cache hit rate;
- batch efficiency;
- accuracy/F1;
- structured-output validity;
- SQL parse and execution correctness;
- failure rate;
- memory/CPU/GPU;
- buffer metrics.

Potential research questions:

- How much do relational predicates reduce model calls?
- When does caching become beneficial?
- How does batching trade latency for throughput?
- What is the smallest model adequate for AI_MATCH?
- Does small-model-first with strong-model fallback improve cost/quality?
- How far is the native C inference runtime from llama.cpp?
- How does operator placement affect cost, latency and quality?
- Do these objectives form a Pareto frontier?

A research release should contain:

- fixed code revision;
- build instructions;
- datasets or generators;
- experiment configuration;
- raw metrics;
- analysis scripts;
- report;
- demo;
- explicit limitations.

---

# 16. Possible final module families

Names are provisional and must be validated against actual dependency boundaries:

```text
aidb_core
aidb_storage
aidb_transaction
aidb_query
aidb_catalog
aidb_ai_runtime
aidb_ai_operator
aidb_infer
aidb_tool_runtime
aidb_skill_runtime
aidb_agent_runtime
aidb_terminal
aidb_server
aidb_mcp
aidb_gui
aidb_model_lab
aidb_bench
```

Not all must become separate libraries or repositories.

Likely language policy:

- DBMS kernel, AI runtime, Tool Runtime and native inference core: C17;
- accelerator-specific or vendor integration: evaluate pragmatically;
- model training/evaluation scripts: Python is acceptable;
- GUI: another language may be used if the kernel boundary remains stable;
- documentation and experiment orchestration: choose the simplest reproducible tool.

---

# 17. Three product horizons

## Horizon A: fastest strong demonstration

Target:

- migrated/tested SQL DBMS chain;
- structured Explain;
- mock-first AI Runtime;
- Ollama or llama.cpp provider;
- one AI_MATCH operator;
- exact cache and metrics;
- minimal AIDB Terminal;
- reproducible demo.

## Horizon B: balanced engineering system

Target:

- clean C17 ownership and errors;
- cross-platform CI;
- Tool Runtime;
- Skills and controlled Agent loop;
- MCP;
- GUI;
- more complete local-model management;
- AI-aware execution rules.

## Horizon C: long-term research platform

Target:

- native C inference runtime;
- GGUF/tokenizer/Transformer/KV cache/quantization;
- model-structure research;
- fine-tuning and distillation later;
- adaptive semantic execution;
- rigorous benchmark and research release.

---

# 18. Scope control

The project must avoid simultaneously implementing every long-term branch.

At any time:

- one main implementation milestone should be active;
- planning tasks should output reports, not code;
- implementation batches should be small and independently testable;
- future branches may be documented without being started;
- version numbers must follow delivered capability, not aspirational architecture.

The integrated roadmap phase must identify:

- critical path;
- optional branches;
- deferred research;
- independent subprojects;
- explicit stop conditions.
