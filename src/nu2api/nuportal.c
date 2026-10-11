#include "saga.h"
#include <math.h>
#include <string.h>

#define ALIGN_UP_16(address) (((address) + 15u) & ~15u)

void NuPortalInit(void) {
    fstack.void_ptr = fstackmem;
    fstack_top.addr = ALIGN_UP_16((u32)fstackmem);
    fstack_limit.void_ptr = fstackmem + NU_PORTAL_STACK_SIZE;
}

void NuPortalSetActive(NUGSCN *scene, i32 portal_id, i32 active) {
    if (scene->max_portals != 0) {
        NUPORTAL *portal = scene->portals;
        if (active == 0) {
            u32 index;
            for (index = 0; index < scene->max_portals; index++, portal++) {
                if (portal->id == portal_id) {
                    portal->flags = 0;
                }
            }
        } else {
            u32 index;
            for (index = 0; index < scene->max_portals; index++, portal++) {
                if (portal->id == portal_id) {
                    portal->flags |= NUPORTAL_FLAG_ACTIVE;
                }
            }
        }
    }
}

i16 __cdecl NuPortalWhichRoom(NUGSCN *scene, NUVEC *position) {
    i16 candidates[4];
    i16 candidate_count = 0;
    i32 room_index;

    if (scene == NULL || position == NULL || scene->num_rooms <= 0) {
        return -1;
    }

    for (room_index = 0; room_index < scene->num_rooms; ++room_index) {
        NUGSCNROOM *room = &scene->rooms[room_index];
        i32 plane_index;
        i32 inside = 1;

        for (plane_index = 0; plane_index < room->plane_count; ++plane_index) {
            NUPLANE *plane = &room->planes[plane_index];
            f32 distance = plane->a * position->x + plane->b * position->y + plane->c * position->z + plane->d;
            if (distance > 0.0f) {
                inside = 0;
                break;
            }
        }
        if (inside == 0) {
            continue;
        }

        if (candidate_count == 2) {
            NUGSCNROOM *first = &scene->rooms[candidates[0]];
            NUGSCNROOM *second = &scene->rooms[candidates[1]];
            if (first->priority < second->priority) {
                candidates[0] = (i16)room_index;
            } else {
                candidates[1] = (i16)room_index;
            }
        } else if (candidate_count < 4) {
            candidates[candidate_count] = (i16)room_index;
        }
        ++candidate_count;

        if (candidate_count == 2) {
            NUGSCNROOM *first = &scene->rooms[candidates[0]];
            NUGSCNROOM *second = &scene->rooms[candidates[1]];
            if (((first->visibility_flags | second->visibility_flags) & 1) == 0) {
                break;
            }
        }
    }

    if (candidate_count == 1) {
        return candidates[0];
    }
    if (candidate_count == 0) {
        return -1;
    }

    {
        NUGSCNROOM *first = &scene->rooms[candidates[0]];
        NUGSCNROOM *second = &scene->rooms[candidates[1]];
        i32 first_portal;

        for (first_portal = 0; first_portal < first->portal_count; ++first_portal) {
            i16 portal_index = first->portal_indices[first_portal];
            i32 second_portal;

            for (second_portal = 0; second_portal < second->portal_count; ++second_portal) {
                if (second->portal_indices[second_portal] == portal_index) {
                    NUPORTAL *portal = &scene->portals[portal_index];
                    f32 side = portal->plane.a * position->x + portal->plane.b * position->y +
                               portal->plane.c * position->z + portal->plane.d;
                    return side < 0.0f ? (i16)portal->front_room : (i16)portal->back_room;
                }
            }
        }
    }
    return -1;
}

void __cdecl NuPortalMaxDepth(NUGSCN *scene, i32 depth) {
    scene->portal_depth = depth;
}

i32 __cdecl clipTestSphere(NUPORTALSPHERE_PC *sphere, NUPORTAL_FRUSTUM_PC *frustum) {
    i32 fully_inside = 0;
    i32 i;
    f32 camera_distance;
    for (i = 0; i < frustum->plane_count; ++i) {
        NUPLANE *plane = &frustum->planes[i];
        f32 distance =
            plane->a * sphere->center.x + plane->b * sphere->center.y + plane->c * sphere->center.z + plane->d;
        if (distance < -sphere->radius)
            return 0;
        if (distance > sphere->radius)
            ++fully_inside;
    }
    camera_distance = NuPortalCameraPlane.a * sphere->center.x + NuPortalCameraPlane.b * sphere->center.y +
                      NuPortalCameraPlane.c * sphere->center.z + NuPortalCameraPlane.d;
    if (camera_distance < -sphere->radius)
        return 0;
    if (camera_distance > sphere->radius)
        ++fully_inside;
    return fully_inside == frustum->plane_count + 1 ? 1 : 2;
}

i32 __cdecl NuPortalClipTest(NUGSCN *scene, NUVEC *position, f32 radius, i16 room_id) {
    i32 i;
    if (scene->max_portals == 0 || scene->camera_room == room_id)
        return 1;
    for (i = 0; i < scene->num_portal_frusta; ++i) {
        NUPORTAL_FRUSTUM_PC *frustum = scene->portal_frusta[i];
        if (frustum != NULL && frustum->room_id == room_id) {
            NUPORTALSPHERE_PC sphere;
            i32 result;
            sphere.center = *position;
            sphere.radius = radius;
            result = clipTestSphere(&sphere, frustum);
            if (result != 0)
                return result;
        }
    }
    return 0;
}

