#include "saga.h"

#include <string.h>

NURNDRLIBSCENE currentScene;
NUVPCURRENT vpCurrent;
i32 sceneParametersCount;
NUDISPLAYLIST PCDisplayList2dPc;
i32 nused_sort_pris;
NUSORTPRI *sort_list;
i32 render_scene_index;
NURNDRLIBSCENE sceneParameters[32];
NUDISPLAYLISTRENDERSCENE *render_scene_ptrs[24];
i32 nudlist_alloc_count;
u32 nudlist_alloc_base;
u32 nudlist_alloc_end;
u32 nudlist_item_base;
i32 nudlist_frame_active;
u32 nudlist_field_e80;
u32 nudlist_field_fc8;
i32 nudlist_field_fc4;
u32 nudlist_field_9d0;
u32 nudlist_field_9d8;
u32 nudlist_field_fe4;
NUDISPLAYLISTITEM nudlist_2d_static;
VARIPTR nudlist_stream_cursor;

i32 __cdecl NuRndrBeginScene(i32 begin_flags) {
    currentScene.clear_alpha_4c = 0.0f;
    currentScene.unknown_24 = 0;
    currentScene.unknown_28 = 0;
    currentScene.unknown_58 = 0;
    currentScene.dyn_light_count = 0;
    currentScene.dyn_light_ptr = NULL;
    currentScene.unknown_48 = 0;
    currentScene.unknown_ac = 0;
    currentScene.unknown_c4 = 0;
    currentScene.unknown_e4 = 0;
    currentScene.unknown_e0 = 0;
    currentScene.unknown_178 = 0;
    currentScene.unknown_174 = 0;
    currentScene.unknown_188 = 0;
    currentScene.unknown_214 = 0;
    currentScene.scene_id = -1;
    currentScene.unknown_40 = -1;
    return 1;
}

void __cdecl NuRndrClear(i32 clear_flags, i32 bg_colour, f32 alpha) {
    currentScene.clear_alpha = alpha;
    currentScene.clear_flags_08 |= clear_flags;
    currentScene.bg_colour = bg_colour;
    NuVpGetPosition2(&currentScene.unknown_14, &currentScene.unknown_18);
    currentScene.calc_1c = (f32)(0.0625 * vpCurrent.width / ((f64)PS2_REZ_W / PS2_VREZ_W));
    currentScene.calc_20 = (f32)(0.0625 * vpCurrent.height / ((f64)PS2_REZ_H / PS2_VREZ_H));
}

void NuRndrAxisArrows(NUVEC *position, void *unused1, f32 length, NUMTL *material) {
    NUMTX matrix;
    NuMtxSetIdentity(&matrix);
    NuMtxTranslate(&matrix, position);
    NuRndrAxisArrowsMtx(&matrix, length, material);
}

void __cdecl NuRndrGradClear(i32 clear_flags, i32 top_colour, i32 bottom_colour, f32 alpha) {
    NuRndrClear(clear_flags, bottom_colour, alpha);
}

static __forceinline f32 NuFdiv(f32 a, f32 b) {
    if (a == 0.0f || b == 0.0f) {
        return 0.0f;
    }
    return a / b;
}

void __cdecl NuVpGetPosition2(f32 *x, f32 *y) {
    *x = NuFdiv((f32)vpCurrent.x * 0.0625f - ((f32)PS2_VCNTR_X - (f32)(PS2_REZ_W >> 1)),
                NuFdiv((f32)PS2_REZ_W, (f32)PS2_VREZ_W));
    *y = NuFdiv((f32)vpCurrent.y * 0.0625f - ((f32)PS2_VCNTR_Y - (f32)(PS2_REZ_H >> 1)),
                NuFdiv((f32)PS2_REZ_H, (f32)PS2_VREZ_H));
}

void __cdecl NuDisplayListResetStateCache(NURNDRLIBSTATE *state) {
    state->mtl = NULL;
    state->tex_id = -1;
    state->global_id = -1;
    state->lights_id = -1;
    state->camera_id = -1;
    state->fog_id = -1;
    state->konst_id = -1;
}

i32 __cdecl NuDisplayListAddRenderScene(void) {
    NUSORTPRI **pris;
    NUSORTPRI *node;
    NUSORTPRI *rebuilt;
    NUDISPLAYSCENE *sub;
    NUDISPLAYLIST *dlist;
    NUDISPLAYLISTITEM *item;
    NUDISPLAYLISTITEM *src;
    NURNDRLIBSTATE *state;
    u32 items;
    u32 offset;
    i32 count;
    i32 m;
    i32 result;

    NuDisplayListStreamCursor->addr = (NuDisplayListStreamCursor->addr + 0xfU) & ~0xfU;
    pris = (NUSORTPRI **)NuDisplayListStreamCursor->addr;
    NuDisplayListStreamCursor->addr += (nused_sort_pris + 1) * sizeof(NUSORTPRI *);
    count = 0;

    for (node = sort_list; node != NULL; node = node->sys_next) {
        sub = node->legacy_display_scene;
        if (sub != NULL && (sub->flags & 0x10) != 0) {
            if ((sub->flags & 0x2) != 0) {
                continue;
            }
            rebuilt = (NUSORTPRI *)((NuDisplayListStreamCursor->addr + 3U) & ~3U);
            NuDisplayListStreamCursor->addr = (u32)rebuilt + sizeof(NUSORTPRI);
            rebuilt->sort_pri = node->sort_pri;
            rebuilt->items = (NUDISPLAYLISTITEM *)((u8 *)rebuilt + sizeof(NUSORTPRI));
            rebuilt->display_scene = node->display_scene;
            rebuilt->nmtls = node->nmtls;
            rebuilt->mtl_first = node->mtl_first;
            items = (u32)rebuilt->items;
            offset = 0;
            if (rebuilt->nmtls != 0) {
                for (m = 0; m < rebuilt->nmtls; m++) {
                    dlist = sub->dlist_mtls[(u16)(rebuilt->mtl_first + m)];
                    if (dlist->mtl_last == dlist->first) {
                        continue;
                    }
                    item = dlist->mtl_item;
                    src = (item[1].type == 0x80) ? &item[1] : &item[3];
                    *(NUDISPLAYLISTITEM *)(items + offset) = *src;
                    offset += 0x10;
                    *(NUDISPLAYLISTITEM *)(items + offset) = *dlist->first;
                    offset += 0x10;
                    dlist->mtl_last->next = (void *)(items + offset);
                    dlist->mtl_last = dlist->first;
                    NuDisplayListResetStateCache(dlist->state);
                }
            }
            *(u8 *)(items + offset) = 0x84;
            *((u8 *)items + offset + 1) = 4;
            *(u32 *)(items + offset + 4) = 0;
            offset += 0x10;
            NuDisplayListStreamCursor->addr = items + offset;
            pris[count] = rebuilt;
            count++;
            rebuilt->flags |= 0x2;
        } else {
            if ((node->flags & 0x2) != 0) {
                continue;
            }
            if (sub != NULL && (sub->flags & 0x6) == 0) {
                continue;
            }
            pris[count] = node;
            count++;
            node->flags |= 0x2;
        }
    }

    if (count == 0 && PCDisplayList2dPc.mtl_last == PCDisplayList2dPc.first) {
        return -1;
    }

    result = (NuDisplayListStreamCursor->addr + 3U) & ~3U;
    render_scene_ptrs[render_scene_index] = (NUDISPLAYLISTRENDERSCENE *)result;
    NuDisplayListStreamCursor->addr = result + sizeof(NUDISPLAYLISTRENDERSCENE);
    render_scene_ptrs[render_scene_index]->nsort_pris = count;
    render_scene_ptrs[render_scene_index]->sort_pris = pris;

    if (PCDisplayList2dPc.mtl_last != PCDisplayList2dPc.first) {
        PCDisplayList2dPc.mtl_last->type = 0x84;
        PCDisplayList2dPc.mtl_last->next = NULL;
        PCDisplayList2dPc.mtl_last->id = 4;
        item = PCDisplayList2dPc.first;
        *(NUDISPLAYLISTITEM *)((u8 *)render_scene_ptrs[render_scene_index] + 8) = *item;
        NuDisplayListResetStateCache(PCDisplayList2dPc.state);
        PCDisplayList2dPc.mtl_last = PCDisplayList2dPc.first;
    } else {
        item = (NUDISPLAYLISTITEM *)((u8 *)render_scene_ptrs[render_scene_index] + 8);
        *(u8 *)item = 0x84;
        *(u32 *)((u8 *)item + 4) = 0;
        *((u8 *)item + 1) = 4;
    }

    result = render_scene_index;
    render_scene_index += 1;
    return result;
}

