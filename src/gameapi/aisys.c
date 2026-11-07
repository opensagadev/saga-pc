#define _CRT_SECURE_NO_DEPRECATE // NOLINT(readability-identifier-naming): VC8 CRT selector.
#include "saga.h"
#include <stdio.h>
#include <float.h>
#include <math.h>

typedef union AI_ACTION_FLAGS_EF4_PC {
    u32 word;
    struct {
        u32 unused0 : 25;
        u32 no_idle_speed : 1;
        u32 unused26 : 6;
    } bits;
} AI_ACTION_FLAGS_EF4_PC;
STATIC_ASSERT(sizeof(AI_ACTION_FLAGS_EF4_PC) == 4);

/* PC AIEDITOR_RENDER_STATE layout (0x980 larger than Android's, after the path storage). */
STATIC_ASSERT(offsetof(AIEDITOR_RENDER_STATE, cursor_check) == 0x48);

extern void __cdecl AiRndrLine3d(NURND_VERTEX3D_PC *, NUMTL *, NUMTX *);

static __forceinline f32 NuFdiv(f32 a, f32 b) {
    if (a == 0.0f || b == 0.0f) {
        return 0.0f;
    }
    return a / b;
}

i32 Action_CameraCut(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                     i32 param_count, i32 first_time, f32 elapsed) {
    NUVEC target_position;
    NUVEC camera_position;
    NUVEC camera_delta;
    f32 range;
    i32 rotation_x;
    i32 rotation_y;
    char *target_locator;
    char *target_character;
    char *target_object;
    char *camera_locator;
    i32 follow_target;
    i32 borders;
    i32 use_current_camera;
    f32 start;
    f32 end;
    f32 blend_in;
    f32 blend_out;
    char *value;
    NUVEC *target;
    AILOCATOR_PC *locator;
    GAMEOBJECT *object;
    NUSPECIALHANDLE special;
    NUMTX *matrix;
    i32 index;

    target_position.x = 1000000000.0f;
    target_position.y = 1000000000.0f;
    target_position.z = 1000000000.0f;
    camera_position.x = 1000000000.0f;
    camera_position.y = 1000000000.0f;
    camera_position.z = 1000000000.0f;
    camera_delta.x = 0.0f;
    camera_delta.y = 0.0f;
    camera_delta.z = 0.0f;
    range = 1.0f;
    rotation_x = 0;
    rotation_y = 0;
    target_locator = NULL;
    target_character = NULL;
    target_object = NULL;
    camera_locator = NULL;
    follow_target = 0;
    borders = 1;
    use_current_camera = 0;
    start = 0.0f;
    end = 1000000000.0f;
    blend_in = 0.0f;
    blend_out = 0.0f;
    if (first_time == 0)
        return 1;
    for (index = 0; index < param_count; ++index) {
        if (NuStrICmp((char *)AITransferLiteral757a68, params[index]) == 0) {
            GameCameraMakeMiniCut2(NULL, NULL, 0, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0);
            return 1;
        } else if (NuStrICmp((char *)AICameraLiteral758090, params[index]) == 0) {
            use_current_camera = 1;
        } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral758084)) != NULL) {
            start = AIParamToFloat(processor, value + 11);
        } else if (NuStrICmp((char *)AICameraLiteral758070, params[index]) == 0) {
            end = 1000000000.0f;
        } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral758064)) != NULL) {
            end = AIParamToFloat(processor, value + 9);
        } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral758054)) != NULL) {
            blend_in = AIParamToFloat(processor, value + 14);
        } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral758044)) != NULL) {
            blend_out = AIParamToFloat(processor, value + 15);
        } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral758034)) != NULL) {
            target_locator = value + 12;
        } else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral757874)) != NULL) {
            target_character = value + 14;
        } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral758028)) != NULL) {
            target_object = value + 8;
        } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral758018)) != NULL) {
            camera_locator = value + 12;
        } else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral757724)) != NULL) {
            range = AIParamToFloat(processor, value + 6);
        } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral758010)) != NULL) {
            rotation_y = (i32)AIParamToFloat(processor, value + 5) * (65536 / 360.0f);
        } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral758008)) != NULL) {
            rotation_x = -(i32)AIParamToFloat(processor, value + 5) * (65536 / 360.0f);
        } else if (NuStrICmp(params[index], (char *)AICameraLiteral757ffc) == 0) {
            follow_target = 1;
        } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral757ff0)) != NULL) {
            camera_delta.x = AIParamToFloat(processor, value + 10);
        } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral757fe4)) != NULL) {
            camera_delta.y = AIParamToFloat(processor, value + 10);
        } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral757fd8)) != NULL) {
            camera_delta.z = AIParamToFloat(processor, value + 10);
        } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral757fcc)) != NULL) {
            camera_position.x = AIParamToFloat(processor, value + 9);
        } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral757fc0)) != NULL) {
            camera_position.y = AIParamToFloat(processor, value + 9);
        } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral757fb4)) != NULL) {
            camera_position.z = AIParamToFloat(processor, value + 9);
        } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral757fa8)) != NULL) {
            target_position.x = AIParamToFloat(processor, value + 9);
        } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral757f9c)) != NULL) {
            target_position.y = AIParamToFloat(processor, value + 9);
        } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral757f90)) != NULL) {
            target_position.z = AIParamToFloat(processor, value + 9);
        } else if (NuStrICmp((char *)AICameraLiteral757f84, params[index]) == 0) {
            borders = 0;
        }
    }

    target = NULL;
    if (target_locator != NULL) {
        locator = AIPathFindLocator(system, target_locator);
        if (locator == NULL)
            return 1;
        target = &locator->position;
    } else if (target_character != NULL) {
        if (packet != NULL && packet->owner != NULL &&
            NuStrICmp(target_character, (char *)AITransferLiteral757f7c) == 0) {
            object = packet->owner->objptr;
        } else {
            APIOBJECT_PC *named;
            if (GetNamedGameObjectCallback == NULL)
                return 1;
            named = GetNamedGameObjectCallback(system, target_character);
            if (named == NULL)
                return 1;
            object = (GAMEOBJECT *)named->owner;
        }
        if (object == NULL)
            return 1;
        target = &object->collision_position;
    } else if (target_object != NULL) {
        if (NuSpecialFind(WORLD->current_gscn, &special, target_object, 1) == 0)
            return 1;
        target = NuSpecialGetDrawPos(&special);
    } else {
        if (target_position.x == 1000000000.0f || target_position.y == 1000000000.0f ||
            target_position.z == 1000000000.0f)
            return 1;
        target = &target_position;
    }
    if (target == NULL)
        return 1;

    if (use_current_camera != 0) {
        matrix = NuCameraGetMtx();
        if (matrix != NULL) {
            camera_position = matrix->translation;
            NuVecAdd(&camera_position, &camera_position, &camera_delta);
            goto camera_ready;
        }
    } else {
        if (camera_position.x != 1000000000.0f && camera_position.y != 1000000000.0f &&
            camera_position.z != 1000000000.0f)
            goto camera_ready;
        if (camera_locator != NULL) {
            locator = AIPathFindLocator(system, camera_locator);
            if (locator != NULL) {
                camera_position = locator->position;
                goto camera_ready;
            }
        }
    }
    camera_position.x = 0.0f;
    camera_position.y = 0.0f;
    camera_position.z = range;
    NuVecRotateX(&camera_position, &camera_position, rotation_x);
    NuVecRotateY(&camera_position, &camera_position, rotation_y);
    NuVecAdd(&camera_position, &camera_position, target);
camera_ready:
    GameCameraMakeMiniCut2(&camera_position, target, 0, start, end, blend_in, blend_out, follow_target, 0, borders);
    return 1;
}

i32 Action_DynamicCameraCut(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                            i32 param_count, i32 first_time, f32 elapsed) {
    NUVEC target_position;
    NUVEC camera_position;
    NUVEC camera_offset;
    u32 flags;
    f32 range;
    i32 rotation_x;
    i32 rotation_y;
    i32 rotation_z;
    f32 start_time;
    f32 blend_in_time;
    f32 end_time;
    f32 blend_out_time;
    f32 blend_time;
    f32 hold_time;
    f32 max_time;
    i32 borders;
    i32 use_current_position;
    i8 mode;
    i8 interpolation;
    NUGSPLINE *spline;
    char *target_locator;
    char *target_character;
    char *target_object;
    char *camera_locator;
    char *value;
    NUVEC *target;
    AILOCATOR_PC *locator;
    GAMEOBJECT *object;
    NUSPECIALHANDLE special;
    i32 have_camera_position;
    NUMTX *matrix;
    i32 index;

    target_position.x = 1000000000.0f;
    target_position.y = 1000000000.0f;
    target_position.z = 1000000000.0f;
    camera_position.x = 1000000000.0f;
    camera_position.y = 1000000000.0f;
    camera_position.z = 1000000000.0f;
    camera_offset.x = 0.0f;
    camera_offset.y = 0.0f;
    camera_offset.z = 0.0f;
    flags = 0;
    range = 1.0f;
    rotation_x = 0;
    rotation_y = 0;
    rotation_z = 0;
    start_time = 0.0f;
    blend_in_time = 0.0f;
    end_time = 1000000000.0f;
    blend_out_time = 0.0f;
    blend_time = 0.0f;
    hold_time = 0.0f;
    max_time = 0.0f;
    borders = 1;
    use_current_position = 0;
    mode = -1;
    interpolation = -1;
    spline = NULL;
    target_locator = NULL;
    target_character = NULL;
    target_object = NULL;
    camera_locator = NULL;
    if (first_time != 0) {
        for (index = 0; index < param_count; ++index) {
            if (NuStrICmp((char *)AICameraLiteral758090, params[index]) == 0) {
                flags |= 0x100;
                use_current_position = 1;
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral758160)) != NULL) {
                flags |= 0x2000;
                spline = NuSplineFind(WORLD->current_gscn, value + 17);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral75814c)) != NULL) {
                flags |= 0x4000;
                range = AIParamToFloat(processor, value + 17);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral758084)) != NULL) {
                flags |= 0x200;
                start_time = AIParamToFloat(processor, value + 11);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral758140)) != NULL) {
                flags |= 0x800;
                blend_time = AIParamToFloat(processor, value + 11);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral758054)) != NULL) {
                blend_in_time = AIParamToFloat(processor, value + 14);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral758044)) != NULL) {
                blend_out_time = AIParamToFloat(processor, value + 15);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral758064)) != NULL) {
                flags |= 0x400;
                end_time = AIParamToFloat(processor, value + 9);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral758134)) != NULL) {
                max_time = AIParamToFloat(processor, value + 9);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral758128)) != NULL) {
                flags |= 0x1000;
                hold_time = AIParamToFloat(processor, value + 10);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral758034)) != NULL) {
                target_locator = value + 12;
            } else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral757874)) != NULL) {
                target_character = value + 14;
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral758028)) != NULL) {
                target_object = value + 8;
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral758018)) != NULL) {
                camera_locator = value + 12;
            } else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral757724)) != NULL) {
                flags |= 1;
                range = AIParamToFloat(processor, value + 6);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral75811c)) != NULL) {
                flags |= 0x20;
                rotation_y = (i32)((i32)AIParamToFloat(processor, value + 10) * 182.0444488525390625f);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral758010)) != NULL) {
                flags |= 0x4;
                rotation_y = (i32)((i32)AIParamToFloat(processor, value + 5) * 182.0444488525390625f);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral758110)) != NULL) {
                flags |= 0x10;
                rotation_x = (i32)((i32)AIParamToFloat(processor, value + 10) * 182.0444488525390625f);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral758008)) != NULL) {
                flags |= 0x2;
                rotation_x = (i32)(-(i32)AIParamToFloat(processor, value + 5) * 182.0444488525390625f);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral758104)) != NULL) {
                flags |= 0x40;
                rotation_z = (i32)((i32)AIParamToFloat(processor, value + 10) * 182.0444488525390625f);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral7580fc)) != NULL) {
                flags |= 0x8;
                rotation_z = (i32)(-(i32)AIParamToFloat(processor, value + 5) * 182.0444488525390625f);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral757fcc)) != NULL) {
                camera_position.x = AIParamToFloat(processor, value + 9);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral757fc0)) != NULL) {
                camera_position.y = AIParamToFloat(processor, value + 9);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral757fb4)) != NULL) {
                camera_position.z = AIParamToFloat(processor, value + 9);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral757fa8)) != NULL) {
                target_position.x = AIParamToFloat(processor, value + 9);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral757f9c)) != NULL) {
                target_position.y = AIParamToFloat(processor, value + 9);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral757f90)) != NULL) {
                target_position.z = AIParamToFloat(processor, value + 9);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral757ff0)) != NULL) {
                camera_offset.x = AIParamToFloat(processor, value + 10);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral757fe4)) != NULL) {
                camera_offset.y = AIParamToFloat(processor, value + 10);
            } else if ((value = NuStrIStr(params[index], (char *)AICameraLiteral757fd8)) != NULL) {
                camera_offset.z = AIParamToFloat(processor, value + 10);
            } else if (NuStrICmp((char *)AICameraLiteral757f84, params[index]) == 0) {
                borders = 0;
            } else if (NuStrICmp((char *)AICameraLiteral7580f0, params[index]) == 0) {
                mode = 0;
            } else if (NuStrICmp((char *)AICameraLiteral7580e4, params[index]) == 0) {
                mode = 1;
            } else if (NuStrICmp((char *)AICameraLiteral7580dc, params[index]) == 0) {
                interpolation = 0;
            } else if (NuStrICmp((char *)AICameraLiteral7580cc, params[index]) == 0) {
                interpolation = 2;
            } else if (NuStrICmp((char *)AICameraLiteral7580c0, params[index]) == 0) {
                interpolation = 1;
            } else if (NuStrICmp((char *)AICameraLiteral7580b0, params[index]) == 0) {
                interpolation = 4;
            } else if (NuStrICmp((char *)AICameraLiteral7580a4, params[index]) == 0) {
                interpolation = 3;
            }
        }
        target = NULL;
        if (target_locator != NULL) {
            locator = AIPathFindLocator(system, target_locator);
            if (locator != NULL)
                target = &locator->position;
        } else if (target_character != NULL) {
            if (packet != NULL && packet->owner != NULL &&
                NuStrICmp(target_character, (char *)AITransferLiteral757f7c) == 0)
                object = packet->owner->objptr;
            else {
                APIOBJECT_PC *named =
                    GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(system, target_character) : NULL;
                object = named != NULL ? (GAMEOBJECT *)named->owner : NULL;
            }
            if (object != NULL)
                target = &object->collision_position;
        } else if (target_object != NULL) {
            if (NuSpecialFind(WORLD->current_gscn, &special, target_object, 1) != 0)
                target = NuSpecialGetDrawPos(&special);
        } else if (target_position.x != 1000000000.0f && target_position.y != 1000000000.0f &&
                   target_position.z != 1000000000.0f) {
            target = &target_position;
        }
        if (target != NULL)
            flags |= 0x80;

        have_camera_position = 0;
        if (use_current_position != 0) {
            matrix = NuCameraGetMtx();
            if (matrix != NULL) {
                camera_position = matrix->translation;
                have_camera_position = 1;
            }
        } else if (camera_position.x != 1000000000.0f && camera_position.y != 1000000000.0f &&
                   camera_position.z != 1000000000.0f) {
            have_camera_position = 1;
        } else if (camera_locator != NULL) {
            locator = AIPathFindLocator(system, camera_locator);
            if (locator != NULL) {
                camera_position = locator->position;
                have_camera_position = 1;
            }
        }
        if (spline != NULL) {
            PointAlongSpline(spline, 0.0f, &camera_position, NULL, NULL, 0);
            if (mode == -1)
                mode = 2;
        } else if (have_camera_position != 0) {
            flags |= 0x100;
            NuVecAdd(&camera_position, &camera_position, &camera_offset);
            if (mode == -1)
                mode = 1;
        } else {
            if (target != NULL) {
                camera_position.x = 0.0f;
                camera_position.y = 0.0f;
                camera_position.z = range;
                NuVecRotateX(&camera_position, &camera_position, rotation_x);
                NuVecRotateY(&camera_position, &camera_position, rotation_y);
                NuVecAdd(&camera_position, &camera_position, target);
            }
            if (mode == -1)
                mode = 0;
        }
        GameCameraMakeMiniCut3(flags, range, rotation_x, rotation_y, rotation_z, target, 0, &camera_position,
                               start_time, blend_in_time, end_time, blend_out_time, blend_time, hold_time, max_time,
                               borders, spline, mode, interpolation);
    }
    return 1;
}

i32 Action_EndCameraCut(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                        i32 param_count, i32 first_time, f32 elapsed) {
    char *value;
    i32 i;

    if (first_time != 0 && MiniCutCam != 0) {
        for (i = 0; i < param_count; ++i) {
            value = NuStrIStr(params[i], (char *)AICameraLiteral758064);
            if (value != NULL) {
                ObstacleCamEnd = AIParamToFloat(processor, value + 9);
                ObstacleCamEnd += ObstacleCamTime;
            } else {
                value = NuStrIStr(params[i], (char *)AICameraLiteral758044);
                if (value != NULL) {
                    ObstacleCamBlendOutTime = AIParamToFloat(processor, value + 15);
                }
            }
        }
    }
    return 1;
}

void AIRetreatFromDestination2(AISYS_PC *, AIPACKET_PC *, APIOBJECT_PC *, i32);

i32 __cdecl Action_GizmoActivate(AISYS_PC *unused0, AISCRIPTPROCESS_PC *unused1, AIPACKET_PC *unused2, char **params,
                                 i32 count, i32 first_time, f32 unused6) {
    void *gizmo = NULL;
    i32 enabled = 1;
    i32 index;
    char *value;
    if (first_time != 0 && count > 0) {
        for (index = 0; index < count; ++index) {
            value = NuStrIStr(params[index], (char *)AIScriptActionNameArgPrefix);
            if (value != NULL)
                gizmo = GizmoFindByName(WORLD->gizmo_sys, -1, value + NuStrLen(AIScriptActionNameArgPrefix));
            else if (NuStrICmp(params[index], AIActionFalseName) == 0)
                enabled = 0;
        }
        if (gizmo != NULL)
            GizmoActivate(WORLD->gizmo_sys, gizmo, enabled, 1);
    }
    return 1;
}

i32 __cdecl Action_GizmoSetVisibility(AISYS_PC *unused0, AISCRIPTPROCESS_PC *unused1, AIPACKET_PC *unused2,
                                      char **params, i32 count, i32 first_time, f32 unused6) {
    void *gizmo = NULL;
    i32 enabled = 1;
    i32 index;
    char *value;
    if (first_time != 0 && count > 0) {
        for (index = 0; index < count; ++index) {
            value = NuStrIStr(params[index], (char *)AIScriptActionNameArgPrefix);
            if (value != NULL)
                gizmo = GizmoFindByName(WORLD->gizmo_sys, -1, value + NuStrLen(AIScriptActionNameArgPrefix));
            else if (NuStrICmp(params[index], AIActionFalseName) == 0)
                enabled = 0;
        }
        if (gizmo != NULL)
            GizmoSetVisibility(WORLD->gizmo_sys, gizmo, enabled, 1);
    }
    return 1;
}

static i32 Action_SetAnimation(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                               i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)param_6;
    if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL && param_5 != 0 && param_4 == 1) {
        GAMEOBJECT *object = packet->owner->objptr;
        const i32 animation = FindAnimIX(object->character_data, params[0]);
        if (animation != -1) {
            ResetAnimPacket(&object->anim_packet, animation);
        }
    }
    return 1;
}

i32 __cdecl Action_BigJump(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                           i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    NUVEC destination;
    GAMEOBJECT *object;
    GAMEOBJECT *target;
    i32 to_origin;
    i32 check_terrain;
    f32 jump_factor;
    i32 slam;
    char *value;
    AICREATURE_PC *creature;
    APIOBJECT_PC *lookup;

    (void)param_6;
    jump_factor = 1.0f;
    destination.x = 1.0e9f;
    destination.y = 1.0e9f;
    destination.z = 1.0e9f;
    target = NULL;
    to_origin = 0;
    check_terrain = 0;
    slam = 0;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL) {
        return 1;
    }

    object = packet->owner->objptr;
    if (param_5 != 0) {

        for (index = 0; index < param_4; ++index) {
            value = NuStrIStr(params[index], (char *)AITransferLiteral757874);
            if (value != NULL) {
                if (GetNamedGameObjectCallback != NULL &&
                    (lookup = GetNamedGameObjectCallback(sys, value + 14)) != NULL)
                    target = (GAMEOBJECT *)lookup->owner;
                else
                    target = NULL;
            } else if (NuStrICmp(params[index], (char *)AITransferLiteral75786c) == 0) {
                slam = 1;
            } else if (NuStrICmp(params[index], (char *)AITransferLiteral757860) == 0) {
                to_origin = 1;
            } else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral757834)) != NULL) {
                jump_factor = AIParamToFloat(processor, value + 12);
                if (jump_factor < 0.0f) {
                    jump_factor = 0.0f;
                }
            } else if ((value = NuStrIStr(params[index], (char *)XArgPrefix)) != NULL) {
                destination.x = AIParamToFloat(processor, value + 2);
            } else if ((value = NuStrIStr(params[index], (char *)YArgPrefix)) != NULL) {
                destination.y = AIParamToFloat(processor, value + 2);
            } else if ((value = NuStrIStr(params[index], (char *)ZArgPrefix)) != NULL) {
                destination.z = AIParamToFloat(processor, value + 2);
            } else if (NuStrICmp(params[index], (char *)AITransferLiteral757844) == 0) {
                check_terrain = 1;
            }
        }

        if (to_origin != 0 && (object->flags_1f8 & 0x400) != 0 && object->ai_packet.creature_index != 0xff) {
            creature = &sys->creatures[object->ai_packet.creature_index];
            StartBigJump(object, &creature->position, slam, jump_factor, 1.0f, 0);
            object->ai_packet.path_info = creature->path_info;
            object->ai_packet.last_path_position = creature->position;
        } else if (target != NULL) {
            StartBigJump(object, &target->last_safe_position, slam, jump_factor, 1.0f, 0);
            object->ai_packet.path_info = target->ai_packet.path_info;
            object->ai_packet.last_path_position = target->ai_packet.last_path_position;
        } else {
            if (destination.x != 1.0e9 || destination.y != 1.0e9 || destination.z != 1.0e9)
                StartBigJump(object, &destination, slam, jump_factor, 1.0f, check_terrain);
            else
                StartBigJump(object, &object->collision_position, slam, jump_factor, 1.0f, check_terrain);
        }
    }
    return object->character_context != 0x1f;
}

