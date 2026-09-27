#include "saga.h"
#include <string.h>

typedef struct NuKeyboardInitKeyRecord_PC {
    u8 unknown_00;
    u8 unknown_01;
    u8 unknown_02;
    u8 unknown_03;
    u16 unknown_04;
} NuKeyboardInitKeyRecord_PC;

STATIC_ASSERT(sizeof(NuKeyboardInitKeyRecord_PC) == 6);

typedef struct NuKeyboardInitTriplet_PC {
    u8 unknown_00;
    u8 unknown_01;
    u8 unknown_02;
    u8 unknown_03_to_09[7];
} NuKeyboardInitTriplet_PC;

STATIC_ASSERT(sizeof(NuKeyboardInitTriplet_PC) == 0x0a);

typedef struct NuKeyboardInitLayout_PC {
    u8 cleared_0000_to_26ff[0x2700];
    u8 cleared_2700_to_4dff[0x2700];
    i32 unknown_4e00;
    i32 unknown_4e04;
    i32 unknown_4e08;
    i32 unknown_4e0c;
    u8 keyboard_polled;
    u8 unknown_4e11;
    u8 unknown_4e12;
    u8 unknown_4e13;
    u8 unknown_4e14_to_4e33[0x20];
    u8 cleared_4e34_to_62fb[0x14c8];
    u8 cleared_62fc_to_6343[0x48];
    u8 unknown_6344;
    u8 unknown_6345_to_6352[14];
    u8 unknown_6353;
    i32 unknown_6354;
    u8 cleared_6358_to_6557[0x200];
    u8 unknown_6558_to_6567[0x10];
    i32 unknown_6568;
    i32 unknown_656c;
    f32 unknown_6570;
    u8 unknown_6574_to_6575[2];
    NuKeyboardInitTriplet_PC unknown_6576_triplets[14];
    u8 unknown_6602_to_6603[2];
    i32 unknown_6604;
    i32 unknown_6608;
    NuKeyboardInitTriplet_PC unknown_660c_triplets[14];
    u8 unknown_6698_to_6719[0x82];
    u8 unknown_671a;
    u8 unknown_671b;
    u8 cleared_671c_to_67df[0xc4];
    u8 unknown_67e0;
    u8 unknown_67e1;
    u8 unknown_67e2_to_67e3[2];
    i32 unknown_67e4;
    u8 unknown_67e8_to_67ef[8];
    NuKeyboardInitKeyRecord_PC key_records[32];
    u8 *keyboard_state;
} NuKeyboardInitLayout_PC;

typedef struct NuKeyboardProfile_PC {
    u8 unknown_00_to_0b[0x0c];
    i32 device_mode_a;
    NuKeyboardDevice_PC *device;
    i32 device_mode_b;
    u8 unknown_18_to_23[0x0c];
    u16 default_key;
    u8 unknown_26_to_35[0x10];
    u8 active_36;
    u8 unknown_37_to_38[2];
    u8 active_39;
    char name[0x40];
    u8 unknown_7a_to_17b[0x17c - 0x7a];
} NuKeyboardProfile_PC;

typedef struct NuKeyboardProfileLayout_PC {
    u8 unknown_0000_to_5cff[0x5d00];
    NuKeyboardProfile_PC profiles[4];
} NuKeyboardProfileLayout_PC;

