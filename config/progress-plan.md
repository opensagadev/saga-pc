# Current work plan

The whole-project mandate remains in force; execution is paused on request.

Verified baseline FFF is [a803b6f](https://github.com/opensagadev/saga-pc/commit/a803b6f0973efced1b6d8e00885ff1f675f9aa53), with [all 13 CI steps passing](https://github.com/opensagadev/saga-pc/actions/runs/38141919062). GGG exact-revision CI is pending publication.

Checkpoint GGG connects genuine per-frame input polling and cache reset in
native caller order. The 27 new functions cover 8,881 complete PC bytes: keyboard
construction/destruction, real DirectInput/XInput sampling, mouse translation,
binding capture, record output and its math/file dependencies. Eight functions
match all 693 native bytes. State polling calls the real XINPUT1_3 DLL;
no device results, enabled flags or provider returns are fabricated.

The earlier FFF debugger run independently established InitSfx returning with
1,130 definitions, 1,404 samples and a non-null SfxInfo table, then entering
GetSfxIdN. This passes the previously attributed null-table startup frontier;
it does not establish audible output, lookup completion or playability.

The unchanged GGG source executable ran under Wine for 60.70 seconds before the
configured deadline stopped it. The PID-attributed window captures at 6.31,
20.83 and 58.83 seconds are identical black images. No fault was logged; no final
wait stack or completed frame was captured. This establishes a persistent game
window during the observation, not responsive input, completed rendering, loaded
world or playability. The actual wait remains unattributed by this ordinary run.
All 344 source objects and frozen input pins were unchanged. Crash-dialog
suppression was set and queried before launch; all owned game processes and
sampled game/crash windows were absent after identity-bound cleanup. Termination
was by the test deadline, not natural game exit.

The next runnable dependency wave is the genuine save-folder registration
(49 bytes) and save initialization/slot enumeration (385 bytes), restoring the
conditional hardware call and original InitOnce arguments. Current startup skips
both producers; the permanent loader waits for saveload_status. Restore actual
folder publication and six-slot file enumeration before readiness is produced.
Then connect the reviewed legal texture/material/color/vertex family (514 bytes),
font/text-table backing and the remaining permanent/world loaders. Keep native
failure behavior, caller order and matching; no forced readiness or substitute
providers. Research proposals remain local and uncompiled.

Work is paused at the user's request after this validated checkpoint is pushed.
Resume only on the user's instruction, using the retained proposals and current
source; the recurring automation is paused.

Whole-PC fuzzy coverage is 29.932080%
(+0.199805 percentage points).
Exact code is 236,143/3,471,246 bytes (+693), exact data is
637,627 bytes (+66), and compared PC code is
1,494,495 bytes (+8,881). Mapping covers code
1,526,428/3,471,246, data
646,798/36,018,966, and combined code/data
2,173,226/39,490,212 bytes
(5.503202%). Unknown original data extents earn zero
allocation or extent credit. These measurements do not measure playability.

Final checks pass: 796 tests (three existing skips), LLVM18 format371/tidy344,
source build/link and object validation (5,247 functions, 11,490 data definitions,
344 objects), and authoritative full retail matching. The isolated DAT fixture
now reuses the actual file-writer endian definition; the initial missing-fixture
link dependency was fixed without changing production behavior or weakening tests.

See [execution evidence](transfer-execution.md) and [runnable milestones](../docs/roadmap.md).
