# Project Overview

AI-NATIVE-DBMS-C explores how a small C database kernel can treat LLM-backed semantic operations as first-class query operators.

The long-term direction has three parts:

- C DBMS Core: storage, catalog, parser, planner, executor, explain, and optimizer foundations.
- Handmade AI Runtime: deterministic mock model support first, then real client integration outside default tests.
- First-class LLM Semantic Operators: query operators such as `AI_MATCH` that can be planned, explained, cached, logged, and optimized.

v0.1 is only the engineering skeleton. It keeps the executable and tests small while reserving the architecture for future work.
