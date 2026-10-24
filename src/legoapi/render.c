#include <stdio.h>
#include "saga.h"
#undef DrawStatusText

extern f32 DrawStatusScaleXPc, DrawStatusScaleYPc;
void __cdecl NuQFntSetMtx(VUFNT *, NUMTX *);
static NUMTX RenderStatusMatrixPrivate = {0};
void __cdecl DrawStatusText(char *text, u16 angle, f32 x, f32 y, f32 scale, u32 colour, i32 alignment) {
    NUVEC origin = {0.0f, 0.0f, 800.0f};
    NUVEC position = {x * DrawStatusScaleXPc, y * DrawStatusScaleYPc, 0.0f};
    u16 encoded[512];
    NuQFntSetJustifiedTolerances(1.2f, 1.2f);
    NuMtxSetIdentity(&RenderStatusMatrixPrivate);
    NuMtxSetRotationX(&RenderStatusMatrixPrivate, 0);
    NuMtxRotateY(&RenderStatusMatrixPrivate, 0);
    NuMtxRotateZ(&RenderStatusMatrixPrivate, angle);
    NuMtxTranslate(&RenderStatusMatrixPrivate, &origin);
    NuMtxTranslate(&RenderStatusMatrixPrivate, &position);
    NuMtxMul(&RenderStatusMatrixPrivate, &RenderStatusMatrixPrivate, NuCameraGetMtx());
    NuQFntSet(QFont3DZ);
    NuQFntSetMtx(QFont3DZ, &RenderStatusMatrixPrivate);
    NuQFntPushPrintMode(4);
    NuQFntSetCoordinateSystem(4);
    NuQFntSetColour(QFont3DZ, colour);
    NuQFntSetScale(QFont3DZ, scale, scale);
    Text3DStringEncode(text, encoded);
    switch (alignment) {
        case 2: {
            f32 move_y = (f32)(-NuQFntHeight(QFont3DZ) * 0.5);
            NuQFntMove(QFont3DZ, 0.0f, move_y, 0.0f);
            break;
        }
        case 8: {
            f32 move_y = (f32)(-NuQFntHeight(QFont3DZ) * 0.5);
            f32 move_x = -NuQFntPrintLenW(QFont3DZ, encoded);
            NuQFntMove(QFont3DZ, move_x, move_y, 0.0f);
            break;
        }
        default: {
            f32 move_y = (f32)(-NuQFntHeight(QFont3DZ) * 0.5);
            f32 move_x = (f32)(-NuQFntPrintLenW(QFont3DZ, encoded) * 0.5);
            NuQFntMove(QFont3DZ, move_x, move_y, 0.0f);
            break;
        }
    }
    NuQFntPrintW(QFont3DZ, encoded);
    NuQFntPopPrintMode();
    NuQFntSetCoordinateSystem(3);
}

void __cdecl DrawStatusTextFraction(i32 current, i32 total, f32 x, f32 y, u16 angle, f32 scale, u32 colour,
                                    f32 animation, f32 animation_duration) {
    char text[16];
    u16 encoded[64];
    NUMTX matrix;
    NUVEC origin = {0.0f, 0.0f, 800.0f};
    NUVEC position = {(f32)((f64)x * 400.0), (f32)((f64)y * 250.0), 0.0f};
    f32 full_width, height;
    NuQFntSetJustifiedTolerances(1.2f, 1.2f);
    NuMtxSetIdentity(&matrix);
    NuMtxSetRotationX(&matrix, 0);
    NuMtxRotateY(&matrix, 0);
    NuMtxRotateZ(&matrix, angle);
    NuMtxTranslate(&matrix, &origin);
    NuMtxTranslate(&matrix, &position);
    NuMtxMul(&matrix, &matrix, NuCameraGetMtx());
    NuQFntSet(QFont3DZ);
    NuQFntSetMtx(QFont3DZ, &matrix);
    NuQFntPushPrintMode(4);
    NuQFntSetCoordinateSystem(4);
    NuQFntSetColour(QFont3DZ, colour);
    NuQFntSetScale(QFont3DZ, scale, scale);
    sprintf(text, "%i/%i", current, total);
    Text3DStringEncode(text, encoded);
    full_width = NuQFntPrintLenW(QFont3DZ, encoded);
    height = NuQFntHeight(QFont3DZ);
    NuQFntMove(QFont3DZ, (f32)(-full_width * 0.5), (f32)(-height * 0.5), 0.0f);
    if (animation > 0.0f) {
        i32 wave_angle = (i32)(((f64)animation / (f64)animation_duration) * 32768.0 + 16384.0);
        f64 blend = 1.0 - ((f64)NuTrigTable[(wave_angle >> 1) & 0x7fff] + 1.0) * 0.5;
        f64 wave = 1.0 - blend;
        f64 half_wave = wave * 0.5;
        f32 large_scale = (f32)(((f64)scale + 0.5) - half_wave);
        f32 small_scale = (f32)(((f64)scale - 0.5) + half_wave);
        f64 full_width_snapshot, half_full_width, current_center;
        f32 suffix_x, current_width, large_x, small_x;
        sprintf(text, "/%i", total);
        Text3DStringEncode(text, encoded);
        full_width_snapshot = (f64)full_width;
        half_full_width = full_width_snapshot * 0.5;
        suffix_x = (f32)((full_width_snapshot - (f64)NuQFntPrintLenW(QFont3DZ, encoded)) - half_full_width);
        NuQFntMove(QFont3DZ, suffix_x, (f32)(-NuQFntHeight(QFont3DZ) * 0.5), 0.0f);
        NuQFntPrintW(QFont3DZ, encoded);
        sprintf(text, "%i", current);
        Text3DStringEncode(text, encoded);
        current_width = NuQFntPrintLenW(QFont3DZ, encoded);
        NuQFntSetScale(QFont3DZ, large_scale, large_scale);
        current_center = (f64)current_width * 0.5 - half_full_width;
        large_x = (f32)(current_center - NuQFntPrintLenW(QFont3DZ, encoded) * 0.5);
        NuQFntMove(QFont3DZ, large_x, (f32)(-NuQFntHeight(QFont3DZ) * 0.5), 0.0f);
        NuQFntPrintW(QFont3DZ, encoded);
        NuQFntSetScale(QFont3DZ, small_scale, small_scale);
        small_x = (f32)(current_center - NuQFntPrintLenW(QFont3DZ, encoded) * 0.5);
        NuQFntMove(QFont3DZ, small_x, (f32)(-NuQFntHeight(QFont3DZ) * 0.5), 0.0f);
        NuQFntPrintW(QFont3DZ, encoded);
    } else {
        sprintf(text, "%i", current);
        Text3DStringEncode(text, encoded);
        NuQFntPrintW(QFont3DZ, encoded);
        sprintf(text, "/%i", total);
        Text3DStringEncode(text, encoded);
        NuQFntPrintW(QFont3DZ, encoded);
    }
    NuQFntPopPrintMode();
    NuQFntSetCoordinateSystem(3);
}

STATUS_STAGE_s *FindStatusStage(i32 type) {
    i32 index;
    for (index = 0; StatusStages[index].type != -1; ++index) {
        if (StatusStages[index].type == type)
            return &StatusStages[index];
    }
    return NULL;
}

/* Native status-stage handlers call this shared routine. */
__declspec(noinline) void NewStatusRumbleBuzz(i32 player, float amount, float buzz, i32 priority) {
    if (StatusPacket.player0_active != 0 && (player == -1 || player == 0)) {
        if (amount > 0.0f && (StatusPacket.player0_rumble_time <= 0.0f ||
                              amount > StatusPacket.player0_rumble_time / StatusPacket.player0_rumble_duration *
                                           StatusPacket.player0_rumble_amount)) {
            StatusPacket.player0_rumble_amount = amount;
            StatusPacket.player0_rumble_duration = amount;
            StatusPacket.player0_rumble_time = amount;
        }
        if (buzz > StatusPacket.player0_buzz_amount) {
            StatusPacket.player0_buzz_amount = buzz;
        }
        if (priority > 0) {
            const i32 old_priority = StatusPacket.player0_rumble_priority;
            priority = (i32)((u32)priority + 1u);
            if (priority > old_priority) {
                StatusPacket.player0_rumble_priority = (u8)(priority);
            }
        }
    }

    if (StatusPacket.player1_active != 0 && (player == -1 || player == 1)) {
        if (amount > 0.0f && (StatusPacket.player1_rumble_time <= 0.0f ||
                              amount > StatusPacket.player1_rumble_time / StatusPacket.player1_rumble_duration *
                                           StatusPacket.player1_rumble_amount)) {
            StatusPacket.player1_rumble_amount = amount;
            StatusPacket.player1_rumble_duration = amount;
            StatusPacket.player1_rumble_time = amount;
        }
        if (buzz > StatusPacket.player1_buzz_amount) {
            StatusPacket.player1_buzz_amount = buzz;
        }
        if (priority > 0) {
            const i32 old_priority = StatusPacket.player1_rumble_priority;
            priority = (i32)((u32)priority + 1u);
            if (priority > old_priority) {
                StatusPacket.player1_rumble_priority = (u8)(priority);
            }
        }
    }
}

i32 AddGoldBrickMessage(STATUSPACKET_s *packet, i16 brick) {
    if (goldbrickmsgcount < 16) {
        ((i16 *)(packet->field_0x12c))[goldbrickmsgcount] = brick;
        ++goldbrickmsgcount;
    }
    AddToGoldBricks();
    return 1;
}

__declspec(noinline) void AddStatusStage(STATUSPACKET_s *packet, i32 type, i32 gold_brick_enabled) {
    packet->stage_types[packet->stage_count] = (i8)(type);
    packet->gold_brick_enabled[packet->stage_count] = (u8)(gold_brick_enabled);
    ++packet->stage_count;
}

i32 __cdecl NuSpecialDrawAtAlpha(NUSPECIALHANDLE *, NUMTX *, f32);
i32 __cdecl DrawPanel3DObject(f32 x, f32 y, f32 z, f32 scale_x, f32 scale_y, f32 scale_z, u16 rotate_x, u16 rotate_y,
                              u16 rotate_z, NUSPECIALHANDLE *special, i32 rotate_order, f32 alpha) {
    if (alpha > 0.0f && special != NULL && NuSpecialExistsFn(special) != 0 &&
        (scale_x != 0.0f || scale_y != 0.0f || scale_z != 0.0f)) {
        NUVEC scale = {scale_x, scale_y, scale_z};
        NUMTX matrix;
        NuMtxSetScale(&matrix, &scale);
        RotateGameMatrix(&matrix, rotate_order, rotate_x, rotate_y, rotate_z);
        matrix.translation.x = x * PANEL3DMULX;
        matrix.translation.y = y * PANEL3DMULY;
        matrix.translation.z = z;
        NuMtxMulVU0(&matrix, &matrix, NuCameraGetMtx());
        NuSpecialDrawAtAlpha(special, &matrix, alpha);
    }
    return 0;
}

i32 DrawPanel3DObjectMtxNoAlpha(nuhspecial_s *special, NUMTX *matrix) {
    if (special == NULL || NuSpecialExistsFn(special) == 0)
        return 0;
    NuMtxMulVU0(matrix, matrix, NuCameraGetMtx());
    return NuSpecialDrawAt(special, matrix);
}

extern NUGSCN *things_scene;
extern i32 DrawCharIconFindPc, DrawCharIconPanelPc, LegoObjectIconQuestionPc;
NUGSCN *__cdecl IconScene_FindById(i32);
STATIC_ASSERT(sizeof(CHARACTERDATA_PC) == 0x4c);
STATIC_ASSERT(offsetof(CHARACTERDATA_PC, icon_model) == 0x42);
STATIC_ASSERT(sizeof(PLAYERCHARACTERCONFIG_PC) == 0x120);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, hover_fall_mode) == 0x116);
STATIC_ASSERT(sizeof(LEVEL_OBJECT_CONFIG_PC) == 8);
STATIC_ASSERT(offsetof(LEVEL_OBJECT_CONFIG_PC, name) == 4);
void __cdecl DrawCharIcon(i32 character_id, f32 x, f32 y, f32 z, f32 scale, i32 frame_object_id, f32 character_alpha,
                          f32 frame_alpha, i32 draw_character, NUSPECIALHANDLE *override_special) {
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    NUSPECIALHANDLE special;
    if (ObjTabList == NULL || world == NULL)
        return;
    if (DrawCharIconFindPc != 0)
        NuSpecialFind(things_scene, &special, ObjTabList[frame_object_id].name, 1);
    else
        special = world->level_objects[frame_object_id].special;
    if (NuSpecialExistsFn(&special) != 0)
        DrawPanel3DObject(x, y, (f32)(z + 1.0), scale, scale, scale, 0, 0, 0, &special, 0, frame_alpha);
    if (draw_character != 0) {
        if (override_special != NULL && NuSpecialExistsFn(override_special) != 0)
            DrawPanel3DObject(x, y, (f32)(z + 1.0), scale, scale, scale, 0, 0, 0, override_special, 0, character_alpha);
        else {
            i32 icon_object_id;
            if (character_id == -1)
                icon_object_id = LegoObjectIconQuestionPc;
            else {
                icon_object_id = CDataList[character_id].icon_model;
                if (icon_object_id != -1 && GCDataList[character_id].hover_fall_mode == 0 &&
                    icon_object_id != PCArcadeUnpairedIconModel)
                    ++icon_object_id;
            }
            if (icon_object_id != -1) {
                if (DrawCharIconFindPc != 0) {
                    NUGSCN *icon_scene = character_id == -1 ? NULL : IconScene_FindById(character_id);
                    if (icon_scene != NULL)
                        NuSpecialFind(icon_scene, &special, ObjTabList[icon_object_id].name, 1);
                    else
                        NuSpecialFind(things_scene, &special, ObjTabList[icon_object_id].name, 1);
                } else
                    special = world->level_objects[icon_object_id].special;
                if (NuSpecialExistsFn(&special) != 0)
                    DrawPanel3DObject(x, y, (f32)(z + 1.0), scale, scale, scale, 0, 0, 0, &special, 0, character_alpha);
            }
        }
    }
    DrawCharIconFindPc = 0;
    DrawCharIconPanelPc = -1;
}
extern NUGSCN *RenderBackdropScenePc;
extern NUSPECIALHANDLE RenderBackdropSpecialsPc[4];
extern void(__cdecl *RenderBackdropAlphaFnPc)(f32 *);

