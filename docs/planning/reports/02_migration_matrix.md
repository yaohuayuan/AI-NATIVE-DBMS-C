# P1 Legacy-to-aidb Migration Matrix

日期：2026-07-14（Asia/Shanghai）

对象：

- 当前仓库：`D:\code\AI-NATIVE-DBMS-C`
- legacy 只读参考仓库：`D:\code\DBMS\NewDBMS\DBMS_C`
- legacy 证据基线：P0 于 2026-07-14 验证过的当前 dirty working-tree snapshot；不是 clean `HEAD`

## Executive Summary

- `PROPOSED`：采用**混合迁移（hybrid migration）**，不是整库搬迁，也不是脱离 legacy 行为证据的全量重写。可验证、边界较清晰的算法和操作序列采用 `PORT_WITH_REFACTOR`；持久化格式、解析/AST、错误传播、恢复、并发、DBMS facade、CLI/trace 等架构边界采用 `REWRITE`；已被新 core 替代、未进入活动路径或首版不需要的代码采用 `REFERENCE_ONLY` 或 `DROP_OR_DEFER`。
- `PROPOSED`：**没有任何 legacy 模块分类为 `REUSE_DIRECTLY`**。legacy 使用 C11、PascalCase public API、公开结构体、裸分配、混合错误模型和穿透平台头；严格警告构建失败，工作树还依赖未跟踪源码。直接复制任何模块都会绕过新项目的 C17、`aidb_`、`aidb_status + out`、统一 allocator、opaque boundary 和默认 CTest 约束。
- `PROPOSED`：**不保持 legacy 磁盘格式兼容**。legacy page/record/log/catalog/index 格式未版本化、无 checksum/corruption contract，并混用 C `int`、裸 `LogRecordHeader` 内存布局和固定 slot/bucket 假设。新格式应从首字节起由明确的定宽编码、版本和边界检查定义。
- `PROPOSED`：**不建立运行时兼容适配器**。当前没有需要保留的生产数据证据；兼容层会把旧 ownership、错误和格式假设带入内核。若人类确认存在必须保留的数据，只允许另立任务实现一次性、只读、离线 importer，并以 golden fixtures 验证；它不得进入正常执行路径。
- `PROPOSED`：最能缩短 DBMS 主链工期的 legacy 资产是 BlockID/Page/FileManager 的行为、LogManager 的逆向页内记录布局、Buffer/BufferManager/LRU 的操作序列、RecordPage/TableScan 的 slotted-page/scan 算法，以及 BasicQueryPlanner/BasicUpdatePlanner 的最小关系执行顺序。它们减少的是算法考古和测试用例设计时间，不减少新 API、错误、所有权、格式和跨平台工作的完成标准。
- `VERIFIED`：P0 已在隔离副本验证 legacy 默认构建、25/25 注册 CTest 和完整 demo；但 restart recovery、真实并发、HashIndex 数据操作/计划选择、资源安全和跨平台结果仍不充分或未知。矩阵据此不会把“构建/测试绿色”外推为完整语义覆盖。
- `DOCUMENTED`：当前 `src/dbms/{storage,record,buffer,log,tx,parser,planner,executor,catalog,optimizer,explain}` 仍为占位 README。以下 target module 是迁移去向和依赖建议，不表示 API 已批准或模块已实现。

## 1. Scope, Inputs, and Evidence

### 1.1 本阶段范围

本报告只完成 P1：对 meaningful legacy modules 给出唯一迁移分类、目标模块、依赖顺序、测试转移、工作量和风险。没有修改或复制源代码，没有构建新 DBMS 模块，没有更新 ROADMAP/任务索引，没有研究 AI 架构或论文，也没有开始 P2/P3。

`DOCUMENTED`：`docs/planning/01_task_index.md` 当前仍把 P1 标为 `BLOCKED`（等待 P0 approval）。本次以用户在独立会话中明确要求执行 P1 作为阶段授权，但不擅自把 P0/P1 改为 `APPROVED`；本报告仍需按 review gate 审阅后才成为后续输入。

### 1.2 已读取输入

- 根 `AGENTS.md`；
- `docs/planning/00_master_spec.md`；
- `docs/planning/01_task_index.md`；
- `docs/planning/02_execution_protocol.md`；
- `docs/planning/prompts/02_migration_matrix.txt`；
- `docs/planning/reports/01_repository_audit.md`；
- `docs/migration/README.md`、`migration_rules.md`、`legacy_inventory.md`、`legacy_to_aidb_mapping.md`；
- 下表列出的 legacy headers/sources/tests，以及当前仓库 DBMS placeholder 和 core public surface。

### 1.3 主要命令与证据解释

本阶段复用 P0 同日的成功 build/test/demo 证据，没有重复整个 repository audit。新增检查均为只读：

```powershell
git status --short
rg --files docs/migration
rg --files Lib File Log buffer tx record metadata parse query plan index DBMS test trace
rg --files src/dbms include/aidb src/core tests
rg -n --glob '*.h' '<representative declarations>' File Log buffer tx record metadata parse query plan index DBMS Lib
rg -n 'add_executable|add_test|add_cmocka_test' CMakeLists.txt
rg -n -i --glob '!bin/**' --glob '!build/**' --glob '!release/**' 'json' .
rg -n '<targeted ownership/error/platform/format/call-path patterns>' <relevant legacy paths>
Get-Content -Raw -Encoding UTF8 <targeted source or planning file>
```

证据标签遵循 `AGENTS.md`：

- `VERIFIED` 表示 P0 已通过源码加成功 build/test/demo 证据确认，或本阶段直接确认了文件/调用/格式事实；
- `IMPLEMENTED_UNTESTED` 表示存在实质实现并可构建/阅读，但缺少足够正式测试或活动路径；
- `DOCUMENTED` 表示只见于文档/placeholder；
- `INFERRED` 表示从实现结构推断但未由运行验证；
- `PROPOSED` 表示本矩阵的迁移决策；
- `UNKNOWN` 表示证据不足。

所有分类均是 `PROPOSED`。矩阵“证据”列描述 legacy 当前状态，不把分类本身伪装成已验证实现。

### 1.4 Environment snapshot

`VERIFIED`（复用 P0 同日记录）：Windows NT `10.0.26200.0` 64-bit、PowerShell `5.1.26100.8655`、CMake/CTest `3.28.0-rc2`、GCC/MinGW-w64 `15.2.0`、Git `2.42.0.windows.2`。P1 没有新增跨平台执行证据，远端 CI 当前状态仍为 `UNKNOWN`。

## 2. Classification Semantics

| 分类 | 本报告中的严格含义 |
|---|---|
| `REUSE_DIRECTLY` | 原文件可基本原样进入新项目，只做命名/构建机械调整；必须已满足新 ownership/error/platform/test 约束。**本报告无此类模块。** |
| `PORT_WITH_REFACTOR` | 保留已验证的核心算法、状态转换或操作序列；用新 C17 API 手写实现，并重做 public/internal/platform、ownership、errors、allocator、format 和 tests。不是逐行翻译。 |
| `REFERENCE_ONLY` | 只提取概念、失败案例、输入/预期结果或局部算法；legacy 实现不作为新实现骨架。 |
| `REWRITE` | 目标能力需要，但 legacy 边界/语义/格式不足以作为实现骨架；从新契约和测试开始设计。 |
| `DROP_OR_DEFER` | 首条 DBMS 执行链不需要，或 legacy 只是 placeholder/inactive/unsupported；不进入近期关键路径。 |

工作量是单模块相对量级，不是日历承诺：`XS`（局部行为/文档）、`S`（单一值对象或小算法）、`M`（一个有 I/O/状态的模块）、`L`（跨模块状态机/持久化）、`XL`（并发、恢复、完整 parser/planner 等高风险系统）。

