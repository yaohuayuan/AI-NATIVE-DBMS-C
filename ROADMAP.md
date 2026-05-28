# ROADMAP

## v0.1 Skeleton

- Establish the project skeleton.
- Keep a minimal CLI entry point.
- Keep basic non-network tests green.
- Document build, run, and test commands.

## v0.2 DBMS Core Baseline

- Add a small DBMS core baseline.
- Support initial schema visibility with `SHOW SCHEMA`.
- Support `EXPLAIN PLAN` / `EXPLAIN SCAN` style output.

## v0.3 Handmade AI Runtime Foundations

- Implement handmade JSON support.
- Add a `MockModel` for deterministic tests.
- Define the `AiClient` interface without requiring real API access in default tests.

## v0.4 AI_MATCH Planning Path

- Introduce `AI_MATCH`.
- Connect `AI_MATCH` into plan and scan flows.
- Keep mock-first testing as the default.

## v0.5 Cache, CallLog, and Mini Optimizer

- Add `AiCache`.
- Add `AiCallLog`.
- Add minimal predicate reordering.

## v1.0 AI-Native DBMS Prototype

- Provide an AI Operator Runtime.
- Provide an AI-aware optimizer.
- Add benchmarks.
- Produce a technical report.

## Deferred Implementation Files

The following future modules are intentionally roadmap-only in v0.1:

- `ai_summary.c`
- `ai_extract.c`
- `ai_rank.c`
- `ai_batcher.c`
- `ai_budget.c`
- `ai_retry.c`
- `ai_metrics.c`
- `cache_aware_optimizer.c`
- `budget_aware_optimizer.c`
- `nl2sql_agent.c`
- `sql_repair_agent.c`
- `explain_agent.c`
- `schema_tool.c`
- `run_sql_tool.c`