void __cdecl BackDrop_Dump(void) {
    if (RenderBackdropScenePc != NULL)
        NuGScnRemove(RenderBackdropScenePc);
}

void __cdecl BackDrop_Draw(f32 alpha, i32 flags) {
    i32 special_index;
    if (RenderBackdropScenePc == NULL)
        return;
    if (flags == 0 && RenderBackdropAlphaFnPc != NULL)
        RenderBackdropAlphaFnPc(&alpha);
    if (!(alpha > 0.0f))
        return;
    for (special_index = 0; special_index < 4; ++special_index) {
        NUSPECIALHANDLE *special = &RenderBackdropSpecialsPc[special_index];
        if (NuSpecialExistsFn(special) != 0) {
            NUMTX matrix = *NuSpecialGetDrawMtx(special);
            NUVEC position;
            u16 angle;
            i32 jitter_index;
            NuSpecialDrawAtAlpha(special, &matrix, alpha);
            position = matrix.translation;
            angle = (u16)qrand();
            for (jitter_index = 0; jitter_index < 3; ++jitter_index) {
                matrix.translation.x = (f32)(NuTrigTable[angle >> 1] * (f64)0.01f + position.x);
                matrix.translation.y =
                    (f32)(NuTrigTable[((i32)angle + 0x4000) >> 1 & 0x7fff] * (f64)0.01f + position.y);
                matrix.translation.z = position.z;
                NuSpecialDrawAtAlpha(special, &matrix, alpha);
                angle = (u16)(angle + 0x5555);
            }
        }
    }
}

void __cdecl DrawParallax(NUSPECIALHANDLE *special) {
    NUVEC scale;
    NUMTX matrix;
    scale.x = scale.y = scale.z = (f32)((f64)pNuCam->far_clip * TerrainScanMargin10Pc * 0.5);
    NuMtxSetScale(&matrix, &scale);
    matrix.m30 = pNuCam->mtx.m30;
    matrix.m31 = pNuCam->mtx.m31;
    matrix.m32 = pNuCam->mtx.m32;
    NuSpecialDrawAt(special, &matrix);
}

void GameFog_Reset() {
    GameFogSnap = 1;
    GameFogDuration = 0.0f;
    GameFogSet = 0;
    GameFogTime = 0.0f;
}

STATIC_ASSERT(sizeof(CUT_DRAW_SETTINGS_PC) == 8);
extern CUT_DRAW_SETTINGS_PC *CutSceneSys;
extern i32 dco_id, dco_wearinghat, dco_reflectaxis;
extern PLAYERCHARACTERCONFIG_PC *dco_gcdata;
extern CHARACTER_MODEL_LAYERS_PC *dco_cmodel;
extern f32 dco_reflectcoord;
i32 __cdecl MatrixReflection(NUMTX *, i32, f32, f32, NUMTX *);

extern i32 dco_locatorposonly;
extern u16 dco_prerotatez;
extern void(__cdecl *DisguiseAdjustFn)(i32, i32, NUVEC *, NUVEC *);
i32 __cdecl NuSpecialDrawAtAlpha(NUSPECIALHANDLE *, NUMTX *, f32);
void DrawObjectOnCharacter(WORLDINFO *world, GAMEOBJECT *object, i32 object_id, NUSPECIALHANDLE *special, i32 locator,
                           i32 second_locator, NUMTX *joints, i32 reflect, u32 layers, NUMTX *rotation,
                           NUVEC *translation, f32 alpha, f32 scale) {
    i32 position_only = dco_locatorposonly;
    u16 rotate_z;
    i32 id, axis, hat, use_alpha;
    f32 plane;
    CHARACTER_MODEL_LAYERS_PC *model;
    NUMTX matrix, reflected;
    dco_locatorposonly = 0;
    rotate_z = dco_prerotatez;
    dco_prerotatez = 0;
    if (object != NULL) {
        id = object->id;
        model = object->character_model;
        axis = object->reflection_enabled;
        plane = object->reflection_height;
        hat = object->helmet;
    } else {
        id = dco_id;
        if (id == -1 || dco_gcdata == NULL || dco_cmodel == NULL)
            return;
        model = dco_cmodel;
        axis = dco_reflectaxis;
        plane = dco_reflectcoord;
        hat = dco_wearinghat;
    }
    dco_gcdata = NULL;
    dco_cmodel = NULL;
    dco_id = -1;
    dco_reflectaxis = 0;
    dco_reflectcoord = 2000000.0f;
    dco_wearinghat = 0;
    if (world == NULL)
        world = WorldInfo_CurrentlyActive();
    if (object_id != -1) {
        if (!world->level_objects[object_id].active)
            return;
    } else if (special == NULL || !NuSpecialExistsFn(special)) {
        return;
    }
    if (locator == -1 || model->points_of_interest[locator] == NULL)
        return;
    if (position_only)
        NuMtxSetTranslation(&matrix, (NUVEC *)&joints[locator].m30);
    else
        matrix = joints[locator];
    if (second_locator != -1 && model->points_of_interest[second_locator] != NULL) {
        NUVEC position = {matrix.m30, matrix.m31, matrix.m32};
        if (!position_only) {
            NUMTX first = matrix;
            NUMTX second = joints[second_locator];
            first.m30 = first.m31 = first.m32 = 0.0f;
            second.m30 = second.m31 = second.m32 = 0.0f;
            QuatInterpolateRotationMatrix(&matrix, &first, &second, 0.5f);
        }
        matrix.m30 = (f64)(position.x + joints[second_locator].m30) * 0.5;
        matrix.m31 = (f64)(position.y + joints[second_locator].m31) * 0.5;
        matrix.m32 = (f64)(position.z + joints[second_locator].m32) * 0.5;
    }
    if (rotate_z != 0)
        NuMtxPreRotateZ(&matrix, rotate_z);
    if (object_id != -1 && CutSceneSys->object_id == object_id && DisguiseAdjustFn != NULL) {
        NUVEC adjustment, offset;
        DisguiseAdjustFn(id, hat, &adjustment, &offset);
        if (adjustment.x != 1.0f || adjustment.y != 1.0f || adjustment.z != 1.0f)
            NuMtxPreScale(&matrix, &adjustment);
        if (offset.x != 0.0f || offset.y != 0.0f || offset.z != 0.0f)
            NuMtxPreTranslate(&matrix, &offset);
    }
    if (rotation != NULL) {
        NUVEC position = {matrix.m30, matrix.m31, matrix.m32};
        NuMtxMulR(&matrix, rotation, &matrix);
        matrix.m30 = position.x;
        matrix.m31 = position.y;
        matrix.m32 = position.z;
    }
    if (translation != NULL)
        NuMtxTranslate(&matrix, translation);
    use_alpha = 0;
    if (object_id != -1) {
        special = &world->level_objects[object_id].special;
        if (ObjTabList != NULL)
            use_alpha = ObjTabList[object_id].unknown_01;
    }
    if (scale != 1.0f)
        NuMtxPreScaleU(&matrix, scale);
    if (use_alpha)
        NuSpecialDrawAtAlpha(special, &matrix, alpha);
    else
        NuSpecialDrawAt(special, &matrix);
    if (reflect && MatrixReflection(&matrix, axis, plane, world->current_level->reflection_y, &reflected)) {
        NuRndrStartReflectionRender(0);
        if (use_alpha)
            NuSpecialDrawAtAlpha(special, &reflected, alpha);
        else
            NuSpecialDrawAt(special, &reflected);
        NuRndrEndReflectionRender();
    }
}

i32 __cdecl Draw3DObjectMtx(WORLDINFO *world, i32 object_id, NUMTX *matrix) {
    if (object_id == -1)
        return 1;
    if (world == NULL) {
        world = WorldInfo_CurrentlyActive();
        if (world == NULL)
            return 1;
    }
    if (world->level_objects[object_id].active != 0)
        return NuSpecialDrawAt(&world->level_objects[object_id].special, matrix);
    return 0;
}

/* Source-first scratch candidate; unclosed real leaf declarations only. */

typedef struct render_post_prim_vertex_pc_s {
    f32 x, y, z;
    u32 colour;
    f32 u, v;
} RENDER_POST_PRIM_VERTEX_PC;
STATIC_ASSERT(sizeof(RENDER_POST_PRIM_VERTEX_PC) == 0x18);
STATIC_ASSERT(offsetof(RENDER_POST_PRIM_VERTEX_PC, colour) == 0xc);
extern i32 NuPrimCoordSystemStack[];
f32 __cdecl RenderPostPowPc(f32, f32);
i32 __cdecl RenderPostASinPc(f32);
#define GRID_TAN_HALF_FOV(half_fov)                                                                                    \
    (NuTrigTable[(i32)((half_fov) * 10430.3779296875f) >> 1 & 0x7fff] /                                                \
     NuTrigTable[(16384 - (i32)((half_fov) * -10430.3779296875f)) >> 1 & 0x7fff])
#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif
#define RGBA_TO_NUCOLOUR32(r, g, b, a)                                                                                 \
    ((u32)(u8)(r) | ((u32)(u8)(g) << 8) | ((u32)(u8)(b) << 16) | ((u32)(u8)(a) << 24))
#define ALPHA_GRID_INLINE_COLOUR                                                                                       \
    colour = RGBA_TO_NUCOLOUR32((MIN(255.0f, (MAX(0.0f, brightness)))), (MIN(255.0f, (MAX(0.0f, brightness)))),        \
                                (MIN(255.0f, (MAX(0.0f, brightness)))), 128)
#define ALPHA_GRID_CLAMP_COLOUR                                                                                        \
    do {                                                                                                               \
        const u32 component = RenderPostClampBytePc(0, 255, brightness);                                               \
        colour = RGBA_TO_NUCOLOUR32(component, component, component, 128);                                             \
    } while (0)
#define ALPHA_GRID_VERTEX(vx, vy, colour_source)                                                                       \
    do {                                                                                                               \
        i32 colour;                                                                                                    \
        u32 converted;                                                                                                 \
        direction.x = (vx);                                                                                            \
        direction.y = -(vy) * camera.aspect;                                                                           \
        direction.z = adjacent;                                                                                        \
        NuVecNorm((NUVEC *)&direction, (NUVEC *)&direction);                                                           \
        direction.w = 1.0f;                                                                                            \
        NuVecMtxTransform((NUVEC *)&direction, (NUVEC *)&direction, &camera.mtx);                                      \
        NuVecNorm((NUVEC *)&direction, (NUVEC *)&direction);                                                           \
        direction.w = 1.0f;                                                                                            \
        brightness = direction.y * 0.5f + 0.5f;                                                                        \
        if (brightness <= near_angle)                                                                                  \
            brightness = near_scale * parameters->intensity;                                                           \
        else if (brightness >= far_angle)                                                                              \
            brightness = far_scale * parameters->intensity;                                                            \
        else                                                                                                           \
            brightness = ((brightness - near_angle) * scale_delta / angle_delta + near_scale) * parameters->intensity; \
        if (parameters->directional) {                                                                                 \
            f32 angle = (i16)(0x4000 - RenderPostASinPc(NuVecDot(&parameters->direction, (NUVEC *)&direction))) *      \
                        0.0054931640625f;                                                                              \
            if (angle < parameters->direction_near_angle) {                                                            \
                brightness += 128.0f * parameters->direction_far_scale;                                                \
            } else if (!(angle > parameters->direction_far_angle)) {                                                   \
                brightness +=                                                                                          \
                    128.0f *                                                                                           \
                    ((1.0f - RenderPostPowPc((angle - parameters->direction_near_angle) /                              \
                                                 (parameters->direction_far_angle - parameters->direction_near_angle), \
                                             parameters->direction_bias)) *                                            \
                         (parameters->direction_far_scale - parameters->direction_near_scale) +                        \
                     parameters->direction_near_scale);                                                                \
            }                                                                                                          \
        }                                                                                                              \
        colour_source;                                                                                                 \
        if (NuPrimNeedsOverbrightening != 0) {                                                                         \
            converted =                                                                                                \
                ((colour >> 16) & 0xff) + ((colour & 0xff) << 16) + (colour & 0xff000000u) + (colour & 0xff00);        \
        } else {                                                                                                       \
            converted =                                                                                                \
                ((colour >> 1) & 0x7f00) + ((colour & 0xfe) << 15) + ((colour >> 17) & 0x7f) + (colour & 0xff000000u); \
        }                                                                                                              \
        ((RENDER_POST_PRIM_VERTEX_PC *)NuPrimStreamBufferPointer->void_ptr)->colour = converted;                       \
        NuPrim2DAddXYZ((vx), (vy), 0.0f);                                                                              \
    } while (0)

