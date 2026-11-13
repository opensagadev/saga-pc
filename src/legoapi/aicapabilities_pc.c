#include "saga.h"

typedef struct character_ai_capabilities_pc_s {
    u8 unknown_00[0x44];
    u32 path_capabilities;
} CHARACTER_AI_CAPABILITIES_PC;
STATIC_ASSERT(offsetof(CHARACTER_AI_CAPABILITIES_PC, path_capabilities) == 0x44);
typedef struct suit_ai_capabilities_pc_s {
    u8 unknown_00[0x10];
    u32 path_capabilities;
} SUIT_AI_CAPABILITIES_PC;
STATIC_ASSERT(offsetof(SUIT_AI_CAPABILITIES_PC, path_capabilities) == 0x10);

void __cdecl PlayerInitCapabilitiesVariant0Pc(GAMEOBJECT *object) {
    object->ai_packet.capabilities = 0x20000000;
    if ((object->flags_1f8 & 1) != 0)
        object->ai_packet.capabilities |= 0x200;
    else if ((object->flags_1f8 & 4) == 0)
        object->ai_packet.capabilities |= 0x100;
    if (object->character_model->action_animations[6] != NULL)
        object->ai_packet.capabilities |= 3;
    if ((object->character_data->model_flags & 0x200000) == 0)
        object->ai_packet.capabilities |= 4;
    if ((object->character_data->model_flags & 0x100000) != 0)
        object->ai_packet.capabilities |= 8;
    if ((u16)object->id == (u16)VehicleBoltId7f17d0Pc)
        object->ai_packet.capabilities |= 0x400;
    if (object->character_model->action_animations[0xcd] != NULL)
        object->ai_packet.capabilities |= 0x10;
}

i32 __cdecl CanPullLevers(i32 id) {
    u32 flags = CDataList[id].model_flags;
    if ((flags & 0x1000010) == 0x1000010)
        return 0;
    return (flags & 0x40088) != 0;
}
