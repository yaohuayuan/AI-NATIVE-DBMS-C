# P0 Repository and Implementation Audit

审计日期：2026-07-14（Asia/Shanghai）

审计对象：

- 当前仓库：`D:\code\AI-NATIVE-DBMS-C`
- legacy 只读参考仓库：`D:\code\DBMS\NewDBMS\DBMS_C`

证据标签沿用根目录 `AGENTS.md`：`VERIFIED`、`IMPLEMENTED_UNTESTED`、`DOCUMENTED`、`INFERRED`、`PROPOSED`、`UNKNOWN`。

## Executive Summary

- `VERIFIED`：当前仓库是可构建的 C17 Core Foundation/Core Containers 基线。按文档执行 MinGW Debug configure/build/CTest 成功，9/9 个 CTest 通过，minimal CLI 退出码为 0。活动 CLI 链路只调用 `context`/`error`；其余已接入 `aidb_core` 的模块主要通过各自测试进入执行路径。
- `VERIFIED`：当前仓库的 `error`、`context`、`memory`、`arena`、little-endian `binary`、`vector`、`list`、`string_utils` 均有实现、CMake 接入、正式测试和本轮成功执行证据。DBMS、AI、JSON、网络、shell、examples、benchmark 仍只有规划文档或占位 README。
- `VERIFIED`：未跟踪 `rbt.h`/`rbt.c` 没有接入 CMake、没有测试，且独立 C17 语法检查失败于 `src/core/rbt.c:255`，因此不能标为 implemented 或 buildable。
- `VERIFIED`：legacy 当前工作树的默认 C11 构建在隔离副本中成功，25/25 个已注册 CMocka 测试通过；完整 demo 在全新临时数据库中成功执行建表、插入、过滤查询、两表 product+filter、更新、删除、视图、索引元数据和提交。
- `VERIFIED`：legacy 的绿色测试不等于完整语义覆盖。`test/DBMSTest.c` 未注册；HashIndex 测试没有执行 insert/find/delete；restart/crash recovery、多线程竞争与死锁检测、索引计划选择、损坏数据、错误 SQL 和资源泄漏均未得到充分验证。
- `VERIFIED`：legacy 严格警告构建（`-Wall -Wextra -Wpedantic -Wshadow -Wconversion -Werror`）失败，首先暴露在 `Lib/ByteBuffer.c`、`Lib/CString.c`、`Lib/map.c` 的转换/符号警告。
- `VERIFIED`：legacy 工作树极度脏且构建依赖未跟踪源文件。它领先 `origin/main` 16 个提交，同时有 167 个 tracked modifications、2 个 tracked deletions 和 50 个 untracked status entries；`CMakeLists.txt` 引用的 `Lib/CList.c` 与 `error/DBError.c` 都未被 Git 跟踪。因此本轮成功仅适用于当前工作树快照，不能外推到 clean checkout。
- `VERIFIED`：legacy 确实存在真实 DBMS 执行链，不是薄 NL2SQL 包装；但 `CREATE INDEX` 只进入索引目录元数据，活动 `BasicQueryPlanner` 不选择 HashIndex，写路径也不维护 HashIndex，不能宣称索引已进入端到端查询执行。

## Scope and Method

### 范围

本阶段仅执行 P0：读取源码、CMake、测试、文档和 Git 状态；构建并运行两个仓库；还原实际调用路径；记录冲突、缺陷、风险和未知项。没有修改源码、没有迁移代码、没有研究论文调查、没有设计最终架构，也没有生成 P1 migration matrix。

### 方法与主要命令

`VERIFIED`：使用了以下只读或构建/执行命令；当前仓库构建产物写入其文档规定的 ignored `build/`、`bin/`，legacy 构建与运行全部位于临时副本。

```powershell
# 两仓库状态和版本
git status --short --branch --untracked-files=all
git rev-parse HEAD
git log -1 --format='%h %cI %s'
git diff --stat
rg --files

# 工具链
cmake --version
ctest --version
gcc --version
mingw32-make --version
ninja --version
git --version

# 当前仓库文档化工作流
cmake --preset mingw-debug
cmake --build --preset mingw-debug-build
ctest --test-dir build/mingw-debug --output-on-failure
.\bin\debug\aidb.exe

# 未接入 CMake 的 rbt 独立语法检查
gcc -std=c17 -Wall -Wextra -Wpedantic -Wshadow -Wconversion \
  -fsyntax-only -Iinclude -Isrc src/core/rbt.c

# legacy：先复制当前工作树到临时目录，排除 .git、旧 build/bin/IDE 产物
robocopy D:\code\DBMS\NewDBMS\DBMS_C \
  $env:TEMP\aidb-p0-legacy-20260714-165718 /E \
  /XD .git build cmake-build-debug bin .idea .vscode \
  /XF *.zip main.pdf

# legacy 默认构建与测试（只在临时副本）
cmake -S $copy -B $copy\build -G "MinGW Makefiles"
mingw32-make -C $copy\build -j8 SHELL=cmd.exe
ctest --test-dir $copy\build --output-on-failure

# legacy 严格警告构建（只在临时副本）
cmake -S $copy -B $copy\build-strict -G "MinGW Makefiles" \
  -DCMAKE_C_FLAGS="-Wall -Wextra -Wpedantic -Wshadow -Wconversion -Werror"
mingw32-make -C $copy\build-strict -j8 SHELL=cmd.exe

# legacy demo：临时 runtime 目录 + demo/demo.sql + exit，开启 trace
$script | $copy\bin\NewDBMS.exe --trace
```

