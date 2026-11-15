# Magic values: what is named, and what is still a literal

`python3 tools/oddscan.py --magic` ranks the fields that are compared with,
assigned or masked by integer literals. The values of a field get names when
the game itself says what they mean: a config keyword that sets the bit, a
name table indexed by the value, an editor menu label, a Windows SDK constant,
or function names that are indexed by the value. A descriptive name follows
from the use where the use makes the meaning plain. Values whose meaning is
not known keep their literal; they are listed here so the next pass knows
where to look.

`python3 tools/magicnames.py FIELD --names PREFIX` writes the names at every
literal of a field once they are chosen: comparisons, assignments, `case`
labels and masks, also through locals, parameters and function results that
only carry the field (`--dry-run` lists the edits and the values left without
a name). Prove the result like any change.

Bare `type`, `state`, `mode` and `flags` locals and parameters of unrelated
functions share a name, so a ranking by name pools them. `python3
tools/magicnames.py --locals type,state,mode,flags` lists their literals per
`Function:name` (the declaration keys of the carrier analysis: a local by its
declaration, a parameter by its function and position), with the fields each
one is stored from, into or compared with, also through other locals. A local
that only carries a named field is named with it already; for the others the
same `Function:name` is a field argument: `magicnames.py
GizmoBlowupBlowup:hit_type --names BLOWUP_HIT_` names the literals of that
parameter, the arguments passed for it and the locals and parameters that
carry it.

Before a number is named, check whether it is a size or a count:
`python3 tools/sizeofnames.py --dry-run` lists the literals that equal the size
or element count of the object they act on (the length of `memset`, `memcpy`,
`NuStrNCpy` and friends, an allocator's size, a `for` bound over arrays indexed
by the counter, an offset past a record) and without `--dry-run` writes
`sizeof(x)`, `sizeof(*p)`, `sizeof(T)` or, for an unsigned loop counter,
`sizeof(a) / sizeof(a[0])` there (a signed counter keeps its literal: `sizeof`
would make the comparison unsigned). The 957 bounds of signed loop counters
that stay literal this way are decided: they are matched code with signed
counters, so they are not rewritten and not listed as open.
tools/castscan.py counts the sites left as `size-literal` (budget 0);
`--list-multiples` shows sizes that are a multiple of an element size, whose
count still needs a name. Loop counters and sizes are not magic values:
`oddscan --magic` leaves out the literals of a variable that a `for` of the
same function initialises and steps (its values are indexes).

## Named