STATIC_ASSERT(offsetof(NuKeyboardInitLayout_PC, keyboard_polled) == 0x4e10);
STATIC_ASSERT(offsetof(NuKeyboardInitLayout_PC, unknown_6345_to_6352) == 0x6345);
STATIC_ASSERT(offsetof(NuKeyboardInitLayout_PC, unknown_6354) == 0x6354);
STATIC_ASSERT(offsetof(NuKeyboardInitLayout_PC, unknown_6568) == 0x6568);
STATIC_ASSERT(offsetof(NuKeyboardInitLayout_PC, unknown_6570) == 0x6570);
STATIC_ASSERT(offsetof(NuKeyboardInitLayout_PC, unknown_6576_triplets) == 0x6576);
STATIC_ASSERT(offsetof(NuKeyboardInitLayout_PC, unknown_6604) == 0x6604);
STATIC_ASSERT(offsetof(NuKeyboardInitLayout_PC, unknown_660c_triplets) == 0x660c);
STATIC_ASSERT(offsetof(NuKeyboardInitLayout_PC, unknown_671a) == 0x671a);
STATIC_ASSERT(offsetof(NuKeyboardInitLayout_PC, unknown_67e4) == 0x67e4);
STATIC_ASSERT(offsetof(NuKeyboardInitLayout_PC, key_records) == 0x67f0);
STATIC_ASSERT(offsetof(NuKeyboardInitLayout_PC, keyboard_state) == 0x68b0);
STATIC_ASSERT(sizeof(NuKeyboardInitLayout_PC) == 0x68b4);
STATIC_ASSERT(sizeof(NuKeyboardProfile_PC) == 0x17c);
STATIC_ASSERT(offsetof(NuKeyboardProfile_PC, device_mode_a) == 0x0c);
STATIC_ASSERT(offsetof(NuKeyboardProfile_PC, device) == 0x10);
STATIC_ASSERT(offsetof(NuKeyboardProfile_PC, device_mode_b) == 0x14);
STATIC_ASSERT(offsetof(NuKeyboardProfile_PC, default_key) == 0x24);
STATIC_ASSERT(offsetof(NuKeyboardProfile_PC, active_36) == 0x36);
STATIC_ASSERT(offsetof(NuKeyboardProfile_PC, active_39) == 0x39);
STATIC_ASSERT(offsetof(NuKeyboardProfile_PC, name) == 0x3a);
STATIC_ASSERT(offsetof(NuKeyboardProfileLayout_PC, profiles) == 0x5d00);

extern const char PCKeyboardMouse1Name_candidate[15];
extern const char PCKeyboardMouse2Name_candidate[16];
extern const char PCKeyboard2Name_candidate[10];
extern const char PCKeyboard1Name_candidate[10];

void __fastcall NuKeyboardInitializeCommandProfiles_candidate(NuKeyboardState_PC *state) {
    NuKeyboardProfileLayout_PC *layout = (NuKeyboardProfileLayout_PC *)state;
    NuKeyboardProfile_PC *profile;

    profile = &layout->profiles[0];
    profile->device_mode_a = 1;
    profile->device_mode_b = 1;
    profile->active_36 = 1;
    profile->active_39 = 1;
    profile->default_key = 0xffff;
    NuStrCpy(profile->name, PCKeyboardMouse1Name_candidate);
    /* PC searches this profile's configured input and may install its key. */
    STUBBED_NAMED(0x006d43e0);
    STUBBED_NAMED(0x006d43e0);
    STUBBED_NAMED(0x006d45f0);

    profile = &layout->profiles[1];
    profile->device_mode_a = 3;
    profile->device = nu_keyboard_owner.device;
    profile->device_mode_b = 3;
    profile->active_36 = 1;
    profile->active_39 = 1;
    profile->default_key = 0xffff;
    NuStrCpy(profile->name, PCKeyboardMouse2Name_candidate);
    STUBBED_NAMED(0x006d43e0);
    STUBBED_NAMED(0x006d43e0);
    STUBBED_NAMED(0x006d45f0);

    profile = &layout->profiles[2];
    profile->device_mode_a = 2;
    profile->device_mode_b = 2;
    profile->active_36 = 1;
    profile->active_39 = 1;
    profile->default_key = 0xffff;
    NuStrCpy(profile->name, PCKeyboard2Name_candidate);
    STUBBED_NAMED(0x006d43e0);
    STUBBED_NAMED(0x006d43e0);
    STUBBED_NAMED(0x006d45f0);

    profile = &layout->profiles[3];
    profile->device_mode_a = 4;
    profile->device = nu_keyboard_owner.device;
    profile->device_mode_b = 4;
    profile->active_36 = 1;
    profile->active_39 = 1;
    profile->default_key = 0xffff;
    NuStrCpy(profile->name, PCKeyboard1Name_candidate);
    STUBBED_NAMED(0x006d43e0);
    STUBBED_NAMED(0x006d43e0);
    STUBBED_NAMED(0x006d45f0);
}

