#include "saga.h"
#include <string.h>

typedef struct NuKeyboardProfile_PC {
    u8 unknown_00_to_0b[0x0c];          // 0x000
    i32 device_mode_a;                  // 0x00c
    NuKeyboardDevice_PC *device;        // 0x010
    i32 device_mode_b;                  // 0x014
    u8 unknown_18_to_23[0x0c];          // 0x018
    u16 default_key;                    // 0x024
    u8 unknown_26_to_35[0x10];          // 0x026
    u8 active_36;                       // 0x036
    u8 unknown_37_to_38[2];             // 0x037
    u8 active_39;                       // 0x039
    char name[0x40];                    // 0x03a
    u8 unknown_7a_to_17b[0x17c - 0x7a]; // 0x07a
} NuKeyboardProfile_PC;                 // size 0x17c

typedef struct NuKeyboardProfileLayout_PC {
    u8 unknown_0000_to_5cff[0x5d00];  // 0x0000
    NuKeyboardProfile_PC profiles[4]; // 0x5d00
} NuKeyboardProfileLayout_PC;         // size 0x62f0

typedef struct NuKeyboardActionRecord_PC {
    u8 unknown_00_to_27[0x28];          // 0x000
    u8 active;                          // 0x028
    u8 unknown_29_to_17b[0x17c - 0x29]; // 0x029
} NuKeyboardActionRecord_PC;            // size 0x17c

typedef struct NuKeyboardActionState_PC {
    u8 unknown_0000_to_4e33[0x4e34];          // 0x0000
    NuKeyboardActionRecord_PC records[14];    // 0x4e34
    u8 unknown_62fc_to_6607[0x6608 - 0x62fc]; // 0x62fc
    i32 fallback_action;                      // 0x6608
} NuKeyboardActionState_PC;                   // size 0x660c

void PcInput_EnableKeyboards(const i16 *keys, i32 count) {
    NuKeyboardActionState_PC *state;
    i32 i;
    i32 recognized;

    state = (NuKeyboardActionState_PC *)nu_input_state;
    recognized = 0;
    for (i = 0; i < count; ++i) {
        i32 key;
        key = keys[i];
        if ((u32)(key - 10) <= 3) {
            recognized = 1;
            state->records[key].active = 1;
        }
    }
    if (!recognized) {
        state->records[state->fallback_action != 0 ? 11 : 10].active = 1;
    }
}
