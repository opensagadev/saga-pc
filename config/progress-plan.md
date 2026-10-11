# Current work plan

The [whole-project mandate](../SOL_IMPLEMENTATION_PLAN.md) remains active.
Verified source baseline DDD is [be647b8](https://github.com/opensagadev/saga-pc/commit/be647b859b259502f46569294b0cff7680ab7703), with [all 13 CI steps passing](https://github.com/opensagadev/saga-pc/actions/runs/38136446273). EEE exact-revision CI will be checked after publication.

The rebuilt source was tested with empty arguments and installed assets, first
under a correctly installed four-byte WineDbg write watch, then normally for
60 seconds. The debugger captured genuine declaration-count writes 1 and 2;
at a later fault the count was 13 and the cache contained real records. This run
passed the formerly observed four-record storage failure. It does not prove the
original cache capacity or establish a bound for every startup/gameplay path.

The new attributed fault is source `0x005347d5`, inside exact `GetSfxIdN`
(native `0x650e40`, 77 complete bytes), reading null `SfxInfo`. The recorded return
chain passes through particle loading. Current source omits the preceding native
sound configuration provider at `0x6521b0`, and never assigns `SfxInfo`.
Recovering that real producer and its consumed dependencies is the next priority;
a null guard or dummy table would hide the missing reconstruction.

The separate normal run reached its 60-second deadline without a logged source
fault; its PID-attributed window remained black in samples at 5.79, 21.44 and
55.81 seconds. Debugger timing changes scheduling, so these are distinct results.
No completed nontrivial rendering, world activation or playable game is established.
Both runs preserve all frozen inputs and objects, set and query crash-dialog
suppression, and leave no owned game/debugger process or sampled game/crash window.
The debugger was explicitly killed/quit after capture; the normal run was stopped
at its deadline. Neither is reported as a natural successful game exit.

1. Follow the latest attributed source failure recorded in the runtime evidence.
   Preserve genuine native behavior and recover its storage/providers with explicit
   confidence and extent qualifications. The inferred cache backing receives zero
   original allocation credit; validate each observed usage domain separately.
2. Close real device/resource/presentation dependencies and verify completed frames.
3. Close permanent/area/world loading and character/camera activation, then recurring
   updates, movement, collision, menus, audio, saves and transitions.
4. Integrate coherent source/layout/data waves, preserve legitimate prior exacts,
   run full checks and retail matching, publish validated checkpoints and verify CI.

EEE adds 132 exact PC code bytes, restores real timing/critical startup calls and
corrects native lock indexing. It explicitly retires 304 falsely credited data bytes.
The declaration backing of 256 records is an architectural inference with unknown original
owning extent, rather than an established PC allocation size.
See [execution evidence and contracts](transfer-execution.md),
[strategy](strategy.md) and [runnable milestones](../docs/roadmap.md).
