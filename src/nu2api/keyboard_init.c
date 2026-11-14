#include "saga.h"
#include <string.h>

typedef struct NuKeyboardInitKeyRecord_PC {
    i8 unknown_00;            // 0x0
    i8 unknown_01;            // 0x1
    i8 unknown_02;            // 0x2
    u8 unknown_03;            // 0x3
    u16 unknown_04;           // 0x4
} NuKeyboardInitKeyRecord_PC; // size 0x6

typedef struct NuKeyboardInitTriplet_PC {
    u8 unknown_00;          // 0x0
    u8 unknown_01;          // 0x1
    u8 unknown_02;          // 0x2
    u8 unknown_03_to_09[7]; // 0x3
} NuKeyboardInitTriplet_PC; // size 0xa

typedef struct NuKeyboardInitLayout_PC {
    u8 cleared_0000_to_26ff[0x2700];                    // 0x0000
    u8 cleared_2700_to_4dff[0x2700];                    // 0x2700
    i32 unknown_4e00;                                   // 0x4e00
    i32 unknown_4e04;                                   // 0x4e04
    i32 unknown_4e08;                                   // 0x4e08
    i32 unknown_4e0c;                                   // 0x4e0c
    u8 keyboard_polled;                                 // 0x4e10
    u8 unknown_4e11;                                    // 0x4e11
    u8 unknown_4e12;                                    // 0x4e12
    u8 unknown_4e13;                                    // 0x4e13
    u8 unknown_4e14_to_4e33[0x20];                      // 0x4e14
    u8 cleared_4e34_to_62fb[0x14c8];                    // 0x4e34
    u8 cleared_62fc_to_6343[0x48];                      // 0x62fc
    u8 unknown_6344;                                    // 0x6344
    u8 unknown_6345_to_6352[14];                        // 0x6345
    u8 unknown_6353;                                    // 0x6353
    i32 unknown_6354;                                   // 0x6354
    u8 cleared_6358_to_6557[0x200];                     // 0x6358
    u8 unknown_6558_to_6567[0x10];                      // 0x6558
    i32 unknown_6568;                                   // 0x6568
    i32 unknown_656c;                                   // 0x656c
    f32 unknown_6570;                                   // 0x6570
    u8 unknown_6574_to_6575[2];                         // 0x6574
    NuKeyboardInitTriplet_PC unknown_6576_triplets[14]; // 0x6576
    u8 unknown_6602_to_6603[2];                         // 0x6602
    i32 unknown_6604;                                   // 0x6604
    i32 unknown_6608;                                   // 0x6608
    NuKeyboardInitTriplet_PC unknown_660c_triplets[14]; // 0x660c
    u8 unknown_6698_to_6719[0x82];                      // 0x6698
    u8 unknown_671a;                                    // 0x671a
    u8 unknown_671b;                                    // 0x671b
    u8 cleared_671c_to_67df[0xc4];                      // 0x671c
    u8 unknown_67e0;                                    // 0x67e0
    u8 unknown_67e1;                                    // 0x67e1
    u8 unknown_67e2_to_67e3[2];                         // 0x67e2
    i32 unknown_67e4;                                   // 0x67e4
    i32 unknown_67e8;                                   // 0x67e8
    i32 unknown_67ec;                                   // 0x67ec
    NuKeyboardInitKeyRecord_PC key_records[32];         // 0x67f0
    u8 *keyboard_state;                                 // 0x68b0
} NuKeyboardInitLayout_PC;                              // size 0x68b4

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

extern const char PCKeyboardMouse1Name[15];
extern const char PCKeyboardMouse2Name[16];
extern const char PCKeyboard2Name[10];
extern const char PCKeyboard1Name[10];

void __fastcall NuKeyboardInitializeDefaultRecords_candidate(NuKeyboardState_PC *state);

void __fastcall NuKeyboardResetInputRecords(NuKeyboardState_PC *state) {
    NuKeyboardInitLayout_PC *layout = (NuKeyboardInitLayout_PC *)state;
    i32 i;

    for (i = 0; i < 32; ++i) {
        layout->key_records[i].unknown_00 = 0;
        layout->key_records[i].unknown_02 = -1;
        layout->key_records[i].unknown_01 = -1;
        layout->key_records[i].unknown_04 = 0;
    }
    layout->unknown_67e4 = 0;
}

