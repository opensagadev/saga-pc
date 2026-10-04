#include "saga.h"

#include <string.h>

NURNDRLIBSCENE currentScene;
NUVPCURRENT vpCurrent;
i32 sceneParametersCount;
NURNDRLIBSTATE *displaylist_2d_state;
NUDISPLAYLISTITEM *displaylist_2d_first;
NUDISPLAYLISTITEM *displaylist_2d_mtl_last;
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
    currentScene.calc_1c = (f32)(0.0625 * vpCurrent.width / ((f64)DebrisGlassTextureWidth / PS2_VREZ_W));
    currentScene.calc_20 = (f32)(0.0625 * vpCurrent.height / ((f64)DebrisGlassTextureHeight / PS2_VREZ_H));
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
    *x = NuFdiv((f32)vpCurrent.x * 0.0625f - ((f32)PS2_VCNTR_X - (f32)(DebrisGlassTextureWidth >> 1)),
                NuFdiv((f32)DebrisGlassTextureWidth, (f32)PS2_VREZ_W));
    *y = NuFdiv((f32)vpCurrent.y * 0.0625f - ((f32)PS2_VCNTR_Y - (f32)(DebrisGlassTextureHeight >> 1)),
                NuFdiv((f32)DebrisGlassTextureHeight, (f32)PS2_VREZ_H));
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

    if (count == 0 && displaylist_2d_mtl_last == displaylist_2d_first) {
        return -1;
    }

    result = (NuDisplayListStreamCursor->addr + 3U) & ~3U;
    render_scene_ptrs[render_scene_index] = (NUDISPLAYLISTRENDERSCENE *)result;
    NuDisplayListStreamCursor->addr = result + sizeof(NUDISPLAYLISTRENDERSCENE);
    render_scene_ptrs[render_scene_index]->nsort_pris = count;
    render_scene_ptrs[render_scene_index]->sort_pris = pris;

    if (displaylist_2d_mtl_last != displaylist_2d_first) {
        *(u8 *)displaylist_2d_mtl_last = 0x84;
        *(u32 *)((u8 *)displaylist_2d_mtl_last + 4) = 0;
        *((u8 *)displaylist_2d_mtl_last + 1) = 4;
        item = displaylist_2d_first;
        *(NUDISPLAYLISTITEM *)((u8 *)render_scene_ptrs[render_scene_index] + 8) = *item;
        NuDisplayListResetStateCache(displaylist_2d_state);
        displaylist_2d_mtl_last = displaylist_2d_first;
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

void __cdecl NuDisplayListStreamAlloc(VARIPTR *cursor, i32 arg2, i32 count, i32 arg4, VARIPTR *arg5) {
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

void __cdecl NuDisplayListInit(VARIPTR *cursor, i32 arg2) {
    STUBBED_NAMED("NuDisplayListInit: 0x5fd1c0 setup");
    nudlist_field_9d8 = 0;
    nudlist_field_e80 = 0;
    sort_list = NULL;
    nudlist_field_fc8 = 0;
    displaylist_2d_first = &nudlist_2d_static;
    STUBBED_NAMED("NuDisplayListInit: NuDisplayListCreateDynMtlList (0x5efbf0)");
    nudlist_flags |= 0x10;
    nudlist_field_e58 = 0;
    NuDisplayListStreamAlloc(cursor, arg2, 0x40, arg2, cursor);
    NuDisplayListStreamCursor = &nudlist_stream_cursor;
    nudlist_field_1598 = nuprim_field_ba8;
    STUBBED_NAMED("NuDisplayListInit: 0x4ffa60 setup");
    STUBBED_NAMED("NuDisplayListInit: 0x5f0d70 first-frame draw");
    STUBBED_NAMED("NuDisplayListInit: NuDisplayListBeginFrame (0x5dfab0)");
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
