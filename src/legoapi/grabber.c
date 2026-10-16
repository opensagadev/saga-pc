#include "saga.h"

typedef struct grabber_progress_source_pc_s {
    u8 unknown_0000[0x484];
    f32 stored_tfactor;
    u8 unknown_0488[4];
    f32 stored_frame;
    u8 unknown_0490[4];
    f32 stored_parameter;
} GRABBER_PROGRESS_SOURCE_PC;
STATIC_ASSERT(offsetof(GRABBER_PROGRESS_SOURCE_PC, stored_tfactor) == 0x484);
STATIC_ASSERT(offsetof(GRABBER_PROGRESS_SOURCE_PC, stored_frame) == 0x48c);
STATIC_ASSERT(offsetof(GRABBER_PROGRESS_SOURCE_PC, stored_parameter) == 0x494);

typedef struct world_grabber_progress_pc_s {
    u8 unknown_0000[0x46ec];
    GRABBER_PROGRESS_SOURCE_PC *grabber;
} WORLD_GRABBER_PROGRESS_PC;
STATIC_ASSERT(offsetof(WORLD_GRABBER_PROGRESS_PC, grabber) == 0x46ec);

typedef struct level_grabber_progress_pc_s {
    u8 unknown_0000[0x2810];
    f32 stored_frame;
    f32 stored_tfactor;
    f32 stored_parameter;
} LEVEL_GRABBER_PROGRESS_PC;
STATIC_ASSERT(offsetof(LEVEL_GRABBER_PROGRESS_PC, stored_frame) == 0x2810);
STATIC_ASSERT(offsetof(LEVEL_GRABBER_PROGRESS_PC, stored_tfactor) == 0x2814);
STATIC_ASSERT(offsetof(LEVEL_GRABBER_PROGRESS_PC, stored_parameter) == 0x2818);

void __cdecl Grabber_StoreProgress(WORLDINFO *world, LEVEL_PROGRESS *progress) {
    if (world != NULL && progress != NULL) {
        GRABBER_PROGRESS_SOURCE_PC *grabber = ((WORLD_GRABBER_PROGRESS_PC *)world)->grabber;
        LEVEL_GRABBER_PROGRESS_PC *saved = (LEVEL_GRABBER_PROGRESS_PC *)progress;
        if (grabber != NULL) {
            saved->stored_frame = grabber->stored_frame;
            saved->stored_tfactor = grabber->stored_tfactor;
            saved->stored_parameter = grabber->stored_parameter;
        } else {
            saved->stored_tfactor = 2000000.0f;
        }
    }
}

typedef struct grabber_config_pc_s {
    NUMTX matrix;
    u8 unknown_040[0x480 - 0x40];
    NUVEC initial_position;
    u8 unknown_48c[0x4b0 - 0x48c];
    CHARACTER_MODEL_LAYERS_PC *character_model;
    u8 unknown_4b4[0x4fc - 0x4b4];
    NUSPECIALHANDLE special, shadow_special;
    u8 unknown_514[0x530 - 0x514];
    f32 scale, speed, radius;
    u8 unknown_53c[0x559 - 0x53c];
    u8 flags;
    u8 unknown_55a[4];
    i16 character_id, platform_id;
    u8 move_xy, invert_x;
    u8 unknown_564[4];
} GRABBER_CONFIG_PC;
STATIC_ASSERT(sizeof(GRABBER_CONFIG_PC) == 0x568);
STATIC_ASSERT(offsetof(GRABBER_CONFIG_PC, matrix) == 0);
STATIC_ASSERT(offsetof(GRABBER_CONFIG_PC, initial_position) == 0x480);
STATIC_ASSERT(offsetof(GRABBER_CONFIG_PC, character_model) == 0x4b0);
STATIC_ASSERT(offsetof(GRABBER_CONFIG_PC, special) == 0x4fc);
STATIC_ASSERT(offsetof(GRABBER_CONFIG_PC, shadow_special) == 0x508);
STATIC_ASSERT(offsetof(GRABBER_CONFIG_PC, scale) == 0x530);
STATIC_ASSERT(offsetof(GRABBER_CONFIG_PC, speed) == 0x534);
STATIC_ASSERT(offsetof(GRABBER_CONFIG_PC, radius) == 0x538);
STATIC_ASSERT(offsetof(GRABBER_CONFIG_PC, flags) == 0x559);
STATIC_ASSERT(offsetof(GRABBER_CONFIG_PC, character_id) == 0x55e);
STATIC_ASSERT(offsetof(GRABBER_CONFIG_PC, platform_id) == 0x560);
STATIC_ASSERT(offsetof(GRABBER_CONFIG_PC, move_xy) == 0x562);
STATIC_ASSERT(offsetof(GRABBER_CONFIG_PC, invert_x) == 0x563);
typedef struct world_grabber_config_pc_s {
    u8 unknown_000[0x104];
    void *giz_buffer;
    u8 unknown_108[0x128 - 0x108];
    LEVELDATA_PC *current_level;
    u8 unknown_12c[0x13c - 0x12c];
    NUGSCN *scene;
    u8 unknown_140[0x46ec - 0x140];
    GRABBER_CONFIG_PC *grabber;
} WORLD_GRABBER_CONFIG_PC;
STATIC_ASSERT(offsetof(WORLD_GRABBER_CONFIG_PC, giz_buffer) == 0x104);
STATIC_ASSERT(offsetof(WORLD_GRABBER_CONFIG_PC, current_level) == 0x128);
STATIC_ASSERT(offsetof(WORLD_GRABBER_CONFIG_PC, scene) == 0x13c);
STATIC_ASSERT(offsetof(WORLD_GRABBER_CONFIG_PC, grabber) == 0x46ec);
extern i16 GrabMachineIdPc, GrabMagnetIdPc, RobotBaseIdPc;
GRABBER_CONFIG_PC *GrabCurrentPc = 0;
static void __cdecl Grab_radius(NUFPAR_PC *parser) {
    GrabCurrentPc->radius = NuFParGetFloat(parser);
}
static void __cdecl Grab_speed(NUFPAR_PC *parser) {
    GrabCurrentPc->speed = NuFParGetFloat(parser);
}
static void __cdecl Grab_rotate(NUFPAR_PC *parser) {
    i32 rotate = NuFParGetInt(parser) & 1;
    GrabCurrentPc->flags = (u8)((GrabCurrentPc->flags & ~8) | (rotate << 3));
}
static void __cdecl Grab_scale(NUFPAR_PC *parser) {
    GrabCurrentPc->scale = NuFParGetFloat(parser);
}
static void __cdecl Grab_move_xy(NUFPAR_PC *parser) {
    if (NuFParGetInt(parser) != 0)
        GrabCurrentPc->move_xy = 1;
}
static void __cdecl Grab_invert_x(NUFPAR_PC *parser) {
    GrabCurrentPc->invert_x = 1;
}
static void __cdecl Grab_type(NUFPAR_PC *parser) {
    if (NuFParGetWord(parser) == 0)
        return;
    if (NuStrICmp(parser->word_buffer_ptr, "GRABMAGNET") == 0)
        GrabCurrentPc->character_id = GrabMagnetIdPc;
    else if (NuStrICmp(parser->word_buffer_ptr, "ROBOTBASE") == 0)
        GrabCurrentPc->character_id = RobotBaseIdPc;
    else
        GrabCurrentPc->character_id = GrabMachineIdPc;
}
static void __cdecl Grab_shadow(NUFPAR_PC *parser) {
    GrabCurrentPc->flags |= 0x10;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0)
        GrabCurrentPc->flags &= ~0x10;
}
typedef struct grab_config_data_pc_s {
    f32 level_y_offset;
    NUFPCOMJMP keywords[9];
} GRAB_CONFIG_DATA_PC;
STATIC_ASSERT(offsetof(GRAB_CONFIG_DATA_PC, keywords) == 4);
STATIC_ASSERT(sizeof(GRAB_CONFIG_DATA_PC) == 76);
static GRAB_CONFIG_DATA_PC GrabConfigDataPc = {-0.3f,
                                               {{"radius", Grab_radius},
                                                {"speed", Grab_speed},
                                                {"rotate", Grab_rotate},
                                                {"scale", Grab_scale},
                                                {"move_xy", Grab_move_xy},
                                                {"type", Grab_type},
                                                {"invert_x", Grab_invert_x},
                                                {"shadow", Grab_shadow},
                                                {NULL, NULL}}};