推荐 order 使用 `O00`–`O15`。相同 order 只表示可在前置条件满足后分批推进，不授权并行实现。

## 3. Core Utility Migration Matrix

| Legacy module；files / representative symbols | Target module | Classification | Evidence | Reusable concept / algorithm | Incompatible assumptions | Ownership / error / platform changes | Disk-format impact | Test migration | Prerequisites | Size | Risks | Order |
|---|---|---|---|---|---|---|---|---|---|---:|---|---:|
| `CString`：`Lib/CString.[ch]`; `CStringCreateFromCStr`, `CStringAppend*`, `CStringSubstring` | existing `aidb_string_utils`; future owning string only on demonstrated need | `REFERENCE_ONLY` | `VERIFIED` core build + `CStringTest`; strict warnings fail in conversions | Length-aware compare/copy/substring scenarios | Heap-owning string is assumed throughout legacy and APIs return mixed owned/borrowed strings | Keep non-owning utilities; future owning type must state owned/view, use `aidb_malloc/free`, status/out and no implicit `strdup`; no platform API | None directly; legacy length-prefixed strings inform negative tests only | Re-express compare/append/substring cases against existing API; add alias/capacity/OOM if owning type is ever approved | Existing core only | `XS` | Reintroducing a second string model; mistaking returned pointers as owned | `O00` |
| `CVector`, `CList`：`Lib/CVector.[ch]`, untracked build input `Lib/CList.[ch]`; `CVectorPushBack`, `CListAppend` | existing `aidb_vector`, `aidb_list` | `REFERENCE_ONLY` | `VERIFIED` core build; `CListTest`/`LibBasicTest`; legacy CList source is untracked | Growth, ordered iteration, insert/delete behavior sequences | callback-based copy/free and public layout conflict with new by-value container contract | No old API; retain new allocator/status semantics and explicit element-pointer shallowness; no platform code | None | Compare missing behavior cases only; do not copy CMocka fixtures or callbacks; add OOM/invariant cases in new harness when gaps are found | Existing core | `XS` | “Feature parity” pressure could mutate stable new contracts | `O00` |
| `RBT`, `CMap`, macro `map`：`Lib/RBT.[ch]`, `CMap.[ch]`, `map.[ch]`; `CMapInsert/Find/Erase`, `map_set` | `aidb_rbt` then future `aidb_map` internal/core | `REFERENCE_ONLY` | RBT/CMap/map build in dirty snapshot; `IMPLEMENTED_UNTESTED` for tree semantics; current aidb rbt is `VERIFIED` incomplete/unbuildable | Tree rotations/search ordering and map-on-tree idea; representative insertion/deletion sequences | nil-node layout, key/value deep-copy callbacks, unsigned iterator sentinel and public structs are incompatible; two legacy map families already show duplication | Finish/approve new rbt independently; by-value contract, comparator lifetime, iterator invalidation, allocator/OOM and invariants must be native aidb semantics | None | New randomized insert/find/delete, red-black invariants, duplicate keys, iterator invalidation and deterministic OOM; legacy only supplies sequences | Repair/approve current aidb rbt in a separate allowed batch; not a DBMS migration prerequisite if simple vectors suffice initially | `M` | Importing one of multiple inconsistent maps; blocking DBMS on unfinished generic map | `O00` |
| `ByteBuffer`：`Lib/ByteBuffer.[ch]`; `bufferPut/GetInt`, position/limit operations | existing `aidb_binary` + future internal `aidb_byte_buffer` | `PORT_WITH_REFACTOR` | `VERIFIED` core build; strict warning failure; source confirms little-endian shifts and mutable position/limit | Little-endian primitives; bounded cursor, flip/compact concepts | Cursor-position variants mutate position before reporting bounds; integer/sign conversions are warning-unsafe; Page is tightly coupled to public buffer layout | Separate stateless encoding from owned/view cursor; status/out; failure must preserve documented cursor state; fixed-width integers and checked addition; allocator via aidb | High: new format may use same endian but is intentionally not byte-compatible without a versioned spec | Golden bytes for each width, truncation, boundary overflow, position atomicity, owned/view cleanup and fuzz corpus | Existing `aidb_binary`; target page-format decision | `M` | Silent cursor advance, integer overflow, accidental claim of legacy compatibility | `O01` |
| JSON utility：no source/header found by repository-wide `rg -i json` outside artifacts | future general JSON only if provider/config work demonstrates need; not DBMS kernel | `DROP_OR_DEFER` | `VERIFIED` no legacy JSON source match in inspected snapshot | None | There is no legacy implementation to migrate | Any future JSON API requires its own scope, ownership/error and limits; do not attach it to storage migration | None | None from legacy | Separate future requirement | `XS` | Inventing work to satisfy a directory-level expectation | `O15` |
| `StreamTokenizer`, `Trie`：`Lib/StreamTokenizer.[ch]`, `Trie.[ch]`; `StreamTokenizerNext` | tokenizer is subsumed by new parser lexer; Trie deferred | `REFERENCE_ONLY` | tokenizer is `VERIFIED` active through Lexer/tests/demo; Trie `IMPLEMENTED_UNTESTED` and not shown active | Token corpus, whitespace/comment behavior; Trie concept only | tokenizer owns raw input cursor without structured source locations; Trie is not required by SQL subset | New lexer owns/borrows immutable input explicitly, returns token spans/status; no low-level exit; Trie only if measured need | None | Reuse SQL text corpus and token expectations; add locations, invalid UTF/input, truncation and fuzz | Parser scope (`O10`) | `S` | Treating a teaching tokenizer as a complete lexer; unnecessary Trie dependency | `O10` / `O15` |
| Error/locking helpers：`Lib/Error.[ch]`, untracked `error/DBError.[ch]`, `Lib/rwlock.h`, near-empty `BlockLockManager` | existing `aidb_status`; future `src/platform` synchronization | `REFERENCE_ONLY` | `VERIFIED` DBError test/build; two error systems coexist; rwlock maps to SRWLOCK/pthread; BlockLockManager is placeholder | Domain error category vocabulary; platform-lock capability list | Error pointer + void APIs, printing/exiting below CLI, public OS types and two parallel error models | Continue `aidb_status + domain out`; OS handles remain internal/platform; define timeout/cancel/deadlock statuses at lock-manager boundary | None | Translate error categories to negative cases, not types/macros; platform sync tests belong to concurrency batch | Existing error; approved concurrency design | `S` | A generic result/error subsystem fork; exposing Windows/POSIX handles | `O00` / `O09` |

## 4. Storage, Buffer, Transaction, and Recovery Matrix

