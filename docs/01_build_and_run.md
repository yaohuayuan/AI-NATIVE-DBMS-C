# 构建与运行

当前项目主要在 Windows + MinGW + CMake Presets 环境下开发。后续如果增加其他平台，应优先保持 preset 清晰、构建路径隔离、默认测试可重复。

## 环境要求

- Windows PowerShell。
- CMake。
- MinGW-w64。
- CTest。

## Configure

```powershell
cmake --preset mingw-debug
```

## Build

```powershell
cmake --build --preset mingw-debug-build
```

## Run

```powershell
.\bin\debug\aidb.exe
```

## Test

```powershell
ctest --test-dir build/mingw-debug --output-on-failure
```

## 默认约束

- 默认 CTest 不联网。
- 默认 CTest 不调用真实 API。
- 默认 CTest 不依赖 API Key。
- 文档变更后也要确认 configure/build/ctest 不受影响。
- 如果 preset 名称变化，应以 `CMakePresets.json` 为准，而不是随手改构建逻辑。
