# 目录职责

本文说明项目目录边界。目录可以先存在并放置 README 或 `.gitkeep`，但不能为了“看起来完整”提前创建未来功能 `.c/.h` 实现文件。

## include

- 只放公开头文件。
- `include/` 下只有 `aidb/` 一个项目命名空间目录是正常的。
- `include/README.md` 负责解释 include 目录职责。
- 不在 `include/` 下创建 `core`、`dbms`、`ai`、`json` 等平级目录。
- 未来如果公开头文件变多，只在 `include/aidb/` 下继续分 `core/dbms/ai/json` 子目录。

## src/core

通用基础设施，包括后续的 `error/result/context/memory/arena/vector/string_utils`。业务模块不应反向污染 core。

## src/platform

跨平台封装，包括后续的 `path/fs/time/thread/socket/env`。平台差异优先收敛在这里。

## src/json

手搓 JSON 子集，包括后续的 `json_builder/json_parser/json_escape`。只实现项目实际需要的 JSON 能力。

## src/net

HTTP/socket/TLS 适配。HTTP 可以手搓，TLS 先适配成熟库，不把密码学实现作为本项目目标。

## src/ai

AI Runtime，包括 `ai/core`、`ai/client`、`ai/runtime`、`ai/operators`。默认开发路径必须 mock-first。

## src/dbms

数据库内核，包括 `storage/log/buffer/tx/record/catalog/parser/planner/optimizer/executor/explain`。

## src/shell

REPL 和命令分发。命令入口保持薄，不堆数据库或 AI 业务逻辑。

## tests

- 镜像 `src`。
- 默认测试不联网。
- `tests/manual` 放真实 API 手动测试。
- 每个模块至少有 basic test。

## examples

演示脚本和 `demo.sql`。示例应帮助理解当前已实现能力，不假装未来功能已经存在。

## docs

项目文档和规范，包括架构、构建、测试、命名、内存、错误处理、Git 和 AI 辅助开发规则。

## scripts

`build/test/run/run_demo` 等辅助脚本。脚本应减少重复命令，但不能隐藏失败。

## benchmark

后期实验评测。当前早期阶段只保留目录，不提前承诺指标。

## tools

后期辅助工具。工具代码不能混入核心库。

## data

本地运行数据。只提交 `.gitkeep`，不提交运行产物。
