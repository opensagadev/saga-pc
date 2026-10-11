#include "saga.h"

typedef struct bulk_ai_helper_pc_s {
    AIPATHCNX_PC *connection;
    void *target;
    u8 direction, type;
    u8 padding[2];
    f32 jump_off_dy;
} BULK_AI_HELPER_PC;
typedef struct bulk_ai_helper_system_pc_s {
    i16 capacity, count;
    BULK_AI_HELPER_PC *helpers;
} BULK_AI_HELPER_SYSTEM_PC;
typedef struct bulk_ai_world_pc_s {
    u8 unknown_0[0x4674];
    BULK_AI_HELPER_SYSTEM_PC *helper_system;
} BULK_AI_WORLD_PC;
STATIC_ASSERT(sizeof(BULK_AI_HELPER_PC) == 0x10);
STATIC_ASSERT(offsetof(BULK_AI_HELPER_PC, jump_off_dy) == 0xc);
STATIC_ASSERT(offsetof(BULK_AI_WORLD_PC, helper_system) == 0x4674);
STATIC_ASSERT(sizeof(BULK_AI_HELPER_SYSTEM_PC) == 8);

extern "C" i32 special_debug;
extern "C" void __cdecl PCSpecialEmptyHandler_candidate(void);
extern "C" {
    BULK_AI_HELPER_PC *__cdecl AIPathCnxHelperSys_AddHelper(BULK_AI_HELPER_SYSTEM_PC *system, AIPATHCNX_PC *connection,
                                                            u8 direction, void *target, u8 type) {
        if (system == NULL || system->count >= system->capacity || connection == NULL || target == NULL)
            return NULL;
        i32 count = system->count;
        BULK_AI_HELPER_PC *helper = system->helpers;
        for (i32 index = 0; index < count; ++index, ++helper) {
            if (helper->connection == connection && helper->type == type && helper->target == target) {
                if (helper->direction != direction)
                    helper->direction = 0xff;
                return helper;
            }
        }
        helper = &system->helpers[count];
        ++system->count;
        helper->connection = connection;
        helper->direction = direction;
        helper->target = target;
        helper->type = type;
        return helper;
    }
    i32 __cdecl CharCategory_FindByName(char *name) {
        if (CharCategory != NULL) {
            for (i32 index = 0; index < CHARCATEGORYCOUNT; ++index)
                if (NuStrICmp(CharCategory[index].name, name) == 0)
                    return index;
        }
        return -1;
    }
    SOCK_PC *__cdecl FindSock(SOCKSYS_PC *sys, char *name) {
        if (sys != NULL) {
            SOCK_PC *sock = sys->socks;
            for (i32 index = 0; index < 64; ++index, ++sock)
                if (sock->valid && NuStrICmp(name, sock->name) == 0)
                    return sock;
        }
        return NULL;
    }
    void __cdecl SockOn(SOCKSYS_PC *sys, i32 index) {
        if (sys != NULL && (u32)index < 64 && (sys->socks[index].flags & 0x100) != 0)
            sys->socks[index].flags &= ~0x100;
    }
    void __cdecl SockOff(SOCKSYS_PC *sys, i32 index) {
        if (sys != NULL && (u32)index < 64 && (sys->socks[index].flags & 0x100) == 0)
            sys->socks[index].flags |= 0x100;
    }
}
static i32 NuSpecialFindMultiDisplayPc(NUGSCN *scene, NUSPECIALHANDLE *dest, char *name, i32 capacity, i32 flags) {
    i32 count = 0;
    NUDISPLAY_SPECIAL_PC *special = scene->display_list->specials;
    for (i32 index = 0; index < scene->display_list->special_count; ++index, ++special) {
        if (NuStrIStr(special->name, name) != NULL) {
            dest->scene = scene;
            dest->display_special = (NUMTX *)special;
            dest->special = NULL;
            ++dest;
            if (++count >= capacity)
                return count;
        }
    }
    if (flags != 0 && special_debug != 0)
        PCSpecialEmptyHandler_candidate();
    return count;
}
extern "C" i32 __cdecl NuSpecialFindMulti(NUGSCN *scene, NUSPECIALHANDLE *dest, char *name, i32 capacity, i32 flags) {
    i32 count = 0;
    if (scene == NULL) {
        PCSpecialEmptyHandler_candidate();
        return 0;
    }
    if (scene->display_list != NULL)
        return NuSpecialFindMultiDisplayPc(scene, dest, name, capacity, flags);
    NUSPECIAL_LEGACY_PC *special = scene->specials;
    for (i32 index = 0; index < scene->special_count; ++index, ++special) {
        if (NuStrIStr(special->name, name) != NULL) {
            dest->scene = scene;
            dest->special = (NUMTX *)special;
            dest->display_special = NULL;
            ++dest;
            if (++count >= capacity)
                return count;
        }
    }
    if (flags != 0 && special_debug != 0)
        PCSpecialEmptyHandler_candidate();
    return count;
}