| Legacy module；files / representative symbols | Target module | Classification | Evidence | Reusable concept / algorithm | Incompatible assumptions | Ownership / error / platform changes | Disk-format impact | Test migration | Prerequisites | Size | Risks | Order |
|---|---|---|---|---|---|---|---|---|---|---:|---|---:|
| Block identity：`File/BlockID.[ch]`; `BlockIDInit`, `BlockIDEqual`, `BlockIDDestroy` | `src/dbms/storage`: `aidb_block_id` value object | `PORT_WITH_REFACTOR` | `VERIFIED` file tests and active DB path | `{file identifier, block number}` value semantics; equality/format scenarios | Heap CString and heap-return BlockID; string serialization used as map key; signed `int` block number | Prefer value type; explicit borrowed/owned file identifier or catalog file id; checked nonnegative range; status for parse/format; no raw allocation in common path | New format only if serialized; do not persist ad-hoc `filename[id]` text as identity | Equality, copy, invalid block number, long/path-like name, hash/format round trip | Existing core string/binary policy | `S` | Filename lifetime and path identity leaking into page/cache keys | `O01` |
| Page：`File/Page.[ch]`; `PageGet/SetInt/String/Bytes`, `PageMaxLength` | `src/dbms/storage`: owned page + bounded page view/codec | `PORT_WITH_REFACTOR` | `VERIFIED` tests/active path; source shows little-endian, length-prefixed strings and ignored buffer errors | Fixed-size page; checked typed reads/writes; length-prefix concept | Functions return values/void and discard bounds errors; `PageGetString` allocates/leaks temp; public ByteBuffer; native `sizeof(int)` assumptions | Opaque/explicit buffer ownership; status/out; offset+length overflow checks; no mutation on failure; allocator injectable; no platform code | Replace format: fixed-width LE fields, page/file type/version where required, reserved bytes and validation; no promise to open legacy files | Golden page bytes, boundary offsets, truncated/corrupt length, aliasing, zero page, reopen via FileManager | `aidb_binary`; block-id decision | `M` | On-disk contract accidentally defined by C layout; unchecked corruption allocation | `O01` |
| File manager：`File/FileManager.[ch]`; `FileManagerRead/Write/Append/Length/GetFile` | `src/dbms/storage`: opaque `aidb_file_manager` + `src/platform` file backend | `PORT_WITH_REFACTOR` | `VERIFIED` two file tests + active path; source exposes `dirent.h`/`unistd.h`, fixed 512-byte path, `perror`, void I/O | Block read/write/append/length; cached handles; missing block reads as zero is a behavior to decide, not inherit | POSIX headers in public API, `/` path assembly, `FILE *` cache, short I/O hidden, source-tree assumptions | Opaque manager; normalized root; Windows/POSIX backend; status maps open/seek/read/write/sync/close; full init rollback/deinit; checked offsets; explicit sync/durability | Does not preserve legacy files; new block size/file header policy explicit; short last block is corruption or defined zero-fill, not silent default | Temp-dir create/open/reopen, exact bytes, append/length, short I/O, missing file, permission/disk-full fault injection, cleanup, three-platform CI | Page/BlockId | `M` | Path traversal/normalization, >2GB offsets, partial I/O, flush semantics | `O02` |
| Schema/Layout：`record/Schema.[ch]`, `Layout.[ch]`; `SchemaAddField`, `LayoutInit/Offset`, `SlotSize` | `src/dbms/record`: typed schema + physical layout | `PORT_WITH_REFACTOR` | `VERIFIED` record/metadata tests and active path | Separate logical fields from physical offsets; compute fixed slot size | linked fields + macro map; duplicate/not-found unchecked; `sizeof(int)` and string `length+4`; borrowed schema ambiguity | Schema owns immutable field descriptors; layout owns/refers under explicit lifetime; status for duplicate/not-found/overflow/type; allocator and cleanup deterministic | New schema/layout encoding; fixed-width sizes and version; alignment policy explicit; no compatibility | Duplicate fields, invalid type/length, deterministic offsets, overflow, schema copy/lifetime and golden layout | Core containers; Page format types | `M` | Layout becomes implicit disk ABI; catalog/schema ownership cycles | `O03` |
| RID：`record/RID.[ch]`; `RIDInit`, `RIDEqual` | `src/dbms/record`: `aidb_record_id` value | `PORT_WITH_REFACTOR` | `VERIFIED` record tests/active HashIndex path | `{block, slot}` stable logical locator | Heap allocation, signed ints, no generation/page identity | Value type; validated ranges; compare/encode via status; no allocator for common operations | New fixed-width encoding if persisted in indexes/WAL | Equality/copy/encode, invalid slot/block, stale/out-of-range RID | BlockId and page/slot limits | `S` | Treating RID as eternally stable after page reorganization | `O03` |
| Record page：`record/RecordPage.[ch]`; `RecordPageFormat/SearchAfter/InsertAfter`, slot flags | `src/dbms/record`: internal slotted/fixed-record page | `PORT_WITH_REFACTOR` | `VERIFIED` record tests/demo for fixed records; source confirms flag-at-slot-start layout | Format page, used/empty slot scan, insert/delete reuse | Fixed-size records only; every field prefilled; direct Transaction dependency; no header/version/free-space metadata; void updates | Internal opaque page accessor; explicit transaction/page guard; status/out and failure atomicity; cursor/page lifetime stated | Replace legacy slot layout; versioned page kind/header, checked slot count/offsets; define delete/tombstone semantics | Empty/full page, reuse, boundary slot, corrupt flag/header, reopen, update failure rollback and golden page | Page, Schema/Layout, transaction page access | `L` | Format lock-in; partial format/update; variable-length scope creep | `O07` |
| Table scan：`record/TableScan.[ch]`; `TableScanInit/Next/Insert/Delete/MoveToRid` | `src/dbms/record` cursor + later executor table-scan node | `PORT_WITH_REFACTOR` | `VERIFIED` record/plan tests/demo; P0 found bad `TableScanMoveToRid` close argument | Cursor over table blocks/slots; append block on full; RID positioning | Scan wrapper and table cursor are conflated; update callbacks mixed with read; `.tbl` naming; faulty close cast; returned strings/RIDs leak ownership | Separate storage cursor from executor node; explicit open/close, borrowed row views or copied values; status/out; close-on-error; no direct public manager internals | Uses new table/page format only; table filename mapping becomes internal catalog policy | Empty/multi-page scan, insert/delete/update, move-to-RID, cursor invalidation, error close, reopen and fixture isolation | RecordPage + Transaction | `L` | Pin leaks, stale cursor, update/read capability confusion | `O08` |
| Buffer frame：`buffer/Buffer.[ch]`; `BufferAssignToBlock`, `BufferFlush`, `BufferSetModified`, pin count | `src/dbms/buffer`: internal frame/state machine | `PORT_WITH_REFACTOR` | `VERIFIED` buffer tests + active path; source confirms WAL flush before data write | frame block identity, pin count, dirty owner/LSN, assign/flush sequence | mutable public frame; unchecked unpin; void I/O; frame owns heap Page/BlockID ambiguously | Internal-only frame; explicit states; checked pin count; status from flush/assign; borrowed frame handle tied to pin guard; mutex policy outside frame | Frame is not format owner; must reject/propagate page validation; WAL-before-data contract uses stable LSN | State transitions, dirty/clean assign, double-unpin, flush failure, WAL ordering spy and cleanup | FileManager, Page, LogManager interface | `M` | Negative pin count, flush failure losing dirty state, use-after-unpin | `O05` |
| Buffer manager：`buffer/BufferManager.[ch]`; `BufferManagerPin/TryToPin/Unpin/FlushAll` | `src/dbms/buffer`: opaque pool manager | `PORT_WITH_REFACTOR` | `VERIFIED` basic pin/hit/dirty/reuse tests + active path; exhaustion edge insufficient | block-to-frame lookup, available count, bounded wait concept | POSIX `sleep(1)`, polling timeout, NULL failure, public arrays/policy, no safe shutdown/concurrency contract | status distinguishes exhaustion/timeout/I/O; cancellation-ready wait abstraction; manager owns frames/policy; pin returns scoped handle/out; platform wait internal; metrics hooks later | None beyond Page/File/WAL contracts | Hit/miss, deterministic exhaustion, eviction, all-pinned timeout, dirty flush error, repeated pin/unpin, shutdown with pins, metrics counters | Frame + replacement policy + LogManager | `L` | Dead wait, race between policy and frame state, shutdown data loss | `O05` |
| LRU/replacement：`buffer/ReplacementPolicy.h`, `LRU/LRUCore.[ch]`, `LRUPolicy.[ch]`; policy function pointers | `src/dbms/buffer` internal replacement-policy interface; first deterministic LRU/clock choice | `PORT_WITH_REFACTOR` | `VERIFIED` compiled and buffer-tested indirectly; no isolated policy test reported | Separate victim policy from pool; recency ordering | `void *impl`, CVector/map coupling, ownership/destructor complexity, policy may diverge from pin state | Internal vtable or concrete policy only after need; manager is state authority; checked frame IDs; allocator/status; no wall-clock dependence | None | Exact access/victim sequence, pinned exclusion, remove/reinsert, capacity 1/0, OOM; deterministic tests | Core container chosen; frame IDs | `M` | Over-general policy API; inconsistent recency after failures | `O04` |
| Log manager/iterator：`Log/LogManager.[ch]`; `LogManagerAppend/FlushLSN`, `LogIteratorNext` | `src/dbms/log`: WAL byte-record store + iterator | `PORT_WITH_REFACTOR` | `VERIFIED` append/flush/reverse iteration/reopen tests + transaction path; source confirms reverse packing | monotonically increasing LSN, append, explicit flush, reverse iteration across blocks | last persisted LSN bookkeeping is not a proven durability boundary; iterator returns owning ByteBuffer unclearly; corruption lengths unchecked | Opaque manager/iterator; status/out; explicit durable-LSN definition and sync backend; bounded record length; iterator owns scratch and reports corrupt/truncated records | New log block header/record framing with magic/version/length/type/checksum decision; encode header field-by-field, never raw struct bytes | Golden blocks, multi-block reverse iteration, reopen, flush ordering, truncated/oversized header, torn tail, sync failure and idempotent close | FileManager + Page codec | `L` | False durability, trusting corrupted lengths, LSN wrap/reopen semantics | `O04` |
| Transaction log records：`tx/recovery/LogRecord.[ch]`; START/COMMIT/ROLLBACK/SETINT/SETSTRING/CHECKPOINT writers and undo callbacks | `src/dbms/log` typed WAL record codec | `REWRITE` | rollback record path partly `VERIFIED`; restart semantics `IMPLEMENTED_UNTESTED`; P0 verified NULL `sprintf` defects | Transaction boundary/update record vocabulary and old-value undo intent | public tagged union + function pointers; format uses enum/int/variable strings without version/check; malformed code may return undefined; stringification bugs | Immutable decoded records; validated variant; explicit owned payload; status/out decode; bounded strings; no function pointers in persisted representation | Completely new format; fixed-width op/type/tx/page/offset/length; checksum/version decision; no enum ABI persistence | Round-trip each record, golden bytes, unknown op, truncation, max values, malformed string, repeated decode/free; no crash claim yet | Log manager codec policy + BlockId | `L` | Format prematurely constrains recovery; unsafe decode of hostile/corrupt log | `O04` |
| Transaction BufferList：`tx/BufferList.[ch]`; `BufferListPin/GetBuffer/UnpinAll` | internal transaction pin set / page guard registry | `PORT_WITH_REFACTOR` | `VERIFIED` active transaction path; P0 inferred NULL/exhaustion hazard | Deduplicate transaction pins; release all on end | macro map + linked pin list duplicate state; raw Buffer pointers; no pin failure result; cleanup gaps | Transaction owns registry; buffer handles are borrowed while pinned; pin is status/out and failure-atomic; `release_all` returns/report first error without leaking remaining pins | None | Duplicate pin counts, missing block, exhaustion, partial failure, release-all, commit/rollback cleanup | BufferManager | `M` | Double-unpin, map/list divergence, hidden pin ownership | `O06` |
| Transaction + TransactionManager：`tx/Transaction.[ch]`, `TransactionManager.[ch]`; `TransactionCommit/Rollback/Recover`, `TransactionSet*` | `src/dbms/tx`: opaque transaction state machine; multi-session registry later | `REWRITE` | single-process commit/rollback path `VERIFIED`; transaction registry is CLI support, not DBMS correctness evidence | transaction ID/state, pin aggregation, write logging, commit/rollback sequencing | void terminal operations, public aggregate pointers, concurrency/recovery constructed unconditionally, no destroy/repeated-end contract; CLI name registry mixed in | Explicit states and legal transitions; commit/rollback status; abort-only/failed state; reverse-order cleanup; manager ownership; no implicit global transaction table; cancellation later | Transaction does not define bytes; it coordinates new WAL/page formats | begin/read/write/commit/rollback, repeated terminal calls, failure at log flush/page flush/unpin, resource release; registry tests deferred | BufferList + typed WAL + lock-free single-user policy initially | `L` core; `M` registry later | Partial commit semantics; circular dependency with recovery/locks; leaked failed tx | `O06` / `O15` |
| Recovery：`tx/recovery/RecoveryManager.[ch]`; `RecoveryDoRollback`, `RecoveryDoRecover` | `src/dbms/tx` recovery subsystem coordinated with `src/dbms/log` | `REWRITE` | same-process logged-int rollback `VERIFIED`; restart recovery `IMPLEMENTED_UNTESTED`; crash/torn/corrupt cases `UNKNOWN` | reverse log traversal; loser transaction identification; old-value undo scenarios | algorithm is effectively undo-oriented, checkpoint semantics incomplete; void failures; heap record leaks; no crash model/idempotence proof | Define recovery invariants before code; status/event reporting; idempotent operations; explicit page/log ownership; no lower-layer exit; fault-injection and crash harness required | Depends entirely on new WAL/page versions; never decode legacy WAL in normal path | same-process rollback plus process-kill points, unflushed/flushed WAL/data combinations, torn tail, corrupt record, repeated restart, checkpoint boundaries | Stable WAL, BufferManager, Transaction state, page update protocol | `XL` | False durability claims and irreversible format/algorithm lock-in | `O07` |
| Lock table/concurrency：`tx/concurrency/LockTable.[ch]`, `ConcurrencyManager.[ch]`; S/X/upgrade/release | future `src/dbms/tx` lock manager | `REWRITE` | single-thread bookkeeping `VERIFIED`; real concurrency `UNKNOWN`; source has process-global static map + busy loops without visible synchronization | S/X compatibility, lock upgrade and per-transaction held-lock set scenarios | global static table, stringified block keys, old Error, busy-wait, no mutex/atomic protection, no cancellation/fairness contract | Opaque synchronized manager; value keys; explicit conflict/timeout/deadlock/cancel statuses; RAII-like held-lock record in C; platform condition/mutex internal | Lock metadata is not persistent; must coordinate recovery separately | deterministic two+ thread barriers, S/S, S/X, upgrade races, timeout, cancellation, release on abort, sanitizer/TSan where supported | Correct single-user transaction chain first; platform sync boundary | `XL` | Data races, starvation, upgrade deadlock and nondeterministic tests | `O09` |
| DeadlockDetector + BlockLockManager placeholder：`tx/concurrency/DeadlockDetector.[ch]`, `Lib/BlockLockManager *` | later lock-manager deadlock policy | `DROP_OR_DEFER` | DeadlockDetector `IMPLEMENTED_UNTESTED`/inactive; BlockLockManager `VERIFIED` placeholder | Wait-for graph terminology may inform future tests | inactive implementation has no integration/lifecycle proof; placeholder has no reusable behavior | Do not port now; later design from lock-wait events with explicit edge ownership, victim policy and cleanup | None | Future deterministic cycle/non-cycle/removal/victim tests; none transferred as authoritative now | Working lock manager and wait instrumentation | `L` later | Premature deadlock layer obscures unsafe base locking | `O15` |