i32 __cdecl Action_SetCurrentSpeed(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                   i32 num_params, i32 first_time, f32 elapsed) {
    i32 i;
    GAMEOBJECT *object;
    i32 speed_mode;
    f32 speed;
    char *value;
    APIOBJECT_PC *named;

    speed = 0.0f;
    speed_mode = -1;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    if (!first_time)
        return 1;
    for (i = 0; i < num_params; i++) {
        value = NuStrIStr(params[i], (char *)AIGameCharacterName);
        if (value != NULL) {
            named = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(sys, value + 10) : NULL;
            object = named != NULL ? (GAMEOBJECT *)named->owner : NULL;
        } else if (NuStrICmp((char *)AITransferLiteral757bb4, params[i]) == 0)
            speed_mode = 2;
        else if (NuStrICmp((char *)AITransferLiteral757ba8, params[i]) == 0)
            speed_mode = 1;
        else if (NuStrICmp((char *)AITransferLiteral757b9c, params[i]) == 0)
            speed_mode = 0;
        else
            speed = AIParamToFloat(processor, params[i]);
    }
    if (object != NULL) {
        if (speed_mode == 2)
            speed = ((PLAYERCHARACTERCONFIG_PC *)object->character_data->game_character)->tiptoe_speed;
        else if (speed_mode == 1)
            speed = ((PLAYERCHARACTERCONFIG_PC *)object->character_data->game_character)->walk_speed;
        else if (speed_mode == 0) {
            speed = ((PLAYERCHARACTERCONFIG_PC *)object->character_data->game_character)->run_speed;
            object->field_dc8 = 1.0f;
        }
        object->velocity.x = 0.0f;
        object->velocity.y = 0.0f;
        object->velocity.z = speed;
        NuVecRotateY(&object->velocity, &object->velocity, object->facing_angle);
    }
    return 1;
}

i32 __cdecl Action_SetGravityHeight(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                    i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    GAMEOBJECT *object;
    f32 minimum;
    f32 maximum;
    char *value;
    f32 random;

    object = packet != NULL && packet->owner != NULL ? packet->owner->objptr : NULL;
    if (object == NULL || first_time == 0) {
        return 1;
    }
    object->portal_flags &= 0x7fffffffu;
    object->hover_height_override = 1.0e9f;

    minimum = 1.0e9f;
    maximum = 1.0e9f;
    for (index = 0; index < param_count; ++index) {
        if (NuStrICmp(params[index], (char *)AIScaleOverrideResetName) == 0) {
            continue;
        }
        value = NuStrIStr(params[index], (char *)AIControlMinimumName);
        if (value != NULL) {
            minimum = AIParamToFloat(processor, value + 4);
            continue;
        }
        value = NuStrIStr(params[index], (char *)AIControlMaximumName);
        if (value != NULL) {
            maximum = AIParamToFloat(processor, value + 4);
        } else {
            object->hover_height_override = AIParamToFloat(processor, params[index]);
        }
    }
    if (minimum != 1.0e9 && maximum != 1.0e9) {
        random = NuRandFloat();
        object->hover_height_override = maximum * random + (1.0f - random) * minimum;
    }
    return 1;
}

i32 __cdecl Action_SnapWeaponOut(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                 i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    i32 enabled;
    i32 keep_out;
    char *value;

    (void)processor;
    (void)param_6;
    if (param_5 == 0) {
        return 1;
    }

    object = packet != NULL && packet->owner != NULL ? packet->owner->objptr : NULL;
    enabled = 1;
    keep_out = -1;
    for (index = 0; index < param_4; ++index) {
        value = NuStrIStr(params[index], (char *)AIGameCharacterName);
        if (value != NULL) {
            object = GetNamedGameObject(sys, value + 10);
        } else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral757ec4)) != NULL) {
            keep_out = NuStrICmp(value + 9, (char *)AITransferLiteral757ebc) == 0;
        } else if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0) {
            enabled = 0;
        }
    }

    if (object != NULL) {
        if (enabled != 0) {
            object->flags_e20 |= 0x10000u;
            object->weapon_scale = 1.0f;
            if (keep_out != -1) {
                object->portal_flags = (u32)((object->portal_flags & ~0x10u) | (keep_out ? 0x10u : 0));
            }
        } else {
            object->weapon_scale = 0.0f;
            object->flags_e20 &= (u32)(~0x10000u);
            object->portal_flags &= (u32)(~0x10u);
        }
    }
    return 1;
}

i32 __cdecl Action_DontAvoidCharacter(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                      char **params, i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    GAMEOBJECT *object;
    GAMEOBJECT *dont_avoid;
    i32 enabled;
    char *value;

    if (first_time) {
        object = packet != NULL && packet->owner != NULL ? packet->owner->objptr : NULL;
        if (param_count > 0) {
            dont_avoid = NULL;
            enabled = 1;
            for (index = 0; index < param_count; ++index) {
                value = NuStrIStr(params[index], (char *)AIGameCharacterName);
                if (value != NULL) {
                    object = GetNamedGameObject(system, value + 10);
                    continue;
                }
                value = NuStrIStr(params[index], (char *)AITransferLiteral757a5c);
                if (value != NULL) {
                    dont_avoid = GetNamedGameObject(system, value + 11);
                    continue;
                }
                if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0)
                    enabled = 0;
            }
            if (object != NULL && dont_avoid != NULL)
                object->ai_packet.dont_avoid_character = enabled ? dont_avoid : NULL;
        }
    }
    return 1;
}

i32 __cdecl Action_CannotDropIn(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    i32 enabled;
    char *value;

    (void)processor;
    (void)param_6;
    if (param_5 != 0) {
        object = NULL;
        if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
            object = packet->owner->objptr;
        enabled = 1;
        for (index = 0; index < param_4; ++index) {
            value = NuStrIStr(params[index], (char *)AIGameCharacterName);
            if (value != NULL) {
                object = GetNamedGameObject(sys, value + 10);
            } else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral757ee0)) != NULL) {
                GetNamedGameObject(sys, value + 7);
            } else if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0) {
                enabled = 0;
            }
        }
        if (object != NULL) {
            object->portal_flags = (object->portal_flags & ~0x4000000u) | ((enabled & 1) << 26);
        }
    }
    return 1;
}

i32 __cdecl Action_SetSpline(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                             i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    NUGSPLINE *spline;
    i32 looping;
    char *value;
    SPLINEPOS *position;

    (void)sys;
    (void)processor;
    (void)param_6;
    object = packet != NULL && packet->owner != NULL ? packet->owner->objptr : NULL;
    if (object == NULL || param_5 == 0) {
        return 1;
    }

    spline = NULL;
    looping = 0;
    for (index = 0; index < param_4; ++index) {
        value = NuStrIStr(params[index], (char *)AITransferLiteral757a84);
        if (value != NULL) {
            spline = NuSplineFind(WORLD->current_gscn, value + 7);
        } else if (NuStrICmp(params[index], (char *)AITransferLiteral757a7c) == 0) {
            looping = 1;
        }
    }

    position = &object->movement_spline_position;
    memset(position, 0, sizeof(*position));
    if (spline != NULL) {
        InitSplinePosition(position, spline, 0.0f, looping);
    }
    return 1;
}

i32 __cdecl Action_CameraShake(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                               i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    f32 amount;
    f32 duration;
    f32 speed;
    char *value;

    (void)sys;
    (void)packet;
    (void)param_6;
    if (param_5 != 0) {
        amount = 1.0f;
        duration = 0.0f;
        speed = 1.0f;
        for (index = 0; index < param_4; ++index) {
            value = NuStrIStr(params[index], (char *)AITransferLiteral757cd8);
            if (value != NULL) {
                amount = AIParamToFloat(processor, value + 4);
            } else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral757cd0)) != NULL) {
                duration = AIParamToFloat(processor, value + 5);
            } else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral757cc8)) != NULL) {
                speed = AIParamToFloat(processor, value + 6);
            }
        }
        GameCam_NewShake(GameCam, amount, duration, speed);
    }
    return 1;
}

i32 Action_ResetGameCamera(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                           i32 param_4, i32 param_5, f32 param_6) {
    if (param_5 != 0) {
        GameCam_Reset(GameCam);
    }
    return 1;
}

i32 Action_PlayCutScene(AISYS_PC *unused0, AISCRIPTPROCESS_PC *unused1, AIPACKET_PC *unused2, char **params,
                        i32 param_count, i32 first_time, float unused6) {
    if (first_time != 0) {
        i32 i;
        char *name = NULL;
        for (i = 0; i < param_count; ++i) {
            char *value = NuStrIStr(params[i], "name");
            if (value != NULL) {
                name = value + 5;
            }
        }
        if (name != NULL) {
            NewCutScene(NULL, WORLD->cutscene_sys, name, 0);
        }
    }
    return 1;
}

i32 __cdecl Action_SetZeroAcceleration(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                       char **params, i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    GAMEOBJECT *object;
    i32 enabled;

    if (first_time) {
        object = NULL;
        if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
            object = packet->owner->objptr;
        enabled = 1;
        for (index = 0; index < param_count; ++index) {
            if (NuStrIStr(params[index], (char *)ActionSnapPlayer1Name) != NULL ||
                NuStrIStr(params[index], (char *)GameAIPlayerName) != NULL)
                object = player;
            else if (NuStrIStr(params[index], (char *)ActionSnapPlayer2Name) != NULL)
                object = player2;
            else if (NuStrIStr(params[index], (char *)AIActionFalseName) != NULL)
                enabled = 0;
        }
        if (object != NULL)
            object->ai_packet.flags = (object->ai_packet.flags & ~0x40000u) | ((enabled & 1) << 18);
    }
    return 1;
}

i32 __cdecl Action_FaceCharacter(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                 i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    char *value;
    GAMEOBJECT *target;

    if (first_time != 0) {
        for (index = 0; index < param_count; ++index) {
            value = NuStrIStr(params[index], (char *)AIGameCharacterName);
            if (value != NULL) {
                processor->action_data_3 = GetNamedGameObject(sys, value + 10);
            } else {
                processor->action_timer = AIParamToFloat(processor, params[index]);
            }
        }
    }
    target = (GAMEOBJECT *)(processor->action_data_3);
    if (packet != NULL && target != NULL) {
        packet->movement_look_target = &target->position;
    }
    if (processor->action_timer > 0.0f) {
        processor->action_timer -= elapsed;
        if (processor->action_timer <= 0.0f) {
            processor->action_timer = 0.0f;
            return 1;
        }
    }
    return 0;
}

i32 __cdecl Action_SetPath(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                           i32 param_4, i32 param_5, f32 param_6) {
    AIPATHSYS_PC *path_system;
    GAMEOBJECT *object;
    AIPATH_PC *path;
    char *value;
    APIOBJECT_PC *named;
    i32 index, path_index;

    (void)param_6;
    path = NULL;
    object = NULL;
    if (param_5 == 0) {
        return 1;
    }
    if (sys == NULL) {
        return 1;
    }
    if (sys->path_sys == NULL) {
        return 1;
    }

    path_system = sys->path_sys;
    if (packet != NULL && packet->owner != NULL) {
        object = packet->owner->objptr;
    }
    for (index = 0; index < param_4; ++index) {
        value = NuStrIStr(params[index], (char *)AIGameCharacterName);
        if (value != NULL) {
            named = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(sys, value + 10) : NULL;
            object = named != NULL ? (GAMEOBJECT *)named->owner : NULL;
        } else if (NuStrIStr(params[index], (char *)AITransferLiteral757e70) != NULL) {
            path = sys->path_sys->active_path;
        } else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral757e68)) != NULL) {
            value += 5;
            for (path_index = 0; path_index < path_system->path_count; ++path_index) {
                if (NuStrICmp(path_system->paths[path_index]->name, value) == 0) {
                    path = path_system->paths[path_index];
                    break;
                }
            }
        }
    }
    if (object != NULL && path != NULL) {
        AISysCharacterSetPath(&object->ai_packet, path);
        AISysGetCharacterPathPos(WORLD->ai_sys, (APIOBJECT_PC *)object, &object->ai_packet, 0xff, 1);
    }
    return 1;
}

i32 __cdecl Action_SetInvulnerable(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                   i32 param_4, i32 param_5, f32 param_6) {
    i32 types[10];
    i32 type_count;
    i32 still_take_hit;
    GAMEOBJECT *object;
    i32 enabled;
    char *value;
    i32 local_type;
    i32 global_type;
    GAMEOBJECT *candidate;
    i32 index, object_index, type_index;
    (void)processor;
    (void)param_6;
    if (param_5 == 0) {
        return 1;
    }

    type_count = 0;
    still_take_hit = 0;
    object = NULL;
    if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
        object = packet->owner->objptr;
    enabled = 1;
    for (index = 0; index < param_4; ++index) {
        value = NuStrIStr(params[index], (char *)AITransferLiteral75772c);
        if (value != NULL) {
            if (LevelCharacterTypeIDFn != NULL && LevelCharacterGlobalIDFn != NULL) {
                local_type = (u8)(LevelCharacterTypeIDFn(value + 5));
                if (local_type != 0xff) {
                    global_type = LevelCharacterGlobalIDFn(local_type);
                    if (global_type != -1 && type_count < 10) {
                        types[type_count++] = global_type;
                    }
                }
            }
            continue;
        }
        value = NuStrIStr(params[index], (char *)AIGameCharacterName);
        if (value != NULL) {
            object = GetNamedGameObject(sys, value + 10);
        } else if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0) {
            enabled = 0;
        } else if (NuStrICmp(params[index], (char *)AITransferLiteral757a44) == 0) {
            still_take_hit = 1;
        }
    }

    if (type_count != 0) {
        candidate = Obj;

        for (object_index = 0; object_index < HIGHGAMEOBJECT; ++object_index, ++candidate) {
            if ((candidate->flags_1fc_word & (1u | 0x1000u)) != (1u | 0x1000u) || (candidate->flags_1f8 & 0x400) == 0) {
                continue;
            }
            for (type_index = 0; type_index < type_count; ++type_index) {
                if ((i16)candidate->id == types[type_index]) {
                    if (enabled) {
                        candidate->flags_ef0 =
                            (candidate->flags_ef0 & ~0x1000u) | 0x400000u | ((still_take_hit & 1) << 12);
                    } else {
                        candidate->flags_ef0 &= ~0x400000u;
                    }
                }
            }
        }
    } else if (object != NULL && (object->flags_1f8 & 0x400) != 0) {
        if (enabled) {

            object->flags_ef0 = (object->flags_ef0 & ~0x1000u) | 0x400000u | ((still_take_hit & 1) << 12);
        } else {
            object->flags_ef0 &= ~0x400000u;
        }
    }
    return 1;
}

typedef struct GIZSPECIAL_PARTIAL_PC {
    u8 unknown_00[0x20];
    GAMEANIMSET *anim_set;
} GIZSPECIAL_PARTIAL_PC;
STATIC_ASSERT(offsetof(GIZSPECIAL_PARTIAL_PC, anim_set) == 0x20);
i32 __cdecl Action_SetSide(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                           i32 param_4, i32 param_5, f32 param_6) {
    char *value;
    GAMEOBJECT *object;
    i32 side;
    f32 range_squared;
    i32 type_ids[10];
    i32 type_count;
    i32 local_type;
    i32 global_type;
    u32 old_side;
    u32 model_flags;
    AICREATURE_PC *creature;
    f32 distance_squared;
    GAMEOBJECT *candidate;
    i32 type_matches;
    NUVEC offset;
    i32 index, type_index;

    (void)param_6;
    range_squared = 0.0f;
    distance_squared = 0.0f;
    side = 0;
    type_count = 0;
    object = NULL;
    if (param_5 != 0) {
        if (packet != NULL && packet->owner != NULL && (packet->owner->flags_1fc & 1u) != 0 &&
            packet->owner->objptr != NULL) {
            object = packet->owner->objptr;
        }

        for (index = 0; index < param_4; ++index) {
            if (NuStrICmp(params[index], (char *)AITransferLiteral75773c) == 0) {
                side = 0;
            } else if (NuStrICmp(params[index], (char *)AITransferLiteral7576a8) == 0 ||
                       NuStrICmp(params[index], (char *)AITransferLiteral7576b0) == 0) {
                side = -1;
            } else if (NuStrICmp(params[index], (char *)AITransferLiteral7576b8) == 0 ||
                       NuStrICmp(params[index], (char *)AITransferLiteral7576c0) == 0) {
                side = 1;
            } else if (NuStrICmp(params[index], (char *)AITransferLiteral75768c) == 0 ||
                       NuStrICmp(params[index], (char *)AITransferLiteral75769c) == 0) {
                side = 2;
            } else if (NuStrICmp(params[index], (char *)GizmoDefaultOutputName) == 0) {
                side = 3;
            } else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral75772c)) != NULL) {
                if (LevelCharacterTypeIDFn != NULL && LevelCharacterGlobalIDFn != NULL) {
                    local_type = (u8)(LevelCharacterTypeIDFn(value + 5));
                    if (local_type != 0xff) {
                        global_type = LevelCharacterGlobalIDFn(local_type);
                        if (global_type != -1 && type_count < 10) {
                            type_ids[type_count++] = global_type;
                        }
                    }
                }
            } else if ((value = NuStrIStr(params[index], (char *)AIGameCharacterName)) != NULL) {
                object = GetNamedGameObject(sys, value + 10);
            } else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral757724)) != NULL) {
                range_squared = AIParamToFloat(processor, value + 6);
                range_squared *= range_squared;
            }
        }

        if (type_count <= 0 && !(range_squared > 0.0f)) {
            if (object != NULL) {
                old_side = object->flags_1f8 & 0x10005;
                object->flags_1f8 &= 0xfffefffau;
                switch (side) {
                    case 0:
                        object->flags_1f8 |= 4;
                        break;
                    case 1:
                        object->flags_1f8 |= 1;
                        break;
                    case 2:
                        object->flags_1f8 |= 0x10000;
                        break;
                    case 3:
                        if ((object->flags_1f8 & 0x400u) != 0 && object->ai_packet.creature_index != 0xff) {
                            creature = sys->creatures + object->ai_packet.creature_index;
                            model_flags = apicharsys->char_data[creature->type].model_flags;
                            if ((model_flags & 0x200) != 0) {
                                object->flags_1f8 |= 4;
                            } else if ((model_flags & 4) != 0) {
                                object->flags_1f8 |= 1;
                            }
                        }
                        break;
                }

                object->ai_packet.capabilities &= ~(LEGO_AIPATHCNX_FORBADDIES | LEGO_AIPATHCNX_FORGOODIES);
                if ((object->flags_1f8 & 1) != 0) {
                    object->ai_packet.capabilities |= LEGO_AIPATHCNX_FORBADDIES;
                } else if ((object->flags_1f8 & 4) == 0) {
                    object->ai_packet.capabilities |= LEGO_AIPATHCNX_FORGOODIES;
                }

                if ((old_side & object->flags_1f8) == 0) {
                    object->ai_packet.flags &= ~0x800u;
                    object->ai_packet.opponent = NULL;
                    object->alert_target = NULL;
                    object->field_ec4 = 0.0f;
                }
            }
            return 1;
        }

        for (index = 0; index < HIGHGAMEOBJECT; ++index) {
            candidate = &Obj[index];
            type_matches = 0;
            if ((candidate->flags_1fc_word & 1u) == 0 || (candidate->flags_1fc_word & 0x1000u) == 0 ||
                (candidate->flags_1f8 & 0x400u) == 0) {
                continue;
            }

            for (type_index = 0; type_index < type_count; ++type_index) {
                if ((i16)candidate->id == type_ids[type_index]) {
                    type_matches = 1;
                }
            }
            if (range_squared > 0.0f && object != NULL) {
                NuVecSub(&offset, &object->position, &candidate->position);
                distance_squared = offset.x * offset.x + offset.y * offset.y + offset.z * offset.z;
            }
            if ((type_matches == 1 && (distance_squared < range_squared || range_squared == 0.0f)) ||
                (type_count == 0 && distance_squared < range_squared)) {
                old_side = candidate->flags_1f8 & 0x10005;
                candidate->flags_1f8 &= 0xfffefffau;
                switch (side) {
                    case 0:
                        candidate->flags_1f8 |= 4;
                        break;
                    case 1:
                        candidate->flags_1f8 |= 1;
                        break;
                    case 2:
                        candidate->flags_1f8 |= 0x10000;
                        break;
                    case 3:
                        if ((candidate->flags_1f8 & 0x400u) != 0 && candidate->ai_packet.creature_index != 0xff) {
                            creature = sys->creatures + candidate->ai_packet.creature_index;
                            model_flags = apicharsys->char_data[creature->type].model_flags;
                            if ((model_flags & 0x200) != 0) {
                                candidate->flags_1f8 |= 4;
                            } else if ((model_flags & 4) != 0) {
                                candidate->flags_1f8 |= 1;
                            }
                        }
                        break;
                }

                candidate->ai_packet.capabilities &= ~(LEGO_AIPATHCNX_FORBADDIES | LEGO_AIPATHCNX_FORGOODIES);
                if ((candidate->flags_1f8 & 1) != 0) {
                    candidate->ai_packet.capabilities |= LEGO_AIPATHCNX_FORBADDIES;
                } else if ((candidate->flags_1f8 & 4) == 0) {
                    candidate->ai_packet.capabilities |= LEGO_AIPATHCNX_FORGOODIES;
                }

                if ((old_side & candidate->flags_1f8) == 0) {
                    candidate->ai_packet.flags &= ~0x800u;
                    candidate->ai_packet.opponent = NULL;
                    candidate->alert_target = NULL;
                    candidate->field_ec4 = 0.0f;
                }
            }
        }
    }
    return 1;
}

i32 __cdecl Action_SetTakeOverTarget(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                     char **params, i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    GAMEOBJECT *object;
    char *type_name;
    GAMEOBJECT *target;
    f32 nearest;
    i32 check_takeover;
    char *value;
    i32 type;
    GAMEOBJECT *candidate;
    NUVEC difference;
    f32 distance;
    GAMEOBJECT *previous;

    if (!first_time)
        return 1;
    object = NULL;
    if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
        object = packet->owner->objptr;
    type_name = NULL;
    target = NULL;
    nearest = 1000000000.0f;
    check_takeover = 0;
    for (index = 0; index < param_count; ++index) {
        value = NuStrIStr(params[index], (char *)AITransferLiteral7576f8);
        if (value != NULL)
            type_name = value + 5;
        else if ((value = NuStrIStr(params[index], (char *)AICombatOpponentName)) != NULL)
            target = GetNamedGameObject(system, value + 9);
        else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral7576e0)) != NULL) {
            nearest = AIParamToFloat(processor, value + 9);
            nearest *= nearest;
        } else if (NuStrICmp(params[index], (char *)AITransferLiteral7576d0) != 0)
            check_takeover = 1;
    }
    if (target == NULL && type_name == NULL)
        return 1;
    type = -1;
    for (index = 0; index < CHARCOUNT && type == -1; ++index) {
        if (NuStrICmp(CDataList[index].name, type_name) == 0)
            type = index;
    }
    for (index = 0; index < HIGHGAMEOBJECT; ++index) {
        candidate = &Obj[index];
        if ((candidate->flags_1fc_word & 0x1001) != 0x1001 || (i16)candidate->id != type)
            continue;
        if (candidate->takeover_target != NULL && candidate->takeover_target != object)
            continue;
        if (candidate->takeover_source != NULL &&
            (candidate->takeover_source->character_context == 0x3b || candidate->character_context != 0x3b))
            continue;

        NuVecSub(&difference, &candidate->collision_position, &packet->owner->collision_position);
        distance = NuVecMagSqr(&difference);
        if (!(distance < nearest))
            continue;
        nearest = distance;
        target = candidate;
    }
    if (target == NULL) {
        object->takeover_target = NULL;
        return 1;
    }
    if (check_takeover &&
        (target->takeover_source != NULL || !(target->character_data->game_character->flags_090 & 0x40) ||
         (object->character_data->model_flags & 0x10)))
        return 1;
    previous = object->takeover_target;
    if (previous != NULL && previous->takeover_target != NULL && previous->takeover_target == object)
        previous->takeover_target = NULL;
    object->takeover_target = target;
    target->takeover_target = object;
    return 1;
}

