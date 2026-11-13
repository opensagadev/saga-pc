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

i32 __cdecl CanPullLevers(i32 id) {
    u32 flags = CDataList[id].model_flags;
    if ((flags & 0x1000010) == 0x1000010)
        return 0;
    return (flags & 0x40088) != 0;
}