void __cdecl NuRndrEndScene(void) {
    i32 scene_id = NuDisplayListAddRenderScene();

    currentScene.scene_id = scene_id;
    if (currentScene.dyn_light_count != 0 && scene_id != -1) {
        NURNDRLIGHTRECORD *records = (NURNDRLIGHTRECORD *)currentScene.dyn_light_ptr;
        NURNDRLIGHTRECORD *record;

        if (records[2].enabled != 0) {
            record = &records[currentScene.unknown_40];
            record->pending[record->count] = scene_id;
            record->count += 1;
        } else {
            currentScene.dyn_light_count = 0;
        }
        currentScene.scene_id = -1;
    }
    sceneParameters[sceneParametersCount] = currentScene;
    sceneParametersCount += 1;
}

void __cdecl NuDisplayListStreamAlloc(VARIPTR *cursor, i32 arg2, i32 count) {
    u32 aligned;
    i32 index;
    u32 offset;

    nudlist_alloc_count = count;
    nudlist_alloc_base = cursor->addr;
    cursor->addr += count;
    aligned = (cursor->addr + 3U) & ~3U;
    nudlist_alloc_end = aligned;
    cursor->addr = aligned + count * 36;
    nudlist_item_base = (cursor->addr + 0xfU) & ~0xfU;
    cursor->addr = nudlist_item_base;
    cursor->addr += count * 32;
    for (index = 0; index < count; index++) {
        *(u8 *)(nudlist_alloc_base + index) = 0;
        offset = index * 0x20;
        *(u8 *)(nudlist_item_base + offset + 0x10) = 0x84;
        *(u32 *)(nudlist_item_base + offset + 0x14) = 0;
        *((u8 *)nudlist_item_base + offset + 0x11) = 4;
    }
}

void __cdecl NuRndrStateInit(void);

void __cdecl NuDisplayListInit(VARIPTR *cursor, i32 arg2) {
    NuRndrStateInit();
    nudlist_field_9d8 = 0;
    nudlist_field_e80 = 0;
    sort_list = NULL;
    nudlist_field_fc8 = 0;
    PCDisplayList2dPc.first = &nudlist_2d_static;
    {
        VARIPTR end;
        end.addr = (u32)arg2;
        NuDisplayListCreateDynMtlList_candidate(cursor, end);
    }
    nudlist_flags |= 0x10;
    nudlist_field_e58 = 0;
    NuDisplayListStreamAlloc(cursor, arg2, 0x40);
    NuDisplayListStreamCursor = &nudlist_stream_cursor;
    nudlist_field_1598 = nuprim_field_ba8;
    nudlist_field_fc4 = PCThreadCriticalSectionCreate_candidate();
    NuDisplayListSwapBuffersEndFrame();
    NuDisplayListBeginFrame();
}

DWA_WEIGHT_BLOCK_PC *__cdecl NuRndrCreateBlendShapeDeformerWeightsArray(i32 count) {
    i32 clear_bytes = count * 4 + 0x204;
    DWA_WEIGHT_BLOCK_PC *result;
    DwaWeightCursorPc -= clear_bytes;
    if (DwaWeightCursorPc < 0) {
        PCClipScreenUnsupported();
        return NULL;
    }
    result = &DwaWeightPoolPc[DwaWeightCursorPc];
    memset(result, 0, clear_bytes);
    return result;
}
DWA_WEIGHT_BLOCK_PC **__cdecl NuRndrCreateBlendShapeDWAPointers(i32 count) {
    DwaPointerCursorPc -= count;
    if (DwaWeightCursorPc < 0) {
        PCClipScreenUnsupported();
        return NULL;
    }
    return &DwaPointerPoolPc[DwaPointerCursorPc];
}

typedef struct PC_HG_SHADOW_PACKET PC_HG_SHADOW_PACKET;
typedef struct PC_HG_SHADOW_STATE_VIEW PC_HG_SHADOW_STATE_VIEW;

void NuRndrCalcRandEllipsePos(NUVEC4 *position, NUMTX *matrix, NUVEC *axes) {
    f32 x;
    f32 y;
    f32 z;
    f32 length;
    f32 scale;

    x = (f32)((f64)(i32)NuWindRand() - 32768.0);
    y = (f32)((f64)(i32)NuWindRand() - 32768.0);
    z = (f32)((f64)(i32)NuWindRand() - 32768.0);
    length = (f32)((f64)x * x + (f64)y * y + (f64)z * z);
    scale = 1.0f / NuFsqrt(length);
    x *= scale;
    y *= scale;
    z *= scale;
    position->x = axes[1].x * x + axes[2].x * y + axes[3].x * z + axes[0].x;
    position->y = axes[1].y * x + axes[2].y * y + axes[3].y * z + axes[0].y;
    position->z = axes[1].z * x + axes[2].z * y + axes[3].z * z + axes[0].z;
    position->w = 1.0f;
    NuVec4MtxTransformVU0(position, position, matrix);
}

void NuRndrCalcRandCylinderPos(NUVEC4 *position, NUMTX *matrix, NUVEC *axes) {
    f32 x;
    f32 y;
    f32 z;
    f32 length;
    f32 scale;

    x = (f32)((f64)((i32)NuWindRand() - 32768) * (1.0 / 32768.0));
    y = (f32)((f64)(i32)NuWindRand() - 32768.0);
    z = (f32)((f64)(i32)NuWindRand() - 32768.0);
    length = (f32)((f64)y * y + (f64)z * z);
    scale = 1.0f / NuFsqrt(length);
    y *= scale;
    z *= scale;
    position->x = axes[1].x * x + axes[2].x * y + axes[3].x * z + axes[0].x;
    position->y = axes[1].y * x + axes[2].y * y + axes[3].y * z + axes[0].y;
    position->z = axes[1].z * x + axes[2].z * y + axes[3].z * z + axes[0].z;
    position->w = 1.0f;
    NuVec4MtxTransformVU0(position, position, matrix);
}

void NuRndrStartReflectionRender(i32 clip) {
    PCSpecialReflectionPass = 1;
    RndrStateSetReflection(1);
    (void)clip;
}

void NuRndrEndReflectionRender(void) {
    PCSpecialReflectionPass = 0;
    RndrStateSetReflection(0);
}

void NuRndrGlobalFrameCountPause(i32 paused) {
    RenderFramePausedPc = paused;
}

void __cdecl NuRndrAddShadow(NUVEC *position, f32 radius, i32 opacity, i32 unused_x, i32 unused_y, i32 unused_z) {
    if (NuCameraClipTestPoints(position, 1, NULL) == 0 && NuRndrShadowCnt < 128) {
        NuRndrShadPolDat[NuRndrShadowCnt].position = *position;
        NuRndrShadPolDat[NuRndrShadowCnt].radius = radius;
        NuRndrShadPolDat[NuRndrShadowCnt].opacity = (u16)opacity;
        ++NuRndrShadowCnt;
    }
}

STATIC_ASSERT(offsetof(NURNDRLIBSCENE, unknown_214) == 0x214);
void __cdecl NuBackbufferCopy(i32 texture_id) {
    currentScene.unknown_214 = (u32)texture_id;
}

typedef struct locator_prim_vertex_pc_s {
    f32 x, y, z;
    u32 colour;
    f32 u, v;
} LOCATOR_PRIM_VERTEX_PC;
STATIC_ASSERT(sizeof(LOCATOR_PRIM_VERTEX_PC) == 0x18);
STATIC_ASSERT(offsetof(LOCATOR_PRIM_VERTEX_PC, x) == 0);
STATIC_ASSERT(offsetof(LOCATOR_PRIM_VERTEX_PC, y) == 4);
STATIC_ASSERT(offsetof(LOCATOR_PRIM_VERTEX_PC, z) == 8);
STATIC_ASSERT(offsetof(LOCATOR_PRIM_VERTEX_PC, colour) == 0xc);
static void NuRndrPrimSetColourPrivate(i32 colour) {
    u32 converted;
    if (NuPrimNeedsOverbrightening != 0)
        converted = ((colour & 0xff) << 16) + ((colour >> 16) & 0xff) + (colour & 0xff00) + (colour & 0xff000000);
    else
        converted =
            ((colour & 0xfe) << 15) + ((colour >> 1) & 0x7f00) + ((colour >> 17) & 0x7f) + (colour & 0xff000000);
    ((LOCATOR_PRIM_VERTEX_PC *)NuPrimStreamBufferPointer->void_ptr)->colour = converted;
}
#define LOCATOR_PRIM_POSITION(X, Y, Z)                                                                                 \
    do {                                                                                                               \
        ((LOCATOR_PRIM_VERTEX_PC *)NuPrimStreamBufferPointer->void_ptr)->x = (X);                                      \
        ((LOCATOR_PRIM_VERTEX_PC *)NuPrimStreamBufferPointer->void_ptr)->y = (Y);                                      \
        ((LOCATOR_PRIM_VERTEX_PC *)NuPrimStreamBufferPointer->void_ptr)->z = (Z);                                      \
        NuPrimStreamBufferPointer->void_ptr = (LOCATOR_PRIM_VERTEX_PC *)NuPrimStreamBufferPointer->void_ptr + 1;       \
        ++NuPrimVertexCount;                                                                                           \
    } while (0)
