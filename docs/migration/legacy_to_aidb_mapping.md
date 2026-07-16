# Legacy DBMS_C 到 AIDB 映射

## 1. 状态口径

本表描述“旧能力在新项目中的目标去向”，不是文件复制清单。新项目当前事实为：

- `vector`、`list`、`string_utils`、`binary` 已 implemented、tested，并由当前 GitHub Actions 在 Windows/MSVC、Ubuntu/GCC、macOS/AppleClang 验证。
- `rbt` 已进入 `aidb_core` 并注册正式 CTest；本地确定性、随机序列和 OOM 测试通过，当前改动的三平台 CI 尚待验证。
- `map`、`byte_buffer` 为 `planned`。
- 存储、事务、查询、索引、catalog 和其他 DBMS 模块均为 `planned`。
- CLI 只有 minimal bootstrap；旧项目的 SQL CLI 不属于新项目当前能力。

“完成”只针对明确列出的新模块。旧模块即使存在实现和测试，也不能改变新项目状态。

## 2. Core 与容器映射

| 旧能力 | 新目标 | 新项目状态 | 可保留的思想 | 必须改变的设计 | 迁移风险 | 推荐测试来源 |
|---|---|---|---|---|---|---|
| `CVector` | `aidb_vector` | **已完成**：implemented/tested/三平台验证 | 连续存储、容量增长、按值元素 | 已采用 C17、`aidb_`、`aidb_status`、统一 allocator；不复制旧 PascalCase API | 旧 copy/free 回调与新按值复制语义不同，元素内部指针不深拷贝 | 旧 `LibBasicTest` 的增长/插入/删除场景，加新项目边界与分配失败测试 |
| `CList` | `aidb_list` | **已完成**：implemented/tested/三平台验证 | 双向/链式遍历、插入删除行为 | 已统一新命名和生命周期；容器按值复制元素，不继承旧裸 `void *` 所有权约定 | 旧 shallow/deep clone 与 callback 行为不可机械映射 | 旧 `CListTest` 行为用例，加空表、迭代失效和失败原子性测试 |
| `CString` | `string_utils`；必要时 future string abstraction | `string_utils` **已完成**；owning string abstraction **planned/未决定** | 长度受控字符串操作和复制场景 | 当前优先使用无所有权工具函数；只有真实需求出现后才设计 owning string | 直接迁移会引入另一套 heap string 和模糊所有权 | 旧 `CStringTest` 的比较/拼接/子串用例；新 API 需补 NULL、容量和别名测试 |
| `CMap` / `RBT` | `aidb_rbt` + `aidb_map` | `rbt` **implemented/locally tested，三平台 CI 待验证**；`map` **planned** | 有序树、比较回调、map 建于树之上 | rbt 使用 borrowed payload、allocator wrapper、status/out 和显式不变量验证；map 仍须另行定义 key/value 契约 | 旧 tree nil-node、key/value 深拷贝和 iterator 生命周期与新设计冲突 | rbt 已覆盖插入/查找/删除、不变量、固定种子随机序列和 OOM；map 必须另建独立测试 |
| `ByteBuffer` | `binary` + `byte_buffer` | `binary` **已完成**；`byte_buffer` **planned** | 区分编码规则与有状态读写位置 | 已把定长编码放入 `binary`；未来 buffer 必须明确 owned/view、position/limit 和失败后位置 | 把旧类整体复制会重新耦合编码、存储和分配 | 旧边界/读写序列作参考；使用 golden bytes、截断输入、端序和位置原子性测试 |

## 3. 存储与事务映射

