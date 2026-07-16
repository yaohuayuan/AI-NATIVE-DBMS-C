# ROADMAP

本文只维护阶段、依赖、当前里程碑、完成状态和下一步。详细模块状态以 [docs/README.md](docs/README.md) 为唯一矩阵，版本推进规则见 [docs/14_versioning.md](docs/14_versioning.md)。

## 当前阶段：Core Containers

### Completed

- [x] 工程骨架、C17 构建基线和工程规范
- [x] `error`
- [x] `context`
- [x] `memory`
- [x] `arena`
- [x] platform alignment
- [x] `binary`
- [x] `vector`
- [x] `list`
- [x] `string_utils`

以上模块已有实现和测试，已接入 CMake/CTest，并由当前 GitHub Actions workflow 在 Windows/MSVC、Ubuntu/GCC、macOS/AppleClang 上验证。

### In progress

- [x] `rbt`：实现、公开契约、CMake/CTest、本地确定性/随机序列/OOM 测试已完成。
- [x] `rbt`：Windows/MSVC、Ubuntu/GCC、macOS/AppleClang CI 验证通过。
- [x] public header hygiene：修复 `memory.h` guard，并在本地完成全部 public header 的独立/重复 include 测试。
- [ ] public header hygiene：当前改动的三平台 CI 验证。

### Next

- [ ] 实现以 `rbt` 为基础的 `map`

### Planned

- [ ] `byte_buffer`
- [ ] DBMS baseline
- [ ] AI runtime 与 AI operator
- [ ] 实验、benchmark、演示和研究发布

## 后续阶段

| 阶段 | 前置依赖 | 最小交付结果 | 状态 |
|---|---|---|---|
| Core Containers | Core Foundation | `rbt`、`map`、`byte_buffer` 具备测试和构建接入 | 进行中 |
| Storage baseline | Core Containers | block/page/file、record/schema/layout 的最小可测试链路 | Planned |
| Buffer and transaction | Storage baseline | buffer manager、log/WAL、transaction/recovery 的受控实现 | Planned |
| Query system | Storage 与 transaction 基线 | tokenizer/parser、expression/predicate、scan/plan/executor | Planned |
| Metadata and index | Query system 基础接口 | catalog/metadata 和一个基础索引路径 | Planned |
| AI-native runtime | 可执行 DBMS 链路 | provider 抽象、mock-first runtime、AI operator 接口 | Planned |
| AI-aware execution | AI runtime 与基准查询 | 缓存、批处理、成本感知执行或优化主线 | Planned |
| Research release | 可重复实验链路 | benchmark、指标、实验报告、CLI/demo 和可复现发布 | Planned |

## 阶段完成标准

一个模块只有在满足以下条件后才能从 In progress/Planned 移入 Completed：

- public/internal 边界和 ownership 已记录；
- 实现已接入 CMake；
- 独立测试已注册到默认 CTest；
- 本地验证通过；
- 当前三平台 CI 通过；
- 相关文档与实际代码状态一致。

后续阶段不得因为目录、头文件或设计文档已经存在，就提前标记为 implemented 或 tested。