临时证据位置为 `C:\Users\HONOR\AppData\Local\Temp\aidb-p0-legacy-20260714-165718`；该路径不是交付物，也不是后续阶段的持久依赖。

### 证据解释限制

- 测试源中的 “assertion tokens” 是对 `assert_`/`fail(`/`mock(` 的词法计数，只用于确认测试不是空壳，不等价于覆盖率。
- 本轮只在 Windows/MinGW 执行；CI YAML 的存在不等于远端 job 当前成功。
- legacy 在隔离副本中构建的是当前 dirty working tree，而不是 clean `HEAD`。

## Environment and Git State

### Toolchain

| 项目 | 本轮证据 |
|---|---|
| OS | `VERIFIED` Microsoft Windows NT `10.0.26200.0`, 64-bit |
| PowerShell | `VERIFIED` 5.1.26100.8655 |
| CMake / CTest | `VERIFIED` 3.28.0-rc2 |
| C compiler | `VERIFIED` GCC/MinGW-w64 15.2.0 (`x86_64-win32-seh`) |
| Make | `VERIFIED` GNU Make 4.4.1 (`mingw32-make`) |
| Ninja | `VERIFIED` 1.13.0.git.kitware.jobserver-pipe-1 |
| Git | `VERIFIED` 2.42.0.windows.2 |
| Clang | `UNKNOWN`，命令不在 PATH |

### Current repository Git state

- `VERIFIED`：branch `main...origin/main`，HEAD `ef69eacf3381e9069c3b27d0b6c66a4ea4d62950`，最近提交 `ef69eac 2026-07-14T15:25:18+08:00 docs: define research prototype and experiments`。
- `VERIFIED`：审计开始时没有 tracked modification/deletion；有 22 个未跟踪文件条目。下列清单为 `git status --short --untracked-files=all` 的审计前状态；本报告写入后会新增/更新报告路径本身。

```text
?? AGENTS.md
?? docs/planning/00_master_spec.md
?? docs/planning/01_task_index.md
?? docs/planning/02_execution_protocol.md
?? docs/planning/03_review_checklist.md
?? docs/planning/README.md
?? docs/planning/prompts/00_initial_check.txt
?? docs/planning/prompts/01_repository_audit.txt
?? docs/planning/prompts/02_migration_matrix.txt
?? docs/planning/prompts/03_related_work.txt
?? docs/planning/prompts/04_target_architecture.txt
?? docs/planning/prompts/05_terminal_tool_skill_agent.txt
?? docs/planning/prompts/06_local_models_native_inference.txt
?? docs/planning/prompts/07_gui_mcp_product.txt
?? docs/planning/prompts/08_benchmark_research.txt
?? docs/planning/prompts/09_final_roadmap.txt
?? docs/planning/prompts/10_implementation_backlog.txt
?? docs/planning/prompts/11_execute_one_batch_template.txt
?? docs/planning/reports/.gitkeep
?? docs/planning/reports/README.md
?? include/aidb/rbt.h
?? src/core/rbt.c
```

### Legacy repository Git state

- `VERIFIED`：branch `main...origin/main [ahead 16]`，HEAD `07218651ab8ebce69ff17665940f291b4301c259`，最近提交 `0721865 2026-05-24T21:24:42+08:00 docs: add integration tutorial chapter`。
- `VERIFIED`：219 个 porcelain status entries：167 个 ` M`、2 个 ` D`、50 个 `??`。`git diff --stat` 覆盖 148 个 tracked files，约 2835 insertions / 3547 deletions，另有 binary PDF 变化。
- `VERIFIED`：tracked deletions 是 `Lib/List.c`、`Lib/List.h`；替代的 `Lib/CList.c`、`Lib/CList.h` 未跟踪。
- `VERIFIED`：下列是 50 个未跟踪条目；大量 `bin/*.exe`、数据库目录和 release 副本说明源码、运行数据和生成物边界尚不干净。

```text
?? .idea/
?? .vscode/
?? Lib/CList.c
?? Lib/CList.h
?? Show2/
?? bin/BufferManagerBasicTest.exe
?? bin/BufferManagerTest.exe
?? bin/CListTest.exe
?? bin/CStringTest.exe
?? bin/ConcurrencyBasicTest.exe
?? bin/DBErrorTest.exe
?? bin/ExpressionTest.exe
?? bin/FileManagerBasicTest.exe
?? bin/FileManagerTest.exe
?? bin/HashIndexTest.exe
?? bin/LibBasicTest.exe
?? bin/LogManagerBasicTest.exe
?? bin/LogTest.exe
?? bin/MetadataBasicTest.exe
?? bin/MetadataManagerTest.exe
?? bin/NewDBMS.exe
?? bin/ParseBasicTest.exe
?? bin/ParserTest.exe
?? bin/PlanScanBasicTest.exe
?? bin/PlanTest.exe
?? bin/RecordBasicTest.exe
?? bin/RecordTest.exe
?? bin/RecordTest/
?? bin/RecoveryBasicTest.exe
?? bin/Show2/
?? bin/SlotTest/
?? bin/TXTest/
?? bin/TransactionBasicTest.exe
?? bin/TranstionTest.exe
?? bin/UpdateBasicTest.exe
?? bin/cmocka.dll
?? bin/demo_db/
?? bin/demo_db2/
?? demo/
?? demos/
?? docs/error_handling.md
?? error/
?? main.pdf
?? release/
?? test/Lib/CListTest.c
?? test/error/
?? test/trace/
?? test_report.txt
?? test_trace_input.sql
?? 王立泽-DBMS_C-毕业设计代码-202211000117.zip
```

