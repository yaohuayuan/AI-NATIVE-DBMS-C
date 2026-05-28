# 项目概览

AI-NATIVE-DBMS-C 是一个面向 AI 原生语义查询处理的 C 语言数据库原型系统，代码短名为 `aidb`。它关注的问题不是“怎样在 SQL 外面包一层聊天接口”，而是“怎样把 LLM 调用放进 DBMS 查询执行链，成为可计划、可解释、可缓存、可测试的语义算子”。

长期方向分为三条线：

- C DBMS Core：storage、catalog、parser、planner、executor、explain、optimizer 等数据库基础能力。
- Handmade AI Runtime：先以确定性 mock 模型为核心，再把真实 API 调用隔离到显式接口之后。
- First-class LLM Semantic Operators：以 `AI_MATCH` 为代表，让语义判断进入 plan/scan/explain/cache/log/optimizer 流程。

当前 `v0.1 skeleton` 只提供可构建、可测试的工程骨架。`v0.1.1` 的重点是冻结项目规范，包括文档语言、命名、目录职责、内存和所有权、错误处理、测试、Git、CMake 和 AI 辅助开发规则。

在规范冻结完成前，不实现 memory、vector、JSON、net、AI Runtime、DBMS Core 或 `AI_MATCH` 的功能代码。
