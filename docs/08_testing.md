# 测试规范

测试的第一目标是让项目在小步演进中保持可信。默认测试必须可重复、可离线、无费用。

## 默认规则

- 默认 CTest 不联网。
- 默认 CTest 不花钱。
- 默认 CTest 不依赖真实 API。
- 默认 CTest 不依赖 API Key。
- `tests/manual` 只放手动真实 API 测试。

## 命名

- 测试文件：`xxx_test.c`。
- 测试函数：`test_xxx`。
- CMake target 使用 `snake_case`。
- 不使用 `WordTest`、`BufferManagerTest` 这类旧风格命名。

## 目录

- `tests` 应镜像 `src`。
- 每个模块至少有 basic test。
- 真实 API、长耗时、需要本地特殊环境的测试放入 `tests/manual`，不进入默认 CTest。

## 验证命令

```powershell
.\scripts\test.ps1
```

Fresh run:

```powershell
.\scripts\test.ps1 -Fresh
```

Manual equivalent:

```powershell
cmake --preset mingw-debug
cmake --build --preset mingw-debug-build
ctest --test-dir build/mingw-debug --output-on-failure
```

## 维护原则

- 不允许为了通过测试而删除测试。
- 新功能必须配测试。
- bug fix 应优先补回归测试。
- 文档变更也要保证构建不受影响。
- AI 相关测试必须优先使用 deterministic mock。
