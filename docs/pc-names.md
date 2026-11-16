# PC markers in names

This is the PC game, so a name says "PC" only where the thing is specific to the PC platform. A `Pc`,
`_PC` or `_pc` added to tell a PC record or function from its Android twin (`GIZMO_PC`, `ApplyPc`,
`texture_pc`) carries no meaning here: the twin either was the same thing (one record now) or is a
different thing (a descriptive name now). The renames are in `renames/16-*.csv` and `renames/17-*.csv`.

oddscan's `pc-suffix` class counts the markers left in code identifiers (strings and comments are not
counted) and lists them with `python3 tools/oddscan.py --list --class pc-suffix`. Its budget only goes down;
the end state is the kept names below (about 20 lines).

## Kept

| Name | Why |
|------|-----|
| `Pc...` and `PC...` prefixes of the platform layer: `PcInput*`, `PcMouse*`, `PcGetUserFolder`, `PCDevice*`, `PCGraphics*`, `PCOptionsBuildFolder`, `PCShader*`, ... | the PC platform layer's own functions and state (input, window, Direct3D device, folders); one prefix, never a second marker after it |
| file names `nufile_pc.c`, `nupad_pc.c`, `nudebug_pc.cpp`, `nushaderobject_pc.cpp`, `nutex_pc.h`, `bgproc_pc.h`, `NuEffectTex_pc.h`, `nu3d_pc_includes.h`, `nucore_pc_includes.h`, `nustencilshadows_PC.h`, `nuwater_PC.h`, `posteffect_shader_pc.cpp`, `posteffect_shader_tokens_pc.cpp`, `gamelib/net/NetMessagePc.*`, `pc_*.cpp` | the retail module names (`nucore\pc\nufile_pc.c`, `nu3d\pc\nutex_pc.cpp`, `nuqfnt_PC.cpp`, ...) or the PC half of a module; they appear in `#include` lines and in comments naming those modules (`nugscn_pc`, `nudlist_pc`, `nuportal_pc`) |
| `Nu3dPcIncludesVuVecLengthSq` (and the other `Nu3dPcIncludes*` copies in config) | a unit copy named after its compiland `nu3d_pc_includes` |
| `NuFileExt_pc_dds`, `NuFileExt_pc_gsc`, `NuFileExt_pc_ghg`, `NuFileExt_lr_pc_ghg` | name the PC asset extensions `_pc.dds`, `_pc.gsc`, `_pc.ghg`, `_lr_pc.ghg` |
| `LevelConfigGenericKeyword_farclip_pc`, `LevelConfigGenericKeyword_nearclip_pc` | name the PC-only level and cutscene keywords `farclip_pc` and `nearclip_pc` |
| `apitxt_PCPRESSSTARTFE`, `apitxt_PCPRESSSTARTP1`, `apitxt_PCPRESSSTARTP2` | the PC "press start" texts, named after their string table keys `GEN_PCPRESSSTART*` |
| string literals: retail source paths, `"_PC. \| _360. \| _PS3."`, `pc\stuff\LegoCubeMap_PC.dds`, `"%s_pc%03d.bmp"` | retail data; never changed |
| `NUAPI_SETUP_PC_*` and their `NUHW_PC_*` argument macros (`NUAPI_SETUP_PC_WINDOWTITLE`, `NUAPI_SETUP_PC_REGISTRYKEY`, ...) | original enum members of NuInitHardware's setup tags (build/reference, the structs header); the macros follow their names |
| `ov_pcm_total`, `ov_pcm_tell` | Ogg Vorbis PCM, not a marker |
