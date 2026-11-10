#include "saga.h"

extern "C" i32 special_debug;
extern "C" void __cdecl PCSpecialEmptyHandler_candidate(void);
extern "C" {
    AI_HELPER_SLOT_PC *__cdecl AIPathCnxHelperSys_AddHelper(AI_HELPER_SYSTEM_PC *system, AIPATHCNX_PC *connection,
                                                            u8 direction, void *target, u8 type) {
        if (system == NULL || system->count >= system->capacity || connection == NULL || target == NULL)
            return NULL;
        i32 count = system->count;
        AI_HELPER_SLOT_PC *helper = system->helpers;
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
        if (CharCategory == NULL)
            return -1;
        for (i32 index = 0; index < CHARCATEGORYCOUNT; ++index)
            if (NuStrICmp(CharCategory[index].name, name) == 0)
                return index;
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
    void __cdecl SockOn(SOCKSYS_PC *sys, i32 index);
    void __cdecl SockOff(SOCKSYS_PC *sys, i32 index);
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

extern "C" {
    static i32 Ledges_Load(void *world_info, void *) {
        WORLDINFO *world = static_cast<WORLDINFO *>(world_info);
        if (world->ledge_count != 0)
            return 0;
        i32 version = EdFileReadInt();
        world->ledge_count = EdFileReadInt();
        for (i32 i = 0; i < world->ledge_count; ++i) {
            LEDGE_s *ledge = &static_cast<LEDGE_s *>(world->ledges)[i];
            EdFileRead(ledge->name, 8);
            EdFileReadNuVec(&ledge->position);
            ledge->y_rotation = EdFileReadShort();
            ledge->type_code = EdFileReadChar();
            if (version > 1) {
                ledge->field_0x1c = EdFileReadShort();
                ledge->field_0x1e = EdFileReadShort();
                if (version == 2)
                    ledge->flags = 0;
                else
                    ledge->flags = EdFileReadUnsignedChar();
            } else {
                ledge->field_0x1c = -1;
                ledge->field_0x1e = -1;
                ledge->flags = 0;
            }
        }
        return 1;
    }
}

extern "C" {
    void *__cdecl CutScene_FindInst(CUTSYS_PC *system, char *name) {
        if (system == NULL)
            return NULL;
        for (i32 index = 0; index < system->count; ++index)
            if (NuStrICmp(system->records[index]->name, name) == 0)
                return system->records[index]->instance;
        return NULL;
    }
}
typedef struct bulk_ai_panel_pc_s {
    u8 unknown_00[0x50];
    NUVEC position;
    u8 unknown_5c[5];
    u8 model_variant;
    u8 flags;
    u8 unknown_63;
    NUVEC floor_position;
} BULK_AI_PANEL_PC;
STATIC_ASSERT(offsetof(BULK_AI_PANEL_PC, position) == 0x50);
STATIC_ASSERT(offsetof(BULK_AI_PANEL_PC, model_variant) == 0x61);
STATIC_ASSERT(offsetof(BULK_AI_PANEL_PC, flags) == 0x62);
STATIC_ASSERT(offsetof(BULK_AI_PANEL_PC, floor_position) == 0x64);
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
}

STATIC_ASSERT(offsetof(ZIPUP_s, flags) == 0x68);
STATIC_ASSERT(offsetof(ZIPUP_s, runtime_flags) == 0x69);
STATIC_ASSERT(offsetof(ZIPUP_s, lower_position) == 0x10);
STATIC_ASSERT(offsetof(ZIPUP_s, hook_origin) == 0x1c);
STATIC_ASSERT(offsetof(ZIPUP_s, upper_position) == 0x28);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, run_speed) == 0x1c);
extern "C" void __cdecl StartBallooning(GAMEOBJECT *, i32);

extern "C" struct AIPATHCNXCONTROLLER_s *__cdecl AIPathCnxControllerCreate(AI_CONTROL_SYSTEM_PC *, AISYS_PC *,
                                                                           AIPATH_PC *, char *, char *, i32, char *,
                                                                           i32, i32);
