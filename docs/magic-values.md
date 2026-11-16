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
| `AIMoveInstruction` mode, `AIPACKET.flags` low bits | `AIMOVE_*`, `AIPACKET_MOVE_MODE` (aisys.h) | the movement function `AISysCharacterMovement` runs for each mode (AIMoveToDestination, AIRetreatFromDestination, AICircle, AIWander, ...); mode 5 moves a formation member alone |
| `AISCRIPTPROCESS.action_data_1` of the follow, circle, go-to-locator and engage actions, `FollowAPIObject` flags | `AIFOLLOW_*`, `AICIRCLE_*`, `AIGOTO_*`, `AIENGAGE_*` (aiscript_types.h) | the keyword of each action that sets the bit (can_go_off_path, ignore_radius, facing, currentdist, locator=, on_ground, face_opponent, offscreen, minrange=, no_fire_in_minicut, ...); the other actions keep a boolean or a counter there |
| `NUMTLATTRIB.alpha_mode`, `.cull_mode`, `.z_mode`, the quick font flags | `NUMTL_ALPHA_*`, `NUMTL_CULL_*`, `NUMTL_ZMODE_*` (numtl.h), `NUQFNT_OPAQUE`, `_3D`, `_Z_READ_ONLY`, `_Z_BUFFERED` (nuqfnt.h) | the render states `NuMtlSetRenderStatesPS` and `PCDisplayHandlerSetZMode` set per value (blend factors, cull mode, z enable/write/func); the material each font flag sets up (0x4 and 0x10, attribute bits 4-5, stay literal) |
| `NuQFntSetCoordinateSystem` and `NuQFntPushCoordinateSystem` modes | `NUQFNT_CS_*` (nuqfnt.h) | the text scale and offset each mode sets |
| `EdRef.attributes`, `EdClass.flags`, `EdStream.flags` | `EDREF_ATTR_*`, `EDCLASS_*`, `EDSTREAM_STATE` (edlevelall.h) | what the editor looks a member up for (`GetAttributeData`, `FindMember`, the property editor's pos/rot/scale rows), the state and .led stream filters, `AddType` folding member bits into the class; the class bits from their single use each |
| `STATUS_STAGE_s.step` (was `unknown_14`), `step_time`, `step_duration` | `STATUS_STEP_*`, `PROMPT_STEP_*` (legoapi_types.h), per stage `TRUEHERO_STEP_*`, `MINIKIT_STEP_*`, `COINS_STEP_*`, `CHALLENGE_CASH_STEP_*` (gamestatus.c), `*_ALT_STEP_*` (status_alternate.cpp), all in legoapi_types.h | what each stage's update callback does in the step and what its draw callback shows; -1 until `DrawStatusBG_*` starts the stage |
| cut scene instance `flags_88` (11 more bits), `flags_8c` (7 bits) | `NUGCUTSCENE_INST_*`, `NUGCUTSCENE_PLAY_*` (nugcutscene.h) | `instNuGCutSceneUpdate` (loop and ping-pong at the ends, pause, auto start, the stream buffer swap and the skip countdown), `CUTINFO_LOOPING`, the cut script's pause, the struct's `active_stream_buffer` bitfield |
| `bolt_s.flags` 0x40, 0x40000 | `BOLT_DONT_GET_DEFLECTED_BACK`, `BOLT_OWNER_RIDDEN` (bolts.h) | `Bolt_Shoot` copies the owner's `APIOBJECT_BOLTS_DONT_GET_DEFLECTED_BACK`; `Bolt_Init` sets 0x40000 for an owner with a rider |
| `AIPACKET.path_connection_state` in the jump connection handlers | `AIJUMP_*` (apiobject.h) | what `StarWars_PrepareJump`, `Indy_PrepareJump` and `Batman_PrepareJump` do in each state (approach, back up, run up, wait for the double jump, jump); the tube and door handlers keep their numbers |
| `AIPACKET.reset_mode` | `AIPACKET_RESET_*` (apiobject.h) | `UpdateGameObjects` and `ResetCreatures`: the "InActive" state, the staggered start, the crate, the disabled mask the save keeps |
| `HATMACHINE.animation_state`, `TECHNO_s.target_mode` | `HATMACHINE_ANIM_*` (hatmachine.h), `TECHNO_TARGET_*` (techno.h) | the lid and arm specials the draw moves in each step; the lookup `Technos_FindTgt` uses per kind |
| `PLAYERCHARACTERCONFIG.variant` (was `weapon_action`), the special moves' attacker and victim variant | `CHARVARIANT_*` (character.h) | the variant names table `StartupCharVariantNames` (`CharVariant_Find`); `SetMoveAndAnimateFunctions` installs the walker, AT-AT, hover droid, critter and Fett functions per variant; variant 21 has no name |
| `eduiitem_s.text_alignment`, the edui print alignment | `EDUI_ALIGN_*` (edui.h) | where `EduiPrintAlignedText` starts the text |
| `TERRAIN_PLATFORM.flags` 0x1, 0x2 | `TERRAIN_PLATFORM_FLAG_ROTATING`, `_COLLIDED` (legoapi_types.h) | `PlatInstRotate` and `AddPickupTerrRot`'s rotating argument; the collision sets 0x2 |
| `freeplaymode` | `FREEPLAY_MENU_*` (hub.h) | what the hub's free play update and draw do in each step |
| `gameobject_s.unknown_7b3` in the super carry and whip contexts, `gameobject_s.whip_action` | `SUPERCARRY_*` (carrying.h), `WHIP_*` (whip.h) | the LEGOACT_SUPERCARRY_* and LEGOACT_WHIP_* animation each step plays; the whip action chosen on the button press |
| `GIZMOPICKUP.flags` (4 bits) | `GIZMOPICKUP_STATE_*` (gizmopickups.h) | `GizmoPickup_TurnOnPickup`, the draw loop, collecting, the saved-minikit alternate type |
| cut scene instance `flags_88` (all views, 6 bits) | `NUGCUTSCENE_INST_*` (nugcutscene.h) | what `instNuGCutSceneStart`/`End`/`Reset`, the `instNuGCutSceneSet*` transforms and `instNuGCutSceneClipTest` set and test |
| `EdRef.member_offset` pseudo ids (0x8000000n) | `EDREF_LIGHT_*`, `EDREF_FOG_*`, `EDREF_PLACEABLE_*`, `EDREF_SPLINE_*`, `EDREF_KNOT_*`, `EDREF_SPECIAL_OBJECT_*` (edlevelall.h) | the member label each editor class registers with the id; `EdRefPlaceable` 0x80000006 is registered as both "Pos" and "Mtx" and stays literal |
| `DOOR_s.flags`, `door_config_s.flags` | `DOOR_FLAG_*` (door.h) | the door config keywords (one_way, cam_lookatplayers, do_not_use, use_as_start, two_players_only) |
| `AIPACKET.flags` (5 of its bits) | `AIPACKET_*` (apiobject.h) | AI script actions and conditions that set or test them (`IgnoreWallSplines`, `CheckWallSplines`, `SetCircleDirection`, `PathBlocked`, `SetFormationCommander`) |
| `gameobject_s.flags_e20` (11 bits) | `APIOBJECT_FORCE_*`, `APIOBJECT_ATTACK_TARGET_SET`, `APIOBJECT_BOLT_ON_COURSE`, `APIOBJECT_WEAPON_OUT`, `APIOBJECT_LAUNCH_ROCKET`, `APIOBJECT_COMBO_IN_TIME`, `APIOBJECT_DETONATOR_WAIT_RELEASE`, `APIOBJECT_SHOT_FIRED` (apiobject.h) | `FindForcePushTarget` (choke and lightning from `CHARACTER_RUNTIME_CHOKE`/`_LIGHTNING`, take-over with the ForceMindTrick sound), the UseForce action's `forcepush_highlight_only` keyword, `SetObjTarget`/`SetGizmoBlowUpTarget`/`SetPartTarget` and the bolt aiming that reads it, `Bolt_AddDeflectedBolt` and `StartDodge`, `SetWeaponOut`/`SetWeaponIn`; the JgRocket release frame and the rocket part `GameObjectStuffAfterAnimation` fires, the sabre combo's mistimed press, `Detonator_MoveCode` waiting for the special button's release, `ShootCode` and `Bolts_Update` marking the animation's shot fired |
| `gameobject_s.portal_flags` and its byte `combat_flags` (18 bits) | `APIOBJECT_CAN_DEFEND` ... `APIOBJECT_NO_GRAVITY` (apiobject.h) | the AI script actions that set and clear them (CanDefend, CanUseWeapon, KeepWeaponOut, SetBoss, Respawnable and Kill's respawn keywords, ApplyGravity FALSE, ...: the keyword of each is in the comment); `respawnable` and `respawn_at_origin` are also the struct's bitfield names |
| `gameobject_s.flags_ef0` and its byte `unknown_ef0` (13 bits) | `APIOBJECT_IGNORE_SHOVE_SYSTEM` ... `APIOBJECT_CAN_TAKE_OVER` (apiobject.h) | AI script actions (IgnoreShoveSystem, CannotBeSeen, SetDontMove, WalkBackwards, CanPullLevers, SetDoomedEscapeLocator take_damage, ...) |
| `gameobject_s.flags_ef4` and its byte `ai_update_flags` (17 bits) | `APIOBJECT_DONT_AIM_AT` ... `APIOBJECT_IGNORE_TURN_AROUND_SPLINE` (apiobject.h) | AI script actions (DontAimAt, CanBeCarried, UseTimeBasedUpdate FALSE, CanCollideWithObjects TRUE, ...); `UpdateGameObjects` sets `APIOBJECT_AI_UPDATE_NOW` on the frames of the object's AI update interval and `APIOBJECT_AI_EVERY_FRAME` when the interval is 1; `APIOBJECT_GIRL` is copied from `CHARACTER_RUNTIME_IS_A_GIRL` (CanPunchGirls, the voices) |
| `GizmoBlowupBlowup` hit_type (`Function:name`), `GIZMOBLOWUP.status_flags` (15 bits) | `BLOWUP_HIT_*`, `BLOWUP_STATUS_*` (gizmoblowups.h) | the callers of each hit type (fighting punches and the whip lash, Batarang, bolts, proximity of a character or of a vehicle, signal, carrying, transform, explosions and the sabre through `GizmoBlowUp_Hit`, torpedo); the bit `GizmoBlowupBlowup` sets per hit type; the outputs Blownup, Punched and Plugging (`Blowup_GetOutput`) and the struct's bitfield names `visible` and `active` |
| `PLAYERCHARACTERCONFIG.flags_094` (32 bits), `.flags_098` (7 bits) | `CHARACTER_OPTION_*` (move.h), the struct's bitfields (`fixed_layers`, `can_poo`, `can_zap`, `dont_draw_rider`, ...) | the character config keyword handlers in charconfig.c that set each bitfield; byte tests read the bitfield |
| `PART.active`, bit 0x1 | `PART_IN_USE` (game_obj.h) | `AddPart` takes a slot without it and sets it, `KillPart` clears it; the other bits (0x2-0x20000) are runtime state set in parts.c |
| `AIPATHCNX.traversal_flags` (top 5 bits) | `AIPATHCNX_*` (aisys.h) | the AI editor's connection types and labels (`aieditor_RegisterPathCnxType`: Permanent Block, Temporary Block, Link Obstacle); `AIPATHCNX_HIDDEN` set while an end node's special is hidden, `AIPATHCNX_TOO_LONG` while the connection is longer than `max_horizontal_distance` |
| `pushblock_s.packed_state_flags` (7 bits) | `PUSHBLOCK_*` (legoapi_types.h) | the struct's bitfield names `push_visible`, `push_active` and `progress_state`; the axis and direction bits `edpush_Load` reads, from the pushes `NearestFacingPushBlock` refuses with each |
| `gameobject_s.context_variant_flags` and its byte `context_flags` (2 bits) | `APIOBJECT_CONTEXT_EVENT_DONE`, `APIOBJECT_CONTEXT_INVULNERABLE` (apiobject.h) | 0x40 is cleared when a context starts and set when its action happens, in every context (lever pulled, signal sent, blowup carried or transformed, detonator released, lunge landed); 0x20000 together with the context info's 0x8000000 makes hits do nothing (fighting, bolts, sabre sparks), set while the batarang is steered |
| `GameCameraMakeMiniCut3` flags, `MINICAM_COMMAND.type`, the mini camera's `mode` and `easing` | `MINICUT_*`, `MINICAM_COMMAND_*`, `MINICAM_MODE_*`, `MINICAM_EASING_*` (minicamcut.h) | the DynamicCameraCut AI action's keywords (range=, rotx=, cont_roty=, start_time=, attach_to_spline=, mode=follow, slowfastslow, ...) and what MiniCam's command loop does with each command |
| `PC_FILEEXTINFO.type`, `.platform` | `NUFILEEXT_*`, `NUFILEEXT_PLATFORM_*` (nufile.h) | the extensions `NuFileExtInit` registers per kind and platform (.an3, .ghg, _pc.dds, .wav, .bik; _ps3, _360, .vag, .dsp, .at3) |
| `ShaderParserNode.kind`, shader declaration flags | `SHADER_NODE_*`, `SHADER_DECL_*` (CRC16.h) | the node type each ShaderParser*Pc constructor stores; the nu2_use/nu2_declare storage keywords (vs_local, fs_local, varying, attribute, uniform, shared) and the vreg/freg bits of `ShaderDeclarationFlags` |
| `ObjHitObj` flags, `BOLT.hit_flags` (6 bits) | `OBJHIT_*` (hits.h) | `ObjHitObj_Flags`: the attacker's side (SetSide keywords) and whether it is an active player; players' bolts may hit gizmos (0x800, `Bolt_HitPlatPrivate`) |
| `eduiitem_s.type` | `EDUIITEM_*` (edui.h) | the type each `eduiItem*Create` constructor stores |
| `AIEDITORSETTINGS.display_options` (8 bits) | `AIEDITOR_*` (aisysall.c) | the AI editor menu toggles and their labels (Draw All Paths, Snap Height, Stop Platforms, ...) |
| `NuMusic::Voice.status` | `NUMUSIC_VOICE_*` (numusic.h) | the `NuMusicVoiceStatusText` names (IDLE_PENDING ... PLAYING) |
| `gamecamera_s.mode`, `previous_mode`, `LEGOCAMMODE_DOORCUT`/`_OBSTACLE` | `GAMECAM_MODE_*` (legoapi_types.h) | the camera update's choice of camera for each condition (sock system, obstacle spline, door cut, ViewCam pad, shop, minikit viewer, the mission_cam, char_cam and point_cam level splines); the game sets `LEGOCAMMODE_DOORCUT` 4 and `LEGOCAMMODE_OBSTACLE` 2 |
| `NuQFntMode`, the print mode stack | `NUQFNT_MODE_*` (nuqfnt.h) | NuQFntPrint2d*/Set*2d push 2 (no material stream), NuQFntPrint3D* push 4 (full y scale); with 1 every digit is as wide as '0' |
| `GAMEPAD.runtime_flags` (5 bits) | `GAMEPAD_AI_*`, `GAMEPAD_WAGGLE`, `GAMEPAD_MAGNITUDE_CHANGED` (move.h) | the AI script actions that set them (AttackOpponent 0x2, UseForce and HelpWithTriggers 0x4, ForcePush 0x10); 0x8 is `MOVEPLAYER_PAD_BITS.waggle` from `GamePad_Waggle`; 0x40 is set when the input magnitude changed |
| `gizforce_s.runtime_flags` (16 bits) | `GIZFORCE_*` (legoapi_types.h) | the bit-field's names (enabled, visible, reward and completion released, destroyed, being used) and the setters in gizforce.c (`GizmoForce_ActivateRev` reversed, the group stack, the camera clip test, the platform search, the post-load blowup name, the shake timer) |
| `AIPATHCNX.traversal_flags`, low bits | `STARWARS_`, `BATMAN_`, `INDY_AIPATHCNX_*` (aisys.h) | the values each game's `*_GameAISysInit` assigns to the `LEGO_AIPATHCNX_*` variables; the AI code of starwars_gameai.c, batarang.cpp (`Batman_*`) and hub.cpp (`Indy_*`) tests those of its own game |
| `PC_CONTROL_ITEM.flags` (4 bits) | `PC_CONTROL_ITEM_*` (apitext.h) | the bit-field's names (unbound for player 1, player 2, the selected player) and 0x8 making keyboard ports unbound |
| `PC_INPUT_DEVICE.kind` | `PC_INPUT_DEVICE_*` (PCInput.h) | `InitKeyboardProfiles` names devices 10-13 KeyboardMouse1, Keyboard1, KeyboardMouse2, Keyboard2 with kinds 1, 3, 2, 4; DirectInput joysticks get 5 (their DIDEVCAPS), XInput pads 6 |
| `grabber_s.flags` | `GRABBER_*` (grabber.h) | the grabbers config keywords rotate and shadow, and the bit-field (lifted, switch latched); 0x1 is the draw result |
| `FLOWBOX.state_flags` (9 bits) | `FLOWBOX_*` (gizflow.h) | the flow config's gizmo keywords (StartInvisible, FinishedInvisible, FinishedDeactive, Reverse, RevInvis, OutputOnly, NotFreeplay, NotStoryMode) packed by `GizFlowConfigGizmo`; 0x1 is the bit-field's enabled |
| `gameanimset_s.flags` | `GAMEANIMSET_PLAYING`, `_STOPPED`, `_IN_ACTIVE_LIST` (legoapi_types.h) | `GameAnimSys_Update` sets 0x1 while an object plays, `GameAnimSet_Stop` sets 0x2, `GameAnimSet_AddToSystemList` 0x4 |
| `gizobstacle_s.runtime_flags` (7 bits), `.progress_flags` (4 bits) | `GIZOBSTACLE_*`, `OBSTACLE_*` (legoapi_types.h) | the bits saved in the level progress (pickups spawned, trigger latched, destroyed, active, visible), the AI locks (AlwaysTriggerObstacle and ObstacleLockedOpen 0x4, CanTriggerObstacle and ObstacleLockedShut 0x8), `GizObstacle_SetTechnoControlled` and `_SetPushControlled` |
| `STATUSPACKET_s.flags_word`, `.unknown_b0` (11 bits) | `STATUSPACKET_*` (legoapi_types.h) | the bit-fields' names, set from the area flags (vehicle, bonus, super bonus) and the globals BuildUpDone, BonusWinner, FreePlay, SuperStory, PauseSaveAndExit |
| `gameobject_s.tag_flags` (3 bits) | `APIOBJECT_TAG_*` (apiobject.h) | the SetTaggable action sets 0x2; the tagging code sets 0x4 with `pending_tag_target` and 0x8 when the pending tag is a take-over |
| `EdRef.attributes` (2 bits) | `EDREF_ATTR_POINTER`, `EDREF_ATTR_CLASS` (edlevelall.h) | the `EdRef` constructor: a type name ending in '*', a name that is a class and not a type |
| `PART.flags`, `ADDPART.flags`, the part type's flags | `PART_FLAG_*` (game_obj.h) | the part editor's Instance Flags, Debris Settings and Emitter Timing toggles and their labels (parts.c, edpart.c) |
| `CUT_LOAD_RECORD.flags` (15 bits) | `CUTINFO_*` (cutscenes.h) | the cutscene config keywords (no_fog, draw_world, snap_out, cam_only, looping, in_game, level_intro, drawgizmosys, ...) |
| `sfx_record_s.sample_id_flags` (5 bits) | `SFX_*` (gameliball.h) | the sound config's Sample keywords loop, global, disable, comment, nofade |
| `PC_SHADER_FILTER_DESC_VIEW.layer_flags` (26 bits) | `PC_SHADER_LAYER_*` (ShaderManager.h) | the shader definitions each bit selects in shader_compiler.cpp (SURFACE_NORMAL_MAP, LIGHTING_GOOCH, WIND, REFRACTION_GLASS, LEGO, PRELIGHT_FX, ...) and `UnfilteredHasLayer` (layers 1-3) |
| `TEXTURE_ANIM_PROGRAM.instructions`, the btex/untiltex comparisons | `TEXANIM_OP_*`, `TEXANIM_CMP_*` (nutexanm.h) | the opcode each script keyword handler writes (tex, texr, wait, goto, gosub, btex, repeat, untiltex, end, xref, ...) and `TextureAnimationReadComparisonPrivate` |
| `ShaderSourceNode.semantic` | `SHADER_SEMANTIC_*` (CRC16.h) | the semantic_* keywords of `ShaderDeclarationSemantic` and the names `SemanticName` prints |
| `NuInitHardware` tags | `NUHW_*` (pcapi.h) | what the tag's case reads (hostfs, file mode, dat names, pads, audio, config name, save folder, window, icon, ...) |
| `gameobject_s.context_variant_flags` in the jump, punch and sabre combo contexts | `APIOBJECT_JUMP_*`, `APIOBJECT_PUNCH_*`, `APIOBJECT_COMBO_ENDED`/`_WAITING` (apiobject.h) | the starts that set them: StartJump in movement states 3/4, Slam_Start and StartLunge (attack), StartEndOfJump and the combat roll and super carry timers (falling), the second jump (double), StartComboJump's ACTION_FLIP (flip); PunchCode's ACTION_PUNCH_BEHIND, SpecialMove_Check and combo style; LightSabreComboCode's input window and end. Written only in jumping.c, carrying.c and those move.c functions, since the bits mean other things in other contexts |
| UI button sets: `CombineUiButtons:buttons`, `edui_cursor_buttons*`, `ProcessFileSel3:buttons` | `PAD_BUTTON_*` (edui.h) | the keys merged into pad buttons (Return and the left mouse button as CROSS, Escape as CIRCLE, the arrow keys and wheel as the d-pad) and the file selector's `FS_GetPadWithRepeat`, which returns `digital_buttons` |
| `GIZMOBLOWUP.draw_flags` (22 bits), `GizmoBlowUp_AddEffects` flags, `GIZMOBLOWUP.status_flags` 0x8000 | `BLOWUP_OPTION_*`, `BLOWUP_EFFECT_*`, `BLOWUP_STATUS_DRAWN` (gizmoblowups.h) | what the game does with each option: the batarang's and the transform's target searches, the terrain platform, character and vehicle smashing (the proximity hit), ShadowMode, the carried-only hit, decal, GizmoBlowupCreateStuff, the mid point at the origin, bolts, shadow, the shake effect, the transform following the animation, the reflection and the fragments of every hit, the power-up (the pickup call), the rider-only bolts, BlowupExFunc, the animated special's timing, the torpedo target and the shared platform instance; the effect bits by what `GizmoBlowUp_AddEffects` adds for each; DRAWN is the result of `NuSpecialDrawAt` |
| `gameobject_s.flags_e24` (16 bits) | `APIOBJECT_GOT_VICTIM` ... `APIOBJECT_DETONATOR_THROW` (apiobject.h) | the setters: the lunge eating its target (`Condition_GotVictim`), `CableTargetGameObject`, the teleport wait, the action button's short press, `Grapple_MaximumLength`, a punch that hit, the panel and hat machine contexts, magnet climbing, the detonator's release frame; the turn away from a boundary (TurnCode waits), the turn asked for in a loop, the locator matrices of this frame (cleared before the draw, read only with them), the forced fall landing and its copy, the context starts that block JumpCode, the frame in which bolts pass through (`Bolt_HitGameObjects`) |
| `GIZSPINNER_s.state_flags`, `GIZSPINNER_s.flags` | `SPINNER_STATE_*` (legoapi_types.h), `GIZSPINNER_FLAG_*` | GizSpinner_Push, _Spin, _Update and _Reset: the direction and side options, the push and stop state, the nudge at an end, the judder; the loader's cleared runtime bits; `GizSpinner_Activate`/`SetVisibility` and the update for the other set |
| `NUHGOBJ_JOINT_OVERRIDE.flags`, the animation node flags (`ANI3_BUFFER.joint_flags`, `NUANIM_LEGACY_SET.flags`) | `NUJOINT_OVERRIDE_*`, `ANI3_NODE_*` (nu3d_includes.h) | what the joint evaluation adds and limits for each bit; the node bits by `CurveGroupMasks` (translation 2, rotation 1, scale 8) and the matrix build |
| `TERRAIN_SURFACE.flags` (12 bits) | `TERSURFACE_*` (legoapi_types.h) | what the game does on the surface: `TerrainKillPlayer`, the reflections, `SURFACEBITS_DUST`, `Conveyor_AdjustSpeed`, `CanObjSlide`, the bolt deflection, `CanClimbSurface`, push block sliding; 0x8, 0x10, 0x40, 0x200, 0x2000 are left |
| `CHARACTERANIM_CONFIG.flags` 0x1, 0x4, 0x8000; `CHARCONFIG.flags` | `CHARANIM_AREA`, `CHARANIM_LOAD`, `CHARANIM_NO_LOAD`; `CHARCONFIG_*` (charconfig.c) | the area and character animation loads, the anim_start default, `AnimList_NoLoad`; the config keywords that set the reader's bits (bsa_default, turn_rate, turn_rate_2, timebaseupdate1-4) and the layer, animation and permanent setup |
| `GIZMO_LEVER.flags`, `HATMACHINE.flags` | `LEVER_FLAG_*` (lever.h), `HATMACHINE_FLAG_*` (hatmachine.cpp) | the lever's bitfield names in lever.c; the hat machine's visible and enabled bitfields, its use, gizmo output and the level file's hidden byte |
| `GIZTURRET_s.behavior_flags`, `GIZTURRET_s.flags` | `TURRET_*`, `TURRET_FLAG_*` (legoapi_types.h) | what the turret update, target choice, hit and draw code do with each option; the runtime bits by the struct's bitfield names. 0x8, 0x10 and 0x8000 of the options are left |
| `gameobject_s.action_movement_state` in the jump context | `JUMP_KIND_*` (apiobject.h) | the starts that set it: JumpCode's flip, StartComboJump, StartLunge, Slam_Start, the zip-up and pushing jumps, the combat roll; written by hand only where the jump context is tested or set, since the force pushed and deactivated contexts use the field for other values (zapped 2, choked 3, mind trick 1) |
| ObjHitObj flags 0x40-0x200, `EXPLOSION.flags`, `GIZMOBLOWUP.draw_flags` 0x10000000 | `OBJHIT_PUNCH`/`_BOLT`/`_SABRE`/`_EXPLOSION` (hits.h), `EXPLOSION_*` (legoapi_types.h), `BLOWUP_OPTION_KNOCKBACK` | the callers that add them (Punch_Hit, every bolt, the sabre, UpdateExplosion) and what ObjHitObj does with each; what UpdateExplosion does with each explosion bit (the blowup hit types it passes are BLOWUP_HIT_*) |
| `SUIT_LAYERS.flags`, `SUIT_INITIALIZER_RECORD.flags` | `SUIT_*` (suit.h) | `SuitRecordInit`: each record's text id (tSHADOWSUIT, tGLIDESUIT, ...) names its suit; 0x100 is left |
| `gizforce_s.config_flags` (11 bits) | `GIZFORCE_CONFIG_*` (legoapi_types.h) | what the force update, draw, use and line-of-sight code does with each option; 0x2 is left |
| `AIGROUP.flags` | `AIGROUP_*` (aisys.h) | `CreateAIGroup`, the group update (all dead), `AIMoveInstruction` and `FormationMove`; 0x10 is left |
| `cut_load_locator_type_s.flags`, `cut_scene_load_s.flags` (3 bits) | `CUTLOCATOR_*`, `CUTSCENE_*` (gcutscn.h) | the effect each locator type drives in `CutSceneLocatorFixUp` and the locator update; the scene's stream buffers, sound request and pending stream part |
| `gizobstacle_s.config_flags` (9 bits) | `GIZOBSTACLE_CONFIG_*` (legoapi_types.h) | what `GizObstacleUpdate_Proximity` and `GizObstacle_SatisfyingTerrainChecks` do with each option; 0x40-0x200, 0x800, 0x1000 are left |
| `bolt_s.flags`, set when shot or deflected; `GIZTURRET_s.behavior_flags` 0x8000 | `BOLT_PLAYER_1` ... `BOLT_DEFLECTED` (bolts.h), `TURRET_SHOOTS_OBSTRUCTIONS` | `Bolt_Add` (the owner's player number, an active player's bolts kept by `Bolt_Alloc` and the draw), the 0x13 test for obstruction hits that the turret option and the Kamino guns join, the deflected copy that gets no double damage |
| `PART.active`, the runtime bits besides `PART_IN_USE` (11 bits) | `PART_STATE_*` (game_obj.h) | `AddPart` (lighting data or scale, the scale-up), `CastPart` (impact pending, the scale phases), the part update (stopped, below the cutoff height), the draw (drawn this frame), the thermal detonator's beep, water and reflection; 0x10, 0x80, 0x400, 0x800 and 0x1000 are left |
| `bolt_s.flags`, `BOLTTYPE_s.flags_60` (7 bits) | `BOLT_NO_TERRAIN` ... `BOLT_SINGLE_DEBRIS` (bolts.h) | the bolt type keywords of `BoltType_ConfigKeywords` (no_terrain, canonlyhitplayers, nodeflect, converge, no_collide, trooper_bolt, single_debris) |
| `game_message_s.flags`, `add_game_message_s.flags` (19 bits) | `GAMEMSG_*` (gizmopickup.h) | what `UpdateGameMessages`, `TransformGameMessages` and `DrawGameMessages` do with each bit (world position and scale, the moves and their easing curves, the fades, the object alpha, the slot and text handling) |
| `gameanimset_s.state` | `GAMEANIMSET_STATE_*` (legoapi_types.h) | `GameAnimSet_EvaluateState` and `GameAnimSys_Update`: at start, playing forward, at end, playing backward, stopped part way |
| `ShaderLexicalClasses` | `SHADER_CHAR_*` (CRC16.h) | the characters `ShaderLexicalClassesInit` puts in each class |
| `AIPACKET.creature_index`, `.current_route` 0xff, `EDPP_PARTICLE.instance_id` 99999 | `AIPACKET_NO_CREATURE`, `AIPACKET_NO_ROUTE`, `EDPP_SHELVED` | the values that mean none (tested before the index is used); `edppPtlShelve` frees the debris instance and keeps the placement |
| `NuCameraClipTestPoints` codes | `NUCAMERA_CLIP_*` (nucamera.h) | the side of the view volume each bit is set for |
| `NuSpecialFind`, `NuSpecialFindDisplay`, `NuSpecialGetFirst` last parameter | renamed `report_missing` | a boolean: nonzero reports a missing special through `PCSpecialEmptyHandler` (its 0 and 1 arguments are not flags) |
| `ShaderCompilerParser::ParseDeclaration` and `ProcessDirective` states, `ShaderParserDeclaration.type` | `SHADER_PARSE_*`, `SHADER_DIRECTIVE_*` (shader_parser.h), `SHADER_TYPE_*` (CRC16.h) | the token each parser state expects; the type keywords of nu2_declare (float ... samplerCUBE, hook) |
| `PCDeviceSetRenderState`, `PCDeviceSetSamplerState`, `PCDeviceSetTextureStageState`, `PCDeviceSetTransform` states | `D3DRS_*`, `D3DSAMP_*`, `D3DTSS_*`, `D3DTS_*` (d3dCalls.h) | the Direct3D 9 SDK values (d3d9types.h), which the device methods take unchanged |
| `cut_script_node_s.type` | `CUTSCRIPT_*` (gcutscn.h) | the cut scene script keywords of `CutScriptParseBlock` |
| NetworkObjectManager message types | `NETOBJ_MSG_*` (gamelib_util_types.h) | the `Receive*Message` handler `NetworkObjectManager::Receive` calls for each type and the `Send*Message` that writes it |
| `SPECIAL_RESOURCE_EVENT.flags` (3 bits) | `SPECIALSFX_EVENT_*` (gameliball.h) | `UpdateSpecialSfx`: a looping sound (`IsSfxLooping` at load), played when the animation passes the trigger frame forwards or backwards |
| `cheat_s.flags`, `cheatsystem_s.flags` (18 bits) | `CHEAT_*` (cheat.h) | the code that tests each bit (`Player_HasInvincibility`, `Player_HasFastBuild`, `Player_HasDeflectBolts`, `Player_HasDoubleWeaponDamage`, `Player_HasDoubleBoltDamage_FromBolt`, `Cheat_MultiplyScore`, the zero lighting, `RegenerateHearts`, the coin magnet, `Cheats_SetFlags` and `Cheats_TurnOff` for the power-up bits) |
| shader uniform semantics (`PC_SHADER_PARAMETER_LOCAL.semantic`, the `semantic` parameters of `NuShaderManagerSet*fv`, `NuShaderObjectSetElementsfv` and `ShaderManagerHLSL`) | `SHADER_UNIFORM_*` (ShaderManager.h) | the record of `ShaderUniformRecords` the value indexes (its vertex or fragment uniform name); the material update's cases agree (bitangentFlip from flip_sign, waterTable, the layer samplers, carpaint_tints) |
| `animation` locals of `IsAFallAnim`, the `Animate_*` functions, `ShieldCode`, `GetDefaultIdle`, `ResetCharacterIdle`, `TerrainPlayer` | `ACTION_*` (character.h) | they hold the animation packet's requested animation |
| `ConfigureJoystickRanges` axis offsets | `offsetof(PC_DI_JOYSTATE2, ...)` (pc_input.cpp) | DIPROPRANGE by offset (DIPH_BYOFFSET) into DirectInput's DIJOYSTATE2 |

## Still literal (meaning not known yet)

| Field | Uses | Values | What is known, where to look |
|---|---:|---|---|
| `STATUS_STAGE_s.step`, other stages | ~10 | 1-6 | the collect characters stage uses step as a 1-based character index; `Prompt_Alt_Draw` has a case 2 no update sets |
| `STATUS_STAGE_s.type`, 5-7, 24, 27-34 | ~15 | | the LevelComplete and BonusTime stages and the Stage5/6 ones of the alternate table; their text ids are unnamed globals |
| `gameobject_s.action_movement_state`, other contexts | ~15 | 1-4 | the force pushed context (2 zapped by lightning, 3 choked, 4 the push cheat) and the deactivated one (2 zapped, 1 mind trick); jump kinds 5 and 7 |
| `AIPACKET.path_connection_state`, other handlers | ~20 | 0..2 | the tube, door and lift connection handlers count their own steps |
| `gameobject_s.unknown_7b3` | 57 | 0..7 | a per-context sub-state; named in the super carry and whip contexts, the others (zip-up, grapple, ledge, jump, batarang) mostly use 0 and 1 |
| `gameobject_s.flags_1fc`, other bits | 45 | 0x4, 0x10, 0x20, 0x40, 0x100, 0x400, 0x2000, 0x10000, 0x40000 | 0x4 is set by the player movement update under many conditions, 0x20 when riding a bike (with 0x400 for a cheap zip), 0x2000 when a respawn position is saved off a path; not settled |
| `gameobject_s.flags_1f8`, other bits | ~70 | 0x2, 0x40, 0x80, 0x100, 0x200, 0x800, 0x1000, 0x4000, 0x20000, 0x40000 | 0x40000 is tested by the pause menu, the network camera and the game update; the sibling build's `APIOBJECT_STATE_FLAG_IGNORE_DOORS` (transitional/unused.h) is unverified |
| `gameobject_s.flags_e20`, other bits | ~130 | 0x20 (tested to skip an object almost everywhere, cleared by `UpdateGameObjects`; its setter is not in the source yet), 0x1000 (`GizPanel_MoveCode`), 0x1-0x8, 0x4000, 0x20000, 0x80000-0x80000000 | runtime state bits set in code; read each setter |
| `gameobject_s.context_variant_flags`, other bits | ~60 | 0x8-0x1000 | the jump, punch and combo bits are named; left: 0x100 in the whip, zip-up and dying contexts (the grapple's is `APIOBJECT_GRAPPLE_CLIMB`; the teleport's 0x400 is `APIOBJECT_TELEPORT_REVERSE`), 0x20 (security door, zip-up), 0x1000 (jump, sets flags_e24 0x8000), 0x8 and 0x10 (the combo's second event frame) |
| `gameobject_s.flags_ef0`, `flags_ef4`, `portal_flags`, other bits | 30-50 each | bit sets | runtime state bits set in code (`AttackOpponent` sets portal 0x20 and 0x400), no keywords; the sibling build's names are guesses |
| `GIZMOBLOWUP.status_flags`, other bits | ~60 | 0x400, 0x1000, 0x10000-0x400000, 0x1000000, 0x2000000 | 0x400 and 0x1000 are hit types 10 and 12; 0x10000 makes `GizmoBlowupEarlyUpdate` recompute the mid point; 0x20000 is the decal after the blow up; 0x80000 waits for the animation's end frame |
| `GIZMOBLOWUP.draw_flags`, other bits | ~5 | 0x2000, 0x4000, 0x10000, 0x10000000 | 0x10000 lets a hit of type 0 through `GizmoBlowUp_Hit`, 0x10000000 adds explosion flag 0x8, 0x2000 with SHARED_PLATFORM chooses the platform instance, 0x4000 is passed to `GameAntinode_RegisterAntiNodeUsingData` |
| `gameobject_s.flags_e24`, other bits | ~2 | 0x70000 (a 3-bit vehicle hit flicker) | `SetFlicker` clears it, the vehicle movement reads it as a value; its setter is not in the source |
| `bolt_s.flags`, other bits | ~40 | 0x200, 0x1000, 0x20000, 0x100000, 0x800000, 0x4000000, ... | 0x200 makes a one-point bolt (its collision radius is the full radius); 0x1000 orients the effect along the bolt |
| cheat flags, other bits | ~6 | 0x2000, 0x40000, 0x100000, 0x200, 0x400000 | 0x200 and 0x40000 turn on the two pickup detectors (`detectors` bits 1 and 2), 0x100000 a deflect for active players in `Bolt_HitGameObjects`, 0x400000 a force speed; the cheat config is not among the retail files |
| cheat indices (`Cheat_IsOn`) | 57 | 0..0x2c | come from the game's cheat config file, which is not among the retail files here |
| `AIPACKET.flags`, other bits | ~100 | 0x1-0x80000000 | path following and movement state set in `AISysProcessCharacter`, `AIMoveToDestination`, `AISysCharacterMovement`; no keyword |
| cut scene instance `flags_88`, other bits | ~6 | 0x40000, 0x3e000 (the 5-bit repeat_count) | 0x40000 makes the camera, character and rigid systems run with the frame counted from the end; its setter is not in the source |
| `NuSound3PlayStereoV` tags | ~30 | 0..12 | each tag fills one `PC_STEREO_PLAY_PACKET` field; tags 9, 11 and 12 set fields whose meaning is not settled (`option9_mode`, `option11`) |
| `mode`, `state`, `type`, `kind`, `index` (locals and parameters) | 50-150 each | | many unrelated locals share these names; name per function where the callee says what it takes |
| `pushblock_s.packed_state_flags`, other bits | ~50 | 0x1-0x800, 0x38000 (a 3-bit snap index), 0x4000000, 0x8000000, 0x10000000-0x40000000 | 0x4000000, 0x8000000, 0x20000000 and 0x40000000 are editor booleans `edpush_Load` reads without labels; the low bits are push and snap state in `UpdatePushBlocks` |

## Every field with 20 or more literal sites

Counted like the magic-state and magic-flag classes of `oddscan.py --magic` (a
comparison, assignment or `case` with a value other than 0, 1 and -1, or a
one-bit mask), per field. On 15 November 2026 the named constants stood at
25827 sites and the literals at 7999 (76.4 % named); the 63 fields below hold
2533 of the literals, and 1670 more fields have fewer than 20 sites each (5466
in all; `oddscan.py --magic` lists them by use).

| Field | Sites | Values | Where | What is known |
|---|---:|---|---|---|
| `flags` | 149 | 0x0 0x1 0x2 0x3 0x4 0x8 0x10 0x17 ... | nu2api/nu3d_includes.cpp, gamelib/net/NetworkObjectManager.cpp | bare `flags` locals and parameters of many functions; `magicnames.py --locals flags` lists them per function (GizSpinner_*, ObjHitObj, NuAnimBuff*, UpdateGameMessages) |
| `*.flags` | 143 | 0x1 0x2 0x3 0x4 0x8 0x10 0x11 0x20 ... | gameapi/aisysall.c, nu2api/nu3d_includes.cpp | `.flags` members whose record the scan cannot resolve (unions, views, casts); name them through their record |
| `gameobject_s.flags_e20` | 135 | 0x1 0x2 0x4 0x8 0x20 0x1000 0x4000 0x20000 ... | legogame/move.c, legogame/creature.c | runtime bits; 0x20 is tested to skip an object almost everywhere and cleared by UpdateGameObjects, its setter is not in the source yet |
| `mode` | 111 | -1 0 1 2 3 4 5 6 ... | nu2api/nucore_includes.c, nu2api/nu3d_platform.cpp | bare `mode` locals and parameters; NuQFntPushPrintMode/NuQFntSetCoordinateSystem modes, EdFileOpen and NuFileOpenDF open modes |
| `gameobject_s.flags_ef0` | 93 | 0x4 0x40 0x80 0x200 0x400 0x1000 0x2000 0x4000 ... | legogame/creature.c, legogame/move.c | runtime bits set in code, no keyword |
| `AIPACKET.flags` | 78 | 0x8 0x20 0x40 0x400 0x800 0x1000 0x2000 0x4000 ... | gameapi/aisysall.c, legogame/gameai.c | path following and movement state (AISysProcessCharacter, AIMoveToDestination) |
| `type` | 73 | 0 1 2 3 4 5 6 7 ... | nu2api/nu3d_includes.cpp, nu2api/nupad_text.cpp | bare `type` locals and parameters; per function in `--locals type` (CutScriptNodeAlloc, TEXT_INPUT_MANAGER::ButtonName, ShaderCompilerParser::ParseDeclaration) |
| `nurnd_vertex3d_s.colour` | 72 | 32768 842150450 4278190080 4278190335 4278255360 4294901760 4294967295 | legoapi/parts.c, gameapi/edbri.c | the editors' debug line colours: 0xffffffff, 0xff00ff00 and 0xff0000ff repeat, but the vertex colour's channel order (which of 0xff0000ff and 0xffff0000 is red) is not settled, so they stay hex |
| `gameobject_s.flags_1f8` | 70 | 0x2 0x40 0x80 0x100 0x200 0x800 0x1000 0x4000 ... | gameapi/apiobject.c, legogame/creature.c | 0x40000 tested by the pause menu, network camera and game update |
| `gameobject_s.portal_flags` | 56 | 0x1 0x4 0x20 0x400 0x800 0x1000 0x8000 0x10000 ... | legogame/gameaiscript.c, legogame/game_obj.c | runtime bits (AttackOpponent sets 0x20 and 0x400) |
| `count` | 51 | -2 1 2 3 4 5 6 8 ... | legogame/hub.cpp, nu2api/shader_dictionary.cpp | no name table, keyword or label found yet |
| `parameter` | 49 | 0 1 2 3 4 5 6 7 ... | nu2api/nu3d_includes.cpp, legoapi/lighting_unit.cpp | no name table, keyword or label found yet |
| `pushblock_s.packed_state_flags` | 49 | 0x1 0x2 0x4 0x8 0x20 0x40 0x80 0x100 ... | legoapi/pushblocks.cpp | push and snap state in UpdatePushBlocks; 0x4000000, 0x8000000, 0x20000000, 0x40000000 are unlabelled editor booleans |
| `result` | 44 | 1 2 3 4 5 6 7 8 ... | nu2api/nushadermanager.cpp, legogame/game_obj.c | no name table, keyword or label found yet |
| `gameobject_s.flags_ef4` | 42 | 0x4 0x40 0x100 0x200 0x4000 0x8000 0x10000 0x100000 ... | legogame/creature.c, legogame/move.c | runtime bits set in code |
| `nudisplaylistitem_s.id` | 41 | 0 1 3 4 | nu2api/nu3d_includes.cpp, nu2api/nu3d_platform.cpp | no name table, keyword or label found yet |
| `GIZMOBLOWUP.status_flags` | 40 | 0x400 0x1000 0x10000 0x20000 0x40000 0x80000 0x100000 0x400000 ... | legoapi/gizmoblowups.c, legoapi/episodeV.c | 0x400 and 0x1000 are hit types 10 and 12; 0x80000 waits for the animation end frame |
| `colours` | 40 | 8421504 2147483648 2147483776 2147532800 2155839359 2155872256 2155905152 2164260863 | legoapi/objectsall.c, legoapi/screen.cpp | no name table, keyword or label found yet |
| `*.device_id` | 39 | 243 335 353 354 355 479 545 785 ... | nu2api/hardware_dispatch.cpp, nu2api/input_runtime.cpp | no name table, keyword or label found yet |
| `mask` | 38 | 0x1 0x2 0x4 0x8 0x10 0x20 0x40 0x80 ... | nu2api/nu3d_includes.cpp, nu2api/pc_input.cpp | no name table, keyword or label found yet |
| `output` | 37 | 0 1 2 3 4 | legoapi/gizobstacle.c, legoapi/gizturret.cpp | no name table, keyword or label found yet |
| `selector` | 37 | 5 9 19 20 21 25 27 30 ... | nu2api/hardware_dispatch.cpp | the `NuInitHardware` tags whose values the function skips or keeps in unnamed `parameter_*` locals; the tags it uses are `NUHW_*` |
| `*.kind` | 35 | 0 1 2 3 4 5 6 7 ... | nu2api/input_runtime.cpp, nu2api/nu3d_platform.cpp | some of its sites already use SHADER_NODE_* (anonymous enum/#define) names |
| `gameobject_s.context_variant_flags` | 34 | 0x8 0x10 0x20 0x100 0x800 0x1000 0x2000 0x8000 | legogame/move.c, legogame/zipup.c | per character context: 0x8000 the jump hold time ran out / the carry throw timer ended, 0x100 grapple, whip and zip-up, 0x80 and 0x1-0x10 sabre combo |
| `kind` | 34 | 1 2 3 4 5 6 7 10 ... | nu2api/input_runtime.cpp, nu2api/pc_input.cpp | no name table, keyword or label found yet |
| `value` | 34 | -32768 -32767 1 6 7 10 12 13 ... | nu2api/input_runtime.cpp, legogame/gizpanel.c | no name table, keyword or label found yet |
| `*.capabilities` | 33 | 0x1 0x2 0x4 0x8 0x10 0x20 0x80 0x100 ... | legoapi/batarang.cpp, legogame/starwars_gameai.c | no name table, keyword or label found yet |
| `id` | 33 | -3 0 2 3 8 9 10 11 ... | nu2api/shader_compiler.cpp, legogame/hub.cpp | no name table, keyword or label found yet |
| `axis` | 31 | 0 1 2 3 4 5 6 7 | editor/edlevelall.cpp, legoapi/utilities.c | no name table, keyword or label found yet |
| `PC_SHADER_FILTER_DESC_VIEW.flags` | 30 | 0x1 0x2 0x8 0x20 0x80 0x100 0x400 0x4000 ... | nu2api/nushadermanager.cpp, nu2api/shader_compiler.cpp | no name table, keyword or label found yet |
| `eduiitem_s.data` | 30 | 0 1 2 3 7 9999 99999 | gameapi/aisysall.c, gameapi/edpp_load.c | some of its sites already use DEBRIS_GENERATOR_* (anonymous enum/#define) names |
| `gameobject_s.flags_1fc_word` | 30 | 0x4 0x10 0x20 0x40 0x100 0x2000 0x10000 0x40000 | legogame/creature.c, legogame/game_obj.c | some of its sites already use APIOBJECT_FLAG_* (anonymous enum/#define) names |
| `animpacket_s.flags` | 29 | 0x1 0x2 0x4 0x8 0x10 0x20 0x40 0x80 | gameapi/apiobject.c, legogame/move.c | no name table, keyword or label found yet |
| `gameobject_s.unknown_283` | 29 | 3 5 6 7 9 16 | legogame/move.c, legogame/creature.c | no name table, keyword or label found yet |
| `JEDIB_STATE.phase` | 28 | 1 2 3 4 5 6 7 | legogame/episodeII.c | no name table, keyword or label found yet |
| `*.type` | 27 | 0 1 2 3 4 12 13 14 ... | nu2api/nu3d_platform.cpp, gameapi/aisysall.c | some of its sites already use NUDLIST_* (anonymous enum/#define) names |
| `source` | 27 | 1 2 128 129 130 133 159 188 ... | legoapi/text.c, nu2api/nucore_includes.c | no name table, keyword or label found yet |
| `alignment` | 25 | 1 2 4 8 16 32 64 | legoapi/text.c, legogame/panel.cpp | no name table, keyword or label found yet |
| `index` | 25 | 0 1 2 3 4 8 15 32 ... | nu2api/nu3d_includes.cpp, nu2api/nurndr.c | some of its sites already use ACTION_STUN_* (anonymous enum/#define) names |
| `models` | 25 | 12 13 89 101 102 103 104 105 ... | legogame/creature.c | no name table, keyword or label found yet |
| `*.aspect_mode` | 24 | 0 1 2 3 | nu2api/pc_device.cpp, legogame/camera.c | no name table, keyword or label found yet |
| `NewPrim` | 24 | 2 3 | editor/edlevelall.cpp | no name table, keyword or label found yet |
| `debris` | 24 | 34 38 44 45 52 69 73 74 ... | legogame/game_obj.c, legogame/game.cpp | no name table, keyword or label found yet |
| `gameobject_s.movement_mode` | 24 | 0 1 2 3 4 5 | legogame/move.c, legogame/creature.c | no name table, keyword or label found yet |
| `gizmofns_s.options` | 24 | 8 12 24 32 40 44 48 64 ... | legoapi/gizbuildit.cpp, legoapi/gizmoblowups.c | no name table, keyword or label found yet |
| `panel_movement_s.flags` | 24 | 0x1 0x2 0x4 0x8 0x10 0x20 0x40 0x80 | legogame/gizpanel.c, legogame/gameaiscript.c | no name table, keyword or label found yet |
| `starfighter_s.model_id` | 24 | -307 -300 -299 -298 -297 54 | legogame/chris.cpp | no name table, keyword or label found yet |
| `state` | 24 | 2 3 4 5 6 7 8 9 | legogame/hub.cpp, legoapi/jumping.c | bare `state` locals; ShaderCompilerParser::ParseDeclaration and ProcessDirective are token-driven parser states (one per expected token) |
| `eduiitem_s.flags` | 23 | 0x1 0x2 0x4 | gameapi/aisysall.c, gameapi/rtl.c | some of its sites already use EDUI_ITEM_* (anonymous enum/#define) names |
| `*.flags_low` | 22 | 0x1 0x80 0x100 0x200 0x400 0x4000 0x100000 | gameapi/apiobject.c, gameapi/aisysall.c | no name table, keyword or label found yet |
| `APIOBJECT.flags_low` | 22 | 0x1 0x4 0x80 0x100 0x800 0x1000 0x40000 0x80000 | gameapi/aisysall.c, legogame/gameai.c | no name table, keyword or label found yet |
| `green` | 22 | 31 63 143 191 223 255 65280 | legogame/panel.cpp, legogame/menu.cpp | no name table, keyword or label found yet |
| `menu` | 22 | 8 12 13 14 15 16 17 | legogame/dropinout.c, legogame/panel.cpp | no name table, keyword or label found yet |
| `panel_movement_s.model_variant` | 22 | 0 1 2 3 | legogame/gizpanel.c | no name table, keyword or label found yet |
| `buildit_reset_s.state_flags` | 21 | 0x1 0x2 0x4 0x8 0x10 0x40 0x80 0x100 ... | legoapi/gizbuildit.cpp | some of its sites already use GIZBUILDIT_STATE_* (anonymous enum/#define) names |
| `model` | 21 | -307 14 25 53 54 84 85 86 ... | legogame/chris.cpp, legogame/creature.c | no name table, keyword or label found yet |
| `*.active` | 20 | 2 3 5 6 7 9 | nu2api/input_runtime.cpp, legoapi/debris_engine.c | some of its sites already use PART_IN_* (anonymous enum/#define) names |
| `buildit_reset_s.availability_flags` | 20 | 0x1 0x2 0x4 0x80 0x200 | legoapi/gizbuildit.cpp | no name table, keyword or label found yet |
| `format` | 20 | 0 1 2 3 4 5 6 7 ... | nu2api/nu3d_platform.cpp, nu2api/nu3d_includes.cpp | no name table, keyword or label found yet |
| `gameobject_s.action_movement_state` | 20 | 2 3 4 5 7 | legogame/move.c, legoapi/jumping.c | jump and movement mode of the jump code (JumpCode states 2-9) |
| `hit_type` | 20 | 2 3 7 8 9 10 12 | legoapi/gizmoblowups.c, legoapi/explosions.c | some of its sites already use BLOWUP_HIT_* (anonymous enum/#define) names |
| `red` | 20 | 63 191 255 | legogame/menu.cpp, legogame/panel.cpp | no name table, keyword or label found yet |
| `version` | 20 | 1 2 3 5 6 11 18 19 ... | legoapi/specialsfx.c, nu2api/nushadermanager.cpp | no name table, keyword or label found yet |
