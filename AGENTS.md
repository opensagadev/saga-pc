# AGENTS.md

Notes for AI agents contributing to saga-pc. Read [CONTRIBUTING.md](CONTRIBUTING.md)
first: setup, workflow, house rules and matching techniques are there and are not
repeated here. [.agents/skills/saga-pc-decomp](.agents/skills/saga-pc-decomp/SKILL.md)
has a short working summary.

## Goal

A codebase that resembles the original source and compiles to identical code.
Find out why a function does not match and what the original most likely
looked like; do not game the compiler or the metrics.

## Rules

- Never push to `main`. Work on a branch and land it through a pull request;
  other people work on this repository too. Do not rewrite others' history.
- No `goto`, branch hints, dummy locals or other constructs the original
  plausibly did not have, just to coax a match. Unavoidable unnatural source is
  marked with a `// MATCH-EXCEPTION:` comment that says why.
- Unknown names are `fun_<8 hex digit retail VA>` and `dat_<address>`.
- Always cap build parallelism (`make -j"$(nproc)"` or lower); never a bare
  `make -j`.
- No third-party precompiled files (toolchains, SDK libraries, game data) in
  the repository; scripts fetch them from their original sources.

## Commits

Add yourself as co-author with one trailer at the end of every commit message,
naming the model that did the work, as the existing history does:

```
Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>
```

Keep the human author as the commit author and any other co-author trailers.