## 5. Query, Planning, Catalog, Index, and Product-Surface Matrix

| Legacy module；files / representative symbols | Target module | Classification | Evidence | Reusable concept / algorithm | Incompatible assumptions | Ownership / error / platform changes | Disk-format impact | Test migration | Prerequisites | Size | Risks | Order |
|---|---|---|---|---|---|---|---|---|---|---:|---|---:|
| Lexer：`parse/Lexer.[ch]` + `Lib/StreamTokenizer.[ch]`; `LexerMatch*`, `LexerEat*` | `src/dbms/parser`: lexer/token spans | `REWRITE` | `VERIFIED` parser tests/demo; source has multiple `exit(1)` paths | SQL keyword/delimiter/literal corpus and comment/whitespace behavior | match/eat API terminates process, lacks stable token positions and recoverable diagnostics | Lexer borrows immutable SQL; tokens carry kind/span/location; status + structured diagnostic; deterministic cleanup/OOM; no printing/exit | None | Valid token corpus, invalid/unterminated strings, unexpected chars, positions, comments, fuzz/OOM | Value/string conventions | `M` | Parser diagnostics become impossible if spans omitted; encoding ambiguity | `O10` |
| Parser + command data：`parse/Parser.[ch]`, `PredParser.[ch]`, `QueryData`, `Insert/Delete/Modify/Create*Data` | `src/dbms/parser`: AST + parse diagnostics | `REWRITE` | `VERIFIED` small SQL subset tests/demo; invalid paths insufficient | Supported grammar: CREATE TABLE/VIEW/INDEX, INSERT, SELECT/FROM/WHERE, UPDATE, DELETE; test SQL | parser directly builds many heap domain objects, mixes parse/query types, no unified AST/free/error recovery; SQL scope exceeds first minimum in places | Arena or explicit AST owner; one destroy/reset; syntax only at parser boundary; semantic errors later; status/out + location; no lower-layer exits | None | Convert parser tests/demo strings to AST golden tests; add invalid corpus, trailing tokens, nesting/limits, OOM/fuzz | Lexer + typed AST decision | `XL` | Accidental SQL-scope expansion; AST tied to legacy executor | `O10` |
| Constants/expressions/terms/predicates：`query/Constant`, `Expression`, `Term`, `Predicate`; `ExpressionEvaluate`, `TermIsSatisfied`, `PredicateSelectSubPred` | parser typed expressions + executor expression evaluator | `REWRITE` | `VERIFIED` ExpressionTest/plan/demo; source contains exits; P0 verified `TermToString` hardcodes `=` while execution uses `OP_GT` | field/constant expressions, comparison and conjunction evaluation scenarios; predicate splitting concept | “typed-ish” int/string only, no NULL/UNKNOWN contract, heap returns and string ownership unclear; trace representation is incorrect | Separate immutable AST expression from bound/executable expression; typed values with owned/view rules; status/out; explicit NULL/type errors; formatter derives from operator enum | Values may appear in record/WAL formats; use new typed encoding only | Type matrix, each comparison operator, AND short-circuit/error, missing field, NULL future decision, formatter/executor agreement | Schema + AST; TableScan/value access | `L` | Semantics drift between parse/eval/explain; memory explosion per value | `O09` design, `O11` integration |
| Generic Scan/Plan interfaces：`query/Scan.[ch]`, `plan/Plan.[ch]`; tagged unions/function pointers `ScanInit`, `PlanInit` | `src/dbms/planner` immutable plan nodes + `src/dbms/executor` runtime operators | `REWRITE` | `VERIFIED` active basic chain; placeholder update abstraction separately classified | Iterator/Volcano `before_first/next/get` model; table/select/project/product node vocabulary | public union layouts, read/write operations mixed, plan/open ownership unclear, no close-on-error/status; estimates mixed into nodes | Stable internal node tags; explicit plan ownership; executor instance owns cursor/state; status/out per operation; guaranteed close; public surface minimal | None directly | Golden plan trees, open/next/close, early close, failure propagation, repeated execution, resource accounting | Record cursor, expressions, catalog | `XL` | Plan/runtime lifetime cycles; public ABI frozen too early | `O11` |
| Concrete scans：`query/ProductScan`, `ProjectScan`, `SelectScan`; TableScan callbacks | executor table/filter/project/product operators | `PORT_WITH_REFACTOR` | `VERIFIED` plan tests/demo for select/product/projection | Nested-loop product cursor, predicate filtering, projection forwarding | callbacks assume mutable union and mixed update operations; errors collapse to false/sentinel; returned values ownership unclear | Each operator has explicit input ownership/borrow; status distinguishes EOS from error; output row/value view lifetime documented; cleanup transitive | None | Empty/single/multi rows, filter selectivity, projection order, two-table product, failure at child, early close | Generic executor + TableScan + expressions | `L` | EOS/error confusion; O(n*m) presented as optimizer; child cursor leaks | `O11` |
| Concrete plans：`plan/TablePlan`, `SelectPlan`, `ProjectPlan`, `ProductPlan` | minimal logical/physical plan nodes | `PORT_WITH_REFACTOR` | `VERIFIED` active basic chain and estimates | Minimal node composition and schema propagation; nested-loop product as first join | mutable shared schema pointers, constructors allocate without failure rollback, estimate semantics not validated broadly | Immutable owned plan tree or arena; bound table/catalog handles explicit; status/out constructors; estimates optional/marked unknown | None | Schema propagation, node composition, plan display, estimates sanity, invalid fields/tables | Catalog + expressions + executor interfaces | `L` | Conflating logical/physical plans before P3 boundary; unowned schemas | `O11` |
| Basic query planner：`plan/BasicQueryPlanner.[ch]`; table/view expansion → product → select → project | `src/dbms/planner`: first rule-based planner | `PORT_WITH_REFACTOR` | `VERIFIED` active path and demo; no HashIndex lookup | Correctness-first assembly order and recursive view expansion scenario | no semantic binding phase, product order only, view reparses stored SQL, ownership/error weak | Planner consumes validated/bound AST; status + diagnostic; plan arena/owner; do not add AI/cost claims; view support can be gated | None | One/multi-table plans, predicate/projection, missing/ambiguous field, plan golden output; view cases only when enabled | Parser, catalog, plan nodes | `L` | Porting view recursion too early; calling simple ordering “optimization” | `O12` |
| Basic update planner/executor：`plan/BasicUpdatePlanner.[ch]`; execute insert/modify/delete/create* | `src/dbms/planner` update plans + `src/dbms/executor` DDL/DML execution | `PORT_WITH_REFACTOR` | `VERIFIED` update tests/demo; CREATE INDEX only writes metadata | Operation sequences for CREATE TABLE, INSERT, UPDATE, DELETE; affected-row expectations | planning and immediate execution are mixed; mutable scan callbacks; partial failures/status absent; index maintenance missing | Separate validate/plan/execute; transaction owns atomicity; status + affected rows; dangerous actions remain outside model control; no index claim until maintained | Table/catalog/page formats only through target APIs | Migrate each statement as isolated integration fixture; duplicate/missing schema, constraint/type errors, rollback and failure injection | Transaction, catalog, parser, executor | `XL` | Partial DML, incorrect affected rows, misleading index support | `O13` |
| BetterQueryPlanner：`plan/BetterQueryPlanner.[ch]`; greedy join helpers | later optimizer research input | `REFERENCE_ONLY` | `IMPLEMENTED_UNTESTED`, compiled but inactive in `Planner`/main | Greedy join ordering questions and estimate-dependent test ideas | unvalidated cost/selectivity, ownership shared with legacy plan tree, no active evidence | Re-evaluate only after structured plan/stats/metrics; no public API migration | None | Future comparison fixtures, never expected-output oracle | Basic planner/executor + verified statistics | `M` later | Premature optimizer claims; encoding legacy heuristic as policy | `O15` |
| Placeholders：`plan/OptimizedProductPlan.[ch]`, `query/UpdateScan.[ch]` | none as separate initial modules | `DROP_OR_DEFER` | `VERIFIED` `.c` files only include header; inactive | None beyond names | No implementation to migrate; TableScan already supplies update callbacks | Do not create matching empty target files; introduce capabilities only from an approved requirement | None | None | N/A | `XS` | Directory/file parity mistaken for progress | `O15` |
| Table catalog：`metadata/TableManager.[ch]`; `tblcat`, `fldcat`, `TableManagerCreateTable/GetLayout` | `src/dbms/catalog`: minimal table/column catalog | `PORT_WITH_REFACTOR` | `VERIFIED` metadata/update tests + active TablePlan path | Catalog-as-tables, reconstruct schema/layout from rows | hard-coded names/lengths, catalog bootstrapping leaks objects, no conflict/not-found status, catalog format tied to record layout | Opaque catalog; owned immutable descriptors; transactional create/get; explicit exists/not-found/corrupt statuses; bootstrap rollback | New catalog schema/version; no legacy `tblcat/fldcat` compatibility | Bootstrap/reopen, create/get, duplicate table/field, missing table, corrupt catalog, rollback | Schema/Layout, RecordPage/TableScan, Transaction | `L` | Bootstrap circular dependency; catalog format locks layout too early | `O09` |
| View catalog：`metadata/ViewManager.[ch]`; `viewcat`, `CreateView/GetViewDef` | later catalog view support | `DROP_OR_DEFER` | `VERIFIED` demo/basic path; not required for first minimum chain | Store definition and recursive planning scenario | raw SQL text reparse, no dependency/cycle/schema invalidation handling | Later define owned normalized definition/AST strategy, cycle detection and diagnostics | New catalog only if enabled | Defer demo view cases; later create/get/reopen/cycle/stale dependency tests | Minimal catalog + parser/planner | `M` later | Expands parser/catalog lifecycle before base SELECT is stable | `O15` |
| Statistics：`metadata/StatManager`, `StatInfo`; row/block scan estimates | later `src/dbms/catalog` statistics / planner estimates | `DROP_OR_DEFER` | `VERIFIED` basic metadata/plan use; estimate quality/refresh semantics insufficient | rows/blocks/distinct-value API vocabulary and fixtures | refresh scans whole catalog/tables; ownership/cache invalidation unclear; distinct values behavior simplistic | Later immutable snapshots with version/ownership; status/out; exact vs estimated clearly labeled | Catalog format later; no legacy compatibility | Defer until basic chain; later refresh/invalidation/empty/stale/large-table tests | Stable catalog/executor metrics | `M` later | Optimizer decisions on unverified estimates | `O15` |
| Index metadata：`metadata/IndexManager`, `IndexInfo`; `idxcat`, `IndexMgrCreateIndex/GetIndexInfo` | later catalog index descriptors | `DROP_OR_DEFER` | metadata registration `VERIFIED`; planner/write integration absent | Index descriptor vocabulary and search-cost input shape | CREATE INDEX does not build/populate/maintain index; public maps return copied pointer-rich structs; cost delegates fixed-bucket formula | Reintroduce atomically with physical index lifecycle; owned descriptors/status; planner sees only usable/valid indexes | New catalog and index format; no `idxcat` compatibility | Existing metadata tests become negative warning examples; later create/build/reopen/drop/invalid-state tests | Working chosen index | `M` later | Advertising unusable indexes; catalog/physical divergence | `O15` |
| Metadata facade：`MetadataManager.[ch]`; table/view/index/stat aggregation | minimal `aidb_catalog`; later submanagers as needed | `PORT_WITH_REFACTOR` | `VERIFIED` active basic metadata path | One coordination boundary for catalog services | eagerly aggregates deferred capabilities and returns varied owning pointers/maps | Start minimal table schema API; opaque context; consistent status/owned descriptor contract; add capabilities only with implementation | New catalog only | Minimal facade integration + init failure rollback/deinit; defer view/index/stat cases | Table catalog | `M` | Recreating a god manager and circular initialization | `O09` |
| Hash index：`index/HashIndex.[ch]`; fixed `HASH_INDEX_NUM_BUCKETS`, `HashBeforeFirst/Next/Insert/Delete` | first index path, only after base DBMS; likely `src/dbms/index` if approved | `REWRITE` | implementation exists but data operations `IMPLEMENTED_UNTESTED`; no planner/update integration | Equality lookup, bucket/table layout and RID entry scenarios | fixed buckets, negative hash modulo risk, `strdup`, TableScan misuse risk, duplicate/collision/null semantics absent, no recovery/maintenance | Opaque index; typed key encoding; status/out; owned handles; transactional build/insert/delete; planner/catalog integration mandatory before capability claim | Completely new versioned index pages; no legacy bucket-table compatibility | Collision/duplicate/delete/reopen, negative/int/string keys, crash/rollback, build existing table, update maintenance, planner choice | Stable table/tx/recovery/catalog + chosen format | `XL` | Corruption and table/index divergence; schedule distraction before base chain | `O14` |
| DBMS facade：`DBMS/DBMS.[ch]`; `SimpleDBInit`, `SimpleDataNewTX` | database/bootstrap context over stable public APIs | `REWRITE` | startup order/demo `VERIFIED`; DBMSTest unregistered; no destroy API | Dependency initialization order and initial catalog transaction sequence | public aggregate, unconditional `windows.h`, fixed logfile/defaults, partial init leaks, no whole destroy | Opaque context; config validated; staged init rollback; reverse deinit; status/out; platform types hidden; transaction/session ownership explicit | Opens only new-format database; unsupported/corrupt version is a domain error | Every init stage failure, empty/new and reopen, repeated close, resource/leak checks, unregistered DBMSTest scenarios rewritten | Storage→log→buffer→tx→catalog→planner/executor complete | `L` | Facade hides incomplete shutdown/recovery; platform leakage | `O13` |
| CLI/demo：root `main.c`, `demo/demo.sql`, `demos/trace_select.sql` | thin `src/cli` client + integration fixtures | `REWRITE` | teaching CLI/demo `VERIFIED`; current aidb CLI is only minimal bootstrap | End-to-end SQL sessions, result formatting expectations, commit/rollback commands | CLI directly accesses internals, fixed `Show2`, lower layers print/exit, lifetime spread across loop | CLI calls stable public DB API only; owns input/results; maps structured errors to text; no DBMS internals/platform headers | New database only; optional importer is separate tool/task | Convert demo into noninteractive default CTest fixtures: create/insert/select/filter/product/update/delete/commit/rollback; view/index deferred | DB facade + query chain | `M` | UI behavior masking kernel errors; flaky filesystem fixtures | `O13` |
| Trace：`trace/DBTrace.[ch]`; global enable/indent, parse/plan/scan/update/tx output | `src/dbms/explain` structured facts + CLI renderer | `REWRITE` | active trace `VERIFIED`; P0 verified wrong `>` rendering as `=` | Stage vocabulary, plan/scan traversal and demo observability | global mutable state, text-only, reads internal unions, formatter can contradict execution; no metrics contract | Structured event/Explain records produced by owning subsystem; renderer outside kernel; status-safe formatter; per-context trace sink | None; do not persist raw structs as trace | Plan/execution fact golden tests, comparator fidelity, disabled/enabled isolation, nested errors; metrics later | Stable plan/executor | `L` | Explanation becomes a second source of truth; thread/global contamination | `O13` |
| Formal tests and fixtures：`test/**`, CMake `add_cmocka_test`, demo/trace scripts | new `tests/{unit,integration,support}` + default CTest | `REFERENCE_ONLY` | `VERIFIED` 25/25 registered tests in isolated dirty snapshot; `DBMSTest.c` unregistered; HashIndex/concurrency/recovery gaps verified | Inputs, operation sequences, expected results and regression cases | bundled Windows CMocka discovery, old API/struct fixtures, fixed directories, lower-layer exit, incomplete teardown and coverage | Rewrite on new API; test support owns temp dirs/resources; default registration independent of prebuilt artifacts; negative/fault/OOM tests; platform-neutral paths | Legacy data bytes only as negative/optional importer fixtures, never normal compatibility oracle | Per-row test transfer above; enforce at least one registered test per implemented batch; three-platform CI required for completion | Each implementation batch | `L` across project | Green count mistaken for coverage; fixture leakage/order dependence | continuous |

