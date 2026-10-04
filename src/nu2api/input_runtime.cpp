#include "saga.h"

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
    i32 unknown_67e8;
    i32 unknown_67ec;
    NuKeyboardInitKeyRecord_PC key_records[32];
    u8 *keyboard_state;
} NuKeyboardInitLayout_PC;

STATIC_ASSERT(sizeof(NuKeyboardInitLayout_PC) == 0x68b4);
STATIC_ASSERT(offsetof(NuKeyboardInitLayout_PC, unknown_4e00) == 0x4e00);
STATIC_ASSERT(offsetof(NuKeyboardInitLayout_PC, unknown_4e04) == 0x4e04);
STATIC_ASSERT(offsetof(NuKeyboardInitLayout_PC, unknown_4e08) == 0x4e08);
STATIC_ASSERT(offsetof(NuKeyboardInitLayout_PC, unknown_4e0c) == 0x4e0c);
STATIC_ASSERT(offsetof(NuKeyboardInitLayout_PC, keyboard_polled) == 0x4e10);
STATIC_ASSERT(offsetof(NuKeyboardInitLayout_PC, unknown_4e11) == 0x4e11);
STATIC_ASSERT(offsetof(NuKeyboardInitLayout_PC, unknown_4e12) == 0x4e12);
STATIC_ASSERT(offsetof(NuKeyboardInitLayout_PC, unknown_4e13) == 0x4e13);
STATIC_ASSERT(offsetof(NuKeyboardInitLayout_PC, cleared_62fc_to_6343) == 0x62fc);
STATIC_ASSERT(offsetof(NuKeyboardInitLayout_PC, unknown_6345_to_6352) == 0x6345);
STATIC_ASSERT(offsetof(NuKeyboardInitLayout_PC, unknown_6354) == 0x6354);
STATIC_ASSERT(offsetof(NuKeyboardInitLayout_PC, unknown_6568) == 0x6568);
STATIC_ASSERT(offsetof(NuKeyboardInitLayout_PC, unknown_656c) == 0x656c);
STATIC_ASSERT(offsetof(NuKeyboardInitLayout_PC, unknown_6570) == 0x6570);
STATIC_ASSERT(offsetof(NuKeyboardInitLayout_PC, unknown_6576_triplets) == 0x6576);
STATIC_ASSERT(offsetof(NuKeyboardInitLayout_PC, unknown_6604) == 0x6604);
STATIC_ASSERT(offsetof(NuKeyboardInitLayout_PC, unknown_6608) == 0x6608);
STATIC_ASSERT(offsetof(NuKeyboardInitLayout_PC, unknown_660c_triplets) == 0x660c);
STATIC_ASSERT(offsetof(NuKeyboardInitLayout_PC, unknown_671a) == 0x671a);
STATIC_ASSERT(offsetof(NuKeyboardInitLayout_PC, cleared_671c_to_67df) == 0x671c);
STATIC_ASSERT(offsetof(NuKeyboardInitLayout_PC, unknown_67e0) == 0x67e0);
STATIC_ASSERT(offsetof(NuKeyboardInitLayout_PC, unknown_67e1) == 0x67e1);
STATIC_ASSERT(offsetof(NuKeyboardInitLayout_PC, unknown_67e4) == 0x67e4);
STATIC_ASSERT(offsetof(NuKeyboardInitLayout_PC, key_records) == 0x67f0);
STATIC_ASSERT(offsetof(NuKeyboardInitLayout_PC, keyboard_state) == 0x68b0);

STATIC_ASSERT(offsetof(NuKeyboardInitTriplet_PC, unknown_00) == 0);
STATIC_ASSERT(offsetof(NuKeyboardInitTriplet_PC, unknown_01) == 1);
STATIC_ASSERT(offsetof(NuKeyboardInitTriplet_PC, unknown_02) == 2);

