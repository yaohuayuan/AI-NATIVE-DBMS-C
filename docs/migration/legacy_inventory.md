# Legacy DBMS_C 模块事实清单

## 1. 范围与证据等级

本清单来自对旧项目 `D:\code\DBMS\NewDBMS\DBMS_C` 的只读考古，优先阅读了根目录设计说明、`CMakeLists.txt`、`main.c`，并检查了 `Lib/`、`File/`、`Log/`、`buffer/`、`tx/`、`record/`、`metadata/`、`parse/`、`query/`、`plan/`、`index/`、`DBMS/`、`test/`、`demo/`、`demos/` 和 `error/`。

本次忽略 `bin/`、`cmake-build-debug/`、`release/`、`.idea/`、压缩包、PDF、Word 文件、构建产物和 IDE 缓存。旧工作树存在本地修改和未跟踪内容，本次没有清理、构建或运行测试，因此：

- “存在实现”表示发现了具有实质内容的源文件，不表示逻辑完整。
- “存在测试”表示发现了测试源或 CMake 注册项，不表示当前测试通过。
- 旧文档中的能力声明只作为线索，源码、调用关系和测试仍需逐项核验。
- 本清单不改变 AI-NATIVE-DBMS-C 的模块状态；所有 DBMS 模块在新项目中仍为 `planned`。

推荐处理方式使用以下标签：`retain concept`、`behavior reference`、`redesign`、`omit from first prototype`。一个模块可以同时需要多个处理方式。

## 2. 跨模块发现

- 多个层级直接调用 `malloc`、`calloc`、`realloc` 和 `free`，没有统一分配器或一致的失败契约。
- API 混用返回 `NULL`、返回整数、`void` 加可选错误对象以及直接结束进程等方式传播错误。
- `parse/Lexer.c`、`parse/Parser.c`、`query/Constant.c`、`query/Expression.c`、`query/Predicate.c` 中发现底层 `exit(...)`。
- `File/FileManager.h` 暴露 `dirent.h`、`unistd.h`，`DBMS/DBMS.h` 直接包含 `windows.h`，平台差异穿透模块边界。
- 公共接口普遍采用 `PascalCase`、公开结构体和裸指针，所有权经常依赖调用约定而不是类型与文档。
- `Lib/BlockLockManager.c/.h`、`query/UpdateScan.c`、`plan/OptimizedProductPlan.c/.h` 接近空壳；目录或 CMake 条目不能作为完成证据。
- 旧 `Lib/Error` 与较新的 `error/DBError` 并存，说明错误模型尚未完全统一。

## 3. 模块清单

### 3.1 Lib

- **主要文件**：`CVector.*`、`CList.*`、`CString.*`、`CMap.*`、`RBT.*`、`map.*`、`ByteBuffer.*`、`StreamTokenizer.*`、`Trie.*`、`rwlock.h`、`Error.*`、近空壳的 `BlockLockManager.*`。
- **核心数据结构与职责**：动态数组、链表、字符串、红黑树/映射、字节缓冲、tokenizer、trie 和读写锁；为上层提供通用容器与辅助能力。
- **依赖与接口形态**：上层几乎普遍直接包含这些头；接口采用 `CVectorInit`、`CListAppend`、`CStringCreate...` 等 PascalCase 函数，公开结构体、函数指针回调和 `void *` 元素。
- **内存与所有权**：大量裸分配；容器通过 copy/free 回调区分复制和销毁，但浅拷贝、深拷贝与借用语义并不统一，调用者需要理解具体函数。
- **错误处理**：常以 `NULL` 或布尔/整数返回；旧 `Error` 类型存在但未成为统一基础契约。
- **已有测试**：发现 `LibBasicTest.c`、`CListTest.c`、`CStringTest.c` 及相应 CMake 测试目标；本次未验证通过状态。
- **已知问题与新项目需求**：通用能力有价值，但旧命名、裸分配、公开布局、平台锁实现和混杂契约不可直接沿用；`BlockLockManager` 不能视为有效实现。新项目需要其中部分能力。
- **推荐处理**：容器行为可作 `behavior reference`；数据结构思想可 `retain concept`；接口、所有权、错误和平台层必须 `redesign`；未进入首个原型的辅助结构可 `omit from first prototype`。