## 6. Explicit Decisions

### 6.1 Full rewrite vs whole migration vs hybrid

`PROPOSED`：选择 **hybrid migration**，但“hybrid”描述的是证据利用方式，不是混编两套源码：

1. 不复制整仓、不链接 legacy、不使用 submodule、不保留 PascalCase compatibility layer。
2. `PORT_WITH_REFACTOR` 只保留算法/状态序列，由新项目 API 和测试驱动手写。
3. `REWRITE` 从目标契约开始，legacy 只提供反例和行为语料。
4. `REFERENCE_ONLY` 不进入 target source lineage。
5. `DROP_OR_DEFER` 不创建空壳占位实现来追求文件对齐。

整库迁移会引入已验证的技术债：dirty/untracked 构建依赖、C11/strict-warning failures、裸 allocator、公开 layouts、混合 `NULL`/`void`/`Error`/`exit`、Windows/POSIX 头穿透、未版本化格式和不完整 shutdown。完全忽略 legacy 的全量重写又会浪费已经验证的 DBMS 操作序列和测试场景。因此混合策略风险最低。

### 6.2 Disk-format compatibility

`PROPOSED`：legacy 磁盘兼容**没有足够价值**，理由：

- `UNKNOWN`：没有生产/不可再生 legacy 数据集需求证据；
- `INFERRED`：格式由 `sizeof(int)`、little-endian helper、裸 `LogRecordHeader { uint32_t length; uint32_t lsn; }` 内存拷贝、固定 slot/bucket 和系统表布局共同形成，缺少独立规范；
- `VERIFIED`：无版本/checksum/corruption contract，restart/crash/index consistency 未验证；
- 兼容会把旧 format bugs 和 ABI 假设永久带入 v0.x。

