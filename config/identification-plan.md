# Current identification work plan

Recover PC functions and data together from verified Android source/call/data
anchors, native callers, constants, RTTI and bounded reference graphs.
Identification supplies implementation work; it does not require implementing a
function first and does not establish reconstruction coverage.

The accepted map is [symbols.csv](symbols.csv). Apply the identity, ABI and
complete-extent rules in [identification.md](identification.md) and
[symbols.md](symbols.md). Tentative candidates, competing identities and rejections
remain research evidence until reviewed. Never insert inferred addresses or sizes
merely to improve a metric.

Use the maintained commands and input schemas in
[the operator runbook](identification-tools.md). The guarded identification entry
point freezes inputs, serializes jobs and enforces its memory budget. Raw exports
and reviewed decisions remain authoritative; SQLite indices are derived caches.
Incomplete runs are not accepted results. Optional BSim/BinDiff evidence must
retain its input provenance and evaluation limits.

Review a bounded family, record its callers/types/full extent and unresolved
contracts, then hand off a reproducible source/header/map/data proposal. Reuse
retained graphs and journal decisions; refresh changed inputs rather than
rebuilding all exports. Preserve human edits in
[identification-decisions.jsonl](identification-decisions.jsonl).

Report known code/data extent coverage and unknown-size symbol counts separately.
Unknown extents add no covered bytes. See [execution policy](transfer-execution.md)
for section denominators and the distinction between identification and matching.

The full [2026-09-30 pipeline design](../docs/history/identification-plan-2026-09-30.md)
is retained as historical rationale; current tool behavior and checked commands
come from the maintained guides and tests.
