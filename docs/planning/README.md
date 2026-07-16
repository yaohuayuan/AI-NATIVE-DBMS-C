# AI-NATIVE-DBMS-C Planning Workspace

This directory stores the long-term project specification and the staged Codex workflow.

## Directory layout

```text
docs/planning/
├── README.md
├── 00_master_spec.md
├── 01_task_index.md
├── 02_execution_protocol.md
├── 03_review_checklist.md
├── prompts/
│   ├── 00_initial_check.txt
│   ├── 01_repository_audit.txt
│   ├── 02_migration_matrix.txt
│   ├── 03_related_work.txt
│   ├── 04_target_architecture.txt
│   ├── 05_terminal_tool_skill_agent.txt
│   ├── 06_local_models_native_inference.txt
│   ├── 07_gui_mcp_product.txt
│   ├── 08_benchmark_research.txt
│   ├── 09_final_roadmap.txt
│   ├── 10_implementation_backlog.txt
│   └── 11_execute_one_batch_template.txt
└── reports/
    └── README.md
```

## Core rule

`00_master_spec.md` contains the complete long-term vision, but it must never be executed as one giant task.

Each Codex conversation executes exactly one prompt from `prompts/`.

Each phase writes a durable report under `reports/`. Later phases read the reports instead of relying on an increasingly long conversation context.

## Recommended order

```text
P0 Repository audit
  -> P1 Migration matrix
  -> P2 Related work
  -> P3 Target architecture
  -> P4 Terminal / Tool / Skill / Agent
  -> P5 Local models / native C inference / model structure
  -> P6 GUI / MCP / product layer
  -> P7 Benchmark / research plan
  -> P8 Integrated final roadmap
  -> P9 Implementation backlog
  -> Execute one implementation batch at a time
```

Do not automatically proceed to the next phase. Review every report first.