新格式应至少在对应 module decision 中明确：magic/type/version、定宽整数与端序、长度上限、校验/损坏错误、升级策略和 golden bytes。具体 header 布局属于后续模块设计，不在 P1 冻结。

### 6.3 Temporary compatibility adapter

`PROPOSED`：不建立 in-process/runtime adapter，也不让新 FileManager/Page/RecordPage 识别两种格式。只有当人类确认真实数据迁移需求时，才考虑一次性 offline importer：

- 只读 legacy 文件；
- 在独立工具进程中使用经过审计的 parser/decoder；
- 输出新 API 的逻辑 rows，不暴露 legacy structs；
- 失败不修改源数据；
- 有 golden legacy fixtures、损坏输入测试和明确停止日期。

### 6.4 Modules that most reduce schedule

`PROPOSED`，按近期价值排序：

1. BlockID/Page/FileManager：行为小、已有测试/活动路径，可快速形成真实持久化基础。
2. LogManager record packing/iterator：可复用逆向追加思路和测试序列，但格式必须重做。
3. Buffer/BufferManager/LRU：pin/dirty/eviction/WAL-before-data 操作序列可显著减少状态考古。
4. Schema/Layout/RID/RecordPage/TableScan：直接缩短从页到行迭代的路径。
5. Basic Query/Update Planner + concrete scans/plans：在底层稳定后可快速组装最小 SQL 链。
6. 旧测试/demo：持续提供端到端行为清单，但不提供新 harness。

