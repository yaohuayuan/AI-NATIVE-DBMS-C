# Staged Codex Execution Protocol

## 1. Initial setup

Copy this package into the repository root so that:

```text
D:\code\AI-NATIVE-DBMS-C\AGENTS.md
D:\code\AI-NATIVE-DBMS-C\docs\planning\...
```

Do not place `AGENTS.md` inside `.agents`.

The existing `.agents` directory and the root `AGENTS.md` serve different purposes.

## 2. One phase, one new conversation

For every phase:

1. Open a new Codex task/conversation in the `AI-NATIVE-DBMS-C` project.
2. Open the matching file under `docs/planning/prompts/`.
3. Copy its entire contents into the new conversation.
4. Let Codex execute only that phase.
5. Confirm the requested report file was created.
6. Review the report before starting the next phase.
7. If the report is weak, continue only with a targeted correction prompt or restart that phase.
8. Do not ask the same conversation to continue automatically into the next phase.

## 3. Why reports are mandatory

Conversation context may be compacted.

A report provides durable, reviewable project memory.

Later phases should read:

- approved reports;
- targeted source files;
- current repository facts.

They should not depend on remembering a massive previous conversation.

## 4. Review gates

After each report, check:

- Did Codex stay inside scope?
- Are claims labeled VERIFIED/DOCUMENTED/PROPOSED?
- Are commands and file paths recorded?
- Did it distinguish build success from test coverage?
- Did it avoid modifying source files?
- Did it list uncertainties?
- Did it stop instead of continuing into the next phase?

Use `03_review_checklist.md`.

## 5. Status updates

After approving a report, update `01_task_index.md`:

- completed phase -> `APPROVED`;
- next phase -> `NOT_STARTED`;
- dependent phases remain `BLOCKED`.

This update may be done manually or in a tiny separate Codex task.

Do not let a large research task casually rewrite the whole task index.

## 6. Parallel work

Allowed parallel planning after dependencies:

- P1 Migration and P2 Related Work may run in parallel after P0.
- P4 Terminal architecture and P5 Native inference architecture may run in parallel after P3.

Implementation should usually keep one active critical-path batch at a time.

## 7. Failed or stuck task

If Codex stalls or context is compacted:

1. Do not resend the whole master specification.
2. Start a new conversation.
3. Use the same phase prompt.
4. Tell it to reuse any already-created report or build evidence, but verify freshness.
5. Ask it to finish only the missing sections.
6. Preserve partial work; do not delete user files.

## 8. Planning versus implementation

P0–P9 are primarily planning/report tasks.

Implementation begins only after:

- final roadmap approval;
- implementation backlog approval;
- selection of one batch.

Each implementation conversation must state:

- exact batch ID;
- allowed files;
- required tests;
- Definition of Done;
- whether commits are allowed;
- explicit non-goals.
