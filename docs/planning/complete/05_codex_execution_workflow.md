# Codex 实施工作流

## 1. 完整路线全部保留，但每次只实现一个 Batch

```text
完整路线文件
   -> 选择一个 Batch
   -> 新建 Codex 对话
   -> 实现、构建、测试、报告
   -> GPT 审核
   -> 下一个 Batch
```

不要把整个 47 阶段路线一次交给 Codex 执行。

## 2. Codex 每次读取

```text
AGENTS.md
docs/planning/complete/00_scope_and_final_goal.md
docs/planning/complete/02_complete_development_roadmap.md
docs/planning/complete/03_complete_capability_matrix.md
当前 Batch 提示词
```

任务中明确写：完整路线不可删除，但本次不得自动实现后续阶段。

## 3. 第一批 Batch

```text
B001 Public Header Hygiene
B002 RBT Contract
B003 RBT Implementation
B004 RBT Tests
B005 Map Contract / Implementation
B006 Byte Buffer Contract
B007 Byte Buffer Implementation / Tests
B008 Storage Format v1 Design
B009 BlockId / Page
B010 FileManager
```

## 4. Batch 提示词模板

```text
读取：
- AGENTS.md
- docs/planning/complete/00_scope_and_final_goal.md
- docs/planning/complete/02_complete_development_roadmap.md
- docs/planning/complete/03_complete_capability_matrix.md

本次只执行：
<BATCH_ID> — <TITLE>

目标：
<GOAL>

允许修改：
<PATHS>

旧项目只读参考：
<LEGACY_PATHS>

必须实现：
<DELIVERABLES>

必须测试：
<TESTS>

Definition of Done：
<DOD>

明确非目标：
<NON_GOALS>

完成后：
1. 列出修改文件；
2. 说明 API、ownership 和 error 决策；
3. 运行 configure/build/CTest；
4. 报告测试；
5. 说明未完成内容；
6. 停止，不得进入下一个 Batch。

不要 commit 或 push。
```

## 5. 分工

- GPT：架构、研究、Batch 切分、API/diff 审核和路线维护。
- Codex：读取本地代码、完成一个 Batch、构建测试、生成实施报告。
- 用户：决定优先级和提交/发布，提供未跟踪本地文件。

## 6. 后期 Track

项目变大后可分为：

```text
Track DBMS
Track AI Runtime / Operators
Track System Platform
Track Native Inference
Track Model Research
Track Benchmark / GUI
```

每条 Track 仍一次只实现一个明确 Batch，并通过稳定 API 集成。
