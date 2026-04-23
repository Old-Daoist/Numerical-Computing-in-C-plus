# Plan & Report Authoring Rules — MatrixV1 Numerical Library

> **Purpose:** This file defines the conventions for all plan and report documents
> stored under `content/`. It is designed to be referenced in LLM/agent prompts so
> that any AI assistant producing plans or reports follows a consistent format.
>
> **Usage in prompts:** Include the directive
> `Follow the rules in content/rules.md when writing plans or reports for this project.`

---

## 1. Directory layout

```
content/
├── rules.md                              # THIS FILE — authoring conventions
├── current_state_report.md              # Latest project state snapshot (living doc)
├── commands.txt                         # Quick-reference CLI commands for build & test
├── plans/                               # One file per plan
│   └── plan009.md                       # e.g., plan009.md
└── report/                              # One file per implementation report
    └── report009.md                     # e.g., report009.md
```

Outside `content/` (project root of `NumericalLibrary-ver2/`):

```
NumericalLibrary-ver2/
├── content/                             # All planning and tracking docs (above)
├── library/
│   ├── MatrixV1/                        # C++ source (CMake project)
│   │   ├── include/                     # Headers (.hpp)
│   │   ├── src/                         # Implementations (.cpp)
│   │   ├── tests/                       # Catch2 test suite
│   │   ├── CMakeLists.txt
│   │   └── main.cpp
│   └── MatrixV1_Python/                 # Python port (pip package)
│       ├── matrixv1/                    # Package source
│       ├── tests/                       # pytest suite
│       └── pyproject.toml
├── specs.md                             # Master project specification
├── reconciliation-001.md                # First full cross-audit report
├── run_cpp.bat                          # Windows build/run shortcut
└── run_python.bat                       # Windows Python run shortcut
```

- **`content/plans/`** stores *forward-looking* documents (what will be done).
- **`content/report/`** stores *backward-looking* documents (what was done, results, learnings).
- A report's number **must match** the plan it implements (e.g., `plan009.md` → `report009.md`).
- `current_state_report.md` is a living document — update it after every plan is completed.

---

## 2. Naming conventions

| Item | Pattern | Example |
|------|---------|---------|
| Plan file | `planNNN.md` | `plan009.md` |
| Report file | `reportNNN.md` | `report009.md` |
| NNN | Zero-padded 3-digit serial | `009`, `010`, … |
| Spec file | `specs.md` | (root level, one file) |
| Reconciliation | `reconciliation-NNN.md` | `reconciliation-001.md` |

---

## 3. Plan document template

Every plan **must** contain these sections in order:

```markdown
# PLAN-NNN: <Title>

**Created:** YYYY-MM-DD
**Status:** Draft | Approved | In Progress | Completed | Abandoned
**Addresses:** <one-line description of the gap/need this plan targets>

---

## 1. Context & Motivation
Why this plan exists. Link to specs.md feature and current_state_report.md
sections that surfaced the need.

## 2. Goals
Bulleted list of measurable outcomes (G1, G2, …).
Each goal must be verifiable — pass/fail, metric threshold, command output.

## 3. Non-Goals
What is explicitly out of scope. Prevents scope creep.

## 4. Approach
Detailed description of the technical approach.
Include algorithm design, interface decisions, trade-offs.
Use subsections (4.1, 4.2, …) for distinct concerns.
Include short code snippets (< 20 lines) where they clarify intent.

## 5. Task Breakdown
| # | Task | Est. | Depends on |
|---|------|------|------------|
| 1 | … | 30 min | — |
| 2 | … | 2 hr  | 1 |

## 6. Risks & Mitigations
| Risk | Likelihood | Impact | Mitigation |
|------|-----------|--------|------------|
| … | Low/Med/High | Low/Med/High | … |

## 7. Success Criteria
Checklist of verifiable conditions.
Reference goals from §2 explicitly (G1 verified: …).

## 8. References
- `specs.md` — Feature N
- `content/current_state_report.md` — relevant section
- Any external algorithm references (textbooks, links)
```

---

## 4. Report document template

Every report **must** contain these sections in order:

```markdown
# REPORT-NNN: <Title>

**Plan:** PLAN-NNN
**Completed:** YYYY-MM-DD
**Author:** <human name or AI agent identifier, e.g., "Claude Sonnet 4.6">

---

## 1. Summary
One paragraph: what was implemented, key outcomes, test counts.

## 2. Goals vs. Actuals
| Goal (from plan) | Outcome | Evidence |
|------------------|---------|----------|
| **G1:** … | ✅ Met / ⚠️ Partial / ❌ Not met | command output or description |

## 3. Changes Made
List every file created, modified, or deleted.
Group by logical unit. Include LOC or scope descriptor.

### 3.1 <Logical unit>
- `path/to/file.cpp` — description of change

## 4. Testing & Validation
Paste actual test output. Show ctest / pytest run.
Include manual verification steps where unit tests are insufficient.

## 5. Known Issues & Follow-ups
Anything left unresolved. Reference follow-up plan number if one exists.

## 6. Metrics
| Metric | Before | After |
|--------|--------|-------|
| Test count | … | … |

## 7. Lessons Learned
What went well, what was harder than expected, what to do differently.
```

