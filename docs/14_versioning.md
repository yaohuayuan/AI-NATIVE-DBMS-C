# 版本号规范

AI-NATIVE-DBMS-C 采用 `vX.Y.Z` 版本格式，对应语义化版本：

- `X`：主版本号 MAJOR
- `Y`：阶段版本号 MINOR
- `Z`：修订版本号 PATCH

## 1. X 什么时候 +1

`X` 表示主版本号。

规则：

- 当前 `0.x.x` 表示早期原型阶段。
- `1.0.0` 表示论文级原型机阶段，核心架构、主要模块、实验体系和技术报告基本稳定。
- 当项目发生根本性架构变化，或者从早期原型进入稳定原型阶段时，`X +1`。

结合本项目：

- `v0.x.x`：探索与原型阶段
- `v1.0.0`：AI Operator Runtime、AI-aware Optimizer、Benchmark、技术报告基本完成

## 2. Y 什么时候 +1

`Y` 表示大的开发阶段。

规则：

- 完成一个大的项目阶段，进入下一个阶段时，`Y +1`。
- `Y +1` 时，`Z` 归 0。

结合本项目：

- `v0.1.x`：工程骨架、项目规范、Core Foundation
- `v0.2.x`：DBMS Core baseline
- `v0.3.x`：Explain Plan / Scan
- `v0.4.x`：Handmade JSON、MockModel、AiClient 接口
- `v0.5.x`：AI_MATCH、AiCache、AiCallLog、最小谓词重排
- `v0.6.x`：NL2SQL、SQL Repair、Explain Agent
- `v0.7.x`：AI_SUMMARY、AI_EXTRACT
- `v0.8.x`：Batching、Budget、Materialization
- `v0.9.x`：Adaptive Semantic Execution
- `v1.0.0`：论文级原型机

示例：

- `v0.1.5` 完成 Core Foundation 后，进入 DBMS Core baseline，应变为 `v0.2.0`。

## 3. Z 什么时候 +1

`Z` 表示当前阶段内的小里程碑。

规则：

- 完成一个小文档、小模块、小测试、小修复，`Z +1`。
- 平时普通 commit 不一定都打版本号。
- 只有完成可说明的小里程碑时才建议打 tag。

结合当前阶段：

- `v0.1.0`：工程骨架初始化
- `v0.1.1`：项目规范冻结
- `v0.1.2`：Core Foundation 设计与版本号规范
- `v0.1.3`：error/result/context 最小实现
- `v0.1.4`：memory + arena
- `v0.1.5`：vector + string_utils

## 4. 当前版本状态

当前版本应写为：

- 当前版本：`v0.1.2`
- 当前阶段：Core Foundation 设计
- 下一阶段：`v0.1.3 error/result/context` 最小实现

说明：`v0.1.2` 仍然是文档和设计阶段，不代表 `error/result/context` 已经实现。

## 5. Git tag 规则

规则：

- Git tag 统一使用 `vX.Y.Z`，例如 `v0.1.2`。
- tag 只打在通过 configure/build/ctest 的 commit 上。
- 不允许给未通过测试的提交打 tag。
- 不允许把 `build`、`bin`、`data`、`.env`、API Key 等产物打入版本。

打 tag 前必须通过：

```powershell
cmake --preset mingw-debug
cmake --build --preset mingw-debug-build
ctest --test-dir build/mingw-debug --output-on-failure
```

推荐命令示例：

```powershell
git tag -a v0.1.2 -m "v0.1.2: add core foundation design and versioning policy"
git push origin v0.1.2
```