void __fastcall fun_006d4e20(NuKeyboardState_PC *state, i32 unused_edx, i32 value) {
    NuKeyboardInitLayout_PC *layout = (NuKeyboardInitLayout_PC *)state;

    layout->unknown_6354 = value;
}

i32 __fastcall NuKeyboardGetInputRecordCount(NuKeyboardState_PC *state, i32 unused_edx) {
    NuKeyboardInitLayout_PC *layout = (NuKeyboardInitLayout_PC *)state;

    return layout->unknown_67e4;
}

i32 __fastcall fun_006d4e40(NuKeyboardState_PC *state, i32 unused_edx) {
    NuKeyboardInitLayout_PC *layout = (NuKeyboardInitLayout_PC *)state;

    return layout->unknown_67e8;
}

i32 __fastcall fun_006d4e50(NuKeyboardState_PC *state, i32 unused_edx) {
    NuKeyboardInitLayout_PC *layout = (NuKeyboardInitLayout_PC *)state;

    return layout->unknown_67ec;
}

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

void __cdecl PcInput_EnableKeyboards(const i16 *keys, i32 count) {
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

void __fastcall NuKeyboardInsertInputRecord(NuKeyboardState_PC *state, i32 unused_edx, i32 index, i32 field_00,
                                            i32 field_01, i32 field_02, u16 field_04) {
    NuKeyboardInitLayout_PC *layout;
    i32 i;

    if ((u32)index > 31) {
        return;
    }

    layout = (NuKeyboardInitLayout_PC *)state;
    for (i = 0; i < 32; ++i) {
        if (field_00 == layout->key_records[i].unknown_00) {
            if (field_01 >= 0 && field_01 == layout->key_records[i].unknown_01) {
                return;
            }
            if (field_02 >= 0 && field_02 == layout->key_records[i].unknown_02) {
                return;
            }
        }
    }

    layout->key_records[index].unknown_00 = (i8)field_00;
    layout->key_records[index].unknown_01 = (i8)field_01;
    layout->key_records[index].unknown_02 = (i8)field_02;
    layout->key_records[index].unknown_04 = field_04;
}

void __fastcall NuKeyboardInitializeDefaultRecords_candidate(NuKeyboardState_PC *state) {
    NuKeyboardInitLayout_PC *layout;

    layout = (NuKeyboardInitLayout_PC *)state;
    NuKeyboardResetInputRecords(state);

#define ADD_DEFAULT_RECORD(field_00, field_01, field_02)                                                               \
    if (layout->unknown_67e4 < 0x20) {                                                                                 \
        NuKeyboardInsertInputRecord(state, 0, layout->unknown_67e4, field_00, field_01, field_02, 0);                  \
        ++layout->unknown_67e4;                                                                                        \
    }

    ADD_DEFAULT_RECORD(1, 0, -1);
    ADD_DEFAULT_RECORD(2, 0, -1);
    ADD_DEFAULT_RECORD(1, 1, -1);
    ADD_DEFAULT_RECORD(2, 1, -1);
    ADD_DEFAULT_RECORD(1, 2, -1);
    ADD_DEFAULT_RECORD(2, 2, -1);
    ADD_DEFAULT_RECORD(1, 3, -1);
    ADD_DEFAULT_RECORD(2, 3, -1);
    ADD_DEFAULT_RECORD(3, -1, 12);
    ADD_DEFAULT_RECORD(3, -1, 13);
    ADD_DEFAULT_RECORD(3, -1, 14);
    ADD_DEFAULT_RECORD(3, -1, 15);
    ADD_DEFAULT_RECORD(3, -1, 4);
    ADD_DEFAULT_RECORD(3, -1, 5);
    ADD_DEFAULT_RECORD(3, -1, 6);
    ADD_DEFAULT_RECORD(3, -1, 7);
    ADD_DEFAULT_RECORD(2, 4, 2);
    ADD_DEFAULT_RECORD(2, 5, 0);
    ADD_DEFAULT_RECORD(2, 6, 3);
    ADD_DEFAULT_RECORD(2, 7, 1);
    ADD_DEFAULT_RECORD(3, -1, 9);
    ADD_DEFAULT_RECORD(3, -1, 10);
    ADD_DEFAULT_RECORD(3, -1, 8);
    ADD_DEFAULT_RECORD(3, -1, 11);

#undef ADD_DEFAULT_RECORD
}
