# Architecture

v0.1 only provides the skeleton. The intended architecture is:

```text
User / SQL
  -> Shell
  -> Parser / Planner / Executor
  -> AI Operator Runtime
  -> Handmade AI Client
  -> MockModel / Real API
```

The default development path is mock-first. Real API integration belongs behind explicit interfaces and must not enter default CTest runs.
