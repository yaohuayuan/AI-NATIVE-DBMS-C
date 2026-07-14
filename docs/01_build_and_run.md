# 构建与运行

项目使用 CMake 3.25+ 和 C17。Windows 本地开发使用 MinGW preset；GitHub Actions 另外验证 Windows/MSVC、Ubuntu/GCC 和 macOS/AppleClang。

## Windows + MinGW 环境

- Windows PowerShell
- CMake 3.25 或更高版本
- MinGW-w64
- CTest（随 CMake 提供）

## Configure, build and test

```powershell
cmake --preset mingw-debug
cmake --build --preset mingw-debug-build
ctest --test-dir build/mingw-debug --output-on-failure
```

等价的项目脚本入口：

```powershell
.\scripts\test.ps1
```

需要重新生成本地 debug 构建目录时：

```powershell
.\scripts\test.ps1 -Fresh
```

## Run

```powershell
.\bin\debug\aidb.exe
```

当前程序只验证 minimal CLI/bootstrap 和 Core Foundation 基础链路，不解析或执行 SQL。

## CI

`.github/workflows/ci.yml` 对 push 到 `main` 和 pull request 运行：

- Windows / MSVC
- Ubuntu / GCC
- macOS / AppleClang

三个 job 都执行 configure、build 和 CTest。当前 workflow 已验证 `aidb_core`、minimal CLI/bootstrap 和已注册的基础模块测试。

## 默认约束

- 默认 CTest 必须离线、可重复、无费用。
- 默认 CTest 不调用真实模型 API，也不依赖 API Key。
- preset 名称和构建目录以 `CMakePresets.json` 为准。
- 新模块只有接入 CMake、注册测试并通过要求的平台验证后，才能在状态矩阵中标记为 tested/cross-platform verified。