i32 __cdecl clipTestBox(NUVEC *minimum, NUVEC *maximum, NUPLANE *planes, i32 plane_count) {
    NUVEC min = *minimum;
    NUVEC max = *maximum;
    i32 inside_vertices = 0;
    i32 plane_index;
    for (plane_index = 0; plane_index < plane_count; ++plane_index) {
        const NUPLANE *plane = &planes[plane_index];
        i32 inside_plane = 0;
        f32 distance1, distance2, distance3, distance4;
        f32 distance5, distance6, distance7, distance8;
        distance1 = plane->a * min.x + plane->b * min.y + plane->c * min.z + plane->d;
        if (!(distance1 < 0.0f))
            ++inside_plane, ++inside_vertices;
        distance2 = plane->a * max.x + plane->b * min.y + plane->c * min.z + plane->d;
        if (!(distance2 < 0.0f))
            ++inside_plane, ++inside_vertices;
        distance3 = plane->a * max.x + plane->b * max.y + plane->c * min.z + plane->d;
        if (!(distance3 < 0.0f))
            ++inside_plane, ++inside_vertices;
        distance4 = plane->a * min.x + plane->b * max.y + plane->c * min.z + plane->d;
        if (!(distance4 < 0.0f))
            ++inside_plane, ++inside_vertices;
        distance5 = plane->a * min.x + plane->b * min.y + plane->c * max.z + plane->d;
        if (!(distance5 < 0.0f))
            ++inside_plane, ++inside_vertices;
        distance6 = plane->a * max.x + plane->b * min.y + plane->c * max.z + plane->d;
        if (!(distance6 < 0.0f))
            ++inside_plane, ++inside_vertices;
        distance7 = plane->a * max.x + plane->b * max.y + plane->c * max.z + plane->d;
        if (!(distance7 < 0.0f))
            ++inside_plane, ++inside_vertices;
        distance8 = plane->a * min.x + plane->b * max.y + plane->c * max.z + plane->d;
        if (!(distance8 < 0.0f))
            ++inside_plane, ++inside_vertices;
        if (inside_plane == 0) {
            return 0;
        }
    }
    return inside_vertices == plane_count * 8 ? 1 : 2;
}

i32 __cdecl NuPortalClipTestBox(NUVEC *center, NUVEC *extent, NUPORTAL_FRUSTUM_PC *frustum) {
    i32 i;
    for (i = 0; i < frustum->plane_count; ++i) {
        const NUPLANE *plane = &frustum->planes[i];
        const f32 distance = plane->a * center->x + plane->b * center->y + plane->c * center->z + plane->d;
        const f32 radius = fabsf(plane->a) * extent->x + fabsf(plane->b) * extent->y + fabsf(plane->c) * extent->z;
        if (distance < -radius) {
            return 0;
        }
    }
    return 1;
}

static void transposeClipPlanes(NUPORTAL_FRUSTUM_PC *frustum) {
    f32 cam_a = NuPortalCameraPlane.a;
    f32 cam_d = NuPortalCameraPlane.d;
    f32 cam_b = NuPortalCameraPlane.b;
    f32 cam_c = NuPortalCameraPlane.c;
    NUPLANE *planes = frustum->planes;
    frustum->transposed_planes[3] = cam_a;
    frustum->transposed_planes[7] = cam_b;
    frustum->transposed_planes[11] = cam_c;
    frustum->transposed_planes[15] = cam_d;
    frustum->transposed_planes[16] = cam_a;
    frustum->transposed_planes[20] = cam_b;
    frustum->transposed_planes[24] = cam_c;
    frustum->transposed_planes[28] = cam_d;
    frustum->transposed_planes[17] = cam_a;
    frustum->transposed_planes[21] = cam_b;
    frustum->transposed_planes[25] = cam_c;
    frustum->transposed_planes[29] = cam_d;
    frustum->transposed_planes[18] = cam_a;
    frustum->transposed_planes[22] = cam_b;
    frustum->transposed_planes[26] = cam_c;
    frustum->transposed_planes[30] = cam_d;
    frustum->transposed_planes[19] = cam_a;
    frustum->transposed_planes[23] = cam_b;
    frustum->transposed_planes[27] = cam_c;
    frustum->transposed_planes[31] = cam_d;

    frustum->transposed_planes[0] = planes[0].a;
    frustum->transposed_planes[4] = planes[0].b;
    frustum->transposed_planes[8] = planes[0].c;
    frustum->transposed_planes[12] = planes[0].d;
    frustum->transposed_planes[1] = planes[1].a;
    frustum->transposed_planes[5] = planes[1].b;
    frustum->transposed_planes[9] = planes[1].c;
    frustum->transposed_planes[13] = planes[1].d;
    frustum->transposed_planes[2] = planes[2].a;
    frustum->transposed_planes[6] = planes[2].b;
    frustum->transposed_planes[10] = planes[2].c;
    frustum->transposed_planes[14] = planes[2].d;
    if (frustum->plane_count <= 3) {
        return;
    }
    frustum->transposed_planes[3] = planes[3].a;
    frustum->transposed_planes[7] = planes[3].b;
    frustum->transposed_planes[11] = planes[3].c;
    frustum->transposed_planes[15] = planes[3].d;
    if (frustum->plane_count == 4) {
        return;
    }
    frustum->transposed_planes[16] = planes[4].a;
    frustum->transposed_planes[20] = planes[4].b;
    frustum->transposed_planes[24] = planes[4].c;
    frustum->transposed_planes[28] = planes[4].d;
    if (frustum->plane_count == 5) {
        return;
    }
    frustum->transposed_planes[17] = planes[5].a;
    frustum->transposed_planes[21] = planes[5].b;
    frustum->transposed_planes[25] = planes[5].c;
    frustum->transposed_planes[29] = planes[5].d;
    if (frustum->plane_count == 6) {
        return;
    }
    frustum->transposed_planes[18] = planes[6].a;
    frustum->transposed_planes[22] = planes[6].b;
    frustum->transposed_planes[26] = planes[6].c;
    frustum->transposed_planes[30] = planes[6].d;
    if (frustum->plane_count == 7) {
        return;
    }
    frustum->transposed_planes[19] = planes[7].a;
    frustum->transposed_planes[23] = planes[7].b;
    frustum->transposed_planes[27] = planes[7].c;
    frustum->transposed_planes[31] = planes[7].d;
}

