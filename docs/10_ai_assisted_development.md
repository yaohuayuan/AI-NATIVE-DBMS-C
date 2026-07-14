# AI 辅助开发规范

AI 可以帮助项目检索、审查和小步实现，但不能替代事实证据、阶段边界和用户对核心设计的控制。

## 1. 状态事实来源

判断模块状态时按以下证据交叉验证：

- source implementation；
- public/internal header；
- CMake source 与 target；
- 独立测试和 CTest 注册；
- 当前 CI 配置及成功运行结果。

Codex/AI 不得仅根据旧文档、目录名、空头文件、占位 README 或旧 DBMS_C 的实现，认定新项目模块已经完成。文档与代码冲突时，应先报告冲突，再以源码、CMake、CTest 和 CI 为事实基线校准文档。

## 2. 用户主导的范围

以下内容由用户主导决策和实现节奏：

- 核心数据结构及其 ownership；
- DBMS storage、record、buffer、transaction、query 的核心执行路径；
- public API、磁盘格式和跨模块依赖；
- AI-native 研究主线、实验问题和评价指标；
- 是否 commit、push、tag 或 release。

Codex 可以提出选项、风险和验证证据，但不得在没有授权时扩大范围或替用户冻结高影响设计。

## 3. Codex 适合的工作

- 只读代码考古和依赖分析；
- 对照 public header、source、CMake、CTest 和 CI 校验状态；
- 编写或补充明确范围内的单元测试；
- 小范围 CMake/CTest 接入；
- code review、错误路径和 ownership 审查；
- GitHub Actions 日志分析；
- 文档事实校准、索引和冲突报告；
- 在用户给定 API/设计后完成机械性、一致性工作。

这些工作仍必须遵守允许文件、禁止文件和验证命令。

## 4. 旧项目边界

旧 DBMS_C 默认只读，只用于：

- 代码考古；
- 模块清单和依赖分析；
- 算法行为参考；
- 发现新项目路线遗漏。

禁止一键迁移、目录级复制或直接复用旧 API/命名/ownership。新项目必须重新设计 public contract、错误处理和测试。

多根工作区用于新旧项目比较时，必须明确旧项目只读。真正修改新项目时，建议只打开 AI-NATIVE-DBMS-C，减少误修改旧项目和路径混淆的风险。

## 5. 修改任务格式

每个写入任务至少说明：

- 当前目标和非目标；
- 允许修改的文件；
- 禁止修改的文件；
- 当前未提交用户改动；
- 可使用的事实来源；
- 必须运行的验证；
- commit/push 是否授权。

没有列入允许范围的文件默认不修改。遇到需要扩大范围的情况，先报告原因并等待用户确认。

## 6. 工作区保护

- 开始前运行 `git status --short`，识别已有修改和未跟踪文件。
- 不覆盖、清理、格式化或暂存用户已有改动。
- 不使用破坏性 reset/checkout 清理工作区。
- 不用 `git add .` 把无关文件纳入提交。
- 不自动 commit、push、tag 或创建 release。
- 不提交 `.env`、API Key、运行数据和构建产物。

## 7. 验证与失败处理

- 文档任务检查状态事实、链接、`git diff --check` 和最终 status。
- C/CMake 任务按要求运行 configure、build 和 CTest。
- push 后的 CI 结果必须单独确认，不能由本地成功推断。
- 构建或测试失败时保留原始错误，先定位根因，不删除测试或弱化警告。
- 权限、网络或环境问题应明确区分于代码缺陷。
- 任何无法从仓库验证的结论都标为 unknown/planned，不由 AI 补全想象。

## 8. 开发节奏

当前顺序仍是小步推进：已验证 core 基础能力，完成 rbt/map/byte_buffer 后，再进入 DBMS baseline；稳定 schema/scan/plan/executor 后才接 AI runtime/operator。AI 展示能力不应绕过 DBMS 主链提前膨胀。
