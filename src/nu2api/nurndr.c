#include "editor/edlevelall.h"
#include "globals.h"
#include "legoapi/render/fx/game_deb.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/nu3d/nudlist.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nu3d/nuprim.h"
#include "nu2api/nu3d/nurndr.h"
#include "nu2api/nu3d/nurndrstat.h"
#include "nu2api/nu3d/nushaderobject.h"
#include "nu2api/nu3d/nuvport.h"
#include "nu2api/nucore/NuSpeedBlurFilter.h"
#include "nu2api/nucore/nuapi.h"
#include "nu2api/nugscn_gen.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/numath_includes.h"
#include "nu2api/numath/numaths.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuplane.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/numath/nuvec4.h"
#include "nu2api/nushadermtldesc_gen.h"
#include "nu2api/nustencilshadows_PC.h"
#include "nu2api/nuvport_gen.h"
#include "nu2api/nuwin32/d3dCore.h"

#include <string.h>
#include <float.h>

NURNDRLIBSCENE currentScene = {0};
f32 PCShaderMaterialSamplerBiasOffset = 0.0f;
u32 PCWindFrameCount = 0;
i32 global_frame_count_paused = 0;
i32 dat_00973934 = 0;
i32 dat_00973938 = 0;
i32 dat_0097393c = 0;
NUVPCURRENT vpCurrent = {0};
i32 sceneParametersCount = 0;
NUDISPLAYLIST PCDisplayList2d = {0};
NUDISPLAYLISTITEM nudlist_2d_static = {0};
i32 nused_sort_pris = 0;
NUSORTPRI *sort_list = 0;
i32 render_scene_index = 0;
NURNDRLIBSCENE sceneParameters[16] = {0};
u8 cacheValues[256] = {0};
NUDISPLAYLISTRENDERSCENE *render_scene_ptrs[24] = {0};
NUDISPLAYLISTRENDERSCENE *PCSafeRenderScenes[24] = {0};
i32 nudlist_alloc_count = 0;
u32 nudlist_alloc_base = 0;
u32 nudlist_alloc_end = 0;
u32 nudlist_item_base = 0;
struct PC_MATERIAL_ANIM_SET *PCMaterialAnimSets = NULL;
u32 nudlist_field_fe4 = 0;
i32 DisplaySceneRenderDisabled = 0;
i32 noscenespecials = 0;
i32 PCShadowPacketSequence = 0;
f32 PCShadowNearValue = 0;
f32 PCShadowFarValue = 0;
i32 PCShadowPacketCount = 0;

i32 NuRndrBeginScene(i32 begin_flags) {
    currentScene.clear_alpha_4c = 0.0f;
    currentScene.clear_flags_08 = 0;
    currentScene.clear_flags = 0;
    currentScene.render_frame_flags = 0;
    currentScene.burnout_enabled = 0;
    currentScene.bloom_enabled = 0;
    currentScene.dyn_light_count = 0;
    currentScene.dyn_light_ptr = NULL;
    currentScene.deferred_shading_enabled = 0;
    currentScene.dof.enabled = 0;
    currentScene.texture_blend_enabled = 0;
    currentScene.motion_effect_set = 0;
    currentScene.motion_params_set = 0;
    currentScene.accumulation_effect_set = 0;
    currentScene.accumulation_params_set = 0;
    currentScene.speed_blur.enabled = 0;
    currentScene.backbuffer_copy_texture = 0;
    currentScene.scene_id = -1;
    currentScene.light_render_set = -1;
    return 1;
}

void NuRndrClear(i32 clear_flags, i32 bg_colour, f32 alpha) {
    currentScene.clear_alpha = alpha;
    currentScene.clear_flags_08 |= clear_flags;
    currentScene.bg_colour = bg_colour;
    NuVpGetPosition2(&currentScene.viewport_x, &currentScene.viewport_y);
    currentScene.calc_1c = (f32)(0.0625 * vpCurrent.width / ((f64)PS2_REZ_W / PS2_VREZ_W));
    currentScene.calc_20 = (f32)(0.0625 * vpCurrent.height / ((f64)PS2_REZ_H / PS2_VREZ_H));
}

void NuRndrSphereTRS(NUVEC *translation, NUANGVEC *rotation, NUVEC *scale, i32 colour, i32 segments, NUMTL *material) {
    NUMTX matrix;
    if (rotation != NULL) {
        NuMtxSetRotateXYZ(&matrix, rotation);
    } else {
        matrix = NuMtxSetIdentityTemplate;
    }
    if (translation != NULL) {
        NuMtxTranslate(&matrix, translation);
    }
    if (scale != NULL) {
        NuMtxPreScale(&matrix, scale);
    }
    NuRndrSphereMtx(&matrix, colour, segments, material);
}

void NuRndrAxisArrows(NUVEC *position, void *unused1, f32 length, NUMTL *material) {
    NUMTX matrix;
    NuMtxSetIdentity(&matrix);
    NuMtxTranslate(&matrix, position);
    NuRndrAxisArrowsMtx(&matrix, length, material);
}

static f32 NuFdiv(f32 a, f32 b) {
    if (a == 0.0f || b == 0.0f) {
        return 0.0f;
    }
    return a / b;
}

void NuVpGetPosition2(f32 *x, f32 *y) {
    *x = NuFdiv((f32)vpCurrent.x * 0.0625f - ((f32)PS2_VCNTR_X - (f32)(PS2_REZ_W >> 1)), NuFdiv((f32)PS2_REZ_W, (f32)PS2_VREZ_W));
    *y = NuFdiv((f32)vpCurrent.y * 0.0625f - ((f32)PS2_VCNTR_Y - (f32)(PS2_REZ_H >> 1)), NuFdiv((f32)PS2_REZ_H, (f32)PS2_VREZ_H));
}

i32 NuDisplayListAddRenderScene(void) {
    VARIPTR *cursor;
    NUSORTPRI **pris;
    NUSORTPRI *node;
    NUSORTPRI *rebuilt;
    NUDISPLAY_LIST *sub;
    NUDISPLAYLIST *dlist;
    NUDISPLAYLISTITEM *item;
    i32 count;
    i32 n;
    i32 m;
    i32 result;

    NuDisplayListStreamCursor->addr = (NuDisplayListStreamCursor->addr + 0xfU) & ~0xfU;
    cursor = NuDisplayListStreamCursor;
    count = 0;
    pris = (NUSORTPRI **)cursor->addr;
    cursor->addr = (u32)(pris + nused_sort_pris + 1);

    for (node = sort_list; node != NULL; node = node->sys_next) {
        sub = node->display_scene;
        if (sub != NULL && (sub->render_flags & 0x10) != 0) {
            if ((sub->render_flags & 0x2) == 0) {
                continue;
            }
            rebuilt = (NUSORTPRI *)((cursor->addr + 3U) & ~3U);
            cursor->addr = ((cursor->addr + 3U) & ~3U) + sizeof(NUSORTPRI);
            rebuilt->sort_pri = node->sort_pri;
            rebuilt->items = (NUDISPLAYLISTITEM *)cursor->addr;
            rebuilt->display_scene = node->display_scene;
            rebuilt->nmtls = node->nmtls;
            rebuilt->mtl_first = node->mtl_first;
            n = 0;
            for (m = 0; m < node->nmtls; m++) {
                dlist = node->display_scene->material_lists[node->mtl_first + m];
                if (dlist->mtl_last == dlist->first) {
                    continue;
                }
                item = dlist->mtl_item;
                if (item[1].type == NUDLIST_MTL) {
                    rebuilt->items[n] = item[1];
                } else {
                    rebuilt->items[n] = item[3];
                }
                n++;
                rebuilt->items[n++] = *dlist->first;
                dlist->mtl_last->next = &rebuilt->items[n];
                dlist->mtl_last = dlist->first;
                NuDisplayListResetStateCache(dlist->state);
            }
            rebuilt->items[n].id = NUDLISTITEM_END;
            rebuilt->items[n].next = NULL;
            rebuilt->items[n].type = NUDLIST_TERMINATE;
            cursor->addr += (n + 1) * sizeof(NUDISPLAYLISTITEM);
            pris[count++] = rebuilt;
            rebuilt->flags |= 0x2;
        } else {
            if ((node->flags & 0x2) != 0) {
                continue;
            }
            if (sub != NULL && (sub->render_flags & 0x6) == 0) {
                continue;
            }
            pris[count++] = node;
            node->flags |= 0x2;
        }
    }

    if (count == 0 && PCDisplayList2d.mtl_last == PCDisplayList2d.first) {
        return -1;
    }

    render_scene_ptrs[render_scene_index] = (NUDISPLAYLISTRENDERSCENE *)((cursor->addr + 3U) & ~3U);
    cursor->addr = ((cursor->addr + 3U) & ~3U) + sizeof(NUDISPLAYLISTRENDERSCENE);
    render_scene_ptrs[render_scene_index]->nsort_pris = count;
    render_scene_ptrs[render_scene_index]->sort_pris = pris;

    if (PCDisplayList2d.mtl_last != PCDisplayList2d.first) {
        item = PCDisplayList2d.mtl_last;
        item->type = NUDLIST_TERMINATE;
        item->next = NULL;
        item->id = NUDLISTITEM_END;
        render_scene_ptrs[render_scene_index]->render_2d_first = *PCDisplayList2d.first;
        NuDisplayListResetStateCache(PCDisplayList2d.state);
        PCDisplayList2d.mtl_last = PCDisplayList2d.first;
    } else {
        item = &render_scene_ptrs[render_scene_index]->render_2d_first;
        item->type = NUDLIST_TERMINATE;
        item->next = NULL;
        item->id = NUDLISTITEM_END;
    }

    render_scene_index += 1;
    return render_scene_index - 1;
}