### 3.2 File

- **主要文件**：`BlockID.*`、`Page.*`、`FileManager.*`。
- **核心数据结构与职责**：块标识、页内字节访问以及数据库目录/块文件的读写、追加和长度管理。
- **依赖与接口形态**：依赖 `CString`、`ByteBuffer` 和系统文件 API；公开 `BlockID`、`Page`、`FileManager` 结构，构造函数返回堆指针，部分 I/O 函数为 `void`。
- **内存与所有权**：对象和临时缓冲直接 `malloc/calloc/free`；文件名、页缓冲和管理器之间的 owned/borrowed 关系需要从实现推断。
- **错误处理**：初始化失败可返回 `NULL`，I/O 接口没有统一状态返回；系统错误到上层的传播不完整。
- **已有测试**：发现 `FileManagerBasicTest.c`、`FileManagerTest.c` 和 CMake 注册项；未运行。
- **已知问题与新项目需求**：POSIX 头暴露在 header，平台边界不清；块大小、短读写、路径和同步错误需要重新定义。新项目需要 block/page/file 能力。
- **推荐处理**：块号与页语义 `retain concept`，旧测试作 `behavior reference`，public API、平台适配、所有权与 I/O 错误全部 `redesign`。

### 3.3 Log

- **主要文件**：`LogManager.*`，包含 `LogManager` 和 `LogIterator`。
- **核心数据结构与职责**：把日志记录追加到日志页、按 LSN flush，并逆向或顺序迭代日志记录。
- **依赖与接口形态**：依赖 `FileManager`、`Page`、`BlockID`、`ByteBuffer` 和锁；公开结构体，append 返回 LSN，iterator 返回缓冲对象。
- **内存与所有权**：管理器、迭代器和 payload 直接分配；header 中的销毁与返回缓冲所有权不够明确。
- **错误处理**：以 `NULL`、整数或隐式失败为主，缺少贯穿 append/flush/read 的统一状态。
- **已有测试**：发现 `LogManagerBasicTest.c`、`LogTest.c` 和相应 CMake 项；未运行。
- **已知问题与新项目需求**：日志页格式、持久化顺序、LSN 语义、崩溃边界和 iterator 生命周期需重新验证。新项目未来需要 WAL/log manager。
- **推荐处理**：日志页与 LSN 思想 `retain concept`，记录顺序测试作 `behavior reference`，耐久性、编码、所有权和错误模型 `redesign`。

### 3.4 buffer

- **主要文件**：`Buffer.*`、`BufferManager.*`、`ReplacementPolicy.h`、`LRU/LRUCore.*`、`LRU/LRUPolicy.*`。
- **核心数据结构与职责**：缓冲帧、pin/unpin、脏页 flush、块到帧查找、替换策略抽象和 LRU 实现。
- **依赖与接口形态**：依赖 File、Log、Lib 容器和系统时间/等待；`ReplacementPolicy` 以 `void *impl` 加函数指针模拟多态，管理器直接暴露内部成员。
- **内存与所有权**：buffer pool、策略、LRU 节点和映射分别裸分配；manager 对 policy、frame、Page/BlockID 的所有权与析构顺序不完整。
- **错误处理**：pin 失败主要返回 `NULL`，等待/超时与“无可替换帧”没有统一 `status + out` 契约。
- **已有测试**：发现 `BufferManagerBasicTest.c`、`BufferManagerTest.c` 及 CMake 目标；未运行。
- **已知问题与新项目需求**：并发安全、替换状态、WAL-before-data、重复 pin、超时和 shutdown 行为需要系统性重写。新项目需要 buffer manager 和替换策略。
- **推荐处理**：pin count、dirty page、策略边界可 `retain concept`，测试情景可作 `behavior reference`，生命周期、错误、并发与 WAL 协作必须 `redesign`。

### 3.5 tx