void DrawAlphaGrid(i32 rows, i32 cols, RENDER_POST_BLOOM_READER_PC *parameters) {
    f32 inv_col = 1.0f / (cols - 1);
    f32 inv_row = 1.0f / (rows - 1);
    f32 x0 = 0.0f * inv_row * 2.0f - 1.0f;
    f32 y_start = 2.0f * (0.0f * inv_col) - 1.0f;
    f32 x1 = inv_row * 2.0f - 1.0f;
    f32 y_next = inv_col * 2.0f - 1.0f;
    f32 step_x = 2.0f * inv_row;
    f32 step_y = 2.0f * inv_col;
    f32 near_scale = 128.0f * parameters->near_scale;
    f32 far_scale = parameters->far_scale * 128.0f;
    f32 near_angle = parameters->near_angle / 180.0f;
    f32 far_angle = parameters->far_angle / 180.0f;
    f32 angle_delta = far_angle - near_angle;
    __declspec(align(16)) NUVEC4 direction;
    f32 scale_delta = far_scale - near_scale;
    NUCAMERA camera;
    f32 brightness;
    static i32 first = 1;
    static f32 saved_fov = 0;
    static f32 adjacent = 0;
    static i32 row = 0, col = 0;
    camera = global_camera;
    if (first) {
        f32 half_fov = 0.5f * camera.fov;
        saved_fov = camera.fov;
        first = 0;
        adjacent = 1.0f / GRID_TAN_HALF_FOV(half_fov);
    }
    if (saved_fov != camera.fov) {
        f32 half_fov = camera.fov * 0.5f;
        adjacent = 1.0f / GRID_TAN_HALF_FOV(half_fov);
        // The original writes the saved FOV back into this camera copy.
        camera.fov = saved_fov;
    }
    camera.mtx.m30 = 0.0f;
    camera.mtx.m31 = 0.0f;
    camera.mtx.m32 = 0.0f;
    camera.mtx.m33 = 1.0f;
    ++NuPrimCSPos;
    NuPrimSetCoordinateSystem(1);
    for (row = 0; row < rows - 1; ++row) {
        f32 y0, y1;
        NuPrim2DBegin(14, 5, NULL);
        y0 = y_start;
        y1 = y_next;
        for (col = 0; col < cols; ++col) {
            ALPHA_GRID_VERTEX(x0, y0, ALPHA_GRID_INLINE_COLOUR);
            ALPHA_GRID_VERTEX(x1, y0, ALPHA_GRID_INLINE_COLOUR);
            if ((cols - 1) != col) {
                ALPHA_GRID_VERTEX(x0, y0, ALPHA_GRID_CLAMP_COLOUR);
                ALPHA_GRID_VERTEX(x0, y1, ALPHA_GRID_CLAMP_COLOUR);
                ALPHA_GRID_VERTEX(x1, y0, ALPHA_GRID_CLAMP_COLOUR);
                ALPHA_GRID_VERTEX(x1, y1, ALPHA_GRID_CLAMP_COLOUR);
                ALPHA_GRID_VERTEX(x1, y0, ALPHA_GRID_CLAMP_COLOUR);
                ALPHA_GRID_VERTEX(x0, y1, ALPHA_GRID_CLAMP_COLOUR);
            }
            y0 += step_y;
            y1 += step_y;
        }
        NuPrim2DEnd();
        x0 += step_x;
        x1 += step_x;
    }
    --NuPrimCSPos;
    NuPrimSetCoordinateSystem(NuPrimCoordSystemStack[NuPrimCSPos]);
}
#undef ALPHA_GRID_VERTEX
#undef ALPHA_GRID_INLINE_COLOUR
#undef ALPHA_GRID_CLAMP_COLOUR

static f32 RenderBackdropTargetTopRPc = 0.0f;
static f32 RenderBackdropTargetTopGPc = 0.0f;
static f32 RenderBackdropTargetTopBPc = 0.0f;
static f32 RenderBackdropTargetBottomRPc = 0.0f;
static f32 RenderBackdropTargetBottomGPc = 0.0f;
static f32 RenderBackdropTargetBottomBPc = 24.0f;
extern f32 RenderBackdropTopRPc;
extern f32 RenderBackdropTopGPc;
extern f32 RenderBackdropTopBPc;
extern f32 RenderBackdropBottomRPc;
extern f32 RenderBackdropBottomGPc;
extern f32 RenderBackdropBottomBPc;
static f32 RenderBackdropWaitPc = 0.0f;
extern i32 RenderBackdropBlackPc;
void __cdecl BackDrop_ClearColourTargets(void) {
    RenderBackdropTargetTopRPc = 0.0f;
    RenderBackdropTargetTopGPc = 0.0f;
    RenderBackdropTargetTopBPc = 0.0f;
    RenderBackdropTargetBottomRPc = 0.0f;
    RenderBackdropTargetBottomGPc = 0.0f;
    RenderBackdropTargetBottomBPc = 0.0f;
}
void __cdecl BackDrop_ResetColours(void) {
    RenderBackdropTopRPc = 0.0f;
    RenderBackdropTopGPc = 0.0f;
    RenderBackdropTopBPc = 0.0f;
    RenderBackdropBottomRPc = 0.0f;
    RenderBackdropBottomGPc = 0.0f;
    RenderBackdropBottomBPc = 0.0f;
    RenderBackdropWaitPc = 0.0001f;
    BackDrop_ClearColourTargets();
}
void __cdecl BackDrop_UpdateColours(i32 instant) {
    if (RenderBackdropBlackPc) {
        RenderBackdropTopRPc = 0.0f;
        RenderBackdropTopGPc = 0.0f;
        RenderBackdropTopBPc = 0.0f;
        RenderBackdropBottomRPc = 0.0f;
        RenderBackdropBottomGPc = 0.0f;
        RenderBackdropBottomBPc = 0.0f;
        return;
    }
    if (RenderBackdropWaitPc > 0.0f) {
        RenderBackdropWaitPc -= FRAMETIME;
        if (!(RenderBackdropWaitPc <= 0.0f))
            return;
        if (instant) {
            RenderBackdropTargetTopBPc = 0.0f;
            RenderBackdropTargetTopGPc = 0.0f;
            RenderBackdropTargetTopRPc = 0.0f;
            RenderBackdropTargetBottomBPc = 0.0f;
            RenderBackdropTargetBottomGPc = 0.0f;
            RenderBackdropTargetBottomRPc = 0.0f;
        } else
            BackDrop_ClearColourTargets();
        return;
    }
    if (RenderBackdropTopRPc == RenderBackdropTargetTopRPc && RenderBackdropTopGPc == RenderBackdropTargetTopGPc &&
        RenderBackdropTopBPc == RenderBackdropTargetTopBPc &&
        RenderBackdropBottomRPc == RenderBackdropTargetBottomRPc &&
        RenderBackdropBottomGPc == RenderBackdropTargetBottomGPc &&
        RenderBackdropBottomBPc == RenderBackdropTargetBottomBPc) {
        RenderBackdropWaitPc = (f32)((f64)qrand() * (f64)(1.0f / 65535.0f) * 2.5 + 2.5);
        return;
    }
    {
        f32 step = (f32)(FRAMETIME * 10.0);
        RenderBackdropTopRPc = SeekLinearF(RenderBackdropTopRPc, RenderBackdropTargetTopRPc, step);
        RenderBackdropTopGPc = SeekLinearF(RenderBackdropTopGPc, RenderBackdropTargetTopGPc, step);
        RenderBackdropTopBPc = SeekLinearF(RenderBackdropTopBPc, RenderBackdropTargetTopBPc, step);
        RenderBackdropBottomRPc = SeekLinearF(RenderBackdropBottomRPc, RenderBackdropTargetBottomRPc, step);
        RenderBackdropBottomGPc = SeekLinearF(RenderBackdropBottomGPc, RenderBackdropTargetBottomGPc, step);
        RenderBackdropBottomBPc = SeekLinearF(RenderBackdropBottomBPc, RenderBackdropTargetBottomBPc, step);
    }
}
void __cdecl BackDrop_Init(char *path, VARIPTR *buffer, VARIPTR *end) {
    NUGSCN *scene = RenderBackdropScenePc;
    if (scene == NULL) {
        scene = NuGScnRead(buffer, *end, path);
        RenderBackdropScenePc = scene;
    }
    memset(RenderBackdropSpecialsPc, 0, sizeof(RenderBackdropSpecialsPc));
    if (scene == NULL)
        return;
    NuSpecialFind(RenderBackdropScenePc, &RenderBackdropSpecialsPc[0], "ball1", 1);
    NuSpecialFind(RenderBackdropScenePc, &RenderBackdropSpecialsPc[1], "ball2", 1);
    NuSpecialFind(RenderBackdropScenePc, &RenderBackdropSpecialsPc[2], "ball3", 1);
    NuSpecialFind(RenderBackdropScenePc, &RenderBackdropSpecialsPc[3], "ball4", 1);
}
void __cdecl BackDrop_Update(f32 dt) {
    if (RenderBackdropScenePc != NULL)
        NuGScnUpdate(RenderBackdropScenePc, (f32)(dt * 60.0));
}

/* Append to render.c. Native PC 552d10, 229 bytes, cdecl full stack ABI. */
i32 __cdecl DrawPanel3DObjectNoAlpha(f32 x, f32 y, f32 z, f32 scale_x, f32 scale_y, f32 scale_z, u16 rotate_x,
                                     u16 rotate_y, u16 rotate_z, NUSPECIALHANDLE *special, i32 rotate_order) {
    if (special != NULL && NuSpecialExistsFn(special) != 0 && (scale_x != 0.0f || scale_y != 0.0f || scale_z != 0.0f)) {
        NUVEC scale = {scale_x, scale_y, scale_z};
        NUMTX matrix;
        NuMtxSetScale(&matrix, &scale);
        RotateGameMatrix(&matrix, rotate_order, rotate_x, rotate_y, rotate_z);
        matrix.translation.x = x * PANEL3DMULX;
        matrix.translation.y = y * PANEL3DMULY;
        matrix.translation.z = z;
        NuMtxMulVU0(&matrix, &matrix, NuCameraGetMtx());
        NuSpecialDrawAt(special, &matrix);
    }
    return 0;
}

/* PC561400/1235. Append to render.c (body valid C99), existing native globals. */
void __cdecl DrawGameMessages(void) {

    GAME_MESSAGE_PC *message = GameMessage;
    GAME_MESSAGE_PC *end = message + 128;
    for (; message != end; ++message) {
        u32 flags;
        f32 progress, scale;
        NUVEC position;
        if (message->active == 0) {
            continue;
        }
        if (message->parameter_fb != 0 && (message->flags & 0x40000) == 0) {
            continue;
        }
        if (message->delay > 0.0f) {
            continue;
        }
        if (!(message->duration > message->elapsed) && message->expired == 0) {
            continue;
        }

        flags = message->flags;
        progress = message->elapsed / message->duration;
        if ((flags & 0x100) != 0) {
            progress = 1.0f - NuTrigTable[((i32)(progress * 16384.0 + 16384.0) >> 1) & 0x7fff];
        } else if ((flags & 0x200) != 0) {
            progress = NuTrigTable[((i32)(progress * 16384.0) >> 1) & 0x7fff];
        } else if ((flags & 0x400) != 0) {
            progress = NuTrigTable[((i32)(progress * 32768.0) >> 1) & 0x7fff];
        } else if ((flags & 0x800) != 0) {
            progress = 1.0f - NuTrigTable[((i32)(progress * 32768.0) >> 1) & 0x7fff];
        }

        scale = (flags & 5) == 5 ? message->scale_current : message->scale_initial;
        position = message->start_position;
        if (message->parameter_d4 != 0.0f && message->parameter_fb == 0) {
            const f32 limit = message->parameter_d4;
            if (position.x < -limit) {
                position.x = -limit;
            } else if (position.x > limit) {
                position.x = limit;
            }
            if (position.y < -limit) {
                position.y = -limit;
            } else if (position.y > limit) {
                position.y = limit;
            }
        }

        if ((flags & 8) != 0) {
            if (message->update_fn != NULL) {
                message->update_fn(message);
                flags = message->flags;
            }
            position.x = position.x + (message->target_position.x - position.x) * progress;
            position.y = position.y + (message->target_position.y - position.y) * progress;
            position.z = position.z + (message->target_position.z - position.z) * progress;
        }
        if ((flags & 0x20) != 0) {
            scale += (message->target_scale - scale) * progress;
        }
        position.z += message->parameter_c8;
        position.x += message->parameter_c4;

        if (message->draw_fn != NULL) {
            message->draw_fn(message, &position, scale);
            continue;
        }

        if (NuSpecialExistsFn(&message->special) != 0) {
            const u32 draw_flags = message->flags;
            if ((draw_flags & 0x20000) != 0) {
                DrawPanel3DObjectNoAlpha(position.x, position.y, position.z, scale, scale, scale, message->parameter_e0,
                                         message->parameter_e2, message->parameter_e4, &message->special, 2);
            } else {
                const f32 alpha = (draw_flags & 0x10000) != 0 ? (f32)(message->alpha * 0.0078125) : 1.0f;
                DrawPanel3DObject(position.x, position.y, position.z, scale, scale, scale, message->parameter_e0,
                                  message->parameter_e2, message->parameter_e4, &message->special, 2, alpha);
            }
            if (GameMsg_GetExtraObjFn != NULL) {
                NUSPECIALHANDLE *extra = GameMsg_GetExtraObjFn(message);
                if (extra != NULL && NuSpecialExistsFn(extra) != 0) {
                    const u32 draw_flags = message->flags;
                    if ((draw_flags & 0x20000) != 0) {
                        DrawPanel3DObjectNoAlpha(position.x, position.y, position.z, scale, scale, scale,
                                                 message->parameter_e0, message->parameter_e2, message->parameter_e4,
                                                 extra, 2);
                    } else {
                        const f32 alpha = (draw_flags & 0x10000) != 0 ? (f32)(message->alpha * 0.0078125) : 1.0f;
                        DrawPanel3DObject(position.x, position.y, position.z, scale, scale, scale,
                                          message->parameter_e0, message->parameter_e2, message->parameter_e4, extra, 2,
                                          alpha);
                    }
                }
            }
        } else {
            const char *text = message->text != NULL ? message->text : message->text_buffer;
            Text3DEx((char *)text, position.x, position.y, position.z, scale, scale, scale, message->parameter_fc,
                     message->red, message->green, message->blue, message->alpha);
        }
    }
}

void __cdecl initGameHintSys_LSW(void) {
    RegisterWithHintSys(DrawHint_LSW, Hints_LSW, Game.hint_completion_bits, 3);
    LswForceUsedHintFlagsPc &= ~7u;
}