extern NUCAMERA *NuPortalCameraPc;
extern NUMTX NuPortalLocalViewPc;
extern NUMTX NuPortalProjectionScratchPc;
extern NUMTX NuPortalProjectionInversePc;
extern NUPORTAL_FRUSTUM_PC **NuPortalFrustaPc;
extern i32 *NuPortalFrustumCountPc;
extern const NUVEC NuPortalBoundsOnePc;
extern const NUVEC NuPortalBoundsMinusOnePc;
void __cdecl NuMtxInvH(NUMTX *, NUMTX *);
f32 __cdecl NuVecMtxTransformH(NUVEC *, NUVEC *, NUMTX *);
void __cdecl NuVec4MtxTransformH(NUVEC4 *, NUVEC4 *, NUMTX *);

static NUPORTAL_FRUSTUM_PC *allocateFrustrum(i16 plane_count) {
    NUPORTAL_FRUSTUM_PC *frustum = (NUPORTAL_FRUSTUM_PC *)fstack_top.void_ptr;
    u32 planes_address = ((fstack_top.addr + 15) & ~15U) + sizeof(*frustum);
    if (planes_address >= fstack_limit.addr) {
        AnimationDiagnosticNoOpPc();
    }
    frustum->planes = (NUPLANE *)planes_address;
    fstack_top.addr = planes_address + (i32)plane_count * sizeof(NUPLANE);
    if (fstack_top.addr >= fstack_limit.addr) {
        AnimationDiagnosticNoOpPc();
    }
    frustum->plane_count = plane_count;
    NuPortalFrustaPc[*NuPortalFrustumCountPc] = frustum;
    ++*NuPortalFrustumCountPc;
    if (*NuPortalFrustumCountPc >= 16) {
        AnimationDiagnosticNoOpPc();
    }
    return frustum;
}

static NUPORTAL_FRUSTUM_PC *copyFrustrum(NUPORTAL_FRUSTUM_PC *source) {
    NUPORTAL_FRUSTUM_PC *copy = allocateFrustrum(source->plane_count);
    i32 i;
    /* The complete native struct copy also overwrites the newly allocated planes pointer. */
    *copy = *source;
    for (i = 0; i < source->plane_count; ++i) {
        copy->planes[i] = source->planes[i];
    }
    return copy;
}

static void computePortalExtents(NUPORTAL *portal, NUVEC *maximum, NUVEC *minimum) {
    i32 i;
    NuPortalProjectionScratchPc = vpc_vport_mtx;
    NuMtxInvH(&NuPortalProjectionInversePc, &NuPortalProjectionScratchPc);
    NuVecMtxTransformH(maximum, &portal->vertices[0], &NuPortalProjectionScratchPc);
    *minimum = *maximum;
    for (i = 1; i < portal->vertex_count; ++i) {
        NUVEC point;
        NuVecMtxTransformH(&point, &portal->vertices[i], &NuPortalProjectionScratchPc);
        NuVecMax(maximum, maximum, &point);
        NuVecMin(minimum, minimum, &point);
    }
    NuVecMin(maximum, maximum, (NUVEC *)&NuPortalBoundsOnePc);
    NuVecMax(maximum, maximum, (NUVEC *)&NuPortalBoundsMinusOnePc);
    NuVecMin(minimum, minimum, (NUVEC *)&NuPortalBoundsOnePc);
    NuVecMax(minimum, minimum, (NUVEC *)&NuPortalBoundsMinusOnePc);
}

