# AI-NATIVE-DBMS-C

代码短名：`aidb`

AI-NATIVE-DBMS-C 是一个使用 C17 开发的 AI-native DBMS 研究原型。项目目标是先建立可测试的 DBMS 端到端执行链，再探索把模型调用作为可计划、可解释、可缓存和可度量的查询算子，而不是在数据库外层简单包装聊天接口。

## 当前状态

项目当前处于 **Core Containers** 阶段。

- 已实现、测试并接入 CMake/CTest：`error`、`context`、`memory`、`arena`、platform alignment、`binary`、`vector`、`list`、`string_utils`。
- 上述能力由当前 GitHub Actions workflow 在 Windows/MSVC、Ubuntu/GCC 和 macOS/AppleClang 上验证。
- `rbt` 已实现、接入 CMake/CTest，并由 Windows/MSVC、Ubuntu/GCC、macOS/AppleClang 验证；正式测试覆盖确定性、随机序列和 OOM 路径。
- 当前可执行程序只是 minimal CLI/bootstrap，不是 SQL CLI。
- `map`、`byte_buffer`、DBMS、AI runtime、实验和发布能力仍处于设计或规划阶段。

## 构建与测试

Windows + MinGW 本地入口：

```powershell
cmake --preset mingw-debug
cmake --build --preset mingw-debug-build
ctest --test-dir build/mingw-debug --output-on-failure
```

也可以运行统一测试脚本：

```powershell
.\scripts\test.ps1
```

运行当前 minimal CLI/bootstrap：

```powershell
.\bin\debug\aidb.exe
```

更多环境与命令见 [构建与运行](docs/01_build_and_run.md)。

## 文档入口

- [文档索引与详细模块状态矩阵](docs/README.md)
- [开发路线图](ROADMAP.md)
- [项目概览](docs/00_project_overview.md)
- [工程规范](docs/03_coding_style.md)

## 下一步

当前先完成 public header hygiene 的三平台 CI，再进入 `map`；`byte_buffer` 及后续 DBMS/AI 阶段按 [ROADMAP](ROADMAP.md) 推进。