void __fastcall NuKeyboardInitializeDefaultRecords_candidate(NuKeyboardState_PC *state);

void __fastcall NuKeyboardResetInputRecords_candidate(NuKeyboardState_PC *state) {
    NuKeyboardInitLayout_PC *layout = (NuKeyboardInitLayout_PC *)state;
    i32 i;

    for (i = 0; i < 32; ++i) {
        layout->key_records[i].unknown_00 = 0;
        layout->key_records[i].unknown_01 = 0xff;
        layout->key_records[i].unknown_02 = 0xff;
        layout->key_records[i].unknown_04 = 0;
    }
    layout->unknown_67e4 = 0;
}

extern const f32 PCKeyboardDefaultValue_candidate;

NuKeyboardState_PC *__fastcall NuKeyboardStateInitialize_candidate(NuKeyboardState_PC *state) {
    NuKeyboardInitLayout_PC *layout = (NuKeyboardInitLayout_PC *)state;
    i32 i;
    i32 language;

    memset(layout->cleared_0000_to_26ff, 0, sizeof(layout->cleared_0000_to_26ff));
    memset(layout->cleared_2700_to_4dff, 0, sizeof(layout->cleared_2700_to_4dff));
    memset(layout->cleared_4e34_to_62fb, 0, sizeof(layout->cleared_4e34_to_62fb));
    memset(layout->cleared_62fc_to_6343, 0, sizeof(layout->cleared_62fc_to_6343));
    memset(layout->cleared_6358_to_6557, 0, sizeof(layout->cleared_6358_to_6557));
    memset(layout->cleared_671c_to_67df, 0, sizeof(layout->cleared_671c_to_67df));
    layout->keyboard_state = NULL;

    for (i = 0; i < 32; ++i) {
        layout->key_records[i].unknown_00 = 0;
        layout->key_records[i].unknown_01 = 0xff;
        layout->key_records[i].unknown_02 = 0xff;
        layout->key_records[i].unknown_03 = 0;
        layout->key_records[i].unknown_04 = 0;
    }
    layout->unknown_67e4 = 0;

    STUBBED_NAMED(0x006d3aa0); /* PC keyboard-specific state initialization */

    layout->unknown_4e00 = -1;
    layout->unknown_671a = 1;
    layout->unknown_4e04 = 0;
    layout->unknown_4e08 = 0;
    layout->unknown_4e0c = 0;

    for (i = 0; i < 14; ++i) {
        layout->unknown_6345_to_6352[i] = 0;
        layout->unknown_6576_triplets[i].unknown_00 = 0;
        layout->unknown_6576_triplets[i].unknown_01 = 0;
        layout->unknown_6576_triplets[i].unknown_02 = 0;
        layout->unknown_660c_triplets[i].unknown_00 = 0;
        layout->unknown_660c_triplets[i].unknown_01 = 0;
        layout->unknown_660c_triplets[i].unknown_02 = 0;
    }
    layout->unknown_6608 = -1;
    layout->unknown_67e0 = 0;
    layout->unknown_6604 = 0;
    layout->unknown_6354 = 0;
    layout->keyboard_polled = 0;
    layout->unknown_4e11 = 0;
    layout->unknown_4e12 = 0;
    layout->unknown_4e13 = 0xff;
    layout->unknown_67e1 = 0;
    layout->unknown_6568 = 0x1fff;
    layout->unknown_656c = 0x20;
    layout->unknown_6570 = PCKeyboardDefaultValue_candidate;

    /* PC also redundantly clears 32 16-bit entries in each of 14 tables.
     * Those entries are already zero from the range clear above. */
    NuKeyboardInitializeDefaultRecords_candidate(state);
    STUBBED_NAMED(0x029755c4); /* Optional PC input-state callback */
    NuKeyboardInitializeCommandProfiles_candidate(state);

    language = NuLanguageGet();
    if (language < 0 || language > 0x12) {
        STUBBED_NAMED(0x006da904); /* PC fallback language tables */
    } else {
        STUBBED_NAMED(0x006da71d); /* PC language-specific input tables */
    }

    return state;
}

