# Magic values: what is named, and what is still a literal

`python3 tools/oddscan.py --magic` ranks the fields that are compared with,
assigned or masked by integer literals. The values of a field get names when
the game itself says what they mean: a config keyword that sets the bit, a
name table indexed by the value, an editor menu label, a Windows SDK constant,
or function names that are indexed by the value. A descriptive name follows
from the use where the use makes the meaning plain. Values whose meaning is
not known keep their literal; they are listed here so the next pass knows
where to look.

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

## Still literal (meaning not known yet)

| Field | Uses | Values | What is known, where to look |
|---|---:|---|---|
| `STATUS_STAGE_s.unknown_14` | 207 | -1..8 | status screen stage sub-state; no name table found; the sibling build's `VOICE_STATUS` overlaps by value only |
| `STATUS_STAGE_s.type` | 57 | 2..30 | status stage kinds; no keyword parser on PC (stages are built in code) |
| `gameobject_s.action_movement_state` | 72 | 2..9 | movement mode of AI packets; the sibling build calls it `AIPACKET_MOVEMENT_MODE`, not verified on PC |
| `AIPACKET.path_connection_state` | 53 | 0..5 | path-following state; no strings |
| `gameobject_s.unknown_7b3` | 57 | 0..7 | a per-context sub-state (batarang, techno); meaning differs per context |
| `nudisplaylistitem_s.type` | 67 | 0x80..0x8d | display list command opcodes; no dump or name table on PC |
| `music_playback_s.state` | 59 | 4..13 | two values named (`MUSIC_PLAYBACK_*`); the rest have no strings |
| `debris_info_s.particle_type` | 48 | 0, 2, 3, 7 | the editor's Particle Type menu lists effect names, not the type |
| `gameobject_s.flags_1fc_word`, `flags_e20`, `flags_ef0`, `flags_ef4`, `flags_1f8`, `portal_flags`, `context_variant_flags` | 100-540 each | bit sets | runtime state bits set in code, no keywords; the sibling build's names are guesses |
| cheat indices (`Cheat_IsOn`) | 57 | 0..0x2c | come from the game's cheat config file, which is not among the retail files here |
| AI script `action_data_1` | ~20 | bit sets | keyword-driven, but each action gives the bits its own meaning: needs a set per action |
| `selector`, `semantic`, `mode`, `state`, `type`, `kind`, `index` (locals and parameters) | 50-150 each | | many unrelated locals share these names; name per function where the callee says what it takes |