- `VERIFIED`：167 个 modified tracked files 横跨 `DBMS/`、`File/`、`Lib/`、`Log/`、`buffer/`、`tx/`、`record/`、`metadata/`、`parse/`、`query/`、`plan/`、`index/`、tests 和 tutorial。完整路径可由本报告记录的 `git status --short --untracked-files=all` 命令重现；P1 必须以该 dirty snapshot 为显式输入，不能把 HEAD 当作本轮证据版本。

## Current Repository Build/Test Evidence

### Configure and build

- `VERIFIED`：`cmake --preset mingw-debug` 成功。Preset 设置 `AIDB_BUILD_TESTS=ON`、`CMAKE_BUILD_TYPE=Debug`、`CMAKE_EXPORT_COMPILE_COMMANDS=ON`。
- `VERIFIED`：`cmake --build --preset mingw-debug-build` 成功，构建 `aidb_core`、`aidb` 和 9 个 test executable。
- `VERIFIED`：GCC/Clang/MinGW 路径由根 CMake 启用 `-Wall -Wextra -Wpedantic -Wshadow -Wconversion`；本轮正常构建输出未出现警告。
- `VERIFIED`：项目设置 `CMAKE_C_STANDARD 17`、`CMAKE_C_STANDARD_REQUIRED ON`、`CMAKE_C_EXTENSIONS OFF`。

### Formal tests and assertions

- `VERIFIED`：CTest 注册 9 个测试，9/9 通过，总耗时约 0.44 秒。

| CTest | 词法 assertion 数 | 结果 |
|---|---:|---|
| `basic_core_test` | 12 | `VERIFIED` pass |
| `error_test` | 20 | `VERIFIED` pass |
| `context_test` | 15 | `VERIFIED` pass |
| `memory_test` | 7 | `VERIFIED` pass |
| `arena_test` | 27 | `VERIFIED` pass |
| `binary_test` | 29 | `VERIFIED` pass |
| `vector_test` | 44 | `VERIFIED` pass |
| `list_test` | 106 | `VERIFIED` pass |
| `string_utils_test` | 60 | `VERIFIED` pass |

合计 320 个 `AIDB_TEST_ASSERT` 词法出现。`VERIFIED`：`tests/` 下没有额外未注册的 `.c` 测试源。

### Executable

- `VERIFIED`：`bin/debug/aidb.exe` 退出码 0，打印项目 banner 和 `Status : ok`。
- `VERIFIED`：其源码 `src/cli/main.c` 只执行 `aidb_context_init` → `aidb_context_last_status` → `aidb_status_message` → `aidb_context_deinit`。它不是 SQL CLI。

### Cross-platform evidence

- `DOCUMENTED`：`.github/workflows/ci.yml` 配置 Windows/MSVC、Ubuntu/GCC、macOS/AppleClang 三个平台的 configure/build/CTest。
- `UNKNOWN`：本轮没有读取 GitHub Actions 运行结果，不能把 YAML 或文档中的 “cross-platform verified” 当成本轮对当前 dirty working tree 的成功证据。尤其未跟踪的 planning/rbt 文件不会出现在普通 checkout CI 中。

## Current Repository Module Matrix

| 领域/模块 | 代码与接入证据 | 测试/活动路径 | 审计结论 |
|---|---|---|---|
| error | `include/aidb/error.h`, `src/core/error.c`, `aidb_core` | `error_test`; CLI 调用 `aidb_status_message` | `VERIFIED` implemented/tested/active |
| context | `include/aidb/context.h`, `src/core/context.c` | `context_test`; CLI init/read/deinit | `VERIFIED` implemented/tested/active |
| memory | `include/aidb/memory.h`, `src/core/memory.c` | `memory_test`; arena/containers 使用 wrapper | `VERIFIED` implemented/tested；但 header guard 有缺陷 |
| arena + platform alignment | `arena.[ch]`, internal `src/platform/align.h` | `arena_test` 覆盖增长、reset、deinit、对齐 | `VERIFIED` implemented/tested |
| binary | `binary.[ch]` | `binary_test` 覆盖 signed/unsigned LE read/write | `VERIFIED` implemented/tested |
| vector | `vector.[ch]` | `vector_test` | `VERIFIED` implemented/tested |
| list | `list.[ch]` | `list_test` | `VERIFIED` implemented/tested |
| string_utils | `string_utils.[ch]` | `string_utils_test` | `VERIFIED` implemented/tested |
| rbt | 未跟踪 `rbt.h`, `rbt.c`；不在 `aidb_core` | 无测试；独立语法检查失败 | `VERIFIED` incomplete/unbuildable，不能标 implemented |
| map / byte_buffer | 无 `.c/.h` 或 CMake target | 无 | `DOCUMENTED` planned only |
| Storage / record / buffer | `src/dbms/*/README.md` 占位 | 无 source、target、test、call path | `DOCUMENTED` planned only |
| log / transaction / recovery / concurrency | 占位 README | 无 source、target、test、call path | `DOCUMENTED` planned only |
| parser / query / plan / executor / optimizer | 占位 README | 无 SQL path | `DOCUMENTED` planned only |
| catalog / statistics / index / explain | 占位 README | 无 source、target、test | `DOCUMENTED` planned only |
| AI runtime/operators/client | `src/ai/*/README.md` | 无 source/provider/mock/test | `DOCUMENTED` designed/planned only |
| JSON / net / shell | README 占位 | 无 source/target/test | `DOCUMENTED` planned only |
| examples / benchmark / tools | README/demo placeholders | 未构建、未注册、不可作为能力证据 | `DOCUMENTED` planned only |
| CI | workflow 文件存在 | 本轮未运行远端 CI | `DOCUMENTED`; current result `UNKNOWN` |

