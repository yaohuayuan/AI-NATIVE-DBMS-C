# 版本号规范

AI-NATIVE-DBMS-C 使用 `vX.Y.Z` 形式的版本号。本文只定义版本语义和推进规则；当前阶段、已完成模块与下一步以 [ROADMAP](../ROADMAP.md) 为准。

## X：主版本号

- `0.x.x` 表示接口、磁盘格式和研究方向仍可调整的早期原型。
- 主版本号在发生不兼容的整体架构变化，或项目进入明确的稳定原型阶段时递增。
- `1.0.0` 只表示经过定义和验证的研究原型基线，不表示工业级 DBMS。

## Y：阶段版本号

- 完成一个大的开发阶段并进入下一阶段时递增。
- `Y` 递增时，`Z` 归零。
- 阶段边界由可交付能力和完成标准决定，不以创建目录、头文件或设计文档作为完成证据。

示例阶段可以包括：

- Core Foundation / Core Containers
- DBMS baseline
- 可解释 query execution
- AI runtime 与 mock provider
- AI operator 与 AI-aware execution
- experiments 与 research release

具体阶段名称和顺序允许随真实依赖调整，调整结果只在 ROADMAP 维护。

## Z：阶段内里程碑

- 在当前阶段完成一个可说明、可验证的小里程碑时递增。
- 普通 commit 不要求都创建新版本或 tag。
- 文档、实现、测试或修复只有形成独立可交付结果时，才适合作为里程碑。

## 版本推进条件

版本号不能替代事实状态。进入下一个实现里程碑前，应确认适用的条件：

- API、ownership 和错误语义已记录；
- 实现已接入构建；
- 测试已注册并通过；
- 要求的平台 CI 已通过；
- README、ROADMAP、状态矩阵和模块文档没有互相冲突；
- Git 工作区中没有误纳入构建产物、运行数据或密钥。

仅完成设计的里程碑必须明确标记为 design milestone，不能暗示代码已经实现。

## Git tag

- tag 使用 `vX.Y.Z`，例如 `v0.2.0`。
- tag 只创建在已提交、可复现且通过对应验证的 commit 上。
- 未通过测试的提交不得打正式 tag。
- `build/`、`bin/`、运行数据、`.env`、API Key 等不得进入 tag。
- tag 和 push 属于显式发布操作，只在用户确认后执行。

发布前至少运行项目当前规定的 configure、build 和 CTest 命令；若里程碑声明 cross-platform verified，还必须确认对应 GitHub Actions workflow 成功。
