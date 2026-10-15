#include <stdio.h>
#include "saga.h"
#undef DrawStatusText

extern f32 DrawStatusScaleXPc, DrawStatusScaleYPc;
void __cdecl NuQFntSetMtx(VUFNT *, NUMTX *);
static NUMTX RenderStatusMatrixPrivate;
void __cdecl DrawStatusText(char *text, u16 angle, f32 x, f32 y, f32 scale, u32 colour, i32 alignment) {
    NUVEC origin = {0.0f, 0.0f, 800.0f};
    NUVEC position = {x * DrawStatusScaleXPc, y * DrawStatusScaleYPc, 0.0f};
    u16 encoded[512];
    f32 height, width;
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
    if (alignment == 2) {
        height = NuQFntHeight(QFont3DZ);
        NuQFntMove(QFont3DZ, 0.0f, (f32)(-height * 0.5), 0.0f);
    } else if (alignment == 8) {
        height = NuQFntHeight(QFont3DZ);
        width = NuQFntPrintLenW(QFont3DZ, encoded);
        NuQFntMove(QFont3DZ, -width, (f32)(-height * 0.5), 0.0f);
    } else {
        height = NuQFntHeight(QFont3DZ);
        width = NuQFntPrintLenW(QFont3DZ, encoded);
        NuQFntMove(QFont3DZ, (f32)(-width * 0.5), (f32)(-height * 0.5), 0.0f);
    }
    NuQFntPrintW(QFont3DZ, encoded);
    NuQFntPopPrintMode();
    NuQFntSetCoordinateSystem(3);
}

STATUS_STAGE_s *FindStatusStage(i32 type) {
    struct STATUS_STAGE_s *stage;
    for (stage = StatusStages; stage->type != -1; ++stage) {
        if (stage->type == type) {
            return stage;
        }
    }
    return NULL;
}