static NUPORTAL_FRUSTUM_PC *buildFrustrum(NUVEC *minimum, NUVEC *maximum, i16 room_id) {
    NUPORTAL_FRUSTUM_PC *frustum = allocateFrustrum(4);
    f32 half_angle = NuPortalCameraPc->fov * 0.5f;
    i64 phase = (i64)((f64)half_angle * (f64)10430.378f);
    f32 tangent = NuTrigTable[((i32)phase >> 1) & 0x7fff] / NuTrigTable[(((i32)phase + 0x4000) >> 1) & 0x7fff];
    f32 horizontal_tangent = tangent / NuPortalCameraPc->aspect;
    f32 edge, squared, inverse_length;
    i32 i;
    frustum->minimum = *minimum;
    frustum->maximum = *maximum;
    frustum->room_id = room_id;
    edge = minimum->y * tangent;
    squared = edge * edge + 1.0f;
    inverse_length = 1.0f / NuFsqrt(squared);
    frustum->planes[2].a = 0.0f;
    frustum->planes[2].b = inverse_length;
    frustum->planes[2].c = -edge * inverse_length;
    frustum->planes[2].d = 0.0f;
    edge = maximum->y * tangent;
    squared = edge * edge + 1.0f;
    inverse_length = 1.0f / NuFsqrt(squared);
    frustum->planes[3].a = 0.0f;
    frustum->planes[3].b = -inverse_length;
    frustum->planes[3].c = edge * inverse_length;
    frustum->planes[3].d = 0.0f;
    edge = maximum->x * horizontal_tangent;
    squared = edge * edge + 1.0f;
    inverse_length = 1.0f / NuFsqrt(squared);
    frustum->planes[0].a = -inverse_length;
    frustum->planes[0].b = 0.0f;
    frustum->planes[0].c = edge * inverse_length;
    frustum->planes[0].d = 0.0f;
    edge = minimum->x * horizontal_tangent;
    squared = edge * edge + 1.0f;
    inverse_length = 1.0f / NuFsqrt(squared);
    frustum->planes[1].a = inverse_length;
    frustum->planes[1].b = 0.0f;
    frustum->planes[1].c = -edge * inverse_length;
    frustum->planes[1].d = 0.0f;
    for (i = 0; i < 4; ++i) {
        NUPLANE *plane = &frustum->planes[i];
        NuVec4MtxTransformH((NUVEC4 *)plane, (NUVEC4 *)plane, &NuPortalLocalViewPc);
        plane->d = -(NuPortalLocalViewPc.m30 * plane->a + NuPortalLocalViewPc.m31 * plane->b +
                     NuPortalLocalViewPc.m32 * plane->c);
    }
    transposeClipPlanes(frustum);
    return frustum;
}

static NUPORTAL_FRUSTUM_PC *buildPortalFrustrum(NUPORTAL *portal, i16 room_id) {
    NUPORTAL_FRUSTUM_PC *frustum = allocateFrustrum(portal->vertex_count);
    NUVEC camera_position = NuPortalLocalViewPc.translation;
    f32 camera_side = portal->plane.a * camera_position.x + portal->plane.b * camera_position.y +
                      portal->plane.c * camera_position.z + portal->plane.d;
    i32 i;
    frustum->room_id = room_id;
    for (i = 0; i < portal->vertex_count; ++i) {
        NUVEC first, second, normal;
        NuVecSub(&first, &camera_position, &portal->vertices[i]);
        NuVecSub(&second, &camera_position, &portal->vertices[(i + 1) % portal->vertex_count]);
        NuVecCross(&normal, &first, &second);
        NuVecNorm(&normal, &normal);
        if (!(camera_side < 0.0f)) {
            normal.x = -normal.x;
            normal.y = -normal.y;
            normal.z = -normal.z;
        }
        frustum->planes[i].normal = normal;
        frustum->planes[i].d =
            -(normal.x * camera_position.x + normal.y * camera_position.y + normal.z * camera_position.z);
    }
    /* Native argument order is portal, maximum, minimum. */
    computePortalExtents(portal, &frustum->maximum, &frustum->minimum);
    transposeClipPlanes(frustum);
    return frustum;
}
/* Native same-TU callers. External declarations below are held genuine targets,
   not substitute implementations; the scratch object is not source-link closed. */
typedef struct PORTAL_INSTANCE_READER_PC {
    u8 unknown_00[0x44];
    u32 flags;
    u8 unknown_48[8];
} PORTAL_INSTANCE_READER_PC;
typedef struct PORTAL_SCENE_READER_PC {
    u8 unknown_00[0x1c];
    i32 instance_count;
    PORTAL_INSTANCE_READER_PC *instances;
    u8 unknown_24[0xcc];
    i32 sphere_count;
    i32 portal_instance_count;
    NUPORTALSPHERE_PC *portal_spheres;
    struct PORTAL_BOX_READER_PC *portal_boxes;
    u8 unknown_100[0x3c];
    u8 *instance_visibility_flags;
} PORTAL_SCENE_READER_PC;
typedef struct PORTAL_BOX_READER_PC {
    NUVEC first;
    f32 first_w;
    NUVEC second;
    f32 second_w;
} PORTAL_BOX_READER_PC;
typedef struct PORTAL_DISPLAY_READER_PC {
    u8 unknown_00[0x48];
    u8 *visibility_flags;
    u8 unknown_4c[0x28];
    u32 render_flags;
    u8 unknown_78[0x10];
    u8 *visibility_overrides;
} PORTAL_DISPLAY_READER_PC;
STATIC_ASSERT(sizeof(PORTAL_INSTANCE_READER_PC) == 0x50);
STATIC_ASSERT(offsetof(PORTAL_INSTANCE_READER_PC, flags) == 0x44);
STATIC_ASSERT(offsetof(PORTAL_SCENE_READER_PC, instance_count) == 0x1c);
STATIC_ASSERT(offsetof(PORTAL_SCENE_READER_PC, instances) == 0x20);
STATIC_ASSERT(offsetof(PORTAL_SCENE_READER_PC, sphere_count) == 0xf0);
STATIC_ASSERT(offsetof(PORTAL_SCENE_READER_PC, portal_instance_count) == 0xf4);
STATIC_ASSERT(offsetof(PORTAL_SCENE_READER_PC, portal_spheres) == 0xf8);
STATIC_ASSERT(offsetof(PORTAL_SCENE_READER_PC, portal_boxes) == 0xfc);
STATIC_ASSERT(offsetof(PORTAL_SCENE_READER_PC, instance_visibility_flags) == 0x13c);
STATIC_ASSERT(sizeof(PORTAL_BOX_READER_PC) == 0x20);
STATIC_ASSERT(offsetof(PORTAL_BOX_READER_PC, second) == 0x10);
STATIC_ASSERT(offsetof(PORTAL_DISPLAY_READER_PC, visibility_flags) == 0x48);
STATIC_ASSERT(offsetof(PORTAL_DISPLAY_READER_PC, render_flags) == 0x74);
STATIC_ASSERT(offsetof(PORTAL_DISPLAY_READER_PC, visibility_overrides) == 0x88);
extern u8 NuPortalVisibilityBitsPc[625];
extern NUVEC NuPortalWorldCameraPc;
extern NUVEC *NuPortalOverrideCameraPc;
extern i16 NuPortalCameraRoomPc;
extern i16 NuPortalRoomsVisitedPc[16];
extern i32 PCPortalsEnabled;
extern i32 NuPortalDebugDrawPc;
extern NUPLANE NuPortalNearPlanePc;
void __cdecl PortalClipRoomPc(NUGSCN *, NUGSCNROOM *, NUPORTAL_FRUSTUM_PC *);
static void drawInitialFrustum(NUPORTAL_FRUSTUM_PC *);
static void drawFullPortal(NUPORTAL *, NUGSCNROOM *);
static void drawPartialPortal(NUVEC *, NUPORTAL *, NUGSCNROOM *, NUVEC *);
static void drawSceneSphere(NUGSCN *, i32);

