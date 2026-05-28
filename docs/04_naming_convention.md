# 命名规范

项目全名：AI-NATIVE-DBMS-C。

工程短名：`aidb`。

正式文档以中文为主，代码标识符、文件名、函数名和 CMake target 使用英文 `snake_case`。

## 规则

- 目录名：`snake_case`。
- 文件名：`snake_case`。
- 函数名：`aidb_` 前缀 + `snake_case`。
- 变量名：`snake_case`。
- 结构体：`struct aidb_xxx`。
- 枚举：`enum aidb_xxx`。
- 宏：`AIDB_UPPER_SNAKE_CASE`。
- 测试文件：`xxx_test.c`。
- 测试函数：`test_xxx`。
- CMake target：`snake_case`。

## 禁止

- 不使用 PascalCase。
- 不使用 camelCase。
- 不使用自定义 `_t` 后缀类型名。
- 不使用 `WordTest`、`BufferManagerTest` 这种旧风格。
- 新项目中应写成 `word_test`、`buffer_manager_test`。

## 示例

- `aidb_malloc`
- `aidb_context_create`
- `aidb_vector_init`
- `aidb_db_open`
- `aidb_ai_client_create`
- `AIDB_MAX_PATH_LEN`

## 说明

`aidb_` 前缀不是装饰，而是 C 项目的命名空间边界。公开 API、内部跨模块函数和测试可见辅助函数都应避免污染全局符号空间。
