# Git 与发布

Git 历史应该服务于回溯和协作，而不是把多个无关改动塞进一个大提交。

## 提交原则

- 小步提交。
- 一个 commit 只做一类事情。
- 提交前必须 `git status`。
- 提交前跑 configure/build/ctest。

## Commit message 建议

- `chore: initialize ai-native-dbms-c skeleton`
- `docs: freeze project coding standards`
- `core: add memory allocator wrapper`
- `core: add arena allocator`
- `test: add vector tests`

## .gitignore 原则

- 不提交 `build/`。
- 不提交 `bin/`。
- 不提交 `__cmake_systeminformation/`。
- 不提交 `data` 运行产物。
- 不提交 `.env`。
- 不提交 API Key。

## GitHub public repo 注意事项

- 不提交敏感信息。
- README 明确当前阶段。
- 早期开源不等于已经完成。
- 公开仓库中的路线图应区分“已完成”和“计划中”。

## Release

- `v0.1 skeleton`。
- `v0.1.1 documentation standards`。
- 后续再考虑 tag。
