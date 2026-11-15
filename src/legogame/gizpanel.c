#include "batman.h"
#include "gameapi/edptl.h"
#include "gameapi/edtools/edfile.h"
#include "globals.h"
#include "legoapi/actions/movement/climbing.h"
#include "legoapi/ai/core/gameai.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/characters/motion.h"
#include "legoapi/characters/motion/gameanim.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/core/input/gamepads.h"
#include "legoapi/core/input/timer.h"
#include "legoapi/gizmo/base/gizmo.h"
#include "legoapi/gizmos/object/gizpanel.h"
#include "legoapi/items/base/animpacket.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/menus/core/hint.h"
#include "legoapi/menus/screens/gamestructure.h"
#include "legoapi/misc/gamemain.h"
#include "legoapi/misc/supportall.h"
#include "legoapi/misc/utilities.h"
#include "legoapi/render/core/gameliball.h"
#include "legoapi/render/core/terrain.h"
#include "legoapi/render/light/shadow.h"
#include "legoapi/text_tables.h"
#include "legoapi/world/area.h"
#include "legoapi/world/level.h"
#include "legoapi/world/levels.h"
#include "legoapi/world/mission.h"
#include "legoapi/world/world.h"
#include "legoapi/world/world_shared.h"
#include "legogame/game.h"
#include "legogame/gamestatus.h"
#include "legogame/startup.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nucore/NuMotionAccumFilter.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/numusic/sfx.h"

typedef struct registry_panel_record_pc_s {
    u8 storage[0x90];
} REGISTRY_PANEL_RECORD;

typedef struct registry_panel_system_pc_s {
    i32 count;
    REGISTRY_PANEL_RECORD *panels;
} REGISTRY_PANEL_SYSTEM;

typedef struct {
    u32 triggered[1];
    u32 enabled[1];
    u32 visible[1];
} PANEL_PROGRESS;

typedef struct panel_movement_s {
    NUMTX matrix;
    char name[16];
    NUVEC position;
    u16 y_rotation;
    u8 unknown_5e[3];
    u8 model_variant;
    u8 flags;
    u8 draw_flags;
    NUVEC floor_position;
    NUVEC target_offset;
    u16 unknown_7c;
    u16 unknown_7e;
    u16 arm_x_rotation;
    u16 target_pitch;
    u16 target_roll;
    i16 platform_id;
    f32 activation_time;
    f32 target_scale;
} PANEL_MOVEMENT;

typedef struct panel_load_system_s {
    i32 count;
    PANEL_MOVEMENT *panels;
} PANEL_SYSTEM;

extern const f64 GameObjectNearFloorNoFloorHeight;
extern const f64 PanelVariantTwoTargetY;
extern const f64 PanelVariantTwoTargetZ;
extern const f64 PanelVariantThreeTargetY;
extern const f64 PanelVariantThreeTargetZ;
extern f32 GizPanelPictureY;
extern f32 GizPanelPictureZ;
extern f32 GizPanelFloorLift;
extern u8 PanelStormActive;
extern u8 PanelDroidActive;
extern u8 PanelBountyHunterActive;
void SetProtocolDroidInterfaceAction(GAMEOBJECT *object);

/* Hint updater records keep their packed field offsets. */
struct panel_movement_s;

/* Terrain helper private to these callers (panel arrives in ESI). */
static void GizPanel_CreateTerrain(PANEL_MOVEMENT *panel) {
    if (panel == NULL) {
        return;
    }
    if (panel->platform_id != -1) {
        DeletePlatinst(panel->platform_id);
    }
    switch (panel->model_variant) {
        case 0:
            panel->platform_id = NewPlatPickupInst(panel, 4);
            break;
        case 1:
            panel->platform_id = NewPlatPickupInst(panel, 7);
            break;
        case 2:
            panel->platform_id = NewPlatPickupInst(panel, 5);
            break;
        case 3:
            panel->platform_id = NewPlatPickupInst(panel, 6);
            break;
        default:
            break;
    }
    PlatInstRotate(panel->platform_id, 1);
}

void GizPanel_InitTerrain(WORLDINFO *world) {
    i32 index;
    PANEL_SYSTEM *system = (PANEL_SYSTEM *)world->giz_panel_sys;
    if (system == NULL) {
        return;
    }
    for (index = 0; index < ((PANEL_SYSTEM *)world->giz_panel_sys)->count; ++index) {
        ((PANEL_SYSTEM *)world->giz_panel_sys)->panels[index].platform_id = -1;
        GizPanel_CreateTerrain(&((PANEL_SYSTEM *)world->giz_panel_sys)->panels[index]);
    }
}

static NUVEC *PanelGetTrackingPosition(PANEL_MOVEMENT *panel, NUVEC *position) {
    *position = panel->position;
    switch (panel->model_variant) {
        case 3:
            position->y = (f32)((f64)position->y + PanelVariantThreeTargetY);
            position->z = (f32)((f64)position->z - PanelVariantThreeTargetZ);
            break;
        case 2:
            position->y = (f32)((f64)position->y + PanelVariantTwoTargetY);
            position->z = (f32)((f64)position->z - PanelVariantTwoTargetZ);
            break;
    }
    return position;
}