i32 __cdecl Action_SetStateArea(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                i32 param_4, i32 param_5, f32 param_6) {
    char *state_name;
    i32 types[10];
    i32 type_count;
    f32 range_squared;
    char *value;
    f32 range;
    i32 local_type;
    i32 global_type;
    NUVEC origin;
    GAMEOBJECT *object;
    NUVEC difference;
    f32 distance_squared;
    i32 matching_type;
    AIPACKET_PC *object_packet;
    i32 index, object_index, type_index;
    (void)sys;
    (void)param_6;
    if (param_5 == 0) {
        return 1;
    }

    state_name = NULL;

    type_count = 0;
    range_squared = 0.0f;
    for (index = 0; index < param_4; ++index) {
        value = NuStrIStr(params[index], (char *)AITransferLiteral757724);
        if (value != NULL) {
            range = AIParamToFloat(processor, value + 6);
            range_squared = range * range;
        } else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral75772c)) != NULL) {
            if (LevelCharacterTypeIDFn != NULL && LevelCharacterGlobalIDFn != NULL) {
                local_type = (u8)(LevelCharacterTypeIDFn(value + 5));
                if (local_type != 0xff) {
                    global_type = LevelCharacterGlobalIDFn(local_type);
                    if (global_type != -1 && type_count < 10) {
                        types[type_count++] = global_type;
                    }
                }
            }
        } else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral757744)) != NULL) {
            state_name = value + 6;
        }
    }

    if (state_name == NULL || (type_count == 0 && !(range_squared > 0.0f))) {
        processor->next_state = AIStateFind(state_name, processor->script);
        return 1;
    }

    if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL) {
        origin = packet->owner->objptr->collision_position;
    }

    for (object_index = 0; object_index < HIGHGAMEOBJECT; ++object_index) {
        object = &Obj[object_index];
        if ((object->flags_1fc_word & (1u | 0x1000u)) != (1u | 0x1000u) || (object->flags_1f8 & 0x400) == 0) {
            continue;
        }

        NuVecSub(&difference, &origin, &object->position);
        distance_squared = difference.x * difference.x + difference.y * difference.y + difference.z * difference.z;

        matching_type = type_count == 0;
        for (type_index = 0; type_index < type_count; ++type_index) {
            if ((i16)object->id == types[type_index]) {
                matching_type = 1;
            }
        }
        if (matching_type != 0 && range_squared > distance_squared) {
            object_packet = ((APIOBJECT_PC *)object)->ai;
            object_packet->script_processor.next_state =
                AIStateFind(state_name, object_packet->script_processor.script);
        }
    }
    return 1;
}

i32 __cdecl Action_SnapToOrigin(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    i32 check_terrain;
    char *value;
    AICREATURE_PC *creature;

    (void)processor;
    (void)param_6;
    if (param_5 == 0) {
        return 1;
    }

    object = NULL;
    if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
        object = packet->owner->objptr;
    check_terrain = 1;
    for (index = 0; index < param_4; ++index) {
        value = NuStrIStr(params[index], (char *)AreaConfigKeyword_character);
        if (value != NULL) {
            object = GetNamedGameObject(sys, value + 10);
        } else if (NuStrIStr(params[index], (char *)ActionSnapDontCheckTerrainName) != NULL) {
            check_terrain = 0;
        }
    }

    if (object == NULL || (object->flags_1f8 & 0x400) == 0 || object->ai_packet.creature_index == 0xff) {
        return 1;
    }

    creature = &sys->creatures[object->ai_packet.creature_index];
    object->position = creature->position;
    object->field_0x276 = (u16)creature->y_rotation;
    object->facing_angle = (u16)creature->y_rotation;
    object->movement_facing_angle = (u16)creature->y_rotation;
    object->ai_packet.path_info.flags_word &= ~1u;
    object->collision_position = object->position;
    object->initial_position = object->position;
    plr_lastpos = object->collision_position;
    object->start_position = object->collision_position;
    object->respawn_position = object->position;
    object->last_safe_position = object->position;
    object->ai_update_position = object->position;
    object->velocity = v000;
    InitSurfaceInfo(object);
    if (check_terrain != 0) {
        SetObjOnSurface(object, 0);
    }
    return 1;
}

i32 __cdecl Action_GameDontPush(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    i32 dont_push;
    i32 types[10];
    i32 type_count;
    i32 type_index;
    i32 object_index;
    GAMEOBJECT *candidate;
    char *value;
    i32 local_type;
    i32 global_type;

    (void)processor;
    (void)param_6;
    if (param_5 == 0) {
        return 1;
    }

    object = NULL;
    if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
        object = packet->owner->objptr;
    dont_push = 1;

    type_count = 0;

    for (index = 0; index < param_4; ++index) {
        value = NuStrIStr(params[index], (char *)AITransferLiteral75772c);
        if (value != NULL) {
            if (LevelCharacterTypeIDFn != NULL && LevelCharacterGlobalIDFn != NULL) {
                local_type = (u32)LevelCharacterTypeIDFn(value + 5);
                if (local_type != 0xff) {
                    global_type = LevelCharacterGlobalIDFn(local_type);
                    if (global_type != -1 && type_count < 10)
                        types[type_count++] = global_type;
                }
            }
            continue;
        }
        value = NuStrIStr(params[index], (char *)AIGameCharacterName);
        if (value != NULL) {
            object = GetNamedGameObject(sys, value + 10);
        } else if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0) {
            dont_push = 0;
        }
    }
    if (type_count != 0) {
        candidate = Obj;
        for (object_index = 0; object_index < HIGHGAMEOBJECT; ++object_index, ++candidate) {
            if ((candidate->flags_1fc_word & 0x1001u) != 0x1001u || (candidate->flags_1f8 & 0x400u) == 0)
                continue;
            for (type_index = 0; type_index < type_count; ++type_index) {
                if ((i16)candidate->id == types[type_index])
                    candidate->flags_1fc_word = (candidate->flags_1fc_word & ~2u) | ((dont_push & 1) << 1);
            }
        }
    } else if (object != NULL) {
        object->flags_1fc_word = (object->flags_1fc_word & ~2u) | ((dont_push & 1) << 1);
    }
    return 1;
}

i32 __cdecl Action_SetAIOverrideControl(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                        char **params, i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    APIOBJECT_PC *object;
    i32 enabled;
    char *value;
    GAMEOBJECT *character;

    enabled = 1;
    if (!first_time) {
        object = processor->action_data_3;
    } else {
        object = packet != NULL && packet->owner != NULL ? (APIOBJECT_PC *)packet->owner : NULL;
        for (index = 0; index < param_count; ++index) {
            value = NuStrIStr(params[index], (char *)AreaConfigKeyword_character);
            if (value != NULL) {
                if (GetNamedGameObjectCallback != NULL)
                    object = GetNamedGameObjectCallback(system, value + 10);
            } else if (NuStrICmp((char *)AIActionFalseName, params[index]) == 0) {
                enabled = 0;
            }
        }
        processor->action_data_3 = object;
    }
    if (object != NULL) {
        if (enabled) {
            character = (GAMEOBJECT *)object->owner;
            if (character != NULL) {
                {
                    character->field_1010 = 0.0f;
                    character->death_timer = 0;
                    if (character->dead != 0) {
                        character->dead = 0;
                        character->current_hp = character->field_107e;
                        character->shield_hitpoints = character->character_data->game_character->shield_hitpoints;
                        character->saved_hitpoints = 4;
                    }
                }
                if (character->takeover_source != NULL) {
                    character->takeover_source->field_1010 = 0.0f;
                    character->takeover_source->death_timer = 0;
                    if (character->takeover_source->dead != 0) {
                        character->takeover_source->dead = 0;
                        character->takeover_source->current_hp = character->takeover_source->field_107e;
                        character->takeover_source->shield_hitpoints =
                            character->takeover_source->character_data->game_character->shield_hitpoints;
                        character->takeover_source->saved_hitpoints = 4;
                    }
                }
            }
        }
        object->flags_low = (object->flags_low & ~0x100u) | ((enabled & 1) << 8);
    }
    return 1;
}

i32 __cdecl Action_PlayGizSpecial(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                  i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GIZSPECIAL_PARTIAL_PC *special;
    i32 backwards;
    i32 snap;
    i32 restart;
    char *value;
    GIZMO_PC *gizmo;

    (void)sys;
    (void)processor;
    (void)packet;
    (void)param_6;
    if (param_5 == 0 || param_4 <= 0) {
        return 1;
    }

    special = NULL;
    backwards = 0;
    snap = 0;
    restart = 0;
    for (index = 0; index < param_4; ++index) {
        value = NuStrIStr(params[index], (char *)AIScriptActionNameArgPrefix);
        if (value != NULL) {
            gizmo = (GIZMO_PC *)GizmoFindByName(WORLD->gizmo_sys, gizspecial_gizmotype_id, value + 5);
            if (gizmo != NULL) {
                special = (GIZSPECIAL_PARTIAL_PC *)(gizmo->object);
            }
        } else if (NuStrICmp(params[index], (char *)AITransferLiteral757ea8) == 0) {
            backwards = 1;
        } else if (NuStrICmp(params[index], (char *)GizmoSnapOptionNamePc) == 0) {
            snap = 1;
        } else if (NuStrICmp(params[index], (char *)AITransferLiteral757eb4) == 0) {
            restart = 1;
        }
    }

    if (special == NULL) {
        return 1;
    }
    if (backwards != 0) {
        if (snap != 0) {
            GameAnimSet_JumpToStart(special->anim_set);
        } else {
            if (restart != 0) {
                GameAnimSet_JumpToEnd(special->anim_set);
            }
            GameAnimSet_Play(special->anim_set, -1.0f, 1);
        }
    } else if (snap != 0) {
        GameAnimSet_JumpToEnd(special->anim_set);
    } else {
        if (restart != 0) {
            GameAnimSet_JumpToStart(special->anim_set);
        }
        GameAnimSet_Play(special->anim_set, 1.0f, 1);
    }
    return 1;
}

i32 Action_SetObstacleToEnd(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                            i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)param_6;
    if (param_5 != 0 && param_4 != 0) {
        i32 index;
        GIZOBSTACLE *obstacle = NULL;
        for (index = 0; index < param_4; ++index) {
            char *value = NuStrIStr(params[index], "name=");
            if (value != NULL) {
                obstacle = GizObstacle_FindByName(WORLD->giz_obstacle_sys, value + 5);
            }
        }
        if (obstacle != NULL) {
            GizObstacle_JumpToEnd(obstacle);
        }
    }
    return 1;
}

i32 __cdecl Action_SetLastAttacker(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                   i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *victim;
    GAMEOBJECT *attacker;
    char *value;

    (void)processor;
    (void)param_6;
    if (param_5 == 0) {
        return 1;
    }

    victim = packet != NULL && packet->owner != NULL ? packet->owner->objptr : NULL;
    attacker = NULL;
    for (index = 0; index < param_4; ++index) {
        value = NuStrIStr(params[index], (char *)AITransferLiteral757a20);
        if (value != NULL) {
            victim = GetNamedGameObject(sys, value + NuStrLen((char *)AITransferLiteral757a20));
            continue;
        }
        if (NuStrICmp(params[index], (char *)AITransferLiteral7583d0) == 0) {
            if (packet != NULL && packet->opponent != NULL) {
                attacker = (GAMEOBJECT *)((APIOBJECT_PC *)packet->opponent)->owner;
            }
            continue;
        }
        value = NuStrIStr(params[index], (char *)AITransferLiteral7583c4);
        if (value != NULL) {
            attacker = GetNamedGameObject(sys, value + 9);
        } else if (NuStrICmp(params[index], (char *)AITransferLiteral7583b4) == 0) {
            attacker = (GAMEOBJECT *)((APIOBJECT_PC *)sys->player_1)->owner;
        }
    }
    if (victim != NULL && attacker != NULL) {
        victim->last_attacker = attacker;
    }
    return 1;
}

i32 __cdecl Action_SetUseOneAtOnce(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                   i32 param_4, i32 param_5, f32 param_6) {
    i32 enabled;
    GAMEOBJECT *object;
    i32 index;
    char *name;
    (void)processor;
    (void)param_6;
    if (param_5 != 0) {
        enabled = 1;
        object = packet != NULL ? packet->owner->objptr : NULL;

        for (index = 0; index < param_4; ++index) {
            name = NuStrIStr(params[index], (char *)AreaConfigKeyword_character);
            if (name != NULL) {
                name += 10;
                if (NuStrICmp(name, (char *)AITransferLiteral757f7c) == 0 && packet != NULL)
                    object = packet->owner->objptr;
                else
                    object = GetNamedGameObject(WORLD->ai_sys, name);
            } else if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0)
                enabled = 0;
        }
        if (object != NULL) {
            object->flags_ef4 = (object->flags_ef4 & ~0x2000u) | ((enabled & 1) << 13);
        }
    }
    return 1;
}

i32 __cdecl Action_SetAO_MaxAttackers(AISYS_PC *unused0, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *unused2,
                                      char **params, i32 count, i32 first_time, f32 unused6) {
    i32 index;
    char *value;
    if (first_time != 0) {
        if (count == 0)
            return 1;
        for (index = 0; index < count; ++index) {
            value = NuStrIStr(params[index], (char *)AIExpressionMaxName);
            if (value != NULL)
                oneAtOnce_SetNumAttackers((i32)AIParamToFloat(processor, value + 4));
        }
    }
    return 1;
}

i32 __cdecl Action_SetAO_AttackersPerRow(AISYS_PC *unused0, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *unused2,
                                         char **params, i32 count, i32 first_time, f32 unused6) {
    i32 index;
    char *value;
    if (first_time != 0) {
        if (count == 0)
            return 1;
        for (index = 0; index < count; ++index) {
            value = NuStrIStr(params[index], (char *)AIActionNumName);
            if (value != NULL)
                oneAtOnce_SetAttackersPerRow((i32)AIParamToFloat(processor, value + 4));
        }
    }
    return 1;
}

i32 __cdecl Action_SetAO_RowDist(AISYS_PC *unused0, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *unused2, char **params,
                                 i32 count, i32 first_time, f32 unused6) {
    i32 index;
    char *value;
    if (first_time != 0) {
        for (index = 0; index < count; ++index) {
            value = NuStrIStr(params[index], (char *)AIActionDistName);
            if (value != NULL)
                oneAtOnce_SetDistPerRow(AIParamToFloat(processor, value + 5));
        }
    }
    return 1;
}

i32 __cdecl Action_SetAO_InitRowDist(AISYS_PC *unused0, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *unused2,
                                     char **params, i32 count, i32 first_time, f32 unused6) {
    i32 index;
    char *value;
    if (first_time != 0) {
        for (index = 0; index < count; ++index) {
            value = NuStrIStr(params[index], (char *)AIActionDistName);
            if (value != NULL)
                oneAtOnce_SetInitDistPerRow(AIParamToFloat(processor, value + 5));
        }
    }
    return 1;
}

static i32 Action_SetTechnoComplete(AISYS_PC *unused0, AISCRIPTPROCESS_PC *unused1, AIPACKET_PC *packet, char **params,
                                    i32 param_count, i32 first_time, f32 unused6) {
    GAMEOBJECT *object = packet != NULL && packet->owner != NULL ? packet->owner->objptr : NULL;
    if (first_time) {
        i32 index;
        TECHNO_s *techno = NULL;
        i32 complete = 1;
        for (index = 0; index < param_count; ++index) {
            char *value = NuStrIStr(params[index], "techno");
            if (value != NULL) {
                char *name = value + NuStrLen("techno") + 1;
                i32 type = GizmoGetTypeIDByName(WORLD->gizmo_sys, "Techno");
                GIZMO_PC *gizmo = GizmoFindByName(WORLD->gizmo_sys, type, name);
                if (gizmo != NULL && gizmo->object != NULL)
                    techno = ((TECHNO_s *)(gizmo->object));
            } else if (NuStrICmp(params[index], "FALSE") == 0) {
                complete = 0;
            } else if ((value = NuStrIStr(params[index], "controlling")) != NULL) {
                object = GetNamedGameObject(WORLD->ai_sys, value + NuStrLen("controlling") + 1);
            }
        }
        if (object != NULL)
            techno = Technos_FindControllingTechno(object);
        if (techno != NULL)
            techno->complete = complete;
    }
    return 1;
}

i32 __cdecl Action_NoIdleSpeed(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                               i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    APIOBJECT_PC *lookup;
    GAMEOBJECT *object;
    i32 enabled = 1;
    char *name;

    (void)processor;
    (void)param_6;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    if (param_5 != 0) {
        for (index = 0; index < param_4; ++index) {
            name = NuStrIStr(params[index], (char *)AIGameCharacterName);
            if (name != NULL) {
                {
                    lookup = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(sys, name + 10) : NULL;
                    object = lookup != NULL ? (GAMEOBJECT *)lookup->owner : NULL;
                }
            } else if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0) {
                enabled = 0;
            }
        }
        if (object != NULL) {
            ((AI_ACTION_FLAGS_EF4_PC *)&object->flags_ef4)->bits.no_idle_speed = enabled;
        }
    }
    return 1;
}

i32 __cdecl Action_CatchUpForbidden(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                    i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    APIOBJECT_PC *lookup;
    GAMEOBJECT *object;
    i32 forbidden;
    char *parameter;

    (void)processor;
    (void)param_6;
    if (param_5 != 0) {
        object = NULL;
        if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
            object = packet->owner->objptr;
        forbidden = 1;
        if (param_4 != 0) {
            for (index = 0; index < param_4; ++index) {
                parameter = NuStrIStr(params[index], (char *)AIGameCharacterName);
                if (parameter != NULL) {
                    {
                        lookup =
                            GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(sys, parameter + 10) : NULL;
                        object = lookup != NULL ? (GAMEOBJECT *)lookup->owner : NULL;
                    }
                } else if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0) {
                    forbidden = 0;
                }
            }
        }
        if (object != NULL) {
            object->portal_flags = (u32)((object->portal_flags & ~0x2000000u) | ((forbidden & 1) << 25));
        }
    }
    return 1;
}

i32 __cdecl Action_KeepWeaponOut(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                 i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    APIOBJECT_PC *lookup;
    GAMEOBJECT *object = NULL;
    i32 keep_out = 1;
    char *value;

    (void)processor;
    (void)param_6;
    if (param_5 != 0) {
        if (packet != NULL && packet->owner != NULL)
            object = packet->owner->objptr;
        if (param_4 != 0) {
            for (index = 0; index < param_4; ++index) {
                if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0) {
                    keep_out = 0;
                } else {
                    value = NuStrIStr(params[index], (char *)AIGameCharacterName);
                    if (value != NULL) {
                        lookup =
                            GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(sys, value + 10) : NULL;
                        object = lookup != NULL ? (GAMEOBJECT *)lookup->owner : NULL;
                    }
                }
            }
        }
        if (object != NULL) {
            object->portal_flags = (object->portal_flags & ~0x10u) | ((keep_out & 1) << 4);
        }
    }
    return 1;
}

i32 __cdecl Action_NotWithParty(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    APIOBJECT_PC *lookup;
    GAMEOBJECT *object = NULL;
    i32 enabled = 1;
    char *name;

    (void)processor;
    (void)param_6;
    if (param_5 != 0) {
        if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
            object = packet->owner->objptr;
        for (index = 0; index < param_4; ++index) {
            name = NuStrIStr(params[index], (char *)AIGameCharacterName);
            if (name != NULL) {
                lookup = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(sys, name + 10) : NULL;
                object = lookup != NULL ? (GAMEOBJECT *)lookup->owner : NULL;
            } else if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0) {
                enabled = 0;
            }
        }
        if (object != NULL) {
            object->flags_ef0 = (object->flags_ef0 & ~0x1000000u) | ((enabled & 1) << 24);
        }
    }
    return 1;
}

i32 __cdecl Action_CanAttack(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                             i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    APIOBJECT_PC *lookup;
    GAMEOBJECT *object = NULL;
    i32 enabled = 1;
    char *name;

    (void)processor;
    (void)param_6;
    if (param_5 != 0) {
        if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
            object = packet->owner->objptr;
        for (index = 0; index < param_4; ++index) {
            name = NuStrIStr(params[index], (char *)AIGameCharacterName);
            if (name != NULL) {
                lookup = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(sys, name + 10) : NULL;
                object = lookup != NULL ? (GAMEOBJECT *)lookup->owner : NULL;
            } else if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0) {
                enabled = 0;
            }
        }
        if (object != NULL) {
            object->portal_flags = (object->portal_flags & ~0x200u) | ((enabled & 1) << 9);
        }
    }
    return 1;
}

i32 __cdecl Action_SetDontMove(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                               i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    APIOBJECT_PC *lookup;
    GAMEOBJECT *object = NULL;
    i32 enabled = 1;
    char *name;

    (void)processor;
    (void)param_6;
    if (param_5 != 0) {
        if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
            object = packet->owner->objptr;
        for (index = 0; index < param_4; ++index) {
            name = NuStrIStr(params[index], (char *)AreaConfigKeyword_character);
            if (name != NULL) {
                lookup = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(sys, name + 10) : NULL;
                object = lookup != NULL ? (GAMEOBJECT *)lookup->owner : NULL;
            } else if (NuStrICmp((char *)AIActionFalseName, params[0]) == 0) {
                // This action's FALSE toggle uses the first parameter, not the current one.
                enabled = 0;
            }
        }
        if (object != NULL) {
            object->flags_ef0 = (object->flags_ef0 & ~0x10u) | ((enabled & 1) << 4);
        }
    }
    return 1;
}