/* Add to render.c. Native PC 552e00/78; cdecl (special,matrix,alpha). */
i32 __cdecl DrawPanel3DObjectMtx(NUSPECIALHANDLE *special, NUMTX *matrix, f32 alpha) {
    if (!(alpha <= 0.0f) && special != NULL && NuSpecialExistsFn(special) != 0) {
        NuMtxMulVU0(matrix, matrix, NuCameraGetMtx());
        NuSpecialDrawAtAlpha(special, matrix, alpha);
    }
    return 0;
}

void __cdecl Text_ExpandAllButtonStrings(char *, char *);
const u8 HintRGB[5][3] = {{255, 0, 0}, {0, 255, 0}, {0, 127, 255}, {127, 0, 255}, {255, 255, 0}};

void __cdecl DrawHint_LSW(HINT_PC *hint, i32 max_lines) {
    f32 alpha, text_x, text_y, icon_x, pulse, base, wave_x;
    i32 alignment, lines, colour_index;
    u16 pulse_angle, rotation;
    i32 red, green, blue;
    u8 text_alpha;
    char *text;
    char expanded[1024];
    LEVEL_OBJECT_RUNTIME_PC *icon;
    if (hint == NULL || FadeSys.fade != 0.0f)
        return;
    alpha = CurrentHintAlpha();
    if (!(alpha > 0.0f))
        return;
    text_y = (WORLD->current_level->flags & 0x400) != 0 ? 0.0f : -0.7f;
    if ((hint->flags & 4) != 0) {
        alignment = 0;
        text_x = 0.0f;
    } else {
        alignment = 2;
        text_x = (f32)((f64)0.09f - (f64)ICONX);
    }
    icon_x = -ICONX;
    text = TTab[hint->id];
    if (text != NULL) {
        Text_ExpandAllButtonStrings(text, expanded);
        pulse_angle = (u16)(i32)((f64)NuFmod(GlobalTimer.time_elapsed_mod_seconds, 0.5f) * 2.0 * 65536.0);
        pulse = (f32)((f64)NU_SIN_LUT(pulse_angle) * 0.5 + 0.5);
        colour_index = hint->colour_index;
        if (colour_index == -1)
            colour_index = hintsys.state;
        base = (f32)HintRGB[colour_index][0];
        red = (i32)((((f64)base + 255.0) * 0.5 - (f64)base) * (f64)pulse + (f64)base);
        base = (f32)HintRGB[colour_index][1];
        green = (i32)((((f64)base + 255.0) * 0.5 - (f64)base) * (f64)pulse + (f64)base);
        base = (f32)HintRGB[colour_index][2];
        blue = (i32)((((f64)base + 255.0) * 0.5 - (f64)base) * (f64)pulse + (f64)base);
        lines = max_lines == -1 ? 3 : max_lines;
        text_alpha = (u8)(i32)((f64)alpha * 128.0);
        if (alignment == 0) {
            SmartTextEx(expanded, text_x, text_y, 1.0f, 0.5f, 0.5f, 0.5f, 0, red, green, blue, 1.4f, lines, NULL, 0,
                        text_alpha);
            icon_x = (f32)(-(f64)SmartTextLongestWidthPc * 0.5 - (f64)0.075f);
        } else {
            SmartTextEx(expanded, text_x, text_y, 1.0f, 0.5f, 0.5f, 0.5f, alignment, red, green, blue,
                        (f32)(2.0 - (1.0 + (f64)text_x) - (f64)0.15f), lines, NULL, 0, text_alpha);
        }
    }
    icon = &WORLD->level_objects[0xd4];
    if (icon->active == 0)
        return;
    wave_x = (f32)(((f64)icon_x - (f64)-1.1f) * (f64)NU_SIN_LUT((i32)((f64)alpha * 16384.0)) - (f64)1.1f);
    rotation = (u16)(i32)((f64)NuFmod(GameTimer.time_elapsed, 2.5f) / 2.5 * 65536.0);
    DrawPanel3DObjectNoAlpha(wave_x, text_y, 1.0f, 0.5f, 0.5f, 0.5f, 0, rotation, 0, &icon->special, 2);
    if (alignment == 0) {
        rotation = (u16)(i32)((f64)NuFmod(GameTimer.time_elapsed, 2.5f) / 2.5 * 65536.0);
        DrawPanel3DObjectNoAlpha(-wave_x, text_y, 1.0f, 0.5f, 0.5f, 0.5f, 0, rotation, 0,
                                 &WORLD->level_objects[0xd4].special, 2);
    }
}

/* Complete native status-control foundation. No registration/table activation. */
void __cdecl StatusStage_Reset(STATUS_STAGE_s *stage) {
    if (stage != NULL) {
        stage->field_0x14 = -1;
        stage->field_0x18 = 0.0f;
        stage->field_0x12 = 0;
        stage->field_0x1c = 1.0f;
    }
}

void __cdecl NextStatusStage(STATUSPACKET_s *packet) {
    STATUS_STAGE_s *stage = packet->stage;
    if (stage != NULL) {
        if (stage->field_0x14 == -1 && packet->field_0x68 > stage->field_0x18) {
            packet->flags_word |= 2;
            return;
        }
        stage->field_0x12 = 1;
    }
    packet->previous_stage_2 = packet->previous_stage;
    packet->current_gold_brick = (i8)((u8)packet->current_gold_brick + 1);
    packet->previous_stage = packet->stage;
    packet->stage = packet->next_stage;
    packet->next_stage = FindStatusStage(packet->stage_types[packet->current_gold_brick + 1]);
    while (packet->stage == NULL || packet->next_stage == NULL) {
        if (packet->stage == NULL && packet->next_stage != NULL) {
            packet->stage = packet->next_stage;
            packet->next_stage = NULL;
        }
        packet->current_gold_brick = (i8)((u8)packet->current_gold_brick + 1);
        if (packet->current_gold_brick + 1 >= packet->stage_count)
            packet->next_stage = FindStatusStage(10);
        else
            packet->next_stage = FindStatusStage(packet->stage_types[packet->current_gold_brick + 1]);
    }
    StatusStage_Reset(packet->stage);
    packet->flags_word &= ~2u;
    status_prompt = 0;
    stage = packet->stage;
    if ((stage != NULL && stage->type == 11) || stage->type == 10) {
        ResetRumble((RUMBLE_PACKET_RESOURCE_PC *)&packet->player0_rumble_amount);
        ResetRumble((RUMBLE_PACKET_RESOURCE_PC *)&packet->player1_rumble_amount);
    }
    stage = packet->stage;
    if (stage != NULL && stage->type == 11)
        ReCalculateCompletionPoints();
}

void __cdecl RegisterStatusScreen(STATUS_STAGE_s *stages, i32 *unused, const REGISTER_STATUS_PACKET_PC *registration) {
    (void)unused;
    StatusStages = stages;
    StatusPacket.lsw_packet = registration->lsw_packet;
    StatusPacket.init_callback = registration->init_callback;
    StatusPacket.finish_callback = registration->finish_callback;
    StatusPacket.reset_callback = registration->reset_callback;
    StatusPacket.draw_background_callback = registration->draw_background_callback;
    StatusPacket.field_0x68 = registration->stage_delay;
}

void __cdecl ResetStatusPacket_LSW(STATUSPACKET_s *packet) {
    packet->lsw_packet->field_0x00 = 0;
}

i32 __cdecl InitStatusScreen_LSW(WORLDINFO *world, STATUSPACKET_s *packet) {
    (void)world;
    (void)packet;
    return 0;
}

i32 __cdecl FinishStatusPacket_LSW(WORLDINFO *world, STATUSPACKET_s *packet, i32 result) {
    (void)world;
    (void)packet;
    (void)result;
    return 0;
}

void __cdecl DrawStatusBG_LSW(STATUSPACKET_s *packet) {
    if (packet->stage->field_0x14 == -1) {
        packet->stage->field_0x18 = 0.0f;
        packet->stage->field_0x14 = 0;
    }
}

i32 __cdecl AddToCollection(i32);
void __cdecl Text_FillInExtendedSaveInfo(void);
i32 __cdecl TriggerAutoSave(void);

void __cdecl RedBrick_LSW_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
    (void)stage;
    NextStatusStage(packet);
}

void __cdecl RedBrick_LSW_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
    if (stage->field_0x14 == 0) {
        stage->field_0x14 = 1;
        stage->field_0x18 = 0.0f;
        stage->field_0x1c = 5.0f;
    } else if (stage->field_0x14 == 1) {
        f32 previous = stage->field_0x18;
        volatile f32 accumulated = stage->field_0x18;
        accumulated += elapsed;
        stage->field_0x18 = accumulated;
        if (stage->field_0x18 >= stage->field_0x1c)
            NextStatusStage(packet);
        else if (previous < 0.75f && stage->field_0x18 >= 0.75f) {
            PlaySfx("TrueJedi_100pc", NULL);
            NewStatusRumbleBuzz(-1, 0.6f, 0.0f, 0);
        }
    }
}

void __cdecl AllMiniKits_LSW_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
    (void)stage;
    AddToCollection(VehicleBoltId7f195cPc);
    NextStatusStage(packet);
}

void __cdecl AllMiniKits_LSW_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
    if (stage->field_0x14 == 0) {
        stage->field_0x14 = 1;
        stage->field_0x18 = 0.0f;
        stage->field_0x1c = 6.5f;
    } else if (stage->field_0x14 == 1) {
        f32 previous = stage->field_0x18;
        volatile f32 accumulated = stage->field_0x18;
        accumulated += elapsed;
        stage->field_0x18 = accumulated;
        if (stage->field_0x18 >= stage->field_0x1c)
            NextStatusStage(packet);
        else if (previous < 0.5f && stage->field_0x18 >= 0.5f) {
            PlaySfx("StatusAward", NULL);
            NewStatusRumbleBuzz(-1, 0.6f, 0.0f, 0);
        } else if (previous < 4.0f && stage->field_0x18 >= 4.0f)
            PlaySfx("Char_Icon_App", NULL);
    }
}

void __cdecl CollectCharactersOff_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
    (void)stage;
    (void)packet;
}

void __cdecl CollectCharactersOff_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
    if (stage->field_0x14 == 0) {
        stage->field_0x14 = 1;
        stage->field_0x18 = 0.0f;
        stage->field_0x1c = 1.0f;
    } else if (stage->field_0x14 == 1) {
        volatile f32 accumulated = stage->field_0x18;
        accumulated += elapsed;
        stage->field_0x18 = accumulated;
        if (stage->field_0x18 >= stage->field_0x1c)
            NextStatusStage(packet);
    }
}

void __cdecl Save_LSW_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
    if (packet->save_state == 3) {
        f32 accumulated = elapsed + stage->field_0x18;
        stage->field_0x18 = accumulated;
        if (stage->field_0x18 >= stage->field_0x1c) {
            u32 flags = packet->flags_word;
            if ((flags & 0x400) == 0 && packet->prompt_choice == 0 && (flags & 0x40) == 0 &&
                packet->mission_state == 0 && packet->challenge_state == 0)
                packet->flags_word = flags | 0x1000;
            NextStatusStage(packet);
        }
    } else if (packet->save_state == 2) {
        if (MenuInMemoryCard() == 0)
            packet->save_state = 3;
    } else if (packet->save_state == 0) {
        packet->save_state = 1;
        Text_FillInExtendedSaveInfo();
        if (TriggerAutoSave() == 0) {
            packet->save_state = 2;
            NewMenu(1000, -1, -1);
        }
    } else if (memcard_autosaveinprogress == 0)
        packet->save_state = 3;
}

f32 StatusIconYPc = 0;
i32 StatusDrawPlayerIconsPc = 0;

f32 __cdecl StatusIconsOnOff(f32 blend) {
    i32 angle;
    f32 high, low;
    volatile f32 position;
    angle = (i32)((f64)blend * 16384.0);
    high = STATSPOSY;
    low = STATSPOS2YPc;
    position = ((f64)high - (f64)low) * (f64)NuTrigTable[(angle >> 1) & 0x7fff] + (f64)low;
    return position;
}

void __cdecl Coins_LSW_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
    u32 remaining;
    (void)stage;
    StatusIconYPc = StatusIconsOnOff(1.0f);
    remaining = packet->coins_remaining[0] + packet->coins_remaining[1];
    IncreaseScore(packet->score, (u64)remaining, 0);
    packet->coins_remaining[0] = 0;
    packet->coins_remaining[1] = 0;
    NextStatusStage(packet);
}

void __cdecl Coins_LSW_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
    i32 finished = 0;
    switch (stage->field_0x14) {
        case 0:
            stage->field_0x18 = 0.0f;
            stage->field_0x14 = 1;
            stage->field_0x1c = 1.0f;
            StatusDrawPlayerIconsPc = 1;
            break;
        case 1: {
            volatile f32 rounded_elapsed = elapsed + stage->field_0x18;
            f32 accumulated = rounded_elapsed;
            stage->field_0x18 = accumulated;
            if (accumulated >= stage->field_0x1c)
                stage->field_0x14 = 2;
            break;
        }
        case 2: {
            i32 player;
            for (player = 0; player < 2; ++player) {
                u32 remaining = packet->coins_remaining[player];
                if (remaining != 0) {
                    u32 rate = 35, threshold = 100, amount;
                    while (remaining >= threshold) {
                        threshold *= 10u;
                        rate *= 10u;
                    }
                    amount = (u32)((f64)rate * (f64)elapsed);
                    if (amount > remaining)
                        amount = remaining;
                    else if (amount == 0)
                        amount = 1;
                    packet->coins_remaining[player] = remaining - amount;
                    IncreaseScore(packet->score, (u64)amount, 0);
                }
                finished += packet->coins_remaining[player] == 0;
            }
            PlaySfx("PickupCoin", NULL);
            if (finished == 2) {
                stage->field_0x14 = 3;
                stage->field_0x18 = 0.0f;
                stage->field_0x1c = 1.0f;
                if (packet->coins_collected[0] + packet->coins_collected[1] == 0)
                    GameAudio_PlaySfx(0x32, NULL, 0, 0);
                else
                    PlaySfx("Shop_BuyCheat", NULL);
            }
            break;
        }
        case 3: {
            volatile f32 rounded_elapsed = elapsed + stage->field_0x18;
            f32 accumulated = rounded_elapsed;
            stage->field_0x18 = accumulated;
            if (accumulated >= stage->field_0x1c)
                NextStatusStage(packet);
            break;
        }
    }
}

