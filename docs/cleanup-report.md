# Cleanup report

State of the readability cleanup. The work is on the branch `cleanup/quick-run`, which is not yet merged to main.

## How the cleanup ran

1. **Proven passes (on main, up to 5717c76e).** Every change was proven object-identical before it landed. Main is exact and green at 5717c76e.
2. **Quick run (on `cleanup/quick-run`, from 5717c76e).** Four workers, one per module, fixed the open list fast without proving each batch. The gate checked format, both builds, tool tests, clang-tidy, the scan checks and SSE alignment, and recorded the match report without blocking. Budgets that rose were raised with the line "quick run, settled in the refix" in config/budget-raises.txt.
3. **Refix (open).** Diff the match against the baseline, bring the dropped functions back, lower the budgets, delete the quick-run raise lines, then merge the branch to main.

Baseline at 5717c76e, as recorded by the proof: 13,516 functions and 17,063 data symbols exact. By matching.json: 13,531 functions and 17,123 data symbols.

## Counts, main 5717c76e → branch

Measured from the budget files, which follow the scans after each batch.

| Item | main | branch |
|---|---:|---:|
| `(void)name;` statements | 1,249 | 3 (volatile reads, kept) |
| Unit macros / `#undef` | 143 / 48 | 50 / 25 |
| Declarations of other units' functions (declscan D) | 425 | 282 |
| Old address-style names (hex-name) | 479 | 280 |
| Duplicate types (oddscan) | 100 | 62 |
| View types / view uses | 105 / 513 | 69 / 294 |
| Placeholder field names | 6,865 | 2,568 |
| Casts: duplicate-type | 920 | 446 |
| Casts: declaration-type | 948 | 832 |
| Casts: record-pun | 641 | 507 |
| Casts: void* in C++ | 710 | 629 |
| Casts: byte-pointer | 415 | 362 |
| Casts: raw-offset | 345 | 359 (up, from written-out macros) |

The workers' final batches landed after these numbers were taken, so the branch is a little further along.

## Functions dropped on the branch (the refix list)

The proof on the branch reads 13,498 exact functions, against 13,516 at the baseline. The drops, grouped by the batch that caused them:

| Batch | Lost functions | Likely cause |
|---|---|---|
| legogame views/renames merge (64e40eb6c) | LightSabreComboCode, ForceCode, SmartBombCode, Animate_WEIRDO, Animate_JEDI, Animate_ASTROMECH, Animate_PROTOCOL, SetPodMergeAnims (move.c); Condition_HelpWithTriggers (gameaiscript.c) | GAMEOBJECT/PLAYERPACKET field access replacing views. SmartBombCode needs its fadd operand order, which the old VEHICLE_WEAPONS_STATE view gave. |
| gameapi ClassObjectListEntry derive | ClassObjectList::GetAveragePosition, EdManipulator::Render, ClassEditor::Render | Fixed in commits 50efd39c8 and 529aefc1e (q/gameapi in the saga-pc-a2 worktree, not pushed yet): member calls and a `ClassObject::Self()` accessor. |
| legoapi macros / GIZBUILDIT, declarations, CUTINFO/MENU merges | 1 + 1 + 3 lost, 1 gained, 2 fragile | See the per-batch lines of the proof history. Run the baseline diff below for the names. |

To list every drop by name:

```
python3 - <<'EOF'
import json
base = set(open('baseline-functions.txt').read().split())   # exact names at 5717c76e
now = {f['name'] for f in json.load(open('matching.json'))['functions'] if f.get('exact_match')}
print('\n'.join(sorted(base - now)))
EOF
```

The baseline list is `matching.json` at 5717c76e: take the names with `exact_match` true.

## How to finish

1. **Refix the drops, cheapest first.** Try declaration tweaks that keep the readable form first, such as member order, a local, or an inline accessor. Where only the old form matches, restore that one site and mark it `// MATCH-EXCEPTION: <reason>`. Start with the move.c group.
2. **Settle the budgets.** Lower each budget to its count with `--write-budget`. Delete the "quick run, settled in the refix" lines from config/budget-raises.txt, and keep a reasoned line only for a count that really stays above main's.
3. **Merge to main.** Once the proof is at or above the baseline and the full push gate passes, merge `cleanup/quick-run` to main in one push.

## What stays, by design

- Every `goto`.
- MATCH-EXCEPTION sites.
- Config defines that must come before includes.
- Win32 name-clash `#undef`s (each with a push/pop).
- Generic cursors and allocator results (VARIPTR, NuAlloc), the polymorphic editor interface, and serialization write buffers.
- Casts whose removal would change a mangled name (const-taking V2Peer and EdClass signatures).
- The APIOBJECT/GAMEOBJECT base casts. They need APIOBJECT embedded as GAMEOBJECT's first member.
- Fields that nothing reads or writes.

## Where everything is

- **main** (30f4fec2e): the proven passes, and the NuInitHardware setup-tag macros.
  - main has the final NuInitHardware text. The branch carries an earlier version of it: `0X4x` enum members, and no `docs/pc-names.md` entry. Take main's version of those files when merging.
- **cleanup/quick-run**: the quick run's landed batches, this report, and the gameapi refix (3 functions back to exact). The proof on the branch reads about 13,501 exact functions, against 13,516 on main. The refix list above still applies, apart from the gameapi rows.
- **Parked work** that is built but was not gated onto the branch. Rebase each one onto cleanup/quick-run and run the quick gate:
  - `cleanup/parked-saga-pc-b`: nu2api, 13 commits. Macros, declarations, views and field names; its match was never measured.
  - `cleanup/parked-saga-pc-b2`: legoapi, 3 commits. The weapon-rate view and about 70 field names.
  - `cleanup/parked-saga-pc-c2`: legogame, 3 commits. 54 declarations moved into headers, which fails the standalone header check (13 headers).
  - `cleanup/parked-saga-pc-c2w`: legogame, 3 commits that never went through a gate. Customise, the hub pad readers, and the episode I countdown.
- **Worktrees:** all quick-run worktrees are removed. The older worktrees saga-pc-a, saga-pc-b2 (work/gate-speed) and saga-pc-c are untouched.