i32 __cdecl Action_DontSetStoppedFlag(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                      char **params, i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    APIOBJECT_PC *lookup;
    GAMEOBJECT *object = NULL;
    i32 enabled = 1;
    char *value;

    if (first_time) {
        if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
            object = packet->owner->objptr;
        for (index = 0; index < param_count; ++index) {
            value = NuStrIStr(params[index], (char *)AreaConfigKeyword_character);
            if (value != NULL) {
                lookup = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(system, value + 10) : NULL;
                object = lookup != NULL ? (GAMEOBJECT *)lookup->owner : NULL;
            } else if (NuStrICmp((char *)AIActionFalseName, params[0]) == 0)
                enabled = 0;
        }
        if (object != NULL)
            object->flags_ef0 = (object->flags_ef0 & ~0x20u) | ((enabled & 1) << 5);
    }
    return 1;
}

i32 __cdecl Action_Respawnable(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                               i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    (void)sys;
    (void)processor;
    (void)param_6;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    if (param_5 != 0) {
        object->portal_flags = (object->portal_flags & ~0x200000u) | 0x100000u;
        for (index = 0; index < param_4; ++index) {
            if (NuStrICmp(params[index], (char *)AIPathOriginName) == 0) {
                object->portal_flags |= 0x200000u;
            } else if (NuStrICmp(params[index], (char *)AIScriptFalse) == 0) {
                object->portal_flags &= ~0x100000u;
            }
        }
    }
    return 1;
}

i32 __cdecl Action_UseTimeBasedUpdate(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                      i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    GAMEOBJECT *object;
    i32 enabled = 0;
    if (packet != NULL && packet->owner != NULL) {
        object = packet->owner->objptr;
        if (first_time != 0) {
            for (index = 0; index < param_count; ++index) {
                if (NuStrICmp(params[index], (char *)AITransferLiteral757ebc) == 0)
                    enabled = 1;
                else if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0)
                    enabled = 0;
            }
        }
        object->flags_ef4 = (object->flags_ef4 & ~0x20u) | (((enabled == 0) & 1) << 5);
    }
    return 1;
}

i32 __cdecl Action_CanOpenDoors(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    (void)sys;
    (void)processor;
    (void)param_6;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    if (param_5 != 0) {
        object->flags_1044 |= 1;
        for (index = 0; index < param_4; ++index) {
            if (NuStrICmp(params[index], (char *)AITransferLiteral757ebc) == 0) {
                object->flags_1044 |= 1u;
            } else if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0) {
                object->flags_1044 &= ~1u;
            }
        }
    }
    return 1;
}

i32 __cdecl Action_CanShootOffScreen(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                     i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    GAMEOBJECT *object;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    if (first_time != 0) {
        object->flags_1044 |= 4u;
        for (index = 0; index < param_count; ++index) {
            if (NuStrICmp(params[index], (char *)AITransferLiteral757ebc) == 0) {
                object->flags_1044 |= 4u;
            } else if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0) {
                object->flags_1044 &= ~4u;
            }
        }
    }
    return 1;
}

i32 __cdecl Action_SetCanTakeOver(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                  i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    if (param_5 != 0 && packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL) {
        object = packet->owner->objptr;
        for (index = 0; index < param_4; ++index) {
            if (NuStrICmp((char *)AITransferLiteral758430, params[index]) == 0) {
                object->flags_ef0 |= 0x40000000u;
            } else if (NuStrIStr(params[index], (char *)AIScriptFalse) != NULL) {
                object->flags_ef0 &= ~0x40000000u;
            }
        }
    }
    return 1;
}

i32 __cdecl Action_ImmuneToKillTerrain(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                       i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    GAMEOBJECT *object;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    if (first_time) {
        object->portal_flags |= 0x40000u;
        for (index = 0; index < param_count; ++index) {
            if (NuStrICmp(params[index], (char *)AIScriptFalse) == 0)
                object->portal_flags &= ~0x40000u;
        }
    }
    return 1;
}

i32 __cdecl Action_ImmuneToBolts(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                 i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    (void)sys;
    (void)processor;
    (void)param_6;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    if (param_5 != 0) {
        object->portal_flags |= 0x80000u;
        for (index = 0; index < param_4; ++index) {
            if (NuStrICmp(params[index], (char *)AIScriptFalse) == 0) {
                object->portal_flags &= (u32)(~0x80000u);
            }
        }
    }
    return 1;
}

i32 __cdecl Action_SetBoss(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                           i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    (void)sys;
    (void)processor;
    (void)param_6;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    if (param_5 != 0) {
        object->portal_flags |= 0x8000000u;
        for (index = 0; index < param_4; ++index) {
            if (NuStrICmp(params[index], (char *)AIScriptFalse) == 0) {
                object->portal_flags &= (u32)(~0x8000000u);
            }
        }
    }
    return 1;
}

i32 __cdecl Action_UpdateSockPos(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                 i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    if (param_5 != 0) {
        object->portal_flags |= 0x4000u;
        for (index = 0; index < param_4; ++index) {
            if (NuStrICmp(params[index], (char *)AIScriptFalse) == 0) {
                object->portal_flags &= ~0x4000u;
            }
        }
    }
    return 1;
}

i32 __cdecl Action_ApplyGravity(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    (void)sys;
    (void)processor;
    (void)param_6;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    if (param_5 != 0) {
        object->portal_flags &= (u32)(~0x80000000u);
        for (index = 0; index < param_4; ++index) {
            if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0) {
                object->portal_flags |= 0x80000000u;
            }
        }
    }
    return 1;
}

i32 __cdecl Action_CannotBeForcedBack(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                      i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    GAMEOBJECT *object;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    if (first_time) {
        object->flags_ef0 |= 0x10000000u;
        for (index = 0; index < param_count; ++index) {
            if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0)
                object->flags_ef0 &= ~0x10000000u;
        }
    }
    return 1;
}

i32 __cdecl Action_CanTurn(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                           i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    (void)sys;
    (void)processor;
    (void)param_6;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    if (param_5 != 0) {
        object->flags_ef0 |= 0x20000000u;
        for (index = 0; index < param_4; ++index) {
            if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0) {
                object->flags_ef0 &= (u32)(~0x20000000u);
            }
        }
    }
    return 1;
}

i32 __cdecl Action_PrefersPlayers(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                  i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    (void)sys;
    (void)processor;
    (void)param_5;
    (void)param_6;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    object->portal_flags |= 0x40000000u;
    if (param_4 != 0) {
        for (index = 0; index < param_4; ++index) {
            if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0) {
                object->portal_flags &= (u32)(~0x40000000u);
            }
        }
    }
    return 1;
}

i32 __cdecl Action_CanShootObstructions(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                        char **params, i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    GAMEOBJECT *object;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    object->portal_flags |= 0x10000000u;
    if (param_count != 0) {
        for (index = 0; index < param_count; ++index) {
            if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0)
                object->portal_flags &= ~0x10000000u;
        }
    }
    return 1;
}

i32 __cdecl Action_UseBigJumpToJump(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                    i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    (void)sys;
    (void)processor;
    (void)param_5;
    (void)param_6;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    object->flags_ef4 |= 0x40000000u;
    if (param_4 != 0) {
        for (index = 0; index < param_4; ++index) {
            if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0) {
                object->flags_ef4 &= ~0x40000000u;
            }
        }
    }
    return 1;
}

i32 __cdecl Action_IgnoreTurnAroundSpline(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                          char **params, i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    GAMEOBJECT *object;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    object->flags_ef4 |= 0x80000000u;
    if (param_count != 0) {
        for (index = 0; index < param_count; ++index) {
            if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0)
                object->flags_ef4 &= ~0x80000000u;
        }
    }
    return 1;
}

i32 __cdecl Action_CanPullLevers(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                 i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    object->flags_ef0 |= 0x800000u;
    if (param_4 != 0) {
        for (index = 0; index < param_4; ++index) {
            if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0) {
                object->flags_ef0 &= ~0x800000u;
            }
        }
    }
    return 1;
}

i32 __cdecl Action_AlwaysBackFlip(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                  i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    (void)sys;
    (void)processor;
    (void)param_5;
    (void)param_6;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    object->portal_flags |= 0x100u;
    if (param_4 != 0) {
        for (index = 0; index < param_4; ++index) {
            if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0) {
                object->portal_flags &= (u32)(~0x100u);
            }
        }
    }
    return 1;
}

i32 __cdecl Action_DeflectPlayersPart(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                      i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    GAMEOBJECT *object;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL || !first_time)
        return 1;
    object = packet->owner->objptr;
    object->flags_ef0 |= 0x100u;
    if (param_count != 0) {
        for (index = 0; index < param_count; ++index) {
            if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0)
                object->flags_ef0 &= ~0x100u;
        }
    }
    return 1;
}

i32 __cdecl Action_CanHelpWithTriggers(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                       i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    GAMEOBJECT *object;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    if (first_time) {
        object->flags_ef4 |= 0x40000u;
        for (index = 0; index < param_count; ++index) {
            if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0)
                object->flags_ef4 &= ~0x40000u;
        }
    }
    return 1;
}

i32 __cdecl Action_CanCollideWithObjects(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                         char **params, i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    GAMEOBJECT *object;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    if (first_time) {
        object->flags_ef4 &= ~0x80000u;
        for (index = 0; index < param_count; ++index) {
            if (NuStrICmp(params[index], (char *)AITransferLiteral757ebc) == 0)
                object->flags_ef4 |= 0x80000u;
        }
    }
    return 1;
}

i32 __cdecl Action_CanDefend(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                             i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    if (param_5 != 0) {
        object->portal_flags |= 2u;
        for (index = 0; index < param_4; ++index) {
            if (NuStrICmp(params[index], (char *)AIScriptFalse) == 0)
                object->portal_flags &= ~2u;
        }
    }
    return 1;
}

i32 __cdecl Action_DontAimAt(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                             i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    (void)sys;
    (void)processor;
    (void)param_6;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    if (param_5 != 0) {
        object->flags_ef4 |= 1;
        for (index = 0; index < param_4; ++index) {
            if (NuStrICmp(params[index], (char *)AIScriptFalse) == 0) {
                object->flags_ef4 &= (u32)(~1u);
            }
        }
    }
    return 1;
}

i32 __cdecl Action_CanUseWeapon(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    (void)sys;
    (void)processor;
    (void)param_6;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    if (param_5 != 0) {
        object->portal_flags |= 8;
        for (index = 0; index < param_4; ++index) {
            if (NuStrICmp(params[index], (char *)AIScriptFalse) == 0) {
                object->portal_flags &= (u32)(~8u);
            }
        }
    }
    return 1;
}

i32 __cdecl Action_IgnoreShoveSystem(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                     i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    GAMEOBJECT *object;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    if (first_time) {
        object->flags_ef0 |= 1;
        for (index = 0; index < param_count; ++index) {
            if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0)
                object->flags_ef0 &= ~1;
        }
    }
    return 1;
}

i32 __cdecl Action_CannotBeSeen(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    (void)sys;
    (void)processor;
    (void)param_6;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    if (param_5 != 0) {
        object->flags_ef0 |= 2;
        for (index = 0; index < param_4; ++index) {
            if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0) {
                object->flags_ef0 &= (u32)(~2u);
            }
        }
    }
    return 1;
}

i32 __cdecl Action_SetBoltsDontGetDeflectedBack(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                                char **params, i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    GAMEOBJECT *object;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    object->flags_ef0 |= 8u;
    if (param_count != 0) {
        for (index = 0; index < param_count; ++index) {
            if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0) {
                object->flags_ef0 &= ~8u;
            }
        }
    }
    return 1;
}

i32 __cdecl Action_CanMoveWhenDeactivated(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                          char **params, i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    GAMEOBJECT *object;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    object->flags_ef8_word |= 1;
    if (param_count != 0) {
        for (index = 0; index < param_count; ++index) {
            if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0)
                object->flags_ef8_word &= ~1;
        }
    }
    return 1;
}

i32 __cdecl Action_DontAttack(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                              i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    object->flags_ef8_word |= 2u;
    if (param_4 != 0) {
        for (index = 0; index < param_4; ++index) {
            if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0)
                object->flags_ef8_word &= ~2u;
        }
    }
    return 1;
}

i32 __cdecl Action_CanHitForceObjects(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                      i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    GAMEOBJECT *object;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    object->portal_flags |= 0x40;
    if (param_count != 0) {
        for (index = 0; index < param_count; ++index) {
            if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0)
                object->portal_flags &= ~0x40;
        }
    }
    return 1;
}

i32 __cdecl Action_PlayerSpeederHack(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                     i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    GAMEOBJECT *object;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    object->flags_ef8_word |= 8u;
    if (param_count != 0) {
        for (index = 0; index < param_count; ++index) {
            if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0) {
                object->flags_ef8_word &= (u32)(~8u);
            }
        }
    }
    return 1;
}

i32 __cdecl Action_UseWeapon(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                             i32 param_4, i32 param_5, f32 param_6) {
    GAMEOBJECT *object;
    (void)sys;
    (void)processor;
    object = packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL ? packet->owner->objptr : NULL;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    if (object != NULL) {
        object->portal_flags |= 0x20000000u;
    }
    return 1;
}

extern const char AITransferLiteral75782c[5];
extern const char AITransferLiteral7583e4[12];
extern const char AITransferLiteral7583f0[8];
extern const char AIActionFalseName[6];
extern const char AIScriptActionNameArgPrefix[6];
extern const char AreaConfigKeyword_character[10];
extern const char AITransferLiteral757a38[12];
extern const char AIGameCharacterName[11];
extern const char AIScriptFalse[6];
extern const char AITransferLiteral757a70[11];
extern const char GizmoDefaultOutputName[8];
typedef struct AITURRET_PARTIAL_PC {
    u8 unknown_00[0xe8];
    GAMEOBJECT *controller;
} AITURRET_PARTIAL_PC;
typedef struct AIOBSTACLE_PARTIAL_PC {
    u8 unknown_00[0x9c];
    u32 runtime_flags;
} AIOBSTACLE_PARTIAL_PC;
STATIC_ASSERT(offsetof(AITURRET_PARTIAL_PC, controller) == 0xe8);
STATIC_ASSERT(offsetof(AIOBSTACLE_PARTIAL_PC, runtime_flags) == 0x9c);
extern i32 turret_gizmotype_id;
extern i32 obstacle_gizmotype_id;
extern i32 blowup_gizmotype_id;
void __cdecl ReleaseTakeOver(GAMEOBJECT *, i32);
void __cdecl PartKill_ForceThrow(PART_PC *, i32);
i32 __cdecl Action_SetLevelPath(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    char *path_name;
    char *value;
    GAMEOBJECT *object;
    AIPACKET_PC *object_packet;
    (void)processor;
    (void)packet;
    (void)param_6;
    if (param_5 == 0 || sys->path_sys == NULL) {
        return 1;
    }

    path_name = NULL;
    for (index = 0; index < param_4; ++index) {
        value = NuStrIStr(params[index], (char *)((char *)AITransferLiteral75782c));
        if (value != NULL) {
            path_name = value + 5;
        }
    }

    if (AISysSetLevelPath(sys, path_name) == 0) {
        return 1;
    }

    for (index = 0; index < 8; ++index) {
        object = Player[index];
        if (object == NULL || (object->flags_1fc_word & (1u | 0x1000u)) != (1u | 0x1000u)) {
            continue;
        }
        object_packet = &object->ai_packet;
        AISysCharacterSetPath(object_packet, sys->path_sys->active_path);
        AISysGetCharacterPathPos(WORLD->ai_sys, (APIOBJECT_PC *)object, object_packet, 0xff, (i8)(object->field_281));
    }
    return 1;
}

i32 Action_SetHint(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params, i32 param_4,
                   i32 param_5, f32 param_6) {
    i32 hint_id = -1;
    i32 ignore_done = 0;
    i32 index;
    char *value;
    if (param_5 != 0) {
        for (index = 0; index < param_4; ++index) {
            value = NuStrIStr(params[index], (char *)AIActionIDName);
            if (value != NULL)
                hint_id = (i32)AIParamToFloat(processor, value + 3);
            else if (NuStrIStr(params[index], (char *)AIActionIgnoreDoneFlagNamePc) != NULL)
                ignore_done = 1;
        }
        Hint_SetHintFromId(hint_id, 0, ignore_done);
    }
    return 1;
}

i32 Action_SetHintComplete(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                           i32 param_4, i32 param_5, f32 param_6) {
    i32 hint_id = -1;
    i32 index;
    char *value;
    if (param_5 != 0) {
        for (index = 0; index < param_4; ++index) {
            value = NuStrIStr(params[index], (char *)AIActionIDName);
            if (value != NULL)
                hint_id = (i32)AIParamToFloat(processor, value + 3);
        }
        Hint_SetComplete(hint_id);
    }
    return 1;
}

i32 Action_CancelHint(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params, i32 param_4,
                      i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    Hint_CancelCurrent();
    return 1;
}

i32 __cdecl Action_LinkTurretToController(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                          char **params, i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    APIOBJECT_PC *lookup;
    AITURRET_PARTIAL_PC *turret;
    GAMEOBJECT *controller;
    char *value;
    GIZMO_PC *gizmo;

    if (first_time == 0) {
        return 1;
    }
    turret = NULL;
    controller = NULL;
    for (index = 0; index < param_count; ++index) {
        value = NuStrIStr(params[index], (char *)AITransferLiteral7583f0);
        if (value != NULL) {
            gizmo = (GIZMO_PC *)GizmoFindByName(WORLD->gizmo_sys, turret_gizmotype_id, value + 7);
            if (gizmo != NULL && gizmo->object != NULL) {
                turret = (AITURRET_PARTIAL_PC *)(gizmo->object);
            }
        } else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral7583e4)) != NULL) {
            {
                lookup = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(system, value + 11) : NULL;
                controller = lookup != NULL ? (GAMEOBJECT *)lookup->owner : NULL;
            }
        }
    }
    if (turret != NULL) {
        turret->controller = controller;
    }
    return 1;
}

i32 __cdecl Action_SetHoverPhase(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                 i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    (void)sys;
    (void)param_6;
    object = packet != NULL && packet->owner != NULL ? packet->owner->objptr : NULL;
    if (object == NULL || (object->character_data->game_character)->hover_fall_mode != 2) {
        return 1;
    }

    if (param_5 != 0) {
        processor->action_data_1 = 1;
        for (index = 0; index < param_4; ++index) {
            if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0) {
                processor->action_data_1 = 0;
            }
        }
    }

    if (processor->action_data_1 != 0) {
        if (object->jump_variant_state == 1) {
            return 1;
        }
    } else if (object->jump_variant_state != 1) {
        return 1;
    }
    object->pad->buttons_pressed |= GAMEPAD_JUMP;
    return 0;
}

void __cdecl FindForcePushTarget(GAMEOBJECT *object, i32 activate, i32 target_filter);
STATIC_ASSERT(offsetof(GAMEOBJECT, force_target) == 0x790);
STATIC_ASSERT(offsetof(GAMEOBJECT, pad) == 0xca4);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_d20) == 0xd20);
STATIC_ASSERT(offsetof(GAMEOBJECT, character_context) == 0x7b5);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_e20) == 0xe20);
STATIC_ASSERT(offsetof(GAMEPAD_PC, runtime_flags) == 0x5a);
STATIC_ASSERT(offsetof(AISCRIPTPROCESS_PC, action_data_1) == 0x68);
STATIC_ASSERT(offsetof(AISCRIPTPROCESS_PC, action_data_3) == 0x6c);
STATIC_ASSERT(offsetof(AISCRIPTPROCESS_PC, action_data_4) == 0x70);
STATIC_ASSERT(offsetof(AISCRIPTPROCESS_PC, action_timer) == 0x9c);

i32 __cdecl Action_ForcePush(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                             i32 param_count, i32 first_time, f32 elapsed) {
    GAMEOBJECT *object;
    GAMEOBJECT *action_object;
    i32 index;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL) {
        return 1;
    }
    object = packet->owner->objptr;
    if (first_time != 0) {
        processor->action_data_3 = NULL;
        for (index = 0; index < param_count; ++index) {
            if (NuStrICmp(params[index], "target=opponent") == 0) {
                processor->action_data_3 = packet->opponent;
                if (processor->action_data_3 == NULL) {
                    return 1;
                }
            } else {
                char *value = NuStrIStr(params[index], "target=");
                if (value != NULL) {
                    processor->action_data_3 = GetNamedGameObject(system, value + 7);
                    if (processor->action_data_3 == NULL) {
                        return 1;
                    }
                } else if (NuStrICmp(params[index], "forcepush_highlight_only") == 0) {
                    processor->action_data_1 = 1;
                }
            }
        }
        if (object->character_context == 0x1b && processor->action_data_1 == 0 && object->force_target != NULL &&
            processor->action_data_3 == NULL) {
            object->pad->runtime_flags |= 0x10;
            object->field_d20 = (u32)object->force_target;
            object->flags_e20 = (object->flags_e20 & ~0x400u) | ((processor->action_data_1 << 10) & 0x400u);
            object->flags_e20 &= ~0x20000u;
            object->character_context = -1;
            FindForcePushTarget(object, 0, 0);
        }
    }
    if (CharCategory_IsCategory(object, 0) == 0) {
        if (FreePlay != 0) {
            f32 timer = processor->action_timer - elapsed;
            processor->action_timer = timer;
            if (timer < 0.0f) {
                processor->action_timer = 0.5f;
                object->pad->buttons_pressed |= GAMEPAD_TOGGLERIGHT;
            }
        }
        return 0;
    }
    object->pad->runtime_flags |= 0x10;
    object->field_d20 = (u32)processor->action_data_3;
    object->flags_e20 = (object->flags_e20 & ~0x400u) | ((processor->action_data_1 << 10) & 0x400u);
    memcpy(&action_object, &processor->action_data_4, sizeof(action_object));
    if (action_object != NULL) {
        return action_object != object->force_target;
    }
    if (object->character_context == 0x1b && object->force_target != NULL) {
        memcpy(&processor->action_data_4, &object->force_target, sizeof(object->force_target));
    }
    return 0;
}

