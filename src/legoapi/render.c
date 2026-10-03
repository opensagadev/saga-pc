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