- **主要文件**：`Transaction.*`、`BufferList.*`、`TransactionManager.*`。
- **核心数据结构与职责**：组合文件、日志、缓冲、恢复和并发管理器，提供事务级读写、append、pin/unpin、commit、rollback、recover。
- **依赖与接口形态**：横跨 File、Log、buffer、concurrency、recovery；公开 `Transaction`，多数操作直接返回值或 `void`，manager 使用旧 map 宏。
- **内存与所有权**：事务及辅助表直接分配；事务是否拥有 recovery/concurrency/buffer list 以及提交后的销毁责任没有统一契约。
- **错误处理**：commit/rollback 等关键路径缺少一致状态返回，错误可能来自下层 `NULL`、旧 `Error` 或进程退出。
- **已有测试**：发现 `TransactionBasicTest.c`、`TranstionTest.c` 与 CMake 目标；未运行。
- **已知问题与新项目需求**：原子性、资源释放、失败状态机、事务编号、重复提交/回滚和 shutdown 语义尚需确认。新项目需要事务抽象，但应晚于稳定存储与 WAL。
- **推荐处理**：事务边界与 buffer list 用例可 `behavior reference`；状态机、public opaque type、所有权与错误传播必须 `redesign`。

### 3.6 concurrency

- **主要文件**：`tx/concurrency/ConcurrencyManager.*`、`LockTable.*`、`DeadlockDetector.*`，以及近空壳的 `Lib/BlockLockManager.*`。
- **核心数据结构与职责**：事务持有锁的跟踪、共享/排他锁表、升级与死锁检测尝试。
- **依赖与接口形态**：依赖 BlockID、旧 map、rwlock 和旧 `Error`；lock 函数多为 `void` 加错误对象，结构体公开。
- **内存与所有权**：锁表、映射、等待关系和错误对象均由裸指针连接；锁 key、等待节点与管理器的销毁责任不清晰。
- **错误处理**：旧 `Error *` 与 `void` API 组合，调用者难以强制检查；timeout/deadlock/冲突的可区分性需核验。
- **已有测试**：发现 `ConcurrencyBasicTest.c` 和 CMake 注册项；未运行，无法确认死锁与多线程覆盖。
- **已知问题与新项目需求**：模块存在不等于并发控制完整；近空壳 BlockLockManager、平台锁混用、等待图正确性和取消清理均有风险。首个单线程原型可暂不需要完整并发。
- **推荐处理**：锁模式和冲突场景可 `behavior reference`；锁管理器整体 `redesign`；完整死锁检测可 `omit from first prototype`。

### 3.7 recovery

- **主要文件**：`tx/recovery/LogRecord.*`、`RecoveryManager.*`。
- **核心数据结构与职责**：checkpoint/start/commit/rollback/set-int/set-string 日志记录，以及事务回滚和系统恢复流程。
- **依赖与接口形态**：依赖 LogManager、BufferManager、Transaction、Page/BlockID/ByteBuffer；以 tagged union 和多种 heap record 公开内部格式。
- **内存与所有权**：日志记录包含多个 heap 字符串和对象指针，创建、解析、undo 与释放的配对不统一；临时链表直接分配。
- **错误处理**：关键操作多为 `void` 或整数，无法稳定表达损坏记录、I/O 失败和恢复中止。
- **已有测试**：发现 `RecoveryBasicTest.c` 和 CMake 注册项；未运行，无法确认 crash/restart 场景。
- **已知问题与新项目需求**：WAL 格式、校验、幂等 undo、redo 策略、checkpoint 和崩溃注入均未被本次确认。新项目未来需要恢复，但依赖稳定日志、缓冲和事务。
- **推荐处理**：日志记录种类和行为用例 `behavior reference`；编码、算法、所有权、错误与测试设施全部 `redesign`。

### 3.8 record

- **主要文件**：`RID.*`、`Schema.*`、`Layout.*`、`RecordPage.*`、`TableScan.*`。
- **核心数据结构与职责**：记录标识、字段 schema、物理 layout、页内 slot 记录操作和表扫描/更新。
- **依赖与接口形态**：依赖 Transaction、BlockID、Page、CString、map/list 和 query Constant/Scan；公开结构体与大量 heap-return API。
- **内存与所有权**：Schema/field/map、RID、RecordPage、TableScan 交叉持有裸指针；Layout、RID 等对象的 destroy 契约不完整。
- **错误处理**：查找失败常返回 `NULL` 或特殊整数，读写/插入函数多为 `void`；slot 越界和类型错误没有统一状态。
- **已有测试**：发现 `RecordBasicTest.c`、`RecordTest.c` 和 CMake 目标；未运行。
- **已知问题与新项目需求**：页格式、空 slot、变长值、schema ownership、scan close 与失败回滚需要重新定义。新项目需要 record_id/schema/layout/record_page/table_scan。
- **推荐处理**：RID、layout 与 slotted-page 概念 `retain concept`，旧测试作 `behavior reference`，磁盘格式、接口与生命周期 `redesign`。