i32 __cdecl Action_ForceLightning(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                  i32 param_4, i32 param_5, f32 param_6) {
    struct gameobject_s *object;
    NUVEC target;
    NUVEC direction;
    NUVEC origins[2];
    i32 origin_index;
    (void)param_6;
    object = NULL;
    if (packet != NULL && packet->owner != NULL) {
        object = packet->owner->objptr;
    }

    if (param_5 != 0) {
        if (param_4 > 0) {
            i32 index;
            for (index = 0; index < param_4; ++index) {
                char *value = NuStrIStr(params[index], "locator=");
                if (value != NULL) {
                    AILOCATOR_PC *locator = AIPathFindLocator(sys, value + 8);
                    if (locator != NULL) {
                        processor->action_pos = locator->position;
                        processor->action_data_1 = 1;
                    }
                } else {
                    value = NuStrIStr(params[index], "randxz=");
                    if (value != NULL) {
                        processor->action_data_4 = AIParamToFloat(processor, value + 7);
                    }
                }
            }
        }
    }

    if (object == NULL || processor->action_data_1 == 0) {
        return 0;
    }

    ForceLightning_Origin(object, &origins[0], &origins[1]);

    target = processor->action_pos;
    if (processor->action_data_4 > 0.0f) {
        f64 range = processor->action_data_4;
        target.x += range - NuRandFloat() * (range + range);
        range = processor->action_data_4;
        target.z += range - NuRandFloat() * (range + range);
    }

    for (origin_index = 0; origin_index < 2; ++origin_index) {
        NUVEC *origin = &origins[origin_index];
        if (origin->y != 1000000000.0) {
            f32 distance = NuVecDist(&target, origin, &direction);
            NuLgtLaser(lightning_type, lightning_sizew[0], lightning_sizel[0], lightning_sizewab[0], origin, &direction,
                       lightning_col[0], lightning_endw[0], distance);
            PlaySfx("ForceLightningLp", &processor->action_pos);
        }
    }
    packet->movement_look_target = &processor->action_pos;
    return 0;
}

i32 __cdecl Action_AlwaysTriggerObstacle(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                         char **params, i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    AIOBSTACLE_PARTIAL_PC *obstacle;
    i32 enabled;
    char *value;
    GIZMO_PC *gizmo;
    if (first_time == 0 || param_count == 0) {
        return 1;
    }
    obstacle = NULL;
    enabled = 1;
    for (index = 0; index < param_count; ++index) {
        value = NuStrIStr(params[index], (char *)AIScriptActionNameArgPrefix);
        if (value != NULL) {
            gizmo = (GIZMO_PC *)GizmoFindByName(WORLD->gizmo_sys, obstacle_gizmotype_id, value + 5);
            if (gizmo != NULL) {
                obstacle = (AIOBSTACLE_PARTIAL_PC *)(gizmo->object);
            }
        } else if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0) {
            enabled = 0;
        }
    }
    if (obstacle != NULL) {
        obstacle->runtime_flags = (u32)((obstacle->runtime_flags & ~4u) | ((enabled & 1) << 2));
    }
    return 1;
}

i32 __cdecl Action_CanTriggerObstacle(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                      i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    AIOBSTACLE_PARTIAL_PC *obstacle;
    i32 blocked;
    char *value;
    GIZMO_PC *gizmo;
    if (first_time == 0 || param_count == 0) {
        return 1;
    }
    obstacle = NULL;
    blocked = 0;
    for (index = 0; index < param_count; ++index) {
        value = NuStrIStr(params[index], (char *)AIScriptActionNameArgPrefix);
        if (value != NULL) {
            gizmo = (GIZMO_PC *)GizmoFindByName(WORLD->gizmo_sys, obstacle_gizmotype_id, value + 5);
            if (gizmo != NULL) {
                obstacle = (AIOBSTACLE_PARTIAL_PC *)(gizmo->object);
            }
        } else if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0) {
            blocked = 1;
        }
    }
    if (obstacle != NULL) {
        obstacle->runtime_flags = (u32)((obstacle->runtime_flags & ~8u) | ((blocked & 1) << 3));
    }
    return 1;
}

i32 __cdecl Action_SetLastSafePathPos(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                      char **params, i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    APIOBJECT_PC *object;
    char *value;
    if (first_time == 0) {
        return 1;
    }
    object = packet != NULL && packet->owner != NULL ? (APIOBJECT_PC *)packet->owner : NULL;
    for (index = 0; index < param_count; ++index) {
        value = NuStrIStr(params[index], (char *)AreaConfigKeyword_character);
        if (value != NULL) {
            if (GetNamedGameObjectCallback != NULL)
                object = GetNamedGameObjectCallback(system, value + 10);
        } else {
            NuStrICmp((char *)AIActionFalseName, params[index]);
        }
    }
    if (object != NULL) {
        object->respawn_position = object->position;
        object->last_safe_position = object->position;
    }
    return 1;
}

i32 __cdecl Action_PressSpecialButton(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                      i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    GAMEOBJECT *object;
    i32 result;
    if (packet == NULL || packet->owner == NULL)
        return 1;
    object = packet->owner->objptr;
    if (object == NULL)
        return 1;
    if (first_time) {
        for (index = 0; index < param_count; ++index) {
            if (NuStrICmp(params[index], (char *)AITransferLiteral757a38) == 0)
                processor->action_data_1 = 1;
        }
    }
    object->pad->buttons_pressed |= GAMEPAD_SPECIAL;
    result = 1;
    if (processor->action_data_1 != 0) {
        object->pad->buttons_held |= GAMEPAD_SPECIAL;
        result = 0;
    }
    return result;
}

i32 __cdecl Action_ReleaseTakeOver(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                   i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    APIOBJECT_PC *lookup;
    GAMEOBJECT *object;
    char *value;

    if (param_5 != 0) {
        object = NULL;
        if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
            object = packet->owner->objptr;
        for (index = 0; index < param_4; ++index) {
            value = NuStrIStr(params[index], (char *)AIGameCharacterName);
            if (value != NULL) {
                {
                    lookup = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(sys, value + 10) : NULL;
                    object = lookup != NULL ? (GAMEOBJECT *)lookup->owner : NULL;
                }
            }
        }
        if (object != NULL && object->takeover_source != NULL) {
            ReleaseTakeOver(object, 0);
        }
    }
    return 1;
}

i32 __cdecl Action_TriggerBlowUp(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                 i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GIZMOBLOWUP_PC *blowup;
    char *value;
    GIZMO_PC *gizmo;
    if (param_5 != 0 && param_4 > 0) {
        blowup = NULL;
        for (index = 0; index < param_4; ++index) {
            value = NuStrIStr(params[index], (char *)AIScriptActionNameArgPrefix);
            if (value != NULL) {
                gizmo = (GIZMO_PC *)GizmoFindByName(WORLD->gizmo_sys, blowup_gizmotype_id, value + 5);
                if (gizmo != NULL) {
                    blowup = (GIZMOBLOWUP_PC *)(gizmo->object);
                }
            }
        }
        if (blowup != NULL && (blowup->status_flags & 1) == 0) {
            GizmoBlowupBlowup(blowup, 1, -1, 1, NULL, 1);
        }
    }
    return 1;
}

i32 __cdecl Action_DontRaycastLOS(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                  i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    (void)sys;
    (void)processor;
    (void)packet;
    (void)param_6;
    if (param_5 != 0 && WORLD->object_sys != NULL) {

        WORLD->object_sys->flags_word |= 1u;
        for (index = 0; index < param_4; ++index) {
            if (NuStrICmp(params[index], (char *)AIScriptFalse) == 0) {
                WORLD->object_sys->flags_word &= ~1u;
            }
        }
    }
    return 1;
}

i32 __cdecl Action_SetLocatorSet(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                 i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    char *value;
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    if (param_5 != 0) {
        for (index = 0; index < param_4; ++index) {
            value = NuStrIStr(params[index], (char *)AIScriptActionNameArgPrefix);
            if (value != NULL) {
                processor->locator_set = AIPathFindLocatorSet(WORLD->ai_sys, value + 5);
            }
        }
    }
    return 1;
}

i32 __cdecl Action_SetControlSystem(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                    i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    (void)sys;
    (void)processor;
    (void)param_6;
    if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL && param_5 != 0) {
        object = packet->owner->objptr;
        object->ai_packet.movement_stopped = 0;
        for (index = 0; index < param_4; ++index) {
            if (NuStrICmp(params[index], (char *)AITransferLiteral757a70) == 0) {
                object->ai_packet.movement_stopped = 1;
            }
        }
    }
    return 1;
}

i32 __cdecl Action_SetWalkSpeed(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                i32 num_params, i32 first_time, f32 elapsed) {
    GAMEOBJECT *object;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    if (first_time) {
        object->walk_speed_override = 1000000000.0f;
        if (num_params != 0 && NuStrICmp(params[0], (char *)GizmoDefaultOutputName) != 0)
            object->walk_speed_override = AIParamToFloat(processor, params[0]);
    }
    return 1;
}

i32 __cdecl Action_PartyCanBeUnderCover(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                        char **params, i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    if (first_time) {
        party_cant_be_under_cover = 0;
        for (index = 0; index < param_count; ++index) {
            if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0)
                party_cant_be_under_cover = 1;
        }
    }
    return 1;
}

i32 __cdecl Action_DisableNarrowSocks(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                      i32 param_count, i32 first_time, f32 elapsed) {
    i32 disabled;
    disabled = 1;
    if (param_count != 0 && params != NULL && params[0] != NULL && NuStrICmp((char *)AIActionFalseName, params[0]) == 0)
        disabled = 0;
    disable_narrow_socks = disabled;
    return 1;
}

i32 __cdecl Action_PressActionButton(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                     i32 param_count, i32 first_time, f32 elapsed) {
    GAMEOBJECT *object;
    if (packet != NULL && packet->owner != NULL) {
        object = packet->owner->objptr;
        if (object != NULL) {
            object->pad->buttons_pressed |= GAMEPAD_ACTION;
            object->portal_flags |= 0x400u;
            object->portal_flags |= 0x20;
        }
    }
    return 1;
}

i32 __cdecl Action_ClearTakeOverTarget(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                       i32 param_count, i32 first_time, f32 elapsed) {
    GAMEOBJECT *object;
    GAMEOBJECT *target;
    if (first_time) {
        object = packet->owner->objptr;
        if (object != NULL) {
            target = object->takeover_target;
            if (target != NULL) {
                object->takeover_target = NULL;
                target->takeover_target = NULL;
            }
        }
    }
    return 1;
}

i32 __cdecl Action_PressTagButton(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                  i32 param_4, i32 param_5, f32 param_6) {
    GAMEOBJECT *object;
    object = packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL ? packet->owner->objptr : NULL;
    if (object != NULL) {
        object->pad->buttons_pressed |= GAMEPAD_TAG;
    }
    return 1;
}

i32 __cdecl Action_PressJumpButton(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                   i32 param_4, i32 param_5, f32 param_6) {
    GAMEOBJECT *object;
    (void)sys;
    (void)processor;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    object = packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL ? packet->owner->objptr : NULL;
    if (object != NULL) {
        object->pad->buttons_pressed |= GAMEPAD_JUMP;
    }
    return 1;
}

i32 __cdecl Action_Explode(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                           i32 param_4, i32 param_5, f32 param_6) {

    (void)sys;
    (void)processor;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    if (packet != NULL && packet->owner != NULL) {
        Detonate(&packet->owner->position, 0);
    }
    return 1;
}

i32 __cdecl Action_BreakFormation(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                  i32 param_4, i32 param_5, f32 param_6) {

    if (packet != NULL && packet->group != NULL) {
        packet->group->flags &= ~8u;
    }
    return 1;
}

i32 __cdecl Action_FormationMove(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                 i32 param_4, i32 param_5, f32 param_6) {

    (void)sys;
    (void)processor;
    if (packet != NULL && packet->group != NULL) {
        packet->group->flags |= 8u;
    }
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 1;
}

extern const char AIGameCharacterName[11];
extern const char AIGameSetName[5];
extern const char AILocatorIndexedFormat[6];
extern const char AIGameNearestName[8];
extern const char AIScriptActionNameArgPrefix[6];
extern const char AIActionPersonalName[9];
extern const char AITransferLiteral7577dc[8];
extern const char AIActionIndexedName[8];
extern const char AILocatorPersonalFormat[6];
extern const char ActionSnapDontCheckTerrainName[19];
extern const char AITransferLiteral75782c[5];
extern const char AITransferLiteral757824[7];
extern const char AreaConfigKeyword_character[10];
extern const char AITransferLiteral75781c[8];
extern const char AITransferLiteral757834[13];
extern const char AITransferLiteral757884[12];
extern const char AITransferLiteral757a68[6];
extern const char AITransferLiteral757c10[13];
extern const char AITransferLiteral757c20[12];
extern const char GizmoDefaultOutputName[8];
extern const char AIActionFalseName[6];
extern const char AIPathLocatorName[9];
extern const char AITransferLiteral758194[11];
extern const char AITransferLiteral758188[11];
extern const char AITransferLiteral7581a0[8];
extern const char AITransferLiteral758174[19];
extern const char AITransferLiteral7581d4[10];
extern const char AITransferLiteral7581ec[9];
extern const char AITransferLiteral7581e0[9];
extern const char AITransferLiteral757cd0[6];
i32 __cdecl Action_DeActivate(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                              i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    APIOBJECT_PC *lookup;
    GAMEOBJECT *object;
    i32 creature_set;
    char *value;
    i32 parsed_set;
    GAMEOBJECT *candidate;

    (void)processor;
    (void)param_6;
    if (param_5 == 0) {
        return 1;
    }

    object = NULL;
    if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
        object = packet->owner->objptr;
    creature_set = 0;
    for (index = 0; index < param_4; ++index) {
        value = NuStrIStr(params[index], (char *)AIGameCharacterName);
        if (value != NULL) {
            {
                lookup = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(sys, value + 10) : NULL;
                object = lookup != NULL ? (GAMEOBJECT *)lookup->owner : NULL;
            }
            continue;
        }
        value = NuStrIStr(params[index], (char *)AIGameSetName);
        if (value != NULL) {
            parsed_set = (i32)(AIParamToFloat(processor, value + 4));
            creature_set = (u32)(parsed_set) < 17 ? parsed_set : 0;
        }
    }

    if (creature_set != 0) {
        candidate = Obj;
        for (index = 0; index < HIGHGAMEOBJECT; ++index, ++candidate) {
            if ((candidate->flags_1fc_word & (1u | 0x1000u)) == (1u | 0x1000u) &&
                candidate->ai_packet.creature_set == creature_set) {
                DeactivateGameObject(candidate);
            }
        }
    } else if (object != NULL) {
        DeactivateGameObject(object);
    }
    return 1;
}

i32 __cdecl Action_GameSetLocator(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                  i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    APIOBJECT_PC *target;
    char *locator_name;
    i32 personal;
    i32 indexed;
    i32 nearest;
    i32 random_count;
    char *value;
    NUVEC difference;
    AILOCATOR_PC *locator;
    f32 best_distance;
    char numbered_name[64];
    AILOCATOR_PC *candidate;
    f32 distance;
    GAMEOBJECT *object;
    (void)param_6;
    best_distance = 1.0e9f;
    target = NULL;
    locator_name = NULL;
    personal = 0;
    indexed = 0;
    nearest = 0;
    random_count = 0;
    locator = NULL;
    if (param_5 == 0) {
        return 1;
    }
    if (packet != NULL && packet->owner != NULL) {
        target = (APIOBJECT_PC *)packet->owner;
    }
    if (param_4 == 0) {
        return 1;
    }
    for (index = 0; index < param_4; ++index) {
        value = NuStrIStr(params[index], (char *)AIScriptActionNameArgPrefix);
        if (value != NULL) {
            locator_name = value + 5;
        } else if (NuStrICmp(params[index], (char *)AIActionPersonalName) == 0) {
            personal = 1;
        } else if (NuStrICmp(params[index], (char *)AIActionIndexedName) == 0) {
            indexed = 1;
        } else if (NuStrICmp(params[index], (char *)AIGameNearestName) == 0) {
            nearest = 1;
        } else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral7577dc)) != NULL) {
            random_count = (i32)(AIParamToFloat(processor, value + 7));
        } else if ((value = NuStrIStr(params[index], (char *)AIGameCharacterName)) != NULL &&
                   GetNamedGameObjectCallback != NULL) {
            target = GetNamedGameObjectCallback(sys, value + 10);
        }
    }

    if (locator_name != NULL && nearest) {
        index = 0;
        do {
            sprintf(numbered_name, (char *)AILocatorIndexedFormat, locator_name, index++);
            candidate = AIPathFindLocator(sys, numbered_name);
            if (candidate != NULL) {
                distance = NuVecDistSqr(&packet->terrain_origin, &candidate->position, &difference);
                if (distance < best_distance) {
                    best_distance = distance;
                    locator = candidate;
                }
            }
        } while (candidate != NULL);
    } else if (locator_name != NULL) {

        object = packet->owner->objptr;
        if (indexed && ((APIOBJECT_PC *)packet->owner)->locator_name_index != -1) {
            sprintf(numbered_name, (char *)AILocatorIndexedFormat, locator_name,
                    ((APIOBJECT_PC *)packet->owner)->locator_name_index);
        } else if (personal && ((APIOBJECT_PC *)packet->owner)->character_data != NULL) {
            sprintf(numbered_name, (char *)AILocatorPersonalFormat, locator_name,
                    ((APIOBJECT_PC *)packet->owner)->character_data->name);
        } else if (random_count != 0) {
            sprintf(numbered_name, (char *)AILocatorIndexedFormat, locator_name, NuRand(0) % random_count);
        } else {
            sprintf(numbered_name, locator_name);
        }
        locator = AIPathFindLocator(sys, numbered_name);
    }
    if (target != NULL) {
        target->ai->locator = locator;
    }
    return 1;
}

i32 __cdecl Action_SnapToLocator(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                 i32 param_count, i32 first_time, f32 elapsed) {
    i32 i;
    APIOBJECT_PC *lookup;
    GAMEOBJECT *object;
    char *name;
    i32 personal;
    i32 indexed;
    i32 random_count;
    i32 check_terrain;
    char *value;
    i32 player;
    AILOCATOR_PC *locator;
    char locator_name[64];

    if (first_time != 0) {
        object = packet != NULL && packet->owner != NULL ? packet->owner->objptr : NULL;
        name = NULL;
        personal = 0;
        indexed = 0;
        random_count = 0;
        check_terrain = 1;
        for (i = 0; i < param_count; ++i) {

            if ((value = NuStrIStr(params[i], (char *)AITransferLiteral75782c)) != NULL)
                name = value + 5;
            else if (NuStrIStr(params[i], (char *)AIActionPersonalName) != NULL)
                personal = 1;
            else if (NuStrIStr(params[i], (char *)AIActionIndexedName) != NULL)
                indexed = 1;
            else if ((value = NuStrIStr(params[i], (char *)AITransferLiteral757824)) != NULL)
                random_count = (i32)AIParamToFloat(processor, value + 9);
            else if ((value = NuStrIStr(params[i], (char *)AreaConfigKeyword_character)) != NULL) {
                lookup = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(sys, value + 10) : NULL;
                object = lookup != NULL ? (GAMEOBJECT *)lookup->owner : NULL;
            } else if ((value = NuStrIStr(params[i], (char *)AITransferLiteral75781c)) != NULL) {
                player = (i32)AIParamToFloat(processor, value + 7) - 1;
                if ((u32)player <= 1)
                    object = Player[player];
            } else if (NuStrIStr(params[i], (char *)ActionSnapDontCheckTerrainName) != NULL)
                check_terrain = 0;
        }
        if (object == NULL && packet != NULL && packet->owner != NULL)
            object = packet->owner->objptr;
        if (object != NULL) {
            locator = object->ai_packet.locator;
            if (name != NULL) {

                if (indexed && (i8)object->field_280 != -1)
                    sprintf(locator_name, (char *)AILocatorIndexedFormat, name, (i8)object->field_280);
                else if (personal && object->character_data != NULL)
                    sprintf(locator_name, (char *)AILocatorPersonalFormat, name, object->character_data->name);
                else if (random_count != 0)
                    sprintf(locator_name, (char *)AILocatorIndexedFormat, name, NuRand(NULL) % random_count);
                else
                    sprintf(locator_name, name);
                locator = AIPathFindLocator(sys, locator_name);
            }
            if (locator != NULL) {
                object->position = locator->position;
                object->field_0x276 = locator->rotation;
                object->facing_angle = locator->rotation;
                object->movement_facing_angle = locator->rotation;
                object->ai_packet.path_info = locator->path_info;
                object->collision_position = object->position;
                object->initial_position = object->position;
                plr_lastpos = object->collision_position;
                object->start_position = object->collision_position;
                object->respawn_position = object->position;
                object->last_safe_position = object->position;
                object->ai_update_position = object->position;
                object->velocity = v000;
                InitSurfaceInfo(object);
                if (check_terrain != 0)
                    SetObjOnSurface(object, 0);
            }
        }
    }
    return 1;
}

i32 __cdecl Action_BigJumpToLocator(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                    i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    i32 personal;
    i32 indexed;
    i32 random_count;
    char *name;
    f32 jump_factor;
    char *value;
    char locator_name[64];
    AILOCATOR_PC *locator;
    (void)param_6;

    jump_factor = 1.0f;
    personal = 0;
    indexed = 0;
    random_count = 0;
    name = NULL;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    if (param_5 != 0) {
        processor->action_data_3 = processor->locator;

        for (index = 0; index < param_4; ++index) {
            value = NuStrIStr(params[index], (char *)((char *)AITransferLiteral75782c));
            if (value != NULL) {
                name = value + 5;
            } else if (NuStrIStr(params[index], (char *)((char *)AIActionPersonalName)) != NULL) {
                personal = 1;
            } else if (NuStrIStr(params[index], (char *)((char *)AIActionIndexedName)) != NULL) {
                indexed = 1;
            } else if ((value = NuStrIStr(params[index], (char *)((char *)AITransferLiteral757824))) != NULL) {
                random_count = (i32)(AIParamToFloat(processor, value + 9));
            } else if ((value = NuStrIStr(params[index], (char *)((char *)AITransferLiteral757834))) != NULL) {
                jump_factor = AIParamToFloat(processor, value + 12);
                if (jump_factor < 0.0f) {
                    jump_factor = 0.0f;
                }
            }
        }

        if (name != NULL) {

            if (indexed != 0 && (i8)(object->field_280) != -1) {
                sprintf(locator_name, (char *)AILocatorIndexedFormat, name, (i8)(object->field_280));
            } else if (personal != 0 && object->character_data != NULL) {
                sprintf(locator_name, (char *)AILocatorPersonalFormat, name, object->character_data->name);
            } else if (random_count != 0) {
                sprintf(locator_name, (char *)AILocatorIndexedFormat, name, NuRand(NULL) % random_count);
            } else {
                sprintf(locator_name, name);
            }
            processor->action_data_3 = AIPathFindLocator(sys, locator_name);
        }

        locator = (AILOCATOR_PC *)(processor->action_data_3);
        if (locator != NULL) {
            StartBigJump(object, &locator->position, 0, jump_factor, 1.0f, 0);
            object->ai_packet.path_info = locator->path_info;
            object->ai_packet.last_path_position = locator->position;
        }
    }
    return object->character_context != 0x1f;
}

