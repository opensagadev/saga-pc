---
name: saga-pc-decomp
description: Work on the saga-pc matching decompilation (MSVC 2005 PC build of LEGO Star Wars: The Complete Saga).
---

# saga-pc decompilation

Read [CONTRIBUTING.md](../../../CONTRIBUTING.md) first; it covers setup, the
workflow, the VC8 source-shape lessons and the automated search.

- Measure before and after every change: `make -j"$(nproc)" diff FUNC=<name>`
  for one function, `make -j"$(nproc)" match` for the report.
- Try `python3 tools/permute.py <name>` before hand-tuning a function; spend
  model effort only on what the search cannot fix.
- Port Android bodies, then apply PC evidence: layouts, constants, ABI and
  translation-unit placement come from the PC executable.
- Keep notes short. Git history and `matching.json` are the record; do not add
  per-function prose journals or checkpoint documents.