### 3.9 metadata

- **主要文件**：`MetadataManager.*`、`TableManager.*`、`ViewManager.*`、`StatManager.*`、`StatInfo.*`、`IndexManager.*`、`IndexInfo.*`。
- **核心数据结构与职责**：表、视图、统计和索引元数据的创建与查询，并聚合为 MetadataMgr。
- **依赖与接口形态**：依赖 record、Transaction、HashIndex、CString/map；manager 与 info 类型公开，返回多种裸指针。
- **内存与所有权**：不同 manager 的 free API 不一致，返回的 Schema/Layout/Info/CString 是 owned 还是 borrowed 需逐函数推断。
- **错误处理**：多以 `NULL`、布尔或隐式失败表示，缺少 catalog 冲突、not found 和持久化错误的统一分类。
- **已有测试**：发现 `MetadataBasicTest.c`、`MetadataManagerTest.c` 及 CMake 目标；未运行。
- **已知问题与新项目需求**：首个原型只需最小 catalog；视图、统计与完整索引元数据可以后置。现有持久化和事务一致性未确认。
- **推荐处理**：catalog 表思路 `retain concept`，测试数据作 `behavior reference`；先 `redesign` minimal catalog，其余 `omit from first prototype`。

### 3.10 parse

- **主要文件**：`Lexer.*`、`Parser.*`、`PredParser.*`、`QueryData.*`、`InsertData.*`、`DeleteData.*`、`ModifyData.*`、`Create*Data.*`。
- **核心数据结构与职责**：token 匹配/消费，解析查询与更新语句，并构造按命令类型区分的数据对象。
- **依赖与接口形态**：依赖 StreamTokenizer、CString、Schema 和 query 表达式类型；PascalCase、公开结构、heap-return command union，尚不是清晰分层的 lexer/parser/AST。
- **内存与所有权**：token 字符串、命令数据、list、schema 和 expression 的释放责任分散；Parser header 未呈现完整统一销毁契约。
- **错误处理**：语法错误路径发现 `exit(1)`，不适合作为库接口；部分失败返回 `NULL`，错误位置和诊断不统一。
- **已有测试**：发现 `ParseBasicTest.c`、`ParserTest.c` 和 CMake 目标；未运行。
- **已知问题与新项目需求**：编码异常注释、底层退出、AST/语义分析边界和错误恢复均需改变。新项目最终需要 lexer/parser/AST，但不属于当前 core containers 阶段。
- **推荐处理**：语法样例可作 `behavior reference`；token、AST、诊断、所有权和 public API 全部 `redesign`。

### 3.11 query

- **主要文件**：`Constant.*`、`Expression.*`、`Term.*`、`Predicate.*`、`Scan.*`、`Table/Product/Project/SelectScan.*`、近空壳 `UpdateScan.c`。
- **核心数据结构与职责**：typed-ish 常量、字段/常量表达式、谓词求值，以及基于函数指针和 tagged union 的扫描算子。
- **依赖与接口形态**：依赖 record、CString/CList、plan；公开布局，函数指针模拟接口，读写 scan 能力混在同一结构中。
- **内存与所有权**：Constant/Expression/Predicate/Scan 及返回字符串/值均使用裸指针，evaluate/getVal 返回值所有权不稳定。
- **错误处理**：发现多个 `exit(EXIT_FAILURE)`；其他路径混用 `NULL`、布尔和整数，类型错误无法可靠上传。
- **已有测试**：发现 `ExpressionTest.c`，scan 行为也出现在 plan/record 测试中；独立 query 覆盖范围未确认。
- **已知问题与新项目需求**：`UpdateScan.c` 近空壳；值类型、NULL 语义、表达式类型检查、operator 生命周期与错误传播需重建。新项目需要 typed value/expression/predicate 和执行算子。
- **推荐处理**：Volcano-style scan 与谓词用例可 `retain concept`/`behavior reference`；值系统和 executor 边界必须 `redesign`。