void __cdecl PortalInitialisePsPc(NUGSCN *scene) {
    ((PORTAL_SCENE_READER_PC *)scene)->instance_visibility_flags = NuPortalVisibilityBitsPc;
}
void __cdecl PortalSetAllVisiblePc(NUGSCN *scene) {
    PORTAL_SCENE_READER_PC *view = (PORTAL_SCENE_READER_PC *)scene;
    i32 i;
    if (scene->display_list != NULL) {
        memset(NuPortalVisibilityBitsPc, 255, 625);
    } else {
        for (i = 0; i < view->instance_count; ++i)
            view->instances[i].flags |= 4;
    }
}
void __cdecl PortalSetAllHiddenPc(NUGSCN *scene) {
    PORTAL_SCENE_READER_PC *view = (PORTAL_SCENE_READER_PC *)scene;
    i32 i;
    if (scene->display_list != NULL) {
        memset(NuPortalVisibilityBitsPc, 0, 625);
    } else {
        for (i = 0; i < view->instance_count; ++i)
            view->instances[i].flags &= ~4U;
    }
}
void __cdecl PortalFlagRoomVisiblePc(NUGSCNROOM *room, NUGSCN *scene) {
    PORTAL_SCENE_READER_PC *view = (PORTAL_SCENE_READER_PC *)scene;
    i32 i;
    for (i = 0; i < room->instance_count; ++i) {
        i32 index = room->instance_indices[i];
        if (scene->display_list != NULL) {
            NuPortalVisibilityBitsPc[index >> 3] |= (u8)(1 << (index & 7));
        } else {
            view->instances[index].flags |= 4;
        }
    }
}

void __cdecl PortalClipRoomPc(NUGSCN *scene, NUGSCNROOM *room, NUPORTAL_FRUSTUM_PC *frustum) {
    PORTAL_SCENE_READER_PC *view = (PORTAL_SCENE_READER_PC *)scene;
    PORTAL_DISPLAY_READER_PC *display = (PORTAL_DISPLAY_READER_PC *)scene->display_list;
    i32 i;
    i32 center_extent = display != NULL && (display->render_flags & 0x4000) != 0;
    for (i = 0; (u32)i < (u32)(i32)room->instance_count; ++i) {
        u32 index = (u32)(i32)room->instance_indices[i];
        u32 byte_index = index >> 3;
        u8 mask = (u8)(1 << (index & 7));
        i32 visible = 0;
        if ((NuPortalVisibilityBitsPc[byte_index] & mask) != 0)
            continue;
        /* Native sphere branch dereferences display even if NULL; no invented guard. */
        if ((display->visibility_overrides[byte_index] & mask) != 0)
            visible = 1;
        else if (center_extent) {
            u8 flags = display->visibility_flags[index];
            if ((flags & 1) != 0) {
                if ((flags & 8) != 0)
                    visible = 1;
                else
                    visible = NuPortalClipTestBox(&view->portal_boxes[index].first, &view->portal_boxes[index].second,
                                                  frustum) != 0;
            }
        } else if ((display->visibility_flags[index] & 8) != 0)
            visible = 1;
        else {
            i32 result = clipTestSphere(&view->portal_spheres[index], frustum);
            if (result == 1)
                visible = 1;
            else if (result == 2)
                visible = clipTestBox(&view->portal_boxes[index].first, &view->portal_boxes[index].second,
                                      frustum->planes, frustum->plane_count) != 0;
        }
        if (visible != 0)
            NuPortalVisibilityBitsPc[byte_index] |= mask;
    }
}

static i32 clipPortalAgainstFrustrum(NUGSCN *scene, i16 room_id, NUPORTAL *portal, NUPORTAL_FRUSTUM_PC *frustum,
                                     i16 next_room) {
    i32 total_inside = 0, plane_index;
    if (scene->rooms[room_id].priority < scene->rooms[next_room].priority) {
        f32 distance = portal->plane.a * NuPortalWorldCameraPc.x + portal->plane.b * NuPortalWorldCameraPc.y +
                       portal->plane.c * NuPortalWorldCameraPc.z + portal->plane.d;
        if (portal->back_room == (u16)room_id && distance >= 0.0f)
            return 0;
        if (portal->front_room == (u16)room_id && distance > 0.0f)
            return 0;
    }
    for (plane_index = 0; plane_index < frustum->plane_count; ++plane_index) {
        i32 outside = 0, vertex_index;
        NUPLANE *plane = &frustum->planes[plane_index];
        for (vertex_index = 0; vertex_index < portal->vertex_count; ++vertex_index) {
            NUVEC *v = &portal->vertices[vertex_index];
            f32 distance = plane->a * v->x + plane->b * v->y + plane->c * v->z + plane->d;
            if (!(distance >= 0.0f))
                ++outside;
            else
                ++total_inside;
        }
        if (outside == portal->vertex_count)
            return 0;
    }
    return total_inside == frustum->plane_count * portal->vertex_count ? 1 : 2;
}