## Legacy Repository Build/Test Evidence

### Isolation

- `VERIFIED`：legacy 根 CMake 把所有 executables 强制写入 `${PROJECT_SOURCE_DIR}/bin`。为保持源树只读，本轮没有直接 configure/build legacy；而是复制当前工作树到 `%TEMP%`，排除 `.git` 和旧生成目录，然后在副本中构建。
- `VERIFIED`：初次复制排除了所有名为 `bin` 的目录，也误排除了 `external/cmocka-1.1.0/bin/cmocka.dll`。第一次 configure 因此显示 “Tests disabled - CMocka not available”，但 core/CLI 默认构建成功。随后只把原仓库已有的 DLL 复制到临时副本，重新 configure 后注册测试。原 legacy 仓库仍无写入。

### Default build

- `VERIFIED`：legacy 设置 C11，不是 C17；`core` 静态库和 `NewDBMS.exe` 默认构建成功。
- `VERIFIED`：`core` 编译了 Lib、File、Log、Buffer、Transaction/Recovery/Concurrency、Record、Metadata、Query、Plan、Parser、HashIndex、DBMS 和 Trace 源。
- `VERIFIED`：默认 CMake 没有开启 `-Wall/-Wextra/-Wpedantic` 等严格警告；因此默认构建成功不能解释为 warning-clean。

### Registered tests

- `VERIFIED`：CMake 注册 25 个 CMocka executable/CTest；25/25 通过，总耗时约 10.87 秒。
- `VERIFIED`：`test/` 有 26 个 `.c` 测试源、108 个 `cmocka_unit_test*` 词法条目、758 个 assertion/mock/fail 词法条目。
- `VERIFIED`：唯一未注册的 `.c` 测试源是 `test/DBMSTest.c`（3 个 case、10 个 assertion tokens），它覆盖 `SimpleDBInit` 和创建事务，但注释明确指出没有 `SimpleDBDestroy`。

注册的 25 个测试为：

```text
FileManagerTest, FileManagerBasicTest,
CStringTest, CListTest, LibBasicTest, DBErrorTest,
LogTest, LogManagerBasicTest,
BufferManagerTest, BufferManagerBasicTest,
TranstionTest, TransactionBasicTest, ConcurrencyBasicTest, RecoveryBasicTest,
RecordTest, RecordBasicTest, ExpressionTest,
PlanTest, PlanScanBasicTest, UpdateBasicTest,
ParserTest, ParseBasicTest,
MetadataManagerTest, MetadataBasicTest,
HashIndexTest
```

### Controlled CLI/demo

- `VERIFIED`：在全新 `%TEMP%/.../runtime-demo` working directory 中，以 `demo/demo.sql` 加 `exit` 作为 stdin，运行 `NewDBMS.exe --trace`，退出码 0。
- `VERIFIED`：实际结果包括：3 行 student scan、`age > 21` 返回 2 行、student×score 过滤后 2 行、UPDATE 后 age=24、DELETE 后剩 2 行、view 返回 2 行、commit 成功。
- `VERIFIED`：Trace 展示 `Parser/QueryData`、Plan tree、Scan chain、record/transaction 更新和输出行数，证实 demo 不是硬编码结果。

### Strict-warning build

- `VERIFIED`：严格构建失败。首批失败包括：
  - `Lib/ByteBuffer.c:51-74`：整数到 `uint8_t` 的可能截断；
  - `Lib/CString.c:139-140`：`size_t`/`int` 转换与符号转换；
  - `Lib/map.c:27-176`：多处 sign/conversion 问题，包括把 `-1` 写入 unsigned iterator index。
- `UNKNOWN`：因为 `-Werror` 在早期 core 文件即停止，后续全部 warning 数量未枚举。

## Legacy Repository Module Matrix