i32 GizPanel_Load(void *world_pointer, void *unused) {
    typedef union PanelLoadFlags { // size 0x1
        u8 value;

        struct {
            u8 unknown_0 : 5;
            u8 flag_5 : 1;
            u8 flag_6 : 1;
            u8 flag_7 : 1;
        } bits;
    } PanelLoadFlags;

    typedef union PanelLoadDrawFlags { // size 0x1
        u8 value;

        struct {
            u8 flag_0 : 1;
            u8 flag_1 : 1;
            u8 flag_2 : 1;
            u8 unknown_3 : 5;
        } bits;
    } PanelLoadDrawFlags;

    typedef struct PanelLoadState { // size 0x64
        u8 unknown_00[0x62];
        PanelLoadFlags flags;          // 0x62
        PanelLoadDrawFlags draw_flags; // 0x63
    } PanelLoadState;

    WORLDINFO *world = world_pointer;
    PANEL_SYSTEM *system;
    i32 version;
    i32 index;
    i32 length;
    u8 value;
    (void)unused;
    if (world == NULL || world->giz_panel_sys == NULL) {
        return 0;
    }
    system = (PANEL_SYSTEM *)world->giz_panel_sys;
    if (system->count != 0) {
        return 0;
    }
    version = EdFileReadInt();
    system->count = EdFileReadInt();
    for (index = 0; index < system->count; ++index) {
        length = EdFileReadInt();
        EdFileRead(system->panels[index].name, length);
        EdFileReadNuVec(&system->panels[index].position);
        system->panels[index].y_rotation = EdFileReadUnsignedShort();
        value = (u8)EdFileReadChar();
        system->panels[index].model_variant = value;
        if (version >= 3) {
            value = (u8)EdFileReadChar();
            ((PanelLoadState *)&system->panels[index])->flags.bits.flag_5 = value & 1;
        }
        if (version >= 4) {
            EdFileReadNuVec(&system->panels[index].target_offset);
            system->panels[index].target_scale = EdFileReadFloat();
        } else {
            system->panels[index].target_offset.y = 0.0f;
            system->panels[index].target_offset.x = 0.0f;
            system->panels[index].target_offset.z = -0.4f;
            system->panels[index].target_scale = 1.0f;
        }
        if (version >= 5) {
            value = (u8)EdFileReadChar();
            ((PanelLoadState *)&system->panels[index])->flags.bits.flag_6 = value & 1;
        }
        if (version >= 6) {
            value = (u8)EdFileReadChar();
            ((PanelLoadState *)&system->panels[index])->flags.bits.flag_7 = value & 1;
            value = (u8)EdFileReadChar();
            ((PanelLoadState *)&system->panels[index])->draw_flags.bits.flag_0 = value & 1;
        }
        if (version >= 7) {
            value = (u8)EdFileReadChar();
            ((PanelLoadState *)&system->panels[index])->draw_flags.bits.flag_1 = value & 1;
        }
        if (version >= 8) {
            value = (u8)EdFileReadChar();
            ((PanelLoadState *)&system->panels[index])->draw_flags.bits.flag_2 = value != 0;
        } else {
            ((PanelLoadState *)&system->panels[index])->draw_flags.bits.flag_2 = 0;
        }
    }
    return 1;
}

void *GizPanels_ReserveBufferSpace(void *pointer) {
    WORLDINFO *world = pointer;
    REGISTRY_PANEL_SYSTEM *system;
    world->giz_panel_sys = NULL;
    if (world->current_level->max_levers > 0) {
        world->giz_buffer.addr = (world->giz_buffer.addr + 15) & ~15u;
        world->giz_panel_sys = world->giz_buffer.void_ptr;
        world->giz_buffer.addr += sizeof(REGISTRY_PANEL_SYSTEM);
        memset(world->giz_panel_sys, 0, sizeof(REGISTRY_PANEL_SYSTEM));
        world->giz_buffer.addr = (world->giz_buffer.addr + 15) & ~15u;
        system = (REGISTRY_PANEL_SYSTEM *)world->giz_panel_sys;
        system->panels = world->giz_buffer.void_ptr;
        world->giz_buffer.addr += world->current_level->max_gizpanels * sizeof(REGISTRY_PANEL_RECORD);
        memset(((REGISTRY_PANEL_SYSTEM *)world->giz_panel_sys)->panels, 0,
               world->current_level->max_gizpanels * sizeof(REGISTRY_PANEL_RECORD));
    }
    return world->giz_panel_sys;
}

void GizPanel_SetVisibility(GIZMO_PC *gizmo, i32 visible) {
    typedef union PanelVisibleFlags { // size 0x1
        u8 value;

        struct {
            u8 unknown_0 : 2;
            u8 visible : 1;
            u8 unknown_3 : 5;
        } bits;
    } PanelVisibleFlags;

    typedef struct PanelVisibleState {
        u8 unknown_00[0x62];
        PanelVisibleFlags flags; // 0x62
    } PanelVisibleState;

    PANEL_MOVEMENT *panel;
    PanelVisibleState *state;
    i32 was_visible;
    if (gizmo == NULL || gizmo->object == NULL) {
        return;
    }
    panel = gizmo->object;
    state = (PanelVisibleState *)panel;
    was_visible = state->flags.bits.visible;
    state->flags.bits.visible = visible != 0;
    if (state->flags.bits.visible && !was_visible) {
        GizPanel_CreateTerrain(panel);
    } else if (!state->flags.bits.visible && was_visible) {
        DeletePlatinst(panel->platform_id);
    }
}

void GizPanel_GetAbsTargetPos(void *context, NUVEC *target, i32 player_position) {
    PANEL_MOVEMENT *panel = context;
    NUVEC offset;
    if (panel == NULL || target == NULL) {
        return;
    }
    if (player_position != 0 && panel->model_variant == 0) {
        offset.x = -0.04f;
        offset.y = panel->target_offset.y;
        offset.z = -0.3f;
    } else if (player_position != 0 && panel->model_variant == 1) {
        offset.x = 0.035f;
        offset.y = panel->target_offset.y;
        offset.z = -0.25f;
    } else {
        offset = panel->target_offset;
    }
    NuVecRotateY(&offset, &offset, panel->y_rotation);
    offset.x += panel->position.x;
    offset.z += panel->position.z;
    *target = offset;
}

