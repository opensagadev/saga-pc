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
| `gameobject_s.flags_e20`, `flags_ef0`, `flags_ef4`, `flags_1f8`, `portal_flags`, `context_variant_flags` | 100-540 each | bit sets | runtime state bits set in code, no keywords; the sibling build's names are guesses |
| cheat indices (`Cheat_IsOn`) | 57 | 0..0x2c | come from the game's cheat config file, which is not among the retail files here |
| AI script `action_data_1` | ~20 | bit sets | keyword-driven, but each action gives the bits its own meaning: needs a set per action |
| `AIPACKET.flags`, other bits | ~100 | 0x1-0x80000000 | path following and movement state set in `AISysProcessCharacter`, `AIMoveToDestination`, `AISysCharacterMovement`; no keyword |
| cut scene instance `flags_88`, other bits | ~30 | 0x1, 0x4, 0x8, 0x800, 0x3e000 (a 5-bit value), 0x8000000-0x40000000 | 0x8 set and cleared by cutscene control code, 0x10000000/0x20000000 by the stream setup, 0x40000000 with the skip countdown; not settled |
| `NuSound3PlayStereoV` tags | ~30 | 0..12 | each tag fills one `PC_STEREO_PLAY_PACKET` field; tags 9, 11 and 12 set fields whose meaning is not settled (`option9_mode`, `option11`) |
| `selector`, `semantic`, `mode`, `state`, `type`, `kind`, `index` (locals and parameters) | 50-150 each | | many unrelated locals share these names; name per function where the callee says what it takes |