i32 __cdecl Action_SetDoomedEscapeLocator(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                          char **params, i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    APIOBJECT_PC *lookup;
    GAMEOBJECT *object;
    char *name;
    i32 personal;
    i32 indexed;
    i32 random_count;
    i32 take_damage;
    char *value;
    char locator_name[64];

    if (first_time) {
        object = NULL;
        if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
            object = packet->owner->objptr;
        if (param_count != 0) {
            name = NULL;
            personal = 0;
            indexed = 0;
            random_count = 0;
            take_damage = 0;
            for (index = 0; index < param_count; ++index) {
                value = NuStrIStr(params[index], (char *)AIGameCharacterName);
                if (value != NULL) {
                    lookup = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(system, value + 10) : NULL;
                    object = lookup != NULL ? (GAMEOBJECT *)lookup->owner : NULL;
                } else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral75782c)) != NULL)
                    name = value + 5;
                else if (NuStrIStr(params[index], (char *)AIActionPersonalName) != NULL)
                    personal = 1;
                else if (NuStrIStr(params[index], (char *)AIActionIndexedName) != NULL)
                    indexed = 1;
                else if (NuStrIStr(params[index], (char *)AITransferLiteral757884) != NULL)
                    take_damage = 1;
                else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral757824)) != NULL)
                    random_count = (i32)(AIParamToFloat(processor, value + 9));
            }
            if (object != NULL) {
                object->flags_ef0 &= ~0x800u;
                object->doomed_escape_locator = NULL;
                if (name != NULL) {

                    if (indexed && (i8)(object->field_280) != -1)
                        sprintf(locator_name, (char *)AILocatorIndexedFormat, name, (i8)(object->field_280));
                    else if (personal && object->character_data != NULL)
                        sprintf(locator_name, (char *)AILocatorPersonalFormat, name, object->character_data->name);
                    else if (random_count != 0)
                        sprintf(locator_name, (char *)AILocatorIndexedFormat, name, NuRand(NULL) % random_count);
                    else
                        sprintf(locator_name, name);
                    object->doomed_escape_locator = AIPathFindLocator(system, locator_name);
                    if (object->doomed_escape_locator != NULL)
                        object->flags_ef0 = (object->flags_ef0 & ~0x800u) | (take_damage << 11);
                }
            }
        }
    }
    return 1;
}

i32 __cdecl Action_AddToSet(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                            i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    i32 creature_set;
    (void)sys;
    (void)param_6;
    object = packet != NULL && packet->owner != NULL ? packet->owner->objptr : NULL;
    if (param_5 != 0 && object != NULL && param_4 > 0) {
        for (index = 0; index < param_4; ++index) {
            if (NuStrICmp(params[index], (char *)AITransferLiteral757a68) == 0) {
                object->ai_packet.creature_set = 0;
                continue;
            }
            creature_set = (i32)(AIParamToFloat(processor, params[index]));
            if ((u32)(creature_set - 1) < 16) {
                object->ai_packet.creature_set = (u8)(creature_set);
                ++aicreature_sets_alive[creature_set - 1];
            }
        }
    }
    return 1;
}

i32 __cdecl Action_SetHitPoints(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                i32 num_params, i32 first_time, f32 elapsed) {
    i32 i;
    APIOBJECT_PC *lookup;
    GAMEOBJECT *object;
    i32 hitpoints;
    i32 set_max;
    char *value;
    GIZAIMESSAGE *message;

    if (!first_time)
        return 1;
    object = NULL;
    if (packet != NULL && packet->owner != NULL)
        object = packet->owner->objptr;
    hitpoints = -1;
    set_max = 1;
    for (i = 0; i < num_params; i++) {
        value = NuStrIStr(params[i], (char *)AIGameCharacterName);
        if (value != NULL) {
            {
                lookup = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(sys, value + 10) : NULL;
                object = lookup != NULL ? (GAMEOBJECT *)lookup->owner : NULL;
            }
        } else if ((value = NuStrIStr(params[i], (char *)AITransferLiteral757c20)) != NULL) {
            message = CheckGizAIMessage(gizaimessagesys, value + 11, NULL);
            if (message != NULL)
                hitpoints = (i32)message->value;
        } else if (NuStrICmp(params[i], (char *)AITransferLiteral757c10) == 0) {
            set_max = 0;
        } else if (NuStrICmp(params[i], (char *)GizmoDefaultOutputName) != 0) {
            hitpoints = (i32)AIParamToFloat(processor, params[i]);
        }
    }
    if (object != NULL) {
        if (hitpoints == -1)
            hitpoints = ((PLAYERCHARACTERCONFIG_PC *)object->character_data->game_character)->hitpoints;
        if (set_max)
            object->field_107e = (u8)hitpoints;
        object->current_hp = (i8)hitpoints;
    }
    return 1;
}

i32 __cdecl Action_SetShieldHitPoints(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                      char **params, i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    APIOBJECT_PC *lookup;
    GAMEOBJECT *object;
    i32 hit_points;
    char *value;

    if (first_time != 0) {
        object = NULL;
        if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
            object = packet->owner->objptr;
        hit_points = -1;
        for (index = 0; index < param_count; ++index) {
            value = NuStrIStr(params[index], (char *)AIGameCharacterName);
            if (value != NULL) {
                lookup = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(system, value + 10) : NULL;
                object = lookup != NULL ? (GAMEOBJECT *)lookup->owner : NULL;
            } else
                hit_points = (i32)(AIParamToFloat(processor, params[index]));
        }
        if (object != NULL) {
            if (hit_points == -1)
                hit_points = object->character_data->game_character->shield_hitpoints;
            object->shield_hitpoints = (u8)(hit_points);
        }
    }
    return 1;
}

i32 __cdecl Action_SetScriptParam(AISYS_PC *unused0, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *unused2, char **params,
                                  i32 count, i32 first_time, f32 unused6) {
    f32 amount = 0.0f;
    i32 operation = 0;
    i32 param_index = -1;
    i32 index;
    i32 candidate;
    char *value;
    if (first_time != 0 && count != 0 && processor->script != NULL) {
        for (index = 0; index < count; ++index) {
            value = NuStrIStr(params[index], (char *)AIScriptActionNameArgPrefix);
            if (value != NULL) {
                value += 5;
                for (candidate = 0; candidate < 4; ++candidate) {
                    if (NuStrICmp(processor->script->params[candidate].name, value) == 0) {
                        param_index = candidate;
                        break;
                    }
                }
            } else if ((value = NuStrIStr(params[index], "ix=")) != NULL) {
                param_index = (i32)AIParamToFloat(processor, value + 3);
                if (param_index >= 4)
                    param_index = -1;
            } else if ((value = NuStrIStr(params[index], (char *)AIAnimSpeedValueName)) != NULL)
                amount = AIParamToFloat(processor, value + 6);
            else if ((value = NuStrIStr(params[index], "increment=")) != NULL) {
                amount = AIParamToFloat(processor, value + 10);
                operation = 1;
            } else if ((value = NuStrIStr(params[index], "decrement=")) != NULL) {
                amount = AIParamToFloat(processor, value + 10);
                operation = -1;
            }
        }
        if (param_index >= 0) {
            switch (operation) {
                case -1:
                    processor->params[param_index] -= amount;
                    break;
                case 0:
                    processor->params[param_index] = amount;
                    break;
                case 1:
                    processor->params[param_index] = processor->params[param_index] + amount;
                    break;
            }
        }
    }
    return 1;
}

i32 __cdecl Action_CopyMessage(AISYS_PC *unused0, AISCRIPTPROCESS_PC *unused1, AIPACKET_PC *unused2, char **params,
                               i32 count, i32 first_time, f32 unused6) {
    char *from = NULL;
    char *to = NULL;
    i32 index;
    char *value;
    GIZAIMESSAGE *source;
    GIZAIMESSAGE *destination;
    if (first_time != 0 && gizaimessagesys != NULL && count > 0) {
        for (index = 0; index < count; ++index) {
            value = NuStrIStr(params[index], "from=");
            if (value != NULL)
                from = value + 5;
            else if ((value = NuStrIStr(params[index], "to=")) != NULL)
                to = value + 3;
        }
        if (from != NULL && to != NULL) {
            source = CheckGizAIMessage(gizaimessagesys, from, NULL);
            destination = CheckGizAIMessage(gizaimessagesys, to, NULL);
            if (source != NULL && destination != NULL)
                destination->value = source->value;
        }
    }
    return 1;
}

i32 __cdecl Action_SetMessage(AISYS_PC *unused0, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *unused2, char **params,
                              i32 count, i32 first_time, f32 unused6) {
    char *name = NULL;
    i32 operation = 0;
    f32 amount = 0.0f;
    i32 index;
    char *value;
    GIZAIMESSAGE *message;
    if (first_time != 0 && gizaimessagesys != NULL && count > 0) {
        for (index = 0; index < count; ++index) {
            value = NuStrIStr(params[index], (char *)AIScriptActionNameArgPrefix);
            if (value != NULL)
                name = value + 5;
            else if ((value = NuStrIStr(params[index], (char *)AIAnimSpeedValueName)) != NULL)
                amount = AIParamToFloat(processor, value + 6);
            else if ((value = NuStrIStr(params[index], "increment=")) != NULL) {
                amount = AIParamToFloat(processor, value + 10);
                operation = 1;
            } else if ((value = NuStrIStr(params[index], "decrement=")) != NULL) {
                amount = AIParamToFloat(processor, value + 10);
                operation = -1;
            }
        }
        if (name != NULL) {
            message = CheckGizAIMessage(gizaimessagesys, name, NULL);
            if (message != NULL) {
                switch (operation) {
                    case -1:
                        message->value -= amount;
                        break;
                    case 0:
                        message->value = amount;
                        break;
                    case 1:
                        message->value = message->value + amount;
                        break;
                }
            }
        }
    }
    return 1;
}

i32 __cdecl Action_SetForceBack(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    APIOBJECT_PC *lookup;
    GAMEOBJECT *object;
    NUVEC *position;
    f32 radius;
    i32 type;
    i32 enabled;
    char *value;
    AILOCATOR_PC *locator;

    (void)param_6;
    if (param_5 == 0) {
        return 1;
    }

    object = NULL;
    if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
        object = packet->owner->objptr;
    position = NULL;
    radius = 1.5f;
    type = 0;
    enabled = 1;
    for (index = 0; index < param_4; ++index) {
        value = NuStrIStr(params[index], (char *)AIGameCharacterName);
        if (value != NULL) {
            {
                lookup = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(sys, value + 10) : NULL;
                object = lookup != NULL ? (GAMEOBJECT *)lookup->owner : NULL;
            }
        } else if ((value = NuStrIStr(params[index], (char *)AIPathLocatorName)) != NULL) {
            locator = AIPathFindLocator(sys, value + 8);
            position = locator != NULL ? &locator->position : NULL;
        } else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral7581a0)) != NULL) {
            radius = AIParamToFloat(processor, value + 7);
        } else if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0) {
            enabled = 0;
        } else if (NuStrICmp(params[index], (char *)AITransferLiteral758194) == 0) {
            type = 1;
        } else if (NuStrICmp(params[index], (char *)AITransferLiteral758188) == 0) {
            type = 2;
        } else if (NuStrICmp(params[index], (char *)AITransferLiteral758174) == 0) {
            type = 3;
        }
    }

    if (enabled != 0) {
        if (object != NULL || position != NULL) {
            SetForceBack(object, position, radius, type);
        }
    } else {
        ResetForceBack();
    }
    return 1;
}

i32 __cdecl Action_SpinOnSpot(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                              i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    GAMEOBJECT *object;
    f32 zero;
    u16 angle;
    f32 min_time;
    f32 max_time;
    char *value;
    (void)sys;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL) {
        return 1;
    }

    object = packet->owner->objptr;
    zero = 0.0f;

    if (param_5 != 0) {
        min_time = 0.0f;
        max_time = 0.0f;
        for (index = 0; index < param_4; ++index) {
            value = NuStrIStr(params[index], (char *)AITransferLiteral7581ec);
            if (value != NULL) {
                min_time = AIParamToFloatEx(packet, processor, value + 8);
            } else {
                value = NuStrIStr(params[index], (char *)AITransferLiteral7581e0);
                if (value != NULL) {
                    max_time = AIParamToFloatEx(packet, processor, value + 8);
                } else {
                    value = NuStrIStr(params[index], (char *)AITransferLiteral757cd0);
                    if (value != NULL) {
                        processor->action_timer = AIParamToFloatEx(packet, processor, value + 5);
                    } else {
                        value = NuStrIStr(params[index], (char *)AITransferLiteral7581d4);
                        if (value != NULL) {
                            processor->action_data_4 =
                                (f32)((i32)(AIParamToFloatEx(packet, processor, value + 9) * 182.04444885253906));
                        }
                    }
                }
            }
        }
        if (min_time != max_time) {
            processor->action_timer = NuRandFloat() * (max_time - min_time) + min_time;
        }
        angle = object->field_0x276;
    } else {
        angle = processor->action_data_6;
    }

    angle += (i32)(param_6 * processor->action_data_4);
    processor->action_data_6 = angle;
    processor->action_pos.x = 0.0f;
    processor->action_pos.y = 0.0f;
    processor->action_pos.z = 1.0f;
    NuVecRotateY(&processor->action_pos, &processor->action_pos, angle);
    NuVecAdd(&processor->action_pos, &processor->action_pos, &object->collision_position);
    packet->movement_look_target = &processor->action_pos;

    if (processor->action_timer > zero) {
        processor->action_timer -= param_6;
        if (zero >= processor->action_timer) {
            processor->action_timer = 0.0f;
            return 1;
        }
    }
    return 0;
}

extern const char AITransferLiteral757a8c[7];
extern const char AITransferLiteral757a94[9];
extern const char AITransferLiteral757a84[8];
extern const char ZArgPrefix[3];
extern const char AITransferLiteral75782c[5];
extern const char AIGameCharacterName[11];
extern const char YArgPrefix[3];
extern const char AITransferLiteral757c5c[4];
extern const char XArgPrefix[3];
extern const char AITransferLiteral757c64[4];
extern const char AITransferLiteral757c60[4];
extern const char AITransferLiteral757f28[8];
extern const char AITransferLiteral75781c[8];
extern const char AIGameOpponentName[9];
extern const char AITransferLiteral7583f8[8];
i32 __cdecl Action_SelectRandomSpline(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                      i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    NUGSPLINE *splines[32];
    NUGSPLINE *unused_splines[32];
    i32 count;
    i32 unused;
    char *value;
    NUGSPLINE *spline;
    GAMEOBJECT *object;
    i32 unused_count;
    if (first_time) {

        count = 0;
        unused = 0;
        script_spline_selected = NULL;
        for (index = 0; index < param_count; ++index) {
            value = NuStrIStr(params[index], (char *)AITransferLiteral757a94);
            if (value != NULL) {
                count += NuSplineFindAllBeg(WORLD->current_gscn, value + 8, &splines[count], 32 - count);
            } else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral757a84)) != NULL) {
                spline = NuSplineFind(WORLD->current_gscn, value + 7);
                if (spline != NULL && count < 32)
                    splines[count++] = spline;
            } else if (NuStrIStr(params[index], (char *)AITransferLiteral757a8c) != NULL) {
                unused = 1;
            }
        }
        if (count != 0) {
            if (unused) {
                for (index = 0; index < HIGHGAMEOBJECT; ++index) {
                    object = &Obj[index];
                    if ((object->flags_1fc_word & 0x1001) == 0x1001 && object->dead == 0 &&
                        object->movement_spline_position.spline != NULL)
                        object->movement_spline_position.spline->length |= 0x8000;
                }
                unused_count = 0;
                for (index = 0; index < count; ++index) {
                    if (splines[index]->length >= 0)
                        unused_splines[unused_count++] = splines[index];
                }
                for (index = 0; index < HIGHGAMEOBJECT; ++index) {
                    object = &Obj[index];
                    if ((object->flags_1fc_word & 0x1001) == 0x1001 && object->dead == 0 &&
                        object->movement_spline_position.spline != NULL)
                        object->movement_spline_position.spline->length &= 0x7fff;
                }
                if (unused_count != 0)
                    script_spline_selected = unused_splines[qrand() / (0xffff / unused_count + 1)];
            } else {
                script_spline_selected = splines[qrand() / (0xffff / count + 1)];
            }
        }
    }
    return 1;
}

extern const char RaceOpponentDistancePrefix[9];
i32 Action_CreateSplineCreatures(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                 i32 num_params, i32 first_time, f32 unused6) {
    u16 angle;
    u16 pitch;
    i32 model_count;
    i32 spline_count;
    i32 use_selected;
    i32 min_group_size;
    i32 max_group_size;
    i32 group_size;
    f32 min_distance;
    f32 max_distance;
    f32 distance;
    i32 relative_to_player;
    i32 creature_set;
    i32 looping;
    struct AILOCATOR_PC *relative_locator;
    struct gameobject_s *rider;
    i32 i;
    struct nugspline_s *spline;
    i32 model;
    i16 models[10];
    struct nugspline_s *splines[32];
    char script_name[64] = "default";
    NUVEC spline_offset = {0.0f, 0.0f, 0.0f};
    if (!first_time || num_params <= 0)
        return 1;

    model_count = 0, spline_count = 0, use_selected = 0;
    min_group_size = -1, max_group_size = -1, group_size = 1;
    min_distance = 1000000000.0f, max_distance = 1000000000.0f, distance = 0.0f;
    relative_to_player = 0, creature_set = 0, looping = 0;
    relative_locator = NULL;
    rider = NULL;
    for (i = 0; i < num_params; i++) {
        char *value = NuStrIStr(params[i], "mingroupsize");
        if (value != NULL)
            min_group_size = (i32)AIParamToFloat(processor, value + 13);
        else if ((value = NuStrIStr(params[i], "maxgroupsize")) != NULL)
            max_group_size = (i32)AIParamToFloat(processor, value + 13);
        else if ((value = NuStrIStr(params[i], "groupsize")) != NULL)
            group_size = (i32)AIParamToFloat(processor, value + 10);
        else if ((value = NuStrIStr(params[i], "type")) != NULL) {
            if (LevelCharacterTypeIDFn != NULL && LevelCharacterGlobalIDFn != NULL) {
                u8 type = LevelCharacterTypeIDFn(value + 5);
                if (type != 0xff) {
                    i16 model = LevelCharacterGlobalIDFn(type);
                    if (model != -1 && model_count < 10)
                        models[model_count++] = model;
                }
            }
        } else if ((value = NuStrIStr(params[i], "script")) != NULL)
            NuStrCpy(script_name, value + 7);
        else if ((value = NuStrIStr(params[i], "mindistance")) != NULL)
            min_distance = AIParamToFloat(processor, value + 12);
        else if ((value = NuStrIStr(params[i], "maxdistance")) != NULL)
            max_distance = AIParamToFloat(processor, value + 12);
        else if ((value = NuStrIStr(params[i], (char *)RaceOpponentDistancePrefix)) != NULL)
            distance = AIParamToFloat(processor, value + 9);
        else if (script_spline_selected != NULL && NuStrICmp("use_selected_spline", params[i]) == 0)
            use_selected = 1;
        else if ((value = NuStrIStr(params[i], "splines=")) != NULL)
            spline_count +=
                NuSplineFindAllBeg(WORLD->current_gscn, value + 8, &splines[spline_count], 32 - spline_count);
        else if (NuStrIStr(params[i], "spline=myspline") != NULL) {
            if (processor->myspline != NULL)
                splines[spline_count++] = processor->myspline;
        } else if ((value = NuStrIStr(params[i], (char *)AITransferLiteral757a84)) != NULL) {
            NUGSPLINE *spline = NuSplineFind(WORLD->current_gscn, value + 7);
            if (spline != NULL && spline_count < 32)
                splines[spline_count++] = spline;
        } else if ((value = NuStrIStr(params[i], "x_offset=")) != NULL)
            spline_offset.x = AIParamToFloat(processor, value + 9);
        else if ((value = NuStrIStr(params[i], "y_offset=")) != NULL)
            spline_offset.y = AIParamToFloat(processor, value + 9);
        else if ((value = NuStrIStr(params[i], "z_offset=")) != NULL)
            spline_offset.z = AIParamToFloat(processor, value + 9);
        else if (NuStrICmp(params[i], "looping") == 0)
            looping = 1;
        else if (NuStrIStr(params[i], "addtoset=myset") != NULL)
            creature_set = processor->creature_set;
        else if ((value = NuStrIStr(params[i], "addtoset=")) != NULL)
            creature_set = (i32)AIParamToFloat(processor, value + 9);
        else if (NuStrICmp(params[i], "relative_to_player") == 0)
            relative_to_player = 1;
        else if ((value = NuStrIStr(params[i], "relative_to_locator=")) != NULL)
            relative_locator = AIPathFindLocator(system, value + 20);
        else if (NuStrIStr(params[i], "ridden_by=myself") != NULL) {
            if (packet != NULL && packet->owner != NULL)
                rider = ((GAMEOBJECT *)packet->owner)->objptr;
        } else if ((value = NuStrIStr(params[i], "ridden_by=")) != NULL)
            rider = GetNamedGameObject(system, value + 10);
    }
    if (min_group_size < max_group_size && min_group_size >= 0)
        group_size = NuRand(NULL) % (max_group_size + 1 - min_group_size) + min_group_size;
    if (min_distance != 1000000000.0f && max_distance != 1000000000.0f) {
        f32 fraction = NuRandFloat();
        distance = max_distance * fraction + min_distance * (1.0f - fraction);
    }
    if ((spline_count | use_selected) == 0 || group_size <= 0 || model_count == 0)
        return 1;
    spline = use_selected ? script_spline_selected : splines[qrand() / (65535 / spline_count + 1)];
    model = models[NuRand(NULL) % model_count];
    for (i = 0; i < group_size; i++) {
        GAMEOBJECT *object = AddDynamicCreature(model, NULL, 0, script_name, NULL, NULL, 0, spline, &spline_offset,
                                                looping, creature_set);
        if (object == NULL)
            continue;
        if (relative_to_player || relative_locator != NULL) {
            SPLINEPOS position;
            i16 start = -1, end = -1;
            if (WORLD->current_level == PODSPRINTA_LDATA)
                PodSprint_GetIAlongVals(spline, &start, &end);
            if (relative_to_player) {
                GetNearestSplinePos(&player->collision_position, &position, spline, looping, start, end);
                object->movement_spline_position = position;
            } else if (relative_locator != NULL) {
                GetNearestSplinePos(&relative_locator->position, &position, spline, looping, start, end);
                object->movement_spline_position = position;
            }
        }
        if (distance != 0.0f) {
            NUVEC position;
            NUVEC offset;
            MoveSplinePosition(&object->movement_spline_position, distance);
            position = position, offset = offset;
            PointAlongSpline(object->movement_spline_position.spline, object->movement_spline_position.along, &position,
                             &angle, &pitch, object->movement_spline_position.looping);
            object->movement_facing_angle = angle;
            object->facing_angle = angle;
            object->field_0x276 = angle;
            object->pitch_angle = -pitch;
            if (object->movement_spline_offset.x != 0.0f || object->movement_spline_offset.y != 0.0f ||
                object->movement_spline_offset.z != 0.0f) {
                NuVecRotateX(&offset, &object->movement_spline_offset, object->pitch_angle);
                NuVecRotateY(&offset, &offset, object->field_0x276);
                NuVecAdd(&position, &position, &offset);
            }
            object->position = position;
            object->initial_position = position;
            object->collision_position = position;
            plr_lastpos = position;
            object->start_position = position;
            GameObjectOrigin(object);
            object->last_safe_position = object->position;
            object->ai_update_position = object->position;
        }
        if (rider != NULL)
            TakeOverGameObject(rider, object, 0, 1);
    }
    return 1;
}