typedef struct NuKeyboardActionRecord_PC {
    u8 unknown_00_to_27[0x28];
    u8 active;
    u8 unknown_29_to_17b[0x17c - 0x29];
} NuKeyboardActionRecord_PC;

typedef struct NuKeyboardActionState_PC {
    u8 unknown_0000_to_4e33[0x4e34];
    NuKeyboardActionRecord_PC records[14];
    u8 unknown_62fc_to_6607[0x6608 - 0x62fc];
    i32 fallback_action;
} NuKeyboardActionState_PC;

STATIC_ASSERT(sizeof(NuKeyboardActionRecord_PC) == 0x17c);
STATIC_ASSERT(offsetof(NuKeyboardActionRecord_PC, active) == 0x28);
STATIC_ASSERT(offsetof(NuKeyboardActionState_PC, records) == 0x4e34);
STATIC_ASSERT(offsetof(NuKeyboardActionState_PC, fallback_action) == 0x6608);

void __cdecl NuKeyboardApplyActionKeys_candidate(const i16 *keys, i32 count) {
    NuKeyboardActionState_PC *state;
    i32 i;
    i32 recognized;

    state = (NuKeyboardActionState_PC *)nu_input_state;
    recognized = 0;
    for (i = 0; i < count; ++i) {
        i32 key;
        key = keys[i];
        if ((u32)(key - 10) <= 3) {
            state->records[key].active = 1;
            recognized = 1;
        }
    }
    if (!recognized) {
        i32 fallback_action;
        fallback_action = state->fallback_action == 0 ? 0 : 1;
        state->records[10 + fallback_action].active = 1;
    }
}

void __fastcall NuKeyboardInsertInputRecord_candidate(NuKeyboardState_PC *state, i32 unused_edx, i32 index,
                                                      i32 field_00, i32 field_01, i32 field_02, u16 field_04) {
    NuKeyboardInitLayout_PC *layout;
    NuKeyboardInitKeyRecord_PC *record;
    i32 i;

    if ((u32)index > 31) {
        return;
    }

    layout = (NuKeyboardInitLayout_PC *)state;
    for (i = 0; i < 32; ++i) {
        record = &layout->key_records[i];
        if ((i32)(i8)record->unknown_00 == field_00) {
            if (field_01 >= 0 && (i32)(i8)record->unknown_01 == field_01) {
                return;
            }
            if (field_02 >= 0 && (i32)(i8)record->unknown_02 == field_02) {
                return;
            }
        }
    }

    record = &layout->key_records[index];
    record->unknown_00 = (u8)field_00;
    record->unknown_01 = (u8)field_01;
    record->unknown_02 = (u8)field_02;
    record->unknown_04 = field_04;
}

void __fastcall NuKeyboardInitializeDefaultRecords_candidate(NuKeyboardState_PC *state) {
    NuKeyboardInitLayout_PC *layout;

    layout = (NuKeyboardInitLayout_PC *)state;
    NuKeyboardResetInputRecords_candidate(state);

#define ADD_DEFAULT_RECORD(field_00, field_01, field_02)                                                               \
    if (layout->unknown_67e4 < 0x20) {                                                                                 \
        NuKeyboardInsertInputRecord_candidate(state, 0, layout->unknown_67e4, field_00, field_01, field_02, 0);        \
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
