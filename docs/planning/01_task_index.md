# AI-NATIVE-DBMS-C Planning Task Index

## How to use this file

Each phase must run in a separate Codex conversation.

Do not start the next phase until the previous report has been reviewed.

Status values:

- `NOT_STARTED`
- `IN_PROGRESS`
- `NEEDS_REVIEW`
- `APPROVED`
- `BLOCKED`

---

## P0 — Repository and implementation audit

**Status:** `NOT_STARTED`

**Prompt:** `prompts/01_repository_audit.txt`

**Inputs:**

- current repository;
- legacy repository;
- `AGENTS.md`;
- `00_master_spec.md`;
- current documentation and Git state.

**Output:**

- `reports/01_repository_audit.md`

**Purpose:**

Establish verified facts about both repositories, builds, tests, actual call paths, incomplete code, documentation conflicts and risks.

**Prohibited:**

- source modifications;
- migration implementation;
- broad paper survey;
- final architecture design.

---

## P1 — Legacy-to-aidb migration matrix

**Status:** `BLOCKED`

**Blocked by:** P0 approval

**Prompt:** `prompts/02_migration_matrix.txt`

**Inputs:**

- `reports/01_repository_audit.md`;
- relevant new and legacy source files;
- existing `docs/migration` documents.

**Output:**

- `reports/02_migration_matrix.md`

**Purpose:**

Classify every legacy module and establish migration order, compatibility decisions, test transfer and work estimates.

---

## P2 — Related work and research landscape

**Status:** `BLOCKED`

**Blocked by:** P0 approval

**Prompt:** `prompts/03_related_work.txt`

**Inputs:**

- `00_master_spec.md`;
- existing `docs/research`;
- primary papers and official repositories.

**Output:**

- `reports/03_related_work.md`

**Purpose:**

Identify current research ideas that should influence operators, runtime, optimizer and experiments without inflating first-version scope.

**Can run in parallel with:** P1 after P0 is approved.

---

## P3 — Integrated target architecture

**Status:** `BLOCKED`

**Blocked by:** P1 and P2 approval

**Prompt:** `prompts/04_target_architecture.txt`

**Inputs:**

- reports 01–03;
- current architecture and engineering documents.

**Output:**

- `reports/04_target_architecture.md`

**Purpose:**

Define stable dependency boundaries among DBMS, AI Runtime, operators and future clients.

---

## P4 — Terminal, Tool, Skill, Agent and Event architecture

**Status:** `BLOCKED`

**Blocked by:** P3 approval

**Prompt:** `prompts/05_terminal_tool_skill_agent.txt`

**Output:**

- `reports/05_terminal_tool_skill_agent.md`

**Purpose:**

Design the minimal system terminal and the shared execution/runtime layer used by DBMS, AI, GUI and MCP.

---

## P5 — Local providers, native C inference and model-structure route

**Status:** `BLOCKED`

**Blocked by:** P3 approval

**Prompt:** `prompts/06_local_models_native_inference.txt`

**Output:**

- `reports/06_local_models_native_inference.md`

**Purpose:**

Separate local provider integration from the long-term native C inference engine, and define the later database-aware model-structure research line.

**Can run in parallel with:** P4.

---

## P6 — GUI, MCP and product-layer architecture

**Status:** `BLOCKED`

**Blocked by:** P3 and P4 approval

**Prompt:** `prompts/07_gui_mcp_product.txt`

**Output:**

- `reports/07_gui_mcp_product.md`

**Purpose:**

Define client interfaces without allowing GUI or protocol code to invade the DBMS kernel.

---

## P7 — Benchmark, datasets and experiment system

**Status:** `BLOCKED`

**Blocked by:** P2, P3 and P5 approval

**Prompt:** `prompts/08_benchmark_research.txt`

**Output:**

- `reports/08_benchmark_research.md`

**Purpose:**

Define research questions, baselines, datasets, metrics, reproducibility and release criteria.

---

## P8 — Final integrated roadmap

**Status:** `BLOCKED`

**Blocked by:** P1–P7 approval

**Prompt:** `prompts/09_final_roadmap.txt`

**Output:**

- `reports/09_final_roadmap.md`

**Purpose:**

Merge all approved reports into one dependency-driven route with three horizons, version milestones and explicit deferrals.

---

## P9 — Implementation backlog and first batches

**Status:** `BLOCKED`

**Blocked by:** P8 approval

**Prompt:** `prompts/10_implementation_backlog.txt`

**Output:**

- `reports/10_implementation_backlog.md`

**Purpose:**

Convert the roadmap into small implementation batches with Definition of Done, tests and stop points.

---

## Implementation phase — one batch per conversation

Use:

- `prompts/11_execute_one_batch_template.txt`

Never ask Codex to implement the entire roadmap in one conversation.