### 6.5 Reference-only or deferred modules

`PROPOSED`：旧 containers/error/locks 只作参考；BetterQueryPlanner 只作研究输入；DeadlockDetector、Trie、TransactionManager 多会话、View、Statistics、Index metadata、HashIndex 和 placeholder OptimizedProduct/UpdateScan 均不进入第一条执行链。JSON 无 legacy 实现可迁移。

### 6.6 Avoiding imported technical debt

每个后续迁移 batch 必须执行以下 gates：

1. 固定并记录只读 legacy 证据文件，不从 release/bin/生成物复制。
2. 在实现前批准 target public/internal/platform boundary；legacy header 不作为起点。
3. 每个指针标注 owned/borrowed/view；禁止无契约 heap-return。
4. 所有可失败领域操作使用 `aidb_status + out`；库层不 print/exit/abort。
5. 只用 aidb allocator；构造失败可回滚，cleanup/deinit 明确。
6. 持久化字段逐个编码，禁止 `memcpy` C struct 作为磁盘协议。
7. legacy 测试只转成行为用例；新 harness 默认注册 CTest，包含 invalid/OOM/I/O/cleanup/fault cases。
8. strict warnings、local clean build/CTest、`git diff --check` 和三平台 CI 均过门后才更新能力状态。
9. 不引入 compatibility API、legacy naming、legacy build target 或跨仓 include path。
10. 每批都写 migration decision，明确 adopt/modify/reject/unknown；`unknown` 不默认采用。

## 7. Strict Dependency Order

`PROPOSED` 的严格依赖顺序如下。箭头表示后者不得在前者的契约和测试未稳定时宣称完成；为避免预先做 P3，本序列不冻结最终库拆分或 public API 名称。

```text
O00 Existing Core Foundation / Containers gates
  -> O01 BlockID + Page codec/value semantics
  -> O02 FileManager + platform file backend
  -> O03 Schema + Layout + RID
  -> O04 typed WAL records + LogManager/iterator durability contract
  -> O05 replacement policy + Buffer frame/BufferManager
  -> O06 transaction pin registry + minimal transaction state machine
  -> O07 Recovery protocol + RecordPage
  -> O08 TableScan/storage cursor
  -> O09 minimal Catalog + typed Value/Expression design
  -> O10 Lexer + Parser + owned AST
  -> O11 Plan nodes + executor operators/scans
  -> O12 BasicQueryPlanner
  -> O13 DDL/DML update path + DB facade + structured Explain + CLI integration
  -> O14 one integrated Index path, only if selected
  -> O15 controlled concurrency/deadlock, views, statistics, better optimizer and other deferred work
```

约束说明：

- WAL 在 BufferManager 的 dirty flush 完成前必须已有可调用的 durable-LSN contract；因此 log manager 排在 buffer dirty path 前。
- RecordPage 的更新/日志原子性依赖 transaction/recovery protocol；Schema/Layout/RID 可更早独立完成。
- Catalog 依赖 TableScan，但 parser 不应依赖 catalog 的内部布局；semantic binding 在 parser AST 之后、planner 之前形成单独边界。
- controlled concurrency 不阻塞第一条单线程、可恢复执行链；它加入时必须重验 buffer/tx/recovery invariants。
- HashIndex 不得早于 table update/recovery/catalog 稳定，也不得只实现 metadata registration。

## 8. First Five Candidate Migration Batches

以下是 P1 候选，不是实施授权；每批必须在后续 backlog/implementation prompt 中进一步缩小 allowed files 和 Definition of Done。

