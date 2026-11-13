#include "saga.h"
#pragma bss_seg(".bss$splcodes")
#pragma bss_seg(".bss$splshelf")
/* Integrate these declarations once. Keep all existing canonical data owners. */
STATIC_ASSERT(offsetof(shopitem_s, item_id) == 0x60);
STATIC_ASSERT(offsetof(shopitem_s, type) == 0x62);
STATIC_ASSERT(offsetof(shopitem_s, unlocked) == 0x63);
STATIC_ASSERT(offsetof(shopitem_s, price) == 0x64);
STATIC_ASSERT(offsetof(shopitem_s, special) == 0x68);
STATIC_ASSERT(offsetof(COLLECTID, cheat_code) == 0xc);
STATIC_ASSERT(offsetof(COLLECTID, field3_0x4) == 4);

/* The retail shop code clamps through nested min/max macros: the inner min is evaluated twice. */
/* Place before Shop_DrawCharacter in shard.c; static native same-TU helpers. */

static i32 Signals_Load(void *world_info, void *unused1) {
    i32 version;
    i32 index;
    WORLDINFO *world = ((WORLDINFO *)(world_info));
    if (world->signal_count != 0) {
        return 0;
    }
    version = EdFileReadInt();
    world->signal_count = EdFileReadInt();
    for (index = 0; index < world->signal_count; ++index) {
        EdFileRead(world->signals[index].name, sizeof(world->signals[index].name));
        EdFileReadNuVec(&world->signals[index].position);
        world->signals[index].character_letter = EdFileReadChar();
        if (version > 1) {
            world->signals[index].suit_letter = EdFileReadChar();
            world->signals[index].yaw = EdFileReadShort();
        } else {
            world->signals[index].suit_letter = 'b';
            world->signals[index].yaw = 0;
        }
    }
    return 1;
}

static void Signals_Draw(void *world_info, void *unused1, float unused2) {
    i32 i;
    WORLDINFO *world = ((WORLDINFO *)(world_info));
    SIGNAL_s *signal = world->signals;
    for (i = 0; i < world->signal_count; i++, signal++) {
        NUMTX matrix;
        if (!signal->visible) {
            continue;
        }
        matrix = signal->matrix;
        if (world->level_objects[0x36].active) {
            NuSpecialDrawAt(&world->level_objects[0x36].special, &matrix);
        }
        if (signal->active) {
            if (world->level_objects[signal->animation_1].active) {
                NuSpecialDrawAt(&world->level_objects[signal->animation_1].special, &matrix);
            }
            if (world->level_objects[signal->animation_2].active) {
                f32 along_normal;
                f32 distance;
                f32 distance_squared;
                f32 alpha = signal->radius;
                NUVEC camera_offset;
                NuVecSub(&camera_offset, &GameCam->position, &signal->position);
                along_normal = NuVecDot(&camera_offset, &signal->normal);
                distance = NuVecMag(&camera_offset);
                distance_squared = distance * distance - along_normal * along_normal;
                if (distance_squared < 2.25f) {
                    f32 fade;
                    if (distance_squared <= 0.5625f) {
                        fade = 0.0f;
                    } else {
                        fade = (NuFsqrt(distance_squared) - 0.75f) / 0.75f;
                    }
                    if (along_normal < 0.0f || along_normal > 2.0f) {
                        if (along_normal >= 0.0f) {
                            along_normal -= 2.0f;
                        } else {
                            along_normal = -along_normal;
                        }
                        if (along_normal < 0.75f) {
                            fade += (1.0f - fade) * (along_normal / 0.75f);
                        } else {
                            fade = 1.0f;
                        }
                    }
                    alpha *= fade;
                }
                alpha *= signal->scale;
                if (alpha > 0.0f) {
                    NuSpecialDrawAtAlpha(&world->level_objects[signal->animation_2].special, &matrix, alpha);
                }
            }
        } else if (world->level_objects[signal->animation_0].active) {
            NuSpecialDrawAt(&world->level_objects[signal->animation_0].special, &matrix);
        }
    }
}

static void Signal_SetGizmoVisibility(GIZMO_PC *gizmo, i32 visible) {
    if (gizmo != NULL) {
        Signal_SetVisibility(((SIGNAL_s *)(gizmo->object)), visible);
    }
}

/* Native PC shop state machine. Child callbacks remain real unresolved native
 * references until their complete functions are ported; menu ID 13 stays unwired. */
typedef i32(__cdecl *SHOP_MENU_CALLBACK_PC)(void *);
typedef void(__cdecl *SHOP_DRAW_CALLBACK_PC)(void);

/* The retail move code is written out per shelf slot: any compiler-unrolled
 * loop in the function makes VC8 schedule the x87 operands differently. */

STATIC_ASSERT(offsetof(AREA_DATA_PC, index) == 0x7c);
STATIC_ASSERT(offsetof(LEVELDATA_PC, episode) == 0xa2);
STATIC_ASSERT(offsetof(LEVELDATA_PC, area_index) == 0xa3);

STATIC_ASSERT(offsetof(GAMESAVE, game_time) == 0x7c20);
STATIC_ASSERT(offsetof(GAMESAVE, area_save) == 0x782c);
STATIC_ASSERT(sizeof(AREA_SAVE_PICKUP_PC) == 0xc);
STATIC_ASSERT(offsetof(AREA_SAVE_PICKUP_PC, red_brick_collected) == 6);
