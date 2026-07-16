# 项目概览

AI-NATIVE-DBMS-C 是一个使用 C17 开发的 AI-native DBMS 研究原型，代码短名为 `aidb`。项目关注的问题不是“怎样在 SQL 外面包一层聊天接口”，而是“怎样把模型调用放进 DBMS 查询执行链，成为可计划、可解释、可缓存、可测试和可度量的语义算子”。

## 项目分层目标

- **DBMS baseline**：storage、record、buffer、transaction、metadata、parser、plan、scan、executor 和基础 index。
- **AI-native runtime**：mock-first 的模型 provider、AI operator runtime、缓存、批处理与调用度量。
- **研究原型**：围绕 AI operator 与普通关系算子的混合执行设计可重复实验，而不是复制完整 PostgreSQL/MySQL。
- **展示能力**：CLI、NL2SQL、SQL repair、plan explanation 等可作为演示入口，但不自动等同于论文创新。

## 当前边界

项目当前处于 Core Containers 阶段。`error`、`context`、`memory`、`arena`、platform alignment、`binary`、`vector`、`list`、`string_utils`、`rbt` 已实现、测试并完成三平台验证；public header hygiene 已在本地通过，当前改动的三平台 CI 尚待验证；`map`、`byte_buffer` 和所有 DBMS/AI 能力尚未实现。

当前 executable 只是 minimal CLI/bootstrap，不是 SQL CLI。详细事实状态以 [文档索引与模块状态矩阵](README.md) 为准，推进顺序以 [ROADMAP](../ROADMAP.md) 为准。

## 迁移原则

旧 DBMS_C 只用于代码考古、模块分析和设计参考。新项目重新设计 public API、ownership、错误处理和测试，不直接复制旧文件、旧命名或旧接口，也不把旧项目实现写成新项目已经完成。