#include <stdio.h>
#include <string.h>
extern "C" void __fastcall NuKeyboardInitializeDefaultRecords_candidate(NuKeyboardState_PC *);
extern void(__cdecl *PcInputStateInitializedCallback)(PC_INPUT_RUNTIME_PC *);
extern const char *PcCompanyName, *PcProductName;
extern "C" char *__cdecl PcGetUserFolder(const char *, const char *);
extern "C" FILE *__cdecl PcOpenPlatformFile(const char *, const char *, i32, const char *);
extern TEXT_INPUT_LABELS_PC PcInputLabels820480;
extern TEXT_INPUT_LABELS_PC PcInputLabels820560;
extern TEXT_INPUT_LABELS_PC PcInputLabels820640;
extern TEXT_INPUT_LABELS_PC PcInputLabels820720;
extern TEXT_INPUT_LABELS_PC PcInputLabels820800;
extern TEXT_INPUT_LABELS_PC PcInputLabels8208e0;
extern TEXT_INPUT_LABELS_PC PcInputLabels8209c0;
extern TEXT_INPUT_LABELS_PC PcInputLabels820aa0;
extern TEXT_INPUT_LABELS_PC PcInputLabels820b80;
extern TEXT_INPUT_LABELS_PC PcInputLabels820c60;
extern TEXT_INPUT_LABELS_PC PcInputLabels820d40;
extern TEXT_INPUT_LABELS_PC PcInputLabels820e20;
extern TEXT_INPUT_LABELS_PC PcInputLabels820f00;
extern TEXT_INPUT_LABELS_PC PcInputLabels820fe0;
extern TEXT_INPUT_LABELS_PC PcInputLabels8210c0;
extern char *PcInputKeyNames821558[];
extern char *PcInputKeyNames821910[];
extern char *PcInputKeyNames821cc8[];
extern char *PcInputKeyNames822080[];
extern char *PcInputKeyNames8223f8[];
extern char *PcInputKeyNames822770[];
extern char *PcInputKeyNames822ae8[];
extern char *PcInputKeyNames822e60[];
extern char *PcInputKeyNames8231d8[];
extern char *PcInputKeyNames823590[];
extern char *PcInputKeyNames823948[];
extern char *PcInputKeyNames823d00[];
extern char *PcInputKeyNames8240b8[];
extern char *PcInputKeyNames824470[];
extern char *PcInputKeyNames824828[];
extern char *PcInputKeyNames824be0[];

