# AI-NATIVE-DBMS-C

代码短名：`aidb`

AI-NATIVE-DBMS-C 是一个面向 AI 原生语义查询处理的 C 语言数据库原型系统。项目目标不是给 DBMS 套一个 ChatGPT 外壳，而是探索将 LLM 调用抽象为 DBMS 查询执行链中的一等语义算子。

当前项目处于早期工程阶段。`v0.1 skeleton` 已完成，当前推进 `v0.1.1` 规范冻结。本仓库先保证工程、目录、命名、测试和协作规则稳定，再逐步实现 core、DBMS、JSON、AI Runtime 和 `AI_MATCH`。

## 项目不是

- 不是工业级数据库。
- 不是 C 语言版 LangChain。
- 不是简单 ChatGPT 外壳。
- 不是完整分布式数据库。
- 不是完整向量数据库。
- 不是一开始就实现完整优化器。

## 当前已具备

- 最小 CMake 工程。
- `aidb` 可执行文件。
- `aidb_core` 基础库目标。
- `basic_core_test`。
- 基础目录骨架。
- 中文文档与规范体系。

## 构建与测试

```powershell
cmake --preset mingw-debug
cmake --build --preset mingw-debug-build
ctest --test-dir build/mingw-debug --output-on-failure
```

运行当前最小可执行文件：

```powershell
.\bin\debug\aidb.exe
```

## 后续路线

- `v0.1.1`：规范冻结。
- `v0.1.2`：`core/error/result/context`。
- `v0.1.3`：`memory + arena`。
- `v0.1.4`：`vector + string_utils`。
- `v0.2`：DBMS Core baseline。
- `v0.3`：Explain Plan/Scan。
- `v0.4`：Handmade JSON + MockModel。
- `v0.5`：`AI_MATCH + AiCache + AiCallLog`。
- `v1.0`：AI Operator Runtime + AI-aware Optimizer + Benchmark。

更详细的阶段拆分见 [ROADMAP.md](ROADMAP.md)。

## 安全说明

- 不要提交 `.env`。
- 不要提交 API Key。
- 真实 API 测试以后放入 `tests/manual`，不进入默认 CTest。
- 默认测试不能联网，不能调用真实 API，不能依赖 API Key。
