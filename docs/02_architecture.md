# 架构草图

当前版本只提供工程骨架，下面是后续目标架构的约束性草图，不代表这些模块已经实现。

```text
User / SQL
  -> Shell / REPL
  -> Parser
  -> Planner
  -> Optimizer
  -> Executor / Scan
  -> AI Operator Runtime
  -> AiClient
  -> MockModel / Real API
```

## 分层原则

- `src/core` 提供通用基础设施，不依赖 DBMS 或 AI 业务模块。
- `src/dbms` 承载数据库内核，包括 storage、buffer、record、catalog、parser、planner、optimizer、executor、explain。
- `src/json` 提供项目需要的 JSON 子集，不引入默认运行期网络依赖。
- `src/net` 负责 HTTP/socket/TLS 适配，真实网络能力不能进入默认测试假设。
- `src/ai` 承载 AI Runtime、AiClient、mock model、AI operators。
- `src/shell` 和 `src/cli` 只做入口、REPL 和命令分发，不堆业务逻辑。

## Mock-first

AI 相关能力必须先有确定性的 mock 路径。真实 API 后续只能通过显式接口接入，并放在手动测试或受控实验中，不能让默认 CTest 依赖网络、费用或密钥。
