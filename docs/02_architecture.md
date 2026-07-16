# 架构

本文区分当前已实现架构与未来目标架构。目录、README 或设计文档存在，不代表对应模块已经 implemented；详细状态以 [文档索引与模块状态矩阵](README.md) 为准。

## 1. Current implemented architecture

当前可构建链路为：

```text
src/cli/main.c
  -> aidb_core
       -> error / context
       -> memory / arena / platform alignment
       -> binary
       -> vector / list / string_utils
```

已实现模块具有 public header、source、CMake 接入和正式测试，并由当前 main 分支的 GitHub Actions 在 Windows/MSVC、Ubuntu/GCC、macOS/AppleClang 上验证。

当前 executable 只是 minimal CLI/bootstrap，用于组合基础模块和验证最小启动路径；它不是 SQL CLI。`rbt` 已进入 `aidb_core` 并注册正式 CTest，本地验证通过；当前工作树的三平台 CI 结果仍为 `UNKNOWN`。

## 2. Target DBMS architecture

以下是 **planned target architecture**，尚未实现：

```text
SQL / CLI
  -> tokenizer / lexer / parser
  -> expression / predicate
  -> metadata / catalog
  -> planner
  -> executor / scan
  -> record / schema / layout
  -> buffer manager
  -> page / file manager
  -> log / WAL / transaction / recovery
  -> lock / concurrency
```

目标是形成一条可测试的端到端 SQL 执行链，而不是一次性复制完整 PostgreSQL/MySQL。每层只有在 public contract、实现、CMake、CTest 和要求的平台验证完成后，才能从 planned 更新为 implemented/tested。

## 3. Target AI-native architecture

以下同样是 **designed/planned target architecture**，尚未实现：

```text
planner / executor
  -> AI operator boundary
       -> AI runtime
            -> provider abstraction
                 -> deterministic mock provider
                 -> optional real provider adapter
            -> cache / batching / metrics / timeout / error mapping
```

AI operator 必须建立在稳定的 schema、scan、plan 和 executor 之上。默认测试采用 deterministic mock；真实模型调用只能通过显式 provider 接入，并与默认离线 CTest 隔离。

NL2SQL、SQL repair、自然语言 CLI 和 plan explanation 属于可选展示能力，不是 AI runtime 的核心依赖，也不自动等同于研究创新。

## 4. Dependency direction

依赖只允许从高层指向低层：

```text
cli / shell
  -> ai operators / dbms executor
       -> ai runtime / dbms plan-scan-metadata
            -> storage / transaction
                 -> core
                      -> platform
```

约束：

- `core` 不依赖 DBMS、AI、CLI 或真实网络。
- `platform` 封装平台和编译器差异，不承载业务语义。
- DBMS 不反向依赖 CLI。
- AI runtime 不直接接管 SQL parser、planner 或 storage 的职责。
- AI operator 通过清晰接口进入 plan/executor，不绕过 DBMS 执行链直接操作底层存储。
- 默认测试路径不依赖网络、费用或 API Key。

## 5. Public/internal/platform boundaries

- `include/aidb`：public API。公开头文件不能依赖 private/internal header，也不能暴露不稳定的平台细节。
- `src/core`：通用核心实现。当前承载已实现的 error、context、memory、arena、binary 和 containers。
- `src/platform`：internal 平台/编译器适配。当前 alignment abstraction 位于此处；未来 path、time、thread 等能力也必须先有明确需求。
- `src/dbms`：planned DBMS 实现区域。当前占位 README 只表达目标目录，不代表模块存在。
- `src/ai`：planned AI runtime/provider/operator 实现区域。当前目录不代表 AI 能力已经实现。
- `src/cli`：组合、参数入口和人类可读输出；保持薄层，不堆 DBMS 或 AI 核心逻辑。
- `tests`：从 public behavior 验证模块；需要访问 internal contract 的测试必须显式说明原因。

public API 应优先隐藏不稳定实现细节。未来 DBMS manager、provider 或 runtime 等复杂对象优先考虑 opaque public type，并将内部结构留在 `src`。