| Batch | Scope | Classification source | Required evidence / stop condition | Estimate |
|---|---|---|---|---:|
| `MIG-01-storage-values-page` | BlockID value、owned/view Page、fixed-width bounded codec contract；不含 FileManager | BlockID/Page `PORT_WITH_REFACTOR` | public/internal ownership approved；golden bytes、bounds/corruption/OOM tests 默认 CTest；不声明 legacy compatibility | `M` |
| `MIG-02-file-manager` | opaque FileManager + Windows/POSIX backend，block read/write/append/length/reopen | FileManager `PORT_WITH_REFACTOR` | temp-dir and fault tests；short I/O/status mapping；clean local build/CTest；no OS header in public API | `M` |
| `MIG-03-schema-layout-rid` | typed Schema、deterministic physical Layout、value RID；不含 RecordPage/TableScan | Schema/Layout/RID `PORT_WITH_REFACTOR` | duplicate/type/overflow/lifetime tests；golden offsets/encoding；不把 C `int`/struct layout 固化为磁盘 ABI | `M` |
| `MIG-04-wal-store` | versioned WAL framing、append/flush/durable LSN、reverse iterator；不含 transaction recovery | LogManager `PORT_WITH_REFACTOR` + LogRecord codec `REWRITE` | golden multi-block log、reopen、truncated/torn-tail and sync-failure tests；明确“不等于 recovery complete” | `L` |
| `MIG-05-buffer-pool` | frame state、deterministic replacement、pin/unpin、dirty flush、WAL-before-data coordination | Buffer/Manager/LRU `PORT_WITH_REFACTOR` | all-pinned exhaustion、eviction、double-unpin、flush failure、WAL-order spy；无并发安全宣称 | `L` |

为什么前五批不立即包含 parser：它们先建立可独立验证的 page/file/layout/WAL/buffer chain，避免把 SQL 层建立在未定义持久化和错误语义上。`Transaction -> Recovery/RecordPage -> TableScan` 是紧随其后的候选序列，而不是本报告中启动的实现。

## 9. Risks and Uncertainties

### 9.1 Highest risks

| Risk | Evidence | Mitigation |
|---|---|---|
| 迁移输入不可复现 | `VERIFIED` legacy 有 167 modified、2 deleted、50 untracked entries，并依赖 untracked CList/DBError | 实施前由人类冻结只读 snapshot/commit/archive manifest；本报告明确只针对 2026-07-14 dirty snapshot |
| 误把绿色测试当完整正确 | `VERIFIED` 25/25 pass，但 crash/concurrency/index/cleanup gaps 明确 | 每个 target module 重写 negative/fault/concurrency/crash tests；能力声明逐项限定 |
| 新磁盘格式过早锁死 | legacy 格式未规范；P1 只提出原则 | 每个持久化模块先写 format decision/golden bytes；不在本矩阵指定具体 header |
| 依赖环 | legacy Transaction/Recovery/Buffer/Record 相互持有 | 以接口契约和分批顺序拆开 WAL store、buffer frame、transaction state、recovery protocol、record cursor |
| 技术债通过“临时兼容”永久化 | 旧 public structs/API/error/platform 假设广泛 | 明确零 `REUSE_DIRECTLY`、零 runtime adapter、零 cross-repo include/link |
| 计划层提前扩张 | Better planner/index/view 代码存在但证据不足 | 全部 reference/defer；先 basic correctness chain |

### 9.2 Unknowns retained

- `UNKNOWN`：legacy clean `HEAD`、ahead-16 commit-only state、dirty snapshot 中哪一个应成为长期、可重现的 migration evidence baseline。
- `UNKNOWN`：当前 aidb 三平台 CI 的本轮实际 run 状态；P0 只验证本地 MinGW，不能沿用旧映射文档的“三平台已验证”文字作为当前证据。
- `UNKNOWN`：legacy restart recovery 在真实 kill/torn-write/checkpoint 条件下的正确性。
- `UNKNOWN`：legacy lock/concurrency 在多线程和 sanitizer 下的安全性。
- `UNKNOWN`：legacy HashIndex 数据操作、碰撞/重复/reopen/rollback 和 table maintenance 的正确性。
- `UNKNOWN`：legacy/new code 的 leak/UAF/overflow/fuzz resistance；无本轮 sanitizer/fuzzer 证据。
- `UNKNOWN`：是否存在必须离线导入的真实 legacy 数据；当前没有此需求证据。
- `UNKNOWN`：当前未跟踪 `aidb_rbt` 的预期最终设计和完成时间；本报告不修复它，也不强制让 DBMS 首批依赖它。

## 10. Unresolved Human Decisions

进入任何实施 batch 前，需要人类决定：

1. 是否把 2026-07-14 legacy dirty snapshot 固定为正式证据基线；若不是，指定 commit/archive，并重新核对本矩阵受影响行。
2. 是否确认“不兼容 legacy 磁盘格式、无 runtime adapter”；若存在必须保留的数据，提供数据集、价值、规模和允许的一次性 importer 范围。
3. 新 page/WAL/catalog format 是否需要从第一版就包含 checksum；P1 建议版本和边界检查必须有，checksum 需结合实现复杂度单独批准。
4. 第一条执行链是否明确为单线程；本报告建议是，以便 concurrency/deadlock 保持 `O15`，但不能把它描述为线程安全 DBMS。
5. 第一个 SQL 子集是否严格限制为 master spec 的 CREATE TABLE、INSERT、SELECT/FROM/WHERE、UPDATE、DELETE；View、CREATE INDEX、statistics/optimizer 均延后。
6. `aidb_rbt/map` 是否作为 Catalog/lock 的前置，还是首批使用已完成 vector/list 加领域专用结构，避免阻塞主链。
7. 在 P8/P9 之前是否允许先审查前五个 candidate batches；本报告只给出候选，不创建 backlog 或实施任务。

## 11. Inputs Required by the Next Phase

P1 的直接交付是本矩阵。按任务索引，可在 P0/P1/P2 review gate 满足后进入 P3 Integrated Target Architecture；P2 Related Work 也可能按其独立会话规则进行。本报告不启动任何下一阶段。

P3 至少需要：

- 经审阅批准的 P0 和本 P1 报告；
- P2 报告（按任务索引是 P3 的共同前置）；
- 人类对 legacy baseline、磁盘兼容、单线程首链和首个 SQL 子集的决定；
- 本报告的零直接复用、零 runtime adapter、strict dependency order 和 deferred list；
- 对 public/internal/platform library boundaries 的架构级确认，而不是直接把矩阵中的 provisional target names 当成最终 API。

---

## Recommended Migration Strategy

`PROPOSED`：冻结可重现 legacy evidence snapshot；按 `O00 -> O15` 依赖顺序，以五个小型候选批次先建立 Page/File/Schema-Layout-RID/WAL/Buffer 基础；每批只移植已验证算法意图并用新 C17 API 手写，重做 ownership/errors/platform/format/tests；之后进入 Transaction/Recovery/Record/Catalog/Parser/Executor 主链；Concurrency 和 Index 只有在各自强测试门槛满足后才能获得能力声明。legacy 代码永远保持只读且不参加 aidb 构建。

## Strict Dependency Order

`Core -> BlockID/Page -> FileManager -> Schema/Layout/RID -> WAL store/records -> Buffer -> Transaction -> Recovery/RecordPage -> TableScan -> Catalog/Expression -> Lexer/Parser -> Plan/Executor -> Basic Planner -> DDL/DML/Facade/Explain/CLI -> optional Index -> deferred Concurrency/Optimizer/View/Statistics`。

## First Five Candidate Batches

1. `MIG-01-storage-values-page`
2. `MIG-02-file-manager`
3. `MIG-03-schema-layout-rid`
4. `MIG-04-wal-store`
5. `MIG-05-buffer-pool`

## Unresolved Human Decisions

正式 legacy snapshot、legacy 数据导入需求、首版 checksum、单线程范围、首个 SQL 子集、rbt/map 是否阻塞主链、以及候选 batch 何时转为经批准 backlog。

**Stop：P1 完成；未开始迁移实现，也未进入下一阶段。**
