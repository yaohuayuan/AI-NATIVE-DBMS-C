# v0.1.5 Binary Encoding 设计

## 背景

AI-NATIVE-DBMS-C 后续会逐步引入 Page、File、Log、Record 等模块。这些模块最终都需要把数据写入磁盘文件，并在之后稳定地读回。

磁盘格式不能直接依赖 C 结构体的内存布局。直接 `fwrite(struct)` 会带来几个问题：

- 不同机器的大小端可能不同。
- 结构体 padding 由编译器和 ABI 决定。
- 字段对齐、布局和未来编译选项变化都可能影响磁盘内容。
- 一旦磁盘格式依赖本机内存布局，跨平台读写和长期兼容会变得不可控。

因此 v0.1.5 需要先建立一个明确的 binary encoding 层，用固定规则在数字和字节之间转换。

## 和旧 ByteBuffer 的关系

旧 ByteBuffer 同时承担了两类职责：

- 缓冲区的 position、limit、offset 和越界检查。
- 整数拆字节和组装读写。

新项目把这两类职责拆成两层：

- `binary`：只负责数字和字节之间的固定编码。
- `byte_buffer`：后续再负责 position、limit、offset、越界检查和更高层的缓冲区操作。

本阶段只实现 `binary` 设计，不实现 `byte_buffer`。

## 字节序规则

AIDB 的磁盘页、日志和记录格式统一使用 little-endian。

binary 模块会显式提供 `read/write little-endian` 函数，而不是把本机内存布局直接写入文件。无论当前机器是 little-endian 还是 big-endian，磁盘上的字节顺序都应保持一致。

后续如果引入网络协议，可以单独考虑 network byte order，但这不属于 v0.1.5 的范围。

## 模块职责

binary 是一个无状态工具层：

- 不分配内存。
- 不拥有 `data` 指针。
- 不管理 position 或 limit。
- 不做缓冲区越界检查。
- 不访问文件。
- 只负责在 `unsigned char *` 上读写固定宽度整数。

更高层模块需要先保证传入的地址有效、缓冲区长度足够，再调用 binary。

## 初始 API 草案

以下 API 只作为设计草案记录在文档中。本阶段不创建头文件。

```c
uint16_t aidb_read_u16_le(const unsigned char *data);
uint32_t aidb_read_u32_le(const unsigned char *data);
uint64_t aidb_read_u64_le(const unsigned char *data);

void aidb_write_u16_le(unsigned char *data, uint16_t value);
void aidb_write_u32_le(unsigned char *data, uint32_t value);
void aidb_write_u64_le(unsigned char *data, uint64_t value);

int16_t aidb_read_i16_le(const unsigned char *data);
int32_t aidb_read_i32_le(const unsigned char *data);
int64_t aidb_read_i64_le(const unsigned char *data);

void aidb_write_i16_le(unsigned char *data, int16_t value);
void aidb_write_i32_le(unsigned char *data, int32_t value);
void aidb_write_i64_le(unsigned char *data, int64_t value);
```

## NULL 参数策略

第一版 read/write 函数如果传入 `NULL`，行为定义为调用者错误。

binary 层保持简单、低开销，不负责参数防御。后续 `byte_buffer` 层负责做边界检查、参数检查和错误返回。

## 测试计划

后续新增 `tests/core/binary_test.c`，覆盖以下内容：

- `u16`、`u32`、`u64` 的 little-endian 字节布局。
- `i16`、`i32`、`i64` 的有符号值写入和读回。
- 特殊值：
  - `0`
  - `1`
  - `0x12345678`
  - `UINT16_MAX`
  - `UINT32_MAX`
  - `UINT64_MAX`
  - `-1`
  - `INT32_MIN`
  - `INT32_MAX`

第一版不测试 `NULL`，因为 binary 层不负责参数防御。

## 和后续模块的关系

后续依赖方向应保持清晰：

```text
byte_buffer -> binary
page        -> byte_buffer
file/log/record -> page 或 byte_buffer
```

`byte_buffer` 会调用 binary 完成固定宽度整数编码。`page` 会调用 `byte_buffer` 读写页内字段。`file`、`log`、`record` 会间接依赖 binary，从而形成稳定、可移植的磁盘格式。

后续 Page、File、Log、Record 不应各自手写位运算。固定宽度整数的拆字节规则应集中在 binary 层，避免格式规则散落在多个模块中。