extern "C" {
    i32 __cdecl Action_EngageObject(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                    i32 param_4, i32 param_5, f32 param_6) {
        (void)sys;
        if (packet == NULL || packet->owner == NULL || (GAMEOBJECT *)((APIOBJECT_PC *)packet->owner)->owner == NULL) {
            return 1;
        }

        GAMEOBJECT *object = (GAMEOBJECT *)((APIOBJECT_PC *)packet->owner)->owner;
        if (param_5 != 0) {
            packet->movement_instruction_parameter = idealgoalrange;
            processor->action_data_5 = engagefiretime;

            char *name = NULL;
            i32 nearest = 0;
            for (i32 index = 0; index < param_4; ++index) {
                char *value = NuStrIStr(params[index], (char *)"name");
                if (value != NULL) {
                    name = value + 5;
                } else if ((value = NuStrIStr(params[index], (char *)"fireinterval")) != NULL) {
                    processor->action_data_5 = AIParamToFloat(processor, value + 13);
                } else if (NuStrIStr(params[index], (char *)"nearest") != NULL) {
                    nearest = 1;
                } else if (NuStrIStr(params[index], (char *)"predictive") != NULL) {
                    processor->action_data_1 = 1;
                }
            }

            processor->action_timer = NuRandFloat() * processor->action_data_5;
            if (name != NULL) {
                if (nearest != 0) {
                    NUSPECIALHANDLE specials[50];
                    const i32 special_count = NuSpecialFindMulti(WORLD->current_gscn, specials, name, 50, 0);
                    if (special_count > 0) {
                        f32 nearest_distance = 1.0e9f;
                        NUVEC difference;
                        for (i32 index = 0; index < special_count; ++index) {
                            NUVEC *position = NuSpecialGetDrawPos(&specials[index]);
                            const f32 distance =
                                NuVecDistSqr(&((APIOBJECT_PC *)packet->owner)->position, position, &difference);
                            if (distance < nearest_distance) {
                                nearest_distance = distance;
                                processor->action_data_3 = position;
                            }
                        }
                    }
                } else {
                    NUSPECIALHANDLE special;
                    if (NuSpecialFind(WORLD->current_gscn, &special, name, 1) != 0) {
                        processor->action_data_3 = NuSpecialGetDrawPos(&special);
                    }
                }
            }

            if (processor->action_data_3 != NULL) {
                object->portal_flags |= 0x10000;
                object->quick_shoot_timer = NuRandFloat() * processor->action_data_5;
            }
        }

        NUVEC *target = static_cast<NUVEC *>(processor->action_data_3);
        if (target == NULL) {
            return 0;
        }

        packet->movement_look_target = target;
        processor->action_timer -= param_6;
        object->field_e4c = *target;
        object->portal_flags |= 0x8000;
        object->portal_flags |= 0x20;
        if (processor->action_timer > 0.0f) {
            return 0;
        }

        object->portal_flags = (object->portal_flags & ~0x20000u) | ((processor->action_data_1 & 1) << 17);
        processor->action_timer = (0.5f + NuRandFloat()) * processor->action_data_5;
        object->pad->buttons_pressed |= GAMEPAD_ACTION;
        return 0;
    }

    i32 __cdecl Action_FaceCamera(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                  i32 param_4, i32 param_5, f32 param_6) {
        (void)sys;
        if (packet == NULL || packet->owner == NULL || (GAMEOBJECT *)((APIOBJECT_PC *)packet->owner)->owner == NULL) {
            return 1;
        }

        GAMEOBJECT *object = (GAMEOBJECT *)((APIOBJECT_PC *)packet->owner)->owner;
        f32 completion_time = 0.0f;
        if (param_5 != 0) {
            f32 minimum_time = 0.0f;
            f32 maximum_time = 0.0f;
            for (i32 index = 0; index < param_4; ++index) {
                if (NuStrICmp("look_at_camera", params[index]) == 0) {
                    processor->action_data_1 = 1;
                } else {
                    char *value = NuStrIStr(params[index], (char *)"mintime");
                    if (value != NULL) {
                        minimum_time = AIParamToFloatEx(packet, processor, value + 8);
                    } else if ((value = NuStrIStr(params[index], (char *)"maxtime")) != NULL) {
                        maximum_time = AIParamToFloatEx(packet, processor, value + 8);
                    } else {
                        processor->action_timer = AIParamToFloat(processor, params[index]);
                    }
                }
            }
            if (processor->action_timer == 0.0f && maximum_time > minimum_time) {
                processor->action_timer = NuRandFloat() * (maximum_time - minimum_time) + minimum_time;
            }
        }

        NUMTX *camera = NuCameraGetMtx();
        if (camera != NULL) {
            packet->movement_look_target = (NUVEC *)&camera->m30;
            if (processor->action_data_1 != 0) {
                SetHeadTarget(object, object->ai_packet.movement_look_target, 6, 1.0f, 0.0f, 0.0f);
            }
        }
        if (processor->action_timer > completion_time) {
            processor->action_timer -= param_6;
            if (completion_time >= processor->action_timer) {
                processor->action_timer = 0.0f;
                return 1;
            }
        }
        return 0;
    }

    i32 __cdecl Action_CycleCharacter(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                      i32 param_4, i32 param_5, f32 param_6) {
        (void)sys;
        if (packet == NULL || packet->owner == NULL)
            return 1;

        GAMEOBJECT *object = (GAMEOBJECT *)((APIOBJECT_PC *)packet->owner)->owner;
        if (object == NULL || FreePlay == 0 || static_cast<i8>(object->field_280) == -1)
            return 1;

        i32 single_switch = 0;
        if (param_5 != 0) {
            processor->action_data_1 = static_cast<u8>(CHARCATEGORYCOUNT);
            processor->action_data_2 = static_cast<u8>(CHARCATEGORYCOUNT);

            for (i32 index = 0; index < param_4; ++index) {
                char *value = NuStrIStr(params[index], (char *)"time");
                if (value != NULL) {
                    processor->action_data_4 = AIParamToFloat(processor, value + 5);
                    continue;
                }

                value = NuStrIStr(params[index], (char *)"Category");
                if (value != NULL) {
                    const i32 category = CharCategory_FindByName(value + 9);
                    if (category != -1) {
                        if (processor->action_data_1 >= CHARCATEGORYCOUNT)
                            processor->action_data_1 = static_cast<u8>(category);
                        else
                            processor->action_data_2 = static_cast<u8>(category);
                        break;
                    }
                    continue;
                }

                value = NuStrIStr(params[index], (char *)"min_speed=");
                if (value != NULL) {
                    processor->action_data_5 = AIParamToFloat(processor, value + 10);
                    continue;
                }

                if (NuStrICmp(params[index], "SingleSwitch") == 0)
                    single_switch = 1;
            }
        }

        if (object->character_context == 0xb)
            return 0;

        const i32 first_category = processor->action_data_1;
        if (first_category < CHARCATEGORYCOUNT) {
            if (processor->action_data_5 == 0.0f ||
                ((APIOBJECT_PC *)packet->owner)->character_data->game_character->run_speed >=
                    processor->action_data_5) {
                if (CharCategory_IsCategory(object, first_category) != 0)
                    return 1;

                const i32 second_category = processor->action_data_2;
                if (second_category < CHARCATEGORYCOUNT && CharCategory_IsCategory(object, second_category) != 0)
                    return 1;
            }

            processor->action_timer += param_6;
            if (processor->action_timer > processor->action_data_4) {
                processor->action_timer = 0.0f;
                object->pad->buttons_pressed |= GAMEPAD_TOGGLERIGHT;
            }
            return 0;
        }

        if (processor->action_data_5 != 0.0f) {
            if (((APIOBJECT_PC *)packet->owner)->character_data->game_character->run_speed >= processor->action_data_5)
                return 1;
        } else {
            if (single_switch != 0)
                object->pad->buttons_pressed |= GAMEPAD_TOGGLERIGHT;
            return 1;
        }

        processor->action_timer += param_6;
        if (processor->action_timer > processor->action_data_4) {
            processor->action_timer = 0.0f;
            object->pad->buttons_pressed |= GAMEPAD_TOGGLERIGHT;
        }
        return 1;
    }

    i32 __cdecl Action_CnxHelper(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                 i32 param_4, i32 param_5, f32 param_6) {
        (void)packet;
        (void)param_6;
        if (param_5 == 0 || param_4 <= 0) {
            return 1;
        }

        AIPATH_PC *path = NULL;
        char *from = NULL;
        char *to = NULL;
        void *grapple = NULL;
        f32 jump_off_dy = 0.0f;
        bool both_ways = false;
        for (i32 index = 0; index < param_4; ++index) {
            char *value = NuStrIStr(params[index], (char *)"pathname=");
            if (value != NULL) {
                path = AISysFindPath(sys, value + NuStrLen("pathname="));
                continue;
            }
            if (NuStrIStr(params[index], (char *)"bothways") != NULL) {
                both_ways = true;
                continue;
            }
            value = NuStrIStr(params[index], (char *)"from=");
            if (value != NULL) {
                from = value + NuStrLen("from=");
                continue;
            }
            value = NuStrIStr(params[index], (char *)"to=");
            if (value != NULL) {
                to = value + NuStrLen("to=");
                continue;
            }
            value = NuStrIStr(params[index], (char *)"jump_off_dy=");
            if (value != NULL) {
                jump_off_dy = AIParamToFloat(processor, value + NuStrLen("jump_off_dy="));
                continue;
            }
            value = NuStrIStr(params[index], (char *)"grapple=");
            if (value != NULL) {
                GIZMO_PC *gizmo =
                    (GIZMO_PC *)GizmoFindByName(WORLD->gizmo_sys, grapple_gizmotype_id, value + NuStrLen("grapple="));
                if (gizmo != NULL) {
                    grapple = gizmo->object;
                }
            }
        }

        if (from != NULL && to != NULL) {
            i32 direction = 0;
            AIPATHCNX_PC *connection = static_cast<AIPATHCNX_PC *>(AIPAthFindPathCnx(sys, path, from, to, &direction));
            if (connection != NULL) {
                if (both_ways) {
                    direction = 0xff;
                }
                if (grapple != NULL) {
                    BULK_AI_HELPER_PC *helper = AIPathCnxHelperSys_AddHelper(
                        ((BULK_AI_WORLD_PC *)WORLD)->helper_system, connection, static_cast<u8>(direction), grapple, 1);
                    if (helper != NULL) {
                        helper->jump_off_dy = jump_off_dy;
                    }
                }
            }
        }
        return 1;
    }

    i32 __cdecl Action_EnableSock(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                  i32 param_4, i32 param_5, f32 param_6) {
        (void)sys;
        (void)packet;
        (void)param_6;
        if (param_5 != 0) {
            i32 sock_index = -1;
            i32 enabled = 1;
            for (i32 index = 0; index < param_4; ++index) {
                char *value = NuStrIStr(params[index], (char *)"ix=");
                if (value != NULL) {
                    sock_index = static_cast<i32>(AIParamToFloat(processor, value + 3));
                } else if ((value = NuStrIStr(params[index], (char *)"name=")) != NULL) {
                    SOCK_PC *sock = FindSock(WORLD->sock_sys, value + 5);
                    if (sock != NULL) {
                        sock_index = sock - WORLD->sock_sys->socks;
                    }
                } else if (NuStrIStr(params[index], (char *)"FALSE") != NULL) {
                    enabled = 0;
                }
            }
            if (enabled == 0) {
                SockOff(WORLD->sock_sys, sock_index);
            } else {
                SockOn(WORLD->sock_sys, sock_index);
            }
        }
        return 1;
    }

    i32 __cdecl Action_SetVisibility(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **params, i32 param_count,
                                     i32 first_time, float) {
        if (first_time == 0) {
            return 1;
        }

        NUSPECIALHANDLE special = {};
        i32 visible = 1;
        for (i32 index = 0; index < param_count; ++index) {
            char *value = NuStrIStr(params[index], (char *)"name");
            if (value != NULL) {
                NuSpecialFind(WORLD->current_gscn, &special, value + 5, 1);
            } else if (NuStrIStr(params[index], (char *)"FALSE") != NULL) {
                visible = 0;
            }
        }
        if (NuSpecialExistsFn(&special) != 0) {
            NuSpecialSetVisibility(&special, visible);
        }
        return 1;
    }
}