void __cdecl NuRndrAxisArrowsMtx(NUMTX *matrix, f32 length, NUMTL *material) {
    f32 base = length * 0.8f;
    f32 width = length * 0.1f;
    NuPrim3DBegin(14, 5, material, matrix);
    NuRndrPrimSetColourPrivate((i32)0xff0000ff);
    LOCATOR_PRIM_POSITION(0, 0, 0);
    NuRndrPrimSetColourPrivate((i32)0xff0000ff);
    LOCATOR_PRIM_POSITION(length, 0, 0);
    NuRndrPrimSetColourPrivate((i32)0xff00ff00);
    LOCATOR_PRIM_POSITION(0, 0, 0);
    NuRndrPrimSetColourPrivate((i32)0xff00ff00);
    LOCATOR_PRIM_POSITION(0, length, 0);
    NuRndrPrimSetColourPrivate((i32)0xffff0000);
    LOCATOR_PRIM_POSITION(0, 0, 0);
    NuRndrPrimSetColourPrivate((i32)0xffff0000);
    LOCATOR_PRIM_POSITION(0, 0, length);
    NuPrim3DEnd();
    NuPrim3DBegin(15, 5, material, matrix);
    NuRndrPrimSetColourPrivate((i32)0xff0000ff);
    LOCATOR_PRIM_POSITION(base, -width, 0);
    NuRndrPrimSetColourPrivate((i32)0xff0000ff);
    LOCATOR_PRIM_POSITION(length, 0, 0);
    NuRndrPrimSetColourPrivate((i32)0xff0000ff);
    LOCATOR_PRIM_POSITION(base, width, 0);
    NuPrim3DEnd();
    NuPrim3DBegin(15, 5, material, matrix);
    NuRndrPrimSetColourPrivate((i32)0xff00ff00);
    LOCATOR_PRIM_POSITION(0, base, width);
    NuRndrPrimSetColourPrivate((i32)0xff00ff00);
    LOCATOR_PRIM_POSITION(0, length, 0);
    NuRndrPrimSetColourPrivate((i32)0xff00ff00);
    LOCATOR_PRIM_POSITION(0, base, -width);
    NuPrim3DEnd();
    NuPrim3DBegin(15, 5, material, matrix);
    NuRndrPrimSetColourPrivate((i32)0xffff0000);
    LOCATOR_PRIM_POSITION(0, width, base);
    NuRndrPrimSetColourPrivate((i32)0xffff0000);
    LOCATOR_PRIM_POSITION(0, 0, length);
    NuRndrPrimSetColourPrivate((i32)0xffff0000);
    LOCATOR_PRIM_POSITION(0, -width, base);
    NuPrim3DEnd();
}
#undef LOCATOR_PRIM_POSITION

void __cdecl NuRndrInitWorld(void) {
}

STATIC_ASSERT(offsetof(LOCATOR_PRIM_VERTEX_PC, u) == 0x10);
STATIC_ASSERT(offsetof(LOCATOR_PRIM_VERTEX_PC, v) == 0x14);

i32 __cdecl NuRndrLine3d(NURND_VERTEX3D_PC *vertices, NUMTL *material, NUMTX *matrix) {
    i32 i;
    NuPrim3DBegin(14, 7, material, matrix);
    for (i = 0; i < 2; ++i) {
        LOCATOR_PRIM_VERTEX_PC *output;
        f32 u, v, x, y, z;
        NuRndrPrimSetColourPrivate((i32)vertices[i].colour);
        u = vertices[i].u;
        v = vertices[i].v;
        output = (LOCATOR_PRIM_VERTEX_PC *)NuPrimStreamBufferPointer->void_ptr;
        output->u = u;
        output->v = v;
        x = vertices[i].position.x;
        y = vertices[i].position.y;
        z = vertices[i].position.z;
        output->x = x;
        output->y = y;
        ((LOCATOR_PRIM_VERTEX_PC *)NuPrimStreamBufferPointer->void_ptr)->z = z;
        NuPrimStreamBufferPointer->addr += sizeof(LOCATOR_PRIM_VERTEX_PC);
        ++NuPrimVertexCount;
    }
    NuPrim3DEnd();
    return 1;
}

i32 __cdecl NuRndrStrip3d(NURND_VERTEX3D_PC *vertices, NUMTL *material, NUMTX *matrix, i32 count) {
    i32 i;
    if (count == 0)
        return 1;
    NuPrim3DBegin(13, 7, material, matrix);
    for (i = 0; i < count; ++i) {
        LOCATOR_PRIM_VERTEX_PC *output;
        f32 u, v, x, y, z;
        NuRndrPrimSetColourPrivate((i32)vertices[i].colour);
        u = vertices[i].u;
        v = vertices[i].v;
        output = (LOCATOR_PRIM_VERTEX_PC *)NuPrimStreamBufferPointer->void_ptr;
        output->u = u;
        output->v = v;
        x = vertices[i].position.x;
        y = vertices[i].position.y;
        z = vertices[i].position.z;
        output->x = x;
        output->y = y;
        ((LOCATOR_PRIM_VERTEX_PC *)NuPrimStreamBufferPointer->void_ptr)->z = z;
        NuPrimStreamBufferPointer->addr += sizeof(LOCATOR_PRIM_VERTEX_PC);
        ++NuPrimVertexCount;
    }
    NuPrim3DEnd();
    return 1;
}

i32 __cdecl NuRndrTriStrip3dClip(NURND_VERTEX3D_PC *vertices, i32 count, NUMTX *matrix, NUMTL *material) {
    return NuRndrStrip3d(vertices, material, matrix, count);
}

extern i32 NuPrimCoordSystemStack[2];

void __cdecl NuRndrLine3dDbg(f32 x0, f32 y0, f32 z0, f32 x1, f32 y1, f32 z1, i32 colour) {
    LOCATOR_PRIM_VERTEX_PC *output;
    NuPrim3DBegin(14, 5, NULL, &NuMtxSetIdentityTemplate);
    NuRndrPrimSetColourPrivate(colour);
    output = (LOCATOR_PRIM_VERTEX_PC *)NuPrimStreamBufferPointer->void_ptr;
    output->x = x0;
    output->y = y0;
    ((LOCATOR_PRIM_VERTEX_PC *)NuPrimStreamBufferPointer->void_ptr)->z = z0;
    NuPrimStreamBufferPointer->addr += sizeof(LOCATOR_PRIM_VERTEX_PC);
    ++NuPrimVertexCount;
    NuRndrPrimSetColourPrivate(colour);
    output = (LOCATOR_PRIM_VERTEX_PC *)NuPrimStreamBufferPointer->void_ptr;
    output->x = x1;
    output->y = y1;
    ((LOCATOR_PRIM_VERTEX_PC *)NuPrimStreamBufferPointer->void_ptr)->z = z1;
    NuPrimStreamBufferPointer->addr += sizeof(LOCATOR_PRIM_VERTEX_PC);
    ++NuPrimVertexCount;
    NuPrim3DEnd();
}

void __cdecl NuRndrGradRectUV2di(i32 x, i32 y, i32 width, i32 height, f32 u0, f32 v0, f32 u1, f32 v1, u32 *colours,
                                 void *material) {
    f32 sx = (f32)((f64)x * 0.0625);
    f32 sy = (f32)((f64)y * 0.0625);
    f32 sw = (f32)((f64)width * 0.0625);
    f32 sh = (f32)((f64)height * 0.0625);
    LOCATOR_PRIM_VERTEX_PC *vertex;
    ++NuPrimCSPos;
    NuPrimSetCoordinateSystem(0);
    NuPrim2DBegin(13, 7, (NUMTL *)material);
    NuRndrPrimSetColourPrivate((i32)colours[0]);
    vertex = (LOCATOR_PRIM_VERTEX_PC *)NuPrimStreamBufferPointer->void_ptr;
    vertex->u = u0;
    vertex->v = v0;
    NuPrim2DAddXYZ(sx, sy, 0.0f);
    NuRndrPrimSetColourPrivate((i32)colours[1]);
    vertex = (LOCATOR_PRIM_VERTEX_PC *)NuPrimStreamBufferPointer->void_ptr;
    vertex->u = u1;
    vertex->v = v0;
    NuPrim2DAddXYZ(sx + sw, sy, 0.0f);
    NuRndrPrimSetColourPrivate((i32)colours[2]);
    vertex = (LOCATOR_PRIM_VERTEX_PC *)NuPrimStreamBufferPointer->void_ptr;
    vertex->u = u0;
    vertex->v = v1;
    NuPrim2DAddXYZ(sx, sy + sh, 0.0f);
    NuRndrPrimSetColourPrivate((i32)colours[3]);
    vertex = (LOCATOR_PRIM_VERTEX_PC *)NuPrimStreamBufferPointer->void_ptr;
    vertex->u = u1;
    vertex->v = v1;
    NuPrim2DAddXYZ(sx + sw, sy + sh, 0.0f);
    NuPrim2DEnd();
    --NuPrimCSPos;
    NuPrimSetCoordinateSystem(NuPrimCoordSystemStack[NuPrimCSPos]);
}