void NewStatusRumbleBuzz(i32 player, float amount, float buzz, i32 priority) {
    if (!(amount > 0.0f)) {
        if (StatusPacket.player0_active != 0 && (player == 0 || player == -1)) {
            goto player0_buzz;
        }
        if (StatusPacket.player1_active != 0 && (player == 1 || player == -1)) {
            goto player1_buzz;
        }
        return;
    }

    if (StatusPacket.player0_active != 0 && (player == 0 || player == -1)) {
        if (StatusPacket.player0_rumble_time <= 0.0f || amount > StatusPacket.player0_rumble_time /
                                                                     StatusPacket.player0_rumble_duration *
                                                                     StatusPacket.player0_rumble_amount) {
            StatusPacket.player0_rumble_amount = amount;
            StatusPacket.player0_rumble_duration = amount;
            StatusPacket.player0_rumble_time = amount;
        }
    player0_buzz:
        if (buzz > StatusPacket.player0_buzz_amount) {
            StatusPacket.player0_buzz_amount = buzz;
        }
        if (priority > 0) {
            const i32 old_priority = StatusPacket.player0_rumble_priority;
            ++priority;
            if (priority > old_priority) {
                StatusPacket.player0_rumble_priority = (u8)(priority);
            }
        }
    }

    if (StatusPacket.player1_active != 0 && (player == 1 || player == -1)) {
        if (StatusPacket.player1_rumble_time <= 0.0f || amount > StatusPacket.player1_rumble_time /
                                                                     StatusPacket.player1_rumble_duration *
                                                                     StatusPacket.player1_rumble_amount) {
            StatusPacket.player1_rumble_amount = amount;
            StatusPacket.player1_rumble_duration = amount;
            StatusPacket.player1_rumble_time = amount;
        }
    player1_buzz:
        if (buzz > StatusPacket.player1_buzz_amount) {
            StatusPacket.player1_buzz_amount = buzz;
        }
        if (priority > 0) {
            const i32 old_priority = StatusPacket.player1_rumble_priority;
            ++priority;
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

void AddStatusStage(STATUSPACKET_s *packet, i32 type, i32 gold_brick_enabled) {
    const u8 index = packet->stage_count;
    packet->stage_types[index] = (i8)(type);
    packet->gold_brick_enabled[index] = (u8)(gold_brick_enabled);
    packet->stage_count = index + 1;
}

i32 __cdecl NuSpecialDrawAtAlpha(NUSPECIALHANDLE *, NUMTX *, f32);
i32 __cdecl DrawPanel3DObject(f32 x, f32 y, f32 z, f32 scale_x, f32 scale_y, f32 scale_z, u16 rotate_x, u16 rotate_y,
                              u16 rotate_z, NUSPECIALHANDLE *special, i32 rotate_order, f32 alpha) {
    if (alpha > 0.0f && special != NULL && NuSpecialExistsFn(special) != 0 &&
        (scale_y != 0.0f || scale_x != 0.0f || scale_z != 0.0f)) {
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
    LEVEL_OBJECT_RUNTIME_PC *object;
    if (object_id == -1)
        return 1;
    if (world == NULL) {
        world = WorldInfo_CurrentlyActive();
        if (world == NULL)
            return 1;
    }
    object = &world->level_objects[object_id];
    if (object->active != 0)
        return NuSpecialDrawAt(&object->special, matrix);
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
#define ALPHA_GRID_VERTEX(vx, vy)                                                                                      \
    do {                                                                                                               \
        i32 colour;                                                                                                    \
        direction.x = (vx);                                                                                            \
        direction.y = -(vy) * camera.aspect;                                                                           \
        direction.z = adjacent;                                                                                        \
        NuVecNorm(&direction, &direction);                                                                             \
        NuVecMtxTransform(&direction, &direction, &camera.mtx);                                                        \
        NuVecNorm(&direction, &direction);                                                                             \
        brightness = direction.y * 0.5f + 0.5f;                                                                        \
        if (brightness <= near_angle)                                                                                  \
            brightness = near_scale * parameters->intensity;                                                           \
        else if (brightness >= far_angle)                                                                              \
            brightness = far_scale * parameters->intensity;                                                            \
        else                                                                                                           \
            brightness = ((brightness - near_angle) * scale_delta / angle_delta + near_scale) * parameters->intensity; \
        if (parameters->directional) {                                                                                 \
            f32 angle =                                                                                                \
                (i16)(0x4000 - RenderPostASinPc(NuVecDot(&parameters->direction, &direction))) * 0.0054931640625f;     \
            if (angle < parameters->direction_near_angle) {                                                            \
                brightness += 128.0f * parameters->direction_far_scale;                                                \
            } else if (!(angle > parameters->direction_far_angle)) {                                                   \
                f32 blend = RenderPostPowPc((angle - parameters->direction_near_angle) /                               \
                                                (parameters->direction_far_angle - parameters->direction_near_angle),  \
                                            parameters->direction_bias);                                               \
                brightness +=                                                                                          \
                    128.0f * ((1.0f - blend) * (parameters->direction_far_scale - parameters->direction_near_scale) +  \
                              parameters->direction_near_scale);                                                       \
            }                                                                                                          \
        }                                                                                                              \
        colour = RGBA_TO_NUCOLOUR32((MIN(255.0f, (MAX(0.0f, brightness)))), (MIN(255.0f, (MAX(0.0f, brightness)))),    \
                                    (MIN(255.0f, (MAX(0.0f, brightness)))), 128);                                      \
        if (!NuPrimNeedsOverbrightening)                                                                               \
            ((RENDER_POST_PRIM_VERTEX_PC *)NuPrimStreamBufferPointer->void_ptr)->colour =                              \
                ((colour >> 1) & 0x7f7f7f) | (colour & 0xff000000);                                                    \
        else                                                                                                           \
            ((RENDER_POST_PRIM_VERTEX_PC *)NuPrimStreamBufferPointer->void_ptr)->colour = colour;                      \
        NuPrim2DAddXYZ((vx), (vy), 0.0f);                                                                              \
    } while (0)

void DrawAlphaGrid(i32 rows, i32 cols, RENDER_POST_BLOOM_READER_PC *parameters) {
    f32 inv_row = 1.0f / (rows - 1);
    f32 inv_col = 1.0f / (cols - 1);
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
    NUVEC direction;
    f32 scale_delta = far_scale - near_scale;
    NUCAMERA camera;
    f32 brightness;
    static i32 first = 1;
    static f32 saved_fov;
    static f32 adjacent;
    static i32 row, col;
    NuCameraGet(&camera);
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
            ALPHA_GRID_VERTEX(x0, y0);
            ALPHA_GRID_VERTEX(x1, y0);
            if ((cols - 1) != col) {
                ALPHA_GRID_VERTEX(x0, y0);
                ALPHA_GRID_VERTEX(x0, y1);
                ALPHA_GRID_VERTEX(x1, y0);
                ALPHA_GRID_VERTEX(x1, y1);
                ALPHA_GRID_VERTEX(x1, y0);
                ALPHA_GRID_VERTEX(x0, y1);
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
        if (!(0.0f >= RenderBackdropWaitPc))
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

/* Add to render.c. Native PC 552e00/78; cdecl (special,matrix,alpha). */
i32 __cdecl DrawPanel3DObjectMtx(NUSPECIALHANDLE *special, NUMTX *matrix, f32 alpha) {
    if (!(alpha <= 0.0f) && special != NULL && NuSpecialExistsFn(special) != 0) {
        NuMtxMulVU0(matrix, matrix, NuCameraGetMtx());
        NuSpecialDrawAtAlpha(special, matrix, alpha);
    }
    return 0;
}
