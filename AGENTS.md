# AI-NATIVE-DBMS-C Codex Working Rules

## 1. Project identity

AI-NATIVE-DBMS-C (`aidb`) is a C17 AI-native DBMS research prototype.

The project must first form a real, testable DBMS execution chain, and then introduce model calls as plannable, explainable, cacheable and measurable database operators. It must not degrade into a thin NL2SQL wrapper around an unrelated database.

Read these files before starting a planning task:

- `docs/planning/00_master_spec.md`
- `docs/planning/01_task_index.md`
- `docs/planning/02_execution_protocol.md`
- the exact phase prompt named by the user

The master specification defines long-term scope. It is not a request to implement or analyze every section in one task.

## 2. Evidence labels

Every audit, plan and technical report must distinguish:

- `VERIFIED`: confirmed by source code plus successful build, test or execution evidence.
- `IMPLEMENTED_UNTESTED`: implementation exists and builds or is readable, but lacks sufficient formal tests.
- `DOCUMENTED`: described in documentation only.
- `INFERRED`: reasonably inferred from code or behavior, but not directly verified.
- `PROPOSED`: a recommendation or future design.
- `UNKNOWN`: insufficient evidence.

Never infer implementation merely from:

- directory names;
- header names;
- comments;
- TODO entries;
- README claims;
- design documents;
- empty or placeholder files.

## 3. Repository boundaries

Current repository:

- `D:\code\AI-NATIVE-DBMS-C`

Legacy read-only reference repository:

- `D:\code\DBMS\NewDBMS\DBMS_C`

Rules:

- Treat the legacy DBMS as read-only unless the user explicitly changes this rule.
- Do not delete, clean, reset, move or overwrite user files.
- Do not remove untracked files.
- Do not commit, tag, push, create branches or open pull requests unless explicitly requested.
- Do not write build artifacts into the legacy repository when an isolated build directory can be used.
- Do not modify source code during planning phases unless the current phase explicitly allows changes.

## 4. Scope discipline

Execute only the phase explicitly requested by the user.

Do not continue automatically into later phases.

For example:

- a repository audit must not expand into a full research survey;
- a migration matrix must not begin porting code;
- an architecture task must not rewrite the ROADMAP unless requested;
- a terminal design task must not start implementing the terminal;
- a native inference design task must not copy llama.cpp code.

If evidence from another phase is required, perform only the minimum targeted inspection needed and state why.

## 5. Engineering completion standard

A module is not complete until applicable requirements are satisfied:

- public/internal/platform boundaries are defined;
- ownership and lifetime semantics are documented;
- errors are represented through stable domain semantics;
- implementation is connected to CMake;
- formal tests are registered in default CTest;
- local build and tests pass;
- required cross-platform CI passes;
- documentation matches actual code state;
- no API keys, runtime data or generated artifacts are committed accidentally.

## 6. Build and test behavior

Prefer project-documented build commands.

For the current repository, start from:

```powershell
cmake --preset mingw-debug
cmake --build --preset mingw-debug-build
ctest --test-dir build/mingw-debug --output-on-failure
```

When auditing the legacy project:

- keep its source tree read-only;
- use isolated build and runtime directories where possible;
- distinguish default-build success from strict-warning cleanliness;
- distinguish registered tests from unregistered test sources;
- distinguish green tests from complete semantic coverage.

## 7. Research behavior

When a phase requires online research:

- prefer original papers, official repositories and author documentation;
- record title, authors, year, venue or arXiv identifier, and source;
- distinguish paper claims from capabilities verified in released code;
- do not fabricate unavailable post-cutoff papers;
- do not force every researched feature into this project.

## 8. Report behavior

Write the requested phase report to the exact path specified by the phase prompt.

A report must include:

- scope;
- evidence and commands used;
- findings;
- uncertainties;
- risks;
- decisions or recommendations;
- inputs required by the next phase.

Do not silently update unrelated reports.

At the end of a task, stop after:

1. writing the requested report;
2. summarizing what was completed;
3. listing unresolved questions;
4. naming the next phase, without starting it.
