#include "saga.h"
typedef struct lever_trigger_flags_pc_s {
    u8 unknown000[0x94];
    u8 flags;
} LEVER_TRIGGER_FLAGS_PC;
STATIC_ASSERT(offsetof(LEVER_TRIGGER_FLAGS_PC, flags) == 0x94);
i32 __cdecl Lever_BeingPulled(void *lever) {
    return (((LEVER_TRIGGER_FLAGS_PC *)lever)->flags >> 1) & 1;
}
