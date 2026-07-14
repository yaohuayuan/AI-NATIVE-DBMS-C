# Binary Encoding 设计

## Status

- `binary`：implemented、tested、registered in CMake/CTest、cross-platform verified。
- `byte_buffer`：planned，尚无 public header、实现、测试或 CMake 接入。

## 1. 设计背景

Page、File、Log、Record 等模块需要稳定地把数据写入字节序列。磁盘格式不能直接依赖 C 结构体内存布局，因为机器字节序、编译器 padding、ABI 和对齐规则都可能变化。

因此 binary 层集中实现固定宽度整数与字节之间的转换，后续模块不各自手写位运算，也不直接 `fwrite(struct)` 作为持久格式。

## 2. 与 byte_buffer 的边界

旧项目的 ByteBuffer 同时承担 position/limit/bounds 和整数编码。新项目拆成两层：

- `binary`：无状态的固定宽度整数编码，当前已实现。
- `byte_buffer`：计划中的有边界缓冲区，负责 position、limit、offset、参数和越界检查。

依赖方向保持为：

```text
byte_buffer -> binary
page        -> byte_buffer
file/log/record -> page or byte_buffer
```

## 3. 字节序

AIDB 计划中的磁盘页、日志和记录格式统一使用 little-endian。binary 显式读写 little-endian 字节序，不把主机内存表示直接复制为磁盘格式。

网络协议如有不同字节序要求，应在未来网络层单独设计，不改变当前磁盘编码规则。

## 4. 当前职责与 API

binary：

- 不分配或拥有内存；
- 不管理 position、limit 或 capacity；
- 不做 NULL 和 bounds 检查；
- 不访问文件；
- 只在调用方提供的有效地址上读写固定宽度整数。

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

NULL 或不足长度的 data 是调用者错误。计划中的 byte_buffer/page 层负责在调用 binary 前完成边界验证。

## 5. 测试

`binary_test` 已覆盖 u16/u32/u64、i16/i32/i64 的 little-endian 布局、读写回环和代表性边界值。测试不把 NULL 当作 binary 层需要防御的输入。

该测试已注册到默认 CTest，并由当前 Windows/MSVC、Ubuntu/GCC、macOS/AppleClang workflow 验证。

## 6. 后续完成条件

`byte_buffer` 只有在 public API、ownership、capacity/position/limit 规则、bounds failure、实现、独立测试、CMake/CTest 和三平台验证全部完成后，才能从 `planned` 更新为 `tested`。