i32 StatusGoldBrickStagePc = 0;
i32 StatusGoldBrickPhasePc = 0;

__declspec(noinline) void __cdecl SetDrawGoldBrick(STATUSPACKET_s *packet, i32 unused) {
    i32 index = packet->current_gold_brick;
    (void)unused;
    if (packet->gold_brick_enabled[index] != 0) {
        STATUS_STAGE_s *stage = packet->stage;
        StatusGoldBrickStagePc = index;
        StatusGoldBrickPhasePc = stage->field_0x14;
    }
}

void __cdecl GoldBrick_LSW_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
    (void)stage;
    if (Game.gold_bricks <= 60)
        NextStatusStage(packet);
}

void __cdecl GoldBrick_LSW_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
    i32 last = (i32)Game.gold_bricks - (i32)packet->previous_gold_bricks - 1;
    char text[256];
    NUVEC position;
    switch (stage->field_0x14) {
        case 0: {
            stage->field_0x18 = 0.0f;
            stage->field_0x14 = 1;
            stage->field_0x1c = (((f64)last * 2.5 + 2.0) + 3.0) + 1.0 + 1.0;
            break;
        }
        case 1: {
            const f32 previous = stage->field_0x18;
            volatile f32 accumulated = stage->field_0x18 + elapsed;
            i32 index;
            stage->field_0x18 = accumulated;
            if (stage->field_0x18 >= stage->field_0x1c) {
                NextStatusStage(packet);
                return;
            }
            for (index = 0; index <= last; ++index) {
                volatile f32 award_time = (f64)index * 2.5 + 2.0;
                if (previous < award_time && stage->field_0x18 >= award_time) {
                    i32 total;
                    i16 message_id;
                    char *message;
                    PlaySfx("TrueJedi_100pc", NULL);
                    NewStatusRumbleBuzz(-1, 0.6f, 0.0f, 0);
                    total = (i32)packet->previous_gold_bricks + index + 1;
                    sprintf(text, "%i/%i", total, GOLDBRICKPOINTS);
                    AddFancyMessage(text, 0.0f, 0.3f, 0.75f, 1.25f, 1, 1);
                    message_id = ((i16 *)packet->field_0x12c)[index];
                    position.x = 0.0f;
                    position.y = 0.0375f;
                    position.z = 1.0f;
                    if (message_id == TextIdPc880f9c) {
                        NuStrCpy(text, TTab[message_id]);
                        if (HubBothTrueJediGoldBricksPc != 0) {
                            NuStrCat(text, CutPlayerTextSeparatorPc);
                            NuStrCat(text, TTab[(packet->field_0xb0 & 0x40) != 0 ? TextIdPc8808a8 : TextIdPc8808a4]);
                        }
                        message = text;
                    } else
                        message = TTab[message_id];
                    AddGameMessage(message, &position, 0.7f, &position, 0.7f, 255, 0, 127, 0x4020, 2.0f);
                    if (total > 60) {
                        IncreaseScore(packet->score, 100000, 0);
                        CoinTotalScale = 1.5f;
                        PlaySfx("Shop_BuyCheat", NULL);
                        Text_MakeScore(100000, text);
                        AddFancyMessage(text, 0.0f, -STATSPOSY, 0.5f, 1.25f, 0, 0);
                    }
                }
            }
        }
    }
}

i32 StatusDemoOptionPc = 0;

/* Native walks the two adjacent 0x14-byte rumble records. */
typedef struct STATUS_RUMBLE_PAIR_PC {
    RUMBLE_PACKET_RESOURCE_PC players[2];
} STATUS_RUMBLE_PAIR_PC;
STATIC_ASSERT(sizeof(RUMBLE_PACKET_RESOURCE_PC) == 0x14);
STATIC_ASSERT(offsetof(STATUSPACKET_s, player1_rumble_amount) - offsetof(STATUSPACKET_s, player0_rumble_amount) ==
              sizeof(RUMBLE_PACKET_RESOURCE_PC));
STATIC_ASSERT(offsetof(STATUSPACKET_s, area_time) ==
              offsetof(STATUSPACKET_s, player0_rumble_amount) + sizeof(STATUS_RUMBLE_PAIR_PC));

void __cdecl StatusRumbleUpdatePc(void) {
    RUMBLE_PACKET_RESOURCE_PC *packet = ((STATUS_RUMBLE_PAIR_PC *)&StatusPacket.player0_rumble_amount)->players;
    i32 player;
    for (player = 0; player < 2; ++player, ++packet) {
        i32 motor1;
        UpdateRumble(packet);
        if (Game.options_save.unknown_0000[player] != 0 && StatusPacket.player_active[player] != 0 &&
            ((u8)GamePadStorage[player].allocated & 0x20) != 0) {
            if (packet->amount > 0.0f)
                motor1 = (i32)((f64)packet->intensity * (f64)packet->amount * 255.0 / (f64)packet->decay_duration);
            else
                motor1 = 0;
            if (packet->duration > 0.0f || packet->active > 0)
                NuPadSetMotorsPc(GamePadStorage[player].pad, 1, motor1);
            else
                NuPadSetMotorsPc(GamePadStorage[player].pad, 0, motor1);
        } else {
            NuPadSetMotorsPc(GamePadStorage[player].pad, 0, 0);
        }
    }
}

void __cdecl UpdateStatusScreen(WORLDINFO *world) {
    const f32 elapsed = FRAMETIME;
    volatile f32 seek_step = (f64)elapsed * 3.0;
    i32 player;
    i32 select = 0;
    i32 up = 0;
    i32 down = 0;
    (void)world;
    CoinTotalScale = SeekLinearF(CoinTotalScale, 1.0f, seek_step);
    StatusGoldBrickPhasePc = -1;
    StatusGoldBrickStagePc = -1;
    switch (GAMEDEMO) {
        case 0:
            break;
        case 1:
            if (FadeSys.fade != 0.0f || NewLData != NULL)
                return;
            {
                for (player = 0; player < 2; ++player) {
                    u32 pressed, directions;
                    if (select != 0 || up != 0 || down != 0)
                        break;
                    if (StatusPacket.player_active[player] == 0)
                        continue;
                    pressed = GamePadStorage[player].menu_buttons_pressed;
                    if ((pressed & GAMEPAD_MENUSELECT) != 0)
                        select = 1;
                    else {
                        directions = GamePadStorage[player].menu_directions;
                        if ((directions & GAMEPAD_DUP) != 0 || (pressed & GAMEPAD_DUP) != 0)
                            up = 1;
                        else if ((directions & GAMEPAD_DDOWN) != 0 || (pressed & GAMEPAD_DDOWN) != 0)
                            down = 1;
                    }
                }
                if (select != 0) {
                    GameAudio_PlaySfx(0x30, NULL, 0, 0);
                    switch (StatusDemoOptionPc) {
                        case 0:
                            GAMEDEMO = 2;
                            NewLData = HUB_LDATA;
                            new_level_from_menu = 1;
                            break;
                        case 1:
                            SendMessageA(PCInputInstance_candidate, WM_CLOSE, 0, 0);
                            break;
                    }
                } else if (up != 0) {
                    if (StatusDemoOptionPc > 0) {
                        --StatusDemoOptionPc;
                        GameAudio_PlaySfx(0x2f, NULL, 0, 0);
                    }
                } else if (down != 0) {
                    if (StatusDemoOptionPc < 1) {
                        ++StatusDemoOptionPc;
                        GameAudio_PlaySfx(0x2f, NULL, 0, 0);
                    }
                }
            }
            return;
        case 2:
            if (GameTimer.time_elapsed >= 8.0f)
                SendMessageA(PCInputInstance_candidate, WM_CLOSE, 0, 0);
            return;
        default:
            return;
    }
    if (StatusPacket.status_flags == 0)
        return;
    if (!(StatusPacket.newly_completed != 0 && StatusPacket.area != NULL && StatusPacket.area == NEGOTIATIONS_ADATA)) {
        u32 button = Text_Language != 0 ? GAMEPAD_JUMP : GAMEPAD_MENUSELECT;
        if ((StatusPacket.player0_active != 0 && (GamePadStorage[0].menu_buttons_pressed & button) != 0) ||
            (StatusPacket.player1_active != 0 && (GamePadStorage[1].menu_buttons_pressed & button) != 0) ||
            (StatusPacket.field_0xb0 & 2) != 0) {
            STATUS_STAGE_s *stage = StatusPacket.stage;
            if (stage == NULL)
                goto status_done;
            if (stage->skip_callback != NULL)
                stage->skip_callback(stage, &StatusPacket);
        }
    }
    if (StatusPacket.stage != NULL && StatusPacket.stage->update_callback != NULL)
        StatusPacket.stage->update_callback(StatusPacket.stage, &StatusPacket, elapsed);
status_done:
    StatusRumbleUpdatePc();
    if (StatusPacket.status_flags == 0 && GameTimer.time_elapsed >= 5.0f)
        FinishStatusPacket(0);
}

i32 StatusNewMinikitCountPc = 0;
i16 StatusNewCharacterIdsPc[8] = {0};
i32 StatusCurrentMinikitPc = 0;
u32 StatusCollectedSuitBitsPc = 0;

void __cdecl StatusPacketReset(STATUSPACKET_s *packet) {
    struct STATUSPACKET_LSW_s *lsw = packet->lsw_packet;
    i32(__cdecl * init)(WORLDINFO *, STATUSPACKET_s *) = packet->init_callback;
    i32(__cdecl * finish)(WORLDINFO *, STATUSPACKET_s *, i32) = packet->finish_callback;
    void(__cdecl * reset)(STATUSPACKET_s *) = packet->reset_callback;
    void(__cdecl * draw)(STATUSPACKET_s *) = packet->draw_background_callback;
    f32 delay = packet->field_0x68;
    reset(packet);
    memset(packet, 0, sizeof(*packet));
    packet->field_0x68 = delay;
    packet->reset_callback = reset;
    packet->lsw_packet = lsw;
    packet->init_callback = init;
    packet->finish_callback = finish;
    packet->draw_background_callback = draw;
}

f32 __cdecl StatusScoreRatioPc(f32 numerator, f32 denominator) {
    if (numerator != 0.0f && denominator != 0.0f) {
        volatile f32 result = (f64)numerator / (f64)denominator;
        return result;
    }
    return 0.0f;
}

i32 __cdecl newCharactersCollected(STATUSPACKET_s *packet) {
    i32 count = 0;
    if (StartupGameFlag1 == 0) {
        if (((u32)packet->lsw_packet->field_0x00 & 0x01000000u) != 0 && Game.initial_store_pack_flags != areaSuitBits)
            return 1;
    } else {
        i16 temple_ids[2];
        i16 *ids;
        temple_ids[0] = VehicleBoltId7f17d4Pc;
        temple_ids[1] = -1;
        ids = packet->area != NULL && packet->area == LOSTTEMPLE_ADATA ? temple_ids : Area_PlayerIDList;
        while (*ids != -1 && count < 7) {
            i32 id = *ids, previous = 0;
            while (previous < count && StatusNewCharacterIdsPc[previous] != id)
                ++previous;
            if (previous == count && (Game_CharacterSave == NULL || (Game_CharacterSave[id] & 1) == 0)) {
                i32 found = InCollectList_Index(id, MasterCollectionPc.list, MasterCollectionPc.count);
                if (found != -1) {
                    COLLECTID_PC *entry = &MasterCollectionPc.list[found];
                    if (entry->shop_enabled == 0) {
                        StatusNewCharacterIdsPc[count++] = (i16)id;
                        if (entry->unknown_price_tail[0] != 0)
                            AddToCompletionPoints(POINTS_PER_CHARACTER);
                    }
                }
            }
            ++ids;
        }
        StatusNewCharacterIdsPc[count] = -1;
    }
    return count;
}

typedef struct StatusPlayerModelPairPc {
    u16 model[2];
} StatusPlayerModelPairPc;
STATIC_ASSERT(sizeof(StatusPlayerModelPairPc) == 4);
STATIC_ASSERT(offsetof(STATUSPACKET_s, player1_model) == offsetof(STATUSPACKET_s, player0_model) + sizeof(u16));
STATIC_ASSERT(offsetof(STATUSPACKET_s, area_id) ==
              offsetof(STATUSPACKET_s, player0_model) + sizeof(StatusPlayerModelPairPc));

#define STATUS_APPEND_STAGE_INLINE_PC(packet_arg, type_arg, gold_arg)                                                  \
    do {                                                                                                               \
        (packet_arg)->stage_types[(packet_arg)->stage_count] = (i8)(type_arg);                                         \
        (packet_arg)->gold_brick_enabled[(packet_arg)->stage_count] = (u8)(gold_arg);                                  \
        ++(packet_arg)->stage_count;                                                                                   \
    } while (0)

