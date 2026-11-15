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
| `gameobject_s.unknown_7aa`, the `ANIMPACKET_PC` animations, `AnimPlaying` and friends | `ACTION_*` (character.h) | `PlayerActionInfoList[1 + action]` names (`ActionFromName`) |
| `RTL_LIGHT.type` | `RTL_LIGHT_*` (rtl.h) | light editor Light Type menu |
| `nudisplaylistitem_s.type`, `NuDisplayListLinkItem` type | `NUDLIST_*` (nudlist.h) | `DisplayListPrintItem` text per type; the render state packets after the packet cache they link (0xa7, 0xae, 0xaf, 0x98, 0x99 still literal) |
| `FRAME_PAD_PC.digital_buttons*`, `FRAME_SCANNED_PAD_PC`, `GAMEPAD_PC.buttons_*`, the `GAMEPAD_*` button globals | `PAD_BUTTON_*` (edui.h) | level editor pad help (`SetPadText` labels, PS2 bit order) |
| `debris_info_s.particle_type` | `DEBRIS_PARTICLE_*` (edgra.h) | particle editor texture type menu (`cbPtlSelTextureType`) |
| `music_playback_s.state` | `MUSIC_PLAYBACK_*` (nusound.h) | what `SoundUpdate`, `PlayMusic`, `SwapMusic` and `PlayCutMusic` do in each state (fade speed, which stream stops or pauses) |
| `STATUS_STAGE_s.type`, `AddStatusStage`, `FindStatusStage` | `STATUS_STAGE_*` (legoapi_types.h) | the stage callbacks of `StatusStagesLsw`/`StatusStagesAlt` and the conditions each stage is added under |
| `gameobject_s.flags_1fc` and its `_half`/`_word`/`_low` views (6 bits) | `APIOBJECT_FLAG_*` (apiobject.h) | `IN_USE` and `CHARACTER` from the object loops; `DONT_PUSH` from `CHARACTER_MODEL_DONT_PUSH` and the GameDontPush action, `SHADOW_TERRAIN` from the ShadowTerrain action, `PLAYER_ACTIVE` set on drop-in and cleared on drop-out, `CAN_FIRE` copied from `CHARACTER_MODEL_CAN_FIRE` |
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
| `gameobject_s.flags_e20`, `flags_ef0`, `flags_ef4`, `flags_1f8`, `portal_flags`, `context_variant_flags` | 100-540 each | bit sets | runtime state bits set in code, no keywords; the sibling build's names are guesses |
| cheat indices (`Cheat_IsOn`) | 57 | 0..0x2c | come from the game's cheat config file, which is not among the retail files here |
| AI script `action_data_1` | ~20 | bit sets | keyword-driven, but each action gives the bits its own meaning: needs a set per action |
| `AIPACKET.flags`, other bits | ~100 | 0x1-0x80000000 | path following and movement state set in `AISysProcessCharacter`, `AIMoveToDestination`, `AISysCharacterMovement`; no keyword |
| `cut_instance_load_s.flags_88` | 82 | bit set | cutscene instance state; 0x80 is set by every `instNuGCutSceneSet*`/`Translate`/`RotateY` (an own matrix), the rest only by internal updates |
| `NuSound3PlayStereoV` tags | ~30 | 0..12 | each tag fills one `PC_STEREO_PLAY_PACKET` field; tags 9, 11 and 12 set fields whose meaning is not settled (`option9_mode`, `option11`) |
| `selector`, `semantic`, `mode`, `state`, `type`, `kind`, `index` (locals and parameters) | 50-150 each | | many unrelated locals share these names; name per function where the callee says what it takes |