i32 __cdecl NuRndrLineStrip2d(NURND_VERTEX2D_PC *vertices, NUMTL *material, i32 count) {
    i32 i;
    ++NuPrimCSPos;
    NuPrimSetCoordinateSystem(0);
    NuPrim2DBegin(14, 7, material);
    for (i = 0; i < count; ++i) {
        LOCATOR_PRIM_VERTEX_PC *output;
        f32 x, y;
        NuRndrPrimSetColourPrivate(-1);
        output = (LOCATOR_PRIM_VERTEX_PC *)NuPrimStreamBufferPointer->void_ptr;
        output->u = 0.0f;
        output->v = 0.0f;
        x = vertices[i].x * 0.0625;
        y = vertices[i].y * 0.0625;
        NuPrim2DAddXYZ(x, y, 0.0f);
    }
    NuPrim2DEnd();
    --NuPrimCSPos;
    NuPrimSetCoordinateSystem(NuPrimCoordSystemStack[NuPrimCSPos]);
    return 1;
}

#define PORTAL_DEBUG_SIN(ANGLE) NuTrigTable[((i32)(ANGLE) >> 1) & 0x7fff]
#define PORTAL_DEBUG_COS(ANGLE) NuTrigTable[(((i32)(ANGLE) + 16384) >> 1) & 0x7fff]
void __cdecl NuRndrSphere(NUVEC *centre, f32 radius, i32 colour, f32 vertical_scale) {
    f64 radius_squared = (f64)radius * radius;
    i32 band, angle;
    for (band = 0; band < 8; ++band) {
        f32 lower_radius = radius * PORTAL_DEBUG_SIN((band < 4 ? band : 8 - band) * 4096);
        f32 upper_radius = radius * PORTAL_DEBUG_SIN((band < 3 ? band + 1 : 7 - band) * 4096);
        f32 lower_y = NuFsqrt((f32)(radius_squared - (f64)lower_radius * lower_radius)) * vertical_scale;
        f32 upper_y = NuFsqrt((f32)(radius_squared - (f64)upper_radius * upper_radius)) * vertical_scale;
        if (band > 4)
            lower_y = -lower_y;
        if (band > 3)
            upper_y = -upper_y;
        lower_y += centre->y;
        upper_y += centre->y;
        for (angle = 0; angle < 65536; angle += 2048) {
            NuRndrLine3dDbg(centre->x + lower_radius * PORTAL_DEBUG_COS(angle), lower_y,
                            centre->z + lower_radius * PORTAL_DEBUG_SIN(angle),
                            centre->x + upper_radius * PORTAL_DEBUG_COS(angle), upper_y,
                            centre->z + upper_radius * PORTAL_DEBUG_SIN(angle), colour);
            NuRndrLine3dDbg(centre->x + lower_radius * PORTAL_DEBUG_COS(angle), lower_y,
                            centre->z + lower_radius * PORTAL_DEBUG_SIN(angle),
                            centre->x + lower_radius * PORTAL_DEBUG_COS(angle + 2048), lower_y,
                            centre->z + lower_radius * PORTAL_DEBUG_SIN(angle + 2048), colour);
        }
    }
}
void __cdecl NuRndrSphereEx(NUVEC *centre, f32 radius, i32 colour, f32 vertical_scale, i32 angle_step) {
    f64 radius_squared = (f64)radius * radius;
    i32 band, angle;
    for (band = 0; band < 8; ++band) {
        f32 lower_radius = radius * PORTAL_DEBUG_SIN((band < 4 ? band : 8 - band) * 4096);
        f32 upper_radius = radius * PORTAL_DEBUG_SIN((band < 3 ? band + 1 : 7 - band) * 4096);
        f32 lower_y = NuFsqrt((f32)(radius_squared - (f64)lower_radius * lower_radius)) * vertical_scale;
        f32 upper_y = NuFsqrt((f32)(radius_squared - (f64)upper_radius * upper_radius)) * vertical_scale;
        if (band > 4)
            lower_y = -lower_y;
        if (band > 3)
            upper_y = -upper_y;
        lower_y += centre->y;
        upper_y += centre->y;
        for (angle = 0; angle < 65536; angle += angle_step) {
            NuRndrLine3dDbg(centre->x + lower_radius * PORTAL_DEBUG_COS(angle), lower_y,
                            centre->z + lower_radius * PORTAL_DEBUG_SIN(angle),
                            centre->x + upper_radius * PORTAL_DEBUG_COS(angle), upper_y,
                            centre->z + upper_radius * PORTAL_DEBUG_SIN(angle), colour);
            NuRndrLine3dDbg(centre->x + lower_radius * PORTAL_DEBUG_COS(angle), lower_y,
                            centre->z + lower_radius * PORTAL_DEBUG_SIN(angle),
                            centre->x + lower_radius * PORTAL_DEBUG_COS(angle + angle_step), lower_y,
                            centre->z + lower_radius * PORTAL_DEBUG_SIN(angle + angle_step), colour);
        }
    }
}

#undef PORTAL_DEBUG_SIN
#undef PORTAL_DEBUG_COS

extern i32 NuPrimCoordSystemStack[];
/* Native EAX-colour calls require the existing genuine private colour/caller owner. */
void __cdecl RenderPostClearScreenPc(void) {
    i32 restore_mode;
    ++NuPrimCSPos;
    NuPrimSetCoordinateSystem(1);
    NuPrim2DBegin(13, 7, NULL);
    NuRndrPrimSetColourPrivate((i32)0x80000000);
    ((LOCATOR_PRIM_VERTEX_PC *)NuPrimStreamBufferPointer->void_ptr)->u = 0.0f;
    ((LOCATOR_PRIM_VERTEX_PC *)NuPrimStreamBufferPointer->void_ptr)->v = 0.0f;
    NuPrim2DAddXYZ(-1.0f, -1.0f, 0.0f);
    NuRndrPrimSetColourPrivate((i32)0x80000000);
    ((LOCATOR_PRIM_VERTEX_PC *)NuPrimStreamBufferPointer->void_ptr)->u = 1.0f;
    ((LOCATOR_PRIM_VERTEX_PC *)NuPrimStreamBufferPointer->void_ptr)->v = 0.0f;
    NuPrim2DAddXYZ(1.0f, -1.0f, 0.0f);
    NuRndrPrimSetColourPrivate((i32)0x80000000);
    ((LOCATOR_PRIM_VERTEX_PC *)NuPrimStreamBufferPointer->void_ptr)->u = 0.0f;
    ((LOCATOR_PRIM_VERTEX_PC *)NuPrimStreamBufferPointer->void_ptr)->v = 1.0f;
    NuPrim2DAddXYZ(-1.0f, 1.0f, 0.0f);
    NuRndrPrimSetColourPrivate((i32)0x80000000);
    ((LOCATOR_PRIM_VERTEX_PC *)NuPrimStreamBufferPointer->void_ptr)->u = 1.0f;
    ((LOCATOR_PRIM_VERTEX_PC *)NuPrimStreamBufferPointer->void_ptr)->v = 1.0f;
    NuPrim2DAddXYZ(1.0f, 1.0f, 0.0f);
    NuPrim2DEnd();
    restore_mode = NuPrimCoordSystemStack[NuPrimCSPos - 1];
    --NuPrimCSPos;
    NuPrimSetCoordinateSystem(restore_mode);
}

STATIC_ASSERT(sizeof(RENDER_POST_VIEWPORT_PREFIX_PC) == 0x30);
extern RENDER_POST_VIEWPORT_PREFIX_PC RenderPostViewportPrefixPc;
void __cdecl RenderPostViewportResetPc(void);
u8 __cdecl RenderPostClampBytePc(i32 minimum_unused, i32 maximum_unused, f32 value);
STATIC_ASSERT(offsetof(NURNDRLIBSCENE, render_post_alpha_cache) == 8);
STATIC_ASSERT(sizeof(((NURNDRLIBSCENE *)0)->render_post_alpha_cache) == 8);
#ifndef RENDER_POST_ALPHA_CACHE_PC
#define RENDER_POST_ALPHA_CACHE_PC (sceneParameters[16].render_post_alpha_cache)
#endif
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define IMAGE_TAN_HALF_FOV(v)                                                                                          \
    (NuTrigTable[(i32)((v) * 10430.3779296875f) >> 1 & 0x7fff] /                                                       \
     NuTrigTable[(16384 - (i32)((v) * -10430.3779296875f)) >> 1 & 0x7fff])