void __cdecl Grabber_Configure(WORLDINFO *world_ptr, char *config) {
    WORLD_GRABBER_CONFIG_PC *world = (WORLD_GRABBER_CONFIG_PC *)world_ptr;
    NUSPECIALHANDLE special;
    NUFPAR_PC *parser;
    GRABBER_CONFIG_PC *grabber;
    u8 active = 0;
    if (VehicleArea != 0 || (world->current_level->flags & 0xe0) != 0 || world->grabber != NULL)
        return;
    if (NuSpecialFind(world->scene, &special, "grabber", 1) == 0)
        return;
    grabber = (GRABBER_CONFIG_PC *)(((u32)world->giz_buffer + 15) & ~15u);
    world->grabber = grabber;
    world->giz_buffer = grabber + 1;
    GrabCurrentPc = grabber;
    grabber->special = special;
    parser = NuFParCreateMem("grabbers", config, 0xffff);
    if (parser == NULL)
        return;
    NuFParPushCom(parser, GrabConfigDataPc.keywords);
    while (NuFParGetLine(parser) != 0) {
        if (NuFParGetWord(parser) != 0) {
            if (NuStrICmp(parser->word_buffer_ptr, "grabber_start") == 0) {
                GrabCurrentPc->move_xy = 0;
                GrabCurrentPc->radius = 0.5f;
                GrabCurrentPc->speed = 1.0f;
                GrabCurrentPc->flags &= ~8;
                GrabCurrentPc->scale = 1.0f;
                GrabCurrentPc->invert_x = 0;
                GrabCurrentPc->flags |= 0x10;
                GrabCurrentPc->character_id = GrabMachineIdPc;
                active = 1;
            } else if (NuStrICmp(parser->word_buffer_ptr, "grabber_end") == 0)
                break;
            else if (active != 0)
                NuFParInterpretWord(parser);
        }
    }
    NuFParDestroy(parser);
    if (GrabCurrentPc->character_id != -1)
        GrabCurrentPc->character_model = APICharacterLoaded(GrabCurrentPc->character_id);
    else
        GrabCurrentPc->character_model = NULL;
    if (GrabCurrentPc->character_model != NULL)
        NuSpecialSetVisibility(&GrabCurrentPc->special, 0);
    else {
        NuSpecialSetVisibility(&GrabCurrentPc->special, 1);
        GrabCurrentPc->character_id = -1;
    }
    GrabCurrentPc->matrix = *NuSpecialGetDrawMtx(&GrabCurrentPc->special);
    if (world->current_level == DEATHSTARRESCUEA_LDATA)
        GrabCurrentPc->matrix.m31 += GrabConfigDataPc.level_y_offset;
    PCByteCopy_candidate(&GrabCurrentPc->initial_position, &GrabCurrentPc->matrix.m30, sizeof(NUVEC));
    NuSpecialFind(world->scene, &GrabCurrentPc->shadow_special, "grabber_shadow", 1);
    world->grabber->platform_id = -1;
}