| 领域/模块 | 代表类型/函数与路径 | 构建、测试、活动性 | 审计结论 |
|---|---|---|---|
| Core strings/list/vector/map/byte buffer/tokenizer/error | `Lib/CString.c`, `CList.c`, `CVector.c`, `CMap.c`, `ByteBuffer.c`, `StreamTokenizer.c`, `error/DBError.c` | core 构建；CString/CList/LibBasic/DBError tests 通过 | `VERIFIED` 当前 dirty snapshot 可构建且有基础测试；CList/DBError 源未跟踪，严格警告不干净 |
| RBT / Trie / legacy map | `Lib/RBT.c`, `Trie.c`, `map.c` | core 构建；没有 RBT/Trie 专项正式测试 | `IMPLEMENTED_UNTESTED`；不能由目录/编译推断语义完整 |
| Storage: BlockID/Page/FileManager | `BlockIDInit`, `PageGet/Set*`, `FileManagerRead/Write/Append` | FileManagerBasic/Test 通过；实际 DB path 使用 | `VERIFIED` 基础块页文件读写 |
| Buffer/LRU | `BufferManagerPin`, `BufferAssignToBlock`, `BufferFlush`, `LRUPolicyCreate` | 两个 buffer tests 通过；DB path 使用 | `VERIFIED` 基础 pin/unpin、cache hit、dirty flush、frame reuse；并发/压力边界不足 |
| Log | `LogManagerAppend`, `LogManagerFlushLSN`, reverse `LogIterator` | append/flush/reverse iteration/reopen tests 通过；transaction path 使用 | `VERIFIED` 基础 log append/flush/reopen；不等于 WAL crash correctness |
| Transaction | `TransactionInit/Commit/Rollback/Get/Set*` | Transaction tests + demo 通过 | `VERIFIED` 单进程 commit、logged int rollback 和 buffer release 基础路径 |
| Recovery | `RecoveryDoRollback`, `RecoveryDoRecover`, log record undo | rollback test 通过；没有 kill/restart crash test | rollback `VERIFIED`; restart recovery `IMPLEMENTED_UNTESTED` |
| Concurrency/locks | `ConCurrencyManager*`, `LockTableSLock/XLock/Upgrade` | 单线程状态测试通过 | `VERIFIED` 简单 lock bookkeeping；真实并发语义 `UNKNOWN` |
| Deadlock detection | `DeadlockDetector.c` | 编入 core，但除自身定义外无调用、无测试 | `IMPLEMENTED_UNTESTED` 且不在活动路径 |
| Record/schema/layout | `Schema`, `Layout`, `RID`, `RecordPage`, `TableScan` | RecordBasic/Record tests 和 demo 通过 | `VERIFIED` 基础布局、slot、insert/read/delete scan |
| Parser/query expressions | `Lexer`, `ParserQuery/UpdateCmd`, `Expression`, `Term`, `Predicate` | Parser/Parse/Expression tests + demo 通过 | `VERIFIED` 小 SQL 子集；错误路径仍会直接 `exit(1)` |
| Basic plan/scan/executor | `BasicQueryPlannerCreatPlan`, `PlanInit`, Table/Select/Project/Product Plan+Scan | PlanScan/Update tests + demo 通过 | `VERIFIED` basic SELECT/WHERE/product/projection 和 updates 活动路径 |
| BetterQueryPlanner | `plan/BetterQueryPlanner.c` | 编译，但 main/Planner 不引用；无专项测试 | `IMPLEMENTED_UNTESTED`, inactive |
| OptimizedProductPlan | 80-byte `.c` 只 include header | 编入 core，无实现/调用/测试 | `VERIFIED` placeholder, not implemented |
| UpdateScan abstraction | 71-byte `.c` 只 include header；header struct 未使用 | 编入 core，TableScan 直接提供 update callbacks | `VERIFIED` placeholder/inactive |
| Metadata/catalog/view/statistics | Table/View/Stat/Index/Metadata managers | Metadata/Update tests + demo 通过；TablePlan 使用 layout/stat | `VERIFIED` 基础 catalog/view/stat path |
| Hash index | `HashIndexInit/Next/Insert/Delete` | core 构建；HashIndexTest 只手工分配 struct 和测试 search-cost | 实现存在但数据操作 `IMPLEMENTED_UNTESTED` |
| Index integration | `CREATE INDEX` → `IndexMgrCreateIndex` 写 `idxcat` | demo/test 只验证 metadata；BasicQueryPlanner 不读 index info，updates 不维护 HashIndex | `VERIFIED` 不在端到端查询/写维护路径 |
| CLI | `main.c` REPL/meta commands | 完整 demo 退出 0 | `VERIFIED` 可用教学 CLI；固定 DB `Show2`，缺少稳定错误隔离 |
| Trace | `trace/DBTrace.c` | demo 产生 parse/plan/scan/update/tx/output trace | `VERIFIED` 活动；不是结构化 Explain，且比较运算符显示有误 |
| Formal tests | 25 CTest | 25/25 pass | `VERIFIED` 当前 MinGW snapshot 绿色；semantic coverage 有明确边界 |
| Cross-platform | CI YAML 声称 Ubuntu/Windows | 本轮只执行 MinGW；源码混用 `windows.h`, `dirent.h`, `unistd.h`, `sleep`, `strdup` | workflow `DOCUMENTED`; cross-platform success `UNKNOWN` |

## Actual End-to-End Call Paths

### Current repository

`VERIFIED`：唯一实际 executable path 是：

```text
src/cli/main.c
  -> aidb_context_init
  -> aidb_context_last_status
  -> aidb_status_message
  -> aidb_context_deinit
```

`aidb_core` 同时链接 memory/arena/binary/vector/list/string_utils，但 CLI 不调用它们；它们的 active execution evidence 来自各自 CTest。不存在 SQL → parser → planner → executor → storage 链。

### Legacy startup

`VERIFIED`：

```text
main
  -> TransactionManagerInit
  -> SimpleDBInit("Show2")
       -> LRUPolicyCreate
       -> FileManagerInit
       -> LogManagerInit
       -> BufferManagerInit
       -> SimpleDataNewTX / TransactionInit
       -> MetadataMgrInit
            -> TableManager / ViewManager / StatManager / IndexManager
       -> BasicUpdatePlannerInit
       -> BasicQueryPlannerInit
       -> PlannerInit
       -> TransactionCommit(initial catalog transaction)
  -> SimpleDataNewTX(user transaction)
```