#define POST_IMAGE_UV(U, V)                                                                                            \
    do {                                                                                                               \
        ((LOCATOR_PRIM_VERTEX_PC *)NuPrimStreamBufferPointer->void_ptr)->u = (U);                                      \
        ((LOCATOR_PRIM_VERTEX_PC *)NuPrimStreamBufferPointer->void_ptr)->v = (V);                                      \
    } while (0)
void DrawAlphaImage(i32 rows, i32 cols, NUMTL *material, i32 use_pixel_offsets,
                    RENDER_POST_BLOOM_READER_PC *parameters) {
    f32 inv_col = 1.0f / (cols - 1);
    f32 inv_row = 1.0f / (rows - 1);
    f32 x0 = 0.0f * inv_row * 2.0f - 1.0f;
    f32 y_start = 0.0f * inv_col * 2.0f - 1.0f;
    f32 x1 = inv_row * 2.0f - 1.0f;
    f32 step_x = inv_row * 2.0f;
    f32 step_y = inv_col * 2.0f;
    f32 near_scale = parameters->near_scale * 128.0f;
    f32 far_scale = parameters->far_scale * 128.0f;
    f32 near_angle = parameters->near_angle / 180.0f;
    f32 far_angle = parameters->far_angle / 180.0f;
    f32 angle_delta = far_angle - near_angle;
    f32 scale_delta = far_scale - near_scale;
    RENDER_POST_VIEWPORT_PREFIX_PC saved_prefix = RenderPostViewportPrefixPc;
    PC_SPECIAL_PROJECTION_ADJUST saved_adjust = PCSpecialProjectionAdjust;
    NUCAMERA camera;
    static f32 pixelOffsetX, pixelOffsetY;
    static i32 first = 1;
    static f32 camFov, adjacent;
    static i32 row, col;
    RenderPostViewportResetPc();

    NuCameraGet(&camera);
    if (parameters->clear_screen != 0)
        RenderPostClearScreenPc();
    if (parameters->directional)
        NuVecNorm(&parameters->direction, &parameters->direction);

    if (use_pixel_offsets) {
        pixelOffsetX = 0.5f / (f32)NuTextureDimensions[material->texture_id - 1]->width;
        pixelOffsetY = 0.5f / (f32)NuTextureDimensions[material->texture_id - 1]->height;
    }

    if (first) {
        f32 half_fov = camera.fov * 0.5f;
        camFov = camera.fov;
        adjacent = 1.0f / IMAGE_TAN_HALF_FOV(half_fov);
        first = 0;
    }
    if (camera.fov != camFov) {
        f32 half_fov = camera.fov * 0.5f;
        adjacent = 1.0f / IMAGE_TAN_HALF_FOV(half_fov);
        camera.fov = camFov;
    }
    camera.mtx.m30 = 0.0f;
    camera.mtx.m31 = 0.0f;
    camera.mtx.m32 = 0.0f;
    camera.mtx.m33 = 1.0f;

    ++NuPrimCSPos;
    NuPrimSetCoordinateSystem(1);
    for (row = 0; row < rows - 1; ++row) {
        const i32 strip_row = row;
        f32 direction_x;
        f32 y0 = y_start;
        NuPrim2DBegin(13, 7, material);
        direction_x = (1.0f + x0) - 1.0f;
        for (col = 0; col < cols; ++col) {
            NUVEC direction;
            u32 colour;
            i32 alpha;
            f32 brightness;
            const f32 direction_y = ((1.0f + y0) - 1.0f) * -camera.aspect;
            direction.x = direction_x;
            direction.y = direction_y;
            direction.z = adjacent;
            NuVecNorm(&direction, &direction);
            NuVecMtxTransform(&direction, &direction, &camera.mtx);
            NuVecNorm(&direction, &direction);

            if (strip_row > 0) {
                colour = ((u32)RENDER_POST_ALPHA_CACHE_PC[col] << 24) | 0x808080;
            } else {
                brightness = direction.y * 0.5f + 0.5f;
                if (brightness <= near_angle)
                    brightness = near_scale * parameters->intensity;
                else if (brightness >= far_angle)
                    brightness = far_scale * parameters->intensity;
                else
                    brightness =
                        ((brightness - near_angle) * scale_delta / angle_delta + near_scale) * parameters->intensity;
                if (parameters->directional) {
                    f32 angle = (i16)(0x4000 - RenderPostASinPc(NuVecDot(&parameters->direction, &direction))) *
                                0.0054931640625f;
                    if (angle < parameters->direction_near_angle) {
                        brightness += 128.0f * parameters->direction_far_scale;
                    } else if (!(angle > parameters->direction_far_angle)) {
                        if (parameters->direction_bias != 1.0f) {
                            brightness +=
                                128.0f * ((1.0 - (f64)RenderPostPowPc((angle - parameters->direction_near_angle) /
                                                                          (parameters->direction_far_angle -
                                                                           parameters->direction_near_angle),
                                                                      parameters->direction_bias)) *
                                              (parameters->direction_far_scale - parameters->direction_near_scale) +
                                          parameters->direction_near_scale);
                        } else {
                            brightness +=
                                128.0f *
                                ((1.0 - ((f64)angle - parameters->direction_near_angle) /
                                            ((f64)parameters->direction_far_angle - parameters->direction_near_angle)) *
                                     (parameters->direction_far_scale - parameters->direction_near_scale) +
                                 parameters->direction_near_scale);
                        }
                    }
                }
                alpha = RenderPostClampBytePc(0, 255, brightness);
                colour = ((u32)alpha << 24) | 0x808080;
            }
            NuRndrPrimSetColourPrivate(colour);
            POST_IMAGE_UV((f32)row * inv_row + pixelOffsetX, ((f32)col * inv_col + pixelOffsetY) * 0.9f);
            NuPrim2DAddXYZ(x0, y0, 0.0f);

            direction.x = (1.0f + x1) - 1.0f;
            direction.y = direction_y;
            direction.z = adjacent;
            NuVecNorm(&direction, &direction);
            NuVecMtxTransform(&direction, &direction, &camera.mtx);
            NuVecNorm(&direction, &direction);
            brightness = direction.y * 0.5f + 0.5f;
            if (brightness <= near_angle)
                brightness = near_scale * parameters->intensity;
            else if (brightness >= far_angle)
                brightness = far_scale * parameters->intensity;
            else
                brightness =
                    ((brightness - near_angle) * scale_delta / angle_delta + near_scale) * parameters->intensity;
            if (parameters->directional) {
                f32 angle =
                    (i16)(0x4000 - RenderPostASinPc(NuVecDot(&parameters->direction, &direction))) * 0.0054931640625f;
                if (angle < parameters->direction_near_angle) {
                    brightness += 128.0f * parameters->direction_far_scale;
                } else if (!(angle > parameters->direction_far_angle)) {
                    if (parameters->direction_bias != 1.0f) {
                        brightness +=
                            128.0f * ((1.0 - (f64)RenderPostPowPc((angle - parameters->direction_near_angle) /
                                                                      (parameters->direction_far_angle -
                                                                       parameters->direction_near_angle),
                                                                  parameters->direction_bias)) *
                                          (parameters->direction_far_scale - parameters->direction_near_scale) +
                                      parameters->direction_near_scale);
                    } else {
                        brightness +=
                            128.0f *
                            ((1.0 - ((f64)angle - parameters->direction_near_angle) /
                                        ((f64)parameters->direction_far_angle - parameters->direction_near_angle)) *
                                 (parameters->direction_far_scale - parameters->direction_near_scale) +
                             parameters->direction_near_scale);
                    }
                }
            }
            alpha = (u8)MIN(255.0f, MAX(0.0f, brightness));
            RENDER_POST_ALPHA_CACHE_PC[col] = (u8)alpha;
            NuRndrPrimSetColourPrivate(((u32)alpha << 24) | 0x808080);
            POST_IMAGE_UV((f32)(row + 1) * inv_row + pixelOffsetX, ((f32)col * inv_col + pixelOffsetY) * 0.9f);
            NuPrim2DAddXYZ(x1, y0, 0.0f);
            y0 += step_y;
        }
        NuPrim2DEnd();
        x0 += step_x;
        x1 += step_x;
    }
    {
        i32 restore_mode = NuPrimCoordSystemStack[NuPrimCSPos - 1];
        --NuPrimCSPos;
        NuPrimSetCoordinateSystem(restore_mode);
    }
    RenderPostViewportPrefixPc = saved_prefix;
    PCSpecialProjectionAdjust = saved_adjust;
}