extern "C" {
    u32 __cdecl AIPathParseBatmanFlagPc(char *name) {
        if (NuStrICmp(name, "JUMP") == 0)
            return 1;
        if (NuStrICmp(name, "COMBATROLL") == 0)
            return 2;
        if (NuStrICmp(name, "BATGLIDE") == 0)
            return 4;
        if (NuStrICmp(name, "GRAPPLE") == 0)
            return 8;
        if (NuStrICmp(name, "USEHATCH") == 0)
            return 0x10;
        if (NuStrICmp(name, "HOVERTUBE") == 0)
            return 0x40;
        if (NuStrICmp(name, "FORGOODIES") == 0)
            return 0x80;
        if (NuStrICmp(name, "FORBADDIES") == 0)
            return 0x100;
        if (NuStrICmp(name, "OBSTACLE") == 0)
            return 0x20000000;
        if (NuStrICmp(name, "JUMP_NOW") == 0)
            return 0x400;
        if (NuStrICmp(name, "DONT_JUMP_NOW") == 0)
            return 0x800;
        return 0;
    }
    u32 __cdecl AIPathParseIndyFlagPc(char *name) {
        if (NuStrICmp(name, "DBLJUMP") == 0)
            return 2;
        if (NuStrICmp(name, "BIGJUMP") == 0)
            return 4;
        if (NuStrICmp(name, "JUMP") == 0)
            return 1;
        if (NuStrICmp(name, "GRAPPLE") == 0)
            return 8;
        if (NuStrICmp(name, "CLIMB") == 0)
            return 0x10;
        if (NuStrICmp(name, "NO_DEST_CHECK") == 0)
            return 0x20;
        if (NuStrICmp(name, "JUMP_NOW") == 0)
            return 0x40;
        if (NuStrICmp(name, "DONT_JUMP_NOW") == 0)
            return 0x80;
        if (NuStrICmp(name, "FORGOODIES") == 0)
            return 0x100;
        if (NuStrICmp(name, "FORBADDIES") == 0)
            return 0x200;
        if (NuStrICmp(name, "OBSTACLE") == 0)
            return 0x20000000;
        if (NuStrICmp(name, "WHIPSWING") == 0)
            return 0x400;
        return 0;
    }
    u32 __cdecl AIPathParseStarWarsFlagPc(char *name) {
        if (NuStrICmp(name, "DBLJUMP") == 0)
            return 2;
        if (NuStrICmp(name, "R2D2GLIDE") == 0)
            return 4;
        if (NuStrICmp(name, "ZIPUP") == 0)
            return 8;
        if (NuStrICmp(name, "USEHATCH") == 0)
            return 0x10;
        if (NuStrICmp(name, "JARJARJUMP") == 0)
            return 0x20;
        if (NuStrICmp(name, "HOVERTUBE") == 0)
            return 0x40;
        if (NuStrICmp(name, "SWAMP") == 0)
            return 0x4000;
        if (NuStrICmp(name, "TAKEOVER") == 0)
            return 0x8000;
        if (NuStrICmp(name, "VEHICLE") == 0)
            return 0x10000;
        if (NuStrICmp(name, "PARTY") == 0)
            return 0x20000;
        if (NuStrICmp(name, "FORGOODIES") == 0)
            return 0x80;
        if (NuStrICmp(name, "FORBADDIES") == 0)
            return 0x100;
        if (NuStrICmp(name, "OBSTACLE") == 0)
            return 0x20000000;
        if (NuStrICmp(name, "JUMP_NOW") == 0)
            return 0x400;
        if (NuStrICmp(name, "DONT_JUMP_NOW") == 0)
            return 0x800;
        if (NuStrICmp(name, "BLOCKAGE") == 0)
            return 0x40000;
        if (NuStrICmp(name, "JUMP") == 0)
            return 1;
        if (NuStrICmp(name, "DONTTOGGLE") == 0)
            return 0x80000;
        if (NuStrICmp(name, "FULLTERRAIN") == 0)
            return 0x100000;
        return 0;
    }
}
extern "C" u32 LEGO_AIPATHCNX_BIGJUMP, LEGO_AIPATHCNX_REQUIRESPERMISSION, LEGO_AIPATHCNX_NO_DESTINATION_CHECK;
extern "C" u32 LEGO_AIPATHCNX_JUMP_NOW, LEGO_AIPATHCNX_DONT_JUMP_NOW;
static u32 ParseAIPathCnxFlagPc(char *name) {
    u32 flag;
    if (StartupGameFlag0 != 0)
        flag = AIPathParseIndyFlagPc(name);
    else if (StartupGameFlag1 != 0)
        flag = AIPathParseStarWarsFlagPc(name);
    else
        flag = AIPathParseBatmanFlagPc(name);
    if (flag != 0)
        return flag;
    if (NuStrICmp(name, "BLOCK") == 0)
        return 0x80000000;
    if (NuStrICmp(name, "BIGJUMP") == 0)
        return LEGO_AIPATHCNX_BIGJUMP;
    if (NuStrICmp(name, "REQUIRESPERMISSION") == 0)
        return LEGO_AIPATHCNX_REQUIRESPERMISSION;
    if (NuStrICmp(name, "NO_DESTINATION_CHECK") == 0)
        return LEGO_AIPATHCNX_NO_DESTINATION_CHECK;
    return 0;
}

