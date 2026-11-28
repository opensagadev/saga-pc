#include <stdio.h>
#include <string.h>
#include "gameapi/apisave.h"
#include "gameapi/edmain.h"
#include "gameapi/edpart.h"
#include "gameapi/edtools/edanim_internal.h"
#include "gameapi/edtools/edcam.h"
#include "gameapi/edtools/edfile.h"
#include "gameapi/edtools/edpart_internal.h"
#include "gameapi/edtools/edrender.h"
#include "gameapi/edtools/edui.h"
#include "globals.h"
#include "legoapi/items/objects/game_obj.h"
#include "legoapi/items/objects/objectsall.h"
#include "legoapi/render/core/gameliball.h"
#include "legoapi/render/fx/game_deb.h"
#include "legoapi/render/fx/parts.h"
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nuanimation_gen.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nufile/nufile.h"
#include "nu2api/nugscn_gen.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/numusic/sfx.h"
#include "gameapi/edptl.h"
#include "gameapi/edtools/edgra.h"
#include "gameapi/edtools/edpp_internal.h"
#include "legoapi/render/fx.h"
#include "nu2api/numath/nurand.h"
#include "gameapi/apitext.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/menus/screens/gamemenuall.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nurndr.h"
#include "nu2api/nucore/nuapi.h"
#include "batman.h"
#include "gameapi/edtools/edstubs.h"
#include "gameapi/part.h"
#include "legoapi/characters/motion.h"
#include "legoapi/core/input/qrand.h"
#include "legoapi/core/input/timer.h"
#include "legoapi/gizmos/fx/gizmopickups.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/collect/bolts.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/misc/utilities.h"
#include "legoapi/render/core/rtl.h"
#include "legoapi/render/core/terrain.h"
#include "legoapi/render/light/lighting.h"
#include "legoapi/world/level.h"
#include "legoapi/world/levels.h"
#include "legoapi/world/levels/episodeIII.h"
#include "legoapi/world/levels/levels.h"
#include "legoapi/world/world.h"
#include "legoapi/world/world_shared.h"
#include "legogame/game.h"
#include "nu2api/nu3d/glutils.h"
#include "nu2api/nu3d/nuportal.h"
#include "nu2api/nu3d/nuptl.h"
#include "nu2api/nu3d/nuqfnt.h"
#include "nu2api/nu3d/nutimebar_plain.h"
#include "nu2api/nualloc.h"
#include "nu2api/nucore/nucore_includes.h"
#include "nu2api/nucore/nuthread.h"
#include "nu2api/nudynamiclight_gen.h"
#include "nu2api/nuerror_gen.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/numath_includes.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/nustencilshadows_PC.h"
#include <float.h>
#include "gameapi/edbits.h"