u8 __cdecl RenderPostClampBytePc(i32 minimum_unused, i32 maximum_unused, f32 value) {
    f32 nonnegative;
    f32 clamped;
    (void)minimum_unused;
    (void)maximum_unused;
    nonnegative = 0.0f > value ? 0.0f : value;
    clamped = 255.0f < nonnegative ? 255.0f : nonnegative;
    return (u8)clamped;
}

#include <float.h>
void __cdecl NuRndrStateSetSpecularLightEx(const NUVEC *, const NUMTX *, const NUVEC *);
i32 __cdecl NuRndrSetViewMtx(NUMTX *view_projection, NUMTX *viewport_clip, NUMTX *scissor_clip) {
    return 0;
}

void __cdecl FaceYDirStream(i32 angle) {
}

i32 __cdecl NuRndrSetSpecularLightPS(const NUVEC *direction, const NUVEC4 *intensity) {
    static NUVEC rndrstream_specular_dir = {0.0f, 0.0f, 1.0f};
    static NUVEC4 rndrstream_specular_intensity = {1.0f, 1.0f, 1.0f, 1.0f};
    static NUVEC camvec = {0.0f, 0.0f, -1.0f};
    NUVEC view_direction, half_direction, light_direction, eye_direction, axis;
    NUMTX matrix;
    f32 horizontal, facing, weight, remainder;
    if (direction != NULL)
        rndrstream_specular_dir = *direction;
    if (intensity != NULL)
        rndrstream_specular_intensity = *intensity;
    NuVecInvMtxRotate(&view_direction, &rndrstream_specular_dir, &global_camera.mtx);
    NuVecLerp(&half_direction, &view_direction, &camvec, 0.5f);
    NuVecMtxRotate(&half_direction, &half_direction, &global_camera.mtx);
    NuVecNorm(&half_direction, &half_direction);
    NuVecNorm(&light_direction, &rndrstream_specular_dir);
    eye_direction.x = -global_camera.mtx.m20;
    eye_direction.y = -global_camera.mtx.m21;
    eye_direction.z = -global_camera.mtx.m22;
    facing = NuVecDot(&eye_direction, &light_direction);
    axis = half_direction;
    matrix = NuMtxSetIdentityTemplate;
    horizontal = NuFsqrt(axis.x * axis.x + axis.z * axis.z);
    if (horizontal > FLT_MIN) {
        matrix.m00 = axis.z / horizontal;
        matrix.m10 = (axis.x * axis.y) / horizontal;
        matrix.m20 = -axis.x;
        matrix.m01 = 0.0f;
        matrix.m11 = horizontal;
        matrix.m21 = axis.y;
        matrix.m02 = axis.x / horizontal;
        matrix.m12 = (axis.z * -axis.y) / horizontal;
        matrix.m22 = axis.z;
    } else {
        matrix.m22 = matrix.m11 = 0.0f;
        matrix.m21 = axis.y;
        matrix.m12 = -axis.y;
    }
    matrix.m30 = matrix.m31 = 0.5f;
    if (0.0f > facing) {
        weight = facing * facing;
        weight *= weight;
        weight *= weight;
        weight *= weight;
        weight *= weight;
        remainder = 1.0f - weight;
        if (matrix.m00 * light_direction.x + matrix.m10 * light_direction.y + matrix.m20 * light_direction.z >= 0.0f) {
            matrix.m00 = matrix.m00 * remainder + light_direction.x * weight;
            matrix.m10 = matrix.m10 * remainder + light_direction.y * weight;
            matrix.m20 = matrix.m20 * remainder + light_direction.z * weight;
        } else {
            matrix.m00 = matrix.m00 * remainder - light_direction.x * weight;
            matrix.m10 = matrix.m10 * remainder - light_direction.y * weight;
            matrix.m20 = matrix.m20 * remainder - light_direction.z * weight;
        }
        if (matrix.m01 * light_direction.x + matrix.m11 * light_direction.y + matrix.m21 * light_direction.z >= 0.0f) {
            matrix.m01 = matrix.m01 * remainder + light_direction.x * weight;
            matrix.m11 = matrix.m11 * remainder + light_direction.y * weight;
            matrix.m21 = matrix.m21 * remainder + light_direction.z * weight;
        } else {
            matrix.m01 = matrix.m01 * remainder - light_direction.x * weight;
            matrix.m11 = matrix.m11 * remainder - light_direction.y * weight;
            matrix.m21 = matrix.m21 * remainder - light_direction.z * weight;
        }
    }
    NuVecNeg(&view_direction, &view_direction);
    NuRndrStateSetSpecularLightEx(&view_direction, &matrix, (const NUVEC *)&rndrstream_specular_intensity);
    return 1;
}

typedef struct FRAME_MTL_ANIM_DESC_PC {
    u8 u_mode, v_mode, reserved02[2];
    f32 u_amplitude, v_amplitude, u_frequency, v_frequency;
} FRAME_MTL_ANIM_DESC_PC;
STATIC_ASSERT(sizeof(FRAME_MTL_ANIM_DESC_PC) == 0x14);
STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_DESC_PC, u_frequency) == 0xc);
typedef struct FRAME_MTL_ANIM_PC {
    u8 reserved00[0x7b];
    u8 disable_u_animation, disable_v_animation;
    u8 reserved7d[0x143];
    i32 texture_animation_data[4];
    f32 texture_animation_offsets[4][2];
    u8 reserved1f0[8];
    FRAME_MTL_ANIM_DESC_PC animation[4];
} FRAME_MTL_ANIM_PC;
STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_PC, disable_u_animation) == 0x7b);
STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_PC, disable_v_animation) == 0x7c);
STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_PC, texture_animation_data) == 0x1c0);
STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_PC, texture_animation_offsets) == 0x1d0);
STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_PC, animation) == 0x1f8);
typedef struct FRAME_MTL_ANIM_SET_PC {
    NUDISPLAY_LIST_PC *scene;
    i32 material_count;
    i32 *material_indices;
    struct FRAME_MTL_ANIM_SET_PC *next;
} FRAME_MTL_ANIM_SET_PC;
STATIC_ASSERT(sizeof(FRAME_MTL_ANIM_SET_PC) == 0x10);
STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_SET_PC, next) == 0xc);
typedef struct FRAME_MTL_SCENE_PC {
    u8 reserved00[0x158];
    i32 shader_texture_animation_enabled;
} FRAME_MTL_SCENE_PC;
STATIC_ASSERT(offsetof(FRAME_MTL_SCENE_PC, shader_texture_animation_enabled) == 0x158);

extern void NuDisplayListAnimateMtls(f32);

static f32 FrameShaderMaterialAnimationTimePc;
static i32 FrameShaderMaterialFirstCallPc = 1;
extern f32 FrameMaterialAnimationSpeedPc;
void NuMtlAnimateShaderMtlTextures(f32 frame_time) {
    i32 i;
    FrameShaderMaterialAnimationTimePc += frame_time;
    if (FrameShaderMaterialFirstCallPc != 0) {
        for (i = 0; i < (i32)nudlist_field_9d8; ++i) {
            NUDISPLAY_LIST_PC *scene = PCDisplayScenes[i];
            if (((FRAME_MTL_SCENE_PC *)scene->gscene)->shader_texture_animation_enabled != 0) {
                i32 j;
                for (j = 0; j < (i32)scene->material_count; ++j) {
                    FRAME_MTL_ANIM_PC *material = (FRAME_MTL_ANIM_PC *)scene->display_materials[j];
                    i32 layer;
                    for (layer = 0; layer < 4; ++layer) {
                        material->texture_animation_offsets[layer][0] = 0.0f;
                        material->texture_animation_offsets[layer][1] = 0.0f;
                    }
                }
            }
        }
        FrameShaderMaterialFirstCallPc = 0;
    }
}
void NuMtlAnimate(f32 frame_time) {
    NuDisplayListAnimateMtls(frame_time * FrameMaterialAnimationSpeedPc);
    NuMtlAnimateShaderMtlTextures(frame_time);
}
STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_DESC_PC, u_mode) == 0x0);
STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_DESC_PC, v_mode) == 0x1);
STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_DESC_PC, u_amplitude) == 0x4);
STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_DESC_PC, v_amplitude) == 0x8);
STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_DESC_PC, v_frequency) == 0x10);
STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_SET_PC, scene) == 0x0);
STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_SET_PC, material_count) == 0x4);
STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_SET_PC, material_indices) == 0x8);

void __cdecl NuRndrStreamInit(i32 size, VARIPTR *buffer);
void __cdecl NuRndrInitConfigPc(VARIPTR *buffer, VARIPTR end);
void __cdecl NuRndrInitEx(i32 size, VARIPTR *buffer, VARIPTR end) {
    NuRndrStreamInit(size, buffer);
    NuRndrInitConfigPc(buffer, end);
}