void __cdecl InitStatusScreen(WORLDINFO *world) {
    STATUSPACKET_s *p = &StatusPacket;
    i32 i, area, episode;
    if (p->lsw_packet == NULL)
        return;
    episode = -1;
    for (i = 0; StatusStages[i].type != -1; ++i)
        StatusStage_Reset(&StatusStages[i]);
    StatusPacketReset(p);
    StatusIconYPc = STATSPOSY;
    p->flags_word &= ~1u;
    StatusNewMinikitCountPc = 0;
    StatusCurrentMinikitPc = 0;
    ResetRumble((RUMBLE_PACKET_RESOURCE_PC *)&p->player0_rumble_amount);
    ResetRumble((RUMBLE_PACKET_RESOURCE_PC *)&p->player1_rumble_amount);
    CoinTotalScale = 1.0f;
    goldbrickmsgcount = 0;
    if (netclient == 0) {
        StatusPlayerModelPairPc *models = (StatusPlayerModelPairPc *)&p->player0_model;
        for (i = 0; i < 2; ++i) {
            GAMEOBJECT *object = Player[i];
            if (object != NULL) {
                models->model[i] = (u16)object->id;
                p->player_active[i] = (u8)((object->flags_1fc_word >> 7) & 1);
                if (object->coin_packet != NULL) {
                    p->coins_remaining[i] = ((PLAYER_COIN_PACKET_PC *)object->coin_packet)->coins;
                    ++p->status_flags;
                } else {
                    p->coins_remaining[i] = 0;
                    ++p->status_flags;
                }
            } else
                models->model[i] = 0xffff;
        }
    }
    if (p->status_flags != 2) {
        p->status_flags = 0;
        return;
    }
    p->flags_word = (p->flags_word & ~0x400u) | (((u32)SuperStory << 10) & 0x400u);
    p->flags_word = (p->flags_word & ~0x40u) | (((u32)FreePlay << 6) & 0x40u);
    p->flags_word = (p->flags_word & ~0x800u) | (((u32)PauseSaveAndExitPc << 11) & 0x800u);
    p->flags_word &= ~0x1000u;
    p->stage_count = 0;
    p->mission = Mission_Active(NULL);
    p->mission_state = p->mission == NULL ? 0 : (u8)Mission_CurrentState(NULL);
    p->challenge_state = (u8)ChallengeMode;
    PauseSaveAndExitPc = 0;
    area = (i8)world->area_index;
    p->flags_word &= ~8u;
    p->previous_completion = (u8)Game.save_hash;
    p->flags_word &= ~0x380u;
    p->next_area = -1;
    p->field_0xbe = -1;
    p->score = &Game.coins;
    p->previous_gold_bricks = Game.gold_bricks;
    p->displayed_gold_bricks = Game.gold_bricks;
    p->newly_completed = 0;
    p->field_0xbd = 0;
    p->area_id = (i16)area;
    if (area != -1) {
        p->area = &ADataList[area];
        p->episode_id = p->area->episode_index;
        episode = p->episode_id;
        if (episode != -1)
            p->episode = &EDataList[(i8)p->area->episode_index];
        p->chapter = p->area->area_index;
        if ((p->area->flags & 1) != 0)
            p->flags_word |= 0x80;
        if ((p->area->flags & 4) != 0)
            p->flags_word |= 0x100;
        if ((p->area->flags & 0x100) != 0)
            p->flags_word |= 0x200;
    } else {
        p->area = NULL;
        p->episode = NULL;
        p->chapter = -1;
    }
    if (p->init_callback(world, p) != 0)
        return;
    if ((p->flags_word & 0x400) != 0) {
        if (NewLData == STATUS_LDATA || WORLD->current_level == STATUS_LDATA) {
            GAME_EPISODE_START_PC *save = &Game.episode_start[episode];
            i32 gold = 0;
            p->previous_best_time = save->superstory_time_limit;
            p->previous_best_score = save->superstory_score_target;
            p->superstory_time = SuperStoryTimer.time_elapsed;
            p->superstory_score = SuperStoryScore;
            if (episode != -1) {
                if (p->previous_best_time > p->superstory_time) {
                    p->new_best_time = p->superstory_time;
                    save->superstory_time_limit = p->superstory_time;
                } else
                    p->new_best_time = 0.0f;
                if (p->superstory_score > p->previous_best_score) {
                    p->new_best_score = p->superstory_score;
                    save->superstory_score_target = p->superstory_score;
                } else
                    p->new_best_score = 0;
                if (save->complete == 0) {
                    save->complete = 1;
                    AddToCompletionPoints(POINTS_PER_SUPERSTORY);
                    if (EpisodeGoldBrickForSuperStoryPc != 0)
                        gold = AddGoldBrickMessage(p, TextIdPc88073c);
                }
            } else {
                p->new_best_score = 0;
                p->new_best_time = 0.0f;
            }
            p->time_reward_score = *p->score;
            if (p->new_best_time != 0.0f) {
                volatile i32 whole = (i32)((f64)p->previous_best_time - (f64)p->new_best_time);
                i32 scaled = (i32)((u32)whole * 100u);
                u64 reward = (u64)(i64)scaled;
                if (reward < 100)
                    reward = 100;
                IncreaseScore(&p->time_reward_score, reward, 0);
            }
            p->final_reward_score = p->time_reward_score;
            IncreaseScore(&p->final_reward_score, (u64)p->superstory_score, 0);
            STATUS_APPEND_STAGE_INLINE_PC(p, 14, gold);
            STATUS_APPEND_STAGE_INLINE_PC(p, 15, 0);
            STATUS_APPEND_STAGE_INLINE_PC(p, 16, 0);
            if (Game.gold_bricks > p->previous_gold_bricks)
                AddStatusStage(p, 19, 0);
            if (netclient == 0 && (p->new_best_time != 0.0f || p->new_best_score != 0 || p->superstory_score != 0))
                AddStatusStage(p, 11, 0);
            hub_from_superstory = (i8)p->area->episode_index;
        }
        STATUS_APPEND_STAGE_INLINE_PC(p, 10, 0);
        p->stage_types[p->stage_count] = -1;
        p->current_gold_brick = -1;
        p->next_stage = FindStatusStage(p->stage_types[0]);
        NextStatusStage(p);
        if (episode != -1 && p->chapter < EDataList[episode].regular_areas) {
            p->next_area = EDataList[episode].area_ids[p->chapter + 1];
            if (p->next_area != -1 && (ADataList[p->next_area].flags & 2) != 0)
                hub_from_superstory = episode;
        }
        return;
    }
    if ((p->flags_word & 0x100) != 0 && area != -1) {
        i32 gold = 0;
        p->elapsed_time = BonusTimer.time_elapsed;
        p->flags_word = (p->flags_word & ~0x20u) | (((u32)BonusWinner << 5) & 0x20u);
        p->previous_best_time = Game.area_save[area].challenge_trial_time;
        if (p->previous_best_time <= 0.0f || p->previous_best_time > p->elapsed_time) {
            if (WORLD->area->challenge_trial_time != 0 && p->previous_best_time >= WORLD->area->challenge_trial_time) {
                AddToCompletionPoints(POINTS_PER_TIMETRIAL);
                gold = AddGoldBrickMessage(p, TextIdPc88071c);
            }
            p->new_best_time = p->elapsed_time;
            Game.area_save[area].challenge_trial_time = p->elapsed_time;
        }
        if ((p->flags_word & 0x200) != 0) {
            if (Game.area_save[p->area_id].area_complete == 0) {
                Game.area_save[p->area_id].area_complete = 1;
                AddToCompletionPoints(POINTS_PER_SUPERBONUSCOMPLETE);
                p->newly_completed = 1;
                if (EpisodeGoldBrickForSuperBonusPc != 0)
                    gold = AddGoldBrickMessage(p, TextIdPc880728);
            }
            AddStatusStage(p, 22, gold);
            gold = 0;
        } else
            AddStatusStage(p, 20, 0);
        AddStatusStage(p, 21, gold);
        if (Game.gold_bricks > p->previous_gold_bricks)
            AddStatusStage(p, 19, 0);
        AddStatusStage(p, 9, 0);
        AddStatusStage(p, 10, 0);
        if (netclient == 0)
            AddStatusStage(p, 11, 0);
        AddStatusStage(p, 12, 0);
        p->stage_types[p->stage_count] = -1;
        p->current_gold_brick = -1;
        p->next_stage = FindStatusStage(p->stage_types[0]);
        NextStatusStage(p);
        StatusDrawPlayerIconsPc = 1;
        return;
    }
    if (p->mission_state != 0) {
        i32 gold = 0;
        p->newly_completed = 0;
        p->original_score = *p->score;
        p->reward_score = p->original_score;
        p->elapsed_time = MissionSys->timer.time_elapsed;
        if (p->mission_state == 2) {
            if (Game_MissionSave == NULL || Game_MissionSave->completed[(i8)MissionSys->active_mission->index] == 0) {
                if (Game_MissionSave != NULL)
                    Game_MissionSave->completed[(i8)MissionSys->active_mission->index] = 1;
                AddToCompletionPoints(POINTS_PER_MISSION);
                gold = AddGoldBrickMessage(p, TextIdPc8806e4);
            }
            {
                volatile i32 whole = (i32)((f64)p->mission->time - (f64)p->elapsed_time);
                i32 scaled = (i32)((u32)whole * 150u);
                u64 reward = (u64)(i64)scaled;
                if (reward < 150)
                    reward = 150;
                IncreaseScore(&p->reward_score, reward, 0);
            }
            if (Game_MissionSave != NULL) {
                f32 *best = &Game_MissionSave->best_times[(i8)MissionSys->active_mission->index];
                if (*best == 0.0f || *best > p->elapsed_time)
                    *best = p->elapsed_time;
            }
        }
        AddStatusStage(p, 26, gold);
        if (Game.gold_bricks > p->previous_gold_bricks)
            AddStatusStage(p, 19, 0);
        STATUS_APPEND_STAGE_INLINE_PC(p, 9, 0);
        STATUS_APPEND_STAGE_INLINE_PC(p, 10, 0);
        if (netclient == 0)
            AddStatusStage(p, 11, 0);
        STATUS_APPEND_STAGE_INLINE_PC(p, 12, 0);
        p->stage_types[p->stage_count] = -1;
        p->current_gold_brick = -1;
        p->next_stage = FindStatusStage(p->stage_types[0]);
        NextStatusStage(p);
        StatusDrawPlayerIconsPc = 1;
        cointotal_i_obj[1] = cointotal_i_obj[0] = 0xc3;
        return;
    }
    if (p->challenge_state != 0) {
        i32 first = 0;
        p->elapsed_time = ChallengeTimer.time_elapsed;
        p->original_score = *p->score;
        p->reward_score = p->original_score;
        if (p->challenge_state == 2) {
            if (Game.area_save[p->area_id].challenge_unlocked == 0) {
                AddToCompletionPoints(POINTS_PER_CHALLENGE);
                Game.area_save[p->area_id].challenge_unlocked = 1;
                first = 1;
            }
            {
                volatile i32 whole = (i32)((f64)ADataList[p->area_id].challenge_trial_time - (f64)p->elapsed_time);
                i32 scaled = (i32)((u32)whole * 500u);
                u64 reward = (u64)(i64)scaled;
                if (reward < 500)
                    reward = 500;
                IncreaseScore(&p->reward_score, reward, 0);
            }
        }
        AddStatusStage(p, 23, 0);
        if (p->challenge_state == 2 && first != 0)
            AddStatusStage(p, 25, 0);
        AddStatusStage(p, 9, 0);
        AddStatusStage(p, 10, 0);
        if (netclient == 0)
            AddStatusStage(p, 11, 0);
        AddStatusStage(p, 12, 0);
        p->stage_types[p->stage_count] = -1;
        p->current_gold_brick = -1;
        p->next_stage = FindStatusStage(p->stage_types[0]);
        NextStatusStage(p);
        StatusDrawPlayerIconsPc = 1;
        cointotal_i_obj[1] = cointotal_i_obj[0] = 0xc3;
        return;
    }
    if (area != -1) {
        if (BuildUpDone != 0)
            p->flags_word |= 4;
        p->minikit_count = (u8)(Game.area_save[area].minikit_count + (u8)AreaGlobals_PC.new_minikit_count);
        p->minikit_max = (p->area->flags & 0x10) != 0 ? 10 : 0;
        if (p->minikit_count > p->minikit_max)
            p->minikit_count = p->minikit_max;
        p->true_hero_target = (f32)p->area->true_hero_targets[(p->flags_word >> 6) & 1];
    }
    p->area_time = AreaTimer.time_elapsed;
    if ((u64)p->coins_remaining[0] + (u64)p->coins_remaining[1] >= 4000000000ULL)
        p->collected_score = 4000000000.0f;
    else
        p->collected_score = (f32)(p->coins_remaining[0] + p->coins_remaining[1]);
    p->newly_completed = 0;
    if ((p->field_0xb0 & 4) != 0)
        p->true_hero_percent = 100.0f;
    else {
        volatile f32 target = p->true_hero_target;
        volatile f32 numerator = (f64)p->collected_score * 100.0;
        p->true_hero_percent = StatusScoreRatioPc(numerator, target);
        if (p->true_hero_percent > 99.0f)
            p->true_hero_percent = 99.0f;
    }
    p->coins_collected[0] = p->coins_remaining[0];
    p->coins_collected[1] = p->coins_remaining[1];
    if (area != -1 && (p->field_0xb0 & 0x40) == 0 && p->episode != NULL && p->chapter < p->episode->regular_areas) {
        p->next_area = p->episode->area_ids[p->chapter + 1];
        if (p->chapter < p->episode->regular_areas - 1)
            p->flags_word |= 8;
    }
    StatusDrawPlayerIconsPc = 0;
    if (area != -1) {
        for (i = 0; i < AreaGlobals_PC.new_minikit_count; ++i) {
            LEVELSAVE_s *save = &((LEVELSAVE_s *)Game.unknown_0011)[NewMiniPiece[i].level];
            if (Game.area_save[area].minikit_count < 10)
                ++Game.area_save[area].minikit_count;
            if (save->minikit_count < 10) {
                NuStrCpy(save->minikit_names[save->minikit_count], NewMiniPiece[i].name);
                ++save->minikit_count;
            }
        }
    }
    {
        i32 gold = 0, completed_episode = 0;
        if (area != -1 && (p->field_0xb0 & 0x40) == 0 && Game.area_save[area].area_complete == 0 &&
            (p->area->flags & 0x26) == 0) {
            p->newly_completed = 1;
            Game.area_save[area].area_complete = 1;
            AddToCompletionPoints(POINTS_PER_STORY);
            if ((p->flags_word & 0x100) == 0 && (p->area->flags & 0x800) == 0)
                gold = AddGoldBrickMessage(p, TextIdPc880728);
            if (p->episode != NULL && Episode_IsComplete(p->episode, NULL) != 0)
                completed_episode = (i8)p->area->episode_index;
            if (StartupGameFlag1 != 0 && p->area == NEGOTIATIONS_ADATA) {
                for (i = 1; i < 6; ++i)
                    Game.area_save[EDataList[i].area_ids[0]].complete = 1;
            }
            if ((p->field_0xb0 & 8) != 0)
                Game.area_save[p->next_area].complete = 1;
            else if (p->episode != NULL) {
                i32 bonus = Episode_FindAreaFromFlags(p->episode, 5, 4);
                i32 bonus2 = Episode_FindAreaFromFlags(p->episode, 5, 5);
                if (bonus != -1)
                    Game.area_save[bonus].complete = 1;
                if (bonus2 != -1)
                    Game.area_save[bonus2].complete = 1;
                hub_startoutsidebonusdoor_area = bonus2;
            }
        }
        if (p->newly_completed != 0)
            AddStatusStage(p, 13, gold);
        if (completed_episode != 0)
            AddStatusStage(p, completed_episode + 29, 0);
    }
    if (GOTHAMSTREETS_ADATA != NULL && p->newly_completed != 0 && p->area == GOTHAMSTREETS_ADATA)
        AddStatusStage(p, 8, 0);
    if (p->area != NULL && (u8)p->area->cheat != 0xff && Game.area_save[area].red_brick_collected == 0 &&
        AreaGlobals_PC.field_08 != 0) {
        AddToCompletionPoints(POINTS_PER_REDBRICK);
        Game.area_save[area].red_brick_collected = 1;
        AddStatusStage(p, 18, 0);
    }
    if (p->newly_completed != 0 && newCharactersCollected(p) > 0) {
        if (StartupGameFlag1 == 0) {
            if (((u32)p->lsw_packet->field_0x00 & 0x01000000u) != 0)
                AddStatusStage(p, 5, 0);
            else {
                AddStatusStage(p, 6, 0);
                AddStatusStage(p, 7, 0);
            }
        } else {
            AddStatusStage(p, 35, 0);
            AddStatusStage(p, 36, 0);
        }
        RememberPlayerIDs(1, (i16)p->player0_model, (i16)p->player1_model);
    }
    if (area != -1 && (p->area->flags & 0x136) == 0x10) {
        if (HubBothTrueJediGoldBricksPc != 0) {
            u8 *complete = &Game.area_save[area].true_hero_complete[(p->flags_word >> 6) & 1];
            if (*complete == 0) {
                if ((p->field_0xb0 & 4) != 0) {
                    *complete = 1;
                    AddToCompletionPoints(POINTS_PER_TRUEJEDI);
                    AddStatusStage(p, 1, AddGoldBrickMessage(p, TextIdPc880818));
                } else if (p->collected_score > 0.0f)
                    AddStatusStage(p, 2, 0);
            }
        } else if (Game.area_save[area].true_hero_complete[0] == 0 && Game.area_save[area].true_hero_complete[1] == 0) {
            if ((p->field_0xb0 & 4) != 0) {
                Game.area_save[area].true_hero_complete[1] = 1;
                Game.area_save[area].true_hero_complete[0] = 1;
                AddToCompletionPoints(POINTS_PER_TRUEJEDI);
                AddStatusStage(p, 1, AddGoldBrickMessage(p, TextIdPc880818));
            } else if (p->collected_score > 0.0f && (p->flags_word & 0x800) == 0)
                AddStatusStage(p, 2, 0);
        }
    }
    if (p->collected_score > 0.0f)
        AddStatusStage(p, 3, 0);
    if (p->minikit_max != 0 && (p->area->flags & 0x36) == 0x10 && Game.area_save[area].minikit_complete == 0 &&
        AreaGlobals_PC.new_minikit_count > 0) {
        i32 gold = 0;
        p->new_minikits = AreaGlobals_PC.new_minikit_count;
        if (Game.area_save[area].minikit_count >= p->minikit_max) {
            Game.area_save[area].minikit_complete = 1;
            p->flags_word |= 0x10;
            AddToCompletionPoints(POINTS_PER_MINIKIT);
            gold = AddGoldBrickMessage(p, TextIdPc8808d0);
        }
        if (PauseSaveAndExitPc == 0 || p->new_minikits != 0)
            AddStatusStage(p, 4, gold);
        if ((p->field_0xb0 & 0x10) != 0 && AllMiniKitsDone(Game.area_save) != 0) {
            AddToCollection(VehicleBoltId7f195cPc);
            AddStatusStage(p, 17, 0);
        }
    }
    if (Game.gold_bricks > p->previous_gold_bricks)
        AddStatusStage(p, 19, 0);
    if ((p->flags_word & 0x800) == 0)
        AddStatusStage(p, 9, 0);
    STATUS_APPEND_STAGE_INLINE_PC(p, 10, 0);
    if (netclient == 0)
        AddStatusStage(p, 11, 0);
    STATUS_APPEND_STAGE_INLINE_PC(p, 12, 0);
    p->stage_types[p->stage_count] = -1;
    p->current_gold_brick = -1;
    p->next_stage = FindStatusStage(p->stage_types[0]);
    NextStatusStage(p);
    StatusCollectedSuitBitsPc = Game.initial_store_pack_flags;
    Game.initial_store_pack_flags = areaSuitBits;
}
#undef STATUS_APPEND_STAGE_INLINE_PC

