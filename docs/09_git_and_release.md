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
- `docs: add core foundation design and versioning policy`
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

## 版本号规则

项目版本号使用 `vX.Y.Z`：

- `X`：主版本号 MAJOR。
- `Y`：阶段版本号 MINOR。
- `Z`：修订版本号 PATCH。

递增规则：

- `X` 在项目发生根本性架构变化，或从早期原型进入稳定原型阶段时递增。
- `Y` 在完成一个大的项目阶段并进入下一个阶段时递增，同时 `Z` 归 0。
- `Z` 在当前阶段内完成一个小文档、小模块、小测试或小修复时递增。

当前约定：

- `v0.x.x` 表示探索与原型阶段。
- `v1.0.0` 表示论文级原型机阶段。
- `v0.1.2` 表示 Core Foundation 设计与版本号规范，不代表 `error/result/context` 已经实现。

## Git tag 规则

- Git tag 统一使用 `vX.Y.Z`，例如 `v0.1.2`。
- tag 只打在通过 configure/build/ctest 的 commit 上。
- 不允许给未通过测试的提交打 tag。
- 不允许把 `build/`、`bin/`、`data`、`.env`、API Key 等产物打入版本。

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

## GitHub public repo 注意事项

- 不提交敏感信息。
- README 明确当前阶段。
- 早期开源不等于已经完成。
- 公开仓库中的路线图应区分“已完成”和“计划中”。

## Release

- `v0.1.0 skeleton`。
- `v0.1.1 project standards`。
- `v0.1.2 core foundation design and versioning policy`。
- 后续 tag 按 `docs/14_versioning.md` 执行。
