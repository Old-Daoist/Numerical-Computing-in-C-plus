# Reconciliation Report — specs.md ↔ content/current_state_report.md ↔ Actual Implementation

**Report ID:** RECON-001
**Date:** 2026-04-23
**Scope:** Full cross-verification of `specs.md`, `content/current_state_report.md`, and the live codebase (`library/MatrixV1/`, `library/MatrixV1_Python/`) after completion of PLAN-001 through PLAN-008.
**Purpose:** Identify all gaps between documentation and reality, flag stale metadata, and produce a prioritised action plan heading into PLAN-009 and beyond.

---

## Executive Summary

The project is in strong shape. All 8 planned features from `specs.md` have been implemented, reported, and tested — **185 total tests pass (81 C++, 104 Python), zero failures.** The biggest discrepancies are not in the code but in the documentation metadata: all 8 plan files still carry `Draft` or `In Progress` status despite their corresponding reports being complete. Secondary gaps include a missing Least Squares feature (not in the original spec but a natural next step), a Catch2 placeholder instead of the real library, and the absence of a CI pipeline. No algorithmic correctness issues were found.

### Documents vs Reality — Key Discrepancies Found

| Area | specs.md says | current_state_report.md says | Actual code |
|------|---------------|------------------------------|-------------|
| Plan statuses | N/A (not tracked in specs) | All 8 plans "completed" implicitly | plan files say `Draft`/`In Progress` |
| Catch2 | "Catch2 v3 (header-only)" | "Placeholder — swap when network available" | Hand-written shim in `tests/catch2/` |
| CI pipeline | Not mentioned | "None — manual build only" | No `.github/workflows/` directory |
| Least Squares | Not in spec | Not in library | Neither C++ nor Python implement it |
| `specs.md` stray `]` | — | Noted as minor issue | Line 12 of `specs.md` has `]\n` |
| Python port | Not originally in spec (added as PLAN-008) | Fully implemented | `MatrixV1_Python/` present and working |
| `SolveLinearEquation.hpp` | Not explicitly described | Present in headers | Wrapper around Gauss Elimination; usage in `main.cpp` only |

---

## Priority 1 — 🔴 CRITICAL (Do Immediately)

### 1.1 Update Plan Status Fields to `Completed`

**Gap:** All 8 plan files in `content/plans/` carry stale status metadata. Per `content/rules.md` §7, a plan's status must be changed to `Completed` when its report is written. Plans 001–008 all have corresponding reports but all read `Draft` or `In Progress`.

**Actions:**
1. Open `content/plans/plan001.md` through `plan008.md`.
2. Change `**Status:** Draft` → `**Status:** Completed` for plans 001–005, 007, 008.
3. Change `**Status:** In Progress` → `**Status:** Completed` for plan006.

**Files to modify:**
- `content/plans/plan001.md` through `content/plans/plan008.md` (8 files, 1-line change each)

**Verification:** `grep "Status:" content/plans/plan00*.md` returns `Completed` for all 8.

---

## Priority 2 — 🟠 HIGH (Architectural Alignment)

### 2.1 Replace Catch2 Placeholder with Real Catch2 v3

**Gap:** `specs.md` specifies "Catch2 v3 (header-only)" as the test framework. The actual files in `tests/catch2/` are a hand-written shim that implements `TEST_CASE`, `REQUIRE`, `CHECK`, and related macros but is not the real Catch2 library. All 81 tests pass, but the shim does not support advanced Catch2 features (BDD-style `GIVEN/WHEN/THEN`, reporters, tags, benchmarks).

**Options:**

**Option A: Replace with real Catch2 (recommended when network is available)**
```bash
curl -sL https://github.com/catchorg/Catch2/releases/download/v3.7.1/catch_amalgamated.hpp \
     -o library/MatrixV1/tests/catch2/catch_amalgamated.hpp
curl -sL https://github.com/catchorg/Catch2/releases/download/v3.7.1/catch_amalgamated.cpp \
     -o library/MatrixV1/tests/catch2/catch_amalgamated.cpp
cd library/MatrixV1 && cmake --build build
ctest --test-dir build --output-on-failure
```

**Option B: Document the shim and accept it until CI is added**
Update `specs.md` Tech Stack table to read "Catch2 v3 (placeholder shim — replace when internet access available)" and note it as a known issue.

**Decision:** Option B now; Option A when internet access is available. The shim does not block any planned work.

**Files to modify (Option B):**
- `specs.md` — update Catch2 row in Tech Stack table
- `content/current_state_report.md` §5 — already noted

---

### 2.2 Least Squares Not in Original Spec

**Gap:** `specs.md` defines features 0–7 (plus the Python port from PLAN-008). It does not mention Least Squares fitting. This is a natural extension of the numerical methods module and the target of PLAN-009, but the spec does not cover it.

**Actions:**
1. Add Feature 9 to `specs.md` Feature Index:
   ```
   | Feature 9 | Least Squares fitting — straight line (y = a + bx) and parabola (y = a + bx + cx²) | PLAN-009 |
   ```
2. Update `specs.md` Success Criteria section to include:
   ```
   - [ ] LeastSquares::fitLine() and fitParabola() pass ≥ 10 automated tests each
   - [ ] Python equivalent functions pass equivalent pytest cases
   ```
