# 版本与发布路线

版本号只代表已经交付的能力，不代表工业成熟度。

## Phase I — DBMS 基础

```text
v0.1.x Core Containers
v0.2.x Storage / Record
v0.3.x Buffer
v0.4.x WAL / Transaction / Recovery
v0.5.x Concurrency
v0.6.x Query / Parser / Executor
v0.7.x Catalog / Index / Explain
```

## Phase II — AI-native DBMS

```text
v0.8.x AI Runtime / Mock / Replay
v0.9.x AI_MATCH
v0.10.x AI Operator Family
v0.11.x Cache / Batching
v0.12.x Provider / Routing
v0.13.x AI-aware Optimizer
v0.14.x Benchmark / First Research Release
```

## Phase III — 系统平台

```text
v0.15.x AIDB Terminal
v0.16.x File / Process Runtime
v0.17.x Tool Runtime
v0.18.x Skill Runtime
v0.19.x Agent / Event / Permission
v0.20.x NL2SQL / Repair / Explain
v0.21.x MCP
v0.22.x GUI / Server
v1.0.0 Integrated AI-native DBMS Platform
```

## Phase IV — 原生模型平台

```text
v1.1.x Tensor / Numeric
v1.2.x GGUF / Tokenizer
v1.3.x Transformer Forward
v1.4.x Generation / KV Cache
v1.5.x Quantization
v1.6.x SIMD / ARM / Raspberry Pi
v1.7.x GPU Backends
v1.8.x Model Lab
v2.0.0 Native C Inference Platform Integrated
```

## Phase V — 数据库专用模型研究

```text
v2.1.x Dataset / Training Pipeline
v2.2.x LoRA / QLoRA
v2.3.x Distillation / Specialized Small Models
v2.4.x Schema / AST / Plan Encoders
v2.5.x Specialized Output Heads
v2.6.x DBMS-guided Decoding
v2.7.x Cache / KV / Runtime Co-design
v2.8.x Adaptive Model Architecture
v3.0.0 Database-aware Model Research Platform
```

## 里程碑含义

- `v1.0`：完整 DBMS + AI Operators + Terminal/Tool/Skill/Agent/MCP/GUI 平台。
- `v2.0`：自主 C 推理引擎成为正式 Provider，支持 PC 与树莓派路线。
- `v3.0`：具备数据库专用模型结构、训练、蒸馏和系统—模型协同研究能力。

因此此前提出的所有功能都进入了正式版本路线，而不是只写在“未来可能”中。

## 每个版本必须交付

- 源码；
- CMake/CTest/CI；
- ownership/error/API 文档；
- Demo；
- changelog；
- benchmark 或 microbench；
- limitations；
- reproducibility command；
- 无 build/data/key 误提交；
- tag/push 必须由用户明确确认。