static i32 intersectPortalExtents(NUVEC *first_minimum, NUVEC *first_maximum, NUVEC *second_minimum,
                                  NUVEC *second_maximum, NUVEC *minimum, NUVEC *maximum) {
    if (first_minimum->x >= second_maximum->x || first_minimum->y >= second_maximum->y ||
        first_maximum->x <= second_minimum->x || first_maximum->y <= second_minimum->y)
        return 0;
    minimum->x = first_minimum->x < second_minimum->x ? second_minimum->x : first_minimum->x;
    minimum->y = first_minimum->y < second_minimum->y ? second_minimum->y : first_minimum->y;
    maximum->x = first_maximum->x > second_maximum->x ? second_maximum->x : first_maximum->x;
    maximum->y = first_maximum->y > second_maximum->y ? second_maximum->y : first_maximum->y;
    return 1;
}

static i32 clipPartialPortal(NUPORTAL *portal, NUGSCNROOM *room, i16 room_id, NUPORTAL_FRUSTUM_PC *frustum,
                             NUPORTAL_FRUSTUM_PC **result) {
    i32 i, in_front = 0;
    NUVEC portal_minimum, portal_maximum;
    for (i = 0; i < portal->vertex_count; ++i) {
        NUVEC *vertex = &portal->vertices[i];
        f32 distance = NuPortalCameraPlane.a * vertex->x + NuPortalCameraPlane.b * vertex->y +
                       NuPortalCameraPlane.c * vertex->z + NuPortalCameraPlane.d;
        if (distance > 0.0f)
            ++in_front;
    }
    if (in_front == 0)
        return 0;
    if (in_front != portal->vertex_count)
        return 4;
    computePortalExtents(portal, &portal_maximum, &portal_minimum);
    if (!intersectPortalExtents(&frustum->minimum, &frustum->maximum, &portal_minimum, &portal_maximum, &portal_minimum,
                                &portal_maximum))
        return 0;
    drawPartialPortal(&portal_maximum, portal, room, &portal_minimum);
    *result = buildFrustrum(&portal_minimum, &portal_maximum, room_id);
    return 1;
}

static void roomRecursive(NUGSCN *scene, NUPORTAL_FRUSTUM_PC *frustum, i16 room_id, i16 previous_room, i32 depth) {
    NUGSCNROOM *room = &scene->rooms[room_id];
    i32 i;
    for (i = 0; i < depth - 1; ++i)
        if (NuPortalRoomsVisitedPc[i] == room_id)
            return;
    if (depth >= 16)
        return;
    NuPortalRoomsVisitedPc[depth] = room_id;
    if (room_id == NuPortalCameraRoomPc)
        PortalFlagRoomVisiblePc(room, scene);
    else
        PortalClipRoomPc(scene, room, frustum);
    room->flags |= 4;
    if (depth >= scene->portal_depth)
        return;
    for (i = 0; i < room->portal_count; ++i) {
        NUPORTAL *portal = &scene->portals[room->portal_indices[i]];
        i16 next_room = (i16)(portal->front_room == (u16)room_id ? portal->back_room : portal->front_room);
        i32 clip;
        if (next_room == previous_room)
            continue;
        clip = (portal->flags & 1) != 0 ? clipPortalAgainstFrustrum(scene, room_id, portal, frustum, next_room) : 0;
        if (clip == 0)
            continue;
        if (clip == 1) {
            NUPORTAL_FRUSTUM_PC *next;
            drawFullPortal(portal, &scene->rooms[next_room]);
            next = buildPortalFrustrum(portal, next_room);
            roomRecursive(scene, next, next_room, room_id, depth + 1);
        } else if (clip == 2) {
            NUPORTAL_FRUSTUM_PC *next;
            i32 partial = clipPartialPortal(portal, &scene->rooms[next_room], next_room, frustum, &next);
            if (partial == 4) {
                next = copyFrustrum(frustum);
                next->room_id = next_room;
            } else if (partial != 1)
                continue;
            roomRecursive(scene, next, next_room, room_id, depth + 1);
        } else
            roomRecursive(scene, frustum, next_room, room_id, depth + 1);
    }
}