### Legacy SELECT

`VERIFIED`：

```text
main
  -> PlannerCreateQueryPlan
  -> ParserInit / Lexer -> ParserQuery -> QueryData
  -> BasicQueryPlannerCreatPlan
       -> TablePlan (one per table or recursively expanded view)
       -> ProductPlan (multiple tables)
       -> SelectPlan
       -> ProjectPlan
  -> plan->open recursively
       -> ProjectScan -> SelectScan -> ProductScan? -> TableScan
  -> TableScan / RecordPage
  -> TransactionGetInt/GetString
  -> ConCurrencyManagerSLock
  -> BufferListGetBuffer -> BufferManager/Buffer/Page
  -> FileManagerRead on cache miss
  -> Predicate/Term/Expression evaluation
  -> SelectDataDisplay
```

### Legacy INSERT/UPDATE/DELETE and WAL ordering

`VERIFIED`：

```text
main
  -> PlannerExecuteUpdate
  -> ParserUpdateCmd
  -> BasicUpdatePlannerExecuteInsert/Modify/Delete
  -> TablePlan -> Table/Select Scan -> RecordPage
  -> TransactionSetInt/SetString
       -> ConCurrencyManagerXLock
       -> RecoverySetInt/SetString (log old value)
       -> PageSet*
       -> BufferSetModified(tx, lsn)
  -> TransactionCommit
       -> RecoveryCommit
       -> BufferManagerFlushAll
       -> BufferFlush
            -> LogManagerFlushLSN
            -> FileManagerWrite(data page)
       -> write/flush COMMIT record
       -> release locks and unpin buffers
```

这条路径提供了 WAL-before-data 的实现意图和本轮执行证据，但没有 crash injection，因此 restart correctness 仍是 `UNKNOWN`。

### Legacy rollback

`VERIFIED`：`TransactionRollback` → `RecoveryRollback` → reverse `LogIterator` → `LogRecordUnDo` → unlogged `TransactionSet*` → flush → ROLLBACK log；`RecoveryBasicTest` 验证一次 logged integer 从 222 恢复到 111。

### Legacy view and index

- `VERIFIED`：view 在 `BasicQueryPlanner` 中读取 view definition、重新 parse，并递归生成子 plan；demo 成功。
- `VERIFIED`：`CREATE INDEX` 只执行 `MetadataMgrCreateIndex` → `IndexMgrCreateIndex`，把 index/table/field 写入 `idxcat`。活动 `BasicQueryPlanner` 没有 `MetadataManagerGetIndexInfo`/`HashIndex` 引用，INSERT/UPDATE/DELETE 也没有 index maintenance。这不是完整 index execution path。

## Test-Coverage Boundaries

### Current repository

- `VERIFIED`：9 个 test executable 有实际 assertions，不是仅构建 smoke tests。
- `VERIFIED`：覆盖 NULL/invalid arguments、container ordering/copy、arena reset/alignment、binary boundary values、字符串行为等基础语义。
- `UNKNOWN`：没有覆盖率报告、sanitizer、fault-injection allocator、OOM determinism、thread sanitizer、fuzzing 或长期压力测试。
- `UNKNOWN`：本轮没有 MSVC/GCC-on-Linux/AppleClang 实际结果。
- `VERIFIED`：rbt 没有正式测试，且源码当前不通过语法检查。

### Legacy repository

- `VERIFIED`：25 个 CTest 中包含实际功能 assertions；PlanScanBasic 和 UpdateBasic 构造真实 File/Log/Buffer/Transaction/Metadata 环境，不只是 struct tests。
- `VERIFIED`：FileManager、Log reopen、dirty flush、logged integer rollback、record page/table scan、parser command families、basic select/open scan、create/insert/update/delete/view/index metadata 均有局部或集成测试。
- `VERIFIED`：`HashIndexTest` 没有调用 `HashIndexInit`、`HashBeforeFirst`、`HashIndexInsert`、`HashIndexNext` 或 `HashIndexDelete`；它只检查手工分配 struct 和 `HashIndexSearchCost`。
- `VERIFIED`：ConcurrencyBasicTest 是单线程 bookkeeping；没有两个并发线程/进程争用、超时、公平性、死锁或数据 race 测试。
- `VERIFIED`：RecoveryBasicTest 验证同一进程中的 rollback；没有非正常终止后重启、torn write、log corruption、checkpoint redo/undo 边界。
- `VERIFIED`：`test/DBMSTest.c` 未注册，故默认 CTest 不验证 SimpleDB aggregate initialization。
- `UNKNOWN`：没有 code coverage、ASan/UBSan/TSan、Valgrind、fuzzing、随机事务、large-table、buffer exhaustion 或 disk-full tests。
- `UNKNOWN`：测试使用固定/生成目录并包含自定义 cleanup；本轮在干净临时副本中顺序运行成功，但并行 CTest 与重复失败后的隔离性没有验证。
- `VERIFIED`：仓库跟踪的是 Windows `libcmocka.dll.a` + `cmocka.dll`，CMake 又只按文件存在性判定 `HAVE_CMOCKA`，不按目标平台选择库；因此 clean Linux checkout 会尝试注册并链接 Windows artifact。实际 Linux 结果仍为 `UNKNOWN`，但配置本身不是可移植的 CMocka discovery。

## Documentation/Code Conflicts