i32 GizPanel_CanUsePanel(GAMEOBJECT *object, PANEL_MOVEMENT *panel) {
    if (panel == NULL || object == NULL) {
        return 0;
    }
    if (SuperWeirdo(object) != 0) {
        return 1;
    }

    switch (panel->model_variant) {
        case 0:
            if ((object->character_data->model_flags & CHARACTER_MODEL_ASTROMECH) != 0 ||
                (object->character_data->model_flags & (CHARACTER_MODEL_DROID | CHARACTER_MODEL_BOUNTY_HUNTER)) ==
                    (CHARACTER_MODEL_DROID | CHARACTER_MODEL_BOUNTY_HUNTER)) {
                return 1;
            }
            break;
        case 1:
            if ((object->character_data->model_flags & CHARACTER_MODEL_PROTOCOL) != 0 ||
                (object->character_data->model_flags & (CHARACTER_MODEL_DROID | CHARACTER_MODEL_BOUNTY_HUNTER)) ==
                    (CHARACTER_MODEL_DROID | CHARACTER_MODEL_BOUNTY_HUNTER)) {
                return 1;
            }
            break;
        case 3:
            if (object->helmet == 5 || (object->character_data->game_character->flags_090 & 0x40000) != 0) {
                return 1;
            }
            if (GCDataList[(i16)object->id].weapon_action == 1 || GCDataList[(i16)object->id].weapon_action == 8) {
                return 1;
            }
            if ((i16)object->id == dat_007f199c || (i16)object->id == id_THEEMPEROR || (i16)object->id == dat_007f1a4c ||
                (i16)object->id == dat_007f1a48 || (i16)object->id == dat_007f1a44) {
                return 1;
            }
            break;
        case 2:
            if ((object->character_data->model_flags & CHARACTER_MODEL_BOUNTY_HUNTER) != 0 || object->helmet == 6) {
                return 1;
            }
            break;
    }
    return 0;
}

PANEL_MOVEMENT *GizPanel_FindByName(WORLDINFO *world, char *name) {
    PANEL_MOVEMENT *panel;
    i32 index;
    if (world == NULL || world->giz_panel_sys == NULL) {
        return NULL;
    }
    panel = ((PANEL_SYSTEM *)world->giz_panel_sys)->panels;
    for (index = 0; index < ((PANEL_SYSTEM *)world->giz_panel_sys)->count; index++, panel++) {
        if (NuStrICmp(panel->name, name) == 0) {
            return panel;
        }
    }
    return NULL;
}

PANEL_MOVEMENT *GizPanel_FindNearest(WORLDINFO *world, NUVEC *position, GAMEOBJECT *object, f32 *distance_squared, i32 check_eligibility) {
    PANEL_SYSTEM *system;
    PANEL_MOVEMENT *nearest = NULL;
    f32 nearest_distance = 1.0e9f;
    i32 index;
    if (world == NULL || world->giz_panel_sys == NULL) {
        return NULL;
    }
    system = (PANEL_SYSTEM *)world->giz_panel_sys;
    for (index = 0; index < system->count; ++index, system = (PANEL_SYSTEM *)world->giz_panel_sys) {
        PANEL_MOVEMENT *panel = &system->panels[index];
        f32 distance;
        if (object != NULL) {
            NUVEC target;
            if ((panel->flags & 8) == 0 || (panel->flags & 4) == 0 || (panel->flags & 3) != 0 || panel->floor_position.y == 2000000.0f) {
                continue;
            }
            if (check_eligibility != 0 && GizPanel_CanUsePanel(object, panel) == 0) {
                continue;
            }
            GizPanel_GetAbsTargetPos(panel, &target, 0);
            distance = NuVecDistSqr(position, &target, NULL);
        } else {
            distance = NuVecDistSqr(position, &panel->position, NULL);
        }
        if (distance < nearest_distance) {
            nearest_distance = distance;
            nearest = panel;
        }
    }
    if (distance_squared != NULL) {
        *distance_squared = nearest_distance;
    }
    return nearest;
}

void GizPanel_Update(void *world_pointer, void *unused, f32 delta_time) {
    WORLDINFO *world = world_pointer;
    PANEL_SYSTEM *system = (PANEL_SYSTEM *)world->giz_panel_sys;
    GAMEOBJECT *nearest_player = NULL;
    NUVEC nearest_direction;
    i32 index;
    (void)unused;
    if (system == NULL) {
        return;
    }
    for (index = 0; index < system->count; ++index) {
        PANEL_MOVEMENT *panel = &system->panels[index];
        if ((panel->flags & 8) != 0) {
            f32 nearest_distance = 1000000000.0f;
            f32 maximum_distance;
            i32 player_index;
            nearest_player = NULL;
            maximum_distance = (panel->flags & 0x10) != 0 ? 1.21f : 0.80999994f;
            for (player_index = 0; player_index < 8; ++player_index) {
                GAMEOBJECT *player = Player[player_index];
                if (player != NULL && (player->flags_1fc_word & APIOBJECT_FLAG_CHARACTER) != 0 && player->dead == 0) {
                    NUVEC position;
                    NUVEC direction;
                    f32 distance;
                    PanelGetTrackingPosition(panel, &position);
                    distance = NuVecDistSqr(&position, &player->collision_position, &direction);
                    if (distance < nearest_distance && distance < maximum_distance) {
                        nearest_distance = distance;
                        nearest_direction = direction;
                        nearest_player = Player[player_index];
                    }
                }
            }
        }
        if (nearest_player != NULL) {
            u16 previous_x = panel->unknown_7c;
            u16 previous_y = panel->unknown_7e;
            u16 target_x;
            u16 target_y;
            FindAnglesXY(&nearest_direction, &target_x, &target_y);
            target_y -= panel->y_rotation;
            if (previous_y <= 0x4000 && target_y > 0x4000 && target_y < 0xc000) {
                target_y = 0x4000;
            } else if (previous_y >= 0xc000 && target_y > 0x4000 && target_y < 0xc000) {
                target_y = 0xc000;
            }
            if (previous_x <= 0x4000 && target_x > 0x4000 && target_x < 0xc000) {
                target_x = (u16)(panel->y_rotation + 0x4000);
            } else if (previous_x >= 0xc000 && target_x > 0x4000 && target_x < 0xc000) {
                target_x = 0xc000;
            }
            panel->unknown_7c = SeekRot(panel->unknown_7c, target_x, 5.0f);
            panel->unknown_7e = SeekRot(panel->unknown_7e, target_y, 5.0f);
            if ((panel->flags & 0x10) == 0) {
                panel->activation_time = 0.0f;
                panel->flags |= 0x10;
                PlaySfx("DPanelAct", &panel->position);
            } else {
                f32 time = panel->activation_time + FRAMETIME;
                panel->activation_time = time;
                if ((f64)time > (f64)0.8f) {
                    panel->activation_time = 0.8f;
                }
            }
        } else {
            panel->unknown_7c = SeekRot(panel->unknown_7c, 0xe000, 10.0f);
            panel->unknown_7e = SeekRot(panel->unknown_7e, 0, 10.0f);
            if ((panel->flags & 0x10) != 0) {
                PlaySfx("DPanelDis", &panel->position);
            }
            panel->flags &= (u8)~0x10;
            if (panel->activation_time > 0.0f) {
                f32 time = panel->activation_time - delta_time;
                panel->activation_time = time;
                if (time < 0.0f) {
                    panel->activation_time = 0.0f;
                }
            }
        }
    }
}

