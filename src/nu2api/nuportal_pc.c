#include "saga.h"
#include <string.h>

/* PC portal visibility backend. Retail keeps these in the platform unit after nu3d
 * (0x600bd0-0x600f20), so nu3d's NuPortalVisibility calls them instead of inlining. */

typedef struct PORTAL_BOX_PC {
    NUVEC first;
    f32 first_w;
    NUVEC second;
    f32 second_w;
} PORTAL_BOX_PC;
STATIC_ASSERT(sizeof(PORTAL_BOX_PC) == 0x20);
STATIC_ASSERT(offsetof(PORTAL_BOX_PC, second) == 0x10);
STATIC_ASSERT(offsetof(NUGSCN, num_instances) == 0x1c);
STATIC_ASSERT(offsetof(NUGSCN, sphere_count) == 0xf0);
STATIC_ASSERT(offsetof(NUGSCN, portal_instance_count) == 0xf4);
STATIC_ASSERT(offsetof(NUGSCN, portal_spheres) == 0xf8);
STATIC_ASSERT(offsetof(NUGSCN, portal_boxes) == 0xfc);
STATIC_ASSERT(offsetof(NUGSCN, instance_visibility_flags) == 0x13c);
STATIC_ASSERT(offsetof(NUINSTANCE, flags) == 0x44);
STATIC_ASSERT(offsetof(NUDISPLAY_LIST_PC, visibility_flags) == 0x48);
STATIC_ASSERT(offsetof(NUDISPLAY_LIST_PC, render_flags) == 0x74);
STATIC_ASSERT(offsetof(NUDISPLAY_LIST_PC, portal_visibility_overrides) == 0x88);

void __cdecl PortalInitialisePsPc(NUGSCN *scene) {
    scene->instance_visibility_flags = NuPortalVisibilityBitsPc;
}
void __cdecl PortalSetAllVisiblePc(NUGSCN *scene) {
    i32 i;
    if (scene->display_list != NULL) {
        memset(NuPortalVisibilityBitsPc, 255, 625);
    } else {
        for (i = 0; i < scene->num_instances; ++i)
            scene->instances[i].flags |= 4;
    }
}
void __cdecl PortalSetAllHiddenPc(NUGSCN *scene) {
    i32 i;
    if (scene->display_list != NULL) {
        memset(NuPortalVisibilityBitsPc, 0, 625);
    } else {
        for (i = 0; i < scene->num_instances; ++i)
            scene->instances[i].flags &= ~4U;
    }
}
void __cdecl PortalFlagRoomVisiblePc(NUGSCNROOM *room, NUGSCN *scene) {
    i32 i;
    if (scene->display_list != NULL) {
        for (i = 0; i < room->instance_count; ++i) {
            i32 index = room->instance_indices[i];
            NuPortalVisibilityBitsPc[index >> 3] |= (u8)(1 << (index & 7));
        }
    } else {
        for (i = 0; i < room->instance_count; ++i)
            scene->instances[room->instance_indices[i]].flags |= 4;
    }
}

void __cdecl PortalClipRoomPc(NUGSCN *scene, NUGSCNROOM *room, NUPORTAL_FRUSTUM_PC *frustum) {
    NUDISPLAY_LIST_PC *display = scene->display_list;
    u32 i;
    if (display != NULL && (display->render_flags & 0x4000) != 0) {
        for (i = 0; i < (u32)(i32)room->instance_count; ++i) {
            u32 index = (u32)(i32)room->instance_indices[i];
            u32 byte_index = index >> 3;
            u32 bit = index & 7;
            if (((NuPortalVisibilityBitsPc[byte_index] >> bit) & 1) != 0)
                continue;
            display = scene->display_list;
            if (((display->portal_visibility_overrides[byte_index] >> bit) & 1) != 0) {
                NuPortalVisibilityBitsPc[byte_index] |= (u8)(1 << bit);
            } else {
                u8 flags = display->visibility_flags[index];
                if ((flags & 1) != 0) {
                    if ((flags & 8) != 0)
                        NuPortalVisibilityBitsPc[byte_index] |= (u8)(1 << bit);
                    else if (NuPortalClipTestBox(&((PORTAL_BOX_PC *)scene->portal_boxes)[index].first,
                                                 &((PORTAL_BOX_PC *)scene->portal_boxes)[index].second, frustum) != 0)
                        NuPortalVisibilityBitsPc[byte_index] |= (u8)(1 << bit);
                }
            }
        }
    } else {
        for (i = 0; i < (u32)(i32)room->instance_count; ++i) {
            u32 index = (u32)(i32)room->instance_indices[i];
            u32 byte_index = index >> 3;
            u32 bit = index & 7;
            u8 bits = NuPortalVisibilityBitsPc[byte_index];
            if (((bits >> bit) & 1) != 0)
                continue;
            /* Native dereferences a null display only for an unseen instance. */
            display = scene->display_list;
            if (((display->portal_visibility_overrides[byte_index] >> bit) & 1) != 0) {
                NuPortalVisibilityBitsPc[byte_index] = bits | (u8)(1 << bit);
                continue;
            }
            if ((display->visibility_flags[index] & 8) != 0) {
                NuPortalVisibilityBitsPc[byte_index] = bits | (u8)(1 << bit);
                continue;
            }
            switch (clipTestSphere(&scene->portal_spheres[index], frustum)) {
                case 1:
                    NuPortalVisibilityBitsPc[byte_index] |= (u8)(1 << bit);
                    break;
                case 2:
                    if (clipTestBox(&((PORTAL_BOX_PC *)scene->portal_boxes)[index].first,
                                    &((PORTAL_BOX_PC *)scene->portal_boxes)[index].second, frustum->planes,
                                    frustum->plane_count) != 0)
                        NuPortalVisibilityBitsPc[byte_index] |= (u8)(1 << bit);
                    break;
            }
        }
    }
}
