# Phase Report Review Checklist

Use this checklist after each Codex phase.

## Scope

- [ ] It completed only the requested phase.
- [ ] It did not automatically begin the next phase.
- [ ] It did not make unrequested code changes.
- [ ] It did not commit, push or delete files.

## Evidence

- [ ] Important claims have file/function/command evidence.
- [ ] `VERIFIED`, `IMPLEMENTED_UNTESTED`, `DOCUMENTED`, `INFERRED`, `PROPOSED`, and `UNKNOWN` are used correctly.
- [ ] Build success is not confused with runtime correctness.
- [ ] Green tests are not confused with complete coverage.
- [ ] Legacy current working-tree state is distinguished from remote history.

## Quality

- [ ] The report contains an executive summary.
- [ ] It records commands and environment.
- [ ] It lists uncertainties and risks.
- [ ] It identifies inputs needed by the next phase.
- [ ] It avoids vague statements such as “fully implemented” without module evidence.
- [ ] It distinguishes near-term implementation from long-term research.

## Project-specific safeguards

- [ ] NL2SQL is not treated as the definition of AI-native DBMS.
- [ ] AI operators enter planner/executor through explicit boundaries.
- [ ] Terminal is distinguished from Tool Runtime.
- [ ] Ollama/llama.cpp integration is distinguished from native C inference.
- [ ] Native inference is distinguished from model fine-tuning.
- [ ] Model structural research is marked long-term.
- [ ] GUI/MCP remain clients of stable APIs rather than kernel dependencies.
- [ ] First-version scope remains small enough to implement and test.

## Decision

- [ ] APPROVE
- [ ] REQUEST TARGETED CORRECTION
- [ ] RESTART PHASE