1. `VERIFIED`：当前仓库 README/ROADMAP/docs 状态矩阵总体准确地区分了已完成 core、未完成 rbt 和 planned DBMS/AI；没有把占位目录宣称为实现。
2. `DOCUMENTED` vs `UNKNOWN`：当前文档多处把模块写成 “cross-platform verified”，本地 workflow 也确实配置三平台，但本轮未获得对应远端成功记录。审计只能确认 workflow intent，不能确认当前工作树的三平台结果。
3. `VERIFIED`：当前 `memory` 被文档标为完成，但 `include/aidb/memory.h` 的 guard 写成先 `#define AIDB_MEMORY_H` 再 `#ifdef AIDB_MEMORY_H`，缺失 `#ifndef`。功能测试通过不消除 public header hygiene 缺陷。
4. `VERIFIED`：legacy README 宣称 `CREATE INDEX` 支持；parser 和 catalog metadata 确实支持，但活动 planner/executor 不选择/维护 HashIndex。文档未区分 “接受 DDL/登记元数据” 与 “索引参与查询和更新”。
5. `VERIFIED`：legacy `test/README.md` 列出 `BlockIDTest.c`、`FileTest.c`、`PageTest.c`、`ByteBufferTest.c`、`CMapTest.c`、`CVectorTest.c`、`RBTTest.c`、`mapTest.c`、`BufferTest.c`、`LRUPolicyTest.c`、`LockTest.c`、`Test.c`、`test_bench_tps.c` 等文件，但当前 `test/` 中不存在这些路径；反之 README 没有准确反映多个 `*BasicTest.c`。
6. `VERIFIED`：legacy `docs/trace_mode.md` 说 CREATE/INSERT/UPDATE/DELETE/COMMIT/ROLLBACK trace “将逐步添加”，但当前 demo 已实际打印这些 trace，文档落后于代码。
7. `VERIFIED`：legacy trace 对 `age > 21` 输出 `predicate: age=21`；`query/Term.c:81-84` 的 `TermToString` 无视 `CompareOp`，硬编码 `=`。执行语义由 `TermIsSatisfied` 正确使用 `OP_GT`，但解释/trace 事实错误。
8. `VERIFIED`：legacy CI release step 复制 `build/DBMS`，但 CMake target 是 `NewDBMS`，且 runtime output 被设置到 source `bin/`；release packaging path 与构建配置冲突。
9. `VERIFIED`：legacy README 的构建/测试说明没有提示 CMake 的测试注册取决于 Windows `cmocka.dll` + import library。缺 DLL 时 configure 成功但显示 tests disabled，容易把 0-test CTest 误认为测试成功；文件存在时，Linux 又会被配置为尝试链接 Windows import library。

## Defects and Risks Found Without Modification

### Current repository

| 严重度 | 证据与风险 |
|---|---|
| High | `VERIFIED` `src/core/rbt.c:255` 是未完成表达式，独立语法检查报 `expected ';' before '}'`；该模块当前不可构建。它尚未接入 CMake，因此默认绿色构建不会发现它。 |
| Medium | `VERIFIED` `include/aidb/memory.h:1-2` include guard 方向错误；重复 include 目前因只有函数声明通常仍可编译，但 public header 约束不正确，未来增加类型/定义时可能触发重复定义。 |
| Medium | `UNKNOWN` `arena` 的 size alignment/addition 极端溢出与 deterministic OOM 未被测试；现有测试不能证明所有 allocation failure path。 |
| Medium | `UNKNOWN` 文档的三平台验证状态没有本轮远端 run 证据；未跟踪 rbt 也不属于普通 CI checkout。 |

### Legacy repository

| 严重度 | 证据与风险 |
|---|---|
| Critical | `VERIFIED` 构建依赖未跟踪 `Lib/CList.c/.h` 和 `error/DBError.c/.h`。clean checkout 无法重现本轮 snapshot；P1 若不先固定输入会迁移错误版本。 |
| High | `VERIFIED` `query/Term.c:83` 硬编码 `=`，已在 demo 中造成 trace 对 `>` 谓词的错误解释。解释层不能作为真实执行事实源。 |
| High | `VERIFIED` `tx/recovery/LogRecord.c:18-19` 和 `:46-47` 令 `char *str = NULL` 后调用 `sprintf`；若 Rollback/Start record stringification 进入路径会写 NULL。现有 tests 未覆盖。 |
| High | `VERIFIED` `CREATE INDEX` 不建立/填充/维护可供活动 planner 使用的索引；HashIndex data operations 也未被正式测试。把它视为完整索引会导致错误迁移优先级和能力声明。 |
| High | `INFERRED` `LockTable` 是进程级 static map + busy-wait，没有可见 mutex/atomic 保护；真实多线程访问存在 data-race 风险。测试均为单线程。 |
| High | `VERIFIED` parser 在至少 `ParserExpression` 错误路径直接 `exit(1)`，许多底层函数打印错误或返回含糊值；不满足稳定 domain error / recoverable CLI boundary。 |
| High | `VERIFIED` legacy 严格警告构建失败；大量整数/size/sign 转换意味着默认成功不等于移植安全或磁盘编码安全。 |
| High | `VERIFIED` CMake 强制 runtime output 到 source `bin/`；直接 out-of-source build 仍污染 legacy 源树，且 CI/release path 与此不一致。 |
| High | `VERIFIED` `DBMS/DBMS.h` 无条件包含 `windows.h`，其他模块又依赖 `dirent.h`、`unistd.h`、`sleep` 和 POSIX `strdup`；跨 MSVC/Linux/macOS 的边界不成立。 |
| Medium | `VERIFIED` `plan/OptimizedProductPlan.c` 和 `query/UpdateScan.c` 是编译进 core 的空占位；`BetterQueryPlanner`、DeadlockDetector 编译但不在 main/Planner 活动路径。源文件存在不等于功能可用。 |
| Medium | `VERIFIED` `TableScanMoveToRid` 调用 `TableScanClose(tableScan)`，而 `TableScanClose` 把参数解释为 `Scan *`；若该路径执行会发生错误类型解释。现有测试未覆盖 move-to-RID。 |
| Medium | `INFERRED` `BufferUnPin` 无下界保护，`BufferListPin` 未检查 `BufferManagerPin` 返回 NULL；buffer exhaustion/timeout 时可能空指针或负 pin count。 |
| Medium | `VERIFIED` `SimpleDB` 没有 destroy API，main shutdown 不销毁 aggregate managers；多处 helper 分配的 CString/BlockID/Page/Plan/Parser 没有成对释放。资源泄漏未被 sanitizer/leak checker 测量。 |
| Medium | `INFERRED` 持久化格式使用 C `int`、手工 struct/header 和本机假设；没有版本、checksum、corruption detection 或跨 ABI reopen 证据。 |
| Medium | `VERIFIED` CI 在缺少 bundled Windows CMocka artifacts 时会禁用全部 tests，CTest 可能以 0 tests 返回成功；artifact 存在时，非 Windows job 又会尝试链接 Windows import library。workflow 没有平台化依赖发现或最小测试数检查。 |