i32 __cdecl NuPortalVisibility(NUGSCN *scene) {
    PORTAL_SCENE_READER_PC *view = (PORTAL_SCENE_READER_PC *)scene;
    NUVEC *camera_position;
    f32 near_clip;
    NUVEC near_position;
    NUPORTAL_FRUSTUM_PC *frustum;
    i32 i;
    PortalInitialisePsPc(scene);
    camera_position = NuPortalOverrideCameraPc != NULL ? NuPortalOverrideCameraPc : &NuPortalWorldCameraPc;
    if (PCPortalsEnabled == 0) {
        PortalSetAllVisiblePc(scene);
        return 0;
    }
    if (scene->max_portals == 0 || scene->num_rooms == 0 || view->portal_instance_count == 0)
        return 0;
    scene->num_portal_frusta = 0;
    NuPortalFrustumCountPc = &scene->num_portal_frusta;
    NuPortalFrustaPc = scene->portal_frusta;
    PortalSetAllHiddenPc(scene);
    NuPortalCameraPc = &global_camera;
    NuPortalLocalViewPc = global_camera.mtx;
    NuPortalWorldCameraPc = global_camera.mtx.translation;
    near_clip = global_camera.near_clip;
    NuPortalCameraRoomPc = NuPortalWhichRoom(scene, camera_position);
    scene->camera_room = NuPortalCameraRoomPc;
    if (NuPortalCameraRoomPc == -1)
        return 0;
    NuPortalCameraPlane.a = NuPortalNearPlanePc.a = NuPortalLocalViewPc.m20;
    NuPortalCameraPlane.b = NuPortalNearPlanePc.b = NuPortalLocalViewPc.m21;
    NuPortalCameraPlane.c = NuPortalNearPlanePc.c = NuPortalLocalViewPc.m22;
    near_position.x = near_clip * NuPortalLocalViewPc.m20 + NuPortalWorldCameraPc.x;
    near_position.y = near_clip * NuPortalLocalViewPc.m21 + NuPortalWorldCameraPc.y;
    near_position.z = near_clip * NuPortalLocalViewPc.m22 + NuPortalWorldCameraPc.z;
    NuPortalNearPlanePc.d = -(NuPortalNearPlanePc.a * near_position.x + NuPortalNearPlanePc.b * near_position.y +
                              NuPortalNearPlanePc.c * near_position.z);
    NuPortalCameraPlane.d =
        -(NuPortalCameraPlane.a * NuPortalWorldCameraPc.x + NuPortalCameraPlane.b * NuPortalWorldCameraPc.y +
          NuPortalCameraPlane.c * NuPortalWorldCameraPc.z);
    frustum = buildFrustrum((NUVEC *)&NuPortalBoundsMinusOnePc, (NUVEC *)&NuPortalBoundsOnePc, -2);
    drawInitialFrustum(frustum);
    for (i = 0; i < scene->num_rooms; ++i)
        scene->rooms[i].flags &= (u8)~4;
    roomRecursive(scene, frustum, NuPortalCameraRoomPc, -1, 0);
    drawSceneSphere(scene, NuPortalDebugDrawPc);
    return 1;
}

typedef NURND_VERTEX2D_PC PORTAL_LINE_VERTEX_PC;
STATIC_ASSERT(sizeof(PORTAL_LINE_VERTEX_PC) == 28);
STATIC_ASSERT(offsetof(PORTAL_LINE_VERTEX_PC, x) == 0);
STATIC_ASSERT(offsetof(PORTAL_LINE_VERTEX_PC, y) == 4);
STATIC_ASSERT(offsetof(PORTAL_LINE_VERTEX_PC, colour) == 0x10);
extern i32 NuPortalDebugEnabledPc;
extern i32 NuPortalCameraSelectPc;
extern NUCAMERA NuPortalDebugCameraPc;
extern NUCAMERA NuPortalRestoreCameraPc;
extern f32 zx;
extern f32 zy;
void __cdecl NuRndrLine3dDbg(f32, f32, f32, f32, f32, f32, i32);
i32 __cdecl NuRndrLineStrip2d(PORTAL_LINE_VERTEX_PC *, NUMTL *, i32);
void __cdecl NuRndrSphere(NUVEC *, f32, i32, f32);

static void drawPortalRectangle(NUVEC *maximum, NUVEC *minimum, i32 colour) {
    PORTAL_LINE_VERTEX_PC vertices[8];
    f32 min_x = 0.5f * (1.0f + minimum->x);
    f32 min_y = 0.5f * (1.0f - minimum->y);
    f32 max_x = 0.5f * (1.0f + maximum->x);
    f32 max_y = 0.5f * (1.0f - maximum->y);
    i32 i;
    vertices[0].x = vertices[7].x = vertices[1].x = vertices[2].x = min_x;
    vertices[3].x = vertices[4].x = vertices[5].x = vertices[6].x = max_x;
    vertices[0].y = vertices[7].y = vertices[5].y = vertices[6].y = min_y;
    vertices[1].y = vertices[2].y = vertices[3].y = vertices[4].y = max_y;
    for (i = 0; i < 8; ++i) {
        vertices[i].z = 1.0f;
        vertices[i].colour = (u32)colour;
    }
    /* Native line-strip consumer reads only x/y; untouched padding are not fabricated. */
    NuRndrLineStrip2d(vertices, NULL, 8);
}

static void drawInitialFrustum(NUPORTAL_FRUSTUM_PC *frustum) {
    NUVEC rays[4], corners[4];
    i32 i;
    (void)frustum;
    if (NuPortalDebugEnabledPc == 0)
        return;
    rays[0].x = -zx;
    rays[0].y = -zy;
    rays[0].z = 1.0f;
    rays[1].x = -zx;
    rays[1].y = zy;
    rays[1].z = 1.0f;
    rays[2].x = zx;
    rays[2].y = zy;
    rays[2].z = 1.0f;
    rays[3].x = zx;
    rays[3].y = -zy;
    rays[3].z = 1.0f;
    for (i = 0; i < 4; ++i)
        NuVecMtxRotate(&rays[i], &rays[i], &global_camera.mtx);
    for (i = 0; i < 4; ++i) {
        NuVecScale(&corners[i], &rays[i], 0.2f);
        NuVecAdd(&corners[i], &corners[i], &NuPortalWorldCameraPc);
    }
    if (NuPortalCameraSelectPc == 1)
        NuCameraSet(&NuPortalDebugCameraPc);
    for (i = 0; i < 4; ++i) {
        NUVEC *a = &corners[i], *b = &corners[(i + 1) % 4];
        NuRndrLine3dDbg(NuPortalWorldCameraPc.x, NuPortalWorldCameraPc.y, NuPortalWorldCameraPc.z, a->x, a->y, a->z,
                        -1);
        NuRndrLine3dDbg(a->x, a->y, a->z, b->x, b->y, b->z, (i32)0xffff0000U);
    }
    if (NuPortalCameraSelectPc == 1)
        NuCameraSet(&NuPortalRestoreCameraPc);
}