i32 __cdecl Action_AddPartDebris(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                 i32 num_params, i32 first_time, f32 elapsed) {
    i32 i;
    NUVEC offset;
    NUVEC position;
    i32 type;
    char *value;
    GAMEOBJECT *object;
    type = -1;
    offset.x = 0.0f;
    offset.y = 0.0f;
    offset.z = 0.0f;
    if (first_time) {
        // The original requires coordinates or a character parameter to supply this position.

        for (i = 0; i < num_params; i++) {
            value = NuStrIStr(params[i], (char *)AITransferLiteral75782c);
            if (value != NULL)
                type = PARTLookupType(value + 5);
            else if ((value = NuStrIStr(params[i], (char *)AITransferLiteral757c64)) != NULL)
                offset.x = AIParamToFloat(processor, value + 3);
            else if ((value = NuStrIStr(params[i], (char *)AITransferLiteral757c60)) != NULL)
                offset.y = AIParamToFloat(processor, value + 3);
            else if ((value = NuStrIStr(params[i], (char *)AITransferLiteral757c5c)) != NULL)
                offset.z = AIParamToFloat(processor, value + 3);
            else if ((value = NuStrIStr(params[i], (char *)XArgPrefix)) != NULL)
                position.x = AIParamToFloat(processor, value + 2);
            else if ((value = NuStrIStr(params[i], (char *)YArgPrefix)) != NULL)
                position.y = AIParamToFloat(processor, value + 2);
            else if ((value = NuStrIStr(params[i], (char *)ZArgPrefix)) != NULL)
                position.z = AIParamToFloat(processor, value + 2);
            else if ((value = NuStrIStr(params[i], (char *)AIGameCharacterName)) != NULL) {
                object = GetNamedGameObject(sys, value + 10);
                if (object != NULL)
                    position = object->collision_position;
            }
        }
        if (type != -1) {
            NuVecAdd(&position, &position, &offset);
            AddFiniteShotPART(type, &position, 1);
        }
    }
    return 1;
}

void GuidedMissile_Kill(PART_PC *part, i32 reason) {
    AddGameDebris(WORLD->game_debris_sys, 0x7d, &part->position);
    AddGameDebris(WORLD->game_debris_sys, 0x21, &part->position);
    GameCam_Judder(GameCam, 0.2f, 0, NULL);
    Game_KillPart(part, reason);
    NewRumbleAllPlayers(0.7f, 0.1f, 0, 0);
}

i32 __cdecl Action_TakeOver(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                            i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    APIOBJECT_PC *lookup;
    GAMEOBJECT *object;
    GAMEOBJECT *target;
    i32 release;
    char *value;
    i32 player_index;
    PLAYERCHARACTERCONFIG_PC *target_config;

    (void)processor;
    (void)param_6;
    object = NULL;
    target = NULL;
    release = 0;
    if (param_5 == 0) {
        return 1;
    }

    if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL) {
        object = packet->owner->objptr;
        target = object->takeover_target;
    }

    for (index = 0; index < param_4; ++index) {
        value = NuStrIStr(params[index], (char *)AIGameCharacterName);
        if (value != NULL) {
            {
                lookup = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(sys, value + 10) : NULL;
                object = lookup != NULL ? (GAMEOBJECT *)lookup->owner : NULL;
            }
        } else {
            value = NuStrIStr(params[index], (char *)AITransferLiteral75781c);
            if (value != NULL) {
                player_index = (i32)(AIParamToFloat(processor, value + 7)) - 1;
                if (player_index >= 0 && player_index <= 1) {
                    object = Player[player_index];
                }
            } else {
                value = NuStrIStr(params[index], (char *)AITransferLiteral757f28);
                if (value != NULL) {
                    {
                        lookup = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(sys, value + 7) : NULL;
                        target = lookup != NULL ? (GAMEOBJECT *)lookup->owner : NULL;
                    }
                } else if (NuStrIStr(params[index], (char *)AIGameOpponentName) != NULL) {
                    if (packet->opponent != NULL) {
                        target = (GAMEOBJECT *)((APIOBJECT_PC *)packet->opponent)->owner;
                    }
                } else if (NuStrIStr(params[index], (char *)AITransferLiteral7583f8) != NULL) {
                    release = 1;
                }
            }
        }
    }

    if (object == NULL) {
        return 1;
    }
    if (target != NULL && release == 0) {
        if ((target->character_data->model_flags & 0x40000000) != 0 &&
            (target->character_context == 0x17 || target->character_context == 0x3e)) {
            return 0;
        }
        target_config = target->character_data->game_character;
        if (target->takeover_source == NULL && (target_config->flags_090 & 0x40) != 0 &&
            (object->character_data->model_flags & 0x10) == 0) {
            TakeOverGameObject(object, target, 1, 0);
        }
    } else if (release != 0) {
        ReleaseTakeOver(object, 0);
    }
    return 1;
}

i32 __cdecl Action_AddDebris(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                             i32 param_4, i32 param_5, f32 param_6) {
    i32 index;
    APIOBJECT_PC *lookup;
    i32 handle;
    NUVEC position;
    GAMEOBJECT *object;
    i32 debris_type;
    char *value;
    NUVEC *debris_position;

    (void)param_6;

    handle = -1;
    position.x = AIPositionUnsetPc;
    position.y = AIPositionUnsetPc;
    position.z = AIPositionUnsetPc;
    object = packet != NULL && packet->owner != NULL ? packet->owner->objptr : NULL;

    if (param_5 != 0) {
        debris_type = 33;
        for (index = 0; index < param_4; ++index) {
            value = NuStrIStr(params[index], (char *)AIGameCharacterName);
            if (value != NULL) {
                {
                    lookup = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(sys, value + 10) : NULL;
                    object = lookup != NULL ? (GAMEOBJECT *)lookup->owner : NULL;
                }
            } else if ((value = NuStrIStr(params[index], (char *)XArgPrefix)) != NULL) {
                position.x = AIParamToFloat(processor, value + 2);
            } else if ((value = NuStrIStr(params[index], (char *)YArgPrefix)) != NULL) {
                position.y = AIParamToFloat(processor, value + 2);
            } else if ((value = NuStrIStr(params[index], (char *)ZArgPrefix)) != NULL) {
                position.z = AIParamToFloat(processor, value + 2);
            } else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral75772c)) != NULL) {
                debris_type = FindGameDebris((APIDEBRISSYS_PC *)(perm_debrissys), value + 5);
            }
        }

        if (position.x == AIPositionUnsetPc && position.y == AIPositionUnsetPc && position.z == AIPositionUnsetPc) {
            if (object == NULL)
                return 1;
            debris_position = &object->collision_position;
        } else {
            debris_position = &position;
        }
        AddFiniteShotDebrisEffect(&handle, WORLD->game_debris_sys->entries[debris_type].effect, debris_position, 1);
    }
    return 1;
}

i32 Action_JudderGameCamera(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                            i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)packet;
    (void)param_6;
    if (param_5 != 0) {
        i32 index;
        i32 axis = 0;
        f32 time = 0.1f;
        for (index = 0; index < param_4; ++index) {
            if (NuStrIStr(params[index], "axis=x") != NULL) {
                axis = 0;
            } else if (NuStrIStr(params[index], "axis=y") != NULL) {
                axis = 1;
            } else if (NuStrIStr(params[index], "axis=z") != NULL) {
                axis = 2;
            } else {
                char *parameter = NuStrIStr(params[index], "time");
                if (parameter != NULL) {
                    time = AIParamToFloat(processor, parameter + 5);
                }
            }
        }
        if (axis == 2 && qrand() < 0x8000) {
            time = -time;
        }
        GameCam_Judder(GameCam, time, axis, NULL);
    }
    return 1;
}

STATIC_ASSERT(offsetof(AIPACKET_PC, antinode_timer) == 0x18c);
STATIC_ASSERT(offsetof(APIOBJECT_PC, ai_collision_priority_pc) == 0x292);
STATIC_ASSERT(offsetof(APIOBJECT_PC, ai_resolved_collision_priority_pc) == 0x294);
STATIC_ASSERT(offsetof(APIOBJECT_PC, collision_link) == 0x298);
typedef void(__cdecl *CHECK_ANTINODE_FN_PC)(APIOBJECT_PC *, AIANTINODE_PC *, NUVEC *, f32);

#include "saga.h"
#include <stdio.h>
#include <string.h>
STATIC_ASSERT(offsetof(AIPATH_PC, name) == 0x0);
STATIC_ASSERT(offsetof(AIPATH_PC, node_count) == 0x10);
STATIC_ASSERT(offsetof(AIPATH_PC, flags) == 0x11);
STATIC_ASSERT(sizeof(AIPATH_PC) == 0xa8);
STATIC_ASSERT(offsetof(AIPATH_PC, connection_count) == 0x12);
STATIC_ASSERT(offsetof(AIPATH_PC, index) == 0x15);
STATIC_ASSERT(offsetof(AIPATH_PC, connections) == 0x80);
STATIC_ASSERT(offsetof(AIPATH_PC, route_matrix) == 0x84);
STATIC_ASSERT(offsetof(AIPATH_PC, bounds_min) == 0x90);
STATIC_ASSERT(offsetof(AIPATH_PC, bounds_max) == 0x9c);

#undef PATH_LOAD_ALLOCATE

/* Original AISYS PC callbacks; bounded native evidence in ../native. */

typedef struct ai_context_reset_view_pc_s {
    u8 unknown_000[0x6c4];
    u8 reset_contexts;
} AI_CONTEXT_RESET_VIEW_PC;
STATIC_ASSERT(offsetof(AI_CONTEXT_RESET_VIEW_PC, reset_contexts) == 0x6c4);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_1f8) == 0x1f8);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_1fc_word) == 0x1fc);
STATIC_ASSERT(offsetof(GAMEOBJECT, dead) == 0x28b);
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET_PC, creature_set) == 0x378);
STATIC_ASSERT(offsetof(GAMEOBJECT, portal_flags) == 0xeec);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_ef4) == 0xef4);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_1010) == 0x1010);
STATIC_ASSERT(sizeof(GAMEOBJECT) == 0x10d8);

i32 __cdecl Action_ResetContext(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                i32 count, i32 first_time, f32 delta) {
    GAMEOBJECT *object;
    APIOBJECT_PC *found;
    char *value;
    i32 index;
    (void)processor;
    (void)delta;
    if (!first_time)
        return 1;
    object = packet != NULL && packet->owner != NULL ? packet->owner->objptr : NULL;
    for (index = 0; index < count; ++index) {
        value = NuStrIStr(params[index], "character=");
        if (value != NULL) {
            found = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(system, value + 10) : NULL;
            object = found != NULL ? found->owner : NULL;
        }
    }
    if (object != NULL) {
        Player_ClearContext(object, 1);
        Player_ResetContexts(&((AI_CONTEXT_RESET_VIEW_PC *)object)->reset_contexts);
    }
    return 1;
}

i32 __cdecl Action_CanBeCarried(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                i32 count, i32 first_time, f32 delta) {
    GAMEOBJECT *object;
    APIOBJECT_PC *found;
    char *value;
    i32 index, enabled = 1;
    (void)processor;
    (void)delta;
    if (!first_time)
        return 1;
    object = packet != NULL && packet->owner != NULL ? packet->owner->objptr : NULL;
    for (index = 0; index < count; ++index) {
        if (NuStrICmp(params[index], "FALSE") == 0)
            enabled = 0;
        else if ((value = NuStrIStr(params[index], "character=")) != NULL) {
            found = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(system, value + 10) : NULL;
            object = found != NULL ? found->owner : NULL;
        }
    }
    if (object != NULL) {
        if (object->character_context == 0x3c || object->takeover_source != NULL)
            Player_ClearContext(object, 1);
        object->flags_ef4 = (object->flags_ef4 & ~2u) | ((u32)(enabled & 1) << 1);
    }
    return 1;
}

i32 __cdecl Action_SetFormationCommander(AISYS_PC *unused0, AISCRIPTPROCESS_PC *unused1, AIPACKET_PC *packet,
                                         char **unused3, i32 unused4, i32 unused5, f32 unused6) {
    GAMEOBJECT *object;
    AIGROUP_PC *group;
    i32 commander_id;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL || packet->group == NULL)
        return 1;
    object = packet->owner->objptr;
    group = packet->group;
    if (packet->group_member_index != group->member_count - 1)
        return 1;
    if (group->count_across != 1 && group->member_count % group->count_across != 1)
        return 1;
    packet->flags |= 0x1000000;
    if ((i16)object->id == id_BATTLEDROID || (i16)object->id == id_BATTLEDROIDSECURITY ||
        (i16)object->id == id_BATTLEDROIDGEONOSIAN)
        commander_id = id_BATTLEDROIDCOMMANDER;
    else if ((i16)object->id == id_CLONEEP3)
        commander_id = id_CLONEEP3SAND;
    else
        return 1;
    NewPlayerCharacter(object, commander_id, (i16)object->id, 1);
    object->current_hp = object->character_data->game_character->hitpoints;
    object->field_107e = object->character_data->game_character->hitpoints;
    return 1;
}

i32 __cdecl Action_AddMiscPickups(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                  i32 count, i32 first_time, f32 unused6) {
    i32 value = 0;
    i32 torpedo = 0;
    GAMEOBJECT *object = NULL;
    APIOBJECT_PC *named;
    NUVEC *position;
    i32 index;
    char *parameter;
    if (first_time != 0) {
        if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
            object = packet->owner->objptr;
        for (index = 0; index < count; ++index) {
            parameter = NuStrIStr(params[index], (char *)AIGameCharacterName);
            if (parameter != NULL) {
                named = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(system, parameter + 10) : NULL;
                object = named != NULL ? named->owner : NULL;
            } else if ((parameter = NuStrIStr(params[index], (char *)AIAnimSpeedValueName)) != NULL)
                value = (i32)AIParamToFloat(processor, parameter + 6);
            else if ((parameter = NuStrIStr(params[index], (char *)AIActionTorpedoPrefixPc)) != NULL)
                torpedo = (i32)AIParamToFloat(processor, parameter + 8);
        }
        if (object != NULL) {
            position = &object->collision_position;
            if (position != NULL && (value != 0 || torpedo != 0))
                AddMiscPickups(position, -1, value, torpedo);
        }
    }
    return 1;
}

i32 __cdecl Action_CreateRider(AISYS_PC *unused0, AISCRIPTPROCESS_PC *unused1, AIPACKET_PC *packet, char **params,
                               i32 count, i32 first_time, f32 unused6) {
    GAMEOBJECT *vehicle = NULL;
    i32 model = -1;
    char *script = NULL;
    i32 index;
    char *value;
    GAMEOBJECT *rider;
    if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
        vehicle = packet->owner->objptr;
    if (first_time != 0 && count > 0) {
        for (index = 0; index < count; ++index) {
            value = NuStrIStr(params[index], (char *)AITransferLiteral75772c);
            if (value != NULL) {
                if (LevelCharacterTypeIDFn != NULL && LevelCharacterGlobalIDFn != NULL) {
                    model = (u8)LevelCharacterTypeIDFn(value + 5);
                    if (model != 0xff)
                        model = LevelCharacterGlobalIDFn(model);
                }
            } else if ((value = NuStrIStr(params[index], "script")) != NULL)
                script = value + 7;
        }
        if (model != -1 && vehicle->rider == NULL) {
            rider =
                AddDynamicCreature(model, &vehicle->collision_position, 0,
                                   script != NULL ? script : GizmoDefaultOutputName, NULL, NULL, 0, NULL, NULL, 0, 0);
            if (rider != NULL)
                TakeOverGameObject(rider, vehicle, 0, 1);
        }
    }
    return 1;
}

i32 __cdecl Action_TakeDamage(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                              i32 count, i32 first_time, f32 delta) {
    GAMEOBJECT *object, *hitter = NULL, *candidate;
    APIOBJECT_PC *found, *opponent;
    char *value;
    i32 index, damage = 1, creature_set = 0;
    (void)delta;
    if (!first_time)
        return 1;
    object = NULL;
    if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
        object = packet->owner->objptr;
    for (index = 0; index < count; ++index) {
        value = NuStrIStr(params[index], "character=");
        if (value != NULL) {
            found = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(system, value + 10) : NULL;
            object = found != NULL ? found->owner : NULL;
        } else if ((value = NuStrIStr(params[index], "hitter=")) != NULL) {
            found = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(system, value + 7) : NULL;
            hitter = found != NULL ? found->owner : NULL;
        } else if ((value = NuStrIStr(params[index], "damage=")) != NULL) {
            damage = (i32)AIParamToFloat(processor, value + 7);
        } else if (NuStrIStr(params[index], "opponent") != NULL) {
            if (packet != NULL && packet->opponent != NULL) {
                opponent = (APIOBJECT_PC *)packet->opponent;
                if (opponent->owner != NULL) {
                    object = opponent->owner;
                    if (packet->owner != NULL && packet->owner->objptr != NULL)
                        hitter = packet->owner->objptr;
                }
            }
        } else if ((value = NuStrIStr(params[index], "set=")) != NULL) {
            creature_set = (i32)AIParamToFloat(processor, value + 4);
            if (creature_set < 0 || creature_set > 16)
                creature_set = 0;
        }
    }
    if (creature_set != 0) {
        candidate = Obj;
        for (index = 0; index < HIGHGAMEOBJECT; ++index, ++candidate) {
            if ((candidate->flags_1fc & 1) != 0 && candidate->ai_packet.creature_set == creature_set)
                ObjHitObj(hitter, candidate, damage, 0, 0, 1);
        }
    } else if (object != NULL) {
        ObjHitObj(hitter, object, damage, 0, 0, 1);
    }
    return 1;
}

i32 __cdecl Action_Kill(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params, i32 count,
                        i32 first_time, f32 delta) {
    GAMEOBJECT *object, *candidate, *excluded = NULL;
    APIOBJECT_PC *found;
    char *value;
    i32 index, creature_set = 0, all_ai = 0, check_dead = 0, parts = 0, debris = 0, respawn = 0, origin = 0;
    (void)delta;
    if (!first_time)
        return 1;
    object = NULL;
    if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
        object = packet->owner->objptr;
    for (index = 0; index < count; ++index) {
        value = NuStrIStr(params[index], "character");
        if (value != NULL) {
            found = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(system, value + 10) : NULL;
            object = found != NULL ? found->owner : NULL;
        } else if (NuStrIStr(params[index], "opponent") != NULL) {
            if (packet->opponent != NULL)
                object = ((APIOBJECT_PC *)packet->opponent)->owner;
        } else if (NuStrICmp(params[index], "respawn") == 0) {
            respawn = 1;
        } else if (NuStrICmp(params[index], "respawn_at_origin") == 0) {
            respawn = 1;
            origin = 1;
        } else if ((value = NuStrIStr(params[index], "all_ai_except")) != NULL) {
            excluded = GetNamedGameObject(system, value + 14);
            all_ai = 1;
        } else if (NuStrICmp(params[index], "all_ai") == 0) {
            all_ai = 1;
        } else if (NuStrICmp(params[index], "check_if_dead") == 0) {
            check_dead = 1;
        } else if (NuStrICmp(params[index], "debris") == 0) {
            debris = 1;
        } else if (NuStrICmp(params[index], "parts_on") == 0) {
            parts = 1;
        } else if ((value = NuStrIStr(params[index], "set=")) != NULL) {
            creature_set = (i32)AIParamToFloat(processor, value + 4);
            if (creature_set < 0 || creature_set > 16)
                creature_set = 0;
        }
    }
    if (all_ai) {
        candidate = Obj;
        for (index = 0; index < HIGHGAMEOBJECT; ++index, ++candidate) {
            if ((candidate->flags_1fc_word & 1) && (candidate->flags_1fc_word & 0x1000) &&
                (candidate->flags_1f8 & 0x400) && candidate != excluded &&
                (!check_dead || (candidate->dead == 0 && candidate->field_1010 <= 0.0f))) {
                candidate->portal_flags =
                    (candidate->portal_flags & ~0x00300000u) | ((u32)((respawn & 1) | ((origin & 1) << 1)) << 20);
                if (parts)
                    KillParts(candidate, -1, -1, 1, 0.0f, 0, NULL);
                if (debris)
                    KillGameObject(candidate, 2, 0);
                else
                    KillGameObject(candidate, 4, 0);
            }
        }
    } else if (creature_set != 0) {
        candidate = Obj;
        for (index = 0; index < HIGHGAMEOBJECT; ++index, ++candidate) {
            if ((candidate->flags_1fc_word & 1) && (candidate->flags_1fc_word & 0x1000) &&
                candidate->ai_packet.creature_set == creature_set &&
                (!check_dead || (candidate->dead == 0 && candidate->field_1010 <= 0.0f))) {
                candidate->portal_flags =
                    (candidate->portal_flags & ~0x00300000u) | ((u32)((respawn & 1) | ((origin & 1) << 1)) << 20);
                if (parts)
                    KillParts(candidate, -1, -1, 1, 0.0f, 0, NULL);
                if (debris)
                    KillGameObject(candidate, 2, 0);
                else
                    KillGameObject(candidate, 4, 0);
            }
        }
    } else if (object != NULL && (!check_dead || (object->dead == 0 && object->field_1010 <= 0.0f))) {
        object->portal_flags =
            (object->portal_flags & ~0x00300000u) | ((u32)((respawn & 1) | ((origin & 1) << 1)) << 20);
        if (parts)
            KillParts(object, -1, -1, 1, 0.0f, 0, NULL);
        if (debris)
            KillGameObject(object, 2, 0);
        else
            KillGameObject(object, 4, 0);
    }
    return 1;
}

void __cdecl GizObstacle_JumpToStart(GIZOBSTACLE *);
void __cdecl GizObstacle_JumpToEnd(GIZOBSTACLE *);
void __cdecl GizObstacle_PlayForwards(GIZOBSTACLE *);
void __cdecl GizObstacle_PlayBackwards(GIZOBSTACLE *);