## Unknowns

- `UNKNOWN`：当前仓库三平台 GitHub Actions 的当前 run 是否成功；本轮没有访问远端 CI 结果。
- `UNKNOWN`：legacy Ubuntu/Windows CI 的实际最近状态；源码与 release path 显示明显阻碍，但没有本轮远端 run 证据。
- `UNKNOWN`：legacy 167 modified / 2 deleted / 50 untracked entries 各自的来源、审核状态和预期保留边界。
- `UNKNOWN`：legacy clean HEAD、ahead-16 commit-only state、dirty working tree 三者中哪一个应作为 P1 的正式迁移输入。
- `UNKNOWN`：restart recovery 在真实 crash/torn write/checkpoint 条件下是否正确。
- `UNKNOWN`：并发锁在真实多线程、锁升级竞争、超时和死锁时是否安全。
- `UNKNOWN`：HashIndex insert/search/delete 的真实数据正确性、reopen 行为、duplicate/null/collision 语义及与 table updates 的一致性。
- `UNKNOWN`：内存泄漏、use-after-free、整数越界、未定义行为和 fuzz-resistance；没有 sanitizer/Valgrind/fuzzer 证据。
- `UNKNOWN`：磁盘格式的 endian/ABI/版本兼容性和 corruption handling。
- `UNKNOWN`：错误 SQL、缺失表/字段、重复 DDL、I/O failure、disk full、buffer exhaustion 的 CLI 与事务语义。
- `UNKNOWN`：当前仓库未跟踪 rbt 的预期完成设计；本阶段只确认其当前不可构建，不提出修复。

## Inputs Required by P1 and P2

### P1 — Legacy-to-aidb migration matrix

P1 开始前需要：

1. 本报告经 review/approval；
2. 用户明确 legacy 输入基线：当前 dirty snapshot、某个固定 commit，或先单独整理 legacy 工作树；
3. 若选择当前 snapshot，必须把未跟踪且参与构建的 `Lib/CList.*`、`error/DBError.*`、相关 tests/trace/demo 一并作为证据输入，不能只看 tracked diff；
4. 本报告的 module/activity/test boundaries，尤其是：Basic planner 活动、Better planner inactive、OptimizedProduct/UpdateScan placeholder、HashIndex 未集成、deadlock detector inactive；
5. P1 对每个模块进一步核对 public/internal/platform boundary、ownership、error semantics、C17 compatibility、test transfer 和依赖顺序。

本报告没有替 P1 给出 `REUSE_DIRECTLY` / `PORT_WITH_REFACTOR` / `REFERENCE_ONLY` / `REWRITE` / `DROP_OR_DEFER` 分类。

### P2 — Related work and research landscape

P2 可使用：

1. `docs/planning/00_master_spec.md`；
2. 当前 `docs/research/research_prototype.md` 与 `experiment_plan.md`；
3. 本报告确认的工程事实：当前仓库没有 DBMS/AI runtime/operator，legacy 只有传统关系执行链和教学 trace；
4. 本报告的测量缺口：无 AI call metrics、cache/batch/cost/quality 基线，传统 DBMS 也缺少结构化 Explain/metrics。

P2 仍需按自己的 prompt 做原始论文/官方仓库研究；P0 没有提前执行该调查。

---

P0 完成状态：

- 已验证当前仓库本地 build、9/9 CTest、minimal CLI、模块接入和 rbt 未完成状态。
- 已验证 legacy dirty snapshot 的隔离默认 build、25/25 registered CTest、严格警告失败、完整受控 demo 和真实 SQL/transaction/storage 调用链。
- 仍未知远端跨平台 CI、legacy dirty provenance、真实 crash recovery/concurrency/index integration/sanitizer 结果。
- P1 与 P2 是本报告经 review/approval 后可能开始的下一阶段；本任务没有启动其中任何一个。