/* Picture Z and Y offsets. */
f32 GizPanelPictureZ = -0.123f;
f32 GizPanelPictureY = 0.0775f;
/* Lift of the panel target above the floor it was dropped onto. */
f32 GizPanelFloorLift = 0.001f;
i32 gizpanel_gizmotype_id = -1;

void GizPanel_Draw(void *world_pointer, void *unused, f32 unused_time) {
    WORLDINFO *world = world_pointer;
    PANEL_SYSTEM *system;
    u16 target_spin;
    f32 target_alpha;
    NUMTX base_matrix;
    i32 index;
    (void)unused;
    (void)unused_time;
    if (world == NULL) {
        return;
    }
    system = (PANEL_SYSTEM *)world->giz_panel_sys;
    if (system == NULL || system->count == 0) {
        return;
    }
    target_spin = (u16)(i32)((f64)NuFmod(GameTimer.time_elapsed, 5.0f) / 5.0 * 65536.0);
    target_alpha =
        (f64)NuTrigTable[((i32)((f64)NuFmod(GameTimer.time_elapsed_mod_seconds, 0.5f) * 2.0 * 65536.0) >> 1) & 0x7fff] * (f64)0.2f +
        (f64)0.8f;
    for (index = 0; index < system->count; ++index) {
        PANEL_MOVEMENT *panel = &system->panels[index];
        i32 base;
        i32 animated;
        i32 transition;
        i32 state_on;
        i32 state_off;
        i32 arm;
        i32 target;
        f32 arm_y;
        f32 arm_z;
        if ((panel->flags & 4) == 0 && editor_active == 0) {
            continue;
        }
        if ((panel->draw_flags & 2) == 0) {
            EnableShadowMapRendering(0);
        } else {
            ResetShadowMapRendering();
        }
        switch (panel->model_variant) {
            case 0:
                base = (panel->draw_flags & 1) | 0x106;
                animated = ((u32)panel->flags >> 7) | 0x108;
                transition = 0x10a;
                state_on = 0x10b;
                state_off = 0x10c;
                arm = -1;
                target = 0x55;
                arm_y = 0.0f;
                arm_z = 0.0f;
                break;
            case 1:
                base = (panel->draw_flags & 1) != 0 ? 0x107 : 0x100;
                animated = (panel->flags & 0x80) != 0 ? 0x102 : 0x101;
                transition = 0x103;
                state_on = 0x104;
                state_off = 0x105;
                arm = -1;
                target = 0x55;
                arm_y = 0.0f;
                arm_z = 0.0f;
                break;
            case 2:
                base = 0x10d;
                animated = 0x10e;
                transition = 0x10f;
                state_on = 0x110;
                state_off = 0x111;
                arm = 0x112;
                target = 0x113;
                arm_y = 0.245f;
                arm_z = -0.19f;
                break;
            case 3:
                base = 0x114;
                animated = 0x115;
                transition = 0x116;
                state_on = 0x117;
                state_off = 0x118;
                arm = 0x119;
                target = 0x11a;
                arm_y = 0.34f;
                arm_z = -0.17f;
                break;
            default:
                base = -1;
                animated = -1;
                transition = -1;
                state_on = -1;
                state_off = -1;
                arm = -1;
                target = -1;
                arm_y = 0.0f;
                arm_z = 0.0f;
                break;
        }
        NuMtxSetRotationY(&base_matrix, panel->y_rotation);
        NuMtxTranslate(&base_matrix, &panel->position);
        if ((panel->flags & 0x20) == 0 && base != -1 && world->level_objects[base].active != 0) {
            NuSpecialDrawAt(&world->level_objects[base].special, &base_matrix);
        }
        if ((panel->flags & 0x20) == 0) {
            if ((panel->flags & 2) != 0) {
                if (state_on != -1 && world->level_objects[state_on].active != 0) {
                    NuSpecialDrawAt(&world->level_objects[state_on].special, &base_matrix);
                }
            } else if (state_off != -1 && world->level_objects[state_off].active != 0) {
                NuSpecialDrawAt(&world->level_objects[state_off].special, &base_matrix);
            }
        }
        if ((panel->flags & 0x20) == 0 && (panel->activation_time > 0.0f || editor_active != 0)) {
            NUVEC picture_offset;
            NUVEC transition_offset;
            NUMTX transition_base;
            NUMTX animated_matrix;
            f32 frame;
            picture_offset.x = 0.0f;
            picture_offset.y = GizPanelPictureY;
            picture_offset.z = GizPanelPictureZ;
            transition_offset.x = 0.0f;
            transition_offset.y = GizPanelPictureY;
            transition_offset.z = (f64)GizPanelPictureZ - (f64)0.0005f;
            NuVecRotateY(&picture_offset, &picture_offset, panel->y_rotation);
            NuVecRotateY(&transition_offset, &transition_offset, panel->y_rotation);
            transition_base = base_matrix;
            NuMtxTranslate(&base_matrix, &picture_offset);
            NuMtxTranslate(&transition_base, &transition_offset);
            if (panel->activation_time < 1.2f && editor_active == 0) {
                f32 progress = panel->activation_time / 1.2f;
                f32 alpha;
                frame = panel->activation_time / 1.2f * 23.0 + 1.0;
                EvalAnim(&world->level_objects[transition].special, frame, &animated_matrix, 0);
                animated_matrix.m32 = 0.0f;
                animated_matrix.m31 = 0.0f;
                animated_matrix.m30 = 0.0f;
                NuMtxMulVU0(&animated_matrix, &animated_matrix, &transition_base);
                alpha = 1.0f - progress;
                NuSpecialDrawAtAlpha(&world->level_objects[transition].special, &animated_matrix, alpha);
                frame = (f64)panel->activation_time / (f64)0.8f * 24.0 + 1.0;
                EvalAnim(&world->level_objects[animated].special, frame, &animated_matrix, 0);
                animated_matrix.m32 = 0.0f;
                animated_matrix.m31 = 0.0f;
                animated_matrix.m30 = 0.0f;
                NuMtxMulVU0(&animated_matrix, &animated_matrix, &base_matrix);
                NuSpecialDrawAtAlpha(&world->level_objects[animated].special, &animated_matrix, 1.0f);
            } else {
                frame = (f64)panel->activation_time / (f64)0.8f * 24.0 + 1.0;
                EvalAnim(&world->level_objects[animated].special, frame, &animated_matrix, 0);
                animated_matrix.m32 = 0.0f;
                animated_matrix.m31 = 0.0f;
                animated_matrix.m30 = 0.0f;
                NuMtxMulVU0(&animated_matrix, &animated_matrix, &base_matrix);
                NuSpecialDrawAtAlpha(&world->level_objects[animated].special, &animated_matrix, 1.0f);
            }
        }
        if ((panel->flags & 0x40) == 0 && (panel->flags & 8) != 0 && (f64)panel->target_offset.y != GameObjectNearFloorNoFloorHeight &&
            target != -1 && world->level_objects[target].active != 0) {
            NUVEC target_position;
            GizPanel_GetAbsTargetPos(panel, &target_position, 0);
            NuMtxSetRotationY(&base_matrix, target_spin);
            if (panel->target_roll != 0) {
                NuMtxRotateZ(&base_matrix, panel->target_roll);
            }
            if (panel->target_pitch != 0) {
                NuMtxRotateX(&base_matrix, panel->target_pitch);
            }
            NuMtxTranslate(&base_matrix, &target_position);
            NuMtxPreScaleU(&base_matrix, panel->target_scale);
            NuSpecialDrawAtAlpha(&world->level_objects[target].special, &base_matrix, (panel->flags & 1) != 0 ? 0.0f : target_alpha);
        }
        if ((panel->flags & 0x20) == 0 && arm != -1 && world->level_objects[arm].active != 0) {
            NUVEC arm_offset;
            arm_offset.x = 0.0f;
            arm_offset.y = arm_y;
            arm_offset.z = arm_z;
            NuVecRotateX(&arm_offset, &arm_offset, panel->arm_x_rotation);
            NuVecRotateY(&arm_offset, &arm_offset, panel->y_rotation);
            NuMtxSetRotationX(&base_matrix, panel->arm_x_rotation);
            NuMtxRotateY(&base_matrix, panel->y_rotation);
            NuMtxPreRotateX(&base_matrix, panel->unknown_7c);
            NuMtxRotateY(&base_matrix, panel->unknown_7e);
            NuMtxTranslate(&base_matrix, &panel->position);
            NuMtxTranslate(&base_matrix, &arm_offset);
            NuSpecialDrawAt(&world->level_objects[arm].special, &base_matrix);
        }
    }
    ResetShadowMapRendering();
}