void __cdecl BonusComplete_LSW_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
    (void)stage;
    NextStatusStage(packet);
}

void __cdecl BonusComplete_LSW_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
    switch (stage->field_0x14) {
        case 0: {
            stage->field_0x18 = 0.0f;
            stage->field_0x14 = 1;
            if (packet->newly_completed != 0) {
                volatile f32 duration = 4.0f;
                stage->field_0x1c = duration;
            } else {
                volatile f32 duration = 0.5f;
                stage->field_0x1c = duration;
            }
            StatusDrawPlayerIconsPc = 1;
            break;
        }
        case 1: {
            SetDrawGoldBrick(packet, packet->current_gold_brick);
            {
                volatile f32 previous = stage->field_0x18;
                volatile f32 rounded_elapsed = stage->field_0x18 + elapsed;
                f32 accumulated = rounded_elapsed;
                stage->field_0x18 = accumulated;
                if (stage->field_0x1c <= accumulated)
                    NextStatusStage(packet);
                else if (packet->newly_completed != 0 && 0.5f > previous && 0.5f <= accumulated) {
                    PlaySfx("StatusAward", NULL);
                    NewStatusRumbleBuzz(-1, 0.6f, 0.0f, 0);
                }
            }
        }
    }
}

void __cdecl BonusTime_LSW_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
    (void)stage;
    (void)packet;
}

void __cdecl BonusTime_LSW_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
    switch (stage->field_0x14) {
        case 0: {
            stage->field_0x14 = 1;
            stage->field_0x18 = 0.0f;
            stage->field_0x1c = 4.0f;
            break;
        }
        case 1: {
            SetDrawGoldBrick(packet, packet->current_gold_brick);
            {
                volatile f32 previous = stage->field_0x18;
                volatile f32 rounded_elapsed = stage->field_0x18 + elapsed;
                f32 accumulated = rounded_elapsed;
                stage->field_0x18 = accumulated;
                if (stage->field_0x1c <= accumulated)
                    NextStatusStage(packet);
                else if (0.5f > previous && 0.5f <= accumulated) {
                    if (packet->new_best_time == 0.0f)
                        GameAudio_PlaySfx(0x32, NULL, 0, 0);
                    else
                        PlaySfx("StatusAward", NULL);
                    NewStatusRumbleBuzz(-1, 0.6f, 0.0f, 0);
                }
            }
        }
    }
}

i32 StatusRedPc = 255;
i32 StatusGreenPc = 191;
i32 StatusBluePc = 0;
f32 StatusTitleYPc = 0.1f;
f32 StatusTextScalePc = 0.7f;
f32 StatusBonusTimeYPc = -0.5f;

f32 __cdecl getFinishedStatusAlpha(STATUSPACKET_s *packet) {
    STATUS_STAGE_s *stage = packet->stage;
    if (stage->type == 12 || stage->type == 11)
        return 0.0f;
    if (stage->type == 10) {
        if (stage->field_0x18 < 1.0f) {
            volatile f32 alpha = 1.0 - (f64)stage->field_0x18;
            return alpha;
        }
        return 0.0f;
    }
    if (stage->type == 19) {
        if (stage->field_0x14 == 0)
            return 1.0f;
        if (stage->field_0x18 < 1.0f) {
            volatile f32 alpha = 1.0 - (f64)stage->field_0x18;
            return alpha;
        } else {
            volatile f32 fade_start = (f64)stage->field_0x1c - 1.0;
            if (stage->field_0x18 >= fade_start) {
                volatile f32 alpha =
                    ((f64)stage->field_0x18 - (f64)fade_start) / ((f64)stage->field_0x1c - (f64)fade_start);
                return alpha;
            }
            return 0.0f;
        }
    }
    return stage == NULL ? 0.0f : 1.0f;
}

void __cdecl DrawBonusTime(STATUSPACKET_s *packet, f32 position, i32 alpha) {
    volatile f32 y = StatusBonusTimeYPc * position;
    char time[256];
    char previous[256];
    char *title;
    i32 red, green;
    if (packet->new_best_time != 0.0f) {
        title = TTab[TextIdPc88071c];
        red = 0;
        green = 255;
    } else {
        title = TTab[TextIdPc880734];
        red = 255;
        green = 0;
    }
    {
        volatile f32 title_y = (f64)y + (f64)0.15f;
        SmartTextEx(title, 0.0f, title_y, 1.0f, 0.7f, 0.7f, 0.7f, 0, (u8)red, (u8)green, 0, 1.7f, 1, NULL, 0,
                    (u8)alpha);
    }
    {
        volatile f32 value = packet->new_best_time != 0.0f ? packet->new_best_time : packet->elapsed_time;
        Text_MakeTime(value, 1, 1, 1, time);
    }
    {
        volatile f32 time_y = (f64)y + 0.0;
        Text3DEx(time, 0.0f, time_y, 1.0f, 0.7f, 0.7f, 0.7f, 0, 255, 255, 255, alpha);
    }
    Text_MakeTime(packet->previous_best_time, 1, 1, 1, time);
    NuStrCpy(previous, "(");
    NuStrCat(previous, time);
    NuStrCat(previous, ")");
    {
        volatile f32 previous_y = (f64)y - (f64)0.15f;
        Text3DEx(previous, 0.0f, previous_y, 1.0f, 0.7f, 0.7f, 0.7f, 0, 255, 255, 255,
                 (i32)(((u32)alpha & 0xffffff00u) | (((u32)alpha & 0xffu) >> 1)));
    }
}

void __cdecl BonusComplete_LSW_Draw(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, i32 current) {
    if (current == 0)
        return;
    if (stage->field_0x14 > 0 && packet->newly_completed != 0) {
        volatile f32 alpha;
        if (stage->field_0x18 < 0.5f)
            alpha = (f64)stage->field_0x18 * 2.0;
        else if (stage->field_0x18 < 3.5)
            alpha = 1.0f;
        else
            alpha = 1.0 - ((f64)stage->field_0x18 - 3.5) * 2.0;
        if (alpha > 0.0f) {
            volatile i32 opacity = (i32)((f64)alpha * 128.0);
            SmartTextEx(TTab[TextIdPc880728], 0.0f, StatusTitleYPc, 1.0f, StatusTextScalePc, StatusTextScalePc,
                        StatusTextScalePc, 0, (u8)StatusRedPc, (u8)StatusGreenPc, (u8)StatusBluePc, 1.7f, 1, NULL, 0,
                        (u32)opacity);
        }
    }
    if (stage->field_0x14 == 0)
        iconalphaoverride = 0.0f;
    else if (packet->newly_completed == 0)
        iconalphaoverride = stage->field_0x18 / stage->field_0x1c;
    else {
        volatile f32 fade_start = (f64)stage->field_0x1c - 0.5;
        if (stage->field_0x18 < fade_start)
            iconalphaoverride = 0.0f;
        else
            iconalphaoverride = ((f64)stage->field_0x18 - (f64)fade_start) * 2.0;
    }
}

void __cdecl BonusTime_LSW_Draw(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, i32 current) {
    if (current != 0) {
        if (stage->field_0x14 > 0) {
            volatile f32 alpha, time = 0.0f;
            if (stage->field_0x18 < 0.5f)
                alpha = (f64)stage->field_0x18 + (f64)stage->field_0x18;
            else {
                alpha = 1.0f;
                if (!(stage->field_0x18 < 3.0f))
                    time = (f64)stage->field_0x18 - 3.0;
            }
            {
                volatile i32 opacity = (i32)((f64)alpha * 128.0);
                volatile i32 angle = (i32)((f64)time * 32768.0 + 16384.0);
                volatile f32 position = 1.0 - ((f64)NuTrigTable[(angle >> 1) & 0x7fff] + 1.0) * 0.5;
                DrawBonusTime(packet, position, opacity);
            }
        }
    } else if (stage->field_0x12 != 0) {
        volatile i32 opacity = (i32)((f64)getFinishedStatusAlpha(packet) * 128.0);
        DrawBonusTime(packet, 1.0f, opacity);
    }
}

void __cdecl SuperStoryTime_LSW_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
    (void)stage;
    *packet->score = packet->time_reward_score;
    NextStatusStage(packet);
}

