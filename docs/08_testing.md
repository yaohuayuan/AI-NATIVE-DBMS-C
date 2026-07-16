# 测试规范

测试的目标是让项目在小步演进中保持可信。默认测试必须可重复、离线、无费用。

## 当前基线

当前 CMake/CTest 已注册：

- `basic_core_test`
- `error_test`
- `context_test`
- `memory_test`
- `arena_test`
- `binary_test`
- `vector_test`
- `list_test`
- `string_utils_test`
- `rbt_test`
- `rbt_oom_test`

当前 GitHub Actions workflow 在 Windows/MSVC、Ubuntu/GCC 和 macOS/AppleClang 上执行 configure、build 和上述 CTest。`rbt_test` 覆盖契约、插入/查找/重复、删除形态、排序压力和固定种子随机序列；`rbt_oom_test` 使用独立测试分配器确定性覆盖初始化与插入分配失败。它们已在本地 MinGW Debug 通过，当前改动的远端三平台结果仍为 `UNKNOWN`。

## 默认规则

- 默认 CTest 不联网、不产生模型调用费用、不依赖 API Key。
- 真实 API、长耗时或需要本地特殊环境的测试放入 `tests/manual`，不进入默认 CTest。
- AI 相关默认测试优先使用 deterministic mock。
- 新模块必须有独立测试并注册到 CTest，不能只依赖间接覆盖。

## 命名和目录

- 测试文件：`xxx_test.c`。
- 测试函数：`test_xxx`。
- CMake target：`snake_case`。
- 测试目录按职责镜像 `src`，不使用旧项目的 PascalCase 测试命名。

## 验证命令

```powershell
.\scripts\test.ps1
```

Fresh run：

```powershell
.\scripts\test.ps1 -Fresh
```

手动等价命令：

```powershell
cmake --preset mingw-debug
cmake --build --preset mingw-debug-build
ctest --test-dir build/mingw-debug --output-on-failure
```

## 完成判定

只有代码实现、CMake source、独立测试、CTest 注册和要求的平台验证全部满足，模块才能标记为 `tested`；三平台 workflow 成功后才能标记为 `cross-platform verified`。

不允许为了通过测试而删除测试。新功能应覆盖正常、边界和失败路径；bug fix 应优先补回归测试。