i32 GizPanel_GetMaxGizmos(void *world_ptr) {
    WORLDINFO *world = world_ptr;
    if (world != NULL) {
        return world->current_level->max_gizpanels;
    }
    return 0;
}

void GizPanel_AddGizmos(GIZMOSYS *giz, i32 type, void *world_ptr, void *data) {
    WORLDINFO *w = world_ptr;
    i32 i;
    (void)data;
    if (w == NULL || w->giz_panel_sys == NULL) {
        return;
    }
    for (i = 0; i < w->giz_panel_sys->count; ++i) {
        if (NuStrLen(((ADD_PANEL *)w->giz_panel_sys->panels)[i].name) != 0) {
            AddGizmo(giz, type, NULL, &((ADD_PANEL *)w->giz_panel_sys->panels)[i]);
        }
    }
}

char *GizPanel_GetGizmoName(void *input) {
    GIZMO_PC *gizmo = input;
    if (gizmo != NULL && gizmo->object != NULL) {
        return ((PANEL_MOVEMENT *)gizmo->object)->name;
    }
    return NULL;
}

i32 fun_0046eb50(PANEL_MOVEMENT *panel) {
    return panel->flags & 1;
}

i32 GizPanel_GetOutput(void *input, i32 output, i32 mode) {
    GIZMO_PC *gizmo = input;
    PANEL_MOVEMENT *panel = gizmo->object;
    (void)output;
    (void)mode;
    if ((panel->flags & 8) != 0 && (panel->flags & 4) != 0) {
        return (panel->flags >> 1) & 1;
    }
    return 0;
}

char *GizPanel_GetOutputName(void *unused, i32 output_index) {
    return output_index == 0 ? (char *)"Finished" : NULL;
}

i32 GizPanel_GetNumOutputs(void *unused) {
    (void)unused;
    return 1;
}

void *GizPanels_AllocateProgressData(void *buffer, void *end) {
    return GizmoBufferAlloc(buffer, end, 0xc);
}

void GizPanels_ClearProgress(void *owner, void *progress_ptr) {
    PANEL_PROGRESS *progress = progress_ptr;
    if (progress == NULL) {
        return;
    }
    progress->triggered[0] = 0;
    progress->enabled[0] = 0xffffffffu;
    progress->visible[0] = 0xffffffffu;
}