static void drawFullPortal(NUPORTAL *portal, NUGSCNROOM *room) {
    i32 i;
    NUVEC nearest;
    if (NuPortalDebugEnabledPc == 0)
        return;
    if (NuPortalCameraSelectPc == 1)
        NuCameraSet(&NuPortalDebugCameraPc);
    for (i = 0; i < portal->vertex_count; ++i) {
        NUVEC direction;
        NUVEC *a = &portal->vertices[i], *b = &portal->vertices[(i + 1) % portal->vertex_count];
        NuVecSub(&direction, a, b);
        NuRndrLine3dDbg(a->x, a->y, a->z, b->x, b->y, b->z, (i32)0xffff00ffU);
    }
    /* PC retains the preceding nearest point if no later positive plane hit is found. */
    for (i = 0; i < portal->vertex_count; ++i) {
        NUVEC direction;
        NUVEC *vertex = &portal->vertices[i];
        f32 nearest_distance = 3.402823466e+38f;
        i32 plane_index;
        NuVecSub(&direction, vertex, &NuPortalWorldCameraPc);
        NuVecNorm(&direction, &direction);
        for (plane_index = 0; plane_index < room->plane_count; ++plane_index) {
            NUVEC point;
            f32 distance = NuPlnLine3(&room->planes[plane_index], vertex, &direction, &point);
            if (distance > 0.0f && nearest_distance > distance) {
                nearest = point;
                nearest_distance = distance;
            }
        }
        NuRndrLine3dDbg(nearest.x, nearest.y, nearest.z, vertex->x, vertex->y, vertex->z, (i32)0xffff00ffU);
    }
    if (NuPortalCameraSelectPc == 1)
        NuCameraSet(&NuPortalRestoreCameraPc);
}

static void drawPartialPortal(NUVEC *maximum, NUPORTAL *portal, NUGSCNROOM *room, NUVEC *minimum) {
    NUVEC rays[4], corners[4], nearest;
    i32 i;
    if (NuPortalDebugEnabledPc == 0)
        return;
    rays[0].x = minimum->x * zx;
    rays[0].y = minimum->y * zy;
    rays[0].z = 1.0f;
    rays[1].x = minimum->x * zx;
    rays[1].y = maximum->y * zy;
    rays[1].z = 1.0f;
    rays[2].x = maximum->x * zx;
    rays[2].y = maximum->y * zy;
    rays[2].z = 1.0f;
    rays[3].x = maximum->x * zx;
    rays[3].y = minimum->y * zy;
    rays[3].z = 1.0f;
    for (i = 0; i < 4; ++i) {
        NuVecMtxRotate(&rays[i], &rays[i], &global_camera.mtx);
        NuPlnLine3(&portal->plane, &NuPortalWorldCameraPc, &rays[i], &corners[i]);
    }
    if (NuPortalCameraSelectPc == 1)
        NuCameraSet(&NuPortalDebugCameraPc);
    drawPortalRectangle(maximum, minimum, 0xff00);
    for (i = 0; i < 4; ++i) {
        NUVEC *a = &corners[i], *b = &corners[(i + 1) % 4];
        NuRndrLine3dDbg(a->x, a->y, a->z, b->x, b->y, b->z, (i32)0xff0000ffU);
    }
    for (i = 0; i < 4; ++i) {
        f32 nearest_distance = 3.402823466e+38f;
        i32 plane_index;
        NuVecNorm(&rays[i], &rays[i]);
        for (plane_index = 0; plane_index < room->plane_count; ++plane_index) {
            NUVEC point;
            f32 distance = NuPlnLine3(&room->planes[plane_index], &corners[i], &rays[i], &point);
            if (distance > 0.0f && nearest_distance > distance) {
                nearest = point;
                nearest_distance = distance;
            }
        }
        NuRndrLine3dDbg(nearest.x, nearest.y, nearest.z, corners[i].x, corners[i].y, corners[i].z, (i32)0xff0000ffU);
    }
    if (NuPortalCameraSelectPc == 1)
        NuCameraSet(&NuPortalRestoreCameraPc);
}

static void drawSceneSphere(NUGSCN *scene, i32 sphere_index) {
    PORTAL_SCENE_READER_PC *view = (PORTAL_SCENE_READER_PC *)scene;
    if (sphere_index != -1 && sphere_index < view->sphere_count) {
        NUPORTALSPHERE_PC *sphere = &view->portal_spheres[sphere_index];
        NuRndrSphere(&sphere->center, sphere->radius, 0x400000, 1.0f);
    }
}

void __cdecl NuPortalSetActiveDirect(NUPORTAL *portal, i32 active) {
    if (portal != NULL) {
        if (active != 0)
            portal->flags |= 1;
        else
            portal->flags = 0;
    }
}