| Field or value | Names | Evidence |
|---|---|---|
| `area_data_s.flags` | `AREAFLAG_*` (hub.h) | levels\areas.txt keywords in `Areas_ConfigureList` |
| `level_data_s.flags` | `LEVEL_*` (level.h) | levels\levels.txt keywords and the `LC_AL_*` config switches |
| `gameobject_s.character_context`, locals named `context` | `CHARACTER_CONTEXT_*` (apiobject.h) | `PCContextInfoTable[1 + context]` row names |
| `characterdata_s.model_flags` | `CHARACTER_MODEL_*` (character.h) | `CC_*` character config keyword handlers |
| `CHARACTERANIM_CONFIG.flags`, `.misc_flags`, `CHARACTER_EFFECT_CONFIG.flags` | `CHARANIM_*`, `CHARANIM_MISC_*`, `CHAREFFECT_*` | anim_start and effect_start keywords |
| `debris_info_s.generator_type` | `DEBRIS_GENERATOR_*` (edgra.h) | debris editor GenSort Type menu, `gensorttab` |
| Windows language IDs, `NuLanguageSet` values | `MAKELANGID(...)`, `NULANGUAGE_*` (nuapi.h) | SDK, `PcNativeSelectLanguage`, `Text_GetLanguagePath` |
| `custom_piece_config_s.availability_flags`, `specialmove_s.flags` | `CUSTOM_PIECE_*`, `SPECIALMOVE_*` | customiser and special move config keywords |
| `gameobject_s.unknown_7aa`, the `ANIMPACKET` animations, `AnimPlaying` and friends | `ACTION_*` (character.h) | `PlayerActionInfoList[1 + action]` names (`ActionFromName`) |
| `RTL_LIGHT.type` | `RTL_LIGHT_*` (rtl.h) | light editor Light Type menu |
| `nudisplaylistitem_s.type`, `NuDisplayListLinkItem` type | `NUDLIST_*` (nudlist.h) | `DisplayListPrintItem` text per type; the render state packets after the packet cache they link (0xa7, 0xae, 0xaf, 0x98, 0x99 still literal) |
| `FRAME_PAD.digital_buttons*`, `FRAME_SCANNED_PAD`, `GAMEPAD.buttons_*`, the `GAMEPAD_*` button globals | `PAD_BUTTON_*` (edui.h) | level editor pad help (`SetPadText` labels, PS2 bit order) |
| `debris_info_s.particle_type` | `DEBRIS_PARTICLE_*` (edgra.h) | particle editor texture type menu (`cbPtlSelTextureType`) |
| `music_playback_s.state` | `MUSIC_PLAYBACK_*` (nusound.h) | what `SoundUpdate`, `PlayMusic`, `SwapMusic` and `PlayCutMusic` do in each state (fade speed, which stream stops or pauses) |
| `STATUS_STAGE_s.type`, `AddStatusStage`, `FindStatusStage` | `STATUS_STAGE_*` (legoapi_types.h) | the stage callbacks of `StatusStagesLsw`/`StatusStagesAlt` and the conditions each stage is added under |
| `gameobject_s.flags_1fc` and its `_half`/`_word`/`_low` views (6 bits) | `APIOBJECT_FLAG_*` (apiobject.h) | `IN_USE` and `CHARACTER` from the object loops; `DONT_PUSH` from `CHARACTER_MODEL_DONT_PUSH` and the GameDontPush action, `SHADOW_TERRAIN` from the ShadowTerrain action, `PLAYER_ACTIVE` set on drop-in and cleared on drop-out, `CAN_FIRE` copied from `CHARACTER_MODEL_CAN_FIRE` |
| `PLAYERCHARACTERCONFIG.flags_090` (31 of 32 bits) | `CHARACTER_RUNTIME_*` (move.h) | the character config keywords whose handlers call `CC_SetRuntimeFlag` (and `is_a_girl`, `combat_roll`) |
| `gameobject_s.flags_1f8` (5 bits) | `APIOBJECT_SIDE_*`, `APIOBJECT_CREATURE`, `APIOBJECT_NO_SHADOWS` (apiobject.h) | the SetSide AI action's keywords (neutral, baddie, goodiebaddie; default from `CHARACTER_MODEL_NEUTRAL`/`_BADDIE`), the NoShadows action, `AddCreature` callers |
| `nuinstanim_s.flags` (2 bits) | `NUINSTANIM_FLAG_PLAYING`, `NUINSTANIM_FLAG_REPEATING` (legoapi_types.h) | the animation editor control modes (start, loop) and the game animation sets' `repeating` setting |
| `SPECIAL_MINIKIT.flags` | `SPECIAL_MINIKIT_*` (gizmopickups.h) | the specialminikit keywords (draw_at_start, draw_while_animating, draw_at_end, orientate, at_gizmo) |
| `GIZMOPICKUP.flags` (4 bits) | `GIZMOPICKUP_STATE_*` (gizmopickups.h) | `GizmoPickup_TurnOnPickup`, the draw loop, collecting, the saved-minikit alternate type |
| cut scene instance `flags_88` (all views, 6 bits) | `NUGCUTSCENE_INST_*` (nugcutscene.h) | what `instNuGCutSceneStart`/`End`/`Reset`, the `instNuGCutSceneSet*` transforms and `instNuGCutSceneClipTest` set and test |
| `EdRef.member_offset` pseudo ids (0x8000000n) | `EDREF_LIGHT_*`, `EDREF_FOG_*`, `EDREF_PLACEABLE_*`, `EDREF_SPLINE_*`, `EDREF_KNOT_*`, `EDREF_SPECIAL_OBJECT_*` (edlevelall.h) | the member label each editor class registers with the id; `EdRefPlaceable` 0x80000006 is registered as both "Pos" and "Mtx" and stays literal |
| `DOOR_s.flags`, `door_config_s.flags` | `DOOR_FLAG_*` (door.h) | the door config keywords (one_way, cam_lookatplayers, do_not_use, use_as_start, two_players_only) |
| `AIPACKET.flags` (5 of its bits) | `AIPACKET_*` (apiobject.h) | AI script actions and conditions that set or test them (`IgnoreWallSplines`, `CheckWallSplines`, `SetCircleDirection`, `PathBlocked`, `SetFormationCommander`) |
| `gameobject_s.flags_e20` (11 bits) | `APIOBJECT_FORCE_*`, `APIOBJECT_ATTACK_TARGET_SET`, `APIOBJECT_BOLT_ON_COURSE`, `APIOBJECT_WEAPON_OUT`, `APIOBJECT_LAUNCH_ROCKET`, `APIOBJECT_COMBO_IN_TIME`, `APIOBJECT_DETONATOR_WAIT_RELEASE`, `APIOBJECT_SHOT_FIRED` (apiobject.h) | `FindForcePushTarget` (choke and lightning from `CHARACTER_RUNTIME_CHOKE`/`_LIGHTNING`, take-over with the ForceMindTrick sound), the UseForce action's `forcepush_highlight_only` keyword, `SetObjTarget`/`SetGizmoBlowUpTarget`/`SetPartTarget` and the bolt aiming that reads it, `Bolt_AddDeflectedBolt` and `StartDodge`, `SetWeaponOut`/`SetWeaponIn`; the JgRocket release frame and the rocket part `GameObjectStuffAfterAnimation` fires, the sabre combo's mistimed press, `Detonator_MoveCode` waiting for the special button's release, `ShootCode` and `Bolts_Update` marking the animation's shot fired |
| `gameobject_s.portal_flags` and its byte `combat_flags` (18 bits) | `APIOBJECT_CAN_DEFEND` ... `APIOBJECT_NO_GRAVITY` (apiobject.h) | the AI script actions that set and clear them (CanDefend, CanUseWeapon, KeepWeaponOut, SetBoss, Respawnable and Kill's respawn keywords, ApplyGravity FALSE, ...: the keyword of each is in the comment); `respawnable` and `respawn_at_origin` are also the struct's bitfield names |
| `gameobject_s.flags_ef0` and its byte `unknown_ef0` (13 bits) | `APIOBJECT_IGNORE_SHOVE_SYSTEM` ... `APIOBJECT_CAN_TAKE_OVER` (apiobject.h) | AI script actions (IgnoreShoveSystem, CannotBeSeen, SetDontMove, WalkBackwards, CanPullLevers, SetDoomedEscapeLocator take_damage, ...) |
| `gameobject_s.flags_ef4` and its byte `ai_update_flags` (17 bits) | `APIOBJECT_DONT_AIM_AT` ... `APIOBJECT_IGNORE_TURN_AROUND_SPLINE` (apiobject.h) | AI script actions (DontAimAt, CanBeCarried, UseTimeBasedUpdate FALSE, CanCollideWithObjects TRUE, ...); `UpdateGameObjects` sets `APIOBJECT_AI_UPDATE_NOW` on the frames of the object's AI update interval and `APIOBJECT_AI_EVERY_FRAME` when the interval is 1; `APIOBJECT_GIRL` is copied from `CHARACTER_RUNTIME_IS_A_GIRL` (CanPunchGirls, the voices) |
| `GizmoBlowupBlowup` hit_type (`Function:name`), `GIZMOBLOWUP_PC.status_flags` (15 bits) | `BLOWUP_HIT_*`, `BLOWUP_STATUS_*` (gizmoblowups.h) | the callers of each hit type (fighting punches and the whip lash, Batarang, bolts, proximity of a character or of a vehicle, signal, carrying, transform, explosions and the sabre through `GizmoBlowUp_Hit`, torpedo); the bit `GizmoBlowupBlowup` sets per hit type; the outputs Blownup, Punched and Plugging (`Blowup_GetOutput`) and the struct's bitfield names `visible` and `active` |
| `PLAYERCHARACTERCONFIG.flags_094` (32 bits), `.flags_098` (7 bits) | `CHARACTER_OPTION_*` (move.h), the struct's bitfields (`fixed_layers`, `can_poo`, `can_zap`, `dont_draw_rider`, ...) | the character config keyword handlers in charconfig.c that set each bitfield; byte tests read the bitfield |
| `PART_PC.active`, bit 0x1 | `PART_IN_USE` (game_obj.h) | `AddPart` takes a slot without it and sets it, `KillPart` clears it; the other bits (0x2-0x20000) are runtime state set in parts.c |
| `AIPATHCNX.traversal_flags` (top 5 bits) | `AIPATHCNX_*` (aisys.h) | the AI editor's connection types and labels (`aieditor_RegisterPathCnxType`: Permanent Block, Temporary Block, Link Obstacle); `AIPATHCNX_HIDDEN` set while an end node's special is hidden, `AIPATHCNX_TOO_LONG` while the connection is longer than `max_horizontal_distance` |
| `pushblock_s.packed_state_flags` (7 bits) | `PUSHBLOCK_*` (legoapi_types.h) | the struct's bitfield names `push_visible`, `push_active` and `progress_state`; the axis and direction bits `edpush_Load` reads, from the pushes `NearestFacingPushBlock` refuses with each |
| `gameobject_s.context_variant_flags` and its byte `context_flags_pc` (2 bits) | `APIOBJECT_CONTEXT_EVENT_DONE`, `APIOBJECT_CONTEXT_INVULNERABLE` (apiobject.h) | 0x40 is cleared when a context starts and set when its action happens, in every context (lever pulled, signal sent, blowup carried or transformed, detonator released, lunge landed); 0x20000 together with the context info's 0x8000000 makes hits do nothing (fighting, bolts, sabre sparks), set while the batarang is steered |
| `GameCameraMakeMiniCut3` flags, `MINICAM_COMMAND.type`, the mini camera's `mode` and `easing` | `MINICUT_*`, `MINICAM_COMMAND_*`, `MINICAM_MODE_*`, `MINICAM_EASING_*` (minicamcut.h) | the DynamicCameraCut AI action's keywords (range=, rotx=, cont_roty=, start_time=, attach_to_spline=, mode=follow, slowfastslow, ...) and what MiniCam's command loop does with each command |
| `PC_FILEEXTINFO.type`, `.platform` | `NUFILEEXT_*`, `NUFILEEXT_PLATFORM_*` (nufile.h) | the extensions `NuFileExtInit` registers per kind and platform (.an3, .ghg, _pc.dds, .wav, .bik; _ps3, _360, .vag, .dsp, .at3) |
| `ShaderParserNodePc.kind`, shader declaration flags | `SHADER_NODE_*`, `SHADER_DECL_*` (CRC16.h) | the node type each ShaderParser*Pc constructor stores; the nu2_use/nu2_declare storage keywords (vs_local, fs_local, varying, attribute, uniform, shared) and the vreg/freg bits of `ShaderDeclarationFlagsPc` |
| `ObjHitObj` flags, `BOLT_PC.hit_flags` (6 bits) | `OBJHIT_*` (hits.h) | `ObjHitObj_Flags`: the attacker's side (SetSide keywords) and whether it is an active player; players' bolts may hit gizmos (0x800, `Bolt_HitPlatPrivate`) |
| `eduiitem_s.type` | `EDUIITEM_*` (edui.h) | the type each `eduiItem*Create` constructor stores |
| `AIEDITORSETTINGS.display_options` (8 bits) | `AIEDITOR_*` (aisysall.c) | the AI editor menu toggles and their labels (Draw All Paths, Snap Height, Stop Platforms, ...) |

## Still literal (meaning not known yet)

| Field | Uses | Values | What is known, where to look |
|---|---:|---|---|
| `STATUS_STAGE_s.unknown_14` | 207 | -1..8 | status screen stage sub-state; no name table found; the sibling build's `VOICE_STATUS` overlaps by value only |
| `STATUS_STAGE_s.type`, 5-7, 24, 27-34 | ~15 | | the LevelComplete and BonusTime stages and the Stage5/6 ones of the alternate table; their text ids are unnamed globals |
| `gameobject_s.action_movement_state` | 72 | 2..9 | movement mode of AI packets; the sibling build calls it `AIPACKET_MOVEMENT_MODE`, not verified on PC |
| `AIPACKET.path_connection_state` | 53 | 0..5 | path-following state; no strings |
| `gameobject_s.unknown_7b3` | 57 | 0..7 | a per-context sub-state (batarang, techno); meaning differs per context |
| `gameobject_s.flags_1fc`, other bits | 45 | 0x4, 0x10, 0x20, 0x40, 0x100, 0x400, 0x2000, 0x10000, 0x40000 | 0x4 is set by the player movement update under many conditions, 0x20 when riding a bike (with 0x400 for a cheap zip), 0x2000 when a respawn position is saved off a path; not settled |
| `gameobject_s.flags_1f8`, other bits | ~70 | 0x2, 0x40, 0x80, 0x100, 0x200, 0x800, 0x1000, 0x4000, 0x20000, 0x40000 | 0x40000 is tested by the pause menu, the network camera and the game update; the sibling build's `APIOBJECT_STATE_FLAG_IGNORE_DOORS` (transitional/unused.h) is unverified |
| `gameobject_s.flags_e20`, other bits | ~130 | 0x20 (tested to skip an object almost everywhere, cleared by `UpdateGameObjects`; its setter is not in the source yet), 0x1000 (`GizPanel_MoveCode`), 0x1-0x8, 0x4000, 0x20000, 0x80000-0x80000000 | runtime state bits set in code; read each setter |
| `gameobject_s.context_variant_flags`, other bits | ~100 | 0x1-0x10000 | the bits mean something else in each character context: 0x8000 is set when the jump animation's hold time runs out (jump) and when the carry throw's timer ends (carrying), 0x100 in grapple, whip and zip-up, 0x80 and 0x1-0x10 in the sabre combo; name them per context |
| `gameobject_s.flags_ef0`, `flags_ef4`, `portal_flags`, other bits | 30-50 each | bit sets | runtime state bits set in code (`AttackOpponent` sets portal 0x20 and 0x400), no keywords; the sibling build's names are guesses |
| `GIZMOBLOWUP_PC.status_flags`, other bits | ~60 | 0x400, 0x1000, 0x8000, 0x10000-0x400000, 0x1000000, 0x2000000 | 0x400 and 0x1000 are hit types 10 and 12; 0x8000 is tested with `BLOWUP_STATUS_VISIBLE` by torpedo and transform targeting; 0x80000 waits for the animation's end frame |
| `gizforce_s.runtime_flags` | 86 | 0x20, 0x40, 0x800, 0x2000, 0x10000, 0x20000 most used | the `GIZFORCE_RUNTIME_*` names of transitional/unused.h (0x1, 0x8) do not cover the bits the code uses |
| cheat indices (`Cheat_IsOn`) | 57 | 0..0x2c | come from the game's cheat config file, which is not among the retail files here |
| AI script `action_data_1` | ~20 | bit sets | keyword-driven, but each action gives the bits its own meaning: needs a set per action |
| `AIPACKET.flags`, other bits | ~100 | 0x1-0x80000000 | path following and movement state set in `AISysProcessCharacter`, `AIMoveToDestination`, `AISysCharacterMovement`; no keyword |
| cut scene instance `flags_88`, other bits | ~30 | 0x1, 0x4, 0x8, 0x800, 0x3e000 (a 5-bit value), 0x8000000-0x40000000 | 0x8 set and cleared by cutscene control code, 0x10000000/0x20000000 by the stream setup, 0x40000000 with the skip countdown; not settled |
| `NuSound3PlayStereoV` tags | ~30 | 0..12 | each tag fills one `PC_STEREO_PLAY_PACKET` field; tags 9, 11 and 12 set fields whose meaning is not settled (`option9_mode`, `option11`) |
| `selector`, `semantic`, `mode`, `state`, `type`, `kind`, `index` (locals and parameters) | 50-150 each | | many unrelated locals share these names; name per function where the callee says what it takes |
| `AIPATHCNX.traversal_flags`, low bits | ~45 | 0x2-0x8000 | the game's connection types: `LEGO_AIPATHCNX_*` variables assigned per game (hub.cpp, starwars_gameai.c, batarang.cpp), while the code of those units tests literals; name them per game once it is settled which assignment each unit runs under |
| `pushblock_s.packed_state_flags`, other bits | ~50 | 0x1-0x800, 0x38000 (a 3-bit snap index), 0x4000000, 0x8000000, 0x10000000-0x40000000 | 0x4000000, 0x8000000, 0x20000000 and 0x40000000 are editor booleans `edpush_Load` reads without labels; the low bits are push and snap state in `UpdatePushBlocks` |