---

## 5. Writing style rules

These apply to **both** plans and reports:

1. **Be concrete, not vague.** Write `"Add LeastSquares::fitLine() to src/numerical/LeastSquares.cpp"` — not `"Add least squares support."` 
2. **Reference files by relative path** from the library root (e.g., `library/MatrixV1/src/numerical/LeastSquares.cpp`).
3. **Use present tense** in plans (`"We add…"`), **past tense** in reports (`"We added…"`).
4. **Include code snippets** when they clarify algorithmic intent; keep them under 20 lines with fenced language tags.
5. **No orphan acronyms.** First use must expand: `"Least Squares (LS)"`.
6. **Dates** use ISO 8601: `YYYY-MM-DD`.
7. **Status field** must be kept current:
   - Work begins → change plan status to `In Progress`
   - Report written → change plan status to `Completed`
8. **Cross-reference** between plan and report using the NNN serial.
9. **Tables over prose** for structured data (task lists, file inventories, metrics).
10. **Test-first workflow.** Implementation tasks follow this sequence:
    1. Write tests — define expected behaviour before production code.
    2. Implement — minimum code to make tests pass.
    3. Run & verify — the task is not done until tests are green.

---

## 6. Rules for AI agents during implementation

### 6.1 Think before coding

1. **State assumptions explicitly.** If uncertain about intent, ask before implementing.
2. **Surface ambiguity.** Present multiple interpretations — don't pick silently.
3. **Propose simpler alternatives.** If a simpler approach exists, say so.
4. **Stop on confusion.** Name what's confusing and ask. Don't guess.

### 6.2 Minimum viable changes

1. **No speculative features.** Don't add capabilities beyond what was requested.
2. **No single-use abstractions.** Don't create helpers used only once.
3. **No impossible-case handling.** Validate only at real boundaries (user input, file I/O).
4. **Question your own output.** If 200 lines could be 50, rewrite before submitting.

### 6.3 Surgical edits

1. **Touch only what you must.** Every changed line must trace to the request.
2. **Don't "improve" adjacent code.** No reformatting or refactoring of untouched code.
3. **Match existing style.** Follow the conventions already in the file.
4. **Clean up only your own orphans.** Don't remove pre-existing dead code unless asked.

### 6.4 Dual-library rule (C++ + Python)

This project maintains two parallel implementations:

- `library/MatrixV1/` — canonical C++ implementation
- `library/MatrixV1_Python/matrixv1/` — Python mirror (same algorithms, same module structure)

When a plan adds a new algorithm or modifies an existing one:

1. **Implement in C++ first.** Get tests passing in Catch2.
2. **Mirror in Python.** Port the same logic to the corresponding Python module under `matrixv1/`.
3. **Write Python tests.** Add equivalent pytest cases covering the same scenarios.
4. **Both test suites must be green** before the plan is marked `Completed`.

The module mapping is:

| C++ path | Python path |
|----------|-------------|
| `src/numerical/LeastSquares.cpp` | `matrixv1/numerical/least_squares.py` |
| `include/numerical/LeastSquares.hpp` | (declaration inline in `.py`) |
| `tests/test_matrix.cpp` (numerical section) | `tests/test_numerical.py` |

### 6.5 Goal-driven execution

1. **Define success criteria before starting.** Transform vague tasks into verifiable goals.
2. **State a brief plan for multi-step work** (step → verify pairs).
3. **Loop until verified.** Don't mark a task done until its success criteria are confirmed.

---

## 7. Lifecycle

```
Draft  →  Approved  →  In Progress  →  Completed  (report written)
                              ↘ Abandoned  (reason noted in §1)
```

1. Create `content/plans/planNNN.md` with `**Status:** Draft`.
2. Review and change to `Approved` before work begins.
3. Begin work → change to `In Progress`.
4. Finish work → write `content/report/reportNNN.md`, change plan status to `Completed`.
5. If abandoned → change status to `Abandoned`, note reason in plan §1.
6. After completing any plan → update `content/current_state_report.md`.

---

## 8. Prompt integration

When instructing an AI agent to create a plan or report, include:

```
Follow the rules defined in content/rules.md for document structure,
naming, and style. The plan directory is content/plans/ and report
directory is content/report/, both relative to NumericalLibrary-ver2/.
Use the next available NNN serial number.
```

---

## 9. Project-specific conventions

### C++ build commands
```bash
cd library/MatrixV1
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
```

### Python test commands
```bash
cd library/MatrixV1_Python
python -m pytest tests/ -v
```

### File layout rule for new numerical methods
New algorithms follow this pattern:
- `include/numerical/ClassName.hpp` — class declaration + doc comment
- `src/numerical/ClassName.cpp` — implementation
- `matrixv1/numerical/class_name.py` — Python mirror
- Tests added to `tests/test_matrix.cpp` (C++) and `tests/test_numerical.py` (Python)
- Register `.cpp` file in `CMakeLists.txt` under `matrix_lib` sources