| 旧能力 | 新目标 | 新项目状态 | 可保留的思想 | 必须改变的设计 | 迁移风险 | 推荐测试来源 |
|---|---|---|---|---|---|---|
| `BlockID` | `aidb_block_id` | **planned** | 文件标识 + block number 的值语义 | 优先按值对象；文件名 view/owned 明确；比较、hash 和格式化返回 status | 旧对象持有 heap CString，复制与销毁易错 | `FileManagerBasicTest` 的相等/块号场景，加路径边界与值复制测试 |
| `Page` | `aidb_page` | **planned** | 固定大小页和 typed read/write helper | 页缓冲 owned/view 明确；编码只经 `binary`/future `byte_buffer`；越界返回 `aidb_status` | 旧 Page 与 ByteBuffer 紧耦合，可能固化未版本化磁盘格式 | `FileManagerTest`、record/log 页访问场景，加截断、越界、端序 golden tests |
| `FileManager` | `aidb_file_manager` | **planned** | 以块为单位 read/write/append/length | opaque public type；POSIX/Windows 放入 `src/platform`；I/O 全部 status/out；完整 init/deinit | 短读写、同步、路径规范和平台错误映射 | 旧 File tests 作行为来源；新建临时目录、故障注入、clean reopen 和三平台测试 |
| `LogManager` / `LogIterator` | WAL/log manager | **planned** | LSN、append/flush、日志迭代 | 先定义 WAL durability 和 record format；manager opaque；iterator ownership 与损坏日志状态明确 | 旧日志格式、flush 顺序与 crash semantics 未验证 | `LogManagerBasicTest`/`LogTest` 用例，加 golden log、截断、重启和 fsync 故障测试 |
| `Buffer` / `BufferManager` / `LRU` | `aidb_buffer` / buffer manager / replacement policy | **planned** | pin count、dirty state、frame table、可替换策略 | manager opaque；策略接口 internal；WAL-before-data；状态机、并发和 shutdown 契约显式化 | 资源泄漏、重复 pin、无帧、flush 失败、策略与 buffer 状态不同步 | 旧 Buffer tests 作序列来源；新加 deterministic policy、故障注入、eviction/dirty/WAL tests |
| `Transaction` / `BufferList` | `aidb_transaction` | **planned** | 事务聚合 buffer/recovery/concurrency，跟踪 pinned blocks | 设计明确状态机；commit/rollback 返回 status；失败后可清理；public type opaque | 旧 void API 和跨 manager 裸指针难以恢复部分失败 | 旧 Transaction tests 作行为来源；新加重复终结、失败注入、资源归还和重启测试 |
| `LockTable` / `ConcurrencyManager` | `aidb_lock_manager` | **planned** | S/X lock、升级、事务持锁表 | status 区分 conflict/timeout/deadlock；等待和取消安全；平台同步仅 internal/platform | 旧模块完整性未确认，BlockLockManager 近空壳；多线程竞态风险高 | `ConcurrencyBasicTest` 仅作场景来源；新写 deterministic multithread、超时、死锁和清理测试 |
| `RecoveryManager` / `LogRecord` | WAL records + recovery | **planned** | 事务边界记录、page update undo 信息、checkpoint 概念 | 先版本化 record encoding，再选 recovery 算法；无底层 exit；损坏输入可报告；所有权清晰 | 旧实现的 crash correctness、redo/undo 和幂等性未确认 | `RecoveryBasicTest` 作行为来源；新建 crash/restart matrix、partial log、重复恢复与故障注入 |

## 4. 记录、目录与查询映射