void __cdecl SuperStoryTime_LSW_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
    switch (stage->field_0x14) {
        case 0: {
            stage->field_0x18 = 0.0f;
            stage->field_0x14 = 1;
            stage->field_0x1c = 5.0f;
            packet->original_score = *packet->score;
            break;
        }
        case 1: {
            volatile f32 previous = stage->field_0x18;
            volatile f32 rounded_elapsed = stage->field_0x18 + elapsed;
            f32 accumulated = rounded_elapsed;
            stage->field_0x18 = accumulated;
            if (stage->field_0x1c <= accumulated) {
                *packet->score = packet->time_reward_score;
                NextStatusStage(packet);
            } else if (0.5f > previous && 0.5f <= accumulated) {
                if (packet->new_best_time == 0.0f)
                    GameAudio_PlaySfx(0x32, NULL, 0, 0);
                else
                    PlaySfx("StatusAward", NULL);
                NewStatusRumbleBuzz(-1, 0.6f, 0.0f, 0);
            }
            if (packet->new_best_time != 0.0f) {
                if (previous < 4.0f && stage->field_0x18 >= 4.0f) {
                    CoinTotalScale = 1.5f;
                    NewStatusRumbleBuzz(-1, 0.0f, 0.1f, 0);
                    PlaySfx("Shop_BuyCheat", NULL);
                }
                if (stage->field_0x18 >= 4.5f)
                    *packet->score = packet->time_reward_score;
                else if (stage->field_0x18 >= 0.5f && stage->field_0x18 < 4.0f) {
                    u32 original = packet->original_score;
                    volatile f32 progress = ((f64)stage->field_0x18 - 0.5) / 3.5;
                    i32 angle = (i32)((f64)progress * 16384.0);
                    u32 difference = packet->time_reward_score - original;
                    i32 amount = (i32)((f64)difference * (f64)NuTrigTable[(angle >> 1) & 0x7fff]);
                    *packet->score = original;
                    IncreaseScore(packet->score, (u64)(i64)amount, 0);
                    {
                        i32 current_bucket = (i32)((f64)stage->field_0x18 / (f64)0.2f);
                        i32 previous_bucket = (i32)((f64)previous / (f64)0.2f);
                        if (current_bucket != previous_bucket)
                            PlaySfx("PickupCoin", NULL);
                    }
                }
            }
        }
    }
}

void __cdecl SuperStoryTime_LSW_Draw(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, i32 current) {
    char text[256], previous[256];
    volatile f32 alpha, coin_progress;
    volatile i32 opacity;
    char *title;
    i32 red, green;
    if (current == 0 || stage->field_0x14 <= 0)
        return;
    if (stage->field_0x18 < 0.5f) {
        alpha = (f64)stage->field_0x18 + (f64)stage->field_0x18;
        coin_progress = alpha;
    } else {
        coin_progress = 1.0f;
        if (stage->field_0x18 < 4.5f)
            alpha = 1.0f;
        else
            alpha = 1.0 - (((f64)stage->field_0x18 - 4.5) + ((f64)stage->field_0x18 - 4.5));
    }
    opacity = (i32)((f64)alpha * 128.0);
    {
        volatile f32 coin_scale = CoinTotalScale;
        volatile i32 angle = (i32)((f64)coin_progress * 16384.0);
        volatile f32 coin_y =
            ((f64)STATSPOSY - (f64)STATSPOS2YPc) * (f64)NuTrigTable[(angle >> 1) & 0x7fff] + (f64)STATSPOS2YPc;
        CoinTotal_Draw((i32)*packet->score, coin_y, coin_scale, 1, 1.0f, 255, 191, 0);
    }
    if (packet->new_best_time != 0.0f) {
        title = TTab[TextIdPc88071c];
        red = 0;
        green = 255;
    } else {
        title = TTab[TextIdPc880734];
        red = 255;
        green = 0;
    }
    SmartTextEx(title, 0.0f, 0.2f, 1.0f, 0.7f, 0.7f, 0.7f, 0, (u8)red, (u8)green, 0, 1.7f, 1, NULL, 0, (u32)opacity);
    {
        volatile f32 displayed_time;
        if (packet->new_best_time != 0.0f) {
            volatile f32 blend;
            if (stage->field_0x18 < 0.5f)
                blend = 0.0f;
            else if (stage->field_0x18 < 4.0f)
                blend = ((f64)stage->field_0x18 - 0.5) / 3.5;
            else
                blend = 1.0f;
            displayed_time = ((f64)packet->new_best_time - (f64)packet->previous_best_time) * (f64)blend +
                             (f64)packet->previous_best_time;
        } else
            displayed_time = packet->superstory_time;
        Text_MakeTime(displayed_time, 1, 1, 1, text);
    }
    Text3DEx(text, 0.0f, 0.0f, 1.0f, 0.7f, 0.7f, 0.7f, 0, 255, 255, 255, opacity);
    Text_MakeTime(packet->previous_best_time, 1, 1, 1, text);
    NuStrCpy(previous, "(");
    NuStrCat(previous, text);
    NuStrCat(previous, ")");
    Text3DEx(previous, 0.0f, -0.2f, 1.0f, 0.7f, 0.7f, 0.7f, 0, 255, 255, 255, opacity / 2);
}

void __cdecl SuperStoryScore_LSW_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
    (void)stage;
    *packet->score = packet->final_reward_score;
    NextStatusStage(packet);
}

void __cdecl SuperStoryScore_LSW_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
    switch (stage->field_0x14) {
        case 0: {
            stage->field_0x18 = 0.0f;
            stage->field_0x14 = 1;
            stage->field_0x1c = 5.0f;
            packet->original_score = *packet->score;
            break;
        }
        case 1: {
            volatile f32 previous = stage->field_0x18;
            volatile f32 rounded_elapsed = stage->field_0x18 + elapsed;
            f32 accumulated = rounded_elapsed;
            stage->field_0x18 = accumulated;
            if (stage->field_0x1c <= accumulated) {
                *packet->score = packet->final_reward_score;
                NextStatusStage(packet);
            } else if (0.5f > previous && 0.5f <= accumulated) {
                if (packet->new_best_score != 0)
                    PlaySfx("StatusAward", NULL);
                else
                    GameAudio_PlaySfx(0x32, NULL, 0, 0);
                NewStatusRumbleBuzz(-1, 0.6f, 0.0f, 0);
            }
            if (packet->final_reward_score != packet->original_score) {
                if (previous < 4.0f && stage->field_0x18 >= 4.0f) {
                    CoinTotalScale = 1.5f;
                    NewStatusRumbleBuzz(-1, 0.0f, 0.1f, 0);
                    PlaySfx("Shop_BuyCheat", NULL);
                }
                if (stage->field_0x18 >= 4.0f)
                    *packet->score = packet->final_reward_score;
                else if (stage->field_0x18 >= 0.5f && stage->field_0x18 < 4.0f) {
                    u32 original = packet->original_score;
                    volatile f32 progress = ((f64)stage->field_0x18 - 0.5) / 3.5;
                    i32 angle = (i32)((f64)progress * 16384.0 + 16384.0);
                    u32 difference = packet->final_reward_score - original;
                    volatile f32 blend = 1.0 - (f64)NuTrigTable[(angle >> 1) & 0x7fff];
                    i32 amount = (i32)((f64)difference * (f64)blend);
                    *packet->score = original;
                    IncreaseScore(packet->score, (u64)(i64)amount, 0);
                    {
                        i32 current_bucket = (i32)((f64)stage->field_0x18 / (f64)0.2f);
                        i32 previous_bucket = (i32)((f64)previous / (f64)0.2f);
                        if (current_bucket != previous_bucket)
                            PlaySfx("PickupCoin", NULL);
                    }
                }
            }
        }
    }
}

void __cdecl SuperStoryScore_LSW_Draw(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, i32 current) {
    char text[256], previous[256];
    volatile f32 coin_progress;
    if (current == 0)
        return;
    if (stage->field_0x14 > 0) {
        volatile f32 alpha, blend;
        volatile i32 opacity;
        char *title;
        i32 red, green;
        if (stage->field_0x18 < 0.5f) {
            alpha = (f64)stage->field_0x18 + (f64)stage->field_0x18;
            coin_progress = 1.0f;
        } else if (stage->field_0x18 < 4.5f) {
            alpha = 1.0f;
            coin_progress = 1.0f;
        } else {
            coin_progress = 1.0 - (((f64)stage->field_0x18 - 4.5) + ((f64)stage->field_0x18 - 4.5));
            alpha = coin_progress;
        }
        opacity = (i32)((f64)alpha * 128.0);
        if (packet->new_best_score != 0) {
            title = TTab[TextIdPc880720];
            red = 0;
            green = 255;
        } else {
            title = TTab[TextIdPc880738];
            red = 255;
            green = 0;
        }
        SmartTextEx(title, 0.0f, 0.2f, 1.0f, 0.7f, 0.7f, 0.7f, 0, (u8)red, (u8)green, 0, 1.7f, 1, NULL, 0,
                    (u32)opacity);
        if (stage->field_0x18 < 0.5f)
            blend = 0.0f;
        else if (stage->field_0x18 < 4.0f)
            blend = ((f64)stage->field_0x18 - 0.5) / 3.5;
        else
            blend = 1.0f;
        Text_MakeScore((u32)((f64)packet->superstory_score * (f64)blend), text);
        Text3DEx(text, 0.0f, 0.0f, 1.0f, 0.7f, 0.7f, 0.7f, 0, 255, 255, 255, opacity);
        Text_MakeScore(packet->previous_best_score, text);
        NuStrCpy(previous, "(");
        NuStrCat(previous, text);
        NuStrCat(previous, ")");
        Text3DEx(previous, 0.0f, -0.2f, 1.0f, 0.7f, 0.7f, 0.7f, 0, 255, 255, 255, opacity / 2);
    } else
        coin_progress = 1.0f;
    {
        volatile f32 coin_scale = CoinTotalScale;
        volatile i32 angle = (i32)((f64)coin_progress * 16384.0);
        volatile f32 coin_y =
            ((f64)STATSPOSY - (f64)STATSPOS2YPc) * (f64)NuTrigTable[(angle >> 1) & 0x7fff] + (f64)STATSPOS2YPc;
        CoinTotal_Draw((i32)*packet->score, coin_y, coin_scale, 1, 1.0f, 255, 191, 0);
    }
}

void __cdecl BonusWin_LSW_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
    (void)stage;
    (void)packet;
}

void __cdecl BonusWin_LSW_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
    switch (stage->field_0x14) {
        case 0: {
            stage->field_0x14 = 1;
            stage->field_0x18 = 0.0f;
            stage->field_0x1c = 4.0f;
            StatusDrawPlayerIconsPc = 1;
            break;
        }
        case 1: {
            volatile f32 previous = stage->field_0x18;
            volatile f32 rounded_elapsed = stage->field_0x18 + elapsed;
            f32 accumulated = rounded_elapsed;
            stage->field_0x18 = accumulated;
            if (stage->field_0x1c <= accumulated) {
                NextStatusStage(packet);
            } else if (previous < 2.0f && accumulated >= 2.0f) {
                ADD_GAME_MESSAGE_PC message = AddGameMsg_Default;
                char text[256];
                NUVEC position;
                i32 player;
                PlaySfx("TrueJedi_100pc", NULL);
                NewStatusRumbleBuzz(-1, 0.6f, 0.0f, 0);
                player = (packet->flags_word >> 5) & 1;
                BonusScore[player] = (i32)((u32)BonusScore[player] + 1u);
                sprintf(text, "%i", BonusScore[(packet->flags_word >> 5) & 1]);
                if ((packet->field_0xb0 & 0x20) == 0) {
                    position.x = -0.015f;
                    message.unknown_4f = 8;
                } else {
                    position.x = 0.015f;
                    message.unknown_4f = 2;
                }
                position.y = STATSPOSY;
                position.z = 1.0f;
                message.text = text;
                message.position = &position;
                message.target_position = &position;
                message.scale = 0.7f;
                message.target_scale = 2.1f;
                message.flags = 0x4028;
                message.duration = 1.0f;
                AddGameMsg(&message);
            } else if ((f64)NuFmod(previous, 0.2f) < (f64)0.1f && (f64)NuFmod(stage->field_0x18, 0.2f) >= (f64)0.1f) {
                PlaySfx((char *)GameAudioName00758838Pc, NULL);
            }
        }
    }
}

void __cdecl BonusWin_LSW_Draw(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, i32 current) {
    if (current != 0) {
        if (stage->field_0x14 == 0) {
            iconalphaoverride = 0.0f;
            return;
        }
        if (stage->field_0x18 >= 0.5f)
            iconalphaoverride = 1.0f;
        else
            iconalphaoverride = (f64)stage->field_0x18 + (f64)stage->field_0x18;
        if (stage->field_0x18 < 2.0f && (f64)NuFmod(stage->field_0x18, 0.2f) >= (f64)0.1f) {
            i32 second = (packet->flags_word >> 5) & 1;
            volatile f32 winner_x = second != 0 ? 0.675f : -0.675f;
            volatile i32 opacity = (i32)((f64)iconalphaoverride * 128.0);
            SmartTextEx(TTab[TextIdPc88075c], winner_x, STATSPOSY, 1.0f, 0.7f, 0.7f, 0.7f, second != 0 ? 8 : 2, 0, 255,
                        0, 0.35f, 1, NULL, 0, (u32)opacity);
        }
        DrawBonusScore(STATSPOSY, StatusPacket.player0_active, StatusPacket.player1_active, iconalphaoverride,
                       BonusScore);
    } else if (stage->field_0x12 != 0 && packet->stage->type != 25) {
        volatile f32 alpha = getFinishedStatusAlpha(packet);
        DrawBonusScore(STATSPOSY, StatusPacket.player0_active, StatusPacket.player1_active, alpha, BonusScore);
    }
}

/* Original source name unknown. The native entry is exactly RET; ReleaseTakeOver passes it the rider and
 * vehicle character names. */
void __cdecl TakeOverReleaseDiagnosticPc(char *rider_name, char *object_name) {
    (void)rider_name;
    (void)object_name;
}
