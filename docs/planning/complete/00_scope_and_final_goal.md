# 全量范围与最终目标

## 1. 最终原则

AI-NATIVE-DBMS-C 不是只做一个最小 AI_MATCH Demo 后停止。

最小 Demo 是第一个重要里程碑，但最终路线包含用户此前提出的全部能力。

所有能力分为三类：

### 核心数据库与 AI 主线

- C17 Core；
- Storage / Record / Buffer；
- Transaction / WAL / Recovery / Concurrency；
- Parser / AST / Planner / Optimizer / Executor；
- Catalog / Statistics / Index / Explain；
- AI Runtime / AI Operator；
- Cache / Batching / Model Routing；
- AI-aware Execution；
- Benchmark。

### 系统交互与智能执行平台

- AIDB Terminal；
- File Runtime；
- Process Runtime；
- Tool Runtime；
- Skill Runtime；
- Agent Runtime；
- Event Runtime；
- Permission / Audit / Security；
- MCP Server；
- GUI；
- NL2SQL；
- SQL Repair；
- Natural-language Explain；
- Model Management；
- Experiment Workspace。

### 长期模型与推理研究

- Ollama / llama.cpp Server / OpenAI-compatible endpoint；
- Qwen / GLM；
- Raspberry Pi；
- Native C inference；
- Tensor / GGUF / Tokenizer / Transformer；
- Attention / RoPE / KV Cache / Sampling；
- Quantization / SIMD / ARM NEON / GPU backend；
- 数据库专用输入结构和输出头；
- LoRA / QLoRA / Adapter / Distillation；
- 数据库感知 attention；
- 模型与 DBMS cache/runtime 协同；
- Model Lab / Research Release。

“长期”表示后做，不表示不做。

## 2. 最终完整产品形态

```text
                               User
                                |
       +------------------------+------------------------+
       |                        |                        |
 AIDB Terminal                 GUI                     MCP
       |                        |                        |
       +------------------------+------------------------+
                                |
                 Tool / Skill / Agent Runtime
                                |
       +------------------------+------------------------+
       |                        |                        |
    DBMS API               File/Process API          AI Runtime
       |                                                 |
 Parser / Planner / Executor                       Provider Layer
       |                                    +------------+-----------+
 Relational + AI Operators                  |            |           |
       |                                  Cloud        Local      Native C
 Catalog / Tx / Buffer / Storage                          |           |
                                                Ollama / llama   aidb_infer
                                                                   |
                                                       DB-aware Model Research
```

## 3. 不得从路线中删除的能力

Terminal、File/Process、Tool、Skill、Agent、Event、MCP、GUI、Native inference、模型结构修改、微调、树莓派、多 AI Operator、AI-aware optimizer、Model Lab 和 Research benchmark 都必须有目标、依赖、版本、交付物、测试、Demo 和 Definition of Done。

## 4. 旧 DBMS 的定位

旧 DBMS 只作为算法、模块、调用链、测试场景和 Demo 参考。它不需要先清理，也不需要成为干净可发布项目。Codex 可读取当前本地旧仓库，但不得修改。