STATIC_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET_PC, creature_index) == 0x400);
i32 __cdecl Action_Activate(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                            i32 count, i32 first_time, f32 delta) {
    GAMEOBJECT *object, *candidate;
    APIOBJECT_PC *found;
    i32 index, creature_set = 0;
    char *value;
    (void)delta;
    if (!first_time)
        return 1;
    object = NULL;
    if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
        object = packet->owner->objptr;
    for (index = 0; index < count; ++index) {
        value = NuStrIStr(params[index], "character=");
        if (value != NULL) {
            found = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(system, value + 10) : NULL;
            object = found != NULL ? found->owner : NULL;
        } else if ((value = NuStrIStr(params[index], "set=")) != NULL) {
            creature_set = (i32)AIParamToFloat(processor, value + 4);
            if (creature_set < 0 || creature_set > 16)
                creature_set = 0;
        }
    }
    if (creature_set != 0) {
        candidate = Obj;
        for (index = 0; index < HIGHGAMEOBJECT; ++index, ++candidate) {
            if ((candidate->flags_1fc & 1) && candidate->ai_packet.creature_set == creature_set) {
                if (candidate->ai_packet.creature_index == 0xff) {
                    candidate->flags_1fc_word |= 0x1000;
                    AIScriptSetBaseScriptStateByName(&candidate->ai_packet.script_processor, "Base");
                } else
                    ResetAICreature(candidate, system);
                ++aicreature_sets_alive[creature_set - 1];
            }
        }
    } else if (object != NULL) {
        if (object->ai_packet.creature_index == 0xff) {
            object->flags_1fc_word |= 0x1000;
            AIScriptSetBaseScriptStateByName(&object->ai_packet.script_processor, "Base");
        } else
            ResetAICreature(object, system);
        if (object->ai_packet.creature_set != 0 && object->ai_packet.creature_set <= 16)
            ++aicreature_sets_alive[object->ai_packet.creature_set - 1];
    }
    return 1;
}

i32 __cdecl Action_RemoveThrownForceObjects(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                            char **params, i32 count, i32 first_time, f32 delta) {
    PART_PC *part = Part;
    i32 index;
    (void)system;
    (void)processor;
    (void)packet;
    (void)params;
    (void)count;
    (void)first_time;
    (void)delta;
    for (index = 0; index < MAXPARTS; ++index, ++part)
        if ((part->active & 1) && part->kill_fn == PartKill_ForceThrow)
            KillPart(part, 0);
    return 1;
}

i32 __cdecl Action_EatVictim(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                             i32 count, i32 first_time, f32 delta) {
    (void)system;
    (void)processor;
    (void)params;
    (void)count;
    (void)first_time;
    (void)delta;
    if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
        EatVictim(packet->owner->objptr);
    return 1;
}

i32 __cdecl Action_ReleaseVictim(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                 i32 count, i32 first_time, f32 delta) {
    (void)system;
    (void)processor;
    (void)params;
    (void)count;
    (void)first_time;
    (void)delta;
    if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
        ReleaseEat(packet->owner->objptr);
    return 1;
}

static i32 Action_Launch(AISYS_PC *unused0, AISCRIPTPROCESS_PC *unused1, AIPACKET_PC *packet, char **unused3,
                         i32 unused4, i32 first_time, f32 unused6) {
    if (first_time && packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
        StartLaunch(packet->owner->objptr);
    return 1;
}

i32 __cdecl Action_AnimTimeRandom(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                  i32 count, i32 first_time, f32 delta) {
    GAMEOBJECT *object;
    (void)system;
    (void)processor;
    (void)params;
    (void)count;
    (void)first_time;
    (void)delta;
    if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL) {
        object = packet->owner->objptr;
        SetAnimTimeRandom(object->character_model, &object->anim_packet);
    }
    return 1;
}

i32 __cdecl Action_AlertCreatures(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                  i32 count, i32 first_time, f32 delta) {
    GAMEOBJECT *object;
    APIOBJECT_PC *found;
    char *value;
    i32 index;
    (void)processor;
    (void)delta;
    if (!first_time)
        return 1;
    object = NULL;
    if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
        object = packet->owner->objptr;
    for (index = 0; index < count; ++index) {
        value = NuStrIStr(params[index], "character=");
        if (value != NULL) {
            found = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(system, value + 10) : NULL;
            object = found != NULL ? found->owner : NULL;
        }
    }
    if (object != NULL)
        AlertSurroundingCreatures(object, &object->collision_position);
    return 1;
}

i32 __cdecl Action_PlayGizObstacle(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                   i32 count, i32 first_time, f32 delta) {
    GIZOBSTACLE *obstacle = NULL;
    GIZMO_PC *gizmo;
    char *value;
    i32 index, backwards = 0, stay_open = 0, stay_shut = 0, snap = 0;
    (void)system;
    (void)processor;
    (void)packet;
    (void)delta;
    if (!first_time || count == 0)
        return 1;
    for (index = 0; index < count; ++index) {
        value = NuStrIStr(params[index], "name=");
        if (value != NULL) {
            gizmo = (GIZMO_PC *)GizmoFindByName(WORLD->gizmo_sys, obstacle_gizmotype_id, value + 5);
            if (gizmo != NULL)
                obstacle = (GIZOBSTACLE *)gizmo->object;
        } else if (NuStrICmp(params[index], "backwards") == 0)
            backwards = 1;
        else if (NuStrICmp(params[index], "stayopen") == 0)
            stay_open = 1;
        else if (NuStrICmp(params[index], "stayshut") == 0)
            stay_shut = 1;
        else if (NuStrICmp(params[index], "SNAP") == 0)
            snap = 1;
    }
    if (obstacle != NULL) {
        if (backwards || stay_shut) {
            if (snap)
                GizObstacle_JumpToStart(obstacle);
            else
                GizObstacle_PlayBackwards(obstacle);
        } else if (snap)
            GizObstacle_JumpToEnd(obstacle);
        else
            GizObstacle_PlayForwards(obstacle);
        obstacle->runtime_flags =
            (obstacle->runtime_flags & ~0xcu) | ((u32)(stay_open & 1) << 2) | ((u32)(stay_shut & 1) << 3);
    }
    return 1;
}

typedef struct ai_force_fields_pc_s {
    u8 unknown000[0x1c];
    NUVEC position;
    u8 unknown028[0x74 - 0x28];
    u32 config_flags;
    u8 unknown078[0xa0 - 0x78];
    u32 runtime_flags;
} AI_FORCE_FIELDS_PC;
typedef struct ai_force_object_pc_s {
    u8 unknown000[0x10cc];
    GIZFORCE_PC *force;
} AI_FORCE_OBJECT_PC;
STATIC_ASSERT(offsetof(AI_FORCE_FIELDS_PC, position) == 0x1c);
STATIC_ASSERT(offsetof(AI_FORCE_FIELDS_PC, config_flags) == 0x74);
STATIC_ASSERT(offsetof(AI_FORCE_FIELDS_PC, runtime_flags) == 0xa0);
STATIC_ASSERT(offsetof(AI_FORCE_OBJECT_PC, force) == 0x10cc);
STATIC_ASSERT(offsetof(GAMEPAD_PC, runtime_flags) == 0x5a);
i32 __cdecl Action_UseForce(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                            i32 count, i32 first_time, f32 delta) {
    GAMEOBJECT *object;
    GIZFORCE_PC *force, *candidates[16];
    AI_FORCE_FIELDS_PC *view;
    GIZMO_PC *gizmo;
    char *name;
    i32 index, candidate_count = 0, triggered_by_hit = 0, valid_category;
    (void)system;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    if (first_time) {
        processor->action_data_3 = NULL;
        for (index = 0; index < count; ++index) {
            if (NuStrICmp(params[index], "throwable") == 0)
                processor->action_data_1 = 1;
            else if (NuStrICmp(params[index], "inrange") == 0)
                processor->action_data_2 = 1;
            else if (NuStrICmp(params[index], "triggered_by_hit") == 0)
                triggered_by_hit = 1;
            else {
                name = NuStrIStr(params[index], "name");
                name = name != NULL ? name + 5 : params[index];
                if (name != NULL && candidate_count < 16) {
                    gizmo = (GIZMO_PC *)GizmoFindByName(WORLD->gizmo_sys, force_gizmotype_id, name);
                    if (gizmo != NULL && gizmo->object != NULL) {
                        force = (GIZFORCE_PC *)gizmo->object;
                        candidates[candidate_count] = force;
                        view = (AI_FORCE_FIELDS_PC *)force;
                        if ((view->runtime_flags & 2) && !(view->runtime_flags & 0x10000))
                            ++candidate_count;
                    }
                }
            }
        }
        if (candidate_count != 0)
            processor->action_data_3 = candidates[qrand() / (0xffff / candidate_count + 1)];
    }
    force = (GIZFORCE_PC *)processor->action_data_3;
    if (force == NULL)
        return 1;
    view = (AI_FORCE_FIELDS_PC *)force;
    if (!(view->runtime_flags & 1))
        return 0;
    valid_category = CharCategory_IsCategory(object, (view->config_flags & 0x10) ? 1 : 0) != 0;
    if (triggered_by_hit) {
        view->runtime_flags |= 0x8000;
        return 0;
    }
    if (valid_category) {
        packet->movement_look_target = &view->position;
        object->pad->runtime_flags |= 4;
        ((AI_FORCE_OBJECT_PC *)object)->force = force;
        if (processor->action_data_1 != 0) {
            if (packet->nearest_opponent == NULL) {
                object->pad->runtime_flags &= ~4;
                ((AI_FORCE_OBJECT_PC *)object)->force = NULL;
            }
            return 1;
        }
        return GizForce_Complete(force) != 0;
    }
    if (FreePlay != 0) {
        processor->action_timer -= delta;
        if (processor->action_timer < 0.0f) {
            processor->action_timer = 0.5f;
            object->pad->buttons_pressed |= GAMEPAD_TOGGLERIGHT;
        }
    }
    return 0;
}

/* Original AISYS callback family; PC-native seven-argument callback ABI. */
extern i32 __cdecl TagCharacter(GAMEOBJECT *, GAMEOBJECT *, i32);
extern void __cdecl RegisterTakeOverObject(GAMEOBJECT *);
STATIC_ASSERT(offsetof(AIPACKET_PC, owner) == 0xd0);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_1fc_word) == 0x1fc);
STATIC_ASSERT(offsetof(GAMEOBJECT, tag_flags) == 0x7c5);
STATIC_ASSERT(offsetof(GAMEOBJECT, takeover_source) == 0xcd0);

i32 __cdecl Action_SetTaggable(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                               i32 count, i32 first_time, f32 delta) {
    GAMEOBJECT *object, *target = NULL;
    APIOBJECT_PC *found;
    char *value;
    i32 index, disabled = 0;
    (void)processor;
    (void)delta;
    if (!first_time)
        return 1;
    object = NULL;
    if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
        object = packet->owner->objptr;
    for (index = 0; index < count; ++index) {
        value = NuStrIStr(params[index], (char *)AIGameCharacterName);
        if (value != NULL) {
            found = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(system, value + 10) : NULL;
            object = found != NULL ? found->owner : NULL;
        } else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral757ee0)) != NULL) {
            found = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(system, value + 7) : NULL;
            target = found != NULL ? found->owner : NULL;
        } else if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0) {
            disabled = 1;
        }
    }
    if (object != NULL) {
        if (disabled && (object->flags_1fc_word & 0x80) != 0) {
            if (object->takeover_source != NULL)
                ReleaseTakeOver(object, 0);
            else if (TagCharacter(object, target, 0) == 0)
                disabled = 0;
            SetPlayer();
        }
        object->tag_flags = (object->tag_flags & ~2U) | ((u32)disabled << 1);
    }
    return 1;
}

i32 __cdecl Action_TagCharacter(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                i32 count, i32 first_time, f32 delta) {
    GAMEOBJECT *object, *target = NULL;
    APIOBJECT_PC *found;
    char *value;
    i32 index;
    (void)processor;
    (void)delta;
    if (!first_time)
        return 1;
    object = NULL;
    if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
        object = packet->owner->objptr;
    if (count == 0)
        return 1;
    for (index = 0; index < count; ++index) {
        value = NuStrIStr(params[index], (char *)AIGameCharacterName);
        if (value != NULL) {
            found = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(system, value + 10) : NULL;
            object = found != NULL ? found->owner : NULL;
        } else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral757ee0)) != NULL) {
            found = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(system, value + 7) : NULL;
            target = found != NULL ? found->owner : NULL;
        }
    }
    if (object != NULL) {
        TagCharacter(object, target, 0);
        SetPlayer();
    }
    return 1;
}

i32 __cdecl Action_RegisterTakeOverObject(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                          char **params, i32 count, i32 first_time, f32 delta) {
    GAMEOBJECT *object;
    APIOBJECT_PC *found;
    char *value;
    i32 index;
    (void)processor;
    (void)delta;
    if (!first_time)
        return 1;
    object = NULL;
    if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
        object = packet->owner->objptr;
    for (index = 0; index < count; ++index) {
        value = NuStrIStr(params[index], (char *)AIGameCharacterName);
        if (value != NULL) {
            found = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(system, value + 10) : NULL;
            object = found != NULL ? found->owner : NULL;
        }
    }
    if (object != NULL)
        RegisterTakeOverObject(object);
    return 1;
}

extern const char AIActionBluePrefixPc[6];
extern const char AIActionGreenPrefixPc[7];
extern const char AIActionRedPrefixPc[5];
extern const char AIActionCountPrefixPc[7];
extern const char AIActionTotalPrefixPc[7];
extern const char AIActionMyPosNamePc[6];
extern const char AIActionCanBeStolenNamePc[12];
extern const char AIActionTorpedoNamePc[8];

typedef struct torpedo_action_packet_pc_s {
    u8 count;
    u8 flags;
    u8 unknown_02;
    u8 state;
    u8 unknown_04[4];
    f32 timer;
    u8 unknown_0c[0x30];
    NUVEC pickup_positions[5];
    u8 unknown_78[0x10];
} TORPEDO_ACTION_PACKET_PC;
STATIC_ASSERT(sizeof(TORPEDO_ACTION_PACKET_PC) == 0x88);
STATIC_ASSERT(offsetof(TORPEDO_ACTION_PACKET_PC, count) == 0);
STATIC_ASSERT(offsetof(TORPEDO_ACTION_PACKET_PC, flags) == 1);
STATIC_ASSERT(offsetof(TORPEDO_ACTION_PACKET_PC, state) == 3);
STATIC_ASSERT(offsetof(TORPEDO_ACTION_PACKET_PC, timer) == 8);
STATIC_ASSERT(offsetof(TORPEDO_ACTION_PACKET_PC, pickup_positions) == 0x3c);

i32 __cdecl Action_AddGameMsgCount(AISYS_PC *unused0, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                   i32 param_count, i32 first_time, f32 unused6) {
    GAMEOBJECT *object;
    NUVEC *position;
    i32 count = 0;
    i32 total = 0;
    u8 red = 0;
    u8 green = 0;
    u8 blue = 0;
    i32 index;
    char *value;
    GIZAIMESSAGE *message;
    position = NULL;
    object = NULL;
    if (first_time != 0) {
        if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
            object = packet->owner->objptr;
        for (index = 0; index < param_count; ++index) {
            if (NuStrICmp(AIActionMyPosNamePc, params[index]) == 0 && object != NULL)
                position = &object->collision_position;
            else if ((value = NuStrIStr(params[index], (char *)AIActionTotalPrefixPc)) != NULL)
                total = (i32)AIParamToFloat(processor, value + 6);
            else if ((value = NuStrIStr(params[index], (char *)AIActionCountPrefixPc)) != NULL)
                count = (i32)AIParamToFloat(processor, value + 6);
            else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral757c20)) != NULL) {
                message = CheckGizAIMessage(gizaimessagesys, value + 11, NULL);
                if (message != NULL)
                    count = (i32)message->value;
            } else if ((value = NuStrIStr(params[index], (char *)AIActionRedPrefixPc)) != NULL)
                red = (u8)AIParamToFloat(processor, value + 4);
            else if ((value = NuStrIStr(params[index], (char *)AIActionGreenPrefixPc)) != NULL)
                green = (u8)AIParamToFloat(processor, value + 6);
            else if ((value = NuStrIStr(params[index], (char *)AIActionBluePrefixPc)) != NULL)
                blue = (u8)AIParamToFloat(processor, value + 5);
        }
        if (count != 0 && count <= total)
            AddGameMsgCount(position, count, total, red, green, blue, 0.75f);
    }
    return 1;
}

i32 __cdecl Action_AddTorpedoPacket(AISYS_PC *unused0, AISCRIPTPROCESS_PC *unused1, AIPACKET_PC *packet, char **params,
                                    i32 param_count, i32 first_time, f32 unused6) {
    GAMEOBJECT *object = NULL;
    i32 count = 0;
    u8 flags = 0;
    i32 index;
    char *value;
    if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
        object = packet->owner->objptr;
    if (first_time != 0) {
        for (index = 0; index < param_count; ++index) {
            value = NuStrIStr(params[index], (char *)AIActionTorpedoNamePc);
            if (value != NULL) {
                count = (i32)AIParamToFloat(&packet->script_processor, value + 8);
                if (count < 0)
                    count = 0;
                else if (count > getMaxTorpedos(NULL))
                    count = getMaxTorpedos(NULL);
            } else if (NuStrICmp(params[index], AIActionCanBeStolenNamePc) == 0)
                flags |= 0x20;
        }
    }
    if (object != NULL) {
        if (object->torpedo != NULL)
            FreeTorpedoPacket(&object->torpedo);
        object->torpedo = GetTorpedoPacket();
        if (object->torpedo != NULL && count != 0) {
            ((TORPEDO_ACTION_PACKET_PC *)object->torpedo)->count = (u8)count;
            ((TORPEDO_ACTION_PACKET_PC *)object->torpedo)->flags |= flags;
            for (index = 0; index < count; ++index) {
                ((TORPEDO_ACTION_PACKET_PC *)object->torpedo)->pickup_positions[index] = object->collision_position;
                ((TORPEDO_ACTION_PACKET_PC *)object->torpedo)->timer = 0.8f;
                ((TORPEDO_ACTION_PACKET_PC *)object->torpedo)->state = 0;
            }
        }
    }
    return 1;
}

STATIC_ASSERT(offsetof(AISYS_PC, player_1) == 0x138c);
STATIC_ASSERT(offsetof(AISYS_PC, player_2) == 0x1390);
STATIC_ASSERT(offsetof(AISCRIPTPROCESS_PC, locator) == 0xa4);
STATIC_ASSERT(offsetof(AILOCATOR_PC, position) == 0x10);
STATIC_ASSERT(offsetof(APIOBJECT_PC, position) == 0x5c);
STATIC_ASSERT(offsetof(AISCRIPTPROCESS_PC, unknown_a0) == 0xa0);
STATIC_ASSERT(offsetof(AISYS_PC, areas) == 0x38);
STATIC_ASSERT(offsetof(AISYS_PC, player_1) == 0x138c);
STATIC_ASSERT(offsetof(AIPACKET_PC, owner) == 0xd0);
STATIC_ASSERT(offsetof(AIPACKET_PC, opponent) == 0xe4);
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_area_mask) == 0x2b0);

STATIC_ASSERT(offsetof(AIPACKET_PC, opponent) == 0xe4);
STATIC_ASSERT(offsetof(AISCRIPTPROCESS_PC, locator) == 0xa4);
STATIC_ASSERT(offsetof(AILOCATOR_PC, position) == 0x10);
STATIC_ASSERT(offsetof(GAMEOBJECT, position) == 0x5c);
STATIC_ASSERT(offsetof(edui_sel_s, selected) == 0x4c);
STATIC_ASSERT(offsetof(edui_sel_s, held) == 0x50);

#undef REBUILD_ALLOCATE
STATIC_ASSERT(sizeof(((AIEDITOR_RENDER_STATE *)0)->pending_locator_name) == 16);
STATIC_ASSERT(offsetof(AISYS_PC, scripts) == 0x1c);
STATIC_ASSERT(offsetof(AISYS_PC, player_1) == 0x138c);
STATIC_ASSERT(offsetof(APIOBJECT_PC, position) == 0x5c);

typedef struct AI_EDITOR_PICK_MODEL_VIEW_PC {
    u8 unknown_00[0x1c4];
    f32 min_height;
    f32 height;
    f32 radius;
} AI_EDITOR_PICK_MODEL_VIEW_PC;
STATIC_ASSERT(offsetof(AI_EDITOR_PICK_MODEL_VIEW_PC, min_height) == 0x1c4);
STATIC_ASSERT(offsetof(AI_EDITOR_PICK_MODEL_VIEW_PC, height) == 0x1c8);
STATIC_ASSERT(offsetof(AI_EDITOR_PICK_MODEL_VIEW_PC, radius) == 0x1cc);
STATIC_ASSERT(sizeof(AI_EDITOR_PICK_MODEL_VIEW_PC) == 0x1d0);

/* SOURCE-ONLY R66 three-function carry: append to genuine gameapi/aisys.c after fresh rebind.
 * Reuses locator's aieditorsettings.locator_rotation and AI_EDITOR_LOCATOR_HOVER_VIEW_PC;
 * does not redefine either. No ABI attributes, helper forcing or compiler measures.
 * Native private register arguments must emerge naturally when all same-TU bodies
 * are compiled; these declarations express their ordinary typed object contracts.
 */

STATIC_ASSERT(offsetof(AIEDITOR_RENDER_STATE, camera_position) == 0x28);

/* Prospective src/gameapi/aisys.c append; existing typed state/shape contracts reused. */
void __cdecl NuRndrBoundingBox(NUVEC *minimum, NUVEC *maximum, NUMTX *matrix, i32 colour);

/* Native settings names are observed 16-byte views within the initializer's
 * 512-byte zero publication; Native owning allocations remain unknown. */

STATIC_ASSERT(offsetof(NUCAMERA, near_clip) == 0x48);
STATIC_ASSERT(sizeof(NURND_VERTEX3D_PC) == 0x24);
STATIC_ASSERT(offsetof(NURND_VERTEX3D_PC, colour) == 0x18);
void AiRndrLine3d(NURND_VERTEX3D_PC *vertices, NUMTL *material, NUMTX *matrix);

/* Native PC solid-edge helper: 0x00522e30, complete 1000-byte body. */
STATIC_ASSERT(offsetof(EDAIPATHNODE_s, position) == 0x18);
i32 __cdecl NewRayCastScaleY(NUVEC *position, NUVEC *movement, f32 radius, f32 scale_y, i32 scan_flags);

/* Native editor path geometry uses the established PC runtime records. */
STATIC_ASSERT(sizeof(AIPATH_PC) == 0xa8);
STATIC_ASSERT(offsetof(AIPATH_PC, node_count) == 0x10);
STATIC_ASSERT(offsetof(AIPATH_PC, connection_count) == 0x12);
STATIC_ASSERT(offsetof(AIPATH_PC, nodes) == 0x7c);
STATIC_ASSERT(offsetof(AIPATH_PC, connections) == 0x80);
STATIC_ASSERT(sizeof(NUVEC) == 12);
void __cdecl routeEditor_Render(i32 x, i32 y, f32 xscale, f32 yscale);
void __cdecl areaEditor_Render(i32 x, i32 y, f32 xscale, f32 yscale);