static i32 RendererConfigEnabledPc, RendererConfigSecondEnabledPc;
static NUMTX RendererConfigMatrixPc;
static PC_TEXTURE_RESOURCE_PC *RendererVertexDeclarationPc;
static PC_BUFFER_RESOURCE_PC *RendererQuadVertexBufferPc;
static PC_BUFFER_RESOURCE_PC *RendererGeometryVertexBufferPc;
static PC_BUFFER_RESOURCE_PC *RendererGeometryIndexBufferPc;
static u8 *RendererArenaDataPc;
static u32 *RendererArenaOffsetsPc;
static i32 *RendererArenaTypesPc;

typedef struct RENDERER_CLEAR_RECORD_PC {
    u8 unknown_00[0x20];
    u16 active;
    u8 unknown_22[6];
} RENDERER_CLEAR_RECORD_PC;
STATIC_ASSERT(sizeof(RENDERER_CLEAR_RECORD_PC) == 0x28);
STATIC_ASSERT(offsetof(RENDERER_CLEAR_RECORD_PC, active) == 0x20);
static RENDERER_CLEAR_RECORD_PC RendererClearRecordsPc[32];

typedef struct RENDERER_SHADOW_DEFAULTS_PC {
    f32 state0, state1, state2;
    u32 packed_colour;
} RENDERER_SHADOW_DEFAULTS_PC;
STATIC_ASSERT(sizeof(RENDERER_SHADOW_DEFAULTS_PC) == 16);
STATIC_ASSERT(offsetof(RENDERER_SHADOW_DEFAULTS_PC, state0) == 0);
STATIC_ASSERT(offsetof(RENDERER_SHADOW_DEFAULTS_PC, state1) == 4);
STATIC_ASSERT(offsetof(RENDERER_SHADOW_DEFAULTS_PC, state2) == 8);
STATIC_ASSERT(offsetof(RENDERER_SHADOW_DEFAULTS_PC, packed_colour) == 12);

typedef struct RENDERER_SCREEN_VERTEX_PC {
    f32 x, y, z, w;
    u32 attribute_bits;
    u8 unknown_14[8];
} RENDERER_SCREEN_VERTEX_PC;
STATIC_ASSERT(sizeof(RENDERER_SCREEN_VERTEX_PC) == 0x1c);
STATIC_ASSERT(offsetof(RENDERER_SCREEN_VERTEX_PC, x) == 0);
STATIC_ASSERT(offsetof(RENDERER_SCREEN_VERTEX_PC, y) == 4);
STATIC_ASSERT(offsetof(RENDERER_SCREEN_VERTEX_PC, z) == 8);
STATIC_ASSERT(offsetof(RENDERER_SCREEN_VERTEX_PC, w) == 12);
STATIC_ASSERT(offsetof(RENDERER_SCREEN_VERTEX_PC, attribute_bits) == 16);
STATIC_ASSERT(offsetof(RENDERER_SCREEN_VERTEX_PC, unknown_14) == 20);

void __cdecl NuRndrInitGeneric(void);

__declspec(noinline) void __cdecl NuRndrArenaInitPc(VARIPTR *buffer, VARIPTR end) {
    RendererArenaDataPc = (u8 *)((buffer->addr + 3) & ~3u);
    buffer->addr = ((buffer->addr + 3) & ~3u) + 0x36b00;
    RendererArenaOffsetsPc = (u32 *)((buffer->addr + 3) & ~3u);
    buffer->addr = ((buffer->addr + 3) & ~3u) + 0x7d0;
    RendererArenaTypesPc = (i32 *)((buffer->addr + 3) & ~3u);
    buffer->addr = ((buffer->addr + 3) & ~3u) + 0x7d0;
}

void AnglesBetweenPoints(NUVEC *from, NUVEC *to, u16 *vertical, u16 *horizontal) {
    f32 x = to->x - from->x;
    f32 z = to->z - from->z;
    f32 y = to->y - from->y;
    *vertical = NuAtan2D(y, NuFsqrt(x * x + z * z));
    *horizontal = NuAtan2D(x, z);
}

void __cdecl NuRndrQuadBufferInitPc(void) {
    void *data;
    RENDERER_SCREEN_VERTEX_PC *vertices;
    if (RendererQuadVertexBufferPc != NULL) {
        RendererQuadVertexBufferPc->methods->release(RendererQuadVertexBufferPc);
        RendererQuadVertexBufferPc = NULL;
    }
    PCDeviceCreateVertexBuffer(0x70, 0, 0, 1, &RendererQuadVertexBufferPc);
    if (RendererQuadVertexBufferPc->methods->lock(RendererQuadVertexBufferPc, 0, 0, &data, 0) < 0)
        return;
    vertices = (RENDERER_SCREEN_VERTEX_PC *)data;
    vertices[0].x = 0.0f;
    vertices[0].y = 0.0f;
    vertices[0].z = 1.0f;
    vertices[1].x = (f32)(u32)PS2_REZ_W;
    vertices[1].y = 0.0f;
    vertices[1].z = 1.0f;
    vertices[2].x = (f32)(u32)PS2_REZ_W;
    vertices[2].y = (f32)(u32)PS2_REZ_H;
    vertices[2].z = 1.0f;
    vertices[3].x = 0.0f;
    vertices[3].y = (f32)(u32)PS2_REZ_H;
    vertices[3].z = 1.0f;
    vertices[3].attribute_bits = 0x4b000000;
    vertices[2].attribute_bits = 0x4b000000;
    vertices[1].attribute_bits = 0x4b000000;
    vertices[0].attribute_bits = 0x4b000000;
    vertices[3].w = 1.0f;
    vertices[2].w = 1.0f;
    vertices[1].w = 1.0f;
    vertices[0].w = 1.0f;
    RendererQuadVertexBufferPc->methods->unlock(RendererQuadVertexBufferPc);
}

void __cdecl NuRndrGeometryBuffersInitPc(void) {
    void *data;
    u16 *indices;
    i32 remaining;
    u32 base;
    if (RendererGeometryVertexBufferPc != NULL) {
        RendererGeometryVertexBufferPc->methods->release(RendererGeometryVertexBufferPc);
        RendererGeometryVertexBufferPc = NULL;
    }
    if (RendererGeometryIndexBufferPc != NULL) {
        RendererGeometryIndexBufferPc->methods->release(RendererGeometryIndexBufferPc);
        RendererGeometryIndexBufferPc = NULL;
    }
    PCDeviceReleaseTexture((PC_TEXTURE_RESOURCE_PC **)&RendererGeometryVertexBufferPc);
    PCDeviceCreateVertexBuffer(0x40000, 0x208, 0, 0, &RendererGeometryVertexBufferPc);
    NuRndrQuadBufferInitPc();
    PCDeviceCreateIndexBuffer(0xc000, 0, 0x65, 1, &RendererGeometryIndexBufferPc, NULL);
    if (RendererGeometryIndexBufferPc->methods->lock(RendererGeometryIndexBufferPc, 0, 0, &data, 0) < 0)
        return;
    indices = (u16 *)data;
    remaining = 0x1000;
    base = 0;
    do {
        *indices++ = (u16)base;
        *indices++ = (u16)(base + 1);
        *indices++ = (u16)(base + 2);
        *indices++ = (u16)base;
        *indices++ = (u16)(base + 2);
        *indices++ = (u16)(base + 3);
        base += 4;
    } while (--remaining != 0);
    RendererGeometryIndexBufferPc->methods->unlock(RendererGeometryIndexBufferPc);
}

void __cdecl NuRndrBufferDescriptorsInitPc(VARIPTR *buffer, VARIPTR end) {
    PC_VERTEX_ELEMENT_PC elements[3];
    elements[0].stream = 0;
    elements[0].offset = 0;
    elements[0].type = 2;
    elements[0].method = 0;
    elements[0].usage = 0;
    elements[0].usage_index = 0;
    elements[1].stream = 0;
    elements[1].offset = 12;
    elements[1].type = 4;
    elements[1].method = 0;
    elements[1].usage = 10;
    elements[1].usage_index = 0;
    elements[2].stream = 0xff;
    elements[2].offset = 0;
    elements[2].type = 17;
    elements[2].method = 0;
    elements[2].usage = 0;
    elements[2].usage_index = 0;
    PCDeviceCreateVertexDeclaration(elements, &RendererVertexDeclarationPc);
    NuRndrArenaInitPc(buffer, end);
    NuRndrQuadBufferInitPc();
    NuRndrGeometryBuffersInitPc();
}