### 3.12 plan

- **主要文件**：`Plan.*`、`Planner.*`、`BasicQueryPlanner.*`、`BasicUpdatePlanner.*`、`BetterQueryPlanner.*`、`Table/Product/Project/SelectPlan.*`、近空壳 `OptimizedProductPlan.*`。
- **核心数据结构与职责**：把解析结果变成 table/product/select/project 计划，估计 blocks/records/distinct values，并打开 Scan 执行。
- **依赖与接口形态**：依赖 parse、query、record、metadata 和 Transaction；Plan 使用函数指针和 union，Planner 直接持有具体 planner。
- **内存与所有权**：Plan 树、Schema、Scan 和 planner 均由裸指针串联，节点及共享 schema 的销毁顺序未统一。
- **错误处理**：创建计划可能返回 `NULL` 或整数；parser/query 的 `exit` 会穿透；缺少结构化 planning/execution error。
- **已有测试**：发现 `PlanScanBasicTest.c`、`PlanTest.c`、`UpdateBasicTest.c` 及 CMake 目标；未运行。
- **已知问题与新项目需求**：`OptimizedProductPlan` 近空壳，成本模型和执行器边界尚不稳定。新项目先需要 schema/scan/plan/executor 的正确闭环，再考虑优化。
- **推荐处理**：逻辑/物理节点和估算用例可 `behavior reference`；AST-to-plan、plan ownership、executor 和成本模型 `redesign`，优化器可 `omit from first prototype`。

### 3.13 index

- **主要文件**：`HashIndex.*`。
- **核心数据结构与职责**：按固定 bucket 数把 key 映射到索引记录，并提供 before-first/next/get-RID/insert/delete/search-cost。
- **依赖与接口形态**：依赖 Transaction、Layout、Constant、TableScan/RID；公开 HashIndex，API 为 PascalCase 且多数更新函数返回 `void`。
- **内存与所有权**：index name、layout、constant 和 scan 以裸指针保存，close 的拥有范围需要从实现推断。
- **错误处理**：更新和定位操作缺少统一状态；重复键、损坏 bucket、I/O/事务失败契约未显式表达。
- **已有测试**：发现 `HashIndexTest.c` 和 CMake 目标；未运行。
- **已知问题与新项目需求**：固定 100 buckets、并发/恢复集成、持久化格式和唯一性语义需要重新设计。新项目未来可先做 basic hash index。
- **推荐处理**：bucketed index 行为可 `behavior reference`；接口、格式、扩展性、事务和错误模型 `redesign`。

### 3.14 DBMS bootstrap

- **主要文件**：`DBMS/DBMS.c`、`DBMS/DBMS.h`、`DBMS/README.md`。
- **核心数据结构与职责**：`SimpleDB` 聚合 FileManager、LogManager、BufferManager、MetadataMgr 和 Planner，并创建事务。
- **依赖与接口形态**：直接依赖几乎所有核心 DBMS manager；公开聚合结构，`SimpleDBInit` 返回 heap pointer，header 直接包含 `windows.h`。
- **内存与所有权**：bootstrap 分配所有 manager，但没有清晰的整体 destroy API 和逆序清理契约；初始化失败的部分构造清理不足。
- **错误处理**：主要依赖 `NULL` 和下层行为，初始化期间直接打印消息；缺少可组合的 status/out 接口。
- **已有测试**：发现 `DBMSTest.c` 及 CMake 目标；未运行。
- **已知问题与新项目需求**：平台依赖泄漏、固定默认参数、部分初始化、资源泄漏和恢复语义需处理。新项目未来需要 database/bootstrap context，但当前 CLI 只有 minimal bootstrap。
- **推荐处理**：manager 组合顺序可 `behavior reference`；以 opaque context、分阶段 init/deinit 和显式错误回滚 `redesign`。

### 3.15 CLI/demo