void PC_INPUT_RUNTIME_PC::ReadConfiguration(PC_INPUT_CONFIG_PC *records, const char *directory) {
    char path[1024];
    u32 version = 0, magic;
    NuStrCpy(path, directory);
    NuStrCat(path, "Mappings.dat");
    memset(records, 0, 0x2700);
    FILE *file = PcOpenPlatformFile(path, "", 0, "rb");
    if (file) {
        fseek(file, 0, SEEK_END);
        i32 bytes = ftell(file);
        fseek(file, 0, SEEK_SET);
        fread(&magic, 4, 1, file);
        if (magic == 0x4d415056) {
            fread(&version, 4, 1, file);
            bytes -= 8;
        } else
            fseek(file, 0, SEEK_SET);
        if (version == 19)
            fread(records, bytes, 1, file);
        fclose(file);
    }
}
void PC_INPUT_RUNTIME_PC::InitConfiguration() {
    char directory[1024];
    const char *company = PcCompanyName;
    const char *product = PcProductName;
    char *folder = PcGetUserFolder(company, product);
    sprintf(directory, "%s\\%s\\%s\\", folder, company, product);
    memset(primary, 0, sizeof(primary));
    ReadConfiguration(primary, directory);
}
PC_INPUT_RUNTIME_PC::PC_INPUT_RUNTIME_PC() {
    NuKeyboardInitLayout_PC *layout = (NuKeyboardInitLayout_PC *)this;
    memset(devices, 0, sizeof(devices));
    memset(primary, 0, sizeof(primary));
    memset(secondary, 0, sizeof(secondary));
    memset(layout->cleared_62fc_to_6343, 0, sizeof(layout->cleared_62fc_to_6343));
    keyboard_state = NULL;
    memset(layout->cleared_671c_to_67df, 0, sizeof(layout->cleared_671c_to_67df));
    memset(auxiliary_bindings, 0, sizeof(auxiliary_bindings));
    for (i32 i = 0; i < 32; ++i) {
        commands[i].active = 0;
        commands[i].axis = -1;
        commands[i].button = -1;
        commands[i].unknown_04 = 0;
    }
    layout->unknown_67e4 = 0;
    InitConfiguration();
    layout->unknown_4e00 = -1;
    layout->unknown_671a = 1;
    layout->unknown_4e08 = 0;
    layout->unknown_4e0c = 0;
    for (i32 i = 0; i < 14; ++i) {
        layout->unknown_6345_to_6352[i] = 0;
        layout->unknown_660c_triplets[i].unknown_00 = 0;
        layout->unknown_660c_triplets[i].unknown_01 = 0;
        layout->unknown_660c_triplets[i].unknown_02 = 0;
        layout->unknown_6576_triplets[i].unknown_00 = 0;
        layout->unknown_6576_triplets[i].unknown_01 = 0;
        layout->unknown_6576_triplets[i].unknown_02 = 0;
    }
    layout->unknown_6608 = -1;
    layout->unknown_67e0 = 0;
    preferred_key = 0;
    layout->unknown_6354 = 0;
    layout->unknown_4e12 = 0;
    layout->unknown_4e13 = 0xff;
    for (i32 i = 0; i < 14; ++i)
        for (i32 j = 0; j < 32; ++j) {
            devices[i].reset_pairs[j].first = 0;
            devices[i].reset_pairs[j].second = 0;
        }
    layout->unknown_6570 = PCKeyboardDefaultValue_candidate;
    layout->unknown_6568 = 0x1fff;
    layout->unknown_656c = 32;
    layout->unknown_4e04 = 0;
    layout->keyboard_polled = 0;
    layout->unknown_4e11 = 0;
    layout->unknown_67e1 = 0;
    NuKeyboardInitializeDefaultRecords_candidate((NuKeyboardState_PC *)this);
    if (PcInputStateInitializedCallback)
        PcInputStateInitializedCallback(this);
    InitKeyboardProfiles();
    switch (NuLanguageGet()) {
        case 0:
            TextInputKeyNamesPc = PcInputKeyNames821910;
            TextInputDefaultLabelsPc = &PcInputLabels820560;
            TextInputAlternateLabelsPc = &PcInputLabels820560;
            break;
        case 1:
            TextInputKeyNamesPc = TextInputKeyNameTablePc;
            TextInputDefaultLabelsPc = &PcInputLabels820480;
            TextInputAlternateLabelsPc = &PcInputLabels820480;
            break;
        case 2:
            TextInputKeyNamesPc = PcInputKeyNames822080;
            TextInputDefaultLabelsPc = &PcInputLabels820aa0;
            TextInputAlternateLabelsPc = &PcInputLabels820aa0;
            break;
        case 3:
            TextInputKeyNamesPc = PcInputKeyNames822ae8;
            TextInputDefaultLabelsPc = &PcInputLabels820d40;
            TextInputAlternateLabelsPc = &PcInputLabels820d40;
            break;
        case 4:
            TextInputKeyNamesPc = PcInputKeyNames822770;
            TextInputDefaultLabelsPc = &PcInputLabels820c60;
            TextInputAlternateLabelsPc = &PcInputLabels820c60;
            break;
        case 5:
            TextInputKeyNamesPc = PcInputKeyNames8223f8;
            TextInputDefaultLabelsPc = &PcInputLabels820b80;
            TextInputAlternateLabelsPc = &PcInputLabels820b80;
            break;
        case 6:
            TextInputKeyNamesPc = PcInputKeyNames8231d8;
            TextInputDefaultLabelsPc = &PcInputLabels820640;
            TextInputAlternateLabelsPc = &PcInputLabels820640;
            break;
        case 7:
            TextInputKeyNamesPc = PcInputKeyNames8240b8;
            TextInputDefaultLabelsPc = &PcInputLabels8209c0;
            TextInputAlternateLabelsPc = &PcInputLabels8209c0;
            break;
        case 8:
            TextInputKeyNamesPc = PcInputKeyNames822e60;
            TextInputDefaultLabelsPc = &PcInputLabels820e20;
            TextInputAlternateLabelsPc = &PcInputLabels820e20;
            break;
        case 10:
            TextInputKeyNamesPc = PcInputKeyNames821cc8;
            TextInputDefaultLabelsPc = &PcInputLabels8210c0;
            TextInputAlternateLabelsPc = &PcInputLabels8210c0;
            break;
        case 11:
            TextInputKeyNamesPc = PcInputKeyNames823590;
            TextInputDefaultLabelsPc = &PcInputLabels820f00;
            TextInputAlternateLabelsPc = &PcInputLabels820f00;
            break;
        case 12:
            TextInputKeyNamesPc = PcInputKeyNames823948;
            TextInputDefaultLabelsPc = &PcInputLabels820fe0;
            TextInputAlternateLabelsPc = &PcInputLabels820fe0;
            break;
        case 14:
            TextInputKeyNamesPc = PcInputKeyNames824828;
            TextInputDefaultLabelsPc = &PcInputLabels820800;
            TextInputAlternateLabelsPc = &PcInputLabels820800;
            break;
        case 15:
            TextInputKeyNamesPc = PcInputKeyNames824470;
            TextInputDefaultLabelsPc = &PcInputLabels820720;
            TextInputAlternateLabelsPc = &PcInputLabels820720;
            break;
        case 16:
            TextInputKeyNamesPc = PcInputKeyNames823d00;
            TextInputDefaultLabelsPc = &PcInputLabels8209c0;
            TextInputAlternateLabelsPc = &PcInputLabels8209c0;
            break;
        case 17:
            TextInputKeyNamesPc = PcInputKeyNames824be0;
            TextInputDefaultLabelsPc = &PcInputLabels8208e0;
            TextInputAlternateLabelsPc = &PcInputLabels8208e0;
            break;
        default:
            TextInputKeyNamesPc = PcInputKeyNames821558;
            TextInputDefaultLabelsPc = &PcInputLabels820480;
            TextInputAlternateLabelsPc = &PcInputLabels820480;
            break;
    }
}