void GizPanels_StoreProgress(void *world_ptr, void *data, void *progress_ptr) {
    PANEL_PROGRESS *progress = progress_ptr;
    WORLDINFO *world = world_ptr;
    PANEL_PROGRESS_SYSTEM *system;
    PANEL_PROGRESS_RECORD *record;
    i32 index;
    i32 word;
    u32 bit;
    if (progress == NULL) {
        return;
    }
    progress->triggered[0] = 0;
    progress->enabled[0] = 0xffffffffu;
    progress->visible[0] = 0xffffffffu;

    if (world == NULL || world->giz_panel_sys == NULL || ((PANEL_PROGRESS_SYSTEM *)world->giz_panel_sys)->records == NULL) {
        return;
    }
    for (index = 0; index < ((PANEL_PROGRESS_SYSTEM *)world->giz_panel_sys)->count && index < 32; ++index) {
        system = (PANEL_PROGRESS_SYSTEM *)world->giz_panel_sys;
        record = &system->records[index];
        word = index / 32;
        bit = 1u << (index % 32);
        if (record->flags & 2) {
            progress->triggered[word] |= bit;
        }
        if (!(record->flags & 4)) {
            progress->visible[word] &= ~bit;
        }
        if (!(record->flags & 8)) {
            progress->enabled[word] &= ~bit;
        }
    }
}

void GizPanel_PlaySfx(char *name, NUVEC *position, i32 player_bits) {
    i16 sfx_id;
    if (name == NULL || position == NULL) {
        return;
    }
    sfx_id = (i16)GetSfxId(name);
    if (sfx_id == -1) {
        return;
    }
    if (player_bits != 0) {
        if ((player_bits & 1) != 0) {
            nusound_special_positions[1] = *position;
            PlaySfxById(sfx_id, &nusound_special_positions[1]);
            nusound_special_positions[1] = nusound_special_positions[0];
        }
        if ((player_bits & 2) != 0) {
            nusound_special_positions[2] = *position;
            PlaySfxById(sfx_id, &nusound_special_positions[2]);
            nusound_special_positions[2] = nusound_special_positions[0];
        }
    } else {
        PlaySfxById(sfx_id, position);
    }
}