void NuRndrEndScene(void) {
    i32 scene_id = NuDisplayListAddRenderScene();

    currentScene.scene_id = scene_id;
    if (currentScene.dyn_light_count != 0 && scene_id != -1) {
        NURNDRLIGHTRECORD *records = currentScene.dyn_light_ptr;
        NURNDRLIGHTRECORD *record;

        if (records[2].enabled != 0) {
            record = &records[currentScene.light_render_set];
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

void NuDisplayListStreamAlloc(VARIPTR *cursor, i32 arg2, i32 count) {
    i32 index;

    nudlist_alloc_count = count;
    nudlist_alloc_base = cursor->addr;
    cursor->addr += count;
    nudlist_alloc_end = (cursor->addr + 3U) & ~3U;
    cursor->addr = ((cursor->addr + 3U) & ~3U) + count * 36;
    nudlist_item_base = (cursor->addr + 0xfU) & ~0xfU;
    cursor->addr = ((cursor->addr + 0xfU) & ~0xfU) + count * 32;
    for (index = 0; index < count; index++) {
        NUDISPLAYLISTITEM *item;
        *(u8 *)(nudlist_alloc_base + index) = 0;
        item = (NUDISPLAYLISTITEM *)(nudlist_item_base + index * 0x20 + 0x10);
        item->type = NUDLIST_TERMINATE;
        item->next = NULL;
        item->id = NUDLISTITEM_END;
    }
}

void NuRndrCalcRandEllipsePos(NUVEC4 *position, NUMTX *matrix, NUVEC *axes) {
    NUVEC random;
    f32 length;
    f32 scale;

    random.x = (f32)((f64)NuWindRand() - 32768.0);
    random.y = (f32)((f64)NuWindRand() - 32768.0);
    random.z = (f32)((f64)NuWindRand() - 32768.0);
    length = (f32)((f64)random.x * random.x + (f64)random.y * random.y + (f64)random.z * random.z);
    scale = 1.0f / NuFsqrt(length);
    random.x *= scale;
    random.y *= scale;
    random.z *= scale;
    position->x = axes[1].x * random.x + axes[2].x * random.y + axes[3].x * random.z + axes[0].x;
    position->y = axes[1].y * random.x + axes[2].y * random.y + axes[3].y * random.z + axes[0].y;
    position->z = axes[1].z * random.x + axes[2].z * random.y + axes[3].z * random.z + axes[0].z;
    position->w = 1.0f;
    NuVec4MtxTransformVU0(position, position, matrix);
}

void NuRndrCalcRandCylinderPos(NUVEC4 *position, NUMTX *matrix, NUVEC *axes) {
    NUVEC random;
    f32 length;
    f32 scale;

    random.x = (f32)((f64)(NuWindRand() - 32768) * (1.0 / 32768.0));
    random.y = (f32)((f64)NuWindRand() - 32768.0);
    random.z = (f32)((f64)NuWindRand() - 32768.0);
    length = (f32)((f64)random.y * random.y + (f64)random.z * random.z);
    scale = 1.0f / NuFsqrt(length);
    random.y *= scale;
    random.z *= scale;
    position->x = axes[1].x * random.x + axes[2].x * random.y + axes[3].x * random.z + axes[0].x;
    position->y = axes[1].y * random.x + axes[2].y * random.y + axes[3].y * random.z + axes[0].y;
    position->z = axes[1].z * random.x + axes[2].z * random.y + axes[3].z * random.z + axes[0].z;
    position->w = 1.0f;
    NuVec4MtxTransformVU0(position, position, matrix);
}

void NuRndrStartReflectionRender(i32 clip) {
    PCSpecialReflectionPass = 1;
    RndrStateSetReflection(1);
}

void NuRndrEndReflectionRender(void) {
    PCSpecialReflectionPass = 0;
    RndrStateSetReflection(0);
}

i32 fun_005d46d0(void) {
    return dat_00973934;
}

i32 fun_005d46e0(void) {
    return dat_0097393c;
}

void fun_005d46f0(void) {
    dat_00973938 = 1;
}

void fun_005d4700(void) {
    dat_00973938 = 0;
}

i32 fun_005d4710(void) {
    return dat_00973938;
}

void fun_005d4720(void) {
}

i32 fun_005d4730(void) {
    return 0;
}

void NuRndrSetWind(f32 speed, f32 scale) {
    global_windspeed = speed;
    global_windscale = scale;
}

u32 NuRndrGetWindFrameCount(void) {
    return PCWindFrameCount;
}

void NuRndrGlobalFrameCountPause(i32 paused) {
    global_frame_count_paused = paused;
}

void NuRndrSetBurnoutParameters(i32 view, const BURN_PARAMETERS *parameters) {
    currentScene.burnout_parameters = *parameters;
}

void NuDepthOfFieldEffectEx(const NU_POST_DOF_PARAMETERS *parameters) {
    currentScene.dof = *parameters;
    if (NuRndrDoingScreenGrab != 0) {
        currentScene.dof.enabled = 0;
    }
}

void NuDepthOfFieldEffect(f32 strength, f32 near_distance, f32 far_distance) {
    currentScene.dof.enabled = 1;
    currentScene.dof.strength = strength;
    currentScene.dof.near_distance = near_distance;
    currentScene.dof.far_distance = far_distance;
    currentScene.dof.bias = 0;
    currentScene.dof.mode = 3;
    if (NuRndrDoingScreenGrab != 0) {
        currentScene.dof.enabled = 0;
    }
}

void NuDepthOfFieldEffect1(f32 near_distance, f32 far_distance) {
    f32 ratio = near_distance / (near_distance + far_distance);
    near_distance = (far_distance - near_distance) * ratio + near_distance;
    NuDepthOfFieldEffect(1.0f, (far_distance * near_distance) / ((far_distance - near_distance) + far_distance), near_distance);
}

void NuDepthOfFieldEffect2(f32 near_distance, f32 far_distance, f32 strength) {
    f32 ratio = near_distance / (near_distance + far_distance);
    near_distance = (far_distance - near_distance) * ratio + near_distance;
    NuDepthOfFieldEffect(strength, (far_distance * near_distance) / ((far_distance - near_distance) + far_distance), near_distance);
}

void NuTextureBlendEffect(i32 arg0, i32 arg1, NUVEC4 *parameters) {
    currentScene.texture_blend_arg0 = arg0;
    currentScene.texture_blend_enabled = 1;
    currentScene.texture_blend_arg1 = arg1;
    currentScene.texture_blend_parameters = *parameters;
}

void NuCameraMotionBlurParams(const NUMTX *current) {
    currentScene.motion_params_set = 1;
    currentScene.motion_current = *current;
}

void NuCameraMotionBlurEffect(const NUMTX *previous, const NUMTX *current, f32 scale, f32 maximum, f32 falloff) {
    currentScene.motion_effect_set = 1;
    currentScene.motion_previous = *previous;
    currentScene.motion_current = *current;
    currentScene.motion_scale = scale;
    currentScene.motion_maximum = maximum;
    currentScene.motion_falloff = falloff;
}

void NuAccumulationMotionBlurParams(i32 frames, f32 blend, f32 mode) {
    currentScene.accumulation_blend = blend;
    currentScene.accumulation_params_set = 1;
    currentScene.accumulation_frames = frames;
    currentScene.accumulation_mode = mode;
}

void NuAccumulationMotionBlurEffect(i32 frames, f32 blend, f32 mode) {
    currentScene.accumulation_blend = blend;
    currentScene.accumulation_params_set = 1;
    currentScene.accumulation_effect_set = 1;
    currentScene.accumulation_frames = frames;
    currentScene.accumulation_mode = mode;
    motionBlurAccumActiveThisFrame = 1;
}

void NuDeferredShadingRender(void) {
    currentScene.deferred_shading_enabled = 1;
}

void NuRndrSetShadowCoefficient(i32 index, f32 value) {
    switch (index) {
        case 0:
            currentScene.shadow_coefficients[0] = value;
            break;
        case 1:
            currentScene.shadow_coefficients[1] = value;
            break;
        case 2:
            currentScene.shadow_coefficients[2] = value;
            break;
        default:
            AnimationDiagnosticNoOp();
            break;
    }
}

void NuDynamicLightingGetParameterfv(i32 parameter, f32 *out) {
    switch (parameter) {
        case 0:
            *out = currentScene.shadow_coefficients[0];
            break;
        case 1:
            *out = currentScene.shadow_coefficients[1];
            break;
        case 2:
            *out = currentScene.shadow_coefficients[2];
            break;
        default:
            AnimationDiagnosticNoOp();
            break;
    }
}

void NuLightSpeedBlurOldCameraPos(NUMTX *camera) {
    NU_POST_SPEED_PARAMETERS *parameters = &currentScene.speed_blur;
    parameters->current = *camera;
    NuMtxInv(&parameters->current, &parameters->current);
}

void NuBackbufferCopy(i32 texture_id) {
    currentScene.backbuffer_copy_texture = texture_id;
}

f32 NuCeilf(f32 value) {
    f32 result = (i32)value;
    if (result < value) {
        result += 1.0f;
    }
    return result;
}

f32 NuRndrClampf(f32 value, f32 minimum, f32 maximum) {
    return value < minimum ? minimum : (value > maximum ? maximum : value);
}

void NuRndrRequestFrameFlags(void) {
    currentScene.render_frame_flags = 1;
}

void fun_005d5250(i32 value) {
    dat_0080b6e4 = value;
}

void NuRndrSetSamplerBiasOffset(f32 offset) {
    PCShaderMaterialSamplerBiasOffset = offset;
}

static void NuRndrPrimSetColourPrivate(i32 colour) {
    i32 converted;
    if (NuPrimNeedsOverbrightening != 0) {
        converted = ((colour & 0xff) << 16) + ((colour >> 16) & 0xff) + (colour & 0xff00) + (colour & 0xff000000);
    } else {
        converted = ((colour & 0xfe) << 15) + ((colour >> 1) & 0x7f00) + ((colour >> 17) & 0x7f) + (colour & 0xff000000);
    }
    NuPrimStreamBufferPointer->vertex_ptr->colour = converted;
}

// Writes the position of the current primitive vertex and advances to the next one.
static void NuRndrPrimPosition(f32 x, f32 y, f32 z) {
    NuPrimStreamBufferPointer->vertex_ptr->x = x;
    NuPrimStreamBufferPointer->vertex_ptr->y = y;
    NuPrimStreamBufferPointer->vertex_ptr->z = z;
    NuPrimStreamBufferPointer->void_ptr = NuPrimStreamBufferPointer->vertex_ptr + 1;
    ++NuPrimVertexCount;
}

void NuRndrAxisArrowsMtx(NUMTX *matrix, f32 length, NUMTL *material) {
    f32 base = length * 0.8f;
    f32 width = length * 0.1f;
    NuPrim3DBegin(14, 5, material, matrix);
    NuRndrPrimSetColourPrivate((i32)0xff0000ff);
    NuRndrPrimPosition(0, 0, 0);
    NuRndrPrimSetColourPrivate((i32)0xff0000ff);
    NuRndrPrimPosition(length, 0, 0);
    NuRndrPrimSetColourPrivate((i32)0xff00ff00);
    NuRndrPrimPosition(0, 0, 0);
    NuRndrPrimSetColourPrivate((i32)0xff00ff00);
    NuRndrPrimPosition(0, length, 0);
    NuRndrPrimSetColourPrivate((i32)0xffff0000);
    NuRndrPrimPosition(0, 0, 0);
    NuRndrPrimSetColourPrivate((i32)0xffff0000);
    NuRndrPrimPosition(0, 0, length);
    NuPrim3DEnd();
    NuPrim3DBegin(15, 5, material, matrix);
    NuRndrPrimSetColourPrivate((i32)0xff0000ff);
    NuRndrPrimPosition(base, -width, 0);
    NuRndrPrimSetColourPrivate((i32)0xff0000ff);
    NuRndrPrimPosition(length, 0, 0);
    NuRndrPrimSetColourPrivate((i32)0xff0000ff);
    NuRndrPrimPosition(base, width, 0);
    NuPrim3DEnd();
    NuPrim3DBegin(15, 5, material, matrix);
    NuRndrPrimSetColourPrivate((i32)0xff00ff00);
    NuRndrPrimPosition(0, base, width);
    NuRndrPrimSetColourPrivate((i32)0xff00ff00);
    NuRndrPrimPosition(0, length, 0);
    NuRndrPrimSetColourPrivate((i32)0xff00ff00);
    NuRndrPrimPosition(0, base, -width);
    NuPrim3DEnd();
    NuPrim3DBegin(15, 5, material, matrix);
    NuRndrPrimSetColourPrivate((i32)0xffff0000);
    NuRndrPrimPosition(0, width, base);
    NuRndrPrimSetColourPrivate((i32)0xffff0000);
    NuRndrPrimPosition(0, 0, length);
    NuRndrPrimSetColourPrivate((i32)0xffff0000);
    NuRndrPrimPosition(0, -width, base);
    NuPrim3DEnd();
}

void NuPrimPopCoordinateSystem3(void) {
    NuPrimSetCoordinateSystem(NuPrimCoordSystemStack[--NuPrimCSPos]);
}

void fun_005c2080(void) {
}

void NuRndrSphereMtx(NUMTX *matrix, i32 colour, i32 segments, NUMTL *material) {
    f32 step = (f32)(65536.0 / segments);
    f32 theta = 0.0f;
    i32 ring;
    i32 i;
    NuPrim3DBegin(15, 5, material, matrix);
    for (ring = 0; ring < segments / 2; ++ring) {
        f32 sin_theta = NuTrigTable[((i32)theta >> 1) & 0x7fff];
        f32 cos_theta = NuTrigTable[((i32)(theta + 16384.0) >> 1) & 0x7fff];
        f32 angle = 0.0f;
        for (i = 0; i <= segments; ++i) {
            f32 radius = NuTrigTable[((i32)angle >> 1) & 0x7fff];
            NuRndrPrimSetColourPrivate(colour);
            NuRndrPrimPosition(radius * sin_theta, NuTrigTable[((i32)(angle + 16384.0) >> 1) & 0x7fff], radius * cos_theta);
            angle += step;
        }
        theta += step;
    }
    NuPrim3DEnd();
    theta = step;
    for (ring = 1; ring < segments / 2; ++ring) {
        f32 radius = NuTrigTable[((i32)theta >> 1) & 0x7fff];
        f32 y = NuTrigTable[((i32)(theta + 16384.0) >> 1) & 0x7fff];
        f32 angle = 0.0f;
        NuPrim3DBegin(15, 5, material, matrix);
        for (i = 0; i <= segments; ++i) {
            NuRndrPrimSetColourPrivate(colour);
            NuRndrPrimPosition(NuTrigTable[((i32)angle >> 1) & 0x7fff] * radius, y,
                               NuTrigTable[((i32)(angle + 16384.0) >> 1) & 0x7fff] * radius);
            angle += step;
        }
        NuPrim3DEnd();
        theta += step;
    }
}

void NuRndrBoundingBox(NUVEC *minimum, NUVEC *maximum, NUMTX *matrix, i32 colour) {
    NuPrim3DBegin(15, 5, NULL, matrix);
    NuRndrPrimSetColourPrivate(colour);
    NuRndrPrimPosition(minimum->x, minimum->y, minimum->z);
    NuRndrPrimSetColourPrivate(colour);
    NuRndrPrimPosition(minimum->x, maximum->y, minimum->z);
    NuRndrPrimSetColourPrivate(colour);
    NuRndrPrimPosition(maximum->x, maximum->y, minimum->z);
    NuRndrPrimSetColourPrivate(colour);
    NuRndrPrimPosition(maximum->x, minimum->y, minimum->z);
    NuRndrPrimSetColourPrivate(colour);
    NuRndrPrimPosition(minimum->x, minimum->y, minimum->z);
    NuRndrPrimSetColourPrivate(colour);
    NuRndrPrimPosition(minimum->x, minimum->y, maximum->z);
    NuRndrPrimSetColourPrivate(colour);
    NuRndrPrimPosition(minimum->x, maximum->y, maximum->z);
    NuRndrPrimSetColourPrivate(colour);
    NuRndrPrimPosition(maximum->x, maximum->y, maximum->z);
    NuRndrPrimSetColourPrivate(colour);
    NuRndrPrimPosition(maximum->x, minimum->y, maximum->z);
    NuRndrPrimSetColourPrivate(colour);
    NuRndrPrimPosition(minimum->x, minimum->y, maximum->z);
    NuPrim3DEnd();
    NuPrim3DBegin(14, 5, NULL, matrix);
    NuRndrPrimSetColourPrivate(colour);
    NuRndrPrimPosition(minimum->x, maximum->y, minimum->z);
    NuRndrPrimSetColourPrivate(colour);
    NuRndrPrimPosition(minimum->x, maximum->y, maximum->z);
    NuRndrPrimSetColourPrivate(colour);
    NuRndrPrimPosition(maximum->x, maximum->y, minimum->z);
    NuRndrPrimSetColourPrivate(colour);
    NuRndrPrimPosition(maximum->x, maximum->y, maximum->z);
    NuRndrPrimSetColourPrivate(colour);
    NuRndrPrimPosition(maximum->x, minimum->y, minimum->z);
    NuRndrPrimSetColourPrivate(colour);
    NuRndrPrimPosition(maximum->x, minimum->y, maximum->z);
    NuPrim3DEnd();
}

void NuRndrAxisBright(NUMTX *matrix, f32 length, i32 brightness) {
    f32 base = length * 0.8f;
    f32 width = length * 0.1f;
    i32 red = (i32)(0xff000000u | (brightness & 0xff));
    i32 green = (i32)(0xff000000u | ((brightness & 0xff) << 8));
    i32 blue = (i32)(0xff000000u | ((brightness & 0xff) << 16));
    NuPrim3DBegin(14, 5, NULL, matrix);
    NuRndrPrimSetColourPrivate(red);
    NuRndrPrimPosition(0, 0, 0);
    NuRndrPrimSetColourPrivate(red);
    NuRndrPrimPosition(length, 0, 0);
    NuRndrPrimSetColourPrivate(green);
    NuRndrPrimPosition(0, 0, 0);
    NuRndrPrimSetColourPrivate(green);
    NuRndrPrimPosition(0, length, 0);
    NuRndrPrimSetColourPrivate(blue);
    NuRndrPrimPosition(0, 0, 0);
    NuRndrPrimSetColourPrivate(blue);
    NuRndrPrimPosition(0, 0, length);
    NuPrim3DEnd();
    NuPrim3DBegin(15, 5, NULL, matrix);
    NuRndrPrimSetColourPrivate(red);
    NuRndrPrimPosition(base, -width, 0);
    NuRndrPrimSetColourPrivate(red);
    NuRndrPrimPosition(length, 0, 0);
    NuRndrPrimSetColourPrivate(red);
    NuRndrPrimPosition(base, width, 0);
    NuPrim3DEnd();
    NuPrim3DBegin(15, 5, NULL, matrix);
    NuRndrPrimSetColourPrivate(green);
    NuRndrPrimPosition(0, base, width);
    NuRndrPrimSetColourPrivate(green);
    NuRndrPrimPosition(0, length, 0);
    NuRndrPrimSetColourPrivate(green);
    NuRndrPrimPosition(0, base, -width);
    NuPrim3DEnd();
    NuPrim3DBegin(15, 5, NULL, matrix);
    NuRndrPrimSetColourPrivate(blue);
    NuRndrPrimPosition(0, width, base);
    NuRndrPrimSetColourPrivate(blue);
    NuRndrPrimPosition(0, 0, length);
    NuRndrPrimSetColourPrivate(blue);
    NuRndrPrimPosition(0, -width, base);
    NuPrim3DEnd();
}

void NuRndrAxes(NUMTX *matrix, f32 length) {
    NuRndrAxisBright(matrix, length, 0xff);
}

void NuRndrGrid(NUVEC *centre, NUVEC *size, i32 columns, i32 rows) {
    NUMTX matrix = NuMtxSetIdentityTemplate;
    f32 dx;
    f32 dz;
    f32 pos;
    i32 i;
    matrix.m30 = centre->x;
    matrix.m31 = 0.0f;
    matrix.m32 = centre->z;
    matrix.m00 = size->x * 0.5;
    matrix.m11 = 1.0f;
    matrix.m22 = size->z * 0.5;
    dx = (f32)(2.0 / columns);
    dz = (f32)(2.0 / rows);
    NuPrim3DBegin(14, 5, NULL, &matrix);
    pos = -1.0f;
    for (i = 0; i <= columns; ++i) {
        NuRndrPrimSetColourPrivate((i32)0xff00ff00);
        NuRndrPrimPosition(pos, 0.0f, -1.0f);
        NuRndrPrimSetColourPrivate((i32)0xff00ff00);
        NuRndrPrimPosition(pos, 0.0f, 1.0f);
        pos += dx;
    }
    pos = -1.0f;
    for (i = 0; i <= rows; ++i) {
        if (NuPrimNeedsOverbrightening != 0) {
            NuPrimStreamBufferPointer->vertex_ptr->colour = 0xff00ff00;
        } else {
            NuPrimStreamBufferPointer->vertex_ptr->colour = 0xff007f00;
        }
        NuRndrPrimPosition(-1.0f, 0.0f, pos);
        if (NuPrimNeedsOverbrightening != 0) {
            NuPrimStreamBufferPointer->vertex_ptr->colour = 0xff00ff00;
        } else {
            NuPrimStreamBufferPointer->vertex_ptr->colour = 0xff007f00;
        }
        NuRndrPrimPosition(1.0f, 0.0f, pos);
        pos += dz;
    }
    NuPrim3DEnd();
}

i32 NuRndrLine3d(NURND_VERTEX3D *vertices, NUMTL *material, NUMTX *matrix) {
    NuPrim3DBegin(14, 7, material, matrix);
    {
        NULGTVERTEX *output;
        f32 u;
        f32 v;
        f32 x;
        f32 y;
        f32 z;
        NuRndrPrimSetColourPrivate((i32)vertices[0].colour);
        u = vertices[0].u;
        v = vertices[0].v;
        output = NuPrimStreamBufferPointer->vertex_ptr;
        output->u = u;
        output->v = v;
        x = vertices[0].position.x;
        y = vertices[0].position.y;
        z = vertices[0].position.z;
        NuPrimStreamBufferPointer->vertex_ptr->x = x;
        NuPrimStreamBufferPointer->vertex_ptr->y = y;
        NuPrimStreamBufferPointer->vertex_ptr->z = z;
        NuPrimStreamBufferPointer->addr += sizeof(NULGTVERTEX);
        ++NuPrimVertexCount;
    }
    {
        NULGTVERTEX *output;
        f32 u;
        f32 v;
        f32 x;
        f32 y;
        f32 z;
        NuRndrPrimSetColourPrivate((i32)vertices[1].colour);
        u = vertices[1].u;
        v = vertices[1].v;
        output = NuPrimStreamBufferPointer->vertex_ptr;
        output->u = u;
        output->v = v;
        x = vertices[1].position.x;
        y = vertices[1].position.y;
        z = vertices[1].position.z;
        NuPrimStreamBufferPointer->vertex_ptr->x = x;
        NuPrimStreamBufferPointer->vertex_ptr->y = y;
        NuPrimStreamBufferPointer->vertex_ptr->z = z;
        NuPrimStreamBufferPointer->addr += sizeof(NULGTVERTEX);
        ++NuPrimVertexCount;
    }
    NuPrim3DEnd();
    return 1;
}

i32 NuRndrStrip3d(NURND_VERTEX3D *vertices, NUMTL *material, NUMTX *matrix, i32 count) {
    i32 i;
    if (count == 0) {
        return 1;
    }
    NuPrim3DBegin(13, 7, material, matrix);
    for (i = 0; i < count; ++i) {
        NULGTVERTEX *output;
        f32 u;
        f32 v;
        f32 x;
        f32 y;
        f32 z;
        NuRndrPrimSetColourPrivate((i32)vertices[i].colour);
        u = vertices[i].u;
        v = vertices[i].v;
        output = NuPrimStreamBufferPointer->vertex_ptr;
        output->u = u;
        output->v = v;
        x = vertices[i].position.x;
        y = vertices[i].position.y;
        z = vertices[i].position.z;
        NuPrimStreamBufferPointer->vertex_ptr->x = x;
        NuPrimStreamBufferPointer->vertex_ptr->y = y;
        NuPrimStreamBufferPointer->vertex_ptr->z = z;
        NuPrimStreamBufferPointer->addr += sizeof(NULGTVERTEX);
        ++NuPrimVertexCount;
    }
    NuPrim3DEnd();
    return 1;
}

i32 NuRndrTriStrip3dClip(NURND_VERTEX3D *vertices, i32 count, NUMTX *matrix, NUMTL *material) {
    return NuRndrStrip3d(vertices, material, matrix, count);
}

i32 NuRndrTri3dClip(NURND_VERTEX3D *vertices, i32 count, NUMTX *matrix, NUMTL *material) {
    i32 i;
    if (count == 0) {
        return 1;
    }
    NuPrim3DBegin(12, 7, material, matrix);
    for (i = 0; i < count; ++i) {
        NULGTVERTEX *output;
        f32 u;
        f32 v;
        f32 x;
        f32 y;
        f32 z;
        NuRndrPrimSetColourPrivate((i32)vertices[i].colour);
        u = vertices[i].u;
        v = vertices[i].v;
        output = NuPrimStreamBufferPointer->vertex_ptr;
        output->u = u;
        output->v = v;
        x = vertices[i].position.x;
        y = vertices[i].position.y;
        z = vertices[i].position.z;
        NuPrimStreamBufferPointer->vertex_ptr->x = x;
        NuPrimStreamBufferPointer->vertex_ptr->y = y;
        NuPrimStreamBufferPointer->vertex_ptr->z = z;
        NuPrimStreamBufferPointer->addr += sizeof(NULGTVERTEX);
        ++NuPrimVertexCount;
    }
    NuPrim3DEnd();
    return 1;
}

extern i32 NuPrimCoordSystemStack[2];

// Writes the texture coordinates of the current primitive vertex.
static __forceinline void NuRndrPrimUV(f32 u, f32 v) {
    NULGTVERTEX *uv_vertex = NuPrimStreamBufferPointer->vertex_ptr;
    uv_vertex->u = u;
    uv_vertex->v = v;
}

void NuRndr3dLine(f32 x0, f32 y0, f32 z0, f32 x1, f32 y1, f32 z1, i32 colour) {
    NuPrim3DBegin(14, 5, NULL, &NuMtxSetIdentityTemplate);
    NuRndrPrimSetColourPrivate(colour);
    NuRndrPrimUV(0.0f, 0.0f);
    NuPrimStreamBufferPointer->vertex_ptr->x = x0;
    NuPrimStreamBufferPointer->vertex_ptr->y = y0;
    NuPrimStreamBufferPointer->vertex_ptr->z = z0;
    NuPrimStreamBufferPointer->addr += sizeof(NULGTVERTEX);
    ++NuPrimVertexCount;
    NuRndrPrimSetColourPrivate(colour);
    NuRndrPrimUV(1.0f, 1.0f);
    NuPrimStreamBufferPointer->vertex_ptr->x = x1;
    NuPrimStreamBufferPointer->vertex_ptr->y = y1;
    NuPrimStreamBufferPointer->vertex_ptr->z = z1;
    NuPrimStreamBufferPointer->addr += sizeof(NULGTVERTEX);
    ++NuPrimVertexCount;
    NuPrim3DEnd();
}

void NuRndrLine3dDbg(f32 x0, f32 y0, f32 z0, f32 x1, f32 y1, f32 z1, i32 colour) {
    NuPrim3DBegin(14, 5, NULL, &NuMtxSetIdentityTemplate);
    NuRndrPrimSetColourPrivate(colour);
    NuPrimStreamBufferPointer->vertex_ptr->x = x0;
    NuPrimStreamBufferPointer->vertex_ptr->y = y0;
    NuPrimStreamBufferPointer->vertex_ptr->z = z0;
    NuPrimStreamBufferPointer->addr += sizeof(NULGTVERTEX);
    ++NuPrimVertexCount;
    NuRndrPrimSetColourPrivate(colour);
    NuPrimStreamBufferPointer->vertex_ptr->x = x1;
    NuPrimStreamBufferPointer->vertex_ptr->y = y1;
    NuPrimStreamBufferPointer->vertex_ptr->z = z1;
    NuPrimStreamBufferPointer->addr += sizeof(NULGTVERTEX);
    ++NuPrimVertexCount;
    NuPrim3DEnd();
}

void NuRndrRectUV2diZ(i32 x, i32 y, i32 width, i32 height, f32 u0, f32 v0, f32 u1, f32 v1, i32 colour, NUMTL *material, i32 z) {
    f32 sx = (f32)((f64)x * 0.0625);
    f32 sy = (f32)((f64)y * 0.0625);
    f32 sz = (f32)((f64)z * (1.0f / 16777215.0f));
    f32 sw;
    f32 sh;
    NULGTVERTEX *vertex;
    ++NuPrimCSPos;
    NuPrimSetCoordinateSystem(0);
    NuPrim2DBegin(16, 7, material);
    vertex = NuPrimStreamBufferPointer->vertex_ptr;
    vertex->u = u0;
    vertex->v = v0;
    NuRndrPrimSetColourPrivate(colour);
    NuPrim2DAddXYZ(sx, sy, sz);
    vertex = NuPrimStreamBufferPointer->vertex_ptr;
    vertex->u = u1;
    vertex->v = v1;
    NuRndrPrimSetColourPrivate(colour);
    sw = (f32)((f64)width * 0.0625);
    sh = (f32)((f64)height * 0.0625);
    NuPrim2DAddXYZ(sx + sw, sy + sh, sz);
    NuPrim2DEnd();
    --NuPrimCSPos;
    NuPrimSetCoordinateSystem(NuPrimCoordSystemStack[NuPrimCSPos]);
}

void NuRndrGradRect2di(i32 x, i32 y, i32 width, i32 height, u32 *colours, NUMTL *material) {
    f32 sx = (f32)((f64)x * 0.0625);
    f32 sy = (f32)((f64)y * 0.0625);
    f32 sw = (f32)((f64)width * 0.0625);
    f32 sh = (f32)((f64)height * 0.0625);
    NULGTVERTEX *vertex;
    ++NuPrimCSPos;
    NuPrimSetCoordinateSystem(0);
    NuPrim2DBegin(13, 7, material);
    NuRndrPrimSetColourPrivate((i32)colours[0]);
    vertex = NuPrimStreamBufferPointer->vertex_ptr;
    vertex->u = 0.0f;
    vertex->v = 0.0f;
    NuPrim2DAddXYZ(sx, sy, 0.0f);
    NuRndrPrimSetColourPrivate((i32)colours[1]);
    vertex = NuPrimStreamBufferPointer->vertex_ptr;
    vertex->u = 1.0f;
    vertex->v = 0.0f;
    NuPrim2DAddXYZ(sx + sw, sy, 0.0f);
    NuRndrPrimSetColourPrivate((i32)colours[2]);
    vertex = NuPrimStreamBufferPointer->vertex_ptr;
    vertex->u = 0.0f;
    vertex->v = 1.0f;
    NuPrim2DAddXYZ(sx, sy + sh, 0.0f);
    NuRndrPrimSetColourPrivate((i32)colours[3]);
    vertex = NuPrimStreamBufferPointer->vertex_ptr;
    vertex->u = 1.0f;
    vertex->v = 1.0f;
    NuPrim2DAddXYZ(sx + sw, sy + sh, 0.0f);
    NuPrim2DEnd();
    --NuPrimCSPos;
    NuPrimSetCoordinateSystem(NuPrimCoordSystemStack[NuPrimCSPos]);
}

void NuRndrGradRectUV2di(i32 x, i32 y, i32 width, i32 height, f32 u0, f32 v0, f32 u1, f32 v1, u32 *colours, void *material) {
    f32 sx = (f32)((f64)x * 0.0625);
    f32 sy = (f32)((f64)y * 0.0625);
    f32 sw = (f32)((f64)width * 0.0625);
    f32 sh = (f32)((f64)height * 0.0625);
    NULGTVERTEX *vertex;
    ++NuPrimCSPos;
    NuPrimSetCoordinateSystem(0);
    NuPrim2DBegin(13, 7, material);
    NuRndrPrimSetColourPrivate((i32)colours[0]);
    vertex = NuPrimStreamBufferPointer->vertex_ptr;
    vertex->u = u0;
    vertex->v = v0;
    NuPrim2DAddXYZ(sx, sy, 0.0f);
    NuRndrPrimSetColourPrivate((i32)colours[1]);
    vertex = NuPrimStreamBufferPointer->vertex_ptr;
    vertex->u = u1;
    vertex->v = v0;
    NuPrim2DAddXYZ(sx + sw, sy, 0.0f);
    NuRndrPrimSetColourPrivate((i32)colours[2]);
    vertex = NuPrimStreamBufferPointer->vertex_ptr;
    vertex->u = u0;
    vertex->v = v1;
    NuPrim2DAddXYZ(sx, sy + sh, 0.0f);
    NuRndrPrimSetColourPrivate((i32)colours[3]);
    vertex = NuPrimStreamBufferPointer->vertex_ptr;
    vertex->u = u1;
    vertex->v = v1;
    NuPrim2DAddXYZ(sx + sw, sy + sh, 0.0f);
    NuPrim2DEnd();
    --NuPrimCSPos;
    NuPrimSetCoordinateSystem(NuPrimCoordSystemStack[NuPrimCSPos]);
}

void NuRndrGradRect2diZ(i32 x, i32 y, i32 width, i32 height, u32 *colours, NUMTL *material, i32 z) {
    f32 sx = (f32)((f64)x * 0.0625);
    f32 sy = (f32)((f64)y * 0.0625);
    f32 sw = (f32)((f64)width * 0.0625);
    f32 sh = (f32)((f64)height * 0.0625);
    f32 sz = (f32)((f64)z * (1.0f / 16777215.0f));
    ++NuPrimCSPos;
    NuPrimSetCoordinateSystem(0);
    NuPrim2DBegin(13, 7, material);
    NuRndrPrimSetColourPrivate((i32)colours[0]);
    NuPrim2DAddXYZ(sx, sy, sz);
    NuRndrPrimSetColourPrivate((i32)colours[1]);
    NuPrim2DAddXYZ(sx + sw, sy, sz);
    NuRndrPrimSetColourPrivate((i32)colours[2]);
    NuPrim2DAddXYZ(sx, sy + sh, sz);
    NuRndrPrimSetColourPrivate((i32)colours[3]);
    NuPrim2DAddXYZ(sx + sw, sy + sh, sz);
    NuPrim2DEnd();
    NuPrimSetCoordinateSystem(NuPrimCoordSystemStack[--NuPrimCSPos]);
}

i32 NuRndrLineStrip2d(NURND_VERTEX2D *vertices, NUMTL *material, i32 count) {
    i32 i;
    ++NuPrimCSPos;
    NuPrimSetCoordinateSystem(0);
    NuPrim2DBegin(14, 7, material);
    for (i = 0; i < count; ++i) {
        NULGTVERTEX *output;
        f32 x;
        f32 y;
        NuRndrPrimSetColourPrivate(-1);
        output = NuPrimStreamBufferPointer->vertex_ptr;
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

void NuRndrSphere(NUVEC *centre, f32 radius, i32 colour, f32 vertical_scale) {
    f64 radius_squared = (f64)radius * radius;
    i32 band;
    i32 angle;
    for (band = 0; band < 8; ++band) {
        f32 lower_radius = radius * NU_SIN_LUT((band < 4 ? band : 8 - band) * 4096);
        f32 upper_radius = radius * NU_SIN_LUT((band < 3 ? band + 1 : 7 - band) * 4096);
        f32 lower_y = NuFsqrt((f32)(radius_squared - (f64)lower_radius * lower_radius)) * vertical_scale;
        f32 upper_y = NuFsqrt((f32)(radius_squared - (f64)upper_radius * upper_radius)) * vertical_scale;
        if (band > 4) {
            lower_y = -lower_y;
        }
        if (band > 3) {
            upper_y = -upper_y;
        }
        lower_y += centre->y;
        upper_y += centre->y;
        for (angle = 0; angle < 65536; angle += 2048) {
            NuRndrLine3dDbg(centre->x + lower_radius * NU_COS_LUT(angle), lower_y, centre->z + lower_radius * NU_SIN_LUT(angle),
                            centre->x + upper_radius * NU_COS_LUT(angle), upper_y, centre->z + upper_radius * NU_SIN_LUT(angle), colour);
            NuRndrLine3dDbg(centre->x + lower_radius * NU_COS_LUT(angle), lower_y, centre->z + lower_radius * NU_SIN_LUT(angle),
                            centre->x + lower_radius * NU_COS_LUT(angle + 2048), lower_y,
                            centre->z + lower_radius * NU_SIN_LUT(angle + 2048), colour);
        }
    }
}

void NuRndrSphereEx(NUVEC *centre, f32 radius, i32 colour, f32 vertical_scale, i32 angle_step) {
    f64 radius_squared = (f64)radius * radius;
    i32 band;
    i32 angle;
    for (band = 0; band < 8; ++band) {
        f32 lower_radius;
        f32 upper_radius;
        f32 lower_y;
        f32 upper_y;
        if (band < 4) {
            lower_radius = radius * NU_SIN_LUT(band * 4096);
        } else {
            lower_radius = radius * NU_SIN_LUT((8 - band) * 4096);
        }
        if (band < 3) {
            upper_radius = radius * NU_SIN_LUT((band + 1) * 4096);
        } else {
            upper_radius = radius * NU_SIN_LUT((7 - band) * 4096);
        }
        lower_y = NuFsqrt((f32)(radius_squared - (f64)lower_radius * lower_radius)) * vertical_scale;
        upper_y = NuFsqrt((f32)(radius_squared - (f64)upper_radius * upper_radius)) * vertical_scale;
        if (band > 4) {
            lower_y = -lower_y;
        }
        if (band > 3) {
            upper_y = -upper_y;
        }
        lower_y = centre->y + lower_y;
        upper_y = centre->y + upper_y;
        for (angle = 0; angle < 65536; angle += angle_step) {
            NuRndrLine3dDbg(centre->x + lower_radius * NU_COS_LUT(angle), lower_y, centre->z + lower_radius * NU_SIN_LUT(angle),
                            centre->x + upper_radius * NU_COS_LUT(angle), upper_y, centre->z + upper_radius * NU_SIN_LUT(angle), colour);
            NuRndrLine3dDbg(centre->x + lower_radius * NU_COS_LUT(angle), lower_y, centre->z + lower_radius * NU_SIN_LUT(angle),
                            centre->x + lower_radius * NU_COS_LUT(angle + angle_step), lower_y,
                            centre->z + lower_radius * NU_SIN_LUT(angle + angle_step), colour);
        }
    }
}

extern i32 NuPrimCoordSystemStack[];

/* Native EAX-colour calls require the existing genuine private colour/caller owner. */
void RenderPostClearScreen(void) {
    i32 restore_mode;
    NULGTVERTEX *vertex;
    ++NuPrimCSPos;
    NuPrimSetCoordinateSystem(1);
    NuPrim2DBegin(13, 7, NULL);
    NuRndrPrimSetColourPrivate((i32)0x80000000);
    vertex = NuPrimStreamBufferPointer->vertex_ptr;
    vertex->u = 0.0f;
    vertex->v = 0.0f;
    NuPrim2DAddXYZ(-1.0f, -1.0f, 0.0f);
    NuRndrPrimSetColourPrivate((i32)0x80000000);
    vertex = NuPrimStreamBufferPointer->vertex_ptr;
    vertex->u = 1.0f;
    vertex->v = 0.0f;
    NuPrim2DAddXYZ(1.0f, -1.0f, 0.0f);
    NuRndrPrimSetColourPrivate((i32)0x80000000);
    vertex = NuPrimStreamBufferPointer->vertex_ptr;
    vertex->u = 0.0f;
    vertex->v = 1.0f;
    NuPrim2DAddXYZ(-1.0f, 1.0f, 0.0f);
    NuRndrPrimSetColourPrivate((i32)0x80000000);
    vertex = NuPrimStreamBufferPointer->vertex_ptr;
    vertex->u = 1.0f;
    vertex->v = 1.0f;
    NuPrim2DAddXYZ(1.0f, 1.0f, 0.0f);
    NuPrim2DEnd();
    restore_mode = NuPrimCoordSystemStack[NuPrimCSPos - 1];
    --NuPrimCSPos;
    NuPrimSetCoordinateSystem(restore_mode);
}

void DrawAlphaImage(i32 rows, i32 cols, NUMTL *material, i32 use_pixel_offsets, NU_POST_BLOOM_PARAMETERS *parameters) {
    f32 inv_col = 1.0f / (cols - 1);
    f32 inv_row = 1.0f / (rows - 1);
    f32 x0 = 0.0f * inv_row * 2.0f - 1.0f;
    f32 y0 = 0.0f * inv_col * 2.0f - 1.0f;
    f32 y_start = y0;
    f32 x1 = inv_row * 2.0f - 1.0f;
    f32 step_x = inv_row * 2.0f;
    f32 step_y = inv_col * 2.0f;
    f32 near_scale = parameters->near_scale * 128.0f;
    f32 far_scale = parameters->far_scale * 128.0f;
    f32 near_angle = parameters->near_angle / 180.0f;
    f32 far_angle = parameters->far_angle / 180.0f;
    f32 scale_delta = far_scale - near_scale;
    f32 angle_delta = far_angle - near_angle;
    RENDER_POST_VIEWPORT_PREFIX saved_prefix = RenderPostViewportPrefix;
    PC_SPECIAL_PROJECTION_ADJUST saved_adjust = PCSpecialProjectionAdjust;
    __declspec(align(16)) NUCAMERA camera;
    static f32 pixelOffsetX = 0;
    static f32 pixelOffsetY = 0;
    static i32 first = 1;
    static f32 camFov = 0;
    static f32 adjacent = 0;
    static i32 row = 0;
    static i32 col = 0;
    RenderPostViewportReset();

    camera = global_camera;
    if (parameters->clear_screen != 0) {
        RenderPostClearScreen();
    }
    if (parameters->directional) {
        NuVecNorm(&parameters->direction, &parameters->direction);
    }

    if (use_pixel_offsets) {
        pixelOffsetX = 0.5f / (f32)NuTextureDimensions[material->texture_id - 1]->width;
        pixelOffsetY = 0.5f / (f32)NuTextureDimensions[material->texture_id - 1]->height;
    }

    if (first) {
        f32 half_fov;
        camFov = camera.fov;
        half_fov = camera.fov * 0.5f;
        adjacent = 1.0f / (NuTrigTable[(i32)(half_fov * 10430.378f) >> 1 & 0x7fff] /
                           NuTrigTable[(16384 - (i32)(half_fov * -10430.378f)) >> 1 & 0x7fff]);
        first = 0;
    }
    if (camFov != camera.fov) {
        f32 half_fov = camera.fov * 0.5f;
        adjacent = 1.0f / (NuTrigTable[(i32)(half_fov * 10430.378f) >> 1 & 0x7fff] /
                           NuTrigTable[(16384 - (i32)(half_fov * -10430.378f)) >> 1 & 0x7fff]);
        camera.fov = camFov;
    }
    camera.mtx.m30 = 0.0f;
    camera.mtx.m31 = 0.0f;
    camera.mtx.m32 = 0.0f;
    camera.mtx.m33 = 1.0f;

    ++NuPrimCSPos;
    NuPrimSetCoordinateSystem(1);
    for (row = 0; row < rows - 1; ++row) {
        const i32 strip_row = row > 0;
        NuPrim2DBegin(13, 7, material);
        for (col = 0; col < cols; ++col) {
            NUVEC4 direction;
            u32 colour;
            i32 alpha;
            f32 brightness;
            const f32 direction_y = ((1.0f + y0) - 1.0f) * -camera.aspect;
            direction.x = (1.0f + x0) - 1.0f;
            direction.y = direction_y;
            direction.z = adjacent;
            NuVecNorm(&direction.xyz, &direction.xyz);
            direction.w = 1.0f;
            NuVecMtxTransform(&direction.xyz, &direction.xyz, &camera.mtx);
            NuVecNorm(&direction.xyz, &direction.xyz);
            direction.w = 1.0f;
            brightness = direction.y * 0.5 + 0.5;
            if (strip_row) {
                colour = ((u32)cacheValues[col] << 24) | 0x808080;
            } else {
                if (brightness <= near_angle) {
                    brightness = near_scale * parameters->intensity;
                } else if (brightness >= far_angle) {
                    brightness = far_scale * parameters->intensity;
                } else {
                    brightness = ((brightness - near_angle) * scale_delta / angle_delta + near_scale) * parameters->intensity;
                }
                if (parameters->directional) {
                    {
                        f32 dot = NuVecDot(&parameters->direction, &direction.xyz);
                        f32 angle = (i16)(0x4000 - RenderPostASin(dot)) * 0.0054931640625;
                        if (angle < parameters->direction_near_angle) {
                            brightness += parameters->direction_far_scale * 128.0;
                        } else if (!(angle > parameters->direction_far_angle)) {
                            brightness +=
                                ((1.0f - (parameters->direction_bias == 1.0f
                                              ? (angle - parameters->direction_near_angle) /
                                                    (parameters->direction_far_angle - parameters->direction_near_angle)
                                              : NuPowFast((angle - parameters->direction_near_angle) /
                                                              (parameters->direction_far_angle - parameters->direction_near_angle),
                                                          parameters->direction_bias))) *
                                     (parameters->direction_far_scale - parameters->direction_near_scale) +
                                 parameters->direction_near_scale) *
                                128.0;
                        }
                    }
                }
                alpha = NuPrim2DAddCLR(0, 255, brightness);
                colour = ((u32)alpha << 24) | 0x808080;
            }
            NuRndrPrimSetColourPrivate(colour);
            NuRndrPrimUV((f32)row * inv_row + pixelOffsetX, ((f32)col * inv_col + pixelOffsetY) * 0.9f);
            NuPrim2DAddXYZ(x0, y0, 0.0f);

            direction.x = (1.0f + x1) - 1.0f;
            direction.y = direction_y;
            direction.z = adjacent;
            NuVecNorm(&direction.xyz, &direction.xyz);
            direction.w = 1.0f;
            NuVecMtxTransform(&direction.xyz, &direction.xyz, &camera.mtx);
            NuVecNorm(&direction.xyz, &direction.xyz);
            direction.w = 1.0f;
            brightness = direction.y * 0.5 + 0.5;
            if (brightness <= near_angle) {
                brightness = near_scale * parameters->intensity;
            } else if (brightness >= far_angle) {
                brightness = far_scale * parameters->intensity;
            } else {
                brightness = ((brightness - near_angle) * scale_delta / angle_delta + near_scale) * parameters->intensity;
            }
            if (parameters->directional) {
                {
                    f32 dot = NuVecDot(&parameters->direction, &direction.xyz);
                    f32 angle = (i16)(0x4000 - RenderPostASin(dot)) * 0.0054931640625;
                    if (angle < parameters->direction_near_angle) {
                        brightness += parameters->direction_far_scale * 128.0;
                    } else if (!(angle > parameters->direction_far_angle)) {
                        brightness += ((1.0f - (parameters->direction_bias == 1.0f
                                                    ? (angle - parameters->direction_near_angle) /
                                                          (parameters->direction_far_angle - parameters->direction_near_angle)
                                                    : NuPowFast((angle - parameters->direction_near_angle) /
                                                                    (parameters->direction_far_angle - parameters->direction_near_angle),
                                                                parameters->direction_bias))) *
                                           (parameters->direction_far_scale - parameters->direction_near_scale) +
                                       parameters->direction_near_scale) *
                                      128.0;
                    }
                }
            }
            cacheValues[col] = (u8)MIN(255.0f, MAX(0.0f, brightness));
            NuRndrPrimSetColourPrivate(((u32)cacheValues[col] << 24) | 0x808080);
            NuRndrPrimUV((f32)(row + 1) * inv_row + pixelOffsetX, ((f32)col * inv_col + pixelOffsetY) * 0.9f);
            NuPrim2DAddXYZ(x1, y0, 0.0f);
            y0 += step_y;
        }
        NuPrim2DEnd();
        y0 = y_start;
        x0 += step_x;
        x1 += step_x;
    }
    {
        i32 restore_mode = NuPrimCoordSystemStack[NuPrimCSPos - 1];
        --NuPrimCSPos;
        NuPrimSetCoordinateSystem(restore_mode);
    }
    RenderPostViewportPrefix = saved_prefix;
    PCSpecialProjectionAdjust = saved_adjust;
}

u8 NuPrim2DAddCLR(i32 minimum_unused, i32 maximum_unused, f32 value) {
    return (u8)MIN(255.0f, MAX(0.0f, value));
}

PC_TEXTURE_RESOURCE *RendererVertexDeclaration = 0;
PC_BUFFER_RESOURCE *RendererQuadVertexBuffer = 0;
PC_HG_SHADOW_STATE PCShadowState = {0};

/* Stencil shadow-volume arena vertex: D3DDECLTYPE_FLOAT3 position and D3DCOLOR. */
typedef struct RENDERER_ARENA_VERTEX { // size 0x10
    NUVEC position;                    // 0x0
    u32 colour;                        // 0xc
} RENDERER_ARENA_VERTEX;

RENDERER_ARENA_VERTEX *RendererArenaData = 0;
i32 *NuPortalFrustumCount = 0;
__declspec(align(16)) CAMERA_CLIP_PLANES CameraClipPlanes = {0};
u32 *RendererArenaOffsets = 0;
VARIPTR fstack = {0};
i32 *RendererArenaTypes = 0;
NUTEXDIM **NuTextureDimensions = 0;
i32 RendererArenaCount = 0;
i32 RendererArenaVertexCount = 0;
i32 RendererArenaIndexCount = 0;
__declspec(align(16)) NUPLANE VisibilityDepthPlane = {0};
char *PCDisplayCaptureFlushLimit = NULL;

/* Shadow outline records: a direction or point with a count (or a -1 marker in the arena) in the fourth word. */
typedef struct pc_shadow_edge_s { // size 0x10
    NUVEC v;                      // 0x0
    i32 count;                    // 0xc
} PC_SHADOW_EDGE;

void NuRndrArenaInit(VARIPTR *buffer, VARIPTR end) {
    RendererArenaData = (RENDERER_ARENA_VERTEX *)((buffer->addr + 3) & ~3u);
    buffer->addr = ((buffer->addr + 3) & ~3u) + 0x36b00;
    RendererArenaOffsets = (u32 *)((buffer->addr + 3) & ~3u);
    buffer->addr = ((buffer->addr + 3) & ~3u) + 0x7d0;
    RendererArenaTypes = (i32 *)((buffer->addr + 3) & ~3u);
    buffer->addr = ((buffer->addr + 3) & ~3u) + 0x7d0;
}

void NuRndrAnglesBetweenPoints(NUVEC *direction, f32 *pitch, f32 *yaw) {
    *yaw = (f32)NuAtan2D(direction->x, direction->z);
    *pitch = (f32)NuAtan2D(-direction->y, NuFsqrt(direction->x * direction->x + direction->z * direction->z));
}

NUVEC NuRndrShadowDirections[8] = {0};

void NuRndrRotateVec(NUVEC *vector, f32 pitch, f32 yaw) {
    f32 sin_pitch = NuTrigTable[((i32)pitch >> 1) & 0x7fff];
    f32 cos_pitch = NuTrigTable[((i32)(pitch + 16384.0) >> 1) & 0x7fff];
    f32 sin_yaw = NuTrigTable[((i32)yaw >> 1) & 0x7fff];
    f32 cos_yaw = NuTrigTable[((i32)(yaw + 16384.0) >> 1) & 0x7fff];
    f32 z = vector->y * sin_pitch + cos_pitch * vector->z;
    vector->y = cos_pitch * vector->y - sin_pitch * vector->z;
    vector->z = z * cos_yaw - vector->x * sin_yaw;
    vector->x = cos_yaw * vector->x + z * sin_yaw;
}

void NuRndrShadowDirectionsSetup(NUVEC *direction) {
    NUVEC angles; /* x: pitch, y: yaw */
    f32 length;
    i32 i;
    NuRndrAnglesBetweenPoints(direction, &angles.x, &angles.y);
    if (angles.x < 8192.0f) {
        angles.x = 8192.0f;
    }
    length = NuFsqrt(direction->x * direction->x + direction->y * direction->y + direction->z * direction->z);
    direction->x = 0.0f;
    direction->y = 0.0f;
    direction->z = length;
    NuRndrRotateVec(direction, angles.x, angles.y);
    for (i = 0; i < 8; i++) {
        i32 angle = i * 0x10000 / 8;
        NuRndrShadowDirections[i].x = NU_COS_LUT(angle);
        NuRndrShadowDirections[i].y = -NU_SIN_LUT(angle);
        NuRndrShadowDirections[i].z = 0.0f;
        NuRndrRotateVec(&NuRndrShadowDirections[i], angles.x, angles.y);
    }
}

/* Shadow cylinder records: a centre, the axis and two side vectors in object space. Bit 0 of the first word selects a
 * rounded (capsule) outline instead of a box. */
typedef struct PC_SHADOW_CYLINDER { // size 0x40

    union { // size 0xc
        NUVEC centre;
        u32 flags;
    }; // 0x00

    NUVEC axis;        // 0x0c
    NUVEC side;        // 0x18
    f32 unknown_24;    // 0x24
    NUVEC up;          // 0x28
    f32 unknown_34[3]; // 0x34
} PC_SHADOW_CYLINDER;

/* Extrudes the silhouette of a queued cylinder along the light offset into arena quads: ten edges of a capsule outline
 * when bit 0 of the record is set, otherwise the four edges of a box. */
void NuRndrArenaAddCylinderVolume(PC_SHADOW_CYLINDER *cylinder, PC_HG_SHADOW_PACKET *packet, NUVEC *offset) {
    NUVEC axis[3];
    NUVEC4 v[32];
    NUVEC4 centre;
    NUVEC length;
    NUVEC perp;
    f32 scale;
    f32 inv_length;
    f32 dot2;
    i32 i;

    if (cylinder->flags & 1) {
        if ((u32)RendererArenaCount < 250) {
            RendererArenaOffsets[RendererArenaCount] = RendererArenaVertexCount;
            RendererArenaTypes[RendererArenaCount] = 10;
            RendererArenaCount++;
        }
        v[4].x = cylinder->centre.x;
        v[4].y = cylinder->centre.y;
        v[4].z = cylinder->centre.z;
        v[4].w = 1.0f;
        NuVec4MtxTransformVU0(&centre, &v[4], &packet->matrix);
        v[0].x = cylinder->axis.x + cylinder->centre.x;
        v[0].y = cylinder->axis.y + cylinder->centre.y;
        v[0].z = cylinder->axis.z + cylinder->centre.z;
        v[0].w = 1.0f;
        NuVec4MtxTransformVU0(&v[0], &v[0], &packet->matrix);
        axis[0].x = v[0].x - centre.x;
        axis[0].y = v[0].y - centre.y;
        axis[0].z = v[0].z - centre.z;
        v[1].x = cylinder->side.x + cylinder->centre.x;
        v[1].y = cylinder->side.y + cylinder->centre.y;
        v[1].z = cylinder->side.z + cylinder->centre.z;
        v[1].w = 1.0f;
        NuVec4MtxTransformVU0(&v[1], &v[1], &packet->matrix);
        axis[1].x = v[1].x - centre.x;
        axis[1].y = v[1].y - centre.y;
        axis[1].z = v[1].z - centre.z;
        v[2].x = cylinder->up.x + cylinder->centre.x;
        v[2].y = cylinder->up.y + cylinder->centre.y;
        v[2].z = cylinder->up.z + cylinder->centre.z;
        v[2].w = 1.0f;
        NuVec4MtxTransformVU0(&v[2], &v[2], &packet->matrix);
        axis[2].x = v[2].x - centre.x;
        axis[2].y = v[2].y - centre.y;
        axis[2].z = v[2].z - centre.z;
        length.x = axis[0].x;
        length.y = axis[0].y;
        length.z = axis[0].z;
        NuVecCross(&perp, &axis[0], offset);
        scale = perp.x * perp.x + perp.y * perp.y + perp.z * perp.z;
        if (scale > 0.0f) {
            scale = NuFdiv(1.0f, NuFsqrt(scale));
        }
        perp.x *= scale;
        perp.y *= scale;
        perp.z *= scale;
        inv_length = length.x * length.x + length.y * length.y + length.z * length.z;
        if (inv_length > 0.0f) {
            inv_length = NuFdiv(1.0f, NuFsqrt(inv_length));
        } else {
            inv_length = 0.0f;
        }
        scale = perp.x * axis[1].x + perp.y * axis[1].y + perp.z * axis[1].z;
        dot2 = perp.x * axis[2].x + perp.y * axis[2].y + perp.z * axis[2].z;
        dot2 = dot2 * dot2 + scale * scale;
        scale = NuFsqrt(dot2);
        for (i = 0; i < 10; i++) {
            RendererArenaIndexCount += 2;
            switch (i) {
                case 0:
                    v[0].x = perp.x * scale + length.x;
                    v[0].y = perp.y * scale + length.y;
                    v[0].z = perp.z * scale + length.z;
                    v[1].x = perp.x * scale - length.x;
                    v[1].y = perp.y * scale - length.y;
                    v[1].z = perp.z * scale - length.z;
                    break;
                case 1:
                    v[0].x = perp.x * scale - length.x;
                    v[0].y = perp.y * scale - length.y;
                    v[0].z = perp.z * scale - length.z;
                    v[1].x = perp.x * scale * 0.707f - length.x - inv_length * length.x * scale * 0.707f;
                    v[1].y = perp.y * scale * 0.707f - length.y - inv_length * length.y * scale * 0.707f;
                    v[1].z = perp.z * scale * 0.707f - length.z - inv_length * length.z * scale * 0.707f;
                    break;
                case 2:
                    v[0].x = perp.x * scale * 0.707f - length.x - inv_length * length.x * scale * 0.707f;
                    v[0].y = perp.y * scale * 0.707f - length.y - inv_length * length.y * scale * 0.707f;
                    v[0].z = perp.z * scale * 0.707f - length.z - inv_length * length.z * scale * 0.707f;
                    v[1].x = -length.x - scale * length.x * inv_length;
                    v[1].y = -length.y - scale * length.y * inv_length;
                    v[1].z = -length.z - scale * length.z * inv_length;
                    break;
                case 3:
                    v[0].x = -length.x - scale * length.x * inv_length;
                    v[0].y = -length.y - scale * length.y * inv_length;
                    v[0].z = -length.z - scale * length.z * inv_length;
                    v[1].x = -length.x - perp.x * scale * 0.707f - inv_length * length.x * scale * 0.707f;
                    v[1].y = -length.y - perp.y * scale * 0.707f - inv_length * length.y * scale * 0.707f;
                    v[1].z = -length.z - perp.z * scale * 0.707f - inv_length * length.z * scale * 0.707f;
                    break;
                case 4:
                    v[0].x = -length.x - perp.x * scale * 0.707f - inv_length * length.x * scale * 0.707f;
                    v[0].y = -length.y - perp.y * scale * 0.707f - inv_length * length.y * scale * 0.707f;
                    v[0].z = -length.z - perp.z * scale * 0.707f - inv_length * length.z * scale * 0.707f;
                    v[1].x = -length.x - perp.x * scale;
                    v[1].y = -length.y - perp.y * scale;
                    v[1].z = -length.z - perp.z * scale;
                    break;
                case 5:
                    v[0].x = -length.x - perp.x * scale;
                    v[0].y = -length.y - perp.y * scale;
                    v[0].z = -length.z - perp.z * scale;
                    v[1].x = length.x - perp.x * scale;
                    v[1].y = length.y - perp.y * scale;
                    v[1].z = length.z - perp.z * scale;
                    break;
                case 6:
                    v[0].x = length.x - perp.x * scale;
                    v[0].y = length.y - perp.y * scale;
                    v[0].z = length.z - perp.z * scale;
                    v[1].x = length.x - perp.x * scale * 0.707f + scale * length.x * inv_length * 0.707f;
                    v[1].y = length.y - perp.y * scale * 0.707f + scale * length.y * inv_length * 0.707f;
                    v[1].z = length.z - perp.z * scale * 0.707f + scale * length.z * inv_length * 0.707f;
                    break;
                case 7:
                    v[0].x = length.x - perp.x * scale * 0.707f + scale * length.x * inv_length * 0.707f;
                    v[0].y = length.y - perp.y * scale * 0.707f + scale * length.y * inv_length * 0.707f;
                    v[0].z = length.z - perp.z * scale * 0.707f + scale * length.z * inv_length * 0.707f;
                    v[1].x = length.x + scale * length.x * inv_length;
                    v[1].y = length.y + scale * length.y * inv_length;
                    v[1].z = length.z + scale * length.z * inv_length;
                    break;
                case 8:
                    v[0].x = length.x + scale * length.x * inv_length;
                    v[0].y = length.y + scale * length.y * inv_length;
                    v[0].z = length.z + scale * length.z * inv_length;
                    v[1].x = perp.x * scale * 0.707f + length.x + scale * length.x * inv_length * 0.707f;
                    v[1].y = perp.y * scale * 0.707f + length.y + scale * length.y * inv_length * 0.707f;
                    v[1].z = perp.z * scale * 0.707f + length.z + scale * length.z * inv_length * 0.707f;
                    break;
                case 9:
                    v[0].x = perp.x * scale * 0.707f + length.x + scale * length.x * inv_length * 0.707f;
                    v[0].y = perp.y * scale * 0.707f + length.y + scale * length.y * inv_length * 0.707f;
                    v[0].z = perp.z * scale * 0.707f + length.z + scale * length.z * inv_length * 0.707f;
                    v[1].x = length.x + perp.x * scale;
                    v[1].y = length.y + perp.y * scale;
                    v[1].z = length.z + perp.z * scale;
                    break;
            }
            v[0].x += centre.x;
            v[0].y += centre.y;
            v[0].z += centre.z;
            v[1].x += centre.x;
            v[1].y += centre.y;
            v[1].z += centre.z;
            v[2].x = v[0].x + offset->x * 5.0;
            v[2].y = v[0].y + offset->y * 5.0;
            v[2].z = v[0].z + offset->z * 5.0;
            v[3].x = v[1].x + offset->x * 5.0;
            v[3].y = v[1].y + offset->y * 5.0;
            v[3].z = v[1].z + offset->z * 5.0;
            RendererArenaData[RendererArenaVertexCount].position.x = v[0].x;
            RendererArenaData[RendererArenaVertexCount].position.y = v[0].y;
            RendererArenaData[RendererArenaVertexCount].position.z = v[0].z;
            RendererArenaData[RendererArenaVertexCount + 1].position.x = v[2].x;
            RendererArenaData[RendererArenaVertexCount + 1].position.y = v[2].y;
            RendererArenaData[RendererArenaVertexCount + 1].position.z = v[2].z;
            RendererArenaData[RendererArenaVertexCount + 2].position.x = v[3].x;
            RendererArenaData[RendererArenaVertexCount + 2].position.y = v[3].y;
            RendererArenaData[RendererArenaVertexCount + 2].position.z = v[3].z;
            RendererArenaData[RendererArenaVertexCount + 3].position.x = v[1].x;
            RendererArenaData[RendererArenaVertexCount + 3].position.y = v[1].y;
            RendererArenaData[RendererArenaVertexCount + 3].position.z = v[1].z;
            RendererArenaData[RendererArenaVertexCount].colour = 0xffffffff;
            RendererArenaData[RendererArenaVertexCount + 1].colour = 0xffffffff;
            RendererArenaData[RendererArenaVertexCount + 2].colour = 0xffffffff;
            RendererArenaData[RendererArenaVertexCount + 3].colour = 0xffffffff;
            RendererArenaVertexCount += 4;
        }
    } else {
        if ((u32)RendererArenaCount < 250) {
            RendererArenaOffsets[RendererArenaCount] = RendererArenaVertexCount;
            RendererArenaTypes[RendererArenaCount] = 4;
            RendererArenaCount++;
        }
        v[4].x = cylinder->centre.x;
        v[4].y = cylinder->centre.y;
        v[4].z = cylinder->centre.z;
        v[4].w = 1.0f;
        NuVec4MtxTransformVU0(&centre, &v[4], &packet->matrix);
        v[0].x = cylinder->axis.x + cylinder->centre.x;
        v[0].y = cylinder->axis.y + cylinder->centre.y;
        v[0].z = cylinder->axis.z + cylinder->centre.z;
        v[0].w = 1.0f;
        NuVec4MtxTransformVU0(&v[0], &v[0], &packet->matrix);
        axis[0].x = v[0].x - centre.x;
        axis[0].y = v[0].y - centre.y;
        axis[0].z = v[0].z - centre.z;
        v[1].x = cylinder->side.x + cylinder->centre.x;
        v[1].y = cylinder->side.y + cylinder->centre.y;
        v[1].z = cylinder->side.z + cylinder->centre.z;
        v[1].w = 1.0f;
        NuVec4MtxTransformVU0(&v[1], &v[1], &packet->matrix);
        axis[1].x = v[1].x - centre.x;
        axis[1].y = v[1].y - centre.y;
        axis[1].z = v[1].z - centre.z;
        v[2].x = cylinder->up.x + cylinder->centre.x;
        v[2].y = cylinder->up.y + cylinder->centre.y;
        v[2].z = cylinder->up.z + cylinder->centre.z;
        v[2].w = 1.0f;
        NuVec4MtxTransformVU0(&v[2], &v[2], &packet->matrix);
        axis[2].x = v[2].x - centre.x;
        axis[2].y = v[2].y - centre.y;
        axis[2].z = v[2].z - centre.z;
        length.x = axis[0].x;
        length.y = axis[0].y;
        length.z = axis[0].z;
        NuVecCross(&perp, &axis[0], offset);
        scale = perp.x * perp.x + perp.y * perp.y + perp.z * perp.z;
        if (scale > 0.0f) {
            scale = NuFdiv(1.0f, NuFsqrt(scale));
        }
        perp.x *= scale;
        perp.y *= scale;
        perp.z *= scale;
        inv_length = length.x * length.x + length.y * length.y + length.z * length.z;
        if (inv_length > 0.0f) {
            inv_length = NuFdiv(1.0f, NuFsqrt(inv_length));
        } else {
            inv_length = 0.0f;
        }
        scale = perp.x * axis[1].x + perp.y * axis[1].y + perp.z * axis[1].z;
        dot2 = perp.x * axis[2].x + perp.y * axis[2].y + perp.z * axis[2].z;
        dot2 = dot2 * dot2 + scale * scale;
        scale = NuFsqrt(dot2);
        perp.x *= scale;
        perp.y *= scale;
        perp.z *= scale;
        for (i = 0; i < 4; i++) {
            switch (i) {
                case 0:
                    v[0].x = perp.x + length.x;
                    v[0].y = perp.y + length.y;
                    v[0].z = perp.z + length.z;
                    v[1].x = perp.x - length.x;
                    v[1].y = perp.y - length.y;
                    v[1].z = perp.z - length.z;
                    break;
                case 1:
                    v[0].x = perp.x - length.x;
                    v[0].y = perp.y - length.y;
                    v[0].z = perp.z - length.z;
                    v[1].x = -length.x - perp.x;
                    v[1].y = -length.y - perp.y;
                    v[1].z = -length.z - perp.z;
                    break;
                case 2:
                    v[0].x = -length.x - perp.x;
                    v[0].y = -length.y - perp.y;
                    v[0].z = -length.z - perp.z;
                    v[1].x = length.x - perp.x;
                    v[1].y = length.y - perp.y;
                    v[1].z = length.z - perp.z;
                    break;
                case 3:
                    v[0].x = length.x - perp.x;
                    v[0].y = length.y - perp.y;
                    v[0].z = length.z - perp.z;
                    v[1].x = perp.x + length.x;
                    v[1].y = perp.y + length.y;
                    v[1].z = perp.z + length.z;
                    break;
            }
            v[0].x += centre.x;
            v[0].y += centre.y;
            v[0].z += centre.z;
            v[1].x += centre.x;
            v[1].y += centre.y;
            v[1].z += centre.z;
            v[2].x = v[0].x + offset->x * 5.0;
            v[2].y = v[0].y + offset->y * 5.0;
            v[2].z = v[0].z + offset->z * 5.0;
            v[3].x = v[1].x + offset->x * 5.0;
            v[3].y = v[1].y + offset->y * 5.0;
            v[3].z = v[1].z + offset->z * 5.0;
            RendererArenaData[RendererArenaVertexCount].position.x = v[0].x;
            RendererArenaData[RendererArenaVertexCount].position.y = v[0].y;
            RendererArenaData[RendererArenaVertexCount].position.z = v[0].z;
            RendererArenaData[RendererArenaVertexCount + 1].position.x = v[2].x;
            RendererArenaData[RendererArenaVertexCount + 1].position.y = v[2].y;
            RendererArenaData[RendererArenaVertexCount + 1].position.z = v[2].z;
            RendererArenaData[RendererArenaVertexCount + 2].position.x = v[3].x;
            RendererArenaData[RendererArenaVertexCount + 2].position.y = v[3].y;
            RendererArenaData[RendererArenaVertexCount + 2].position.z = v[3].z;
            RendererArenaData[RendererArenaVertexCount + 3].position.x = v[1].x;
            RendererArenaData[RendererArenaVertexCount + 3].position.y = v[1].y;
            RendererArenaData[RendererArenaVertexCount + 3].position.z = v[1].z;
            RendererArenaData[RendererArenaVertexCount].colour = 0xffffffff;
            RendererArenaData[RendererArenaVertexCount + 1].colour = 0xffffffff;
            RendererArenaData[RendererArenaVertexCount + 2].colour = 0xffffffff;
            RendererArenaData[RendererArenaVertexCount + 3].colour = 0xffffffff;
            RendererArenaVertexCount += 4;
        }
    }
}

/* Extrudes the eight-direction silhouette of a queued ellipsoid (centre and three axes) along the light offset into
 * arena quads. */
void NuRndrArenaAddEllipsoidVolume(NUVEC *axes, PC_HG_SHADOW_PACKET *packet, NUVEC *offset, f32 near_value, f32 far_value) {
    NUVEC4 v[32];
    NUVEC4 centre;
    NUVEC axis[3];
    NUVEC local;
    f32 scale[3];
    i32 i;

    v[4].x = axes[0].x;
    v[4].y = axes[0].y;
    v[4].z = axes[0].z;
    v[4].w = 1.0f;
    NuVec4MtxTransformVU0(&centre, &v[4], &packet->matrix);
    v[0].x = axes[2].x + axes[0].x;
    v[0].y = axes[2].y + axes[0].y;
    v[0].z = axes[2].z + axes[0].z;
    v[0].w = 1.0f;
    NuVec4MtxTransformVU0(&v[0], &v[0], &packet->matrix);
    axis[0].x = v[0].x - centre.x;
    axis[0].y = v[0].y - centre.y;
    axis[0].z = v[0].z - centre.z;
    v[1].x = axes[1].x + axes[0].x;
    v[1].y = axes[1].y + axes[0].y;
    v[1].z = axes[1].z + axes[0].z;
    v[1].w = 1.0f;
    NuVec4MtxTransformVU0(&v[1], &v[1], &packet->matrix);
    axis[1].x = v[1].x - centre.x;
    axis[1].y = v[1].y - centre.y;
    axis[1].z = v[1].z - centre.z;
    v[2].x = axes[3].x + axes[0].x;
    v[2].y = axes[3].y + axes[0].y;
    v[2].z = axes[3].z + axes[0].z;
    v[2].w = 1.0f;
    NuVec4MtxTransformVU0(&v[2], &v[2], &packet->matrix);
    axis[2].x = v[2].x - centre.x;
    axis[2].y = v[2].y - centre.y;
    axis[2].z = v[2].z - centre.z;
    scale[0] = NuFdiv(1.0f, NuFsqrt((axis[0].x * axis[0].x) + (axis[0].y * axis[0].y) + (axis[0].z * axis[0].z)));
    scale[1] = NuFdiv(1.0f, NuFsqrt(axis[1].x * axis[1].x + axis[1].y * axis[1].y + axis[1].z * axis[1].z));
    scale[2] = NuFdiv(1.0f, NuFsqrt(axis[2].x * axis[2].x + axis[2].y * axis[2].y + axis[2].z * axis[2].z));
    if ((u32)RendererArenaCount < 250) {
        RendererArenaOffsets[RendererArenaCount] = RendererArenaVertexCount;
        RendererArenaTypes[RendererArenaCount] = 8;
        RendererArenaCount++;
    }
    for (i = 0; i < 8; i++) {
        RendererArenaIndexCount += 2;
        local.x =
            (NuRndrShadowDirections[i].x * axis[0].x + NuRndrShadowDirections[i].y * axis[0].y + NuRndrShadowDirections[i].z * axis[0].z) *
            scale[0];
        local.y =
            (NuRndrShadowDirections[i].x * axis[1].x + NuRndrShadowDirections[i].y * axis[1].y + NuRndrShadowDirections[i].z * axis[1].z) *
            scale[1];
        local.z =
            (NuRndrShadowDirections[i].x * axis[2].x + NuRndrShadowDirections[i].y * axis[2].y + NuRndrShadowDirections[i].z * axis[2].z) *
            scale[2];
        v[0].x = axis[0].x * local.x + axis[1].x * local.y + axis[2].x * local.z;
        v[0].y = axis[0].y * local.x + axis[1].y * local.y + axis[2].y * local.z;
        v[0].z = axis[0].z * local.x + axis[1].z * local.y + axis[2].z * local.z;
        local.x = (NuRndrShadowDirections[(i + 1) & 7].x * axis[0].x + NuRndrShadowDirections[(i + 1) & 7].y * axis[0].y +
                   NuRndrShadowDirections[(i + 1) & 7].z * axis[0].z) *
                  scale[0];
        local.y = (NuRndrShadowDirections[(i + 1) & 7].x * axis[1].x + NuRndrShadowDirections[(i + 1) & 7].y * axis[1].y +
                   NuRndrShadowDirections[(i + 1) & 7].z * axis[1].z) *
                  scale[1];
        local.z = (NuRndrShadowDirections[(i + 1) & 7].x * axis[2].x + NuRndrShadowDirections[(i + 1) & 7].y * axis[2].y +
                   NuRndrShadowDirections[(i + 1) & 7].z * axis[2].z) *
                  scale[2];
        v[1].x = axis[0].x * local.x + axis[1].x * local.y + axis[2].x * local.z;
        v[1].y = axis[0].y * local.x + axis[1].y * local.y + axis[2].y * local.z;
        v[1].z = axis[0].z * local.x + axis[1].z * local.y + axis[2].z * local.z;
        v[0].x += centre.x;
        v[0].y += centre.y;
        v[0].z += centre.z;
        v[1].x += centre.x;
        v[1].y += centre.y;
        v[1].z += centre.z;
        v[2].x = v[0].x + offset->x * 5.0;
        v[2].y = v[0].y + offset->y * 5.0;
        v[2].z = v[0].z + offset->z * 5.0;
        v[3].x = v[1].x + offset->x * 5.0;
        v[3].y = v[1].y + offset->y * 5.0;
        v[3].z = v[1].z + offset->z * 5.0;
        RendererArenaData[RendererArenaVertexCount].colour = PCShadowState.packed_colour;
        RendererArenaData[RendererArenaVertexCount].position.x = v[0].x;
        RendererArenaData[RendererArenaVertexCount].position.y = v[0].y;
        RendererArenaData[RendererArenaVertexCount].position.z = v[0].z;
        RendererArenaData[RendererArenaVertexCount + 1].colour = PCShadowState.packed_colour;
        RendererArenaData[RendererArenaVertexCount + 1].position.x = v[2].x;
        RendererArenaData[RendererArenaVertexCount + 1].position.y = v[2].y;
        RendererArenaData[RendererArenaVertexCount + 1].position.z = v[2].z;
        RendererArenaData[RendererArenaVertexCount + 2].colour = PCShadowState.packed_colour;
        RendererArenaData[RendererArenaVertexCount + 2].position.x = v[3].x;
        RendererArenaData[RendererArenaVertexCount + 2].position.y = v[3].y;
        RendererArenaData[RendererArenaVertexCount + 2].position.z = v[3].z;
        RendererArenaData[RendererArenaVertexCount + 3].colour = PCShadowState.packed_colour;
        RendererArenaData[RendererArenaVertexCount + 3].position.x = v[1].x;
        RendererArenaData[RendererArenaVertexCount + 3].position.y = v[1].y;
        RendererArenaData[RendererArenaVertexCount + 3].position.z = v[1].z;
        RendererArenaVertexCount += 4;
    }
}

void NuRndrArenaAddEdgeVolumes(PC_HG_SHADOW_PACKET *packet, PC_SHADOW_EDGE *groups, PC_SHADOW_EDGE *points, NUVEC *offset) {
    f32 buffer[256];
    NUMTX *matrix = (NUMTX *)(((u32)buffer + 15) & ~15u);
    NUVEC4 *v;
    i32 group_count;
    *matrix = packet->matrix;
    v = matrix[1].rows;
    for (group_count = points->count; group_count > 0; group_count--) {
        v[0].xyz = groups->v;
        v[0].w = 1.0f;
        NuVec4MtxRotateVU0(&v[0], &v[0], matrix);
        if (!(v[0].x * offset->x + v[0].y * offset->y + v[0].z * offset->z > 0.0)) {
            PC_SHADOW_EDGE *point = points;
            i32 remaining;
            if ((u32)RendererArenaCount < 250) {
                RendererArenaOffsets[RendererArenaCount] = RendererArenaVertexCount;
                RendererArenaTypes[RendererArenaCount] = groups->count;
                RendererArenaCount++;
            }
            for (remaining = groups->count; remaining > 0; remaining--) {
                RendererArenaIndexCount += 6;
                v[0].xyz = point->v;
                v[0].w = 1.0f;
                NuVec4MtxTransformVU0(&v[0], &v[0], matrix);
                if (remaining == 1) {
                    v[1] = *(NUVEC4 *)points;
                } else {
                    v[1] = *(NUVEC4 *)&point[1];
                }
                v[1].w = 1.0f;
                NuVec4MtxTransformVU0(&v[1], &v[1], matrix);
                v[2].x = v[0].x + offset->x;
                v[2].y = v[0].y + offset->y;
                v[2].z = v[0].z + offset->z;
                v[2].w = 1.0f;
                v[3].x = v[1].x + offset->x;
                v[3].y = v[1].y + offset->y;
                v[3].z = v[1].z + offset->z;
                v[3].w = 1.0f;
                RendererArenaData[RendererArenaVertexCount].colour = 0xffffffff;
                RendererArenaData[RendererArenaVertexCount].position.x = v[0].x;
                RendererArenaData[RendererArenaVertexCount].position.y = v[0].y;
                RendererArenaData[RendererArenaVertexCount].position.z = v[0].z;
                RendererArenaData[RendererArenaVertexCount + 1].colour = 0xffffffff;
                RendererArenaData[RendererArenaVertexCount + 1].position.x = v[2].x;
                RendererArenaData[RendererArenaVertexCount + 1].position.y = v[2].y;
                RendererArenaData[RendererArenaVertexCount + 1].position.z = v[2].z;
                RendererArenaData[RendererArenaVertexCount + 2].colour = 0xffffffff;
                RendererArenaData[RendererArenaVertexCount + 2].position.x = v[3].x;
                RendererArenaData[RendererArenaVertexCount + 2].position.y = v[3].y;
                RendererArenaData[RendererArenaVertexCount + 2].position.z = v[3].z;
                RendererArenaData[RendererArenaVertexCount + 3].colour = 0xffffffff;
                RendererArenaData[RendererArenaVertexCount + 3].position.x = v[1].x;
                RendererArenaData[RendererArenaVertexCount + 3].position.y = v[1].y;
                RendererArenaData[RendererArenaVertexCount + 3].position.z = v[1].z;
                RendererArenaVertexCount += 4;
                point++;
            }
            v[8] = v[1];
        }
        points += groups->count;
        groups++;
    }
}

/* Third hierarchy shadow shape: an outline of edge groups and their points. */
typedef struct PC_SHADOW_EDGE_SET {
    PC_SHADOW_EDGE *groups;
    PC_SHADOW_EDGE *points;
} PC_SHADOW_EDGE_SET;

/* Builds the shadow-volume arena from the queued hierarchy shadow packets, one light setup per packet sequence. */
void NuRndrArenaBuildShadowVolumes(i32 unused0, i32 unused1) {
    PC_HG_SHADOW_PACKET *packet;
    NUVEC *direction;
    NUVEC offset;
    f32 near_value;
    f32 far_value;
    f32 distance;
    f32 low;
    f32 high;
    i32 sequence = -1;
    i32 visible = 0;
    i32 mode = 1;
    i32 i;
    i32 j;

    RendererArenaIndexCount = 0;
    for (i = 0; i < PCShadowPacketCount; i++) {
        packet = &PCShadowPackets[i];
        if (packet->sequence != sequence) {
            sequence = packet->sequence;
            low = 10000000.0f;
            high = -10000000.0f;
            near_value = packet->near_value;
            far_value = packet->far_value;
            distance = (packet->matrix.m30 - global_camera.mtx.m30) * (packet->matrix.m30 - global_camera.mtx.m30) +
                       (packet->matrix.m31 - global_camera.mtx.m31) * (packet->matrix.m31 - global_camera.mtx.m31) +
                       (packet->matrix.m32 - global_camera.mtx.m32) * (packet->matrix.m32 - global_camera.mtx.m32);
            if (distance >= far_value * far_value) {
                visible = 0;
                continue;
            }
            visible = 1;
            direction = (NUVEC *)&packet->state0;
            if (direction->x >= 65536.0f) {
                offset.x = 0.0f;
                offset.y = -1.0f;
                offset.z = 0.0f;
                NuRndrShadowDirectionsSetup(&offset);
                offset.y = -4.0f;
                if (mode != 5 && direction->z == 0.0f) {
                    mode = 5;
                }
                low = direction->y;
                high = direction->z;
            } else {
                offset.x = direction->x;
                offset.y = direction->y;
                offset.z = direction->z;
                if (mode != 1) {
                    mode = 1;
                }
                NuRndrShadowDirectionsSetup(&offset);
                offset.x *= 4.0;
                offset.y *= 4.0;
                offset.z *= 4.0;
            }
            if (distance > near_value * near_value) {
                distance = NuFsqrt(distance);
            }
        } else if (visible == 0) {
            continue;
        }
        if (packet->cylinder != NULL) {
            for (j = 0; j < packet->cylinder_count; j++) {
                NuRndrArenaAddCylinderVolume((PC_SHADOW_CYLINDER *)packet->cylinder + j, packet, &offset);
            }
        }
        if (packet->ellipse != NULL) {
            for (j = 0; j < packet->ellipse_count; j++) {
                NuRndrArenaAddEllipsoidVolume(((NUVEC(*)[4])packet->ellipse)[j], packet, &offset, low, high);
            }
        }
        if (packet->third_shape != NULL) {
            for (j = 0; j < packet->third_count; j++) {
                NuRndrArenaAddEdgeVolumes(packet, ((PC_SHADOW_EDGE_SET *)packet->third_shape)->groups,
                                          ((PC_SHADOW_EDGE_SET *)packet->third_shape)->points, &offset);
            }
        }
    }
}

/* PC 601a30. Minimum reader for this initialization cohort, not a NUMTL extent. */

void NuRndrLine2di(i32 x0, i32 y0, i32 x1, i32 y1, i32 colour, NUMTL *material) {
    i32 colour_local = colour; // MATCH-EXCEPTION: parameter copied to a local; VC8 register choice (U20)
    NULGTVERTEX *vertex;
    ++NuPrimCSPos;
    NuPrimSetCoordinateSystem(0);
    NuPrim2DBegin(14, 7, material);
    NuRndrPrimSetColourPrivate(colour_local);
    vertex = NuPrimStreamBufferPointer->vertex_ptr;
    vertex->u = 0.0f;
    vertex->v = 0.0f;
    NuPrim2DAddXYZ((f32)((f64)x0 * 0.0625), (f32)((f64)y0 * 0.0625), 0.0f);
    NuRndrPrimSetColourPrivate(colour_local);
    vertex = NuPrimStreamBufferPointer->vertex_ptr;
    vertex->u = 1.0f;
    vertex->v = 1.0f;
    NuPrim2DAddXYZ((f32)((f64)x1 * 0.0625), (f32)((f64)y1 * 0.0625), 0.0f);
    NuPrim2DEnd();
    --NuPrimCSPos;
    NuPrimSetCoordinateSystem(NuPrimCoordSystemStack[NuPrimCSPos]);
}

typedef struct NULINEVERTEX2D { // size 0x1c
    f32 x;                      // 0x00
    f32 y;                      // 0x04
    f32 unused[2];              // 0x08
    i32 colour;                 // 0x10
    f32 u;                      // 0x14
    f32 v;                      // 0x18
} NULINEVERTEX2D;

i32 NuRndrLine2d(NULINEVERTEX2D *vertices, NUMTL *material) {
    f32 u;
    f32 v;
    NuPrim2DBegin(14, 7, material);
    NuRndrPrimSetColourPrivate(vertices[0].colour);
    v = vertices[0].v;
    u = vertices[0].u;
    NuRndrPrimUV(u, v);
    NuPrim2DAddXYZ(vertices[0].x, vertices[0].y, 0.0f);
    NuRndrPrimSetColourPrivate(vertices[1].colour);
    v = vertices[1].v;
    u = vertices[1].u;
    NuRndrPrimUV(u, v);
    NuPrim2DAddXYZ(vertices[1].x, vertices[1].y, 0.0f);
    NuPrim2DEnd();
    return 1;
}

void NuRndrRect(f32 x, f32 y, f32 z, f32 width, f32 height, f32 u0, f32 v0, f32 u1, f32 v1, i32 colour, NUMTL *material) {
    NuPrim2DBegin(16, 7, material);
    NuRndrPrimUV(u0, v0);
    NuRndrPrimSetColourPrivate(colour);
    NuPrim2DAddXYZ(x, y, z);
    NuRndrPrimUV(u1, v1);
    NuRndrPrimSetColourPrivate(colour);
    NuPrim2DAddXYZ(x + width, y + height, z);
    NuPrim2DEnd();
}

void NuRndrRectUV2dNoScale(f32 x, f32 y, f32 z, f32 width, f32 height, f32 u0, f32 v0, f32 u1, f32 v1, i32 colour, NUMTL *material) {
    NuPrim2DBegin(16, 7, material);
    NuRndrPrimUV(u0, v0);
    NuRndrPrimSetColourPrivate(colour);
    NuPrim2DAddXYZ(x, y, 0.0f);
    NuRndrPrimUV(u1, v1);
    NuRndrPrimSetColourPrivate(colour);
    NuPrim2DAddXYZ(x + width, y + height, 0.0f);
    NuPrim2DEnd();
}

void NuRndrRect2di(i32 x, i32 y, i32 width, i32 height, i32 colour, NUMTL *material) {
    f32 sx = (f32)((f64)x * 0.0625);
    f32 sy = (f32)((f64)y * 0.0625);
    f32 sw;
    f32 sh;
    NULGTVERTEX *vertex;
    ++NuPrimCSPos;
    NuPrimSetCoordinateSystem(0);
    NuPrim2DBegin(16, 7, material);
    NuRndrPrimSetColourPrivate(colour);
    vertex = NuPrimStreamBufferPointer->vertex_ptr;
    vertex->u = 0.0f;
    vertex->v = 0.0f;
    NuPrim2DAddXYZ(sx, sy, 0.0f);
    NuRndrPrimSetColourPrivate(colour);
    vertex = NuPrimStreamBufferPointer->vertex_ptr;
    vertex->u = 1.0f;
    vertex->v = 1.0f;
    sw = (f32)((f64)width * 0.0625);
    sh = (f32)((f64)height * 0.0625);
    NuPrim2DAddXYZ(sx + sw, sy + sh, 0.0f);
    NuPrim2DEnd();
    --NuPrimCSPos;
    NuPrimSetCoordinateSystem(NuPrimCoordSystemStack[NuPrimCSPos]);
}

void NuRndrRectUV2di(i32 x, i32 y, i32 width, i32 height, f32 u0, f32 v0, f32 u1, f32 v1, i32 colour, NUMTL *material) {
    f32 sx = (f32)((f64)x * 0.0625);
    f32 sy = (f32)((f64)y * 0.0625);
    f32 sw;
    f32 sh;
    NULGTVERTEX *vertex;
    ++NuPrimCSPos;
    NuPrimSetCoordinateSystem(0);
    NuPrim2DBegin(16, 7, material);
    vertex = NuPrimStreamBufferPointer->vertex_ptr;
    vertex->u = u0;
    vertex->v = v0;
    NuRndrPrimSetColourPrivate(colour);
    NuPrim2DAddXYZ(sx, sy, 0.0f);
    vertex = NuPrimStreamBufferPointer->vertex_ptr;
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

void NuRndrLineRect2di(i32 x, i32 y, i32 width, i32 height, i32 colour, NUMTL *material) {
    struct {
        f32 x;
        f32 y;
        f32 width;
        f32 height;
    } rectangle = {(f32)((f64)x * 0.0625), (f32)((f64)y * 0.0625), (f32)((f64)width * 0.0625), (f32)((f64)height * 0.0625)};

    f32 ex;
    f32 ey;
    NULGTVERTEX *vertex;
    ++NuPrimCSPos;
    NuPrimSetCoordinateSystem(0);
    NuPrim2DBegin(15, 7, material);
    NuRndrPrimSetColourPrivate(colour);
    vertex = NuPrimStreamBufferPointer->vertex_ptr;
    vertex->u = 0.0f;
    vertex->v = 0.0f;
    NuPrim2DAddXYZ(rectangle.x, rectangle.y, 0.0f);
    NuRndrPrimSetColourPrivate(colour);
    vertex = NuPrimStreamBufferPointer->vertex_ptr;
    vertex->u = 1.0f;
    vertex->v = 0.0f;
    ex = rectangle.x + rectangle.width;
    NuPrim2DAddXYZ(ex, rectangle.y, 0.0f);
    NuRndrPrimSetColourPrivate(colour);
    vertex = NuPrimStreamBufferPointer->vertex_ptr;
    vertex->u = 1.0f;
    vertex->v = 1.0f;
    ey = rectangle.y + rectangle.height;
    NuPrim2DAddXYZ(ex, ey, 0.0f);
    NuRndrPrimSetColourPrivate(colour);
    vertex = NuPrimStreamBufferPointer->vertex_ptr;
    vertex->u = 0.0f;
    vertex->v = 1.0f;
    NuPrim2DAddXYZ(rectangle.x, ey, 0.0f);
    NuRndrPrimSetColourPrivate(colour);
    vertex = NuPrimStreamBufferPointer->vertex_ptr;
    vertex->u = 0.0f;
    vertex->v = 0.0f;
    NuPrim2DAddXYZ(rectangle.x, rectangle.y, 0.0f);
    NuPrim2DEnd();
    --NuPrimCSPos;
    NuPrimSetCoordinateSystem(NuPrimCoordSystemStack[NuPrimCSPos]);
}

f32 global_windscale = 1.0f;
f32 global_windspeed = 1.0f;
i32 dat_0080b6e4 = 13;

/* Native speed-blur state has distinct writable storage. */
i32 NuLightSpeedBlurEnabled = 1;
static NUMTX NuLightSpeedBlurViewProjection = {0};
static NUMTX NuLightSpeedBlurPreviousViewProjection = {0};

void NuLightSpeedBlur(i32 reuse_camera, f32 scale) {
    NU_POST_SPEED_PARAMETERS *parameters = &currentScene.speed_blur;
    f32 frame_time;
    if (NuLightSpeedBlurEnabled == 0) {
        return;
    }
    parameters->enabled = 1;
    if (reuse_camera == 0) {
        NuMtxMulH(&NuLightSpeedBlurPreviousViewProjection, &parameters->current, &pmtx);
        NuMtxMulH(&NuLightSpeedBlurViewProjection, &vmtx, &pmtx);
    }
    frame_time = nuapi.frame_time;
    parameters->previous = NuLightSpeedBlurViewProjection;
    parameters->current = NuLightSpeedBlurPreviousViewProjection;
    parameters->frame_time = frame_time;
    parameters->scale = scale;
}

void NuRndrLineStrip2di(i32 *positions, f32 *uvs, i32 count, i32 colour, NUMTL *material) {
    i32 i;
    ++NuPrimCSPos;
    NuPrimSetCoordinateSystem(0);
    NuPrim2DBegin(14, 7, material);
    if (count > 0) {
        i = 0;
        do {
            NuRndrPrimSetColourPrivate(colour);
            if (uvs != NULL) {
                f32 v = uvs[i * 2 + 1];
                NULGTVERTEX *vertex = NuPrimStreamBufferPointer->vertex_ptr;
                vertex->u = uvs[i * 2];
                vertex->v = v;
            }
            NuPrim2DAddXYZ((f32)((f64)positions[i * 2] * 0.0625), (f32)((f64)positions[i * 2 + 1] * 0.0625), 0.0f);
            ++i;
        } while (--count != 0);
    }
    NuPrim2DEnd();
    --NuPrimCSPos;
    NuPrimSetCoordinateSystem(NuPrimCoordSystemStack[NuPrimCSPos]);
}

void NuRndrTriStrip2di(i32 *positions, f32 *uvs, i32 count, i32 colour, NUMTL *material) {
    i32 i;
    ++NuPrimCSPos;
    NuPrimSetCoordinateSystem(0);
    NuPrim2DBegin(13, 7, material);
    if (count > 0) {
        i = 0;
        do {
            NuRndrPrimSetColourPrivate(colour);
            if (uvs != NULL) {
                f32 v = uvs[i * 2 + 1];
                NULGTVERTEX *vertex = NuPrimStreamBufferPointer->vertex_ptr;
                vertex->u = uvs[i * 2];
                vertex->v = v;
            }
            NuPrim2DAddXYZ((f32)((f64)positions[i * 2] * 0.0625), (f32)((f64)positions[i * 2 + 1] * 0.0625), 0.0f);
            ++i;
        } while (--count != 0);
    }
    NuPrim2DEnd();
    --NuPrimCSPos;
    NuPrimSetCoordinateSystem(NuPrimCoordSystemStack[NuPrimCSPos]);
}

void fun_005dc3e0(void) {
    AnimationDiagnosticNoOp();
}

void NuRndrRect2d(f32 x, f32 y, f32 z, f32 width, f32 height, i32 colour, NUMTL *material) {
    NULGTVERTEX *vertex;
    NuPrim2DBegin(16, 7, material);
    NuRndrPrimSetColourPrivate(colour);
    vertex = NuPrimStreamBufferPointer->vertex_ptr;
    vertex->u = 0.0f;
    vertex->v = 0.0f;
    NuPrim2DAddXYZ(x, y, z);
    NuRndrPrimSetColourPrivate(colour);
    vertex = NuPrimStreamBufferPointer->vertex_ptr;
    vertex->u = 1.0f;
    vertex->v = 1.0f;
    NuPrim2DAddXYZ(x + width, y + height, z);
    NuPrim2DEnd();
}