extern "C" {
    i32 __cdecl Action_SetPathCnxFlag(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                      i32 param_count, i32 first_time, f32 param_6) {
        (void)processor;
        (void)packet;
        (void)param_6;
        if (sys == NULL || sys->path_sys == NULL || sys->path_sys->path_count == 0 || first_time == 0 ||
            param_count < 1) {
            return 1;
        }

        char *from = NULL;
        char *to = NULL;
        u32 add_flags = 0;
        u32 remove_flags = 0;
        bool set = true;
        bool both_ways = false;
        for (i32 index = 0; index < param_count; ++index) {
            char *match = NuStrIStr(params[index], (char *)"from=");
            if (match != NULL) {
                from = match + 5;
                continue;
            }
            match = NuStrIStr(params[index], (char *)"to=");
            if (match != NULL) {
                to = match + 3;
                continue;
            }
            const u32 flag = ParseAIPathCnxFlagPc(params[index]);
            if (flag != 0) {
                add_flags |= flag;
                if (flag == static_cast<u32>(LEGO_AIPATHCNX_JUMP_NOW)) {
                    remove_flags |= static_cast<u32>(LEGO_AIPATHCNX_DONT_JUMP_NOW);
                } else if (flag == static_cast<u32>(LEGO_AIPATHCNX_DONT_JUMP_NOW)) {
                    remove_flags |= static_cast<u32>(LEGO_AIPATHCNX_JUMP_NOW);
                } else if (flag == 0x20000000u) {
                    both_ways = true;
                }
                continue;
            }
            if (NuStrICmp(params[index], "bothways") == 0) {
                both_ways = true;
            } else if (NuStrICmp(params[index], "FALSE") == 0) {
                set = false;
            }
        }

        if (from != NULL && to != NULL) {
            i32 direction = 0;
            AIPATHCNX_PC *connection =
                static_cast<AIPATHCNX_PC *>(AIPAthFindPathCnx(sys, sys->path_sys->active_path, from, to, &direction));
            if (connection != NULL) {
                if (set) {
                    connection->traversal_flags[direction] =
                        (connection->traversal_flags[direction] | add_flags) & ~remove_flags;
                    if (both_ways) {
                        connection->traversal_flags[direction ^ 1] =
                            (connection->traversal_flags[direction ^ 1] | add_flags) & ~remove_flags;
                    }
                } else {
                    connection->traversal_flags[direction] &= ~add_flags;
                    if (both_ways) {
                        connection->traversal_flags[direction ^ 1] &= ~add_flags;
                    }
                }
            }
        }
        return 1;
    }
}