- **主要文件**：根 `main.c`、`demo/demo.sql`、`demos/trace_select.sql`。
- **核心数据结构与职责**：交互式读取 SQL/控制命令，调用 planner/transaction，显示 SELECT 结果，并演示 commit、rollback、trace 等路径。
- **依赖与接口形态**：直接包含 DBMS、planner、scan、schema 等内部接口；CLI 知道大量执行细节，不是薄组合层。
- **内存与所有权**：命令字符串、parser/planner 结果、scan 和 transaction 的释放分散，长期会话的资源边界需核验。
- **错误处理**：使用 stdout/stderr、特殊返回和下层退出；诊断不可稳定嵌入其他前端。
- **已有测试**：SQL demo 文件和 trace 脚本可提供集成场景；是否由默认 CTest 覆盖未确认。
- **已知问题与新项目需求**：旧 CLI 可以展示 SQL 路径，但不能被视为新项目能力。新项目最终可有 CLI/examples/integration tests，CLI 应只负责组合和入口。
- **推荐处理**：命令场景与输出期望可 `behavior reference`；CLI 边界、资源管理和错误展示 `redesign`。

### 3.16 tests

- **主要文件**：`test/` 下的 Lib、File、Log、buffer、tx、record、metadata、parse、query、plan、index、error 与 `DBMSTest.c`；另有 trace 脚本。
- **核心数据结构与职责**：CMocka 单元/基础测试和部分跨模块场景；根 CMake 为多个测试目标调用 `add_test`。
- **依赖与接口形态**：依赖 bundled CMocka 产物；CMake 只有在相应库/DLL 存在时才建立测试目标，因此“测试源存在”不等于任意 clean clone 都会执行测试。
- **内存与所有权**：测试常直接构造旧公开结构和 heap 对象，其 setup/teardown 本身需要审查，不能照搬为新生命周期契约。
- **错误处理**：以断言和进程结果为主；底层 `exit` 可能绕过可验证错误结果。
- **已有测试**：上述文件是事实证据，但本次未配置、构建或运行，当前通过数量未知。
- **已知问题与新项目需求**：覆盖率、隔离性、Windows-only 假设、失败路径和 crash recovery 覆盖均未确认。新项目需要重写测试并注册默认 CTest。
- **推荐处理**：输入、预期结果和回归样例作 `behavior reference`；测试 harness、fixture、所有权断言和三平台执行全部 `redesign`。

### 3.17 error

- **主要文件**：`error/DBError.*`，以及标记为 deprecated 的 `Lib/Error.*`。
- **核心数据结构与职责**：`DBStatus` 分类常见参数、内存、文件、解析、记录、buffer、锁、事务、日志和恢复错误；`DBError` 携带模块、文件、行号和消息。
- **依赖与接口形态**：提供 init/clear/set/print 和宏；但多数旧模块仍未统一返回 `DBStatus`，并继续使用 `NULL`、`void`、旧 `Error` 或 `exit`。
- **内存与所有权**：`DBError` 可由调用者提供；旧 `ErrorInit` 则返回 heap 对象，两套模型并存。
- **错误处理**：新旧契约并行，不能据 `DBError` 文件存在就认为全仓错误传播已统一。
- **已有测试**：发现 `DBErrorTest.c` 及 CMake 目标；旧 Error 也可能被 Lib 测试间接覆盖；未运行。
- **已知问题与新项目需求**：错误类型思想可参考，但命名、宏、状态分类和跨层传播要服从新项目既有 `aidb_status` 规则。
- **推荐处理**：状态分类作 `behavior reference`；不得迁移为独立 generic result 基础模块，新项目继续使用 `aidb_status + domain-specific out`。

## 4. 尚无法确认的事项

在不修改、不清理、不构建旧工作树的约束下，本次无法确认：

- 所有 CMake 测试在 clean clone 中是否都会被创建并通过。
- concurrency 的多线程正确性和 deadlock detector 完整性。
- recovery 是否覆盖真实进程崩溃、重启、redo/undo 和损坏日志。
- HashIndex、metadata、update scan 和优化计划的功能完整度。
- 所有 manager 的销毁路径是否无泄漏，以及失败初始化是否能完整回滚。
- Windows 与 POSIX 构建是否来自同一源码状态并具有等价行为。

这些不确定项应在对应模块真正进入迁移前，写入该模块的 decision，并通过新项目自己的测试和三平台 CI 消除。