3. Update `specs.md` version and Last Updated date.

**Files to modify:**
- `specs.md` — add Feature 9, update Success Criteria, bump Last Updated

---

### 2.3 No CI Pipeline

**Gap:** The project has no automated build or test pipeline. Every verification is manual. This is a risk as the library grows — a regression can go undetected if the developer forgets to run tests.

**Options:**

**Option A: Add GitHub Actions (recommended)**
Add `.github/workflows/ci.yml` that runs:
```yaml
- cmake -B build && cmake --build build
- ctest --test-dir build --output-on-failure
- pip install -e library/MatrixV1_Python
- pytest library/MatrixV1_Python/tests/
```

**Option B: Document the gap and defer**
Note CI as a standing follow-up. Acceptable while the project is solo-developer.

**Decision:** This is a candidate for a standalone PLAN-010 after PLAN-009 is complete. Not blocking.

---

## Priority 3 — 🟡 MEDIUM (Functional Gaps)

### 3.1 `SolveLinearEquation` Wrapper Is Undocumented

**Gap:** `library/MatrixV1/include/solvers/SolveLinearEquation.hpp` exists and is used by `main.cpp`, but it is not described in `specs.md` and has no mention in any plan or report. It appears to be a thin dispatch wrapper around `GaussElimination`.

**Actions:**
1. Read `SolveLinearEquation.hpp` and `src/` counterpart to confirm its role.
2. Add a one-line description to `specs.md` Tech Stack table under Solvers.
3. If it is truly just a wrapper, note that in the tech stack rather than creating a new plan.

**Files:** `specs.md` — Solvers table

---

### 3.2 Python `main.py` Menu Strings Are Not Synced with C++ `main.cpp`

**Gap:** PLAN-007 rewrote `main.cpp` with a boxed 4-section menu and 20 options. PLAN-008 created `main.py` with equivalent menu text. Any future change to menu prompts (e.g., adding PLAN-009 options) must be made in two files independently.

**Actions:**
1. When adding PLAN-009 menu options, update **both** `main.cpp` and `main.py`.
2. Consider a shared constants approach in a future cleanup plan (low priority).

---

### 3.3 `specs.md` Stray `]` on Line 12

**Gap:** Line 12 of `specs.md` reads `]` as a standalone character before `## Current Focus`. This is a formatting artifact — likely a leftover from an incomplete markdown link.

**Actions:**
1. Open `specs.md`.
2. Delete the stray `]` on line 12.

**Files:** `specs.md` — line 12

---

## Priority 4 — 🟢 LOW (Documentation & Cleanup)

### 4.1 File Naming Does Not Match AGENT_MD Convention

**Gap:** The AGENT_MD standard (from which this project's workflow was derived) uses `PLAN-NNN_<slug>.md` and `REPORT-NNN_<slug>.md` naming. The actual files use `plan001.md` / `report001.md` — no uppercase, no slug. This is not a correctness problem but makes cross-referencing with AGENT_MD tooling harder.

**Actions:**
1. Accept the current naming as the project standard (already codified in `content/rules.md`).
2. If the project is ever shared publicly or integrated with AGENT_MD tooling, rename files at that point.

No immediate action required.

---

### 4.2 `content/report/` Folder Name Is Singular

**Gap:** AGENT_MD uses `reports/` (plural). This project uses `report/` (singular). Minor inconsistency.

**Actions:**
1. Accept `report/` as the project convention (already documented in `content/rules.md`).
2. Do not rename — it would break any script or shortcut referencing the existing path.

No immediate action required.

---

### 4.3 `commands.txt` Has No Equivalent for C++ Build

**Gap:** `content/commands.txt` covers only Python commands (pip, pytest, git). It has no section for C++ CMake build commands.

**Actions:**
1. Add a C++ section to `content/commands.txt`:
   ```
   # C++ BUILD & TEST
   cd library/MatrixV1
   cmake -B build -DCMAKE_BUILD_TYPE=Release
   cmake --build build
   ctest --test-dir build --output-on-failure -V
   ```

**Files:** `content/commands.txt`

---

## Action Plan Summary

| # | Priority | Action | Est. Effort |
|---|----------|--------|-------------|
| 1.1 | 🔴 Critical | Update 8 plan files: `Draft` → `Completed` | 5 min |
| 2.1 | 🟠 High | Document Catch2 shim in specs.md (Option B now; Option A when network available) | 10 min |
| 2.2 | 🟠 High | Add Feature 9 (Least Squares) to specs.md; this unblocks PLAN-009 | 15 min |
| 2.3 | 🟠 High | Plan CI pipeline as PLAN-010 after PLAN-009 is complete | — |
| 3.1 | 🟡 Medium | Document `SolveLinearEquation` wrapper in specs.md | 10 min |
| 3.2 | 🟡 Medium | Keep C++ and Python menus in sync when adding PLAN-009 options | — |
| 3.3 | 🟡 Medium | Delete stray `]` on line 12 of `specs.md` | 2 min |
| 4.1 | 🟢 Low | Accept current file naming as project standard (already in rules.md) | Done |
| 4.2 | 🟢 Low | Accept `report/` singular naming | Done |
| 4.3 | 🟢 Low | Add C++ build commands to `content/commands.txt` | 5 min |