extern "C" {
    void *__cdecl CutScene_FindInst(CUTSYS_PC *system, char *name) {
        if (system != NULL) {
            for (i32 index = 0; index < system->count; ++index)
                if (NuStrICmp(system->records[index]->name, name) == 0)
                    return system->records[index]->instance;
        }
        return NULL;
    }
}

typedef struct bulk_ai_lever_pc_s {
    u8 unknown_00[0x40];
    NUVEC floor_position;
    u8 unknown_4c[0x1c];
    NUVEC position;
    u8 unknown_74[4];
    f32 pull_progress;
    u8 unknown_7c[0x12];
    u16 animation_frame;
    u8 unknown_90[4];
    u16 flags;
} BULK_AI_LEVER_PC;
typedef struct bulk_ai_panel_pc_s {
    u8 unknown_00[0x50];
    NUVEC position;
    u8 unknown_5c[5];
    u8 model_variant;
    u8 flags;
    u8 unknown_63;
    NUVEC floor_position;
} BULK_AI_PANEL_PC;
typedef struct bulk_ai_techno_pc_s {
    u8 unknown_00[0x10];
    NUVEC position;
    NUVEC ground_position;
    u8 unknown_28[0x14];
    u8 flags;
} BULK_AI_TECHNO_PC;
STATIC_ASSERT(offsetof(BULK_AI_LEVER_PC, floor_position) == 0x40);
STATIC_ASSERT(offsetof(BULK_AI_LEVER_PC, position) == 0x68);
STATIC_ASSERT(offsetof(BULK_AI_LEVER_PC, pull_progress) == 0x78);
STATIC_ASSERT(offsetof(BULK_AI_LEVER_PC, animation_frame) == 0x8e);
STATIC_ASSERT(offsetof(BULK_AI_LEVER_PC, flags) == 0x94);
STATIC_ASSERT(offsetof(BULK_AI_PANEL_PC, position) == 0x50);
STATIC_ASSERT(offsetof(BULK_AI_PANEL_PC, model_variant) == 0x61);
STATIC_ASSERT(offsetof(BULK_AI_PANEL_PC, flags) == 0x62);
STATIC_ASSERT(offsetof(BULK_AI_PANEL_PC, floor_position) == 0x64);
STATIC_ASSERT(offsetof(BULK_AI_TECHNO_PC, position) == 0x10);
STATIC_ASSERT(offsetof(BULK_AI_TECHNO_PC, ground_position) == 0x1c);
STATIC_ASSERT(offsetof(BULK_AI_TECHNO_PC, flags) == 0x3c);
typedef struct bulk_ai_full_object_pc_s {
    APIOBJECT_PC object;
    u8 trailing[0x2b8 - sizeof(APIOBJECT_PC)];
} BULK_AI_FULL_OBJECT_PC;
STATIC_ASSERT(sizeof(BULK_AI_FULL_OBJECT_PC) == 0x2b8);
typedef struct bulk_ai_full_packet_pc_s {
    AIPACKET_PC packet;
    u8 trailing[0x210 - sizeof(AIPACKET_PC)];
} BULK_AI_FULL_PACKET_PC;
STATIC_ASSERT(sizeof(BULK_AI_FULL_PACKET_PC) == 0x210);
extern "C" {
    void __cdecl AISysGetPathPos2(AISYS_PC *system, NUVEC *position, AIPATHINFO_PC *path_info, NUVEC *path_position,
                                  AIPATH_PC *, i32) {
        if (position != NULL && system != NULL && path_info != NULL) {
            BULK_AI_FULL_OBJECT_PC full_object;
            BULK_AI_FULL_PACKET_PC full_packet;
            AIPACKET_PC &packet = full_packet.packet;
            memset(&full_object, 0, sizeof(full_object));
            memset(&full_packet, 0, sizeof(full_packet));
            APIOBJECT_PC *object = &full_object.object;
            object->ai = &packet;
            object->position = *position;
            packet.terrain_origin = *position;
            AISysGetCharacterPathPos(system, object, &packet, 0xff, 1);
            *path_info = packet.path_info;
            if (path_position != NULL)
                *path_position = packet.last_path_position;
        }
    }
    i32 __cdecl GizPanel_CanUsePanel(GAMEOBJECT *object, BULK_AI_PANEL_PC *panel) {
        if (panel == NULL || object == NULL)
            return 0;
        if (SuperWeirdo(object) != 0)
            return 1;
        switch (panel->model_variant) {
            case 0:
                return (object->character_data->model_flags & 0x40) != 0 ||
                       (object->character_data->model_flags & 0x1000010) == 0x1000010;
            case 1:
                return (object->character_data->model_flags & 0x20) != 0 ||
                       (object->character_data->model_flags & 0x1000010) == 0x1000010;
            case 2:
                return (object->character_data->model_flags & 0x1000000) != 0 || object->helmet == 6;
            case 3:
                if (object->helmet == 5 || (object->character_data->game_character->flags_090 & 0x40000) != 0)
                    return 1;
                if (GCDataList[(i16)object->id].weapon_action == 8 || GCDataList[(i16)object->id].weapon_action == 1)
                    return 1;
                return object->id == FreePlayId007f199cPc || object->id == id_THEEMPEROR ||
                       object->id == FreePlayId007f1a4cPc || object->id == FreePlayId007f1a48Pc ||
                       object->id == FreePlayId007f1a44Pc;
            default:
                return 0;
        }
    }
    i32 __cdecl GizTechno_CanUseTechno(GAMEOBJECT *, BULK_AI_TECHNO_PC *) {
        return 1;
    }
    i32 __cdecl Action_PullLever(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                 i32 param_count, i32 first_time, f32 elapsed) {
        i32 instant = 0;
        if (first_time != 0) {
            for (i32 index = 0; index < param_count; ++index) {
                char *name = NuStrIStr(params[index], (char *)"lever=");
                if (name != NULL) {
                    GIZMO_PC *gizmo = (GIZMO_PC *)GizmoFindByName(WORLD->gizmo_sys, lever_gizmotype_id, name + 6);
                    if (gizmo == NULL || gizmo->object == NULL)
                        continue;
                    BULK_AI_LEVER_PC *lever = static_cast<BULK_AI_LEVER_PC *>(gizmo->object);
                    processor->action_data_3 = lever;
                    if ((lever->flags & 2))
                        continue;
                    processor->action_pos = lever->floor_position;
                    f32 height = GameShadow(NULL, &processor->action_pos, 5.0f, -1);
                    if (height != 2000000.0f)
                        processor->action_pos.y = height;
                    AISysGetPathPos2(system, &processor->action_pos, &processor->path_info, &processor->action_pos,
                                     NULL, 0xff);
                } else if (NuStrICmp(params[index], "instant") == 0) {
                    instant = 1;
                }
            }
        }
        BULK_AI_LEVER_PC *lever = static_cast<BULK_AI_LEVER_PC *>(processor->action_data_3);
        if (lever == NULL)
            return 1;
        if (instant != 0) {
            lever->flags |= 2;
            lever->animation_frame = 0x8000;
            lever->pull_progress = 1.0f;
            return 1;
        }
        if (packet == NULL || packet->owner == NULL || (GAMEOBJECT *)((APIOBJECT_PC *)packet->owner)->owner == NULL)
            return 1;
        GAMEOBJECT *object = (GAMEOBJECT *)((APIOBJECT_PC *)packet->owner)->owner;
        AIMoveInstruction(packet, &processor->action_pos, 0.0f, &processor->path_info, 1, 0.0f);
        if ((object->flags_ef0 & 0x800000) != 0) {
            f32 distance = NuVecDistSqr(&packet->terrain_origin, &processor->action_pos, NULL);
            if (distance < ai_moveradius * ai_moveradius) {
                packet->movement_look_target = &lever->position;
                object->pad->buttons_pressed |= GAMEPAD_SPECIAL;
            }
        } else if (FreePlay != 0) {
            processor->action_timer -= elapsed;
            if (processor->action_timer < 0.0f) {
                processor->action_timer = 0.5f;
                object->pad->buttons_pressed |= GAMEPAD_TOGGLERIGHT;
            }
        }
        return object->character_context == 0x4a && object->grapple == lever;
    }

    i32 __cdecl Action_UsePanel(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                i32 param_count, i32 first_time, f32 elapsed) {
        if (packet == NULL || packet->owner == NULL || (GAMEOBJECT *)((APIOBJECT_PC *)packet->owner)->owner == NULL)
            return 1;
        GAMEOBJECT *object = (GAMEOBJECT *)((APIOBJECT_PC *)packet->owner)->owner;
        if (first_time != 0) {
            for (i32 index = 0; index < param_count; ++index) {
                char *name = NuStrIStr(params[index], (char *)"name=");
                if (name == NULL)
                    continue;
                GIZMO_PC *gizmo = (GIZMO_PC *)GizmoFindByName(WORLD->gizmo_sys, gizpanel_gizmotype_id, name + 5);
                if (gizmo == NULL || gizmo->object == NULL)
                    continue;
                BULK_AI_PANEL_PC *panel = static_cast<BULK_AI_PANEL_PC *>(gizmo->object);
                processor->action_data_3 = panel;
                if ((panel->flags & 2) != 0)
                    continue;
                processor->action_pos = panel->floor_position;
                f32 height = GameShadow(NULL, &processor->action_pos, 5.0f, -1);
                if (height != 2000000.0f)
                    processor->action_pos.y = height;
                AISysGetPathPos2(system, &processor->action_pos, &processor->path_info, &processor->action_pos, NULL,
                                 0xff);
            }
        }
        BULK_AI_PANEL_PC *panel = static_cast<BULK_AI_PANEL_PC *>(processor->action_data_3);
        if (panel == NULL || (panel->flags & 2) != 0)
            return 1;
        AIMoveInstruction(packet, &processor->action_pos, 0.0f, &processor->path_info, 1, 0.0f);
        if (GizPanel_CanUsePanel(object, panel) != 0) {
            f32 distance = NuVecDistSqr(&packet->terrain_origin, &processor->action_pos, NULL);
            if (distance < ai_moveradius * ai_moveradius) {
                packet->movement_look_target = &panel->position;
                object->pad->buttons_pressed |= GAMEPAD_SPECIAL;
            }
        } else if (FreePlay != 0) {
            processor->action_timer -= elapsed;
            if (processor->action_timer < 0.0f) {
                processor->action_timer = 0.5f;
                object->pad->buttons_pressed |= GAMEPAD_TOGGLERIGHT;
            }
        }
        return object->character_context == 0x0b && object->grapple == panel;
    }

    i32 __cdecl Action_UseTechno(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                 i32 param_count, i32 first_time, f32 elapsed) {
        if (packet == NULL || packet->owner == NULL || (GAMEOBJECT *)((APIOBJECT_PC *)packet->owner)->owner == NULL)
            return 1;
        GAMEOBJECT *object = (GAMEOBJECT *)((APIOBJECT_PC *)packet->owner)->owner;
        if (first_time != 0) {
            for (i32 index = 0; index < param_count; ++index) {
                char *name = NuStrIStr(params[index], (char *)"name=");
                if (name == NULL)
                    continue;
                GIZMO_PC *gizmo = (GIZMO_PC *)GizmoFindByName(WORLD->gizmo_sys, techno_gizmotype_id, name + 5);
                if (gizmo == NULL || gizmo->object == NULL)
                    continue;
                BULK_AI_TECHNO_PC *techno = static_cast<BULK_AI_TECHNO_PC *>(gizmo->object);
                processor->action_data_3 = techno;
                processor->action_pos = techno->ground_position;
                f32 height = GameShadow(NULL, &processor->action_pos, 5.0f, -1);
                if (height != 2000000.0f)
                    processor->action_pos.y = height;
                AISysGetPathPos2(system, &processor->action_pos, &processor->path_info, &processor->action_pos, NULL,
                                 0xff);
            }
        }
        BULK_AI_TECHNO_PC *techno = static_cast<BULK_AI_TECHNO_PC *>(processor->action_data_3);
        if (techno == NULL || (techno->flags & 1) == 0 || (techno->flags & 2) == 0 || (techno->flags & 8) != 0)
            return 1;
        AIMoveInstruction(packet, &processor->action_pos, 0.0f, &processor->path_info, 1, 0.0f);
        if (GizTechno_CanUseTechno(object, techno) == 0) {
            if (FreePlay != 0) {
                processor->action_timer -= elapsed;
                if (processor->action_timer < 0.0f) {
                    processor->action_timer = 0.5f;
                    object->pad->buttons_pressed |= GAMEPAD_TOGGLERIGHT;
                }
            }
        } else {
            f32 distance = NuVecDistSqr(&packet->terrain_origin, &processor->action_pos, NULL);
            if (distance < ai_moveradius * ai_moveradius) {
                packet->movement_look_target = &techno->position;
                GameObjectSetCanUse(object, techno, 2, 1, 0.0f);
            }
        }
        return 0;
    }
}