| 旧能力 | 新目标 | 新项目状态 | 可保留的思想 | 必须改变的设计 | 迁移风险 | 推荐测试来源 |
|---|---|---|---|---|---|---|
| `RID` | `aidb_record_id` | **planned** | block + slot 的值标识 | 按值、可比较/编码；不以 heap-return 为默认；slot 范围显式 | 旧 heap RID 容易泄漏或产生悬空指针 | Record tests 的定位/移动场景，加序列化、无效 slot 和值复制测试 |
| `Schema` / `Layout` | `aidb_schema` / `aidb_layout` | **planned** | 逻辑字段与物理 offset 分离 | typed field、重复名/overflow 状态、owned/borrowed schema 规则；public manager 可 opaque | 旧 map/list/CString ownership 和磁盘布局耦合 | Record/Metadata tests 作来源；新加重复字段、非法类型、对齐和 layout golden tests |
| `RecordPage` / `TableScan` | `aidb_record_page` / `aidb_table_scan` | **planned** | slotted record page、scan cursor、insert/delete/update | page format 版本化；scan/executor 边界；close/deinit 明确；所有失败返回 status | 变长数据、空槽复用、失败原子性和 cursor 失效 | `RecordBasicTest`/`RecordTest`，再加满页、删除复用、重开和故障注入 |
| Metadata managers | minimal catalog；later full metadata | **planned** | 用系统表描述 table/index/view/stat | 首个原型只做最小 catalog；事务一致性、缓存 ownership 和 opaque manager 先设计 | 整体照搬会过早引入视图、统计与索引耦合 | Metadata tests 拆分为最小表定义用例；以后分别补 view/stat/index tests |
| `Lexer` / `Parser` | new lexer/parser/AST | **planned** | SQL 子集的 token 和语法场景 | lexer、parser、AST、semantic analysis 分层；错误带位置；绝不 `exit`；AST ownership 明确 | 旧 parser 直接构造多种对象，错误恢复和释放复杂 | Parse tests/demo SQL 作语料；新加 invalid corpus、fuzz、OOM 和诊断位置测试 |
| `Constant` / `Expression` / `Predicate` | typed value/expression/predicate | **planned** | 常量/字段引用、term/predicate 求值 | 明确类型与 NULL 语义；evaluation 返回 status/out；不可从库层退出；AST/physical expr 分离 | 旧返回值所有权和类型错误传播模糊 | `ExpressionTest` 和 parser/plan 谓词用例，加类型矩阵、NULL、错误传播测试 |
| `Scan` / `Plan` | operator/plan/executor | **planned** | iterator-style execution、table/select/project/product 节点 | public/internal 分层；executor 拥有运行态；plan immutable/owned 明确；先正确性后 cost | 函数指针 union、update scan 空壳、optimized plan 近空壳 | Plan/Scan tests 作行为来源；新加 plan golden tree、close-on-error、pipeline 和资源测试 |
| `HashIndex` | first basic hash index | **planned** | bucketed equality lookup 和 RID entries | bucket 数/增长、键编码、重复键、事务/WAL 集成和 opaque API 重设计 | 固定 bucket、持久化和恢复正确性未确认 | `HashIndexTest` 作来源；新加碰撞、重复、删除、重开、恢复和大数据集测试 |

## 5. 组合与展示映射

| 旧能力 | 新目标 | 新项目状态 | 可保留的思想 | 必须改变的设计 | 迁移风险 | 推荐测试来源 |
|---|---|---|---|---|---|---|
| `SimpleDB` | database/bootstrap context | **planned** | 按依赖顺序组合 file/log/buffer/catalog/planner | opaque context；部分初始化可回滚；逆序 deinit；平台头不进入 public API；status/out | 旧 bootstrap 缺整体 destroy，固定配置并泄漏 Windows 依赖 | `DBMSTest` 的启动场景；新加每阶段失败、重复关闭、clean reopen 和资源检查 |
| `main` / demo | CLI / examples / integration tests | 新项目仅有 **minimal CLI/bootstrap implemented**；SQL CLI **planned** | 交互命令和端到端 SQL 场景 | CLI 只组合 public API；不直接访问 manager/scan 内部；错误由上层展示 | 把旧 main 复制会把旧架构、所有权和 SQL 能力声明一起带入 | `demo.sql`、`trace_select.sql` 和旧 CLI 会话转成独立 integration fixtures |

## 6. 映射使用规则

1. 表中的目标名称是设计方向，不自动授权创建文件或 public API。
2. 每次只迁移一个可验证的逻辑切片，先写对应 decision，再写 header、实现和测试。
3. 旧测试只提供输入与预期行为；新测试必须使用新 API、默认注册 CTest，并验证错误与所有权。
4. 完成状态只在新项目源码、CMake/CTest 和三平台 CI 证据一致后更新。
