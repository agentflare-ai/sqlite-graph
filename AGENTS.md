# Repository Guidelines

## Project Structure & Module Organization
Source lives in `src/` (extension code plus the Cypher engine), headers are under `include/`, and vendor builds are staged in `_deps/`. Tests are split between `tests/` (Unity-based C suites plus Python drivers) and root-level smoke scripts like `test_lab.py`. Docs (`docs/*.md`) cover architecture, testing, and release flows, while `examples/` and `sqlite-utils-plugin/` supply runnable demos and the Python adapter. Place build artifacts in `build/`—anything else is treated as generated noise.

## Build, Test, and Development Commands
Use `make` from the repo root to compile the extension and accompanying tests (outputs to `build/libgraph.so`). `make test` compiles and runs the Unity harnesses, while `make test_tck` rebuilds and executes the Cypher TCK variants defined in `tests/Makefile.tck`. Run `scripts/verify_all.sh` for the full presubmit sweep (format, static analysis, tests), and `scripts/harden_memory.sh && make test SANITIZE=1` when touching allocator or concurrency paths.

## Coding Style & Naming Conventions
All C code targets GNU99 with SQLite flags (`-std=gnu99 -DSQLITE_ENABLE_LOAD_EXTENSION`). Indent with 2 spaces, wrap lines at ~80 chars, and favor `snake_case` for functions/variables plus `UPPER_CASE` for macros. Memory management must go through `sqlite3_malloc/sqlite3_free`, and comments use SQLite-style block notes. Filenames follow `graph-<feature>.c` or `cypher/<component>.c`; tests mirror features (`tests/tck_test_clauses_match.c`), so extend those patterns for new modules.

## Testing Guidelines
Unity test cases live beside the code they exercise; name individual tests `test_<behavior>`. When adding new TCK coverage, update `tests/tck_test_*` and register the file in `tests/Makefile` or `.tck`. `scripts/coverage.sh` and `scripts/perf_regression.sh` exist for optional quality gates but should be run before large rewrites. Keep coverage meaningful by asserting both success paths and SQLite error codes, and document any expected failures in `docs/TESTING_PLAN.md`.

## Commit & Pull Request Guidelines
Recent history mixes descriptive sentences and conventional prefixes (`chore: update docs`, “Fix release infrastructure … (#4)”). Match that: short imperative subjects, optional scope, and reference issues/PRs via `(#123)` when applicable. For PRs, include: summary of behavior change, test evidence (`make test`, targeted binaries), regressions avoided, and any screenshots/logs for tooling updates. Link the relevant roadmap item or issue, call out breaking changes explicitly, and request reviewers familiar with the touched module.

## Security & Configuration Tips
Review `SECURITY.md` before shipping anything exposed publicly. Run `scripts/security_audit.sh` whenever modifying parser or JSON glue, and prefer hardened builds (`make SANITIZE=1`) when debugging potential memory errors. Never commit secrets or generated certs; reference configuration examples via `examples/` or `docs/ARCHITECTURE.md` instead.