extern "C" {
    extern struct eduimenu_s *edpart_varemit_menu;
    extern struct eduimenu_s *edpart_varstart_menu;
    extern struct eduimenu_s *edpart_emittime_menu;
    extern struct eduimenu_s *edpart_emitvel_menu;
    extern i32 edpart_first_time_this_level;
    extern i32 edpart_curr;
    extern NUVEC edpart_entry_position;
    extern struct eduimenu_s *edpart_sscale_menu;
}
extern "C" {

    void ObjectsallNuError(void) {
    }

    /* The SDK NuFabs; the editor half of this unit keeps its own copy. */
    static f32 PartsFabs(f32 f) {
        f32 rv;
        *(i32 *)&rv = *(i32 *)&f & 0x7fffffff;
        return rv;
    }

    /* a / b, 0 when either is 0. */
    static f32 PartsFdiv(f32 a, f32 b) {
        if (a == 0.0f || b == 0.0f) {
            return 0.0f;
        }
        return a / b;
    }

    static void PartRuntimeDirectionalLightsPrivate(NUVEC *direction0, NUVEC *colour0, NUVEC *direction1, NUVEC *colour1, NUVEC *direction2,
                                                    NUVEC *colour2) {
        PCGlobalLightingState.direction[0] = *direction0;
        PCGlobalLightingState.direction[1] = *direction1;
        PCGlobalLightingState.direction[2] = *direction2;
        PCGlobalLightingState.intensity[0] = *colour0;
        PCGlobalLightingState.intensity[1] = *colour1;
        PCGlobalLightingState.intensity[2] = *colour2;
        NuRndrSetDirectionalLightsPS(direction0, colour0, direction1, colour1, direction2, colour2);
    }

    /* The ambient colour both in the PC lighting state and in the pixel shader constants. */
    static i32 PartsSetAmbientLight(NUVEC *ambient) {
        PCGlobalLightingState.ambient = *ambient;
        return NuRndrSetAmbientLightPS(ambient);
    }

    static void PartRuntimeMatrixCopyPrivate(NUMTX *destination, NUMTX *source) {
        destination->m00 = source->m00;
        destination->m01 = source->m01;
        destination->m02 = source->m02;
        destination->m03 = source->m03;
        destination->m10 = source->m10;
        destination->m11 = source->m11;
        destination->m12 = source->m12;
        destination->m13 = source->m13;
        destination->m20 = source->m20;
        destination->m21 = source->m21;
        destination->m22 = source->m22;
        destination->m23 = source->m23;
        destination->m30 = source->m30;
        destination->m31 = source->m31;
        destination->m32 = source->m32;
        destination->m33 = source->m33;
    }

    void fun_00642520(void) {
    }

    static NUVEC PartImpactNormalPrivate = {0};

    i32 PartRayCast(NUVEC *position, NUVEC *movement, f32 radius, i32 flags) {
        i32 hit = NewRayCast(position, movement, radius, flags);
        NewRayCastGetImpactNormal(&PartImpactNormalPrivate);
        return hit;
    }

    i32 PartPlatId(void) {
        return TerrainPlatId();
    }

    f32 PARTSCALEUPTIME = 0.5f;

    void CastPart(PART *part, f32 time) {
        i32 hit;
        f32 distance;
        f32 radius;
        NUVEC movement;
        NUVEC cast_movement;
        part->active &= ~PART_STATE_IMPACT_PENDING;
        part->time_of_impact = time;
        movement.x = time * part->velocity.x;
        movement.y = time * time * (0.5 * part->gravity) + time * part->velocity.y;
        movement.z = time * part->velocity.z;
        if (part->velocity.y > 0.0f && movement.y <= 0.0f) {
            time = -part->velocity.y / part->gravity;
            part->time_of_impact = time;
            movement.x = time * part->velocity.x;
            movement.y = time * time * (0.5 * part->gravity) + time * part->velocity.y;
            movement.z = time * part->velocity.z;
        }
        distance = NuVecMagSqr(&movement);
        if (distance < 0.001f || distance > 10000.0f || !_finite(distance) || _isnan(distance)) {
            part->surface_type = -1;
            return;
        }
        if ((part->active & (PART_STATE_SCALING_UP | PART_STATE_SCALING_DOWN)) == PART_STATE_SCALING_UP) {
            radius = (part->time_step + time) / PARTSCALEUPTIME * part->radius;
            if (radius > part->radius) {
                radius = part->radius;
            }
            cast_movement = movement;
            hit = PartRayCast(&part->position, &cast_movement, radius, 0);
            if (hit == 0) {
                part->surface_type = -1;
                return;
            }
            part->surface_type = NewRayCastGetImpactTerrainType();
            if ((NewRayCastGetTOFI() > 0.0f || PartImpactNormalPrivate.y > 0.5f) &&
                ((part->flags & 0x10000000) == 0 || NewRayCastHitWallSpline() != 4)) {
                part->active |= PART_STATE_IMPACT_PENDING;
                part->platform_id = PartPlatId();
                part->impact_on_platform = part->platform_id != -1;
                part->time_of_impact *= NewRayCastGetTOFI();
                part->impact_position.x = part->position.x + cast_movement.x;
                part->impact_position.y = part->position.y + cast_movement.y;
                part->impact_position.z = part->position.z + cast_movement.z;
                part->impact_normal = PartImpactNormalPrivate;
                return;
            }
            if (hit > 0x10) {
                part->position.x -= PartImpactNormalPrivate.x * NewRayCastGetEmbedDist() * 1.05f;
                part->position.y -= PartImpactNormalPrivate.y * NewRayCastGetEmbedDist() * 1.05f;
                part->position.z -= PartImpactNormalPrivate.z * NewRayCastGetEmbedDist() * 1.05f;
            }
        }
        part->active &= ~(PART_STATE_SCALING_UP | PART_STATE_SCALING_DOWN);
        cast_movement = movement;
        hit = PartRayCast(&part->position, &cast_movement, part->glow_radius, 0);
        if (hit == 0) {
            part->surface_type = -1;
            return;
        }
        part->surface_type = NewRayCastGetImpactTerrainType();
        if ((NewRayCastGetTOFI() > 0.0f || PartImpactNormalPrivate.y > 0.5f) &&
            ((part->flags & 0x10000000) == 0 || NewRayCastHitWallSpline() != 4)) {
            part->active |= PART_STATE_IMPACT_PENDING;
            part->platform_id = PartPlatId();
            part->impact_on_platform = part->platform_id != -1;
            part->time_of_impact *= NewRayCastGetTOFI();
            part->impact_position.x = part->position.x + cast_movement.x;
            part->impact_position.y = part->position.y + cast_movement.y;
            part->impact_position.z = part->position.z + cast_movement.z;
            part->impact_normal = PartImpactNormalPrivate;
            return;
        }
        if (hit > 0x10) {
            part->position.x -= PartImpactNormalPrivate.x * NewRayCastGetEmbedDist() * 1.05f;
            part->position.y -= PartImpactNormalPrivate.y * NewRayCastGetEmbedDist() * 1.05f;
            part->position.z -= PartImpactNormalPrivate.z * NewRayCastGetEmbedDist() * 1.05f;
        }
        radius = (part->time_step - time) / PARTSCALEUPTIME * part->radius;
        if (!(radius > 0.0f)) {
            return;
        }
        part->active = (part->active & ~(PART_STATE_SCALING_UP | PART_STATE_SCALING_DOWN)) | PART_STATE_SCALING_DOWN;
        cast_movement = movement;
        if (PartRayCast(&part->position, &cast_movement, radius, 0) == 0) {
            part->surface_type = -1;
            return;
        }
        part->surface_type = NewRayCastGetImpactTerrainType();
        if ((part->flags & 0x10000000) != 0 && NewRayCastHitWallSpline() == 4) {
            return;
        }
        part->active |= PART_STATE_IMPACT_PENDING;
        part->platform_id = PartPlatId();
        part->impact_on_platform = part->platform_id != -1;
        part->time_of_impact *= NewRayCastGetTOFI();
        part->impact_position.x = part->position.x + cast_movement.x;
        part->impact_position.y = part->position.y + cast_movement.y;
        part->impact_position.z = part->position.z + cast_movement.z;
        part->impact_normal = PartImpactNormalPrivate;
    }

    void edpartRegisterPointerToGameCharLocation(NUVEC *charloc) {
        edmainSetLocVec(charloc);
    }

    PART_EMIT part_emits[40] = {0};

    void edpartPtlShelve(i32 index) {
        if (part_emits[index].instance_id != -1 && part_emits[index].instance_id != 99999) {
            DebFreeInstantly(&part_emits[index].instance_id);
            part_emits[index].instance_id = 99999;
        }
    }

    i32 edpart_emitrotz = 0;
    i32 edpart_emitroty = 0;
    i32 edpart_emitrotx = 0;

    i32 edpartPlace(i32 index, NUVEC *position) {
        part_emits[index].position = *position;
        part_emits[index].rotation_2c = (i16)edpart_emitrotz;
        part_emits[index].rotation_2e = (i16)edpart_emitroty;
        part_emits[index].rotation_30 = (i16)edpart_emitrotx;
        return index;
    }

    i32 edpart_copyroty = 0;
    NUVEC PartEditorCameraPos = {0};
    NUVEC edpart_copy_source_vec = {0};
    i32 edpart_copy_source[8] = {0};
    i32 edpart_copy_source_count = 0;
    f32 edpart_copy_size = 0.2f;

    void edpartMultipleCopyCopy(void) {
        i32 index;
        NUVEC delta;

        f32 abs_delta[3];

        edpart_copy_source_count = 0;
        for (index = 0; index < 512; ++index) {
            if (part_emits[index].instance_id == -1 || part_emits[index].instance_id == 99999) {
                continue;
            }
            NuVecSub(&delta, &PartEditorCameraPos, &part_emits[index].position);
            abs_delta[0] = PartsFabs(delta.x);
            if (!(abs_delta[0] <= edpart_copy_size)) {
                continue;
            }
            abs_delta[1] = PartsFabs(delta.y);
            if (!(abs_delta[1] <= edpart_copy_size)) {
                continue;
            }
            abs_delta[2] = PartsFabs(delta.z);
            if (!(abs_delta[2] <= edpart_copy_size)) {
                continue;
            }
            edpart_copy_source[edpart_copy_source_count++] = index;
            if (edpart_copy_source_count == 8) {
                break;
            }
        }
        edpart_copy_source_vec = PartEditorCameraPos;
        edpart_copyroty = 0;
    }

    void edpartMultipleCopyClear(void) {
        edpart_copy_source_count = 0;
        edpart_copyroty = 0;
    }

    i32 edpart_rotz = 0;
    i32 edpart_roty = 0;
    i32 edpart_refrotz = 0;
    i32 edpart_refroty = 0;
    i32 edpart_copyrotz = 0;
    NUMTL *edpart_boxmtl = 0;
    NUMTL *edpart_mtl = 0;
    i32 edpart_nearest_duplicates = 0;
    i32 edpart_nearest = 0;
    PART_EMIT *edpart_nearest_emit = 0;
    PARTTYPE_LOOKUP *edpart_nearest_type = 0;
    i8 edpart_effect_list = 0;
    i32 edpart_copy_mode = 0;
    i32 edpart_copy_enclosed = 0;
    i32 edpart_num_orphans = 0;
    i32 edpart_dpad_mode = 0;
    i32 edpart_readout = 0;
    i32 edpart_create_type = -1;
    i32 edpart_nearest_orphans = 0;
    i32 part_types_used = 0;
    i32 part_emits_used = 0;

    void edpartDrawCursor(void) {
        i32 rotation_z;
        i32 rotation_y;
        NUVEC vector;
        NURND_VERTEX3D line[2];

        if (edpart_copy_mode != 0) {
            rotation_z = edpart_copyrotz;
            rotation_y = edpart_copyroty;
        } else {
            rotation_z = edpart_rotz;
            rotation_y = edpart_roty;
        }
        vector.x = 0.5f;
        vector.y = 0.0f;
        vector.z = 0.0f;
        NuVecRotateZ(&vector, &vector, rotation_z);
        NuVecRotateY(&vector, &vector, rotation_y);
        line[0].position.x = PartEditorCameraPos.x - vector.x;
        line[0].position.y = PartEditorCameraPos.y - vector.y;
        line[0].position.z = PartEditorCameraPos.z - vector.z;
        line[0].colour = 0xffffffff;
        line[1].position.x = PartEditorCameraPos.x;
        line[1].position.y = PartEditorCameraPos.y;
        line[1].position.z = PartEditorCameraPos.z;
        line[1].colour = 0xffffffff;
        NuRndrLine3d(line, edpart_mtl, NULL);
        line[0].position.x = PartEditorCameraPos.x;
        line[0].position.y = PartEditorCameraPos.y;
        line[0].position.z = PartEditorCameraPos.z;
        line[0].colour = 0xff00ff00;
        line[1].position.x = PartEditorCameraPos.x + vector.x;
        line[1].position.y = PartEditorCameraPos.y + vector.y;
        line[1].position.z = PartEditorCameraPos.z + vector.z;
        line[1].colour = 0xff00ff00;
        NuRndrLine3d(line, edpart_mtl, NULL);
        vector.x = 0.0f;
        vector.y = 0.5f;
        vector.z = 0.0f;
        NuVecRotateZ(&vector, &vector, rotation_z);
        NuVecRotateY(&vector, &vector, rotation_y);
        line[0].position.x = PartEditorCameraPos.x - vector.x;
        line[0].position.y = PartEditorCameraPos.y - vector.y;
        line[0].position.z = PartEditorCameraPos.z - vector.z;
        line[0].colour = 0xffffffff;
        line[1].position.x = PartEditorCameraPos.x;
        line[1].position.y = PartEditorCameraPos.y;
        line[1].position.z = PartEditorCameraPos.z;
        line[1].colour = 0xffffffff;
        NuRndrLine3d(line, edpart_mtl, NULL);
        line[0].position.x = PartEditorCameraPos.x;
        line[0].position.y = PartEditorCameraPos.y;
        line[0].position.z = PartEditorCameraPos.z;
        line[0].colour = 0xff00ff00;
        line[1].position.x = PartEditorCameraPos.x + vector.x;
        line[1].position.y = PartEditorCameraPos.y + vector.y;
        line[1].position.z = PartEditorCameraPos.z + vector.z;
        line[1].colour = 0xff00ff00;
        NuRndrLine3d(line, edpart_mtl, NULL);
        vector.x = 0.0f;
        vector.y = 0.0f;
        vector.z = 0.5f;
        NuVecRotateZ(&vector, &vector, rotation_z);
        NuVecRotateY(&vector, &vector, rotation_y);
        line[0].position.x = PartEditorCameraPos.x - vector.x;
        line[0].position.y = PartEditorCameraPos.y - vector.y;
        line[0].position.z = PartEditorCameraPos.z - vector.z;
        line[0].colour = 0xffffffff;
        line[1].position.x = PartEditorCameraPos.x;
        line[1].position.y = PartEditorCameraPos.y;
        line[1].position.z = PartEditorCameraPos.z;
        line[1].colour = 0xffffffff;
        NuRndrLine3d(line, edpart_mtl, NULL);
        line[0].position.x = PartEditorCameraPos.x;
        line[0].position.y = PartEditorCameraPos.y;
        line[0].position.z = PartEditorCameraPos.z;
        line[0].colour = 0xff00ff00;
        line[1].position.x = PartEditorCameraPos.x + vector.x;
        line[1].position.y = PartEditorCameraPos.y + vector.y;
        line[1].position.z = PartEditorCameraPos.z + vector.z;
        line[1].colour = 0xff00ff00;
        NuRndrLine3d(line, edpart_mtl, NULL);
        vector.x = 0.55f;
        vector.y = 0.05f;
        vector.z = 0.0f;
        NuVecRotateZ(&vector, &vector, rotation_z);
        NuVecRotateY(&vector, &vector, rotation_y);
        line[0].position.x = PartEditorCameraPos.x + vector.x;
        line[0].position.y = PartEditorCameraPos.y + vector.y;
        line[0].position.z = PartEditorCameraPos.z + vector.z;
        line[0].colour = 0xff00ff00;
        vector.x = 0.6f;
        vector.y = -0.05f;
        vector.z = 0.0f;
        NuVecRotateZ(&vector, &vector, rotation_z);
        NuVecRotateY(&vector, &vector, rotation_y);
        line[1].position.x = PartEditorCameraPos.x + vector.x;
        line[1].position.y = PartEditorCameraPos.y + vector.y;
        line[1].position.z = PartEditorCameraPos.z + vector.z;
        line[1].colour = 0xff00ff00;
        NuRndrLine3d(line, edpart_mtl, NULL);
        vector.x = 0.6f;
        vector.y = 0.05f;
        vector.z = 0.0f;
        NuVecRotateZ(&vector, &vector, rotation_z);
        NuVecRotateY(&vector, &vector, rotation_y);
        line[0].position.x = PartEditorCameraPos.x + vector.x;
        line[0].position.y = PartEditorCameraPos.y + vector.y;
        line[0].position.z = PartEditorCameraPos.z + vector.z;
        line[0].colour = 0xff00ff00;
        vector.x = 0.55f;
        vector.y = -0.05f;
        vector.z = 0.0f;
        NuVecRotateZ(&vector, &vector, rotation_z);
        NuVecRotateY(&vector, &vector, rotation_y);
        line[1].position.x = PartEditorCameraPos.x + vector.x;
        line[1].position.y = PartEditorCameraPos.y + vector.y;
        line[1].position.z = PartEditorCameraPos.z + vector.z;
        line[1].colour = 0xff00ff00;
        NuRndrLine3d(line, edpart_mtl, NULL);
        vector.x = 0.0f;
        vector.y = 0.65f;
        vector.z = -0.025f;
        NuVecRotateZ(&vector, &vector, rotation_z);
        NuVecRotateY(&vector, &vector, rotation_y);
        line[0].position.x = PartEditorCameraPos.x + vector.x;
        line[0].position.y = PartEditorCameraPos.y + vector.y;
        line[0].position.z = PartEditorCameraPos.z + vector.z;
        line[0].colour = 0xff00ff00;
        vector.x = 0.0f;
        vector.y = 0.6f;
        vector.z = 0.0f;
        NuVecRotateZ(&vector, &vector, rotation_z);
        NuVecRotateY(&vector, &vector, rotation_y);
        line[1].position.x = PartEditorCameraPos.x + vector.x;
        line[1].position.y = PartEditorCameraPos.y + vector.y;
        line[1].position.z = PartEditorCameraPos.z + vector.z;
        line[1].colour = 0xff00ff00;
        NuRndrLine3d(line, edpart_mtl, NULL);
        vector.x = 0.0f;
        vector.y = 0.65f;
        vector.z = 0.025f;
        NuVecRotateZ(&vector, &vector, rotation_z);
        NuVecRotateY(&vector, &vector, rotation_y);
        line[0].position.x = PartEditorCameraPos.x + vector.x;
        line[0].position.y = PartEditorCameraPos.y + vector.y;
        line[0].position.z = PartEditorCameraPos.z + vector.z;
        line[0].colour = 0xff00ff00;
        vector.x = 0.0f;
        vector.y = 0.6f;
        vector.z = 0.0f;
        NuVecRotateZ(&vector, &vector, rotation_z);
        NuVecRotateY(&vector, &vector, rotation_y);
        line[1].position.x = PartEditorCameraPos.x + vector.x;
        line[1].position.y = PartEditorCameraPos.y + vector.y;
        line[1].position.z = PartEditorCameraPos.z + vector.z;
        line[1].colour = 0xff00ff00;
        NuRndrLine3d(line, edpart_mtl, NULL);
        vector.x = 0.0f;
        vector.y = 0.6f;
        vector.z = 0.0f;
        NuVecRotateZ(&vector, &vector, rotation_z);
        NuVecRotateY(&vector, &vector, rotation_y);
        line[0].position.x = PartEditorCameraPos.x + vector.x;
        line[0].position.y = PartEditorCameraPos.y + vector.y;
        line[0].position.z = PartEditorCameraPos.z + vector.z;
        line[0].colour = 0xff00ff00;
        vector.x = 0.0f;
        vector.y = 0.55f;
        vector.z = 0.0f;
        NuVecRotateZ(&vector, &vector, rotation_z);
        NuVecRotateY(&vector, &vector, rotation_y);
        line[1].position.x = PartEditorCameraPos.x + vector.x;
        line[1].position.y = PartEditorCameraPos.y + vector.y;
        line[1].position.z = PartEditorCameraPos.z + vector.z;
        line[1].colour = 0xff00ff00;
        NuRndrLine3d(line, edpart_mtl, NULL);
        vector.x = 0.0f;
        vector.y = 0.05f;
        vector.z = 0.55f;
        NuVecRotateZ(&vector, &vector, rotation_z);
        NuVecRotateY(&vector, &vector, rotation_y);
        line[0].position.x = PartEditorCameraPos.x + vector.x;
        line[0].position.y = PartEditorCameraPos.y + vector.y;
        line[0].position.z = PartEditorCameraPos.z + vector.z;
        line[0].colour = 0xff00ff00;
        vector.x = 0.0f;
        vector.y = 0.05f;
        vector.z = 0.6f;
        NuVecRotateZ(&vector, &vector, rotation_z);
        NuVecRotateY(&vector, &vector, rotation_y);
        line[1].position.x = PartEditorCameraPos.x + vector.x;
        line[1].position.y = PartEditorCameraPos.y + vector.y;
        line[1].position.z = PartEditorCameraPos.z + vector.z;
        line[1].colour = 0xff00ff00;
        NuRndrLine3d(line, edpart_mtl, NULL);
        vector.x = 0.0f;
        vector.y = 0.05f;
        vector.z = 0.6f;
        NuVecRotateZ(&vector, &vector, rotation_z);
        NuVecRotateY(&vector, &vector, rotation_y);
        line[0].position.x = PartEditorCameraPos.x + vector.x;
        line[0].position.y = PartEditorCameraPos.y + vector.y;
        line[0].position.z = PartEditorCameraPos.z + vector.z;
        line[0].colour = 0xff00ff00;
        vector.x = 0.0f;
        vector.y = -0.05f;
        vector.z = 0.55f;
        NuVecRotateZ(&vector, &vector, rotation_z);
        NuVecRotateY(&vector, &vector, rotation_y);
        line[1].position.x = PartEditorCameraPos.x + vector.x;
        line[1].position.y = PartEditorCameraPos.y + vector.y;
        line[1].position.z = PartEditorCameraPos.z + vector.z;
        line[1].colour = 0xff00ff00;
        NuRndrLine3d(line, edpart_mtl, NULL);
        vector.x = 0.0f;
        vector.y = -0.05f;
        vector.z = 0.55f;
        NuVecRotateZ(&vector, &vector, rotation_z);
        NuVecRotateY(&vector, &vector, rotation_y);
        line[0].position.x = PartEditorCameraPos.x + vector.x;
        line[0].position.y = PartEditorCameraPos.y + vector.y;
        line[0].position.z = PartEditorCameraPos.z + vector.z;
        line[0].colour = 0xff00ff00;
        vector.x = 0.0f;
        vector.y = -0.05f;
        vector.z = 0.6f;
        NuVecRotateZ(&vector, &vector, rotation_z);
        NuVecRotateY(&vector, &vector, rotation_y);
        line[1].position.x = PartEditorCameraPos.x + vector.x;
        line[1].position.y = PartEditorCameraPos.y + vector.y;
        line[1].position.z = PartEditorCameraPos.z + vector.z;
        line[1].colour = 0xff00ff00;
        NuRndrLine3d(line, edpart_mtl, NULL);

        if (edpart_copy_mode != 0) {
            edbitsDrawCube(PartEditorCameraPos.x, PartEditorCameraPos.y, PartEditorCameraPos.z, edpart_copy_size, edpart_copy_size,
                           edpart_copy_size, 0, 0, 0, 0, 0, 0xffffffff, edpart_mtl);
        } else {
            vector.x = 0.0f;
            vector.y = 0.375f;
            vector.z = 0.0f;
            NuVecRotateZ(&vector, &vector, edpart_emitrotz);
            NuVecRotateY(&vector, &vector, edpart_emitroty);
            NuVecRotateX(&vector, &vector, edpart_emitrotx);
            NuVecRotateZ(&vector, &vector, rotation_z);
            NuVecRotateY(&vector, &vector, rotation_y);
            line[0].position.x = PartEditorCameraPos.x;
            line[0].position.y = PartEditorCameraPos.y;
            line[0].position.z = PartEditorCameraPos.z;
            line[1].position.x = PartEditorCameraPos.x + vector.x;
            line[1].position.y = PartEditorCameraPos.y + vector.y;
            line[1].position.z = PartEditorCameraPos.z + vector.z;
            line[0].colour = 0xff0000ff;
            line[1].colour = 0xff0000ff;
            NuRndrLine3d(line, edpart_mtl, NULL);
            vector.x = 0.0f;
            vector.y = 0.375f;
            vector.z = 0.125f;
            NuVecRotateZ(&vector, &vector, edpart_emitrotz);
            NuVecRotateY(&vector, &vector, edpart_emitroty);
            NuVecRotateX(&vector, &vector, edpart_emitrotx);
            NuVecRotateZ(&vector, &vector, rotation_z);
            NuVecRotateY(&vector, &vector, rotation_y);
            line[0].position.x = PartEditorCameraPos.x + vector.x;
            line[0].position.y = PartEditorCameraPos.y + vector.y;
            line[0].position.z = PartEditorCameraPos.z + vector.z;
            line[0].colour = 0xff0000ff;
            NuRndrLine3d(line, edpart_mtl, NULL);
            if (edpart_dpad_mode == 3) {
                vector.x = 0.0f;
                vector.y = 0.25f;
                vector.z = 0.0f;
                NuVecRotateZ(&vector, &vector, edpart_refrotz);
                NuVecRotateY(&vector, &vector, edpart_refroty);
                NuVecRotateZ(&vector, &vector, rotation_z);
                NuVecRotateY(&vector, &vector, rotation_y);
                line[0].position.x = PartEditorCameraPos.x;
                line[0].position.y = PartEditorCameraPos.y;
                line[0].position.z = PartEditorCameraPos.z;
                line[1].position.x = PartEditorCameraPos.x + vector.x;
                line[1].position.y = PartEditorCameraPos.y + vector.y;
                line[1].position.z = PartEditorCameraPos.z + vector.z;
                line[0].colour = 0xffff0000;
                line[1].colour = 0xffff0000;
                NuRndrLine3d(line, edpart_mtl, NULL);
                if (edpart_nearest != -1) {
                    vector.x = 0.0f;
                    vector.y = *(f32 *)(&part_emits[edpart_nearest].trailing_state_words[4]);
                    vector.z = 0.0f;
                    NuVecRotateZ(&vector, &vector, edpart_refrotz);
                    NuVecRotateY(&vector, &vector, edpart_refroty);
                    NuVecRotateZ(&vector, &vector, rotation_z);
                    NuVecRotateY(&vector, &vector, rotation_y);
                    edbitsDrawCube(PartEditorCameraPos.x + vector.x, PartEditorCameraPos.y + vector.y, PartEditorCameraPos.z + vector.z,
                                   0.25f, 0.0f, 0.25f, edpart_refrotz, edpart_refroty, 0, rotation_z, rotation_y, 0xffff0000, edpart_mtl);
                }
            }
        }

        NuRndrRect2di(0x1720, 0x9b0, 0xf00, 0x410, 0x80808080, edpart_boxmtl);
        NuRndrRect2di(0x1710, 0x8f0, 0xf20, 0xc0, 0x80000000, edpart_boxmtl);
        NuRndrLine2di(0x1710, 0x9b0, 0x1710, 0xdc8, 0x80000000, edpart_boxmtl);
        NuRndrLine2di(0x2630, 0x9b0, 0x2630, 0xdc8, 0x80000000, edpart_boxmtl);
        NuRndrLine2di(0x1710, 0xdc8, 0x2630, 0xdc8, 0x80000000, edpart_boxmtl);
        NuQFntPushPrintMode(NUQFNT_MODE_2D);
        NuQFntPushCoordinateSystem(NUQFNT_CSMODE_PS2);
        NuQFntSet(system_qfont);
        NuQFntSetColour(system_qfont, 0x80808080);
        NuQFntPrintEx(system_qfont, 0x17c0, 0x988, 0x10, "Info Box");
        if (edpart_readout == 0) {
            if (edpart_num_orphans > 0) {
                NuQFntSetColour(system_qfont, 0x80202080);
                NuQFntPrintEx(system_qfont, 0x17c0, 0x988, 0x10, "WARNING: %d Orphans", edpart_num_orphans);
            } else {
                NuQFntSetColour(system_qfont, 0x80808080);
                NuQFntPrintEx(system_qfont, 0x17c0, 0x988, 0x10, "Info Box");
            }
        } else if (edpart_readout == 1) {
            NuQFntSetColour(system_qfont, 0x80808080);
            NuQFntPrintEx(system_qfont, 0x17c0, 0x988, 0x10, "Co-ordinates");
        }
        NuQFntSetColour(system_qfont, 0x80000000);
        if (edpart_readout == 0) {
            if (edpart_copy_mode == 0) {
                if (edpart_create_type == -1) {
                    NuQFntPrintEx(system_qfont, 0x17c0, 0xa50, 0x10, "Current Type: <none>");
                } else {
                    NuQFntPrintEx(system_qfont, 0x17c0, 0xa50, 0x10, "Current Type: %s", part_types[edpart_create_type].name);
                }
                if (edpart_create_type == -1) {
                    NuQFntPrintEx(system_qfont, 0x17c0, 0xaf0, 0x10, "Current List: <none>");
                } else if (edpart_effect_list == 1) {
                    NuQFntPrintEx(system_qfont, 0x17c0, 0xaf0, 0x10, "Current List: Level");
                } else if (edpart_effect_list == 0) {
                    NuQFntPrintEx(system_qfont, 0x17c0, 0xaf0, 0x10, "Current List: General");
                }
            }
            if (edpart_copy_mode != 0) {
                if (edpart_copy_enclosed > 8) {
                    NuQFntSetColour(system_qfont, 0x80000080);
                }
                NuQFntPrintEx(system_qfont, 0x17c0, 0xc30, 0x10, "Enclosed: %d", edpart_copy_enclosed);
                NuQFntSetColour(system_qfont, 0x80000000);
            } else {
                if (edpart_nearest == -1) {
                    NuQFntPrintEx(system_qfont, 0x17c0, 0xc30, 0x10, "Highlight: <none>");
                } else {
                    edbitsDrawCube(PartEditorCameraPos.x, PartEditorCameraPos.y, PartEditorCameraPos.z,
                                   edpart_nearest_type->position_random.x, edpart_nearest_type->position_random.y,
                                   edpart_nearest_type->position_random.z, edpart_emitrotz, edpart_emitroty, edpart_emitrotx, rotation_z,
                                   rotation_y, 0xff0000ff, edpart_mtl);
                    NuQFntSet(system_qfont);
                    NuQFntSetColour(system_qfont, 0x80000000);
                    NuQFntPrintEx(system_qfont, 0x17c0, 0xc30, 0x10, "Highlight: %s", edpart_nearest_type->name);
                    if (edpart_nearest_orphans == 0 && edpart_nearest_duplicates == 0) {
                        NuQFntPrintEx(system_qfont, 0x1860, 0xcd0, 0x10, "Instances: %d of %d", edpart_nearest_type->variant_count, 8);
                    } else {
                        NuQFntSetColour(system_qfont, 0x80000080);
                        NuQFntPrintEx(system_qfont, 0x1860, 0xcd0, 0x10, "Instances: %d of %d (Or = %d, Du = %d)",
                                      edpart_nearest_type->variant_count, 8, edpart_nearest_orphans, edpart_nearest_duplicates);
                        NuQFntSetColour(system_qfont, 0x80000000);
                    }
                }
            }
            NuQFntPrintEx(system_qfont, 0x17c0, 0xd70, 0x10, "Types: %d/%d; Emits: %d/%d", part_types_used, 128, part_emits_used, 40);
        } else if (edpart_readout == 1) {
            if (edpart_nearest == -1) {
                NuQFntPrintEx(system_qfont, 0x17c0, 0xa50, 0x10, "Highlight: <none>");
            } else {
                NuQFntPrintEx(system_qfont, 0x17c0, 0xa50, 0x10, "Highlight: %s", edpart_nearest_type->name);
                NuQFntPrintEx(system_qfont, 0x17c0, 0xaf0, 0x10, "XYZ: %0.2f %0.2f %0.2f", edpart_nearest_emit->position.x,
                              edpart_nearest_emit->position.y, edpart_nearest_emit->position.z);
                NuQFntPrintEx(system_qfont, 0x17c0, 0xb90, 0x10, "RotZ: %d",
                              ((const i16 *)&edpart_nearest_emit->trailing_state_words[0])[0]);
                NuQFntPrintEx(system_qfont, 0x17c0, 0xc30, 0x10, "RotY: %d",
                              ((const i16 *)&edpart_nearest_emit->trailing_state_words[0])[1]);
                NuQFntPrintEx(system_qfont, 0x17c0, 0xcd0, 0x10, "EmitRotZ: %d", edpart_nearest_emit->rotation_2c);
                NuQFntPrintEx(system_qfont, 0x17c0, 0xd70, 0x10, "EmitRotY: %d", edpart_nearest_emit->rotation_2e);
            }
        }
        NuQFntPopPrintMode();
        NuQFntPopCoordinateSystem();
    }

    void edpartDetermineNearest(f32 distance) {
        NUVEC delta;
        f32 squared_distance;
        i32 i;
        if (edpart_nearest != -1 && part_emits[edpart_nearest].effect_id != -1) {
            NuVecSub(&delta, &PartEditorCameraPos, &part_emits[edpart_nearest].position);
            squared_distance = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;
            if (squared_distance == 0.0f) {
                return;
            }
        }
        edpart_nearest = -1;
        for (i = 0; i < 40; ++i) {
            if (part_emits[i].effect_id != -1) {
                NuVecSub(&delta, &PartEditorCameraPos, &part_emits[i].position);
                squared_distance = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;
                if (distance < 0.0f || squared_distance < distance) {
                    distance = squared_distance;
                    edpart_nearest = i;
                }
            }
        }
        edpart_nearest_emit = NULL;
        edpart_nearest_type = NULL;
        if (edpart_nearest != -1) {
            edpart_nearest_emit = &part_emits[edpart_nearest];
            if (edpart_nearest_emit->effect_id != -1) {
                edpart_nearest_type = &part_types[edpart_nearest_emit->effect_id];
            }
        }
    }

    void edpartHighlightNearest(void) {
        i32 index;
        NUVEC delta;
        NUVEC offset;
        NUVEC destination;

        union {
            f32 value;
            u32 bits;
        } temp;

        union {
            f32 value[3];
            u32 bits[3];
        } abs_delta;

        if (edpart_copy_mode != 0) {
            edpart_copy_enclosed = 0;
            for (index = 0; index < 512; ++index) {
                if (part_emits[index].instance_id == -1 || part_emits[index].instance_id == 99999) {
                    continue;
                }
                NuVecSub(&delta, &PartEditorCameraPos, &part_emits[index].position);
                temp.value = delta.x;
                abs_delta.bits[0] = temp.bits & 0x7fffffff;
                if (!(abs_delta.value[0] <= edpart_copy_size)) {
                    continue;
                }
                temp.value = delta.y;
                abs_delta.bits[1] = temp.bits & 0x7fffffff;
                if (!(abs_delta.value[1] <= edpart_copy_size)) {
                    continue;
                }
                temp.value = delta.z;
                abs_delta.bits[2] = temp.bits & 0x7fffffff;
                if (!(abs_delta.value[2] <= edpart_copy_size)) {
                    continue;
                }
                ++edpart_copy_enclosed;
                edbitsDrawDiagonalCross(part_emits[index].position.x, part_emits[index].position.y, part_emits[index].position.z, 0.05f, -1,
                                        edpart_mtl);
            }
            if (edpart_copy_source_count > 0) {
                for (index = 0; index < edpart_copy_source_count; ++index) {
                    NuVecSub(&offset, &part_emits[edpart_copy_source[index]].position, &edpart_copy_source_vec);
                    NuVecRotateY(&offset, &offset, edpart_copyroty);
                    NuVecAdd(&destination, &PartEditorCameraPos, &offset);
                    edbitsDrawDiagonalCross(destination.x, destination.y, destination.z, 0.05f, 0xff808080, edpart_mtl);
                }
            }
        } else if (edpart_nearest != -1) {
            edbitsDrawCube(part_emits[edpart_nearest].position.x, part_emits[edpart_nearest].position.y,
                           part_emits[edpart_nearest].position.z, 0.5f, 0.5f, 0.5f, 0, 0, 0, 0, 0, -1, edpart_mtl);
        }
    }
    struct eduimenu_s *edpart_active_menu = 0;

    void edpartRender() {
        edcamSet();
        edpartDrawCursor();
        edpartHighlightNearest();
        if (edpart_active_menu) {
            eduiMenuRender(edpart_active_menu);
        }
    }

    void edpartChangeName(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
        if (edpart_create_type != -1) {
            i32 index;
            char *name = ((edui_textpicker_s *)(item))->value;
            NuStrNCpy(part_types[edpart_create_type].name, name, sizeof(part_types[edpart_create_type].name));
            for (index = 0; index < 40; ++index) {
                if (part_emits[index].effect_id == edpart_create_type) {
                    NuStrNCpy(part_emits[index].name, name, sizeof(part_emits[index].name));
                }
            }
        }
    }
    struct eduimenu_s *edpart_name_menu = 0;

    void edpartCancelChangeNameMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
        eduiMenuDestroy(edpart_name_menu);
        edpart_name_menu = NULL;
    }

    void edpartChangeNameMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
        if (edpart_create_type != -1) {
            PARTTYPE_LOOKUP *type = &part_types[edpart_create_type];
            edpart_name_menu = eduiMenuCreate(70, 70, 180, 250, ed_fnt, edpartCancelChangeNameMenu, "Type Name");
            if (edpart_name_menu != NULL) {
                eduiMenuAddItem(edpart_name_menu, eduiItemTextPickCreate(0, edblack, edpartChangeName, "Name: "));
                strcpy(((edui_textpicker_s *)(edui_last_item))->value, type->name);
                ((edui_textpicker_s *)(edui_last_item))->max_length = 15;
                eduiMenuAttach(menu, edpart_name_menu);
                edpart_name_menu->x = menu->x + 10;
                edpart_name_menu->y = menu->y + 40;
            }
        }
    }

    void edpartInitType(i32 index) {
        i32 i;
        for (i = 0; i < 8; ++i) {
            part_types[index].effect_ids[i] = -1;
            part_types[index].effect_pages[i] = 1;
        }
        part_types[index].variant_count = 0;
        part_types[index].lifetime = 0.0f;
        part_types[index].speed = 1.0f;
        part_types[index].gravity = 0.0f;
        part_types[index].emission_rate = 1.0f;
        part_types[index].emission_period = 1.0f;
        part_types[index].emission_period_random = 0.0f;
        part_types[index].emission_pause = 0.0f;
        part_types[index].emission_pause_random = 0.0f;
        part_types[index].rotation[0] = 0;
        part_types[index].rotation[1] = 0;
        part_types[index].rotation[2] = 0;
        part_types[index].flags = 0;
        part_types[index].trail_effects[0] = -1;
        part_types[index].trail_effects[1] = -1;
        part_types[index].attached_effect = -1;
        part_types[index].kill_effect = -1;
        part_types[index].impact_effect = -1;
        part_types[index].impact_part = -1;
        part_types[index].last_used_time = 0.0f;
        part_types[index].source_type = -1;
        part_types[index].scale = 1.0f;
        for (i = 0; i < 4; ++i) {
            part_types[index].sounds[i] = -1;
            part_types[index].sound_modes[i] = 0;
        }
    }

    void edpartCopyType(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
        struct eduimenu_s *parent;
        i32 index;
        parent = NULL;
        index = 0;
        if (part_types_used < 128 && edpart_create_type != -1) {
            for (; index < 128; ++index) {
                if (part_types[index].name[0] == '\0') {
                    u32 name_length;
                    PARTTYPE_LOOKUP *source = &part_types[edpart_create_type];
                    PARTTYPE_LOOKUP *copy = &part_types[index];
                    *copy = *source;
                    name_length = strlen(source->name);
                    if (name_length <= 12) {
                        sprintf(copy->name, "%s%03d", source->name, index);
                    } else {
                        char name[16];
                        sprintf(name, "%s", source->name);
                        name[12] = '\0';
                        sprintf(copy->name, "%s%03d", name, index);
                    }
                    ++part_types_used;
                    edpart_create_type = index;
                    break;
                }
            }
        }
        if (menu->parent) {
            parent = menu->parent;
            eduiMenuDetach(menu);
        }
        if (menu->callback) {
            menu->callback(menu, parent);
        }
    }

    void edpartChangeCutOff(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
        if (edpart_nearest_type) {
            edpart_nearest_type->maximum_distance = ((edui_slider_s *)(item))->value;
        }
    }

    void edpartChangeGenRate(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
        edui_slider_s *slider = (edui_slider_s *)item;
        if (edpart_nearest_type) {
            edpart_nearest_type->emission_rate = (f32)(i32)slider->value;
        }
    }
    struct eduimenu_s *edpart_changegenrate_menu = 0;

    void edpartCancelChangeGenRateMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
        eduiMenuDestroy(edpart_changegenrate_menu);
        edpart_changegenrate_menu = NULL;
    }

    i32 edpart_superscale = 1;

    void edpartChangeGenRateMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
        if (edpart_nearest_type == NULL) {
            return;
        }
        edpart_changegenrate_menu = eduiMenuCreate(70, 70, 250, 250, ed_fnt, edpartCancelChangeGenRateMenu, "Emits per Second");
        if (edpart_changegenrate_menu == NULL) {
            return;
        }
        eduiMenuAddItem(edpart_changegenrate_menu,
                        eduiItemSliderCreateInt(0, edblack, 0, edpartChangeGenRate, 0, (i32)(edpart_superscale * 1200.0),
                                                (i32)edpart_nearest_type->emission_rate, "Emits per Second"));
        eduiMenuAttach(menu, edpart_changegenrate_menu);
        edpart_changegenrate_menu->x = menu->x + 10;
        edpart_changegenrate_menu->y = menu->y + 40;
    }

    void edpartChangeMaxLife(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
        if (edpart_nearest_type) {
            edpart_nearest_type->lifetime = ((edui_slider_s *)item)->value;
        }
    }

    void edpartChangeRanMaxLife(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
        if (edpart_nearest_type) {
            edpart_nearest_type->lifetime_random = ((edui_slider_s *)item)->value;
        }
    }
    struct eduimenu_s *edpart_maxlife_menu = 0;

    void edpartCancelChangeMaxLifeMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
        eduiMenuDestroy(edpart_maxlife_menu);
        edpart_maxlife_menu = NULL;
    }

    void edpartChangeMaxLifeMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
        if (edpart_nearest_type == NULL) {
            return;
        }
        edpart_maxlife_menu = eduiMenuCreate(70, 70, 250, 250, ed_fnt, edpartCancelChangeMaxLifeMenu, "Max Instance Life");
        if (edpart_maxlife_menu == NULL) {
            return;
        }
        eduiMenuAddItem(edpart_maxlife_menu, eduiItemSliderCreate(0, edblack, 0, edpartChangeMaxLife, 0.0f, edpart_superscale * 10.0,
                                                                  edpart_nearest_type->lifetime, "Base Max Life"));
        eduiMenuAddItem(edpart_maxlife_menu, eduiItemSliderCreate(0, edblack, 0, edpartChangeRanMaxLife, 0.0f, edpart_superscale * 10.0,
                                                                  edpart_nearest_type->lifetime_random, "Random Max Life"));
        eduiMenuAttach(menu, edpart_maxlife_menu);
        edpart_maxlife_menu->x = menu->x + 10;
        edpart_maxlife_menu->y = menu->y + 40;
    }

    void edpartChangeTintR(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
        if (edpart_nearest_type) {
            edpart_nearest_type->lighting_scale.x = ((edui_slider_s *)item)->value;
        }
    }

    void edpartChangeTintG(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
        if (edpart_nearest_type) {
            edpart_nearest_type->lighting_scale.y = ((edui_slider_s *)item)->value;
        }
    }

    void edpartChangeTintB(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
        if (edpart_nearest_type) {
            edpart_nearest_type->lighting_scale.z = ((edui_slider_s *)item)->value;
        }
    }
    struct eduimenu_s *edpart_tint_menu = 0;

    void edpartCancelTintMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
        eduiMenuDestroy(edpart_tint_menu);
        edpart_tint_menu = NULL;
    }

    void edpartTintMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
        if (edpart_nearest_type != NULL) {
            edpart_tint_menu = eduiMenuCreate(70, 70, 250, 250, ed_fnt, edpartCancelTintMenu, "Instance Tint");
            if (edpart_tint_menu != NULL) {
                eduiMenuAddItem(edpart_tint_menu, eduiItemSliderCreate(0, edblack, 0, edpartChangeTintR, 0.0f, 2.0f,
                                                                       edpart_nearest_type->lighting_scale.x, "Red Tint"));
                eduiMenuAddItem(edpart_tint_menu, eduiItemSliderCreate(0, edblack, 0, edpartChangeTintG, 0.0f, 2.0f,
                                                                       edpart_nearest_type->lighting_scale.y, "Green Tint"));
                eduiMenuAddItem(edpart_tint_menu, eduiItemSliderCreate(0, edblack, 0, edpartChangeTintB, 0.0f, 2.0f,
                                                                       edpart_nearest_type->lighting_scale.z, "Blue Tint"));
                eduiMenuAttach(menu, edpart_tint_menu);
                edpart_tint_menu->x = menu->x + 10;
                edpart_tint_menu->y = menu->y + 40;
            }
        }
    }

    void edpartChangeInstanceScale(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
        if (edpart_nearest_type) {
            edpart_nearest_type->particle_scale = ((edui_slider_s *)item)->value;
        }
    }
    struct eduimenu_s *edpart_instancescale_menu = 0;

    void edpartCancelInstanceScaleMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
        eduiMenuDestroy(edpart_instancescale_menu);
        edpart_instancescale_menu = NULL;
    }

    void edpartInstanceScaleMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
        edpart_instancescale_menu = eduiMenuCreate(70, 70, 180, 300, ed_fnt, edpartCancelInstanceScaleMenu, "Instance Scale");
        if (edpart_instancescale_menu != NULL && edpart_nearest_type != NULL) {
            eduiMenuAddItem(edpart_instancescale_menu,
                            eduiItemSliderCreate(0, edblack, 0, edpartChangeInstanceScale, 0.0f, edpart_superscale * 10.0f,
                                                 edpart_nearest_type->particle_scale, "Instance Scale"));
            eduiMenuAttach(menu, edpart_instancescale_menu);
            edpart_instancescale_menu->x = menu->x + 10;
            edpart_instancescale_menu->y = menu->y + 40;
        }
    }
    struct eduimenu_s *edpart_type_menu = 0;

    void edpartSelType(eduimenu_s *menu, eduiitem_s *item, u32 unused2) {
        edpart_type_menu = NULL;
        edpart_active_menu = NULL;
        edpart_create_type = item->data;
        edpart_effect_list = part_types[edpart_create_type].effect_list;
        eduiMenuDetach(menu);
        eduiMenuDestroy(menu);
    }

    eduimenu_s *edpart_message_menu = NULL;

    static void edpartCancelMessageMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
        eduiMenuDestroy(edpart_message_menu);
        edpart_message_menu = NULL;
    }

    static void edpartSaveMessage(eduimenu_s *parent, char *message, i32 status) {
        u32 colors[4] = {0x800000c0u, 0x80ff0000u, 0x80808080u, 0x80404040u};
        if (status == 1) {
            colors[0] = 0x8000c000u;
        }
        edpart_message_menu = eduiMenuCreate(70, 70, 300, 250, ed_fnt, edpartCancelMessageMenu, "Message");
        if (edpart_message_menu != NULL) {
            eduiMenuAddItem(edpart_message_menu, eduiItemSelCreate(1, colors, 0, 0, NULL, message));
            eduiMenuAttach(parent, edpart_message_menu);
            edpart_message_menu->x = parent->x + 10;
            edpart_message_menu->y = parent->y + 40;
        }
    }
    struct eduimenu_s *edpart_switchtype_menu = 0;

    void edpartSetSwitchType(eduimenu_s *menu, eduiitem_s *item, u32 unused2) {
        edpart_switchtype_menu = NULL;
        if (edpart_nearest_emit != NULL) {
            edpart_nearest_emit->trigger_type = item->data;
            if (edpart_nearest_emit->trigger_type == 0) {
                edpart_nearest_emit->state = 1;
            }
        }
        eduiMenuDetach(menu);
        eduiMenuDestroy(menu);
    }

    void edpartSetSwitchId(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
        if (edpart_nearest_emit != NULL) {
            edpart_nearest_emit->trigger_id = (i16)((edui_slider_s *)item)->value;
        }
    }

    void edpartCancelSwitchTypeMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
        eduiMenuDestroy(edpart_switchtype_menu);
        edpart_switchtype_menu = NULL;
    }

    void edpartSwitchTypeMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
        edpart_switchtype_menu = eduiMenuCreate(70, 70, 180, 250, ed_fnt, edpartCancelSwitchTypeMenu, "Switch Type");
        if (edpart_switchtype_menu != NULL) {
            eduiMenuAddItem(edpart_switchtype_menu,
                            eduiItemCheckCreate(0, edblack, part_emits[edpart_nearest].trigger_type == 0, 1, edpartSetSwitchType, "None"));
            eduiMenuAddItem(edpart_switchtype_menu, eduiItemCheckCreate(1, edblack, part_emits[edpart_nearest].trigger_type == 1, 1,
                                                                        edpartSetSwitchType, "Global Switch"));
            eduiMenuAttach(menu, edpart_switchtype_menu);
            edpart_switchtype_menu->x = menu->x + 10;
            edpart_switchtype_menu->y = menu->y + 40;
        }
    }
    struct eduimenu_s *edpart_switch_menu = 0;

    void edpartCancelSwitchMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
        eduiMenuDestroy(edpart_switch_menu);
        edpart_switch_menu = NULL;
    }

    void edpartSwitchMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
        if (edpart_nearest != -1 && part_emits[edpart_nearest].instance_id != -1) {
            edpart_switch_menu = eduiMenuCreate(70, 70, 180, 250, ed_fnt, edpartCancelSwitchMenu, "Switch Menu");
            if (edpart_switch_menu != NULL) {
                eduiMenuAddItem(edpart_switch_menu, eduiItemSelCreate(1, edblack, 0, 0, edpartSwitchTypeMenu, "Switch Type..."));
                eduiMenuAddItem(edpart_switch_menu, eduiItemSliderCreateInt(0, edblack, 0, edpartSetSwitchId, 0, 128,
                                                                            part_emits[edpart_nearest].trigger_id, "Switch ID"));
                eduiMenuAttach(menu, edpart_switch_menu);
                edpart_switch_menu->x = menu->x + 10;
                edpart_switch_menu->y = menu->y + 40;
            }
        }
    }

    f32 edpart_scale_factor = 1.0f;

    void edpartSetScaleFactor(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
        edpart_scale_factor = ((edui_slider_s *)(item))->value;
    }

    void edpartScaleType(i32 index, f32 scale) {
        PARTTYPE_LOOKUP *type = &part_types[index];
        type->particle_scale *= scale;
        type->effect_scale *= scale;
        type->speed *= scale;
        type->gravity *= scale;
        type->position_random.x *= scale;
        type->position_random.y *= scale;
        type->position_random.z *= scale;
        type->velocity_random.x *= scale;
        type->velocity_random.y *= scale;
        type->velocity_random.z *= scale;
        type->maximum_distance *= scale;
    }

    void edpartApplyScaleType(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
        struct eduimenu_s *parent;
        parent = NULL;
        if (edpart_nearest_type) {
            edpartScaleType(edpart_nearest_emit->effect_id, edpart_scale_factor);
        }
        if (menu->parent) {
            parent = menu->parent;
            eduiMenuDetach(menu);
        }
        if (menu->callback) {
            menu->callback(menu, parent);
        }
    }
    struct eduimenu_s *edpart_scaletype_menu = 0;

    void edpartCancelScaleTypeMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
        eduiMenuDestroy(edpart_scaletype_menu);
        edpart_scaletype_menu = NULL;
    }

    void edpartScaleTypeMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
        if (edpart_nearest != -1) {
            edpart_scaletype_menu = eduiMenuCreate(70, 70, 180, 250, ed_fnt, edpartCancelScaleTypeMenu, "Scale Type");
            if (edpart_scaletype_menu != NULL) {
                eduiMenuAddItem(edpart_scaletype_menu,
                                eduiItemSliderCreate(0, edblack, 0, edpartSetScaleFactor, 0.1f, 9.9f, edpart_scale_factor, "Scale Factor"));
                eduiMenuAddItem(edpart_scaletype_menu, eduiItemSelCreate(1, edblack, 0, 0, edpartApplyScaleType, "Apply Scale to Type"));
                eduiMenuAttach(menu, edpart_scaletype_menu);
                edpart_scaletype_menu->x = menu->x + 10;
                edpart_scaletype_menu->y = menu->y + 40;
            }
        }
    }

    i32 part_scene_pageid[32] = {0};
    i32 part_page_used[8] = {0};
    NUGSCN *part_scene[32] = {0};
    i32 part_page_on[8] = {0};

    void edpartAddGeneralType(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
        struct eduimenu_s *parent = NULL;
        i32 index;
        i32 i;
        if (part_types_used < 128) {
            for (index = 0; index < 128; ++index) {
                if (part_types[index].name[0] == 0) {
                    sprintf(part_types[index].name, "New%d", index);
                    edpartInitType(index);
                    ++part_types_used;
                    part_scene[0] = edbits_things_scene;
                    part_types[index].effect_list = 0;
                    part_types[index].page = 0;
                    part_page_on[0] = 1;
                    part_page_used[0] = 1;
                    part_scene_pageid[0] = 0;
                    edpart_create_type = index;
                    for (i = 0; i < 8; ++i) {
                        part_types[index].effect_ids[i] = -1;
                        part_types[index].effect_pages[i] = -1;
                    }
                    part_types[index].lighting_scale.x = 1.0f;
                    part_types[index].lighting_scale.y = 1.0f;
                    part_types[index].lighting_scale.z = 1.0f;
                    break;
                }
            }
        }
        if (menu->parent) {
            parent = menu->parent;
            eduiMenuDetach(menu);
        }
        if (menu->callback) {
            menu->callback(menu, parent);
        }
    }
    struct eduimenu_s *edpart_generaltype_menu = 0;

    void edpartCancelGeneralTypeMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
        eduiMenuDestroy(edpart_generaltype_menu);
        edpart_generaltype_menu = NULL;
    }

    void edpartGeneralTypeMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
        i32 index;
        i32 selected;
        edpart_generaltype_menu = eduiMenuCreate(70, 70, 250, 250, ed_fnt, edpartCancelGeneralTypeMenu, "Emitter Type (General)");
        if (edpart_generaltype_menu == NULL) {
            return;
        }
        if (part_types_used < 128) {
            eduiMenuAddItem(edpart_generaltype_menu, eduiItemSelCreate(1, edblack, 0, 0, edpartAddGeneralType, "Add Type"));
        } else {
            eduiMenuAddItem(edpart_generaltype_menu, eduiItemSelCreate(1, edgrey, 0, 0, NULL, "Add Type"));
        }
        for (index = 0; index < 128; ++index) {
            if (part_types[index].name[0] != 0 && part_types[index].effect_list == 0) {
                selected = edpart_create_type == index;
                eduiMenuAddItem(edpart_generaltype_menu,
                                eduiItemCheckCreate(index, edblack, selected, 1, edpartSelType, part_types[index].name));
                if (selected == 1) {
                    edpart_generaltype_menu->selected = edui_last_item;
                }
            }
        }
        eduiMenuAttach(menu, edpart_generaltype_menu);
        edpart_generaltype_menu->x = menu->x + 10;
        edpart_generaltype_menu->y = menu->y + 40;
    }

    void edpartAddLevelType(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
        struct eduimenu_s *parent = NULL;
        i32 index;
        i32 i;
        if (part_types_used < 128) {
            for (index = 0; index < 128; ++index) {
                if (part_types[index].name[0] == 0) {
                    sprintf(part_types[index].name, "New%d", index);
                    edpartInitType(index);
                    ++part_types_used;
                    part_scene[1] = edbits_base_scene;
                    part_types[index].effect_list = 1;
                    part_types[index].page = 1;
                    part_page_on[1] = 1;
                    part_page_used[1] = 1;
                    part_scene_pageid[1] = 1;
                    edpart_create_type = index;
                    for (i = 0; i < 8; ++i) {
                        part_types[index].effect_ids[i] = -1;
                        part_types[index].effect_pages[i] = -1;
                    }
                    part_types[index].lighting_scale.x = 1.0f;
                    part_types[index].lighting_scale.y = 1.0f;
                    part_types[index].lighting_scale.z = 1.0f;
                    break;
                }
            }
        }
        if (menu->parent) {
            parent = menu->parent;
            eduiMenuDetach(menu);
        }
        if (menu->callback) {
            menu->callback(menu, parent);
        }
    }
    struct eduimenu_s *edpart_leveltype_menu = 0;

    void edpartCancelLevelTypeMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
        eduiMenuDestroy(edpart_leveltype_menu);
        edpart_leveltype_menu = NULL;
    }

    void edpartLevelTypeMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
        i32 index;
        i32 selected;
        edpart_leveltype_menu = eduiMenuCreate(70, 70, 250, 250, ed_fnt, edpartCancelLevelTypeMenu, "Emitter Type (Level)");
        if (edpart_leveltype_menu == NULL) {
            return;
        }
        if (part_types_used < 128) {
            eduiMenuAddItem(edpart_leveltype_menu, eduiItemSelCreate(1, edblack, 0, 0, edpartAddLevelType, "Add Type"));
        } else {
            eduiMenuAddItem(edpart_leveltype_menu, eduiItemSelCreate(1, edgrey, 0, 0, NULL, "Add Type"));
        }
        for (index = 0; index < 128; ++index) {
            if (part_types[index].name[0] != 0 && part_types[index].effect_list == 1) {
                selected = edpart_create_type == index;
                eduiMenuAddItem(edpart_leveltype_menu,
                                eduiItemCheckCreate(index, edblack, selected, 1, edpartSelType, part_types[index].name));
                if (selected == 1) {
                    edpart_leveltype_menu->selected = edui_last_item;
                }
            }
        }
        eduiMenuAttach(menu, edpart_leveltype_menu);
        edpart_leveltype_menu->x = menu->x + 10;
        edpart_leveltype_menu->y = menu->y + 40;
    }

    void edpartCancelTypeMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
        eduiMenuDestroy(edpart_type_menu);
        edpart_type_menu = NULL;
    }

    void edpartTypeMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
        edpart_type_menu = eduiMenuCreate(70, 70, 250, 250, ed_fnt, edpartCancelTypeMenu, "Emitter Type");
        if (edpart_type_menu != NULL) {
            eduiMenuAddItem(edpart_type_menu, eduiItemSelCreate(1, edblack, 0, 0, edpartGeneralTypeMenu, "General List..."));
            eduiMenuAddItem(edpart_type_menu, eduiItemSelCreate(1, edblack, 0, 0, edpartLevelTypeMenu, "Level List..."));
            eduiMenuAttach(menu, edpart_type_menu);
            edpart_type_menu->x = menu->x + 10;
            edpart_type_menu->y = menu->y + 40;
        }
    }
    static struct eduiitem_s *edpart_nullobject_highlight = 0;
    i32 edpart_which_scene = 1;

    void edpartSetInstanceType(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
        i32 index;
        i32 slot;
        if (edpart_nearest_type == NULL) {
            return;
        }
        if (item->data == 9999) {
            for (index = 0; index < 8; ++index) {
                edpart_nearest_type->effect_ids[index] = -1;
                edpart_nearest_type->effect_pages[index] = 1;
            }
            if (item->highlighted) {
                edpart_nearest_type->effect_ids[0] = 9999;
            }
            edpart_nearest_type->flags |= PART_FLAG_KEEP_OFF_SCREEN;
            edpart_nearest_type->variant_count = 0;
            return;
        }
        edpart_nullobject_highlight->highlighted = 0;
        if (item->highlighted) {
            if (edpart_nearest_type->variant_count < 8) {
                edpart_nearest_type->effect_ids[edpart_nearest_type->variant_count] = item->data;
                edpart_nearest_type->effect_pages[edpart_nearest_type->variant_count] = edpart_which_scene;
                ++edpart_nearest_type->variant_count;
            } else {
                item->highlighted = 0;
            }
        } else {
            for (index = 0; index < 8; ++index) {
                if (edpart_nearest_type->effect_ids[index] == item->data) {
                    for (slot = index; slot < 7; ++slot) {
                        edpart_nearest_type->effect_ids[slot] = edpart_nearest_type->effect_ids[slot + 1];
                        edpart_nearest_type->effect_pages[slot] = edpart_nearest_type->effect_pages[slot + 1];
                    }
                    edpart_nearest_type->effect_ids[7] = -1;
                    edpart_nearest_type->effect_pages[7] = -1;
                    --edpart_nearest_type->variant_count;
                    return;
                }
            }
        }
    }
    struct eduimenu_s *edpart_worldinstance_menu = 0;

    void edpartCancelWorldInstanceMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
        eduiMenuDestroy(edpart_worldinstance_menu);
        edpart_worldinstance_menu = NULL;
    }

    char edpart_filter_string[16] = "PART";
    i32 edpart_filter = 0;

    void edpartWorldInstanceMenu(eduimenu_s *parent, eduiitem_s *unused1, u32 unused2) {
        i32 group;
        i32 selected_first;
        i32 count;
        i32 index;
        selected_first = 0;
        edpart_which_scene = 0;
        edpart_worldinstance_menu = eduiMenuCreate(70, 70, 250, 250, ed_fnt, edpartCancelWorldInstanceMenu,
                                                   (char *)(edpart_filter ? "World Scene (Filtered)" : "World Scene"));
        if (edpart_worldinstance_menu == NULL || edbits_base_scene == NULL || edpart_nearest_type == NULL) {
            return;
        }
        group = 1;
        count = NuGScnNumSpecials(edbits_base_scene);
        for (index = 0; index < count; ++index) {
            char *name;
            i32 selected;
            i32 included;
            i32 variant;
            nuhspecial_s special;
            selected = 0;
            included = 0;
            name = NULL;
            NuGScnGetSpecial(&special, edbits_base_scene, index);
            if (NuSpecialExistsFn(&special)) {
                name = NuSpecialGetName(&special);
            }
            for (variant = 0; variant < edpart_nearest_type->variant_count; ++variant) {
                if (edpart_nearest_type->effect_ids[variant] == index && edpart_nearest_type->effect_pages[variant] == 0) {
                    selected = 1;
                    included = 1;
                }
            }
            if (!edpart_filter || NuStrNCmp(edpart_filter_string, name, NuStrLen(edpart_filter_string)) == 0 || included) {
                eduiMenuAddItem(edpart_worldinstance_menu,
                                eduiItemToggleCreate(index, edblack, selected, group, edpartSetInstanceType, name));
                group++;
                if (selected && !selected_first) {
                    selected_first = 1;
                    edpart_worldinstance_menu->selected = edui_last_item;
                }
            }
        }
        if (group == 1) {
            eduiMenuAddItem(edpart_worldinstance_menu, eduiItemSelCreate(1, edgrey, 0, 0, NULL, "All Instances Filtered"));
        }
        eduiMenuAttach(parent, edpart_worldinstance_menu);
        edpart_worldinstance_menu->x = parent->x + 10;
        edpart_worldinstance_menu->y = parent->y + 40;
    }
    struct eduimenu_s *edpart_thingsinstance_menu = 0;

    void edpartCancelThingsInstanceMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
        eduiMenuDestroy(edpart_thingsinstance_menu);
        edpart_thingsinstance_menu = NULL;
    }

    void edpartThingsInstanceMenu(eduimenu_s *parent, eduiitem_s *unused1, u32 unused2) {
        i32 group;
        i32 selected_first;
        i32 count;
        i32 index;
        selected_first = 0;
        edpart_which_scene = 1;
        edpart_thingsinstance_menu = eduiMenuCreate(70, 70, 250, 250, ed_fnt, edpartCancelThingsInstanceMenu,
                                                    (char *)(edpart_filter ? "Things Scene (Filtered)" : "Things Scene"));
        if (edpart_thingsinstance_menu == NULL || edbits_things_scene == NULL || edpart_nearest_type == NULL) {
            return;
        }
        group = 1;
        count = NuGScnNumSpecials(edbits_things_scene);
        for (index = 0; index < count; ++index) {
            char *name;
            i32 selected;
            i32 included;
            i32 variant;
            nuhspecial_s special;
            selected = 0;
            included = 0;
            name = NULL;
            NuGScnGetSpecial(&special, edbits_things_scene, index);
            if (NuSpecialExistsFn(&special)) {
                name = NuSpecialGetName(&special);
            }
            for (variant = 0; variant < edpart_nearest_type->variant_count; ++variant) {
                if (edpart_nearest_type->effect_ids[variant] == index && edpart_nearest_type->effect_pages[variant] == 1) {
                    selected = 1;
                    included = 1;
                }
            }
            if (!edpart_filter || NuStrNCmp(edpart_filter_string, name, NuStrLen(edpart_filter_string)) == 0 || included) {
                eduiMenuAddItem(edpart_thingsinstance_menu,
                                eduiItemToggleCreate(index, edblack, selected, group, edpartSetInstanceType, name));
                group++;
                if (selected && !selected_first) {
                    selected_first = 1;
                    edpart_thingsinstance_menu->selected = edui_last_item;
                }
            }
        }
        if (group == 1) {
            eduiMenuAddItem(edpart_thingsinstance_menu, eduiItemSelCreate(1, edgrey, 0, 0, NULL, "All Instances Filtered"));
        }
        eduiMenuAttach(parent, edpart_thingsinstance_menu);
        edpart_thingsinstance_menu->x = parent->x + 10;
        edpart_thingsinstance_menu->y = parent->y + 40;
    }

    static void edpartDeleteInstanceOrphan(eduimenu_s *menu, eduiitem_s *item, u32 unused2) {
        struct eduimenu_s *parent;
        i32 slot;
        parent = NULL;
        for (slot = item->data; slot < 7; ++slot) {
            edpart_nearest_type->effect_ids[slot] = edpart_nearest_type->effect_ids[slot + 1];
            edpart_nearest_type->effect_pages[slot] = edpart_nearest_type->effect_pages[slot + 1];
        }
        edpart_nearest_type->effect_ids[7] = -1;
        edpart_nearest_type->effect_pages[7] = -1;
        --edpart_nearest_type->variant_count;
        --edpart_nearest_orphans;
        if (menu->parent) {
            parent = menu->parent;
            eduiMenuDetach(menu);
        }
        if (menu->callback) {
            menu->callback(menu, parent);
        }
    }

    static void edpartDeleteAllInstanceOrphans(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
        struct eduimenu_s *parent;
        i32 index;
        i32 slot;
        parent = NULL;
        for (index = 0; index < 8; ++index) {
            if (edpart_nearest_type->effect_ids[index] == 9998) {
                for (slot = index; slot < 7; ++slot) {
                    edpart_nearest_type->effect_ids[slot] = edpart_nearest_type->effect_ids[slot + 1];
                    edpart_nearest_type->effect_pages[slot] = edpart_nearest_type->effect_pages[slot + 1];
                }
                edpart_nearest_type->effect_ids[7] = -1;
                edpart_nearest_type->effect_pages[7] = -1;
                --edpart_nearest_type->variant_count;
                --index;
            }
        }
        edpart_nearest_orphans = 0;
        if (menu->parent) {
            parent = menu->parent;
            eduiMenuDetach(menu);
        }
        if (menu->callback) {
            menu->callback(menu, parent);
        }
    }

    void edpartDeleteAllInstanceDuplicates(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
        struct eduimenu_s *parent;
        i32 index;
        i32 previous;
        i32 slot;
        parent = NULL;
        for (index = 0; index < 8; ++index) {
            i16 effect = edpart_nearest_type->effect_ids[index];
            if (effect == -1 || effect == 9999 || effect == 9998) {
                continue;
            }
            for (previous = 0; previous < index; ++previous) {
                if (effect == edpart_nearest_type->effect_ids[previous]) {
                    for (slot = index; slot < 7; ++slot) {
                        edpart_nearest_type->effect_ids[slot] = edpart_nearest_type->effect_ids[slot + 1];
                        edpart_nearest_type->effect_pages[slot] = edpart_nearest_type->effect_pages[slot + 1];
                    }
                    edpart_nearest_type->effect_ids[7] = -1;
                    edpart_nearest_type->effect_pages[7] = -1;
                    --edpart_nearest_type->variant_count;
                    --index;
                    break;
                }
            }
        }
        edpart_nearest_duplicates = 0;
        if (menu->parent) {
            parent = menu->parent;
            eduiMenuDetach(menu);
        }
        if (menu->callback) {
            menu->callback(menu, parent);
        }
    }
    struct eduimenu_s *edpart_instanceorphans_menu = 0;

    static void edpartCancelInstanceOrphansMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
        i32 index;
        if (edpart_nearest_type->variant_count == 0) {
            for (index = 0; index < 8; ++index) {
                edpart_nearest_type->effect_ids[index] = -1;
                edpart_nearest_type->effect_pages[index] = 1;
            }
            edpart_nearest_type->effect_ids[0] = 9999;
            edpart_nearest_type->flags |= PART_FLAG_KEEP_OFF_SCREEN;
        }
        eduiMenuDestroy(edpart_instanceorphans_menu);
        edpart_instanceorphans_menu = NULL;
    }

    void edpartInstanceOrphansMenu(eduimenu_s *parent, eduiitem_s *unused1, u32 unused2) {
        i32 group;
        i32 index;
        char effect_name[20];
        char label[32];
        edpart_which_scene = 1;
        edpart_instanceorphans_menu = eduiMenuCreate(70, 70, 250, 250, ed_fnt, edpartCancelInstanceOrphansMenu, "Instance Orphans/Dupes");
        if (edpart_instanceorphans_menu != NULL && edpart_nearest_type != NULL) {
            group = 1;
            for (index = 0; index < 8; ++index) {
                if (edpart_nearest_type->effect_ids[index] == 9998) {
                    NuStrNCpy(effect_name, edpart_nearest_type->object_names[index], 17);
                    sprintf(label, "Remove - %s", effect_name);
                    eduiMenuAddItem(edpart_instanceorphans_menu,
                                    eduiItemSelCreate(index, edblack, 0, group, edpartDeleteInstanceOrphan, label));
                    ++group;
                }
            }
            if (edpart_nearest_orphans) {
                eduiMenuAddItem(edpart_instanceorphans_menu,
                                eduiItemSelCreate(index, edblack, 0, 0, edpartDeleteAllInstanceOrphans, "Remove All Orphans"));
            } else {
                eduiMenuAddItem(edpart_instanceorphans_menu, eduiItemSelCreate(index, edgrey, 0, 0, NULL, "Remove All Orphans"));
            }
            if (edpart_nearest_duplicates) {
                eduiMenuAddItem(edpart_instanceorphans_menu,
                                eduiItemSelCreate(index, edblack, 0, 0, edpartDeleteAllInstanceDuplicates, "Remove Duplicates"));
            } else {
                eduiMenuAddItem(edpart_instanceorphans_menu, eduiItemSelCreate(index, edgrey, 0, 0, NULL, "Remove Duplicates"));
            }
            eduiMenuAttach(parent, edpart_instanceorphans_menu);
            edpart_instanceorphans_menu->x = parent->x + 10;
            edpart_instanceorphans_menu->y = parent->y + 40;
        }
    }

    void edpartSetDistribution(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
        if (edpart_nearest_type != NULL) {
            edpart_nearest_type->variant_mode = item->data;
        }
    }
    struct eduimenu_s *edpart_instance_menu = 0;

    void edpartCancelInstanceMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
        eduiMenuDestroy(edpart_instance_menu);
        edpart_instance_menu = NULL;
        edpart_nullobject_highlight = NULL;
    }

    char PartNullInstanceName[14] = "NULL instance";

    void edpartInstanceMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
        i32 selected;
        selected = 0;
        edpart_instance_menu = eduiMenuCreate(70, 70, 250, 250, ed_fnt, edpartCancelInstanceMenu, "Instance Select");
        if (edpart_instance_menu != NULL && edpart_nearest_type != NULL) {
            if (edpart_nearest_type->effect_ids[0] == 9999) {
                selected = 1;
            }
            eduiMenuAddItem(edpart_instance_menu,
                            eduiItemToggleCreate(9999, edblack, selected, 2, edpartSetInstanceType, PartNullInstanceName));
            edpart_nullobject_highlight = edui_last_item;
            if (edpart_nearest_type->effect_list == 1) {
                eduiMenuAddItem(edpart_instance_menu, eduiItemSelCreate(1, edblack, 0, 0, edpartWorldInstanceMenu, "World Scene..."));
            } else {
                eduiMenuAddItem(edpart_instance_menu, eduiItemSelCreate(1, edgrey, 0, 0, NULL, "World Scene..."));
            }
            eduiMenuAddItem(edpart_instance_menu, eduiItemSelCreate(1, edblack, 0, 0, edpartThingsInstanceMenu, "Things Scene..."));
            if (edpart_nearest_orphans == 0 && edpart_nearest_duplicates == 0) {
                eduiMenuAddItem(edpart_instance_menu, eduiItemSelCreate(1, edgrey, 0, 0, NULL, "Instance Orphans/Dupes..."));
            } else {
                eduiMenuAddItem(edpart_instance_menu,
                                eduiItemSelCreate(1, edblack, 0, 0, edpartInstanceOrphansMenu, "Instance Orphans/Dupes..."));
            }
            eduiMenuAddItem(edpart_instance_menu, eduiItemCheckCreate(0, edblack, edpart_nearest_type->variant_mode == 0, 1,
                                                                      edpartSetDistribution, "Random Distribution"));
            eduiMenuAddItem(edpart_instance_menu, eduiItemCheckCreate(1, edblack, edpart_nearest_type->variant_mode == 1, 1,
                                                                      edpartSetDistribution, "Sequential Distribution"));
            eduiMenuAttach(menu, edpart_instance_menu);
            edpart_instance_menu->x = menu->x + 10;
            edpart_instance_menu->y = menu->y + 40;
        }
    }
    struct eduimenu_s *edpart_soundid_menu = 0;

    void edpartSetSoundID(eduimenu_s *menu, eduiitem_s *item, u32 unused2) {
        u32 slot;
        i32 value;
        i32 emitter;
        i32 sound;
        edpart_soundid_menu = NULL;
        slot = (u32)item->data >> 16;
        value = item->data - (slot << 16);
        if (value == 9999) {
            value = -1;
        }
        if (edpart_nearest_type != NULL) {
            edpart_nearest_type->sounds[slot] = value;
        }
        for (emitter = 0; emitter < 40; ++emitter) {
            if (part_emits[emitter].effect_id == edpart_nearest_emit->effect_id) {
                part_emits[emitter].enabled = 0;
                for (sound = 0; emitter < 4; ++sound) {
                    if (part_types[part_emits[emitter].effect_id].sounds[sound] != -1 &&
                        part_types[part_emits[emitter].effect_id].sound_modes[sound] != 0) {
                        part_emits[emitter].enabled = 1;
                        break;
                    }
                }
            }
        }
        eduiMenuDetach(menu);
        eduiMenuDestroy(menu);
    }
    struct eduimenu_s *edpart_soundcontrol_menu = 0;

    void edpartSetSoundControl(eduimenu_s *menu, eduiitem_s *item, u32 unused2) {
        u32 slot;
        i32 value;
        i32 emitter;
        i32 sound;
        edpart_soundcontrol_menu = NULL;
        slot = (u32)item->data >> 16;
        value = item->data - (slot << 16);
        if (edpart_nearest_type != NULL) {
            if (value == 9999) {
                edpart_nearest_type->sound_modes[slot] = 0xff;
            } else {
                edpart_nearest_type->sound_modes[slot] = value;
            }
        }
        for (emitter = 0; emitter < 40; ++emitter) {
            if (part_emits[emitter].effect_id == edpart_nearest_emit->effect_id) {
                part_emits[emitter].enabled = 0;
                for (sound = 0; emitter < 4; ++sound) {
                    if (part_types[part_emits[emitter].effect_id].sounds[sound] != -1 &&
                        part_types[part_emits[emitter].effect_id].sound_modes[sound] != 0) {
                        part_emits[emitter].enabled = 1;
                        break;
                    }
                }
            }
        }
        eduiMenuDetach(menu);
        eduiMenuDestroy(menu);
    }

    void edpartCancelSoundIDMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
        eduiMenuDestroy(edpart_soundid_menu);
        edpart_soundid_menu = NULL;
    }

    void edpartSoundIDMenu(eduimenu_s *menu, eduiitem_s *item, u32 unused2) {
        i32 sound;
        if (edpart_nearest_type != NULL) {
            edpart_soundid_menu = eduiMenuCreate(70, 70, 250, 200, ed_fnt, edpartCancelSoundIDMenu, "Sound ID");
            if (edpart_soundid_menu != NULL) {
                eduiMenuAddItem(edpart_soundid_menu,
                                eduiItemCheckCreate((item->data << 16) + 9999, edblack, edpart_nearest_type->sounds[item->data] == -1, 0,
                                                    edpartSetSoundID, "NONE"));
                for (sound = 0; sound < 1600; ++sound) {
                    if (SfxInfo[sound].name != NULL) {
                        i32 selected = edpart_nearest_type->sounds[item->data] == sound;
                        eduiMenuAddItem(edpart_soundid_menu, eduiItemCheckCreate((item->data << 16) + sound, edblack, selected, 1,
                                                                                 edpartSetSoundID, SfxInfo[sound].name));
                        if (selected == 1) {
                            edpart_soundid_menu->selected = edui_last_item;
                        }
                    }
                }
                eduiMenuAttach(menu, edpart_soundid_menu);
                edpart_soundid_menu->x = menu->x + 10;
                edpart_soundid_menu->y = menu->y + 40;
            }
        }
    }

    void edpartCancelSoundControlMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
        eduiMenuDestroy(edpart_soundcontrol_menu);
        edpart_soundcontrol_menu = NULL;
    }

    const char edpartSoundModeOffText[4] = "Off";

    void edpartSoundControlMenu(eduimenu_s *menu, eduiitem_s *item, u32 unused2) {
        if (edpart_nearest_type != NULL) {
            edpart_soundcontrol_menu = eduiMenuCreate(70, 70, 250, 250, ed_fnt, edpartCancelSoundControlMenu, "Sound Control");
            if (edpart_soundcontrol_menu != NULL) {
                eduiMenuAddItem(edpart_soundcontrol_menu,
                                eduiItemCheckCreate((item->data << 16) + 0, edblack, edpart_nearest_type->sound_modes[item->data] == 0, 1,
                                                    edpartSetSoundControl, ((char *)edpartSoundModeOffText)));
                if (edui_last_item->highlighted) {
                    edpart_soundcontrol_menu->selected = edui_last_item;
                }
                eduiMenuAddItem(edpart_soundcontrol_menu,
                                eduiItemCheckCreate((item->data << 16) + 1, edblack, edpart_nearest_type->sound_modes[item->data] == 1, 1,
                                                    edpartSetSoundControl, "On Edge"));
                if (edui_last_item->highlighted) {
                    edpart_soundcontrol_menu->selected = edui_last_item;
                }
                eduiMenuAddItem(edpart_soundcontrol_menu,
                                eduiItemCheckCreate((item->data << 16) + 2, edblack, edpart_nearest_type->sound_modes[item->data] == 2, 1,
                                                    edpartSetSoundControl, "Off Edge"));
                if (edui_last_item->highlighted) {
                    edpart_soundcontrol_menu->selected = edui_last_item;
                }
                eduiMenuAddItem(edpart_soundcontrol_menu,
                                eduiItemCheckCreate((item->data << 16) + 3, edblack, edpart_nearest_type->sound_modes[item->data] == 3, 1,
                                                    edpartSetSoundControl, "Per PART"));
                if (edui_last_item->highlighted) {
                    edpart_soundcontrol_menu->selected = edui_last_item;
                }
                eduiMenuAddItem(edpart_soundcontrol_menu,
                                eduiItemCheckCreate((item->data << 16) + 4, edblack, edpart_nearest_type->sound_modes[item->data] == 4, 1,
                                                    edpartSetSoundControl, "Continuous"));
                if (edui_last_item->highlighted) {
                    edpart_soundcontrol_menu->selected = edui_last_item;
                }
                eduiMenuAttach(menu, edpart_soundcontrol_menu);
                edpart_soundcontrol_menu->x = menu->x + 10;
                edpart_soundcontrol_menu->y = menu->y + 40;
            }
        }
    }
    struct eduimenu_s *edpart_soundx_menu = 0;

    void edpartCancelSoundXMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
        eduiMenuDestroy(edpart_soundx_menu);
        edpart_soundx_menu = NULL;
    }

    void edpartSoundXMenu(eduimenu_s *menu, eduiitem_s *item, u32 unused2) {
        char title[16];
        if (edpart_nearest_type != NULL) {
            sprintf(title, "Sound %d Menu", item->data + 1);
            edpart_soundx_menu = eduiMenuCreate(70, 70, 250, 300, ed_fnt, edpartCancelSoundXMenu, title);
            if (edpart_soundx_menu != NULL) {
                eduiMenuAddItem(edpart_soundx_menu, eduiItemSelCreate(item->data, edblack, 0, 0, edpartSoundIDMenu, "Sound ID..."));
                eduiMenuAddItem(edpart_soundx_menu,
                                eduiItemSelCreate(item->data, edblack, 0, 0, edpartSoundControlMenu, "Sound Control..."));
                eduiMenuAttach(menu, edpart_soundx_menu);
                edpart_soundx_menu->x = menu->x + 10;
                edpart_soundx_menu->y = menu->y + 40;
            }
        }
    }
    struct eduimenu_s *edpart_sounds_menu = 0;

    void edpartCancelSoundsMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
        eduiMenuDestroy(edpart_sounds_menu);
        edpart_sounds_menu = NULL;
    }

    void edpartSoundsMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
        char text[16];
        i32 index;
        if (edpart_nearest_type != NULL) {
            edpart_sounds_menu = eduiMenuCreate(70, 70, 250, 300, ed_fnt, edpartCancelSoundsMenu, "Attached Sounds");
            if (edpart_sounds_menu != NULL) {
                for (index = 0; index < 4; ++index) {
                    sprintf(text, "Sound %d...", index + 1);
                    eduiMenuAddItem(edpart_sounds_menu, eduiItemSelCreate(index, edblack, 0, 0, edpartSoundXMenu, text));
                }
                eduiMenuAttach(menu, edpart_sounds_menu);
                edpart_sounds_menu->x = menu->x + 10;
                edpart_sounds_menu->y = menu->y + 40;
            }
        }
    }
    struct eduimenu_s *edpart_cutoff_menu = 0;

    void edpartCancelCutOffMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
        eduiMenuDestroy(edpart_cutoff_menu);
        edpart_cutoff_menu = NULL;
    }

    void edpartCutOffMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
        if (edpart_nearest_type != NULL) {
            edpart_cutoff_menu = eduiMenuCreate(70, 70, 250, 200, ed_fnt, edpartCancelCutOffMenu, "Radii");
            if (edpart_cutoff_menu != NULL) {
                eduiMenuAddItem(edpart_cutoff_menu, eduiItemSliderCreate(0, edblack, 0, edpartChangeCutOff, 0.0f, edpart_superscale * 25.0f,
                                                                         edpart_nearest_type->maximum_distance, "CutOff Rad"));
                eduiMenuAttach(menu, edpart_cutoff_menu);
                edpart_cutoff_menu->x = menu->x + 10;
                edpart_cutoff_menu->y = menu->y + 40;
            }
        }
    }

    void edpartChangeVarEmit(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
        if (edpart_nearest_type != NULL) {
            if (item->data == 0) {
                edpart_nearest_type->velocity_random.x = ((edui_slider_s *)item)->value;
            } else if (item->data == 1) {
                edpart_nearest_type->velocity_random.y = ((edui_slider_s *)item)->value;
            } else if (item->data == 2) {
                edpart_nearest_type->velocity_random.z = ((edui_slider_s *)item)->value;
            }
        }
    }
    struct eduimenu_s *edpart_varemit_menu = 0;

    void edpartCancelVarEmitMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
        eduiMenuDestroy(edpart_varemit_menu);
        edpart_varemit_menu = NULL;
    }

    void edpartVarEmitMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
        if (edpart_nearest_type != NULL) {
            edpart_varemit_menu = eduiMenuCreate(70, 70, 250, 300, ed_fnt, edpartCancelVarEmitMenu, "Emitter Variation");
            if (edpart_varemit_menu != NULL) {
                eduiMenuAddItem(edpart_varemit_menu,
                                eduiItemSliderCreate(0, edblack, 0, edpartChangeVarEmit, 0.0f, edpart_superscale * 5.0f,
                                                     edpart_nearest_type->velocity_random.x, "Rand Emit X"));
                eduiMenuAddItem(edpart_varemit_menu,
                                eduiItemSliderCreate(1, edblack, 0, edpartChangeVarEmit, 0.0f, edpart_superscale * 5.0f,
                                                     edpart_nearest_type->velocity_random.y, "Rand Emit Y"));
                eduiMenuAddItem(edpart_varemit_menu,
                                eduiItemSliderCreate(2, edblack, 0, edpartChangeVarEmit, 0.0f, edpart_superscale * 5.0f,
                                                     edpart_nearest_type->velocity_random.z, "Rand Emit Z"));
                eduiMenuAttach(menu, edpart_varemit_menu);
                edpart_varemit_menu->x = menu->x + 10;
                edpart_varemit_menu->y = menu->y + 40;
            }
        }
    }

    void edpartChangeInstanceRot(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
        if (edpart_nearest_type != NULL) {
            if (item->data == 0) {
                edpart_nearest_type->rotation[0] = ((i32)(((edui_slider_s *)(item))->value * (65536.0f / 360.0f)));
            } else if (item->data == 1) {
                edpart_nearest_type->rotation[1] = ((i32)(((edui_slider_s *)(item))->value * (65536.0f / 360.0f)));
            } else if (item->data == 2) {
                edpart_nearest_type->rotation[2] = ((i32)(((edui_slider_s *)(item))->value * (65536.0f / 360.0f)));
            }
        }
    }

    void edpartChangeInstanceVarRot(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
        if (edpart_nearest_type != NULL) {
            if (item->data == 0) {
                edpart_nearest_type->rotation_random[0] = ((i32)(((edui_slider_s *)(item))->value * (65536.0f / 360.0f)));
            } else if (item->data == 1) {
                edpart_nearest_type->rotation_random[1] = ((i32)(((edui_slider_s *)(item))->value * (65536.0f / 360.0f)));
            } else if (item->data == 2) {
                edpart_nearest_type->rotation_random[2] = ((i32)(((edui_slider_s *)(item))->value * (65536.0f / 360.0f)));
            }
        }
    }
    struct eduimenu_s *edpart_instorient_menu = 0;

    void edpartCancelInstanceOrientMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
        eduiMenuDestroy(edpart_instorient_menu);
        edpart_instorient_menu = NULL;
    }

    void edpartInstanceOrientMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
        if (edpart_nearest_type == NULL) {
            return;
        }
        edpart_instorient_menu = eduiMenuCreate(70, 70, 250, 250, ed_fnt, edpartCancelInstanceOrientMenu, "Instance Orientation");
        if (edpart_instorient_menu == NULL) {
            return;
        }
        eduiMenuAddItem(edpart_instorient_menu,
                        eduiItemSliderCreate(0, edblack, 0, edpartChangeInstanceRot, edpart_superscale * -180.0f,
                                             edpart_superscale * 360.0f, edpart_nearest_type->rotation[0] * (360.0f / 65536.0f),
                                             "Base Rotation X"));
        eduiItemSliderSetFmt((edui_slider_s *)edui_last_item, "(%1.01f)");
        eduiItemSliderSetGranularity((edui_slider_s *)edui_last_item, 0.1f);
        eduiMenuAddItem(edpart_instorient_menu,
                        eduiItemSliderCreate(1, edblack, 0, edpartChangeInstanceRot, edpart_superscale * -180.0f,
                                             edpart_superscale * 360.0f, edpart_nearest_type->rotation[1] * (360.0f / 65536.0f),
                                             "Base Rotation Y"));
        eduiItemSliderSetFmt((edui_slider_s *)edui_last_item, "(%1.01f)");
        eduiItemSliderSetGranularity((edui_slider_s *)edui_last_item, 0.1f);
        eduiMenuAddItem(edpart_instorient_menu,
                        eduiItemSliderCreate(2, edblack, 0, edpartChangeInstanceRot, edpart_superscale * -180.0f,
                                             edpart_superscale * 360.0f, edpart_nearest_type->rotation[2] * (360.0f / 65536.0f),
                                             "Base Rotation Z"));
        eduiItemSliderSetFmt((edui_slider_s *)edui_last_item, "(%1.01f)");
        eduiItemSliderSetGranularity((edui_slider_s *)edui_last_item, 0.1f);
        eduiMenuAddItem(edpart_instorient_menu,
                        eduiItemSliderCreate(0, edblack, 0, edpartChangeInstanceVarRot, 0.0f, edpart_superscale * 180.0f,
                                             edpart_nearest_type->rotation_random[0] * (360.0f / 65536.0f), "Rotation Variation X"));
        eduiItemSliderSetFmt((edui_slider_s *)edui_last_item, "(%1.01f)");
        eduiItemSliderSetGranularity((edui_slider_s *)edui_last_item, 0.1f);
        eduiMenuAddItem(edpart_instorient_menu,
                        eduiItemSliderCreate(1, edblack, 0, edpartChangeInstanceVarRot, 0.0f, edpart_superscale * 180.0f,
                                             edpart_nearest_type->rotation_random[1] * (360.0f / 65536.0f), "Rotation Variation Y"));
        eduiItemSliderSetFmt((edui_slider_s *)edui_last_item, "(%1.01f)");
        eduiItemSliderSetGranularity((edui_slider_s *)edui_last_item, 0.1f);
        eduiMenuAddItem(edpart_instorient_menu,
                        eduiItemSliderCreate(2, edblack, 0, edpartChangeInstanceVarRot, 0.0f, edpart_superscale * 180.0f,
                                             edpart_nearest_type->rotation_random[2] * (360.0f / 65536.0f), "Rotation Variation Z"));
        eduiItemSliderSetFmt((edui_slider_s *)edui_last_item, "(%1.01f)");
        eduiItemSliderSetGranularity((edui_slider_s *)edui_last_item, 0.1f);
        eduiMenuAttach(menu, edpart_instorient_menu);
        edpart_instorient_menu->x = menu->x + 10;
        edpart_instorient_menu->y = menu->y + 40;
    }

    void edpartChangeVarStart(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
        if (edpart_nearest_type != NULL) {
            if (item->data == 0) {
                edpart_nearest_type->position_random.x = ((edui_slider_s *)item)->value;
            } else if (item->data == 1) {
                edpart_nearest_type->position_random.y = ((edui_slider_s *)item)->value;
            } else if (item->data == 2) {
                edpart_nearest_type->position_random.z = ((edui_slider_s *)item)->value;
            }
        }
    }
    struct eduimenu_s *edpart_varstart_menu = 0;

    void edpartCancelVarStartMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
        eduiMenuDestroy(edpart_varstart_menu);
        edpart_varstart_menu = NULL;
    }

    void edpartVarStartMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
        if (edpart_nearest_type != NULL) {
            edpart_varstart_menu = eduiMenuCreate(70, 70, 180, 300, ed_fnt, edpartCancelVarStartMenu, "Random Start");
            if (edpart_varstart_menu != NULL) {
                eduiMenuAddItem(edpart_varstart_menu,
                                eduiItemSliderCreate(0, edblack, 0, edpartChangeVarStart, 0.0f, edpart_superscale * 5.0f,
                                                     edpart_nearest_type->position_random.x, "Rand Start X"));
                eduiMenuAddItem(edpart_varstart_menu,
                                eduiItemSliderCreate(1, edblack, 0, edpartChangeVarStart, 0.0f, edpart_superscale * 5.0f,
                                                     edpart_nearest_type->position_random.y, "Rand Start Y"));
                eduiMenuAddItem(edpart_varstart_menu,
                                eduiItemSliderCreate(2, edblack, 0, edpartChangeVarStart, 0.0f, edpart_superscale * 5.0f,
                                                     edpart_nearest_type->position_random.z, "Rand Start Z"));
                eduiMenuAttach(menu, edpart_varstart_menu);
                edpart_varstart_menu->x = menu->x + 10;
                edpart_varstart_menu->y = menu->y + 40;
            }
        }
    }

    void edpartChangeSScale(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
        edpart_superscale = (i32)((edui_slider_s *)item)->value;
    }
    struct eduimenu_s *edpart_sscale_menu = 0;

    void edpartCancelSScaleMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
        eduiMenuDestroy(edpart_sscale_menu);
        edpart_sscale_menu = NULL;
    }

    void edpartSScaleMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
        edpart_sscale_menu = eduiMenuCreate(70, 70, 180, 300, ed_fnt, edpartCancelSScaleMenu, "Super Scale");
        if (edpart_sscale_menu != NULL) {
            eduiMenuAddItem(edpart_sscale_menu,
                            eduiItemSliderCreateInt(0, edblack, 0, edpartChangeSScale, 1, 99, edpart_superscale, "Super Scale"));
            eduiMenuAttach(menu, edpart_sscale_menu);
            edpart_sscale_menu->x = menu->x + 10;
            edpart_sscale_menu->y = menu->y + 40;
        }
    }

    void edpartChangeInstanceFlag(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
        if (edpart_nearest_type != NULL) {
            if (item->highlighted) {
                edpart_nearest_type->flags |= item->data;
            } else {
                edpart_nearest_type->flags &= ~item->data;
            }
        }
    }

    void edpartChangeIvalOn(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
        if (edpart_nearest_type != NULL) {
            edpart_nearest_type->emission_period = ((edui_slider_s *)item)->value;
        }
    }

    void edpartChangeIvalOnRan(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
        if (edpart_nearest_type != NULL) {
            edpart_nearest_type->emission_period_random = ((edui_slider_s *)item)->value;
        }
    }

    void edpartChangeIvalOff(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
        if (edpart_nearest_type != NULL) {
            edpart_nearest_type->emission_pause = ((edui_slider_s *)item)->value;
        }
    }

    void edpartChangeIvalOffRan(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
        if (edpart_nearest_type != NULL) {
            edpart_nearest_type->emission_pause_random = ((edui_slider_s *)item)->value;
        }
    }
    struct eduimenu_s *edpart_emittime_menu = 0;

    void edpartCancelEmitTimeMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
        eduiMenuDestroy(edpart_emittime_menu);
        edpart_emittime_menu = NULL;
    }

    void edpartEmitTimeMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
        if (edpart_nearest_type != NULL) {
            edpart_emittime_menu = eduiMenuCreate(70, 70, 260, 300, ed_fnt, edpartCancelEmitTimeMenu, "Emitter Timing");
            if (edpart_emittime_menu != NULL) {
                eduiMenuAddItem(edpart_emittime_menu,
                                eduiItemToggleCreate(PART_FLAG_TRIGGER_INSIDE_CUTOFF, edblack, (edpart_nearest_type->flags >> 22) & 1, 1,
                                                     edpartChangeInstanceFlag, "Trigger effect inside cutoff"));
                eduiMenuAddItem(edpart_emittime_menu,
                                eduiItemSliderCreate(0, edblack, 0, edpartChangeIvalOn, 0.01f, edpart_superscale * 5.0f,
                                                     edpart_nearest_type->emission_period, "On Time"));
                eduiMenuAddItem(edpart_emittime_menu,
                                eduiItemSliderCreate(0, edblack, 0, edpartChangeIvalOnRan, 0.0f, edpart_superscale * 5.0f,
                                                     edpart_nearest_type->emission_period_random, "Random On Time"));
                eduiMenuAddItem(edpart_emittime_menu,
                                eduiItemSliderCreate(0, edblack, 0, edpartChangeIvalOff, 0.0f, edpart_superscale * 5.0f,
                                                     edpart_nearest_type->emission_pause, "Off Time"));
                eduiMenuAddItem(edpart_emittime_menu,
                                eduiItemSliderCreate(0, edblack, 0, edpartChangeIvalOffRan, 0.0f, edpart_superscale * 5.0f,
                                                     edpart_nearest_type->emission_pause_random, "Random Off Time"));
                eduiMenuAttach(menu, edpart_emittime_menu);
                edpart_emittime_menu->x = menu->x + 10;
                edpart_emittime_menu->y = menu->y + 40;
            }
        }
    }

    void edpartChangeEmitVel(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
        if (edpart_nearest_type != NULL) {
            edpart_nearest_type->speed = ((edui_slider_s *)item)->value;
        }
    }
    struct eduimenu_s *edpart_emitvel_menu = 0;

    void edpartCancelEmitVelMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
        eduiMenuDestroy(edpart_emitvel_menu);
        edpart_emitvel_menu = NULL;
    }

    void edpartEmitVelMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
        if (edpart_nearest_type != NULL) {
            edpart_emitvel_menu = eduiMenuCreate(70, 70, 180, 250, ed_fnt, edpartCancelEmitVelMenu, "Emitter Vel");
            if (edpart_emitvel_menu != NULL) {
                eduiMenuAddItem(edpart_emitvel_menu,
                                eduiItemSliderCreate(0, edblack, 0, edpartChangeEmitVel, -(edpart_superscale * 10.0f),
                                                     edpart_superscale * 20.0f, edpart_nearest_type->speed, "Emitter Vel"));
                eduiMenuAttach(menu, edpart_emitvel_menu);
                edpart_emitvel_menu->x = menu->x + 10;
                edpart_emitvel_menu->y = menu->y + 40;
            }
        }
    }

    void edpartChangeGrav(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
        if (edpart_nearest_type) {
            edpart_nearest_type->gravity = ((edui_slider_s *)(item))->value * 2.0f;
        }
    }

    void edpartChangeBounce(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
        if (edpart_nearest_type) {
            edpart_nearest_type->bounce = ((edui_slider_s *)(item))->value;
        }
    }
    struct eduimenu_s *edpart_grav_menu = 0;

    void edpartCancelGravMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
        eduiMenuDestroy(edpart_grav_menu);
        edpart_grav_menu = NULL;
    }

    void edpartGravMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
        if (edpart_nearest_type != NULL) {
            edpart_grav_menu = eduiMenuCreate(70, 70, 180, 250, ed_fnt, edpartCancelGravMenu, "Gravity");
            if (edpart_grav_menu != NULL) {
                eduiMenuAddItem(edpart_grav_menu,
                                eduiItemSliderCreate(0, edblack, 0, edpartChangeGrav, edpart_superscale * -10.0f, edpart_superscale * 20.0f,
                                                     edpart_nearest_type->gravity * 0.5f, "Gravity"));
                eduiMenuAddItem(edpart_grav_menu, eduiItemSliderCreate(0, edblack, 0, edpartChangeBounce, 0.0f, edpart_superscale,
                                                                       edpart_nearest_type->bounce, "Bounce"));
                eduiMenuAttach(menu, edpart_grav_menu);
                edpart_grav_menu->x = menu->x + 10;
                edpart_grav_menu->y = menu->y + 40;
            }
        }
    }

    i32 edpartSaveEffects(char *filename, u8 page) {
        char empty_name[16];
        i32 index;
        i32 variant;
        i32 type_count = 0;
        i32 emitter_count = 0;
        memset(empty_name, 0, sizeof(empty_name));
        for (index = 0; index < 128; ++index) {
            if (part_types[index].name[0] != '\0' && part_types[index].effect_list == page) {
                ++type_count;
            }
        }

        EdFileSetMedia(1);
        if (EdFileOpen(filename, 1)) {
            EdFileSetReadWrongEndianess(1);
            EdFileWriteInt(16);
            EdFileWriteInt(type_count);

            for (index = 0; index < 128; ++index) {
                if (part_types[index].name[0] == '\0' || part_types[index].effect_list != page) {
                    continue;
                }

                EdFileWrite(part_types[index].name, 16);
                for (variant = 0; variant < 8; ++variant) {
                    EdFileWriteChar((u8)part_types[index].effect_pages[variant]);
                }
                for (variant = 0; variant < 8; ++variant) {
                    i16 effect_id = part_types[index].effect_ids[variant];
                    nuhspecial_s special;
                    if (effect_id == -1) {
                        EdFileWrite(empty_name, 16);
                    } else if (effect_id == 9999) {
                        EdFileWrite(PartNullInstanceName, 16);
                    } else if (effect_id == 9998) {
                        EdFileWrite(part_types[index].object_names[variant], 16);
                    } else if (part_types[index].effect_pages[variant] == 0) {
                        NuGScnGetSpecial(&special, edbits_base_scene, effect_id);
                        EdFileWrite(NuSpecialGetName(&special), 16);
                    } else if (part_types[index].effect_pages[variant] == 1) {
                        NuGScnGetSpecial(&special, edbits_things_scene, effect_id);
                        EdFileWrite(NuSpecialGetName(&special), 16);
                    } else {
                        EdFileWrite(empty_name, 16);
                    }
                }
                EdFileWriteChar(part_types[index].variant_mode);
                EdFileWriteFloat(part_types[index].particle_scale);
                EdFileWriteFloat(part_types[index].effect_scale);
                EdFileWriteFloat(part_types[index].lifetime);
                EdFileWriteFloat(part_types[index].lifetime_random);
                EdFileWriteFloat(part_types[index].speed);
                EdFileWriteFloat(part_types[index].gravity);
                EdFileWriteFloat(part_types[index].emission_rate);
                EdFileWriteFloat(part_types[index].bounce);
                EdFileWriteNuVec(&part_types[index].position_random);
                EdFileWriteNuVec(&part_types[index].velocity_random);
                EdFileWriteFloat(part_types[index].emission_period);
                EdFileWriteFloat(part_types[index].emission_period_random);
                EdFileWriteFloat(part_types[index].emission_pause);
                EdFileWriteFloat(part_types[index].emission_pause_random);
                EdFileWriteInt(part_types[index].rotation[0]);
                EdFileWriteInt(part_types[index].rotation[1]);
                EdFileWriteInt(part_types[index].rotation[2]);
                EdFileWriteInt(part_types[index].rotation_random[0]);
                EdFileWriteInt(part_types[index].rotation_random[1]);
                EdFileWriteInt(part_types[index].rotation_random[2]);
                EdFileWriteUnsignedInt(part_types[index].flags);

                if (part_types[index].trail_effects[0] != -1) {
                    EdFileWrite(debtab[part_types[index].trail_effects[0]]->name, 16);
                } else {
                    EdFileWrite(empty_name, 16);
                }
                if (part_types[index].trail_effects[1] != -1) {
                    EdFileWrite(debtab[part_types[index].trail_effects[1]]->name, 16);
                } else {
                    EdFileWrite(empty_name, 16);
                }
                if (part_types[index].attached_effect != -1) {
                    EdFileWrite(debtab[part_types[index].attached_effect]->name, 16);
                } else {
                    EdFileWrite(empty_name, 16);
                }
                EdFileWriteFloat(part_types[index].trail_rates[0]);
                EdFileWriteFloat(part_types[index].trail_rates[1]);
                if (part_types[index].kill_effect != -1) {
                    EdFileWrite(debtab[part_types[index].kill_effect]->name, 16);
                } else {
                    EdFileWrite(empty_name, 16);
                }
                if (part_types[index].impact_effect != -1) {
                    EdFileWrite(debtab[part_types[index].impact_effect]->name, 16);
                } else {
                    EdFileWrite(empty_name, 16);
                }
                if (part_types[index].impact_part != -1) {
                    EdFileWrite(part_types[part_types[index].impact_part].name, 16);
                } else {
                    EdFileWrite(empty_name, 16);
                }
                for (variant = 0; variant < 4; ++variant) {
                    if (part_types[index].sounds[variant] != -1) {
                        EdFileWrite(SfxInfo[part_types[index].sounds[variant]].name, 16);
                    } else {
                        EdFileWrite(empty_name, 16);
                    }
                }
                for (variant = 0; variant < 4; ++variant) {
                    EdFileWriteChar(part_types[index].sound_modes[variant]);
                }
                EdFileWriteFloat(part_types[index].maximum_distance);
                EdFileWriteFloat(part_types[index].lighting_scale.x);
                EdFileWriteFloat(part_types[index].lighting_scale.y);
                EdFileWriteFloat(part_types[index].lighting_scale.z);
            }

            if (page == 1) {
                for (index = 0; index < 40; ++index) {
                    if (part_emits[index].effect_id != -1 && part_emits[index].shots_remaining == 0) {
                        ++emitter_count;
                    }
                }
                EdFileWriteInt(emitter_count);
                for (index = 0; index < 40; ++index) {
                    if (part_emits[index].effect_id == -1) {
                        continue;
                    }
                    EdFileWriteNuVec(&part_emits[index].position);
                    EdFileWrite(part_emits[index].name, 16);
                    EdFileWriteShort(part_emits[index].rotation_30);
                    EdFileWriteShort(part_emits[index].rotation_2e);
                    EdFileWriteShort(part_emits[index].rotation_2c);
                    EdFileWriteShort(part_emits[index].trigger_type);
                    EdFileWriteShort(part_emits[index].trigger_id);
                }
            } else {
                EdFileWriteInt(0);
            }
            EdFileSetReadWrongEndianess(0);
            EdFileClose();
            return 1;
        }
        return 0;
    }

    char edbits_level_save_directory[256] = {0};
    char edbits_level_save_name[256] = {0};
    char edbits_level_save_extension[256] = {0};

    void edpartFileSaveEffects(eduimenu_s *parent, eduiitem_s *unused1, u32 unused2) {
        char general_directory[256];
        char general_extension[256];
        char general_name[256];
        char level_directory[256];
        char level_extension[256];
        char level_name[256];
        char path[256];
        char backup[256];
        i32 i;
        i32 general_backup = 1;
        i32 level_backup = 1;
        i32 general_saved;
        i32 level_saved;
        if (edbits_general_save_directory[0] == 0) {
            sprintf(general_directory, ".");
        } else {
            i = 0;
            do {
                general_directory[i] = edbits_general_save_directory[i];
            } while (general_directory[i++] != 0);
        }
        if (edbits_general_save_name[0] == 0) {
            sprintf(general_name, "part");
        } else {
            i = 0;
            do {
                general_name[i] = edbits_general_save_name[i];
            } while (general_name[i++] != 0);
        }
        if (edbits_general_save_extension[0] == 0) {
            sprintf(general_extension, "par");
        } else {
            i = 0;
            do {
                general_extension[i] = edbits_general_save_extension[i];
            } while (general_extension[i++] != 0);
        }
        if (edbits_level_save_directory[0] == 0) {
            sprintf(level_directory, ".");
        } else {
            i = 0;
            do {
                level_directory[i] = edbits_level_save_directory[i];
            } while (level_directory[i++] != 0);
        }
        if (edbits_level_save_name[0] == 0) {
            sprintf(level_name, "part");
        } else {
            i = 0;
            do {
                level_name[i] = edbits_level_save_name[i];
            } while (level_name[i++] != 0);
        }
        if (edbits_level_save_extension[0] == 0) {
            sprintf(level_extension, "par");
        } else {
            i = 0;
            do {
                level_extension[i] = edbits_level_save_extension[i];
            } while (level_extension[i++] != 0);
        }
        sprintf(path, "%s\\%s.%s", general_directory, general_name, general_extension);
        sprintf(backup, "%s\\%s.%s.bak", general_directory, general_name, general_extension);
        if (!edbits_override_backups) {
            general_backup = EdFileBackup(path, backup);
        }
        general_saved = edpartSaveEffects(path, 0);
        sprintf(path, "%s\\%s.%s", level_directory, level_name, level_extension);
        sprintf(backup, "%s\\%s.%s.bak", level_directory, level_name, level_extension);
        if (!edbits_override_backups) {
            level_backup = EdFileBackup(path, backup);
        }
        level_saved = edpartSaveEffects(path, 1);
        if (general_saved) {
            if (general_backup && level_saved && level_backup) {
                edpartSaveMessage(parent, "Saved OK", 1);
            } else if (level_saved) {
                if (general_backup) {
                    edpartSaveMessage(parent, "Saved OK - Level Backup Failed", 0);
                } else if (level_backup) {
                    edpartSaveMessage(parent, "Saved OK - General Backup Failed", 0);
                } else {
                    edpartSaveMessage(parent, "Saved OK - Both Backups Failed", 0);
                }
            } else {
                edpartSaveMessage(parent, "Level Save Failed", 0);
            }
        } else if (level_saved) {
            edpartSaveMessage(parent, "General Save Failed", 0);
        } else {
            edpartSaveMessage(parent, "Both Saves Failed", 0);
        }
    }

    void edpartFileSaveEffectsGeneral(eduimenu_s *parent, eduiitem_s *unused1, u32 unused2) {
        char directory[256];
        char extension[256];
        char name[256];
        char path[256];
        char backup[256];
        i32 i;
        i32 backed_up = 1;
        if (edbits_general_save_directory[0] == 0) {
            sprintf(directory, ".");
        } else {
            i = 0;
            do {
                directory[i] = edbits_general_save_directory[i];
            } while (directory[i++] != 0);
        }
        if (edbits_general_save_name[0] == 0) {
            sprintf(name, "part");
        } else {
            i = 0;
            do {
                name[i] = edbits_general_save_name[i];
            } while (name[i++] != 0);
        }
        if (edbits_general_save_extension[0] == 0) {
            sprintf(extension, "par");
        } else {
            i = 0;
            do {
                extension[i] = edbits_general_save_extension[i];
            } while (extension[i++] != 0);
        }
        sprintf(path, "%s\\%s.%s", directory, name, extension);
        sprintf(backup, "%s\\%s.%s.bak", directory, name, extension);
        if (!edbits_override_backups) {
            backed_up = EdFileBackup(path, backup);
        }
        if (edpartSaveEffects(path, 0)) {
            if (backed_up) {
                edpartSaveMessage(parent, "Saved OK", 1);
            } else {
                edpartSaveMessage(parent, "Saved OK - Backup Failed", 0);
            }
        } else {
            edpartSaveMessage(parent, "Save Failed", 0);
        }
    }

    void edpartFileSaveEffectsLevel(eduimenu_s *parent, eduiitem_s *unused1, u32 unused2) {
        char directory[256];
        char extension[256];
        char name[256];
        char path[256];
        char backup[256];
        i32 i;
        i32 backed_up = 1;
        if (edbits_level_save_directory[0] == 0) {
            sprintf(directory, ".");
        } else {
            i = 0;
            do {
                directory[i] = edbits_level_save_directory[i];
            } while (directory[i++] != 0);
        }
        if (edbits_level_save_name[0] == 0) {
            sprintf(name, "part");
        } else {
            i = 0;
            do {
                name[i] = edbits_level_save_name[i];
            } while (name[i++] != 0);
        }
        if (edbits_level_save_extension[0] == 0) {
            sprintf(extension, "par");
        } else {
            i = 0;
            do {
                extension[i] = edbits_level_save_extension[i];
            } while (extension[i++] != 0);
        }
        sprintf(path, "%s\\%s.%s", directory, name, extension);
        sprintf(backup, "%s\\%s.%s.bak", directory, name, extension);
        if (!edbits_override_backups) {
            backed_up = EdFileBackup(path, backup);
        }
        if (edpartSaveEffects(path, 1)) {
            if (backed_up) {
                edpartSaveMessage(parent, "Saved OK", 1);
            } else {
                edpartSaveMessage(parent, "Saved OK - Backup Failed", 0);
            }
        } else {
            edpartSaveMessage(parent, "Save Failed", 0);
        }
    }

    i32 edpartLookupObjectInScene(char *name, NUGSCN *scene) {
        nuhspecial_s special;
        i32 count;
        i32 i;
        char *special_name;
        if (name[0] == 0) {
            return -1;
        }
        if (NuStrNCmp(name, PartNullInstanceName, 16) == 0) {
            return 9999;
        }
        if (scene == NULL) {
            return -1;
        }
        count = NuGScnNumSpecials(scene);
        for (i = 0; i < count; ++i) {
            NuGScnGetSpecial(&special, scene, i);
            if (NuSpecialExistsFn(&special)) {
                special_name = NuSpecialGetName(&special);
                if (NuStrNCmp(name, special_name, 16) == 0) {
                    return i;
                }
            }
        }
        return -1;
    }

    i32 edpart_load_particle_page = -1;

    i32 LookupDebrisEffectOnLoadPage(char *name) {
        if (edpart_load_particle_page == -1) {
            return LookupDebrisEffect(name);
        }
        return LookupDebrisEffectPageOnly(name, edpart_load_particle_page);
    }

    void edpartSetParticlePage(i32 page) {
        edpart_load_particle_page = page;
    }

    i32 edpartLookupObject(char *name) {
        if (edbits_base_scene == NULL) {
            return -1;
        }
        return edpartLookupObjectInScene(name, edbits_base_scene);
    }

    void edpartStartPage(u8 page) {
        i32 i;
        i8 signed_page = (i8)page;

        for (i = 0; i < 40; ++i) {
            if (part_emits[i].effect_id != -1 && part_emits[i].page == signed_page) {
                part_emits[i].camera_distance = 9999.9f;
            }
        }
        part_page_on[signed_page] = 1;
    }

    // noinline: the original calls edpartStopPage out of line from Parts_Stop; VC8 would inline it there.
    __declspec(noinline) void edpartStopPage(u8 page) {
        part_page_on[(i8)page] = 0;
    }

    void edpartMoveList(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
        struct eduimenu_s *parent;
        parent = NULL;
        if (part_types[edpart_create_type].effect_list == 0) {
            part_types[edpart_create_type].effect_list = 1;
            part_types[edpart_create_type].page = 1;
        } else {
            part_types[edpart_create_type].effect_list = 0;
            part_types[edpart_create_type].page = 0;
        }
        edpart_effect_list = part_types[edpart_create_type].effect_list;
        if (menu->parent) {
            parent = menu->parent;
            eduiMenuDetach(menu);
        }
        if (menu->callback) {
            menu->callback(menu, parent);
        }
    }
    struct eduimenu_s *edpart_data_menu = 0;

    void edpartCancelDataMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
        eduiMenuDestroy(edpart_data_menu);
        edpart_data_menu = NULL;
    }
    struct eduimenu_s *edpart_instanceflags_menu = 0;

    void edpartCancelInstanceFlagsMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
        eduiMenuDestroy(edpart_instanceflags_menu);
        edpart_instanceflags_menu = NULL;
    }

    void edpartInstanceFlagsMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
        i32 group = 1;
        if (edpart_nearest_type == NULL) {
            return;
        }
        edpart_instanceflags_menu = eduiMenuCreate(70, 70, 300, 250, ed_fnt, edpartCancelInstanceFlagsMenu, "Instance Flags");
        if (edpart_instanceflags_menu != NULL) {
            eduiMenuAddItem(edpart_instanceflags_menu, eduiItemToggleCreate(PART_FLAG_DIE_WHEN_STOPPED, edblack,
                                                                            (edpart_nearest_type->flags & PART_FLAG_DIE_WHEN_STOPPED) != 0,
                                                                            group++, edpartChangeInstanceFlag, "Die when stopped"));
            eduiMenuAddItem(edpart_instanceflags_menu,
                            eduiItemToggleCreate(PART_FLAG_COLLIDE_WITH_CHARACTERS, edblack,
                                                 (edpart_nearest_type->flags & PART_FLAG_COLLIDE_WITH_CHARACTERS) != 0, group++,
                                                 edpartChangeInstanceFlag, "Collide with Characters"));
            eduiMenuAddItem(edpart_instanceflags_menu,
                            eduiItemToggleCreate(PART_FLAG_ACTIVE_CHARACTERS_ONLY, edblack,
                                                 (edpart_nearest_type->flags & PART_FLAG_ACTIVE_CHARACTERS_ONLY) != 0, group++,
                                                 edpartChangeInstanceFlag, "Interact only with Active Chars"));
            eduiMenuAddItem(edpart_instanceflags_menu, eduiItemToggleCreate(PART_FLAG_DAMAGE_CHARACTERS, edblack,
                                                                            (edpart_nearest_type->flags & PART_FLAG_DAMAGE_CHARACTERS) != 0,
                                                                            group++, edpartChangeInstanceFlag, "Damage Characters"));
            if (edpart_nearest_type->effect_ids[0] != 9999) {
                eduiMenuAddItem(edpart_instanceflags_menu,
                                eduiItemToggleCreate(PART_FLAG_KEEP_OFF_SCREEN, edblack,
                                                     (edpart_nearest_type->flags & PART_FLAG_KEEP_OFF_SCREEN) != 0, group++,
                                                     edpartChangeInstanceFlag, "Don't die when off-screen"));
            }
            eduiMenuAddItem(edpart_instanceflags_menu,
                            eduiItemToggleCreate(PART_FLAG_COLLECTIBLE, edblack, (edpart_nearest_type->flags & PART_FLAG_COLLECTIBLE) != 0,
                                                 group++, edpartChangeInstanceFlag, "Is a Collectible"));
            eduiMenuAddItem(edpart_instanceflags_menu, eduiItemToggleCreate(PART_FLAG_ROTATE_RANDOMLY, edblack,
                                                                            (edpart_nearest_type->flags & PART_FLAG_ROTATE_RANDOMLY) != 0,
                                                                            group++, edpartChangeInstanceFlag, "Rotate Randomly"));
            eduiMenuAddItem(edpart_instanceflags_menu,
                            eduiItemToggleCreate(PART_FLAG_ORDERED_ROTATE_RANDOMLY, edblack,
                                                 (edpart_nearest_type->flags & PART_FLAG_ORDERED_ROTATE_RANDOMLY) != 0, group++,
                                                 edpartChangeInstanceFlag, "Ordered Rotate Randomly"));
            eduiMenuAddItem(edpart_instanceflags_menu,
                            eduiItemToggleCreate(PART_FLAG_NO_BOUNCE, edblack, (edpart_nearest_type->flags & PART_FLAG_NO_BOUNCE) != 0,
                                                 group++, edpartChangeInstanceFlag, "No Bounce"));
            eduiMenuAddItem(edpart_instanceflags_menu,
                            eduiItemToggleCreate(PART_FLAG_SLOT_CANNOT_BE_STOLEN, edblack,
                                                 (edpart_nearest_type->flags & PART_FLAG_SLOT_CANNOT_BE_STOLEN) != 0, group++,
                                                 edpartChangeInstanceFlag, "Slot Cannot be Stolen"));
            eduiMenuAddItem(edpart_instanceflags_menu, eduiItemToggleCreate(PART_FLAG_IGNORE_TERRAIN, edblack,
                                                                            (edpart_nearest_type->flags & PART_FLAG_IGNORE_TERRAIN) != 0,
                                                                            group++, edpartChangeInstanceFlag, "Ignore Terrain"));
            eduiMenuAddItem(edpart_instanceflags_menu,
                            eduiItemToggleCreate(PART_FLAG_FACE_MOVEMENT, edblack,
                                                 (edpart_nearest_type->flags & PART_FLAG_FACE_MOVEMENT) != 0, group++,
                                                 edpartChangeInstanceFlag, "Face Direction of Movement"));
            eduiMenuAddItem(edpart_instanceflags_menu, eduiItemToggleCreate(PART_FLAG_DISABLE_DRAW, edblack,
                                                                            (edpart_nearest_type->flags & PART_FLAG_DISABLE_DRAW) != 0,
                                                                            group++, edpartChangeInstanceFlag, "Disable Draw"));
            eduiMenuAddItem(edpart_instanceflags_menu,
                            eduiItemToggleCreate(PART_FLAG_REAL_TIME_LIGHTING, edblack,
                                                 (edpart_nearest_type->flags & PART_FLAG_REAL_TIME_LIGHTING) != 0, group++,
                                                 edpartChangeInstanceFlag, "Real Time Lighting"));
            eduiMenuAddItem(edpart_instanceflags_menu, eduiItemToggleCreate(PART_FLAG_IGNORE_CREATURE, edblack,
                                                                            (edpart_nearest_type->flags & PART_FLAG_IGNORE_CREATURE) != 0,
                                                                            group++, edpartChangeInstanceFlag, "Ignore Creature"));
            eduiMenuAddItem(edpart_instanceflags_menu,
                            eduiItemToggleCreate(PART_FLAG_THROWN, edblack, (edpart_nearest_type->flags & PART_FLAG_THROWN) != 0, group++,
                                                 edpartChangeInstanceFlag, "Thrown"));
            eduiMenuAddItem(edpart_instanceflags_menu, eduiItemToggleCreate(PART_FLAG_CAN_DAMAGE_OWNER, edblack,
                                                                            (edpart_nearest_type->flags & PART_FLAG_CAN_DAMAGE_OWNER) != 0,
                                                                            group++, edpartChangeInstanceFlag, "Can Damage Owner"));
        }
        eduiMenuAttach(menu, edpart_instanceflags_menu);
        edpart_instanceflags_menu->x = menu->x + 10;
        edpart_instanceflags_menu->y = menu->y + 40;
    }

    i32 edpart_set_debris = 0;

    void edpartChangeDebrisIndex(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
        if (edpart_nearest_type != NULL) {
            if (edpart_set_debris == 0) {
                edpart_nearest_type->trail_effects[0] = item->data;
            } else if (edpart_set_debris == 1) {
                edpart_nearest_type->trail_effects[1] = item->data;
            } else if (edpart_set_debris == 2) {
                edpart_nearest_type->attached_effect = item->data;
            } else if (edpart_set_debris == 3) {
                edpart_nearest_type->impact_effect = item->data;
            } else if (edpart_set_debris == 4) {
                edpart_nearest_type->kill_effect = item->data;
            }
        }
    }
    struct eduimenu_s *edpart_debrisindex_menu = 0;

    void edpartCancelDebrisIndexMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
        eduiMenuDestroy(edpart_debrisindex_menu);
        edpart_debrisindex_menu = NULL;
    }

    i32 edpart_particle_list = 0;

    void edpartDebrisIndexMenu(eduimenu_s *menu) {
        i32 chosen;
        i32 found;
        i32 index;
        edpart_debrisindex_menu = eduiMenuCreate(70, 70, 250, 250, ed_fnt, edpartCancelDebrisIndexMenu, "Debris Type");
        if (edpart_debrisindex_menu == NULL || edpart_nearest_type == NULL) {
            return;
        }
        chosen = -1;
        if (edpart_set_debris == 0) {
            chosen = edpart_nearest_type->trail_effects[0];
        } else if (edpart_set_debris == 1) {
            chosen = edpart_nearest_type->trail_effects[1];
        } else if (edpart_set_debris == 2) {
            chosen = edpart_nearest_type->attached_effect;
        } else if (edpart_set_debris == 3) {
            chosen = edpart_nearest_type->impact_effect;
        } else if (edpart_set_debris == 4) {
            chosen = edpart_nearest_type->kill_effect;
        }
        found = 0;
        for (index = 1; index < debinf_count; ++index) {
            if (debtab[index] != NULL && (i8)debtab[index]->category == edpart_particle_list) {
                i32 selected = chosen == index;
                eduiMenuAddItem(edpart_debrisindex_menu,
                                eduiItemCheckCreate(index, edblack, selected, 1, edpartChangeDebrisIndex, debtab[index]->name));
                if (selected == 1) {
                    edpart_debrisindex_menu->selected = edui_last_item;
                    found = selected;
                }
            }
        }
        eduiMenuSortItemsByTxt(edpart_debrisindex_menu);
        eduiMenuAddItemFirst(edpart_debrisindex_menu, eduiItemCheckCreate(-1, edblack, chosen == -1, 1, edpartChangeDebrisIndex, "None"));
        eduiMenuAttach(menu, edpart_debrisindex_menu);
        edpart_debrisindex_menu->x = menu->x + 10;
        edpart_debrisindex_menu->y = menu->y + 40;
        if (found) {
            edpart_debrisindex_menu->first_visible = edpart_debrisindex_menu->selected;
        }
    }

    void edpartGeneralDebrisIndexMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
        edpart_particle_list = 0;
        edpartDebrisIndexMenu(menu);
    }

    void edpartLevelDebrisIndexMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
        edpart_particle_list = 1;
        edpartDebrisIndexMenu(menu);
    }

    i32 edpart_set_part = 5;

    void edpartChangePartIndex(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
        if (edpart_nearest_type != NULL && edpart_set_part == 5) {
            edpart_nearest_type->impact_part = item->data;
        }
    }
    struct eduimenu_s *edpart_partindex_menu = 0;

    void edpartCancelPartIndexMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
        eduiMenuDestroy(edpart_partindex_menu);
        edpart_partindex_menu = NULL;
    }

    i32 edpart_part_list = 0;

    void edpartPartIndexMenu(eduimenu_s *menu) {
        i32 chosen;
        i32 index;
        edpart_partindex_menu = eduiMenuCreate(70, 70, 250, 250, ed_fnt, edpartCancelPartIndexMenu, "Part Type");
        if (edpart_partindex_menu == NULL || edpart_nearest_type == NULL) {
            return;
        }
        chosen = -1;
        if (edpart_set_part == 3) {
            chosen = edpart_nearest_type->impact_part;
        }
        eduiMenuAddItem(edpart_partindex_menu, eduiItemCheckCreate(-1, edblack, chosen == -1, 1, edpartChangePartIndex, "None"));
        for (index = 0; index < 128; ++index) {
            if (part_types[index].name[0] != '\0' && (i8)part_types[index].effect_list == edpart_part_list) {
                i32 selected = chosen == index;
                if (index == edpart_nearest_emit->effect_id) {
                    eduiMenuAddItem(edpart_partindex_menu, eduiItemCheckCreate(index, edgrey, 0, 0, NULL, part_types[index].name));
                } else {
                    eduiMenuAddItem(edpart_partindex_menu,
                                    eduiItemCheckCreate(index, edblack, selected, 1, edpartChangePartIndex, part_types[index].name));
                }
                if (selected == 1) {
                    edpart_partindex_menu->selected = edui_last_item;
                }
            }
        }
        eduiMenuAttach(menu, edpart_partindex_menu);
        edpart_partindex_menu->x = menu->x + 10;
        edpart_partindex_menu->y = menu->y + 40;
    }

    void edpartGeneralPartIndexMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
        edpart_part_list = 0;
        edpartPartIndexMenu(menu);
    }

    void edpartLevelPartIndexMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
        edpart_part_list = 1;
        edpartPartIndexMenu(menu);
    }

    void edpartChangeDebrisPerSec(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
        if (edpart_nearest_type != NULL) {
            if (edpart_set_debris == 0) {
                edpart_nearest_type->trail_rates[0] = (f32)(i32)((edui_slider_s *)item)->value;
            } else if (edpart_set_debris == 1) {
                edpart_nearest_type->trail_rates[1] = (f32)(i32)((edui_slider_s *)item)->value;
            }
        }
    }
    struct eduimenu_s *edpart_instancesettings_menu = 0;

    void edpartCancelInstanceSettingsMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
        eduiMenuDestroy(edpart_instancesettings_menu);
        edpart_instancesettings_menu = NULL;
    }

    void edpartInstanceSettingsMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
        if (edpart_nearest_type == NULL) {
            return;
        }
        edpart_instancesettings_menu = eduiMenuCreate(70, 70, 250, 300, ed_fnt, edpartCancelInstanceSettingsMenu, "Instance Settings");
        if (edpart_instancesettings_menu != NULL) {
            eduiMenuAddItem(edpart_instancesettings_menu,
                            eduiItemSelCreate(1, edblack, 0, 0, edpartChangeMaxLifeMenu, "Max Instance Life..."));
            eduiMenuAddItem(edpart_instancesettings_menu,
                            eduiItemSelCreate(1, edblack, 0, 0, edpartInstanceOrientMenu, "Instance Orientation..."));
            eduiMenuAddItem(edpart_instancesettings_menu,
                            eduiItemSelCreate(1, edblack, 0, 0, edpartInstanceFlagsMenu, "Instance Flags..."));
            eduiMenuAddItem(edpart_instancesettings_menu, eduiItemSelCreate(1, edblack, 0, 0, edpartTintMenu, "Instance Tint..."));
            eduiMenuAddItem(edpart_instancesettings_menu,
                            eduiItemSelCreate(1, edblack, 0, 0, edpartInstanceScaleMenu, "Instance Scale..."));
        }
        eduiMenuAttach(menu, edpart_instancesettings_menu);
        edpart_instancesettings_menu->x = menu->x + 10;
        edpart_instancesettings_menu->y = menu->y + 40;
    }
    struct eduimenu_s *edpart_trail1debris_menu = 0;

    void edpartCancelTrail1DebrisMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
        eduiMenuDestroy(edpart_trail1debris_menu);
        edpart_trail1debris_menu = NULL;
    }

    void edpartTrail1DebrisMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
        if (edpart_nearest_type == NULL) {
            return;
        }
        edpart_set_debris = 0;
        edpart_trail1debris_menu = eduiMenuCreate(70, 70, 300, 300, ed_fnt, edpartCancelTrail1DebrisMenu, "Trail 1 Debris");
        if (edpart_trail1debris_menu != NULL) {
            eduiMenuAddItem(edpart_trail1debris_menu, eduiItemSelCreate(1, edblack, 0, 0, edpartGeneralDebrisIndexMenu, "General List..."));
            if (edpart_nearest_type->effect_list == 1) {
                eduiMenuAddItem(edpart_trail1debris_menu, eduiItemSelCreate(1, edblack, 0, 0, edpartLevelDebrisIndexMenu, "Level List..."));
            } else {
                eduiMenuAddItem(edpart_trail1debris_menu, eduiItemSelCreate(1, edgrey, 0, 0, NULL, "Level List..."));
            }
            eduiMenuAddItem(edpart_trail1debris_menu,
                            eduiItemSliderCreateInt(0, edblack, 0, edpartChangeDebrisPerSec, 0, ((i32)(edpart_superscale * 1200.0f)),
                                                    ((i32)(edpart_nearest_type->trail_rates[0])), "Trail 1 particles per second"));
        }
        eduiMenuAttach(menu, edpart_trail1debris_menu);
        edpart_trail1debris_menu->x = menu->x + 10;
        edpart_trail1debris_menu->y = menu->y + 40;
    }
    struct eduimenu_s *edpart_trail2debris_menu = 0;

    void edpartCancelTrail2DebrisMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
        eduiMenuDestroy(edpart_trail2debris_menu);
        edpart_trail2debris_menu = NULL;
    }

    void edpartTrail2DebrisMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
        if (edpart_nearest_type == NULL) {
            return;
        }
        edpart_set_debris = 1;
        edpart_trail2debris_menu = eduiMenuCreate(70, 70, 300, 300, ed_fnt, edpartCancelTrail2DebrisMenu, "Trail 2 Debris");
        if (edpart_trail2debris_menu != NULL) {
            eduiMenuAddItem(edpart_trail2debris_menu, eduiItemSelCreate(1, edblack, 0, 0, edpartGeneralDebrisIndexMenu, "General List..."));
            if (edpart_nearest_type->effect_list == 1) {
                eduiMenuAddItem(edpart_trail2debris_menu, eduiItemSelCreate(1, edblack, 0, 0, edpartLevelDebrisIndexMenu, "Level List..."));
            } else {
                eduiMenuAddItem(edpart_trail2debris_menu, eduiItemSelCreate(1, edgrey, 0, 0, NULL, "Level List..."));
            }
            eduiMenuAddItem(edpart_trail2debris_menu,
                            eduiItemSliderCreateInt(0, edblack, 0, edpartChangeDebrisPerSec, 0, ((i32)(edpart_superscale * 1200.0f)),
                                                    ((i32)(edpart_nearest_type->trail_rates[1])), "Trail 2 particles per second"));
        }
        eduiMenuAttach(menu, edpart_trail2debris_menu);
        edpart_trail2debris_menu->x = menu->x + 10;
        edpart_trail2debris_menu->y = menu->y + 40;
    }
    struct eduimenu_s *edpart_emitterdebris_menu = 0;

    void edpartCancelEmitterDebrisMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
        eduiMenuDestroy(edpart_emitterdebris_menu);
        edpart_emitterdebris_menu = NULL;
    }

    void edpartEmitterDebrisMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
        if (edpart_nearest_type == NULL) {
            return;
        }
        edpart_set_debris = 2;
        edpart_emitterdebris_menu = eduiMenuCreate(70, 70, 250, 300, ed_fnt, edpartCancelEmitterDebrisMenu, "Emitter Debris");
        if (edpart_emitterdebris_menu != NULL) {
            eduiMenuAddItem(edpart_emitterdebris_menu,
                            eduiItemSelCreate(1, edblack, 0, 0, edpartGeneralDebrisIndexMenu, "General List..."));
            if (edpart_nearest_type->effect_list == 1) {
                eduiMenuAddItem(edpart_emitterdebris_menu,
                                eduiItemSelCreate(1, edblack, 0, 0, edpartLevelDebrisIndexMenu, "Level List..."));
            } else {
                eduiMenuAddItem(edpart_emitterdebris_menu, eduiItemSelCreate(1, edgrey, 0, 0, NULL, "Level List..."));
            }
        }
        eduiMenuAttach(menu, edpart_emitterdebris_menu);
        edpart_emitterdebris_menu->x = menu->x + 10;
        edpart_emitterdebris_menu->y = menu->y + 40;
    }
    struct eduimenu_s *edpart_impactdebris_menu = 0;

    void edpartCancelImpactDebrisMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
        eduiMenuDestroy(edpart_impactdebris_menu);
        edpart_impactdebris_menu = NULL;
    }

    void edpartImpactDebrisMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
        if (edpart_nearest_type == NULL) {
            return;
        }
        edpart_set_debris = 3;
        edpart_impactdebris_menu = eduiMenuCreate(70, 70, 250, 300, ed_fnt, edpartCancelImpactDebrisMenu, "Impact Debris");
        if (edpart_impactdebris_menu != NULL) {
            eduiMenuAddItem(edpart_impactdebris_menu, eduiItemSelCreate(1, edblack, 0, 0, edpartGeneralDebrisIndexMenu, "General List..."));
            if (edpart_nearest_type->effect_list == 1) {
                eduiMenuAddItem(edpart_impactdebris_menu, eduiItemSelCreate(1, edblack, 0, 0, edpartLevelDebrisIndexMenu, "Level List..."));
            } else {
                eduiMenuAddItem(edpart_impactdebris_menu, eduiItemSelCreate(1, edgrey, 0, 0, NULL, "Level List..."));
            }
        }
        eduiMenuAttach(menu, edpart_impactdebris_menu);
        edpart_impactdebris_menu->x = menu->x + 10;
        edpart_impactdebris_menu->y = menu->y + 40;
    }
    struct eduimenu_s *edpart_impactpart_menu = 0;

    void edpartCancelImpactPartMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
        eduiMenuDestroy(edpart_impactpart_menu);
        edpart_impactpart_menu = NULL;
    }

    void edpartImpactPartMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
        if (edpart_nearest_type == NULL) {
            return;
        }
        edpart_set_debris = 5;
        edpart_impactpart_menu = eduiMenuCreate(70, 70, 250, 300, ed_fnt, edpartCancelImpactPartMenu, "Impact Part");
        if (edpart_impactpart_menu != NULL) {
            eduiMenuAddItem(edpart_impactpart_menu, eduiItemSelCreate(1, edblack, 0, 0, edpartGeneralPartIndexMenu, "General List..."));
            if (edpart_nearest_type->effect_list == 1) {
                eduiMenuAddItem(edpart_impactpart_menu, eduiItemSelCreate(1, edblack, 0, 0, edpartLevelPartIndexMenu, "Level List..."));
            } else {
                eduiMenuAddItem(edpart_impactpart_menu, eduiItemSelCreate(1, edgrey, 0, 0, NULL, "Level List..."));
            }
        }
        eduiMenuAttach(menu, edpart_impactpart_menu);
        edpart_impactpart_menu->x = menu->x + 10;
        edpart_impactpart_menu->y = menu->y + 40;
    }
    struct eduimenu_s *edpart_diedebris_menu = 0;

    void edpartCancelDieDebrisMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
        eduiMenuDestroy(edpart_diedebris_menu);
        edpart_diedebris_menu = NULL;
    }

    void edpartDieDebrisMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
        if (edpart_nearest_type == NULL) {
            return;
        }
        edpart_set_debris = 4;
        edpart_diedebris_menu = eduiMenuCreate(70, 70, 250, 300, ed_fnt, edpartCancelDieDebrisMenu, "Die Debris");
        if (edpart_diedebris_menu != NULL) {
            eduiMenuAddItem(edpart_diedebris_menu, eduiItemSelCreate(1, edblack, 0, 0, edpartGeneralDebrisIndexMenu, "General List..."));
            if (edpart_nearest_type->effect_list == 1) {
                eduiMenuAddItem(edpart_diedebris_menu, eduiItemSelCreate(1, edblack, 0, 0, edpartLevelDebrisIndexMenu, "Level List..."));
            } else {
                eduiMenuAddItem(edpart_diedebris_menu, eduiItemSelCreate(1, edgrey, 0, 0, NULL, "Level List..."));
            }
        }
        eduiMenuAttach(menu, edpart_diedebris_menu);
        edpart_diedebris_menu->x = menu->x + 10;
        edpart_diedebris_menu->y = menu->y + 40;
    }

    void edpartChangeDebrisScale(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
        if (edpart_nearest_type) {
            edpart_nearest_type->effect_scale = ((edui_slider_s *)(item))->value;
        }
    }
    struct eduimenu_s *edpart_debrisscale_menu = 0;

    void edpartCancelDebrisScaleMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
        eduiMenuDestroy(edpart_debrisscale_menu);
        edpart_debrisscale_menu = NULL;
    }

    void edpartDebrisScaleMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
        edpart_debrisscale_menu = eduiMenuCreate(70, 70, 180, 300, ed_fnt, edpartCancelDebrisScaleMenu, "Debris Scale");
        if (edpart_debrisscale_menu != NULL && edpart_nearest_type != NULL) {
            eduiMenuAddItem(edpart_debrisscale_menu,
                            eduiItemSliderCreate(0, edblack, 0, edpartChangeDebrisScale, 0.0f, edpart_superscale * 10.0f,
                                                 edpart_nearest_type->effect_scale, "Debris Scale"));
            eduiMenuAttach(menu, edpart_debrisscale_menu);
            edpart_debrisscale_menu->x = menu->x + 10;
            edpart_debrisscale_menu->y = menu->y + 40;
        }
    }
    struct eduimenu_s *edpart_debrissettings_menu = 0;

    void edpartCancelDebrisSettingsMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
        eduiMenuDestroy(edpart_debrissettings_menu);
        edpart_debrissettings_menu = NULL;
    }

    void edpartDebrisSettingsMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
        if (edpart_nearest_type != NULL) {
            edpart_debrissettings_menu = eduiMenuCreate(70, 70, 300, 300, ed_fnt, edpartCancelDebrisSettingsMenu, "Debris Settings");
            if (edpart_debrissettings_menu != NULL) {
                eduiMenuAddItem(edpart_debrissettings_menu,
                                eduiItemSelCreate(1, edblack, 0, 0, edpartTrail1DebrisMenu, "Trail 1 Debris..."));
                eduiMenuAddItem(edpart_debrissettings_menu,
                                eduiItemSelCreate(1, edblack, 0, 0, edpartTrail2DebrisMenu, "Trail 2 Debris..."));
                eduiMenuAddItem(edpart_debrissettings_menu,
                                eduiItemSelCreate(1, edblack, 0, 0, edpartEmitterDebrisMenu, "Emitter Debris..."));
                eduiMenuAddItem(edpart_debrissettings_menu,
                                eduiItemSelCreate(1, edblack, 0, 0, edpartImpactDebrisMenu, "Impact Debris..."));
                eduiMenuAddItem(edpart_debrissettings_menu, eduiItemSelCreate(1, edblack, 0, 0, edpartDieDebrisMenu, "Die Debris..."));
                eduiMenuAddItem(edpart_debrissettings_menu, eduiItemSelCreate(1, edblack, 0, 0, edpartImpactPartMenu, "Impact Part..."));
                eduiMenuAddItem(edpart_debrissettings_menu,
                                eduiItemToggleCreate(PART_FLAG_DEBRIS_STOPS_WITH_PART, edblack, (edpart_nearest_type->flags >> 21) & 1, 1,
                                                     edpartChangeInstanceFlag, "Debris stops when Part stops"));
                eduiMenuAddItem(edpart_debrissettings_menu, eduiItemSelCreate(1, edblack, 0, 0, edpartDebrisScaleMenu, "Debris Scale..."));
            }
            eduiMenuAttach(menu, edpart_debrissettings_menu);
            edpart_debrissettings_menu->x = menu->x + 10;
            edpart_debrissettings_menu->y = menu->y + 40;
        }
    }
    struct eduimenu_s *edpart_emit_menu = 0;

    void edpartCancelEmitMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
        eduiMenuDestroy(edpart_emit_menu);
        edpart_emit_menu = NULL;
    }

    void edpartEmitMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
        if (edpart_nearest_type != NULL) {
            edpart_emit_menu = eduiMenuCreate(70, 70, 250, 300, ed_fnt, edpartCancelEmitMenu, "Emitter Settings");
            if (edpart_emit_menu != NULL) {
                eduiMenuAddItem(edpart_emit_menu, eduiItemSelCreate(1, edblack, 0, 0, edpartEmitVelMenu, "Emitter Vel..."));
                eduiMenuAddItem(edpart_emit_menu, eduiItemSelCreate(1, edblack, 0, 0, edpartGravMenu, "Gravity..."));
                eduiMenuAddItem(edpart_emit_menu, eduiItemSelCreate(1, edblack, 0, 0, edpartVarStartMenu, "Random Start..."));
                eduiMenuAddItem(edpart_emit_menu, eduiItemSelCreate(1, edblack, 0, 0, edpartVarEmitMenu, "Random Emit..."));
                eduiMenuAddItem(edpart_emit_menu, eduiItemSelCreate(1, edblack, 0, 0, edpartChangeGenRateMenu, "Emits per Sec..."));
                eduiMenuAddItem(edpart_emit_menu, eduiItemSelCreate(1, edblack, 0, 0, edpartEmitTimeMenu, "Emitter Timing..."));
                eduiMenuAddItem(edpart_emit_menu, eduiItemSelCreate(1, edblack, 0, 0, edpartCutOffMenu, "Radii..."));
            }
            eduiMenuAttach(menu, edpart_emit_menu);
            edpart_emit_menu->x = menu->x + 10;
            edpart_emit_menu->y = menu->y + 40;
        }
    }

    void edpartToggleFilter(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
        edpart_filter = item->highlighted;
    }

    void edpartChangeFilterName(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
        NuStrNCpy(edpart_filter_string, ((edui_textpicker_s *)item)->value, sizeof(edpart_filter_string));
    }
    struct eduimenu_s *edpart_opt_menu = 0;

    void edpartCancelOptMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
        edpart_active_menu = NULL;
        edpart_opt_menu = NULL;
    }

    i32 edpart_curr = 0;
    i32 edpart_first_time_this_level = 0;

    void edpartInit(void) {
        edpart_first_time_this_level = 1;
        edpart_mtl = NuMtlCreate3D(1);
        (edpart_mtl)->diffuse_r = 0.5f;
        (edpart_mtl)->diffuse_g = 0.5f;
        (edpart_mtl)->diffuse_b = 0.5f;
        (edpart_mtl)->attribs.cull_mode = NUMTL_CULL_NONE;
        (edpart_mtl)->attribs.z_mode = NUMTL_ZMODE_NORMAL;
        (edpart_mtl)->opacity = 1.0f;
        (edpart_mtl)->attribs.alpha_mode = NUMTL_ALPHA_OPAQUE;
        NuMtlUpdate(edpart_mtl);
        edpart_boxmtl = NuMtlCreate(1);
        (edpart_boxmtl)->diffuse_r = 0.5f;
        (edpart_boxmtl)->diffuse_g = 0.5f;
        (edpart_boxmtl)->diffuse_b = 0.5f;
        (edpart_boxmtl)->attribs.cull_mode = NUMTL_CULL_NONE;
        (edpart_boxmtl)->attribs.z_mode = NUMTL_ZMODE_OFF;
        (edpart_boxmtl)->opacity = 1.0f;
        (edpart_boxmtl)->attribs.alpha_mode = NUMTL_ALPHA_OPAQUE;
        NuMtlUpdate(edpart_boxmtl);
        edpart_nearest = -1;
        edpart_curr = -1;
        edpart_create_type = -1;
        edpart_effect_list = 0;
        edpart_emitrotz = 0;
        edpart_emitroty = 0;
        edpart_emitrotx = 0;
    }

    void edpartClose(void) {
        eduiMenuDestroy(edpart_opt_menu);
    }

    NUVEC edpart_entry_position = {0};

    /* Remembers where the editor was entered and moves the
     * camera there on the first entry of a level. */
    void edpartEnter(void) {
        NUVEC origin = {0.0f, 0.0f, 0.0f};
        if (edmainQueryLocVec() != NULL) {
            edpart_entry_position = *edmainQueryLocVec();
        } else {
            edpart_entry_position.x = global_camera.mtx.m30;
            edpart_entry_position.y = global_camera.mtx.m31;
            edpart_entry_position.z = global_camera.mtx.m32;
        }
        if (edpart_first_time_this_level != 0) {
            if (edmainQueryLocVec() != NULL) {
                edcamSetPosAng(edmainQueryLocVec(), 0, 0);
            } else {
                edcamSetPosAng(&origin, 0, 0);
            }
            edpart_first_time_this_level = 0;
        }
        edpart_nearest = -1;
    }

    void fun_0064a430(void) {
    }

    static i32 PartRaycastsEnabledPrivate = 1;

    void NuPartEnableRayCasts(i32 enabled) {
        PartRaycastsEnabledPrivate = enabled;
    }

    u32 partseed = 0x5c0999;

    void NuPartSetSeed(i32 seed) {
        partseed = (u32)seed;
    }

    u32 NuPartGetSeed(void) {
        return partseed;
    }

    f32 partglobaltime = 0;

    void NuPartResetGlobalTime(void) {
        partglobaltime = 0.0f;
    }

    i32 part_slot_cursor = -1;

    void ResetParts(void) {
        if (Part != NULL) {
            memset(Part, 0, MAXPARTS * sizeof(PART));
        }
        part_slot_cursor = 0;
    }

    i32 part_platimpactcnt = 0;

    void InitParts(i32 count, VARIPTR *buffer, VARIPTR end) {
        i32 i;
        i32 j;
        MAXPARTS = count;
        buffer->addr = (buffer->addr + 15) & ~(u32)15;
        Part = (PART *)buffer->void_ptr;
        buffer->addr += MAXPARTS * sizeof(PART);
        if (Part != NULL) {
            memset(Part, 0, MAXPARTS * sizeof(PART));
        }
        part_slot_cursor = 0;
        memset(part_types, 0, sizeof(part_types));
        for (i = 0; i < 128; ++i) {
            for (j = 0; j < 8; ++j) {
                part_types[i].effect_ids[j] = -1;
                part_types[i].effect_pages[j] = -1;
            }
        }
        part_types_used = 0;
        memset(part_emits, 0, sizeof(part_emits));
        for (i = 0; i < 40; ++i) {
            part_emits[i].effect_id = -1;
        }
        for (i = 0; i < 32; ++i) {
            part_scene_pageid[i] = -1;
        }
        part_emits_used = 0;
        for (i = 0; i < 8; ++i) {
            part_page_used[i] = 0;
            part_page_on[i] = 0;
        }
        for (i = 0; i < 32; ++i) {
            part_scene[i] = NULL;
        }
        part_platimpactcnt = 0;
    }

    void NewPartRotation(PART *part) {
        i32 index;
        for (index = 0; index < 2; ++index) {
            do {
                part->rotation_axes[index] = (u8)((NuRandIntSeeded(&partseed) % 65535u) / 21845u + 1);
            } while (index == 1 && part->rotation_axes[1] == part->rotation_axes[0]);
            part->spin_rates[index + 3] = (i32)(((f64)NuRandFloatSeeded(&partseed) * 2.0 - 1.0) * 65536.0);
        }
    }

    void NewPartOrderedRotation(PART *part) {
        part->spin_rates[0] =
            part->rotation_parameters[0] + (i32)(((f64)NuRandFloatSeeded(&partseed) * 2.0 - 1.0) * part->rotation_parameters[3]);
        part->spin_rates[1] =
            part->rotation_parameters[1] + (i32)(((f64)NuRandFloatSeeded(&partseed) * 2.0 - 1.0) * part->rotation_parameters[4]);
        part->spin_rates[2] =
            part->rotation_parameters[2] + (i32)(((f64)NuRandFloatSeeded(&partseed) * 2.0 - 1.0) * part->rotation_parameters[5]);
    }

    void PartTimeSlip(void) {
        i32 i;
        PARTTYPE_LOOKUP *types = part_types;
        for (i = 0; i < 40; ++i) {
            if (part_emits[i].effect_id != -1) {
                part_emits[i].time_20 -= 800.0f;
                part_emits[i].time_24 -= 800.0f;
                part_emits[i].time_28 -= 800.0f;
            }
        }
        for (i = 0; i < 128; ++i) {
            if (types[i].effect_ids[0] != -1) {
                types[i].last_used_time -= 800.0f;
            }
        }
        partglobaltime -= 800.0f;
    }

    PART *HitParts(GAMEOBJECT *excluded_owner, NUVEC *points, i32 point_count, f32 radius, NUVEC *minimum, NUVEC *maximum, u32 flags) {
        PART *part = Part;
        i32 index;

        for (index = 0; index < MAXPARTS; ++index, ++part) {
            i32 point;
            if ((part->active & PART_IN_USE) == 0 || part->owner == excluded_owner || (part->flags & flags) == 0) {
                continue;
            }
            if (maximum->x < part->collision_min.x || minimum->x > part->collision_max.x || maximum->z < part->collision_min.z ||
                minimum->z > part->collision_max.z || maximum->y < part->collision_min.y || minimum->y > part->collision_max.y) {
                continue;
            }
            for (point = point_count - 1; point >= 0; --point) {
                NUVEC difference;
                f32 combined_radius;
                NuVecSub(&difference, &points[point], &part->position);
                combined_radius = radius + part->radius;
                if (difference.x * difference.x + difference.y * difference.y + difference.z * difference.z <=
                    combined_radius * combined_radius) {
                    return part;
                }
            }
        }
        return NULL;
    }

    void KillPart(PART *part, i32 reason) {
        if ((part->active & PART_IN_USE) == 0) {
            return;
        }
        part->active &= ~PART_IN_USE;
        if ((part->flags & 0x20000) != 0 && part->debris_handle != NULL) {
            DebFreeInstantly(part->debris_handle);
        }
        if (part->lighting_handle != NULL) {
            rtlDynamicFree(*part->lighting_handle);
            *part->lighting_handle = -1;
        }
        if (part->kill_fn != NULL) {
            part->kill_fn(part, reason);
        }
        if (reason != 6 && part->effect_id_1f4 != -1) {
            i32 handle = -1;
            AddScaledFiniteShotDebrisEffect(&handle, part->effect_id_1f4, &part->position, NULL, NULL, 1, part->effect_scale);
        }
    }

    void KillAllParts(void) {
        PART *part = Part;
        i32 i;
        if (part != NULL) {
            for (i = 0; i < MAXPARTS; ++i, ++part) {
                if ((part->active & PART_IN_USE) != 0) {
                    KillPart(part, 0);
                }
            }
        }
    }

    PART *FindPart(NUVEC *position, i32 player, GAMEOBJECT *owner) {
        PART *part;
        f32 nearest;
        PART *result;
        i32 i;
        if (Part == NULL) {
            return NULL;
        }
        part = Part;
        nearest = 1000000.0f;
        result = NULL;
        for (i = 0; i < MAXPARTS; ++i, ++part) {
            if ((part->active & PART_IN_USE) != 0 && (player == -1 || part->player_mask == player) &&
                (owner == NULL || part->owner == owner)) {
                if (position != NULL) {
                    f32 distance = NuVecDistSqr(position, &part->position, NULL);
                    if (distance < nearest) {
                        nearest = distance;
                        result = part;
                    }
                } else {
                    return part;
                }
            }
        }
        return result;
    }

    RTLSET *PartRTL = 0;

    void SetPartRTLSet(RTLSET *rtl_set) {
        PartRTL = rtl_set;
    }

    void PARTStartOffset(i32 index, f32 offset) {
        PART_EMIT *emitter;
        PARTTYPE_LOOKUP *type;
        f32 period;
        f64 random_product;
        f32 interval;
        f32 phase_product;
        if (index == -1) {
            return;
        }
        emitter = &part_emits[index];
        if (emitter->effect_id == -1) {
            return;
        }
        type = &part_types[emitter->effect_id];
        if (type->emission_period_random == 0.0f && type->emission_pause_random == 0.0f) {
            period = type->emission_period + type->emission_pause;
            phase_product = (i32)(partglobaltime / period) * period;
            emitter->time_24 = phase_product + offset;
            emitter->time_28 = emitter->time_24 + type->emission_period;
            while (partglobaltime > emitter->time_24 && partglobaltime > emitter->time_28) {
                if (emitter->time_24 >= emitter->time_28) {
                    emitter->time_28 = emitter->time_24 + type->emission_period;
                } else {
                    emitter->time_24 = emitter->time_28 + type->emission_pause;
                }
            }
        } else {
            emitter->time_24 = partglobaltime;
            random_product = (f64)NuRandFloatSeeded(&partseed) * type->emission_period_random;
            emitter->time_28 = ((f64)type->emission_period + emitter->time_24) + random_product;
        }
        interval = 1.0f / type->emission_rate;
        if (partglobaltime > emitter->time_28) {
            emitter->time_20 = emitter->time_24 - interval;
        } else {
            emitter->time_20 = (i32)(partglobaltime / interval) * interval;
        }
        emitter->time_38 = partglobaltime;
    }

    i32 PARTLookupType(char *name) {
        i32 index;
        if (name == NULL || name[0] == 0) {
            return -1;
        }
        for (index = 0; index < 128; ++index) {
            if (NuStrNICmp(name, part_types[index].name, 16) == 0) {
                return index;
            }
        }
        return -1;
    }

    i32 PARTLookupTypePageOnly(char *name, i32 page) {
        i32 i;
        if (name == NULL || name[0] == 0) {
            return -1;
        }
        if ((u32)(page - 1) <= 6) {
            for (i = 0; i < 128; ++i) {
                if (part_types[i].page == page && NuStrNICmp(name, part_types[i].name, 16) == 0) {
                    return i;
                }
            }
        }
        for (i = 0; i < 128; ++i) {
            if (part_types[i].page == 0 && NuStrNCmp(name, part_types[i].name, 16) == 0) {
                return i;
            }
        }
        return -1;
    }

    i32 GetPartCount(void) {
        i32 n;
        i32 used = 0;
        for (n = 0; n < 40; ++n) {
            if (part_emits[n].effect_id != -1) {
                ++used;
            }
        }
        return used;
    }

    i32 GetMaxPartTypes(void) {
        return 128;
    }

    char *GetPartName(i32 type) {
        if (part_types[type].name[0] == 0) {
            return NULL;
        }
        return part_types[type].name;
    }

    void fun_0064b150(void) {
    }

    f32 PARTGetTotalOnTime(i32 type) {
        if (type >= 0) {
            return part_types[type].emission_period + part_types[type].emission_period_random;
        }
        return 0.0f;
    }

    f32 PARTGetTotalOffTime(i32 type) {
        if (type >= 0) {
            return part_types[type].emission_pause + part_types[type].emission_pause_random;
        }
        return 0.0f;
    }

    i32 AddPARTEffect(i32 effect, NUVEC *position) {
        i32 i;
        if (part_emits_used == 40) {
            return -1;
        }
        if (part_types[effect].effect_ids[0] == -1) {
            return -1;
        }
        for (i = 0; i < 40; ++i) {
            if (part_emits[i].effect_id == -1) {
                part_emits[i].position = *position;
                part_emits[i].effect_id = effect;
                NuStrNCpy(part_emits[i].name, part_types[effect].name, sizeof(part_emits[i].name));
                PARTStartOffset(i, 0.0f);
                part_emits[i].camera_distance = 9999.9f;
                ++part_emits_used;
                part_emits[i].rotation_30 = 0;
                part_emits[i].rotation_2e = 0;
                part_emits[i].rotation_2c = 0;
                part_emits[i].page = part_types[effect].page;
                part_emits[i].shots_remaining = 0;
                part_emits[i].variant = 0;
                part_emits[i].trigger_type = 0;
                part_emits[i].trigger_id = 0;
                part_emits[i].state = 1;
                return i;
            }
        }
        return -1;
    }

    void RemovePARTEffect(i32 index) {
        if (index != -1 && part_emits[index].effect_id != -1) {
            --part_emits_used;
            part_emits[index].effect_id = -1;
        }
    }

    void PARTEmitterPos(i32 index, f32 x, f32 y, f32 z) {
        if (index != -1 && part_emits[index].effect_id != -1) {
            part_emits[index].position.x = x;
            part_emits[index].position.y = y;
            part_emits[index].position.z = z;
        }
    }

    void PARTEmitterOrientation(i32 index, i16 emitrotz, i16 emitroty, i16 emitrotx) {
        if (index != -1 && part_emits[index].effect_id != -1) {
            part_emits[index].rotation_30 = emitrotx;
            part_emits[index].rotation_2e = emitroty;
            part_emits[index].rotation_2c = emitrotz;
        }
    }

    i32 AddFiniteShotPART(i32 effect, NUVEC *position, i32 count) {
        i32 index;
        if (effect == -1) {
            return effect;
        }
        if (part_types[effect].effect_ids[0] == -1) {
            return -1;
        }
        index = AddPARTEffect(effect, position);
        if (index != -1) {
            part_emits[index].shots_remaining = count;
            part_emits[index].time_24 = partglobaltime;
            part_emits[index].time_28 = part_types[effect].emission_period + partglobaltime +
                                        NuRandFloatSeeded(&partseed) * part_types[effect].emission_period_random;
            part_emits[index].time_20 = partglobaltime - 1.0f / part_types[effect].emission_rate;
        }
        return index;
    }

    i32 CreateScaledPARTEffect(i32 effect_index, f32 requested_scale) {
        i32 closest_index;
        i32 i;
        i32 result = 0;
        f32 ratio;
        f32 closest_distance;
        // Inclusive upper check; loaded type IDs are 0..127.
        if (effect_index < 1) {
            return -1;
        }
        if (effect_index > 128) {
            return -1;
        }
        if (part_types[effect_index].effect_ids[0] == -1) {
            return -1;
        }
        if (part_types[effect_index].scale != 1.0f) {
            // The final word identifies the original, unscaled type.
            effect_index = part_types[effect_index].source_type;
            if (part_types[effect_index].effect_ids[0] == -1) {
                return -1;
            }
        }
        if (requested_scale == 1.0f) {
            result = effect_index;
        }
        if (requested_scale < 0.01f) {
            requested_scale = 0.01f;
        }

        if (result == 0) {
            ratio = requested_scale / part_types[effect_index].scale;
            if (ratio <= 1.0f) {
                ratio = 1.0f - ratio;
            } else {
                ratio = ratio - 1.0f;
            }
            closest_index = effect_index;
            closest_distance = ratio;
            for (i = 0; i < 128; ++i) {
                if (part_types[i].effect_ids[0] != -1 && part_types[i].source_type == (u32)effect_index) {
                    ratio = requested_scale / part_types[i].scale;
                    ratio = ratio <= 1.0f ? 1.0f - ratio : ratio - 1.0f;
                    if (ratio < closest_distance) {
                        closest_index = i;
                        closest_distance = ratio;
                    }
                }
            }
            if (closest_index != 0 && closest_distance < 1.1f) {
                return closest_index;
            }
            for (i = 0; i < 128; ++i) {
                PARTTYPE_LOOKUP *scaled;
                PARTTYPE_LOOKUP *source;
                if (part_types[i].effect_ids[0] != -1) {
                    continue;
                }
                scaled = &part_types[i];
                source = &part_types[effect_index];
                *scaled = *source;
                edpartScaleType(i, requested_scale);
                scaled->last_used_time = partglobaltime;
                scaled->scale = requested_scale;
                scaled->source_type = effect_index;
                if (strlen(source->name) > 12) {
                    char shortened[16];
                    sprintf(shortened, "%s", source->name);
                    shortened[12] = '\0';
                    sprintf(scaled->name, "%s%03d", shortened, i);
                } else {
                    sprintf(scaled->name, "%s%03d", source->name, i);
                }
                ++part_types_used;
                result = i;
                break;
            }
            if (result == 0) {
                result = closest_index;
            }
        }
        return result;
    }

    i32 AddScaledFiniteShotPART(i32 effect, NUVEC *position, i32 count, f32 scale) {
        i32 scaled = CreateScaledPARTEffect(effect, scale);
        if (scaled != -1) {
            AddFiniteShotPART(scaled, position, count);
        }
        return scaled;
    }

    void KillPartsByScene(NUGSCN *scene) {
        PART *part = Part;
        i32 index;
        for (index = 0; index < MAXPARTS; ++index, ++part) {
            if ((part->active & PART_IN_USE) != 0 && part->source_special != NULL && part->special.scene == scene) {
                nuhspecial_s *special = &part->special;
                special->scene = NULL;
                special->special = NULL;
                special->display_special = NULL;
                part->active &= ~PART_IN_USE;
                part->source_special = NULL;
            }
        }
    }

    static void PartRuntimeMatrixSetIdentity(NUMTX *matrix) {
        PartRuntimeMatrixCopyPrivate(matrix, (NUMTX *)&ANI3_EVAL_IDENTITY_MATRIX);
    }

    void edpartDestroy(i32 index) {
        if (part_emits[index].effect_id != -1) {
            --part_emits_used;
            part_emits[index].effect_id = -1;
        }
    }

    void edpartCreate(NUVEC *position, i32 type) {
        AddPARTEffect(type, position);
    }

    i32 edpartPtlChangeType(i32 index, i32 type) {
        if (part_emits[index].effect_id != type) {
            edpartDestroy(index);
            AddDebrisEffect(&part_emits[index].instance_id, type, part_emits[index].position.x, part_emits[index].position.y,
                            part_emits[index].position.z);
            part_emits[index].effect_id = type;
        }
        return index;
    }

    void edpartDestroyAllEffects(void) {
        i32 i;
        // Walks 512 slots here although part_emits holds 40.
        for (i = 0; i < 512; ++i) {
            if (part_emits[i].effect_id != -1) {
                part_emits[i].effect_id = -1;
                --part_emits_used;
            }
        }
    }

    void edpartDeleteType(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
        struct eduimenu_s *parent;
        i32 emitter;
        parent = NULL;
        if (edpart_nearest_type == &part_types[edpart_create_type]) {
            edpart_nearest = -1;
            edpart_nearest_emit = NULL;
            edpart_nearest_type = NULL;
        }
        for (emitter = 0; emitter < 40; ++emitter) {
            if (part_emits[emitter].effect_id == edpart_create_type && part_emits[emitter].effect_id != -1) {
                part_emits[emitter].effect_id = -1;
                --part_emits_used;
            }
        }
        edpartDetermineNearest(1.0f);
        part_types[edpart_create_type].name[0] = '\0';
        part_types[edpart_create_type].effect_ids[0] = -1;
        --part_types_used;
        if (menu->parent) {
            parent = menu->parent;
            eduiMenuDetach(menu);
        }
        if (menu->callback) {
            menu->callback(menu, parent);
        }
        edpart_create_type = -1;
    }

    void edpartLoadSingleType(PARTTYPE_LOOKUP *type, i32 version, i32 page) {
        char name[16];
        i32 i;
        i32 scene;
        char mode;
        EdFileRead(type->name, 16);
        if (version >= 6) {
            for (i = 0; i < 8; ++i) {
                type->effect_pages[i] = EdFileReadChar();
            }
        } else {
            if (version >= 2) {
                type->effect_pages[0] = EdFileReadChar();
            } else {
                type->effect_pages[0] = 0;
            }
            for (i = 1; i < 8; ++i) {
                type->effect_pages[i] = -1;
            }
        }
        if (version >= 6) {
            for (i = 0; i < 8; ++i) {
                EdFileRead(type->object_names[i], 16);
                type->effect_ids[i] = -1;
                if (type->effect_pages[i] == 0) {
                    for (scene = 0; scene < 32; ++scene) {
                        if (part_scene_pageid[scene] == page) {
                            type->effect_ids[i] = edpartLookupObjectInScene(type->object_names[i], part_scene[scene]);
                        }
                        if (type->effect_ids[i] != -1) {
                            type->scene_indices[i] = scene;
                            break;
                        }
                    }
                } else if (type->effect_pages[i] == 1) {
                    for (scene = 0; scene < 32; ++scene) {
                        if (part_scene_pageid[scene] == 0) {
                            type->effect_ids[i] = edpartLookupObjectInScene(type->object_names[i], part_scene[scene]);
                        }
                        if (type->effect_ids[i] != -1) {
                            type->scene_indices[i] = scene;
                            break;
                        }
                    }
                }
                if (type->effect_ids[i] == -1 && type->object_names[i][0] != 0) {
                    type->effect_ids[i] = 9998;
                }
            }
        } else {
            EdFileRead(type->object_names[0], 16);
            type->effect_ids[0] = -1;
            if (type->effect_pages[0] == 0) {
                for (scene = 0; scene < 32; ++scene) {
                    if (part_scene_pageid[scene] == page) {
                        type->effect_ids[0] = edpartLookupObjectInScene(type->object_names[0], part_scene[scene]);
                    }
                    if (type->effect_ids[0] != -1) {
                        type->scene_indices[0] = scene;
                        break;
                    }
                }
            } else if (type->effect_pages[0] == 1) {
                for (scene = 0; scene < 32; ++scene) {
                    if (part_scene_pageid[scene] == 0) {
                        type->effect_ids[0] = edpartLookupObjectInScene(type->object_names[0], part_scene[scene]);
                    }
                    if (type->effect_ids[0] != -1) {
                        type->scene_indices[0] = scene;
                        break;
                    }
                }
            }
            if (type->effect_ids[0] == -1 && type->object_names[0][0] != 0) {
                type->effect_ids[0] = 9998;
            }
        }
        type->variant_count = 0;
        for (i = 0; i < 8; ++i) {
            if (type->effect_ids[i] != -1 && type->effect_ids[i] != 9999) {
                ++type->variant_count;
            }
        }
        if (version >= 6) {
            type->variant_mode = EdFileReadChar();
        } else {
            type->variant_mode = 0;
        }
        if (version >= 16) {
            type->particle_scale = EdFileReadFloat();
            type->effect_scale = EdFileReadFloat();
        } else {
            type->particle_scale = 1.0f;
            type->effect_scale = 1.0f;
        }
        type->lifetime = EdFileReadFloat();
        if (version >= 14) {
            type->lifetime_random = EdFileReadFloat();
        } else {
            type->lifetime_random = 0.0f;
        }
        type->speed = EdFileReadFloat();
        type->gravity = EdFileReadFloat();
        type->emission_rate = EdFileReadFloat();
        if (version >= 8) {
            type->bounce = EdFileReadFloat();
        } else {
            type->bounce = 0.75f;
        }
        type->position_random.x = EdFileReadFloat();
        type->position_random.y = EdFileReadFloat();
        type->position_random.z = EdFileReadFloat();
        type->velocity_random.x = EdFileReadFloat();
        type->velocity_random.y = EdFileReadFloat();
        type->velocity_random.z = EdFileReadFloat();
        type->emission_period = EdFileReadFloat();
        type->emission_period_random = EdFileReadFloat();
        type->emission_pause = EdFileReadFloat();
        type->emission_pause_random = EdFileReadFloat();
        if (version >= 13) {
            type->rotation[0] = EdFileReadInt();
            type->rotation[1] = EdFileReadInt();
            type->rotation[2] = EdFileReadInt();
            type->rotation_random[0] = EdFileReadInt();
            type->rotation_random[1] = EdFileReadInt();
            type->rotation_random[2] = EdFileReadInt();
        } else {
            type->rotation[0] = EdFileReadShort();
            type->rotation[1] = EdFileReadShort();
            type->rotation[2] = EdFileReadShort();
            type->rotation_random[0] = EdFileReadShort();
            type->rotation_random[1] = EdFileReadShort();
            type->rotation_random[2] = EdFileReadShort();
        }
        type->flags = EdFileReadUnsignedInt();
        if (type->effect_ids[0] == 9999) {
            type->flags |= 0x10;
        }
        if (version >= 9) {
            EdFileRead(name, 16);
            type->trail_effects[0] = (edpart_load_particle_page == -1 ? LookupDebrisEffect(name)
                                                                      : LookupDebrisEffectPageOnly(name, (i8)edpart_load_particle_page));
            EdFileRead(name, 16);
            type->trail_effects[1] = (edpart_load_particle_page == -1 ? LookupDebrisEffect(name)
                                                                      : LookupDebrisEffectPageOnly(name, (i8)edpart_load_particle_page));
            EdFileRead(name, 16);
            type->attached_effect = (edpart_load_particle_page == -1 ? LookupDebrisEffect(name)
                                                                     : LookupDebrisEffectPageOnly(name, (i8)edpart_load_particle_page));
            type->trail_rates[0] = EdFileReadFloat();
            type->trail_rates[1] = EdFileReadFloat();
        } else if (version >= 5) {
            EdFileRead(name, 16);
            type->trail_effects[0] = (edpart_load_particle_page == -1 ? LookupDebrisEffect(name)
                                                                      : LookupDebrisEffectPageOnly(name, (i8)edpart_load_particle_page));
            type->trail_effects[1] = -1;
            EdFileRead(name, 16);
            type->attached_effect = (edpart_load_particle_page == -1 ? LookupDebrisEffect(name)
                                                                     : LookupDebrisEffectPageOnly(name, (i8)edpart_load_particle_page));
            type->trail_rates[0] = EdFileReadFloat();
            type->trail_rates[1] = 60.0f;
        } else if (version >= 3) {
            type->trail_effects[0] = -1;
            type->trail_effects[1] = -1;
            type->attached_effect = -1;
            mode = EdFileReadChar();
            EdFileRead(name, 16);
            if (mode == 0) {
                type->trail_effects[0] =
                    (edpart_load_particle_page == -1 ? LookupDebrisEffect(name)
                                                     : LookupDebrisEffectPageOnly(name, (i8)edpart_load_particle_page));
            } else if (mode == 1) {
                type->attached_effect = (edpart_load_particle_page == -1 ? LookupDebrisEffect(name)
                                                                         : LookupDebrisEffectPageOnly(name, (i8)edpart_load_particle_page));
            }
            type->trail_rates[0] = EdFileReadFloat();
            type->trail_rates[1] = 60.0f;
        } else {
            type->trail_effects[0] = -1;
            type->trail_effects[1] = -1;
            type->attached_effect = -1;
            type->trail_rates[0] = 60.0f;
            type->trail_rates[1] = 60.0f;
        }
        if (version >= 4) {
            EdFileRead(name, 16);
            type->kill_effect = (edpart_load_particle_page == -1 ? LookupDebrisEffect(name)
                                                                 : LookupDebrisEffectPageOnly(name, (i8)edpart_load_particle_page));
            EdFileRead(name, 16);
            type->impact_effect = (edpart_load_particle_page == -1 ? LookupDebrisEffect(name)
                                                                   : LookupDebrisEffectPageOnly(name, (i8)edpart_load_particle_page));
        } else {
            type->impact_effect = -1;
            type->kill_effect = -1;
        }
        if (version >= 12) {
            EdFileRead(type->impact_part_name, 16);
            type->impact_part = PARTLookupType(type->impact_part_name);
        } else {
            type->impact_part_name[0] = 0;
            type->impact_part = -1;
        }
        if (version >= 7) {
            for (i = 0; i < 4; ++i) {
                EdFileRead(name, 16);
                type->sounds[i] = GetSfxIdN(name, 16);
            }
            for (i = 0; i < 4; ++i) {
                type->sound_modes[i] = EdFileReadChar();
            }
        } else {
            for (i = 0; i < 4; ++i) {
                type->sounds[i] = -1;
                type->sound_modes[i] = 0;
            }
        }
        if (version >= 10) {
            type->maximum_distance = EdFileReadFloat();
        } else {
            type->maximum_distance = 25.0f;
        }
        if (version >= 15) {
            type->lighting_scale.x = EdFileReadFloat();
            type->lighting_scale.y = EdFileReadFloat();
            type->lighting_scale.z = EdFileReadFloat();
        } else {
            type->lighting_scale.x = 1.0f;
            type->lighting_scale.y = 1.0f;
            type->lighting_scale.z = 1.0f;
        }
        type->scale = 1.0f;
        type->source_type = 0;
    }

    i32 PartLevelPage = 1;
    i32 PartGeneralPage = 0;

    i32 edpartLoadPageEx(char *path, i8 mode, NUGSCN **scenes, i32 scene_count) {
        i32 page;
        i32 scene_slot;
        i32 i;
        i32 version;
        i32 type_count;
        i32 skipped_types;
        i32 type_slot;
        i32 emitter_count;
        i32 emitter_slot;
        i32 sound;
        PARTTYPE_LOOKUP discarded_type;
        if (mode == 0) {
            page = 0;
        } else {
            page = -1;
            for (i = 1; i < 8; ++i) {
                if (part_page_used[i] == 0) {
                    page = i;
                    break;
                }
            }
            if (page == -1) {
                return -1;
            }
        }
        scene_slot = 0;
        for (i = 0; i < scene_count; ++i) {
            if (scenes[i] != NULL) {
                while (part_scene[scene_slot] != NULL && scene_slot < 32) {
                    ++scene_slot;
                }
                if (scene_slot < 32) {
                    part_scene[scene_slot] = scenes[i];
                    part_scene_pageid[scene_slot] = page;
                }
            }
        }
        EdFileSetMedia(1);
        if (EdFileOpen(path, 0) != 0) {
            EdFileSetReadWrongEndianess(1);
            version = EdFileReadInt();
            // MATCH-EXCEPTION: the original tests the low and high version bounds separately; one && folds them
            // into a single unsigned range compare.
            if (version < 1) {
                EdFileClose();
                return -1;
            }
            if (version <= 16) {
                type_count = EdFileReadInt();
                type_slot = 0;
                skipped_types = 0;
                if (type_count + part_types_used > 128) {
                    skipped_types = type_count + part_types_used - 128;
                    type_count -= skipped_types;
                }
                for (i = 0; i < type_count; ++i) {
                    while (part_types[type_slot].name[0] != 0) {
                        ++type_slot;
                    }
                    edpartLoadSingleType(&part_types[type_slot], version, page);
                    if (part_types[type_slot].name[0] != 0) {
                        part_types[type_slot].page = page;
                        part_types[type_slot].effect_list = mode;
                        ++part_types_used;
                    }
                }
                for (i = 0; i < skipped_types; ++i) {
                    edpartLoadSingleType(&discarded_type, version, page);
                }
                emitter_count = EdFileReadInt();
                emitter_slot = 0;
                if (emitter_count + part_emits_used > 40) {
                    emitter_count = 40 - part_emits_used;
                }
                for (i = 0; i < emitter_count; ++i) {
                    while (part_emits[emitter_slot].effect_id != -1) {
                        ++emitter_slot;
                    }
                    part_emits[emitter_slot].position.x = EdFileReadFloat();
                    part_emits[emitter_slot].position.y = EdFileReadFloat();
                    part_emits[emitter_slot].position.z = EdFileReadFloat();
                    EdFileRead(part_emits[emitter_slot].name, 16);
                    part_emits[emitter_slot].effect_id = PARTLookupTypePageOnly(part_emits[emitter_slot].name, page);
                    part_emits[emitter_slot].rotation_30 = EdFileReadShort();
                    part_emits[emitter_slot].rotation_2e = EdFileReadShort();
                    part_emits[emitter_slot].rotation_2c = EdFileReadShort();
                    if (version >= 11) {
                        part_emits[emitter_slot].trigger_type = EdFileReadShort();
                        part_emits[emitter_slot].trigger_id = EdFileReadShort();
                    } else {
                        part_emits[emitter_slot].trigger_type = 0;
                        part_emits[emitter_slot].trigger_id = 0;
                    }
                    part_emits[emitter_slot].page = page;
                    part_emits[emitter_slot].enabled = 0;
                    for (sound = 0; sound < 4; ++sound) {
                        if (part_types[part_emits[emitter_slot].effect_id].sounds[sound] != -1 &&
                            part_types[part_emits[emitter_slot].effect_id].sound_modes[sound] != 0) {
                            part_emits[emitter_slot].enabled = 1;
                            break;
                        }
                    }
                    if (part_emits[emitter_slot].effect_id != -1) {
                        ++part_emits_used;
                        PARTStartOffset(emitter_slot, 0.0f);
                        part_emits[emitter_slot].state = 1;
                    }
                }
                EdFileSetReadWrongEndianess(0);
                EdFileClose();
                edpartDetermineNearest(1.0f);
                part_page_used[page] = 1;
                if (page == 0) {
                    part_page_on[0] = 1;
                }
                if (mode == 0) {
                    PartGeneralPage = page;
                } else if (mode == 1) {
                    PartLevelPage = page;
                }
                return page;
            }
            EdFileClose();
        }
        return -1;
    }

    i32 edpartLoadPage(char *path, i8 mode, NUGSCN *scene) {
        return edpartLoadPageEx(path, mode, &scene, 1);
    }

    void edpartClearPage(i8 page) {
        i32 index;
        NuThreadDisableThreadSwap();
        if (part_page_on[page] != 0) {
            part_page_on[page] = 0;
        }
        for (index = 0; index < 128; ++index) {
            if (part_types[index].page == page && part_types[index].name[0] != 0) {
                part_types[index].name[0] = 0;
                part_types[index].effect_ids[0] = -1;
                --part_types_used;
            }
        }
        for (index = 0; index < 40; ++index) {
            if (part_emits[index].page == page && part_emits[index].effect_id != -1) {
                part_emits[index].effect_id = -1;
                --part_emits_used;
            }
        }
        for (index = 0; index < 32; ++index) {
            if (part_scene_pageid[index] == page) {
                KillPartsByScene(part_scene[index]);
                part_scene[index] = NULL;
                part_scene_pageid[index] = -1;
            }
        }
        NuThreadEnableThreadSwap();
        part_page_used[page] = 0;
    }

    void edpartFileLoadEffects(eduimenu_s *parent, eduiitem_s *unused1, u32 unused2) {
        char path[256];
        char general_directory[256];
        char general_name[256];
        char general_extension[256];
        char level_directory[256];
        char level_name[256];
        char level_extension[256];
        i32 i;
        i32 index;
        i32 variant;
        i8 page;
        NUGSCN *scene;
        if (Part != NULL) {
            memset(Part, 0, MAXPARTS * sizeof(PART));
        }
        part_slot_cursor = 0;
        memset(part_types, 0, sizeof(part_types));
        for (index = 0; index < 128; ++index) {
            for (variant = 0; variant < 8; ++variant) {
                part_types[index].effect_ids[variant] = -1;
                part_types[index].effect_pages[variant] = -1;
            }
        }
        part_types_used = 0;
        memset(part_emits, 0, sizeof(part_emits));
        for (index = 0; index < 40; ++index) {
            part_emits[index].effect_id = -1;
        }
        for (index = 0; index < 32; ++index) {
            part_scene_pageid[index] = -1;
        }
        part_emits_used = 0;
        for (index = 0; index < 8; ++index) {
            part_page_used[index] = 0;
            part_page_on[index] = 0;
        }
        for (index = 0; index < 32; ++index) {
            part_scene[index] = NULL;
        }
        part_platimpactcnt = 0;
        if (edbits_general_save_directory[0] == 0) {
            sprintf(general_directory, ".");
        } else {
            i = 0;
            do {
                general_directory[i] = edbits_general_save_directory[i];
            } while (general_directory[i++] != 0);
        }
        if (edbits_general_save_name[0] == 0) {
            sprintf(general_name, "part");
        } else {
            i = 0;
            do {
                general_name[i] = edbits_general_save_name[i];
            } while (general_name[i++] != 0);
        }
        if (edbits_general_save_extension[0] == 0) {
            sprintf(general_extension, "par");
        } else {
            i = 0;
            do {
                general_extension[i] = edbits_general_save_extension[i];
            } while (general_extension[i++] != 0);
        }
        if (edbits_level_save_directory[0] == 0) {
            sprintf(level_directory, ".");
        } else {
            i = 0;
            do {
                level_directory[i] = edbits_level_save_directory[i];
            } while (level_directory[i++] != 0);
        }
        if (edbits_level_save_name[0] == 0) {
            sprintf(level_name, "part");
        } else {
            i = 0;
            do {
                level_name[i] = edbits_level_save_name[i];
            } while (level_name[i++] != 0);
        }
        if (edbits_level_save_extension[0] == 0) {
            sprintf(level_extension, "par");
        } else {
            i = 0;
            do {
                level_extension[i] = edbits_level_save_extension[i];
            } while (level_extension[i++] != 0);
        }
        sprintf(path, "%s\\%s.%s", general_directory, general_name, general_extension);
        if (NuFileExists(path)) {
            scene = edbits_things_scene;
            edpartLoadPageEx(path, 0, &scene, 1);
        }
        sprintf(path, "%s\\%s.%s", level_directory, level_name, level_extension);
        if (NuFileExists(path)) {
            scene = edbits_base_scene;
            page = (i8)edpartLoadPageEx(path, 1, &scene, 1);
            for (index = 0; index < 40; ++index) {
                if (part_emits[index].effect_id != -1 && part_emits[index].page == page) {
                    part_emits[index].camera_distance = 9999.9f;
                }
            }
            part_page_on[page] = 1;
        }
        edpartSaveMessage(parent, "Loaded OK", 1);
    }

    void edpartDataMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
        edpart_data_menu = eduiMenuCreate(70, 70, 250, 250, ed_fnt, edpartCancelDataMenu, "Data Menu");
        if (edpart_data_menu != NULL) {
            if (edpart_create_type != -1) {
                eduiMenuAddItem(edpart_data_menu, eduiItemSelCreate(1, edblack, 0, 0, edpartChangeNameMenu, "Type Name..."));
            } else {
                eduiMenuAddItem(edpart_data_menu, eduiItemSelCreate(1, edgrey, 0, 0, NULL, "Type Name..."));
            }
            if (edpart_create_type != -1) {
                eduiMenuAddItem(edpart_data_menu, eduiItemSelCreate(1, edblack, 0, 0, edpartCopyType, "Copy Type"));
                eduiMenuAddItem(edpart_data_menu, eduiItemSelCreate(1, edblack, 0, 0, edpartDeleteType, "Delete Type"));
                if (part_types[edpart_create_type].effect_list == 0) {
                    eduiMenuAddItem(edpart_data_menu, eduiItemSelCreate(1, edblack, 0, 0, edpartMoveList, "Move Type to Level List"));
                } else {
                    eduiMenuAddItem(edpart_data_menu, eduiItemSelCreate(1, edblack, 0, 0, edpartMoveList, "Move Type to General List"));
                }
            } else {
                eduiMenuAddItem(edpart_data_menu, eduiItemSelCreate(1, edgrey, 0, 0, NULL, "Copy Type"));
                eduiMenuAddItem(edpart_data_menu, eduiItemSelCreate(1, edgrey, 0, 0, NULL, "Delete Type"));
                eduiMenuAddItem(edpart_data_menu, eduiItemSelCreate(1, edgrey, 0, 0, NULL, "Move Type"));
            }
            eduiMenuAddItem(edpart_data_menu, eduiItemSelCreate(1, edblack, 0, 0, edpartFileSaveEffectsGeneral, "Save General List"));
            eduiMenuAddItem(edpart_data_menu, eduiItemSelCreate(1, edblack, 0, 0, edpartFileSaveEffectsLevel, "Save Level List"));
            eduiMenuAddItem(edpart_data_menu, eduiItemSelCreate(1, edblack, 0, 0, edpartFileSaveEffects, "Save All"));
            eduiMenuAddItem(edpart_data_menu, eduiItemSelCreate(1, edblack, 0, 0, edpartFileLoadEffects, "Load All"));
            eduiMenuAttach(menu, edpart_data_menu);
            edpart_data_menu->x = menu->x + 10;
            edpart_data_menu->y = menu->y + 40;
        }
    }

    void edpartOptMenu(void) {
        edpart_opt_menu = eduiMenuCreate(70, 70, 250, 300, ed_fnt, edpartCancelOptMenu, "PART Editor");
        if (edpart_opt_menu == NULL) {
            return;
        }
        eduiMenuAddItem(edpart_opt_menu, eduiItemSelCreate(1, edblack, 0, 0, edpartTypeMenu, "Emitter Type..."));
        if (edpart_nearest != -1) {
            eduiMenuAddItem(edpart_opt_menu, eduiItemSelCreate(1, edblack, 0, 0, edpartInstanceMenu, "Instance Select..."));
            eduiMenuAddItem(edpart_opt_menu, eduiItemSelCreate(1, edblack, 0, 0, edpartInstanceSettingsMenu, "Instance Settings..."));
            eduiMenuAddItem(edpart_opt_menu, eduiItemSelCreate(1, edblack, 0, 0, edpartEmitMenu, "Emitter Settings..."));
            eduiMenuAddItem(edpart_opt_menu, eduiItemSelCreate(1, edblack, 0, 0, edpartDebrisSettingsMenu, "Debris Settings..."));
            eduiMenuAddItem(edpart_opt_menu, eduiItemSelCreate(1, edblack, 0, 0, edpartSoundsMenu, "Attached Sounds..."));
            eduiMenuAddItem(edpart_opt_menu, eduiItemSelCreate(1, edblack, 0, 0, edpartSwitchMenu, "Switch Menu..."));
            eduiMenuAddItem(edpart_opt_menu, eduiItemSelCreate(1, edblack, 0, 0, edpartScaleTypeMenu, "Scale Type..."));
        } else {
            eduiMenuAddItem(edpart_opt_menu, eduiItemSelCreate(1, edgrey, 0, 0, NULL, "Instance Select..."));
            eduiMenuAddItem(edpart_opt_menu, eduiItemSelCreate(1, edgrey, 0, 0, NULL, "Instance Settings..."));
            eduiMenuAddItem(edpart_opt_menu, eduiItemSelCreate(1, edgrey, 0, 0, NULL, "Emitter Settings..."));
            eduiMenuAddItem(edpart_opt_menu, eduiItemSelCreate(1, edgrey, 0, 0, NULL, "Debris Settings..."));
            eduiMenuAddItem(edpart_opt_menu, eduiItemSelCreate(1, edgrey, 0, 0, NULL, "Attached Sounds..."));
            eduiMenuAddItem(edpart_opt_menu, eduiItemSelCreate(1, edgrey, 0, 0, NULL, "Switch Menu..."));
            eduiMenuAddItem(edpart_opt_menu, eduiItemSelCreate(1, edgrey, 0, 0, NULL, "Scale Type..."));
        }
        eduiMenuAddItem(edpart_opt_menu, eduiItemSelCreate(1, edblack, 0, 0, edpartDataMenu, "Data Menu..."));
        eduiMenuAddItem(edpart_opt_menu, eduiItemSelCreate(1, edblack, 0, 0, edpartSScaleMenu, "Super Scale..."));
        eduiMenuAddItem(edpart_opt_menu, eduiItemToggleCreate(1, edblack, edpart_filter, 1, edpartToggleFilter, "Instance Filter"));
        eduiMenuAddItem(edpart_opt_menu, eduiItemTextPickCreate(0, edblack, edpartChangeFilterName, "Filter String: "));
        strcpy(((edui_textpicker_s *)edui_last_item)->value, edpart_filter_string);
        ((edui_textpicker_s *)edui_last_item)->max_length = 15;
    }

    PART *AddPart(ADDPART *parameters) {
        NUVEC forward = {0.0f, 0.0f, -1.0f};
        PART *part;
        i32 scanned;
        i32 index;
        u8 hostility_group;

        if (parameters->special == NULL && parameters->part_id != -1 && parameters->part_id != 9999) {
            return NULL;
        }
        if (parameters->special != NULL && NuSpecialExistsFn(parameters->special) == 0) {
            return NULL;
        }
        scanned = 0;
        while ((Part[part_slot_cursor].active & PART_IN_USE) != 0 &&
               (Part[part_slot_cursor].flags & PART_FLAG_SLOT_CANNOT_BE_STOLEN) != 0 && scanned < MAXPARTS) {
            if (++part_slot_cursor == MAXPARTS) {
                part_slot_cursor = 0;
            }
            ++scanned;
        }
        if (scanned == MAXPARTS) {
            return NULL;
        }
        part = &Part[part_slot_cursor];
        if ((part->active & PART_IN_USE) != 0) {
            if (part->destroy_fn != NULL) {
                part->destroy_fn(part);
            }
            KillPart(part, 0);
        }
        if (parameters->matrix != NULL) {
            NUMTX matrix;
            part->transform = *parameters->matrix;
            matrix = *parameters->matrix;
            NuVecMtxRotate(&forward, &forward, &matrix);
            part->rotation_y = (u16)NuAtan2D(forward.x, forward.z);
            NuVecRotateY(&forward, &forward, -part->rotation_y);
            part->rotation_x = (u16)-NuAtan2D(forward.y, forward.z);
        } else if (parameters->position != NULL) {
            NuMtxSetTranslation(&part->transform, parameters->position);
        } else {
            return NULL;
        }
        part->previous_position = part->position;
        part->owner = parameters->owner;
        part->recipient = parameters->recipient;
        part->velocity = *parameters->velocity;
        part->time_remaining = part->initial_time = parameters->fade_time;
        part->player_mask = -1;
        part->effect_slot = -1;
        part->scale_time = 0.0f;
        part->time_step = parameters->time_step;
        part->active = (part->active & ~PART_STATE_SCALING_DOWN) | PART_STATE_SCALING_UP;
        part->radius = parameters->start_radius;
        part->glow_radius = (part->time_step / PARTSCALEUPTIME) * part->radius;
        part->collision_radius = parameters->end_radius;
        part->particle_scale = parameters->model_scale;
        part->gravity = parameters->gravity;
        part->bounce = parameters->bounce;
        part->cutoff_height = parameters->lifetime;
        part->part_id = (i16)parameters->part_id;
        part->active = (part->active & ~PART_STATE_STOPPED) | PART_IN_USE;
        part->flags = parameters->flags;
        part->flicker_flags = parameters->flicker_flags;
        part->active &= ~PART_STATE_BEEPED;
        if (parameters->lighting_data != NULL) {
            part->lighting[0] = parameters->lighting_data->intensity_vectors[0];
            part->lighting[3] = parameters->lighting_data->direction[0];
            part->lighting[1] = parameters->lighting_data->intensity_vectors[1];
            part->lighting[4] = parameters->lighting_data->direction[1];
            part->lighting[2] = parameters->lighting_data->intensity_vectors[2];
            part->lighting[5] = parameters->lighting_data->direction[2];
            part->lighting[6] = parameters->lighting_data->ambient;
            part->active |= PART_STATE_LIT;
        } else {
            part->active &= ~PART_STATE_LIT;
            if (parameters->lighting_scale_x != 1.0f || parameters->lighting_scale_y != 1.0f || parameters->lighting_scale_z != 1.0f) {
                // lighting_scale_x/y/z are one NUVEC in the original (struct copy).
                part->lighting[6] = *(NUVEC *)&parameters->lighting_scale_x;
                part->active |= PART_STATE_LIGHT_SCALE;
            } else {
                part->active &= ~PART_STATE_LIGHT_SCALE;
            }
        }
        part->source_special = parameters->special;
        if (part->source_special != NULL) {
            part->special = *part->source_special;
        } else {
            memset(&part->special, 0, sizeof(part->special));
        }
        part->active &= ~0x14u;
        part->time_of_impact = 0.0f;
        part->collision_min.x = part->position.x - part->radius;
        part->collision_max.x = part->position.x + part->radius;
        part->collision_min.y = part->position.y - part->radius;
        part->collision_max.y = part->position.y + part->radius;
        part->collision_min.z = part->position.z - part->radius;
        part->collision_max.z = part->position.z + part->radius;
        part->callback_1ac = parameters->callback_30;
        part->move_fn = parameters->move_fn;
        part->deflect_fn = parameters->deflect_fn;
        part->impact_fn = parameters->impact_fn;
        part->collision_fn = parameters->collision_fn;
        part->kill_fn = parameters->kill_fn;
        part->update_fn = parameters->update_fn;
        part->stop_fn = parameters->stop_fn;
        part->destroy_fn = parameters->destroy_fn;
        part->draw_fn = parameters->draw_fn;
        memcpy(part->pickup_data, parameters->rotation_parameters, sizeof(part->pickup_data));
        for (index = 0; index < 5; ++index) {
            part->spin_rates[index] = 0;
        }
        if ((part->flags & PART_FLAG_ROTATE_RANDOMLY) != 0) {
            NewPartRotation(part);
        }
        if ((part->flags & PART_FLAG_ORDERED_ROTATE_RANDOMLY) != 0) {
            NewPartOrderedRotation(part);
        }
        if (parameters->effect_id_58 != -1) {
            i32 effect;
            part->inline_debris_handle = -1;
            part->debris_handle = &part->inline_debris_handle;
            if (parameters->effect_scale != 1.0f) {
                effect = CreateScaledEffect(parameters->effect_id_58, parameters->effect_scale);
            } else {
                effect = parameters->effect_id_58;
            }
            AddDebrisEffect(part->debris_handle, effect, part->position.x, part->position.y, part->position.z);
            DebrisPopulateInstance(*part->debris_handle, 0.0f);
        } else {
            part->debris_handle = parameters->debris_handle;
        }
        part->effect_id_1e0 = parameters->effect_id_60;
        part->trail_rate_a = parameters->trail_rate_a;
        part->effect_id_1e4 = parameters->effect_id_64;
        part->trail_rate_b = parameters->trail_rate_b;
        part->effect_id_1f0 = parameters->effect_id_70;
        part->effect_id_1f8 = parameters->effect_id_74;
        part->effect_id_1f4 = parameters->effect_id_78;
        part->effect_scale = parameters->effect_scale;
        part->previous_transform = part->transform;
        part->user_word = 0;
        part->first_frame = 1;
        part->force_flags = 0;
        part->trail_state = 0;
        hostility_group = (u8)parameters->hostility_group;
        part->active |= 0x1800;
        part->hostility_group = hostility_group;
        part->damage = 1;
        if ((part->flags & PART_FLAG_REAL_TIME_LIGHTING) != 0 && parameters->lighting_handle != NULL && PartRTL != NULL) {
            part->lighting_handle = parameters->lighting_handle;
            *parameters->lighting_handle = rtlDynamicAllocTemplate(PartRTL, parameters->lighting_id);
        }
        if (++part_slot_cursor == MAXPARTS) {
            part_slot_cursor = 0;
        }
        return part;
    }

    static i32 PartCleanupCursorPrivate = 0;
    static i32 PartCleanupFramesPrivate = 0;

    void PartCleanupTypes(void) {
        i32 i;
        if (++PartCleanupFramesPrivate > 5) {
            PartCleanupFramesPrivate = 0;
            if (part_types[PartCleanupCursorPrivate].effect_ids[0] != -1 &&
                partglobaltime > (f64)part_types[PartCleanupCursorPrivate].last_used_time + 5.0 &&
                part_types[PartCleanupCursorPrivate].scale != 1.0f) {
                for (i = 0; i < 40; ++i) {
                    if (part_emits[i].effect_id == PartCleanupCursorPrivate) {
                        if (part_emits[i].effect_id != -1) {
                            part_emits[i].effect_id = -1;
                            --part_emits_used;
                        }
                    }
                }
                part_types[PartCleanupCursorPrivate].name[0] = 0;
                part_types[PartCleanupCursorPrivate].effect_ids[0] = -1;
                --part_types_used;
            }
            if (++PartCleanupCursorPrivate >= 128) {
                PartCleanupCursorPrivate = 0;
            }
        }
    }

    ADDPART Default_ADDPART = {
        0,                                    // matrix
        0,                                    // position
        0,                                    // velocity
        0,                                    // owner
        0,                                    // recipient
        0.1f,                                 // unknown_14
        0.1f,                                 // unknown_18
        -5.0f,                                // gravity
        0.75f,                                // bounce
        0,                                    // special
        -1,                                   // part_id
        0x00000000u,                          // flags
        0,                                    // callback_30
        0,                                    // callback_34
        0,                                    // callback_38
        0,                                    // callback_3c
        0,                                    // callback_40
        0,                                    // callback_44
        0,                                    // callback_48
        0,                                    // callback_4c
        0,                                    // callback_50
        0,                                    // callback_54
        -1,                                   // effect_id_58
        0,                                    // debris_handle
        -1,                                   // effect_id_60
        -1,                                   // effect_id_64
        60.0f,                                // unknown_68
        60.0f,                                // unknown_6c
        -1,                                   // effect_id_70
        -1,                                   // effect_id_74
        -1,                                   // effect_id_78
        1.0f,                                 // effect_scale
        0,                                    // lighting_handle
        -1,                                   // lighting_id
        2000000.0f,                           // unknown_88
        0.016666668f,                         // time_step
        0xffffffffu,                          // hostility_group
        0,                                    // lighting_data
        1.0f,                                 // lighting_scale_x
        1.0f,                                 // lighting_scale_y
        1.0f,                                 // lighting_scale_z
        0.0f,                                 // unknown_a4
        {0x0u, 0x0u, 0x0u, 0x0u, 0x0u, 0x0u}, // rotation_parameters
        1.0f,                                 // unknown_c0
        0,                                    // flicker_flags
    };

    void UpdatePartEmits(f32 time) {
        i32 switch_changes[32][2];
        i32 i;
        i32 sound;
        i32 axis;
        i32 remaining;
        i32 variant;
        PART_EMIT *emitter;
        PARTTYPE_LOOKUP *type;
        ADDPART params;
        NUVEC position;
        NUVEC velocity;
        NUMTX matrix;
        nuhspecial_s special;
        f32 interval;
        f32 shift;
        f32 previous_distance;
        f32 emission_time;
        f32 pause;
        f32 range;
        i16 angle_x;
        i16 angle_y;
        i16 angle_z;
        i32 last_switch_change = -1;
        for (i = 0; i < 40; ++i) {
            emitter = &part_emits[i];
            if (part_page_on[emitter->page] == 0 || emitter->effect_id == -1) {
                continue;
            }
            type = &part_types[emitter->effect_id];
            if (type->effect_ids[0] == -1) {
                continue;
            }
            interval = 1.0f / type->emission_rate;
            if (emitter->trigger_type == 1 && emitter->state != 2) {
                if (emitter->trigger_id != -1) {
                    switch (debris_object_switches[emitter->trigger_id]) {
                        case 0:
                            emitter->state = 0;
                            break;
                        case 1:
                            emitter->state = 1;
                            break;
                        case 2:
                            last_switch_change = (last_switch_change + 1) & 31;
                            emitter->state = 2;
                            switch_changes[last_switch_change][1] = 4;
                            switch_changes[last_switch_change][0] = emitter->trigger_id;
                            break;
                        case 3:
                            emitter->state = 2;
                            last_switch_change = (last_switch_change + 1) & 31;
                            switch_changes[last_switch_change][1] = 0;
                            switch_changes[last_switch_change][0] = emitter->trigger_id;
                            break;
                        case 4:
                            last_switch_change = (last_switch_change + 1) & 31;
                            switch_changes[last_switch_change][1] = 0;
                            switch_changes[last_switch_change][0] = emitter->trigger_id;
                            break;
                    }
                }
                if (emitter->state == 2) {
                    PARTStartOffset(i, 0.0f);
                    shift = emitter->time_24 - partglobaltime;
                    emitter->time_24 -= shift;
                    emitter->time_28 -= shift;
                    emitter->time_20 = partglobaltime - interval;
                }
            }
            previous_distance = emitter->camera_distance;
            emitter->camera_distance = CameraEmitterDistance(&emitter->position);
            if (emitter->enabled != 0) {
                for (sound = 0; sound < 4; ++sound) {
                    if (type->sounds[sound] != -1) {
                        SetSfxBit_On(type->sounds[sound]);
                        if (type->sound_modes[sound] == 4) {
                            PlaySfxById(type->sounds[sound], &emitter->position);
                        }
                    }
                }
            }
            remaining = 0;
            if ((type->flags & 0x400000) != 0) {
                if (emitter->camera_distance < type->maximum_distance && previous_distance >= type->maximum_distance) {
                    emitter->time_20 = partglobaltime - interval + 0.001f;
                    emitter->time_24 = partglobaltime;
                    emitter->time_28 = emitter->time_24 - 1.0f;
                } else if (emitter->time_24 > emitter->time_28) {
                    emitter->time_28 += time;
                    emitter->time_24 += time;
                    emitter->time_20 += time;
                }
            }
            emission_time = emitter->time_20 + interval;
            while (partglobaltime + time > emission_time && remaining++ < 100) {
                while (emission_time >= emitter->time_28 && emission_time > emitter->time_24 && partglobaltime + time > emission_time) {
                    if (emitter->time_24 >= emitter->time_28) {
                        emitter->time_28 =
                            NuRandFloatSeeded(&partseed) * type->emission_period_random + (type->emission_period + emitter->time_24);
                        if (emitter->enabled != 0 && emitter->state != 0) {
                            for (sound = 0; sound < 4; ++sound) {
                                if (type->sounds[sound] != -1 && type->sound_modes[sound] == 1) {
                                    PlaySfxById(type->sounds[sound], &emitter->position);
                                }
                            }
                        }
                        if (emitter->shots_remaining != 0 && --emitter->shots_remaining == 0) {
                            RemovePARTEffect(i);
                            emission_time = partglobaltime + 99999.9f;
                        }
                    } else {
                        pause = NuRandFloatSeeded(&partseed) * type->emission_pause_random + type->emission_pause;
                        emitter->time_24 = emitter->time_28 + pause;
                        emission_time = emitter->time_24 + 0.001f;
                        emitter->time_20 = emission_time - interval + 0.001f;
                        if (emitter->state == 2) {
                            emitter->state = 0;
                        }
                        if (emitter->enabled != 0 && emitter->state != 0) {
                            for (sound = 0; sound < 4; ++sound) {
                                if (type->sounds[sound] != -1 && type->sound_modes[sound] == 2) {
                                    PlaySfxById(type->sounds[sound], &emitter->position);
                                }
                            }
                        }
                    }
                }
                if (partglobaltime + time <= emission_time) {
                    continue;
                }
                if (emitter->state != 0 && (type->maximum_distance == 0.0f || type->maximum_distance > emitter->camera_distance)) {
                    params = Default_ADDPART;
                    params.model_scale = type->particle_scale;
                    position.x = NuRandFloatSeeded(&partseed) * 2.0f * type->position_random.x - type->position_random.x;
                    position.y = NuRandFloatSeeded(&partseed) * 2.0f * type->position_random.y - type->position_random.y;
                    position.z = NuRandFloatSeeded(&partseed) * 2.0f * type->position_random.z - type->position_random.z;
                    NuVecRotateZ(&position, &position, emitter->rotation_2c);
                    NuVecRotateY(&position, &position, emitter->rotation_2e);
                    NuVecRotateX(&position, &position, emitter->rotation_30);
                    NuVecAdd(&position, &position, &emitter->position);
                    params.position = &position;
                    velocity.x = 0.0f;
                    velocity.y = type->speed;
                    velocity.z = 0.0f;
                    velocity.x += NuRandFloatSeeded(&partseed) * 2.0f * type->velocity_random.x - type->velocity_random.x;
                    velocity.y += NuRandFloatSeeded(&partseed) * 2.0f * type->velocity_random.y - type->velocity_random.y;
                    velocity.z += NuRandFloatSeeded(&partseed) * 2.0f * type->velocity_random.z - type->velocity_random.z;
                    NuVecRotateZ(&velocity, &velocity, emitter->rotation_2c);
                    NuVecRotateY(&velocity, &velocity, emitter->rotation_2e);
                    NuVecRotateX(&velocity, &velocity, emitter->rotation_30);
                    params.velocity = &velocity;
                    params.gravity = type->gravity;
                    params.bounce = type->bounce;
                    variant = 0;
                    if (type->variant_count != 0) {
                        if (type->variant_mode == 0) {
                            variant = (i32)(NuRandFloatSeeded(&partseed) * 65535.0f) % type->variant_count;
                        } else if (type->variant_mode == 1) {
                            // MATCH-EXCEPTION: the original treats unknown_33 as signed; the header declares it u8.
                            if (++*(i8 *)&emitter->variant >= type->variant_count) {
                                emitter->variant = 0;
                            }
                            variant = *(i8 *)&emitter->variant;
                        }
                    }
                    params.part_id = type->effect_ids[variant];
                    if (type->effect_ids[variant] == 9999 || type->effect_ids[variant] == 9998) {
                        params.special = NULL;
                    } else if (type->effect_ids[variant] != -1) {
                        params.special = &special;
                        NuGScnGetSpecial(&special, part_scene[type->scene_indices[variant]], params.part_id);
                    }
                    params.fade_time = type->lifetime + NuRandFloatSeeded(&partseed) * type->lifetime_random;
                    NuMtxSetIdentity(&matrix);
                    range = (f32)type->rotation_random[0];
                    angle_x = (i16)(i32)(NuRandFloatSeeded(&partseed) * 2.0f * range + type->rotation[0] - range);
                    range = (f32)type->rotation_random[1];
                    angle_y = (i16)(i32)(NuRandFloatSeeded(&partseed) * 2.0f * range + type->rotation[1] - range);
                    range = (f32)type->rotation_random[2];
                    angle_z = (i16)(i32)(NuRandFloatSeeded(&partseed) * 2.0f * range + type->rotation[2] - range);
                    for (axis = 0; axis < 3; ++axis) {
                        params.rotation_parameters[axis] = type->rotation[axis];
                        params.rotation_parameters[axis + 3] = type->rotation_random[axis];
                    }
                    NuMtxRotateX(&matrix, angle_x);
                    NuMtxRotateY(&matrix, angle_y);
                    NuMtxRotateZ(&matrix, angle_z);
                    matrix.m30 = position.x;
                    matrix.m31 = position.y;
                    matrix.m32 = position.z;
                    params.matrix = &matrix;
                    params.flags = type->flags & ~0x60000;
                    if (type->trail_effects[0] != -1) {
                        params.effect_id_60 = type->trail_effects[0];
                        params.flags |= 0x40000;
                    }
                    if (type->trail_effects[1] != -1) {
                        params.effect_id_64 = type->trail_effects[1];
                        params.flags |= 0x40000;
                    }
                    if (type->attached_effect != -1) {
                        params.effect_id_58 = type->attached_effect;
                        params.flags |= 0x20000;
                    }
                    params.trail_rate_a = type->trail_rates[0];
                    params.trail_rate_b = type->trail_rates[1];
                    params.effect_id_70 = type->impact_effect;
                    if (type->impact_part == -1 && type->impact_part_name[0] != 0) {
                        type->impact_part = PARTLookupType(type->impact_part_name);
                    }
                    params.effect_id_74 = type->impact_part;
                    params.effect_scale = type->effect_scale;
                    params.lighting_scale_x = type->lighting_scale.x;
                    params.lighting_scale_y = type->lighting_scale.y;
                    params.lighting_scale_z = type->lighting_scale.z;
                    params.effect_id_78 = type->kill_effect;
                    AddPart(&params);
                    if (emitter->enabled != 0) {
                        for (sound = 0; sound < 4; ++sound) {
                            if (type->sounds[sound] != -1 && type->sound_modes[sound] == 3) {
                                PlaySfxById(type->sounds[sound], &emitter->position);
                            }
                        }
                    }
                    type->last_used_time = emission_time;
                }
                emitter->time_20 = emission_time;
                emission_time += interval;
            }
        }
        for (i = 0; i <= last_switch_change; ++i) {
            debris_object_switches[switch_changes[i][0]] = switch_changes[i][1];
        }
    }

    void AddVariableShotPARTEffect(i32 effect_index, NUVEC *position, f32 rate, f32 duration, NUMTX *orientation) {
        PARTTYPE_LOOKUP *type = &part_types[effect_index];
        f32 interval;
        f32 aligned_time;
        f32 emission_time;
        f32 end_time;
        i32 count;
        if (type->name[0] == 0 || type->emission_rate <= 0.0f) {
            return;
        }
        if (rate <= 0.0f) {
            rate = type->emission_rate;
        }
        if (orientation == NULL) {
            orientation = &NuMtxSetIdentityTemplate;
        }
        if (rate != 0.0f) {
            interval = 1.0f / rate;
        } else {
            interval = 0.0f;
        }
        end_time = partglobaltime + duration;
        emission_time = PartsFdiv(partglobaltime, interval);
        aligned_time = (i32)emission_time * interval;
        emission_time = aligned_time + interval;
        for (count = 0; end_time >= emission_time && ++count < 100;) {
            ADDPART params = Default_ADDPART;
            NUVEC part_position;
            NUVEC velocity;
            nuhspecial_s special;
            NUMTX matrix;
            f32 range;
            i16 angle_x;
            i16 angle_y;
            i16 angle_z;
            i32 variant;
            i32 axis;
            i32 sound;
            part_position.x = NuRandFloatSeeded(&partseed) * 2.0f * type->position_random.x - type->position_random.x;
            part_position.y = NuRandFloatSeeded(&partseed) * 2.0f * type->position_random.y - type->position_random.y;
            part_position.z = NuRandFloatSeeded(&partseed) * 2.0f * type->position_random.z - type->position_random.z;
            NuVecMtxTransformVU0(&part_position, &part_position, orientation);
            NuVecAdd(&part_position, &part_position, position);
            velocity.x = velocity.z = 0.0f;
            velocity.y = type->speed;
            velocity.x += NuRandFloatSeeded(&partseed) * 2.0f * type->velocity_random.x - type->velocity_random.x;
            velocity.y += NuRandFloatSeeded(&partseed) * 2.0f * type->velocity_random.y - type->velocity_random.y;
            velocity.z += NuRandFloatSeeded(&partseed) * 2.0f * type->velocity_random.z - type->velocity_random.z;
            NuVecMtxTransformVU0(&velocity, &velocity, orientation);
            params.gravity = type->gravity;
            params.position = &part_position;
            params.velocity = &velocity;
            params.bounce = type->bounce;
            if (type->variant_count == 0) {
                variant = 0;
            } else if (type->variant_mode == 0 || type->variant_mode == 1) {
                variant = (i32)((f64)NuRandFloatSeeded(&partseed) * 65535.0) % type->variant_count;
            } else {
                variant = 0;
            }
            params.part_id = type->effect_ids[variant];
            if (type->effect_ids[variant] == 9999) {
                params.special = NULL;
            } else if (type->effect_ids[variant] != -1) {
                params.special = &special;
                NuGScnGetSpecial(&special, part_scene[type->scene_indices[variant]], params.part_id);
            }
            params.fade_time = type->lifetime + NuRandFloatSeeded(&partseed) * type->lifetime_random;
            NuMtxSetIdentity(&matrix);
            range = (f32)type->rotation_random[0];
            angle_x = (i16)(i32)(NuRandFloatSeeded(&partseed) * 2.0f * range + type->rotation[0] - range);
            range = (f32)type->rotation_random[1];
            angle_y = (i16)(i32)(NuRandFloatSeeded(&partseed) * 2.0f * range + type->rotation[1] - range);
            range = (f32)type->rotation_random[2];
            angle_z = (i16)(i32)(NuRandFloatSeeded(&partseed) * 2.0f * range + type->rotation[2] - range);
            for (axis = 0; axis < 3; ++axis) {
                params.rotation_parameters[axis] = type->rotation[axis];
                params.rotation_parameters[axis + 3] = type->rotation_random[axis];
            }
            NuMtxRotateX(&matrix, angle_x);
            NuMtxRotateY(&matrix, angle_y);
            NuMtxRotateZ(&matrix, angle_z);
            matrix.m30 = part_position.x;
            matrix.m31 = part_position.y;
            matrix.m32 = part_position.z;
            params.matrix = &matrix;
            params.flags = type->flags & ~0x60000;
            if (type->trail_effects[0] != -1) {
                params.effect_id_60 = type->trail_effects[0];
                params.flags |= 0x40000;
            }
            if (type->trail_effects[1] != -1) {
                params.effect_id_64 = type->trail_effects[1];
                params.flags |= 0x40000;
            }
            if (type->attached_effect != -1) {
                params.effect_id_58 = type->attached_effect;
                params.flags |= 0x20000;
            }
            params.trail_rate_a = type->trail_rates[0];
            params.trail_rate_b = type->trail_rates[1];
            params.effect_id_70 = type->impact_effect;
            if (type->impact_part == -1 && type->impact_part_name[0] != 0) {
                type->impact_part = PARTLookupType(type->impact_part_name);
            }
            params.effect_id_74 = type->impact_part;
            params.effect_id_78 = type->kill_effect;
            params.lighting_scale_x = type->lighting_scale.x;
            params.lighting_scale_y = type->lighting_scale.y;
            params.lighting_scale_z = type->lighting_scale.z;
            AddPart(&params);
            for (sound = 0; sound < 4; ++sound) {
                if (type->sounds[sound] != -1 && type->sound_modes[sound] == 3) {
                    PlaySfxById(type->sounds[sound], &part_position);
                }
            }
            type->last_used_time = emission_time;
            emission_time += interval;
        }
    }

    i16 PartPlatformImpactList[4] = {0, 0, 0, 0};
    f32 PartCollisionInterval = 0.1f;
    f32 PartStopSpeedSquared = 0.1f;

    void UpdateParts(f32 time) {
        i32 i;
        i32 axis;
        i32 angle;
        PART *part;
        NUVEC velocity;
        NUVEC direction;
        NUMTX mtx;
        partglobaltime += time;
        if (partglobaltime > 900.0f) {
            PartTimeSlip();
        }
        PartCleanupTypes();
        part_platimpactcnt = 0;
        UpdatePartEmits(time);
        part = Part;
        for (i = 0; i < MAXPARTS; ++i, ++part) {
            if ((part->active & PART_IN_USE) == 0) {
                continue;
            }
            PartRuntimeMatrixCopyPrivate(&part->previous_transform, &part->transform);
            part->scale_time += time;
            if ((part->active & PART_STATE_STOPPED) == 0) {
                f32 movement_time = time;
                if ((part->flags & PART_FLAG_IGNORE_TERRAIN) == 0) {
                    if (time >= part->time_of_impact) {
                        if (part->glow_radius < part->radius) {
                            part->active = (part->active & ~PART_STATE_SCALING_DOWN) | PART_STATE_SCALING_UP;
                        } else {
                            part->active &= ~(PART_STATE_SCALING_UP | PART_STATE_SCALING_DOWN);
                        }
                        if ((part->active & PART_STATE_IMPACT_PENDING) != 0) {
                            if ((part->flags & 0x80000) != 0 && (part->active & 0x10) != 0 && part_platimpactcnt < 4) {
                                PartPlatformImpactList[part_platimpactcnt++] = part->platform_id;
                            }
                            if ((part->flags & PART_FLAG_NO_BOUNCE) != 0) {
                                part->active |= PART_STATE_STOPPED;
                                if (part->stop_fn != NULL) {
                                    part->stop_fn(part);
                                }
                                if ((part->flags & PART_FLAG_DIE_WHEN_STOPPED) != 0 || (part->active & 0x10) != 0) {
                                    KillPart(part, 0);
                                }
                            } else {
                                movement_time -= part->time_of_impact;
                                part->position.x = part->impact_position.x;
                                part->position.y = part->impact_position.y;
                                part->position.z = part->impact_position.z;
                                FullReflect(&part->impact_normal, &part->velocity, &part->velocity);
                                if ((part->flags & PART_FLAG_ROTATE_RANDOMLY) != 0) {
                                    NewPartRotation(part);
                                }
                                if ((part->flags & PART_FLAG_ORDERED_ROTATE_RANDOMLY) != 0) {
                                    NewPartOrderedRotation(part);
                                }
                                part->velocity.x *= part->bounce;
                                part->velocity.z *= part->bounce;
                                if (part->impact_normal.y > 0.5f) {
                                    part->velocity.y *= part->bounce;
                                    if (part->velocity.x * part->velocity.x + part->velocity.y * part->velocity.y +
                                            part->velocity.z * part->velocity.z <
                                        PartStopSpeedSquared) {
                                        part->active |= PART_STATE_STOPPED;
                                        if (part->stop_fn != NULL) {
                                            part->stop_fn(part);
                                        }
                                        if ((part->flags & PART_FLAG_DIE_WHEN_STOPPED) != 0 || (part->active & 0x10) != 0 ||
                                            (part->active & PART_STATE_BELOW_CUTOFF) != 0) {
                                            KillPart(part, 0);
                                        }
                                    } else if ((part->active & PART_STATE_BELOW_CUTOFF) != 0) {
                                        KillPart(part, 0);
                                    }
                                }
                            }
                            if (part->effect_id_1f0 != -1) {
                                i32 key = -1;
                                PartRuntimeMatrixSetIdentity(&mtx);
                                NuMtxAlignY(&mtx, &part->impact_normal);
                                AddScaledFiniteShotDebrisEffect(&key, part->effect_id_1f0, &part->impact_position, NULL, NULL, 1,
                                                                part->effect_scale);
                                DebrisOrientationMtx(key, &mtx);
                            }
                            if (part->effect_id_1f8 != -1) {
                                AddFiniteShotPART(part->effect_id_1f8, &part->impact_position, 1);
                            }
                            if (part->impact_fn != NULL) {
                                part->impact_fn(part);
                            }
                        }
                        if (PartRaycastsEnabledPrivate != 0) {
                            CastPart(part, PartCollisionInterval);
                        }
                        if ((part->active & PART_STATE_IMPACT_PENDING) != 0 && part->time_of_impact <= movement_time) {
                            movement_time = 0.0f;
                        }
                    }
                    part->time_of_impact -= movement_time;
                }
                if (movement_time > 0.0f) {
                    if (part->move_fn != NULL) {
                        part->move_fn(part, movement_time);
                    } else {
                        part->position.x += part->velocity.x * movement_time;
                        part->position.y += part->velocity.y * movement_time;
                        part->velocity.y += part->gravity * movement_time;
                        part->position.z += part->velocity.z * movement_time;
                    }
                }
                if ((part->active & PART_STATE_BELOW_CUTOFF) == 0 && part->cutoff_height != 2000000.0f &&
                    part->position.y < part->cutoff_height) {
                    part->active |= PART_STATE_BELOW_CUTOFF;
                }
                part->collision_min.x = part->position.x - part->radius;
                part->collision_max.x = part->position.x + part->radius;
                part->collision_min.y = part->position.y - part->radius;
                part->collision_max.y = part->position.y + part->radius;
                part->collision_min.z = part->position.z - part->radius;
                part->collision_max.z = part->position.z + part->radius;
                if (((part->active >> 5) & 3) == 1) {
                    part->time_step += time;
                    part->glow_radius = part->time_step / PARTSCALEUPTIME * part->radius;
                    if (part->glow_radius > part->radius) {
                        part->glow_radius = part->radius;
                        part->active &= ~(PART_STATE_SCALING_UP | PART_STATE_SCALING_DOWN);
                    }
                } else if (((part->active >> 5) & 3) == 2) {
                    part->time_step -= time;
                    if (part->time_step <= 0.0f) {
                        KillPart(part, 0);
                    } else {
                        part->glow_radius = part->time_step / PARTSCALEUPTIME * part->radius;
                    }
                }
                if ((part->flags & PART_FLAG_ROTATE_RANDOMLY) != 0) {
                    for (axis = 0; axis < 2; ++axis) {
                        angle = (i32)((f32)part->spin_rates[3 + axis] * time);
                        switch (part->rotation_axes[axis]) {
                            case 1:
                                NuMtxPreRotateX(&part->transform, angle);
                                break;
                            case 2:
                                NuMtxPreRotateY(&part->transform, angle);
                                break;
                            case 3:
                                NuMtxPreRotateZ(&part->transform, angle);
                                break;
                        }
                    }
                }
                if ((part->flags & PART_FLAG_ORDERED_ROTATE_RANDOMLY) != 0) {
                    if (part->spin_rates[0] != 0) {
                        NuMtxPreRotateX(&part->transform, (i32)((f64)part->spin_rates[0] * time));
                    }
                    if (part->spin_rates[1] != 0) {
                        NuMtxPreRotateY(&part->transform, (i32)((f64)part->spin_rates[1] * time));
                    }
                    if (part->spin_rates[2] != 0) {
                        NuMtxPreRotateZ(&part->transform, (i32)((f64)part->spin_rates[2] * time));
                    }
                }
                if ((part->flags & PART_FLAG_FACE_MOVEMENT) != 0) {
                    direction.x = -part->velocity.x;
                    direction.y = -part->velocity.y;
                    direction.z = -part->velocity.z;
                    PartRuntimeMatrixSetIdentity(&mtx);
                    NuMtxLookAtZ(&mtx, &direction);
                    PCByteCopy(&part->transform, &mtx, 0x30);
                }
            }
            if ((part->active & PART_IN_USE) != 0) {
                if ((part->flags & PART_FLAG_COLLIDE_WITH_CHARACTERS) != 0 && part->collision_fn != NULL) {
                    part->collision_fn(part);
                }
                if ((part->active & PART_IN_USE) != 0) {
                    if (part->update_fn != NULL) {
                        part->update_fn(part);
                    }
                    if (part->time_remaining > 0.0f) {
                        part->time_remaining -= time;
                        if (part->time_remaining <= 0.0f) {
                            KillPart(part, 0);
                        }
                    }
                }
            }
            if ((part->flags & 0x20000) != 0 && part->debris_handle != NULL) {
                velocity = part->velocity;
                PartRuntimeMatrixSetIdentity(&mtx);
                velocity.x = -velocity.x;
                velocity.y = -velocity.y;
                velocity.z = -velocity.z;
                NuMtxLookAtY(&mtx, &velocity);
                DebrisOrientationMtx(*part->debris_handle, &mtx);
                DebrisEmitterPos(*part->debris_handle, part->position.x, part->position.y, part->position.z);
                if (((part->flags & PART_FLAG_DEBRIS_STOPS_WITH_PART) != 0 && (part->active & PART_STATE_STOPPED) != 0) ||
                    (part->flags & PART_FLAG_DISABLE_DRAW) != 0) {
                    DebrisStatusAlwaysOff(part->debris_handle);
                }
            }
            if (time != 0.0f) {
                if ((part->flags & 0x40000) != 0 && part->effect_id_1e0 != -1 &&
                    !((part->flags & PART_FLAG_DEBRIS_STOPS_WITH_PART) != 0 && (part->active & PART_STATE_STOPPED) != 0) &&
                    (part->flags & PART_FLAG_DISABLE_DRAW) == 0) {
                    AddScaledVariableShotDebrisEffect1(part->effect_id_1e0, &part->position, (i32)(part->trail_rate_a), time, 0, 0, NULL,
                                                       part->effect_scale);
                }
                if ((part->flags & 0x40000) != 0 && part->effect_id_1e4 != -1 &&
                    !((part->flags & PART_FLAG_DEBRIS_STOPS_WITH_PART) != 0 && (part->active & PART_STATE_STOPPED) != 0) &&
                    (part->flags & PART_FLAG_DISABLE_DRAW) == 0) {
                    AddScaledVariableShotDebrisEffect1(part->effect_id_1e4, &part->position, (i32)(part->trail_rate_b), time, 0, 0, NULL,
                                                       part->effect_scale);
                }
            }
            if (part->lighting_handle != NULL) {
                if ((part->flags & PART_FLAG_DISABLE_DRAW) == 0) {
                    rtlDynamicSetPos(*part->lighting_handle, &part->position);
                } else {
                    NUVEC offscreen;
                    offscreen.x = 10000.0f;
                    offscreen.y = 10000.0f;
                    offscreen.z = 10000.0f;
                    rtlDynamicSetPos(*part->lighting_handle, &offscreen);
                }
            }
            if (part->callback_1ac != NULL) {
                part->callback_1ac(part);
            }
            part->first_frame = 0;
            if ((part->active & PART_IN_USE) == 0) {
                KillPart(part, 0);
                memset(part, 0, sizeof(PART));
            }
        }
    }

    void DrawParts(i32 keep_offscreen) {
        PART *part = Part;
        i32 i;
        NUMTX scaled;
        NUMTX *matrix;
        NUVEC scale;
        for (i = 0; i < MAXPARTS; ++i, ++part) {
            if ((part->active & PART_IN_USE) == 0 || (part->flags & PART_FLAG_DISABLE_DRAW) != 0) {
                continue;
            }
            if (part->draw_fn != NULL && part->draw_fn(part) == 0) {
                continue;
            }
            if ((part->active & PART_STATE_LIT) != 0) {
                PartRuntimeDirectionalLightsPrivate(&part->lighting[3], &part->lighting[0], &part->lighting[4], &part->lighting[1],
                                                    &part->lighting[5], &part->lighting[2]);
                PartsSetAmbientLight(&part->lighting[6]);
            } else if ((part->active & PART_STATE_LIGHT_SCALE) != 0) {
                PartsSetAmbientLight(&part->lighting[6]);
            }
            part->active &= ~PART_STATE_DRAWN;
            matrix = &part->transform;
            if (part->particle_scale != 1.0f) {
                PartRuntimeMatrixCopyPrivate(&scaled, matrix);
                matrix = &scaled;
                scale.z = part->particle_scale;
                scale.x = scale.y = scale.z;
                NuMtxPreScale(matrix, &scale);
            }
            if (part->source_special != NULL && NuSpecialDrawAt(&part->special, matrix) != 0) {
                part->active |= 0x600;
            } else if (keep_offscreen == 0 &&
                       ((part->flags & PART_FLAG_KEEP_OFF_SCREEN) == 0 || (part->active & PART_STATE_BELOW_CUTOFF) != 0)) {
                KillPart(part, 0);
            }
        }
    }

    void AddScaledVariableShotPARTEffect(i32 effect, NUVEC *position, f32 rate, f32 time, NUMTX *orientation, f32 scale) {
        i32 scaled = CreateScaledPARTEffect(effect, scale);
        if (scaled != -1) {
            AddVariableShotPARTEffect(scaled, position, rate, time, orientation);
        }
    }

    void edpartDestroyAllPages(void) {
        i32 i;
        for (i = 0; i < 8; ++i) {
            if (part_page_used[i] != 0) {
                edpartClearPage(i);
            }
        }
    }

    f32 edpart_offset = 0;
    i32 edpart_snap_enabled = 0;
    i32 edpart_cam_ax = 0;
    i32 edpart_cam_ay = 0;

    void edpartDoInput(FRAME_PAD *pad) {
        i32 nearest;
        i32 variant;
        i32 previous;
        if ((pad->digital_buttons & PAD_BUTTON_SELECT) == 0) {
            edcamMove(pad);
        }
        if (pad->digital_buttons & PAD_BUTTON_SELECT) {
            nearest = edpart_nearest;
            if (nearest != -1) {
                if (pad->digital_buttons_pressed & PAD_BUTTON_R1) {
                    do {
                        ++nearest;
                        if (nearest == 40) {
                            nearest = 0;
                        }
                    } while (part_emits[nearest].effect_id == -1);
                    edpart_nearest = nearest;
                }
                if (pad->digital_buttons_pressed & PAD_BUTTON_R2) {
                    do {
                        --nearest;
                        if (nearest == -1) {
                            nearest = 39;
                        }
                    } while (part_emits[nearest].effect_id == -1);
                    edpart_nearest = nearest;
                }
            } else {
                edpartDetermineNearest(-1.0f);
                nearest = edpart_nearest;
            }
            if (nearest != -1) {
                edcamSetPos(&part_emits[nearest].position);
                edpart_offset = *(f32 *)&part_emits[edpart_nearest].trailing_state_words[1];
                edpart_rotz = ((i16 *)&part_emits[edpart_nearest].trailing_state_words[0])[0];
                edpart_roty = ((i16 *)&part_emits[edpart_nearest].trailing_state_words[0])[1];
                edpart_emitrotz = part_emits[edpart_nearest].rotation_2c;
                edpart_emitroty = part_emits[edpart_nearest].rotation_2e;
                edpart_emitrotx = part_emits[edpart_nearest].rotation_30;
                edpart_create_type = part_emits[edpart_nearest].effect_id;
                edpart_effect_list = part_types[edpart_create_type].effect_list;
            }
        }
        if (edpart_snap_enabled) {
            edcamGetPosAngSnap(&PartEditorCameraPos, &edpart_cam_ax, &edpart_cam_ay);
        } else {
            edcamGetPosAng(&PartEditorCameraPos, &edpart_cam_ax, &edpart_cam_ay);
        }
        if ((pad->digital_buttons & PAD_BUTTON_SELECT) == 0) {
            if (pad->digital_buttons_pressed & PAD_BUTTON_SQUARE) {
                edpartOptMenu();
                edpart_active_menu = edpart_opt_menu;
            }
            if ((pad->digital_buttons_pressed & PAD_BUTTON_CROSS) && edpart_copy_mode == 0 && edpart_create_type != -1) {
                AddPARTEffect(edpart_create_type, &PartEditorCameraPos);
            }
            if (pad->digital_buttons & PAD_BUTTON_CIRCLE) {
                if (edpart_copy_mode != 0) {
                    edpartMultipleCopyCopy();
                } else if (edpart_nearest != -1) {
                    edpartPlace(edpart_nearest, &PartEditorCameraPos);
                }
            }
            if ((pad->digital_buttons & PAD_BUTTON_R3) && edpart_copy_mode == 0 && edpart_nearest != -1) {
                edpartPlace(edpart_nearest, &PartEditorCameraPos);
            }
            if (pad->digital_buttons_pressed & PAD_BUTTON_TRIANGLE) {
                if (edpart_copy_mode != 0) {
                    edpart_copy_source_count = 0;
                    edpart_copyroty = 0;
                } else {
                    if (edpart_nearest != -1) {
                        edpartDestroy(edpart_nearest);
                    }
                    edpart_nearest = -1;
                }
            }
        }
        if (edpart_copy_mode != 0) {
            edpart_copy_size += pad->analog_left_pad_up / 5000.0f;
            edpart_copy_size -= pad->analog_left_pad_down / 5000.0f;
            if (edpart_copy_size < 0.05f) {
                edpart_copy_size = 0.05f;
            } else if (edpart_copy_size > 2.0f) {
                edpart_copy_size = 2.0f;
            }
            edpart_copyroty += pad->analog_left_pad_right;
            edpart_copyroty -= pad->analog_left_pad_left;
        } else {
            if (edpart_dpad_mode == 1) {
                if (pad->digital_buttons & PAD_BUTTON_L3) {
                    edpart_rotz = 0;
                    edpart_roty = 0;
                }
                edpart_roty += pad->analog_left_pad_right;
                edpart_roty -= pad->analog_left_pad_left;
                edpart_rotz += pad->analog_left_pad_up;
                if (edpart_rotz > 0) {
                    edpart_rotz = 0;
                }
                edpart_rotz -= pad->analog_left_pad_down;
                if (edpart_rotz < -0x8000) {
                    edpart_rotz = -0x8000;
                }
            } else if (edpart_dpad_mode == 0) {
                if (pad->digital_buttons & PAD_BUTTON_L3) {
                    edpart_emitrotz = 0;
                    edpart_emitroty = 0;
                    edpart_emitrotx = 0;
                }
                if (pad->digital_buttons & PAD_BUTTON_R3) {
                    edpart_emitrotx += pad->analog_left_pad_right;
                    edpart_emitrotx -= pad->analog_left_pad_left;
                } else {
                    edpart_emitroty += pad->analog_left_pad_right;
                    edpart_emitroty -= pad->analog_left_pad_left;
                    edpart_emitrotz += pad->analog_left_pad_up;
                    if (edpart_emitrotz > 0x8000) {
                        edpart_emitrotz = 0x8000;
                    }
                    edpart_emitrotz -= pad->analog_left_pad_down;
                    if (edpart_emitrotz < -0x8000) {
                        edpart_emitrotz = -0x8000;
                    }
                }
            } else if (edpart_dpad_mode == 3) {
                edpart_refroty += pad->analog_left_pad_right;
                edpart_refroty -= pad->analog_left_pad_left;
                edpart_refrotz += pad->analog_left_pad_up;
                if (edpart_refrotz > 0) {
                    edpart_refrotz = 0;
                }
                edpart_refrotz -= pad->analog_left_pad_down;
                if (edpart_refrotz < -0x8000) {
                    edpart_refrotz = -0x8000;
                }
            } else if (edpart_dpad_mode == 2) {
                if (pad->analog_left_pad_up == 255 || (pad->digital_buttons_pressed & PAD_BUTTON_UP)) {
                    edpart_offset += 0.01f;
                }
                if (pad->analog_left_pad_down == 255 || (pad->digital_buttons_pressed & PAD_BUTTON_DOWN)) {
                    edpart_offset -= 0.01f;
                }
                if (edpart_offset < 0.0f) {
                    edpart_offset = 0.0f;
                }
            }
        }
        if (edpart_nearest == -1) {
            return;
        }
        edpart_nearest_emit = &part_emits[edpart_nearest];
        if (edpart_nearest_emit->effect_id != -1) {
            edpart_nearest_type = &part_types[edpart_nearest_emit->effect_id];
        }
        edpart_nearest_duplicates = 0;
        edpart_nearest_orphans = 0;
        for (variant = 0; variant < 8; ++variant) {
            i16 effect = edpart_nearest_type->effect_ids[variant];
            if (effect == -1 || effect == 9999) {
                continue;
            }
            if (effect == 9998) {
                ++edpart_nearest_orphans;
                continue;
            }
            for (previous = 0; previous < variant; ++previous) {
                if (edpart_nearest_type->effect_ids[previous] == effect) {
                    ++edpart_nearest_duplicates;
                    break;
                }
            }
        }
    }

    i32 edpart_count = 0;

    i32 edpartProc(float delta_time, FRAME_PAD *pad) {
        edpart_count += 5;
        if (edpart_active_menu) {
            eduiMenuProcess(edpart_active_menu, delta_time, pad);
            return 0;
        }
        edpartDoInput(pad);
        edpartDetermineNearest(1.0f);
        return (pad->digital_buttons_pressed >> 11) & 1;
    }
}