void __cdecl NuRndrInitConfigPc(VARIPTR *buffer, VARIPTR end) {
    RENDERER_SHADOW_DEFAULTS_PC *shadow = (RENDERER_SHADOW_DEFAULTS_PC *)&PCShadowState;
    RENDERER_CLEAR_RECORD_PC *record;
    RendererConfigEnabledPc = 1;
    RendererConfigSecondEnabledPc = 1;
    NuMtxSetIdentity(&RendererConfigMatrixPc);
    NuRndrBufferDescriptorsInitPc(buffer, end);
    shadow->state0 = 0.0f;
    shadow->state1 = -0.75f;
    shadow->state2 = -0.75f;
    PCShadowNearValue = 4.0f;
    PCShadowFarValue = 6.0f;
    shadow->packed_colour = 0x00101010;
    NuRndrShadowCnt = 0;
    for (record = RendererClearRecordsPc; record < RendererClearRecordsPc + 32; ++record)
        record->active = 0;
    NuRndrInitGeneric();
}

ShaderUniformRecordPc *__cdecl NuShaderUniformGetByString(const char *name) {
    i32 i;
    for (i = 0; i < 100; ++i) {
        ShaderUniformRecordPc *record = &ShaderUniformRecordsPc[i];
        if ((record->fragment_name != NULL && strcmp(record->fragment_name, name) == 0) ||
            (record->vertex_name != NULL && strcmp(record->vertex_name, name) == 0))
            return record;
    }
    return NULL;
}

/* PC 601a30. Minimum reader for this initialization cohort, not a NUMTL extent. */
typedef struct PC_STARTUP_MATERIAL_READER_PC {
    u8 unknown_00[0x40];
    u32 render_flags;
    u32 secondary_flags;
    u8 unknown_48[0xc];
    NUVEC diffuse;
    u8 unknown_60[0x10];
    f32 opacity;
    i16 texture_id;
    i16 sort_priority;
} PC_STARTUP_MATERIAL_READER_PC;
STATIC_ASSERT(sizeof(PC_STARTUP_MATERIAL_READER_PC) == 0x78);
STATIC_ASSERT(offsetof(PC_STARTUP_MATERIAL_READER_PC, render_flags) == 0x40);
STATIC_ASSERT(offsetof(PC_STARTUP_MATERIAL_READER_PC, secondary_flags) == 0x44);
STATIC_ASSERT(offsetof(PC_STARTUP_MATERIAL_READER_PC, diffuse) == 0x54);
STATIC_ASSERT(offsetof(PC_STARTUP_MATERIAL_READER_PC, opacity) == 0x70);
STATIC_ASSERT(offsetof(PC_STARTUP_MATERIAL_READER_PC, texture_id) == 0x74);
STATIC_ASSERT(offsetof(PC_STARTUP_MATERIAL_READER_PC, sort_priority) == 0x76);

void __cdecl NuRndrInitStartupMaterialsPc(void) {
    PC_STARTUP_MATERIAL_READER_PC *material;
    PCStartupMaterial1Pc = NuMtlCreate(1);
    material = (PC_STARTUP_MATERIAL_READER_PC *)PCStartupMaterial1Pc;
    material->diffuse.x = material->diffuse.y = material->diffuse.z = 1.0f;
    material->render_flags = (material->render_flags & ~0x1000u) | 0x2000u;
    material->render_flags = (material->render_flags & ~0x8000u) | 0x4000u;
    material->opacity = 0.999f;
    material->render_flags = (material->render_flags & 0xfffffff1u) | 1u;
    material->secondary_flags = (material->secondary_flags & ~2u) | 1u;
    material->render_flags = (material->render_flags & ~0x200u) | 0x100u;
    material->render_flags = (material->render_flags & ~0x800u) | 0x400u;
    material->render_flags = (material->render_flags & ~0x10000u) | 0x20000u;
    material->texture_id = 0;
    material->sort_priority = 0x100;
    NuMtlUpdate(PCStartupMaterial1Pc);

    PCStartupMaterial2Pc = NuMtlCreate(1);
    material = (PC_STARTUP_MATERIAL_READER_PC *)PCStartupMaterial2Pc;
    material->diffuse.x = material->diffuse.y = material->diffuse.z = 1.0f;
    material->render_flags = (material->render_flags & ~0x1000u) | 0x2000u;
    material->render_flags = (material->render_flags & ~0x8000u) | 0x4000u;
    material->opacity = 0.999f;
    material->render_flags = (material->render_flags & 0xfffffff2u) | 2u;
    material->secondary_flags = (material->secondary_flags & ~2u) | 1u;
    material->render_flags = (material->render_flags & ~0x200u) | 0x100u;
    material->render_flags = (material->render_flags & ~0x800u) | 0x400u;
    material->render_flags = (material->render_flags & ~0x10000u) | 0x20000u;
    material->texture_id = 0;
    material->sort_priority = 0x100;
    NuMtlUpdate(PCStartupMaterial2Pc);
}

void __cdecl NuRndrLine2di(i32 x0, i32 y0, i32 x1, i32 y1, i32 colour, NUMTL *material) {
    LOCATOR_PRIM_VERTEX_PC *vertex;
    ++NuPrimCSPos;
    NuPrimSetCoordinateSystem(0);
    NuPrim2DBegin(14, 7, material);
    NuRndrPrimSetColourPrivate(colour);
    vertex = (LOCATOR_PRIM_VERTEX_PC *)NuPrimStreamBufferPointer->void_ptr;
    vertex->u = 0.0f;
    vertex->v = 0.0f;
    NuPrim2DAddXYZ((f32)((f64)x0 * 0.0625), (f32)((f64)y0 * 0.0625), 0.0f);
    NuRndrPrimSetColourPrivate(colour);
    vertex = (LOCATOR_PRIM_VERTEX_PC *)NuPrimStreamBufferPointer->void_ptr;
    vertex->u = 1.0f;
    vertex->v = 1.0f;
    NuPrim2DAddXYZ((f32)((f64)x1 * 0.0625), (f32)((f64)y1 * 0.0625), 0.0f);
    NuPrim2DEnd();
    --NuPrimCSPos;
    NuPrimSetCoordinateSystem(NuPrimCoordSystemStack[NuPrimCSPos]);
}

void __cdecl NuRndrRect2di(i32 x, i32 y, i32 width, i32 height, i32 colour, NUMTL *material) {
    f32 sx = (f32)((f64)x * 0.0625);
    f32 sy = (f32)((f64)y * 0.0625);
    f32 sw;
    f32 sh;
    LOCATOR_PRIM_VERTEX_PC *vertex;
    ++NuPrimCSPos;
    NuPrimSetCoordinateSystem(0);
    NuPrim2DBegin(16, 7, material);
    NuRndrPrimSetColourPrivate(colour);
    vertex = (LOCATOR_PRIM_VERTEX_PC *)NuPrimStreamBufferPointer->void_ptr;
    vertex->u = 0.0f;
    vertex->v = 0.0f;
    NuPrim2DAddXYZ(sx, sy, 0.0f);
    NuRndrPrimSetColourPrivate(colour);
    vertex = (LOCATOR_PRIM_VERTEX_PC *)NuPrimStreamBufferPointer->void_ptr;
    vertex->u = 1.0f;
    vertex->v = 1.0f;
    sw = (f32)((f64)width * 0.0625);
    sh = (f32)((f64)height * 0.0625);
    NuPrim2DAddXYZ(sx + sw, sy + sh, 0.0f);
    NuPrim2DEnd();
    --NuPrimCSPos;
    NuPrimSetCoordinateSystem(NuPrimCoordSystemStack[NuPrimCSPos]);
}

void __cdecl NuRndrRectUV2di(i32 x, i32 y, i32 width, i32 height, f32 u0, f32 v0, f32 u1, f32 v1, i32 colour,
                             NUMTL *material) {
    f32 sx = (f32)((f64)x * 0.0625);
    f32 sy = (f32)((f64)y * 0.0625);
    f32 sw;
    f32 sh;
    LOCATOR_PRIM_VERTEX_PC *vertex;
    ++NuPrimCSPos;
    NuPrimSetCoordinateSystem(0);
    NuPrim2DBegin(16, 7, material);
    vertex = (LOCATOR_PRIM_VERTEX_PC *)NuPrimStreamBufferPointer->void_ptr;
    vertex->u = u0;
    vertex->v = v0;
    NuRndrPrimSetColourPrivate(colour);
    NuPrim2DAddXYZ(sx, sy, 0.0f);
    vertex = (LOCATOR_PRIM_VERTEX_PC *)NuPrimStreamBufferPointer->void_ptr;
    vertex->u = u1;
    vertex->v = v1;
    NuRndrPrimSetColourPrivate(colour);
    sw = (f32)((f64)width * 0.0625);
    sh = (f32)((f64)height * 0.0625);
    NuPrim2DAddXYZ(sx + sw, sy + sh, 0.0f);
    NuPrim2DEnd();
    --NuPrimCSPos;
    NuPrimSetCoordinateSystem(NuPrimCoordSystemStack[NuPrimCSPos]);
}