void GizPanel_MoveCode(WORLDINFO *world, GAMEOBJECT *object, i32 use) {
    PANEL_MOVEMENT *panel;
    f32 distance;
    f32 radius;
    f32 radius_squared;
    (void)world;
    if (object->unknown_db0 > 0.0f) {
        object->unknown_db0 -= FRAMETIME;
    }
    PanelStormActive = 0;
    PanelDroidActive = 0;
    PanelBountyHunterActive = 0;
    if (object->character_context == CHARACTER_CONTEXT_INTERFACE && object->grapple != NULL) {
        if (object->character_model->model_data_b[object->unknown_7aa] == NULL ||
            AnimPlaying(&object->anim_packet, object->unknown_7aa, 1, 0) != NULL) {
            object->context_hover_target_y += FRAMETIME;
            if (object->context_hover_target_y > 1.0f) {
                object->context_hover_target_y = 1.0f;
            }
            object->context_timer -= FRAMETIME;
            if (object->context_timer <= 0.0f) {
                object->character_context = CHARACTER_CONTEXT_NONE;
                ((PANEL_MOVEMENT *)object->grapple)->flags &= (u8)~1;
                ((PANEL_MOVEMENT *)object->grapple)->flags |= 2;
                object->grapple = NULL;
            }
            if ((object->flags_e20 & 0x1000) == 0) {
                object->flags_e20 |= 0x1000;
            }
        } else {
            object->context_timer -= FRAMETIME;
            if (object->context_timer <= 0.0f) {
                if (object->grapple != NULL) {
                    NewBuzz(object->pad->bound_pad, 0.1f, 0);
                    if (((PANEL_MOVEMENT *)object->grapple)->model_variant == 2) {
                        PlaySfx("Hunter_Granted", &object->collision_position);
                    }
                    if (((PANEL_MOVEMENT *)object->grapple)->model_variant == 3) {
                        PlaySfx("Trooper_Granted", &object->collision_position);
                    }
                } else {
                    NewRumble(object->pad->bound_pad, 0.5f, 0);
                }
                if ((object->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0) {
                    u8 variant = ((PANEL_MOVEMENT *)object->grapple)->model_variant;
                    if (variant == 0 || variant == 1) {
                        if (StartupGameFlag1 != 0 && Mission_Active(NULL) != NULL) {
                            Hint_SetComplete(0x2b9);
                        } else {
                            Hint_SetComplete(0x25f);
                            LswForceUsedHintFlags |= 4;
                        }
                    } else if (variant == 3) {
                        Hint_SetComplete(0x260);
                    } else if (variant == 2) {
                        Hint_SetComplete(0x261);
                    }
                }
                object->character_context = CHARACTER_CONTEXT_NONE;
                ((PANEL_MOVEMENT *)object->grapple)->flags &= (u8)~1;
                ((PANEL_MOVEMENT *)object->grapple)->flags |= 2;
                object->grapple = NULL;
            } else if ((object->character_data->model_flags & CHARACTER_MODEL_PROTOCOL) != 0) {
                SetProtocolDroidInterfaceAction(object);
            }
        }
        return;
    }
    if (object->character_context != CHARACTER_CONTEXT_NONE && object->character_context != CHARACTER_CONTEXT_LAND_JUMP &&
        object->character_context != CHARACTER_CONTEXT_LAND_JUMP_2 && !objInNetWaitContext(object, 0x0b)) {
        return;
    }
    panel = GizPanel_FindNearest(WORLD, &object->position, object, &distance, 1);
    if (panel != NULL) {
        radius = (f32)((0.25 + object->bounds_radius) * panel->target_scale);
        radius_squared = radius * radius;
        if (!(distance < radius_squared) || (use == 0 && (object->panel_use_request != 1 || object->panel_request_data == NULL))) {
            return;
        }
        object->movement_facing_angle = panel->y_rotation;
        object->context_hover_target_y = 0.0f;
        object->delayed_turn_timer = 0.0f;
        object->flags_e20 &= ~0x1000u;
        object->character_context = CHARACTER_CONTEXT_INTERFACE;
        object->grapple = panel;
        object->unknown_7b3 = 0;
        FastWeaponIn(object, 0);
        object->flags_e24 |= 0x200;
        panel = object->grapple;
        if (panel->model_variant == 2) {
            if (object->character_model->model_data_b[0x46] != NULL) {
                object->unknown_7aa = ACTION_INTERFACE3;
            } else {
                object->unknown_7aa = ACTION_INTERFACE2;
            }
            Hint_SetComplete(0x261);
            Hint_SetComplete(0x26a);
        } else if (panel->model_variant == 3) {
            if (object->character_model->model_data_b[0x45] == NULL && object->character_model->model_data_b[0x46] != NULL) {
                object->unknown_7aa = ACTION_INTERFACE3;
            } else {
                object->unknown_7aa = ACTION_INTERFACE2;
            }
            MakeBaddiesForgetAboutParty(1);
            Hint_SetComplete(0x260);
            Hint_SetComplete(0x269);
        } else {
            object->unknown_7aa = ACTION_INTERFACE;
            if (panel->model_variant == 1) {
                if (!InStory() || (((PANEL_MOVEMENT *)object->grapple)->draw_flags & 4) == 0) {
                    GizPanel_PlaySfx("TC14_VLA", &object->collision_position, 1 << object->unknown_280);
                }
                if ((object->character_data->model_flags & CHARACTER_MODEL_PROTOCOL) != 0) {
                    SetProtocolDroidInterfaceAction(object);
                }
            } else {
                if (!InStory() || (((PANEL_MOVEMENT *)object->grapple)->draw_flags & 4) == 0) {
                    GizPanel_PlaySfx("R2D2_VLA", &object->collision_position, 1 << object->unknown_280);
                }
            }
            Hint_SetComplete(0x25f);
            LswForceUsedHintFlags |= 4;
        }
        {
            f32 duration = AnimDuration((i16)object->id, object->unknown_7aa, 0.0f, 0.0f, 1);
            object->context_timer = duration;
            if (duration <= 0.0f) {
                object->context_timer = 2.0f;
            }
            object->unknown_db0 = 0.0f;
        }
    } else if (use != 0 && (object->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0 &&
               (object->character_data->model_flags & CHARACTER_MODEL_PROTOCOL) != 0 && object->unknown_db0 <= 0.0f) {
        GizPanel_PlaySfx("TC14_VLN", &object->collision_position, 1 << object->unknown_280);
        object->unknown_db0 = 0.5f;
    }
}

i32 GizPanel_UpdateHint(HINT *hint) {
    u8 storm = 0;
    u8 droid = 0;
    u8 bountyhunter = 0;
    i32 index;
    if (WORLD->area != NULL && WORLD->area == HUB_ADATA) {
        return 0;
    }
    for (index = 0; index < 8; ++index) {
        GAMEOBJECT *object = Player[index];
        if (object != NULL) {
            f32 distance;
            HINT_PANEL *panel = (HINT_PANEL *)GizPanel_FindNearest(WORLD, &object->position, object, &distance, 0);
            if (panel != NULL && (panel->flags & 8) != 0 && (panel->flags & 3) == 0) {
                f32 radius = (f32)(((f64)Player[index]->bounds_radius + 1.0) * panel->target_scale);
                radius *= radius;
                if (distance < radius) {
                    switch (panel->model_variant) {
                        case 3:
                            storm = 1;
                            break;
                        case 0:
                        case 1:
                            droid = 1;
                            break;
                        case 2:
                            bountyhunter = 1;
                            break;
                    }
                }
            }
        }
    }
    if (storm != 0 && ((u16)hint->id == 0x260 || (u16)hint->id == 0x269)) {
        if (AvailableToPlayer(0, 1, 5, 0) != 0) {
            if ((u16)hint->id == 0x260) {
                return 1;
            }
        } else if ((u16)hint->id == 0x269) {
            return 1;
        }
        return 0;
    }
    if (bountyhunter != 0 && ((u16)hint->id == 0x261 || (u16)hint->id == 0x26a)) {
        if (AvailableToPlayer(CHARACTER_MODEL_BOUNTY_HUNTER, -1, 6, 0) != 0) {
            if ((u16)hint->id == 0x261) {
                return 1;
            }
        } else if ((u16)hint->id == 0x26a) {
            return 1;
        }
        return 0;
    }
    if (droid != 0 && (u16)hint->id == 0x25f) {
        if ((LswForceUsedHintFlags & 2) == 2) {
            return 1;
        }
        return 0;
    }
    return 0;
}

void GizPanel_Reset(void *context) {
    PANEL_MOVEMENT *panel = context;
    NUVEC offset;
    NUVEC probe_position;
    f32 height;
    panel->floor_position.y = 0.0f;
    panel->floor_position.x = 0.0f;
    panel->floor_position.z = 0.4f;
    NuVecRotateY(&panel->floor_position, &panel->floor_position, panel->y_rotation + 0x8000);
    NuVecAdd(&panel->floor_position, &panel->floor_position, &panel->position);
    offset = panel->target_offset;
    NuVecRotateY(&offset, &offset, panel->y_rotation);
    probe_position.x = panel->position.x + offset.x;
    probe_position.z = panel->position.z + offset.z;
    probe_position.y = panel->position.y;
    panel->floor_position.y = GameShadow(NULL, &panel->floor_position, 0.2f, -1);
    height = GameShadow(NULL, &probe_position, 0.2f, -1);
    panel->target_offset.y = height;
    if ((f64)height != GameObjectNearFloorNoFloorHeight) {
        panel->target_offset.y = height + GizPanelFloorLift;
        FindAnglesZX(&ShadNorm, &panel->target_pitch, &panel->target_roll);
    }
    panel->flags = (u8)((panel->flags & 0xfcu) | 0x0cu);
    panel->activation_time = 0.0f;
    panel->arm_x_rotation = 0;
    NuMtxSetRotationY(&panel->matrix, panel->y_rotation);
    NuMtxTranslate(&panel->matrix, &panel->position);
}

void GizPanels_Reset(void *world_pointer, void *unused, void *progress_pointer) {
    typedef union PanelProgressFlags { // size 0x1
        u8 value;

        struct {
            u8 unknown_0 : 2;
            u8 progress_2 : 1;
            u8 progress_3 : 1;
            u8 unknown_4 : 4;
        } bits;
    } PanelProgressFlags;

    typedef struct PanelProgressState { // size 0x63
        u8 unknown_00[0x62];
        PanelProgressFlags flags; // 0x62
    } PanelProgressState;

    WORLDINFO *world = world_pointer;
    PANEL_SYSTEM *system;
    PANEL_MOVEMENT *panel;
    u32 *progress = progress_pointer;
    i32 index;
    (void)unused;
    if (world == NULL) {
        return;
    }
    system = (PANEL_SYSTEM *)world->giz_panel_sys;
    if (system == NULL || system->panels == NULL) {
        return;
    }
    for (index = 0; index < system->count; ++index) {
        panel = &system->panels[index];
        GizPanel_Reset(panel);
        if (progress != NULL && index < 32) {
            PanelProgressState *state = (PanelProgressState *)panel;
            i32 word = index / 32;
            u32 bit = 1u << (index % 32);
            if (progress[word] & bit) {
                panel->flags |= 2;
            }
            state->flags.bits.progress_2 = (progress[word + 2] & bit) != 0;
            state->flags.bits.progress_3 = (progress[word + 1] & bit) != 0;
        }
    }
}

void fun_0046f6f0(PANEL_MOVEMENT *panel, NUVEC *position) {
    if (position != NULL) {
        if (panel != NULL && (panel->model_variant == 3 || panel->model_variant == 2)) {
            GizPanel_GetAbsTargetPos(panel, position, 0);
            return;
        }
        *position = panel->floor_position;
    }
}

void GizPanel_Activate(void *input, i32 active) {
    typedef union PanelActiveFlags { // size 0x1
        u8 value;

        struct {
            u8 unknown_0 : 3;
            u8 active : 1;
            u8 unknown_4 : 4;
        } bits;
    } PanelActiveFlags;

    typedef struct PanelActiveState { // size 0x63
        u8 unknown_00[0x62];
        PanelActiveFlags flags; // 0x62
    } PanelActiveState;

    GIZMO_PC *gizmo = input;
    PANEL_MOVEMENT *panel;
    if (gizmo != NULL && gizmo->object != NULL) {
        panel = gizmo->object;
        ((PanelActiveState *)panel)->flags.bits.active = active != 0;
        if (active != 0) {
            GizPanel_Reset(panel);
        }
    }
}

u8 PanelStormActive = 0;
u8 PanelDroidActive = 0;
u8 PanelBountyHunterActive = 0;
static ADDGIZMOTYPE GizmoRegistryGizPanelAddTypePc = {0};

ADDGIZMOTYPE *GizPanel_RegisterGizmo(i32 type_id) {
    ADDGIZMOTYPE *addtype = &GizmoRegistryGizPanelAddTypePc;
    *addtype = Default_ADDGIZMOTYPE;
    addtype->fns.early_update_fn = NULL;
    addtype->fns.panel_draw_fn = NULL;
    addtype->fns.activate_rev_fn = NULL;
    addtype->fns.get_visibility_fn = NULL;
    addtype->fns.get_pos_fn = NULL;
    addtype->fns.using_special_fn = NULL;
    addtype->fns.bolt_hit_plat_fn = NULL;
    addtype->fns.get_best_bolt_target_fn = NULL;
    addtype->fns.bolt_hit_fn = NULL;
    addtype->fns.post_load_fn = NULL;
    addtype->fns.add_level_sfx_fn = NULL;
    gizpanel_gizmotype_id = type_id;
    addtype->name = "Panel";
    addtype->prefix = "";
    addtype->fns.options = 0xc;
    addtype->fns.get_max_gizmos_fn = (i32(*)(void *))GizPanel_GetMaxGizmos;
    addtype->fns.add_gizmos_fn = (void *)GizPanel_AddGizmos;
    addtype->fns.late_update_fn = (void (*)(void *, void *, f32))GizPanel_Update;
    addtype->fns.draw_fn = (void (*)(void *, void *, f32))GizPanel_Draw;
    addtype->fns.get_gizmo_name_fn = (char *(*)(GIZMO_PC *))GizPanel_GetGizmoName;
    addtype->fns.get_output_fn = (i32(*)(GIZMO_PC *, i32, i32))GizPanel_GetOutput;
    addtype->fns.get_output_name_fn = (char *(*)(GIZMO_PC *, i32))GizPanel_GetOutputName;
    addtype->fns.get_num_outputs_fn = (void *)GizPanel_GetNumOutputs;
    addtype->fns.activate_fn = (void (*)(GIZMO_PC *, i32))GizPanel_Activate;
    addtype->fns.set_visibility_fn = (void (*)(GIZMO_PC *, i32))GizPanel_SetVisibility;
    addtype->fns.allocate_progress_data_fn = (void *(*)(VARIPTR *, VARIPTR *))GizPanels_AllocateProgressData;
    addtype->fns.clear_progress_fn = (void (*)(void *, void *))GizPanels_ClearProgress;
    addtype->fns.store_progress_fn = (void (*)(void *, void *, void *))GizPanels_StoreProgress;
    addtype->fns.reset_fn = (void (*)(void *, void *, void *))GizPanels_Reset;
    addtype->fns.reserve_buffer_space_fn = (void *(*)(void *))GizPanels_ReserveBufferSpace;
    addtype->fns.load_fn = (i32(*)(void *, void *))GizPanel_Load;
    return addtype;
}
