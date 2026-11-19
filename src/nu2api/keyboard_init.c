#include "nu2api/PCInput.h"
#include <string.h>

/* C view of PC_INPUT_RUNTIME (a C++ class): devices[].action_keys_active and captured_port. */
typedef struct NuKeyboardActionRecord { // size 0x17c
    u8 unknown_00_to_27[0x28];          // 0x000
    u8 active;                          // 0x028
    u8 unknown_29_to_17b[0x17c - 0x29]; // 0x029
} NuKeyboardActionRecord;

typedef struct NuKeyboardActionState {        // size 0x660c
    u8 unknown_0000_to_4e33[0x4e34];          // 0x0000
    NuKeyboardActionRecord records[14];       // 0x4e34
    u8 unknown_62fc_to_6607[0x6608 - 0x62fc]; // 0x62fc
    i32 fallback_action;                      // 0x6608
} NuKeyboardActionState;

void PcInput_EnableKeyboards(const i16 *keys, i32 count) {
    NuKeyboardActionState *state;
    i32 i;
    i32 recognized;

    state = (NuKeyboardActionState *)nu_input_state;
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
