#include "saga.h"

i32 PC_INPUT_RUNTIME_PC::ResolveAuxiliaryConflictsPc(i32 auxiliary_index, i32 bank_count) {
    if ((u32)(auxiliary_ports[auxiliary_index] - 10) > 3)
        return 0;
    PC_INPUT_BINDING_PC *bindings = auxiliary_bindings[auxiliary_index];
    i32 missing = 0;
    for (i32 bank = 0; bank < bank_count; ++bank) {
        if (bank == auxiliary_index || (u32)(auxiliary_ports[bank] - 10) > 3)
            continue;
        if (PcInputActionOrder[0] >= 0) {
            for (i32 a = 0; PcInputActionOrder[a] >= 0; ++a) {
                i32 first = PcInputActionOrder[a];
                if (!bindings[first].kind)
                    continue;
                for (i32 b = 0; PcInputActionOrder[b] >= 0; ++b) {
                    i32 second = PcInputActionOrder[b];
                    if (bindings[second].kind && bindings[second].value != 255 &&
                        bindings[first].value == auxiliary_bindings[bank][second].value &&
                        bindings[first].kind == auxiliary_bindings[bank][second].kind) {
                        bindings[first].value = 0;
                        bindings[first].kind = 0;
                    }
                }
            }
        } else {
            for (i32 first = 0; first < 32; ++first) {
                if (!bindings[first].kind)
                    continue;
                for (i32 second = 0; second < 32; ++second) {
                    if (bindings[second].kind && bindings[first].value == auxiliary_bindings[bank][second].value &&
                        bindings[first].kind == auxiliary_bindings[bank][second].kind) {
                        bindings[first].value = 0;
                        bindings[first].kind = 0;
                    }
                }
            }
        }
    }
    for (i32 index = 0; PcInputActionOrder[index] >= 0; ++index) {
        i32 command = PcInputActionOrder[index];
        if ((PcInputCommandFilterPc == NULL || PcInputCommandFilterPc(auxiliary_ports[auxiliary_index], command)) &&
            bindings[command].kind == 0) {
            missing = 1;
            break;
        }
    }
    return missing;
}

i32 PC_INPUT_RUNTIME_PC::SelectConfigurationPc(i32 port, i32 key, i32 auxiliary_index) {
    PC_INPUT_CONFIG_PC *config = NULL;
    for (i32 attempt = 0; attempt < 3; ++attempt) {
        config = FindConfig(3, port, key);
        if (config != NULL)
            break;
        if (++key == 3)
            key = 0;
    }
    InstallConfig(config, port, key, auxiliary_index >= 0, auxiliary_index);
    if (auxiliary_index >= 0 && (u32)(port - 10) <= 3) {
        for (i32 device = 0; device < 3; ++device)
            (void)TextInputLinkedDevicePc(device);
        if (ResolveAuxiliaryConflictsPc(auxiliary_index, 4))
            config = NULL;
    }
    return config != NULL;
}

u8 PC_INPUT_RUNTIME_PC::InstallDropInConfigurationPc(i32 port, i32 player_index) {
    for (i32 bank = 0; bank < 3; ++bank) {
        if (bank == player_index)
            continue;
        i32 linked_port = TextInputLinkedDevicePc(bank);
        if (linked_port < 0)
            continue;
        auxiliary_ports[bank] = linked_port;
        PCByteCopy_candidate(auxiliary_bindings[bank], devices[linked_port].bindings, 0x80);
    }
    i32 key = player_index;
    PC_INPUT_CONFIG_PC *config = NULL;
    for (i32 attempt = 0; attempt < 3; ++attempt) {
        config = FindConfig(3, port, key);
        if (config != NULL)
            break;
        if (++key == 3)
            key = 0;
    }
    if (config == NULL)
        return 0;
    InstallConfig(config, port, player_index, 1, player_index);
    return ResolveAuxiliaryConflictsPc(player_index, 3) == 0;
}

extern "C" u8 __cdecl PcInputInstallDropInConfigurationPc(i32 port, i32 player_index) {
    return ((PC_INPUT_RUNTIME_PC *)nu_input_state)->InstallDropInConfigurationPc(port, player_index);
}

extern "C" i32 __cdecl PcInputSelectPressedDevicePc(u32 excluded, u8 kinds, i32 *kind, i32 *value, i32 skip_active) {
    PC_INPUT_RUNTIME_PC *runtime = (PC_INPUT_RUNTIME_PC *)nu_input_state;
    runtime->action_mask = (u32)-1;
    for (i32 port = 0; port < 14; ++port) {
        if ((skip_active && runtime->mouse.port_activity[port]) || (excluded & (1u << port)))
            continue;
        PC_INPUT_CAPTURE_PC *capture = &runtime->staged_capture[port];
        i32 selected = -1;
        if ((u32)(port - 10) <= 3) {
            if ((kinds & 16) && capture->kind[0])
                selected = 0;
            else if ((kinds & 8) && capture->kind[1])
                selected = 1;
            else if ((kinds & 4) && capture->kind[2])
                selected = 2;
        } else {
            if ((kinds & 2) && capture->kind[0])
                selected = 0;
            else if ((kinds & 1) && capture->kind[2])
                selected = 2;
        }
        if (selected < 0)
            continue;
        i32 selected_kind = (i8)capture->kind[selected];
        if (!selected_kind)
            return -1;
        if (kind != NULL)
            *kind = selected_kind;
        if (value != NULL)
            *value = capture->value[selected];
        return port;
    }
    return -1;
}

extern "C" i32 __cdecl PcInputMapKeyboardPortPc(i32 port, i32 player) {
    if (port >= 10 && port < 14) {
        PC_INPUT_RUNTIME_PC *runtime = (PC_INPUT_RUNTIME_PC *)nu_input_state;
        if (player != runtime->captured_port)
            ++port;
        if (player >= 0)
            port += player * 2;
    }
    return port;
}

extern "C" u8 __cdecl PcInputSelectConfigurationPc(i32 port, i32 key, i32 auxiliary_index) {
    return ((PC_INPUT_RUNTIME_PC *)nu_input_state)->SelectConfigurationPc(port, key, auxiliary_index);
}

extern "C" void __cdecl PcInputSetCommandFilterPc(i32(__cdecl *filter)(i32, i32)) {
    PcInputCommandFilterPc = filter;
}

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

STATIC_ASSERT(offsetof(GRAPHICS_CAPS_PC, pixel_shader_version) == 0xcc);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, caps) + offsetof(GRAPHICS_CAPS_PC, vertex_shader_version) == 0xcc);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, caps) + offsetof(GRAPHICS_CAPS_PC, pixel_shader_version) == 0xd4);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, identifier) + offsetof(GRAPHICS_ADAPTER_IDENTIFIER_PC, vendor) == 0x5cc);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, identifier) + offsetof(GRAPHICS_ADAPTER_IDENTIFIER_PC, device_id) ==
              0x5d0);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, force_software) == 0x5f8);
extern "C" i32 __cdecl PcConfigureInputGraphicsCaps() {
    if (PcCapsConfigured)
        return 1;
    u32 vertex = PCDeviceBackend.caps.vertex_shader_version;
    u32 pixel = PCDeviceBackend.caps.pixel_shader_version;
    PcCapsConfigured = 1;
    if (vertex >= 0xfffe0101 && pixel >= 0xffff0101)
        PCDeviceHardwareVertexProcessingPC = 1;
    if (vertex >= 0xfffe0200) {
        if (pixel >= 0xffff0200)
            PCDeviceHardwareVertexProcessingPC = 2;
        if (vertex > 0xfffe0200 && pixel > 0xffff0200)
            PCDeviceHardwareVertexProcessingPC = 3;
    }
    if (vertex >= 0xfffe0300 && pixel >= 0xffff0300)
        PCDeviceHardwareVertexProcessingPC = 4;
    u32 device = PCDeviceBackend.identifier.device_id;
    u32 vendor = PCDeviceBackend.identifier.vendor;
    i32 original_class = PCDeviceHardwareVertexProcessingPC;
    if (!original_class) {
        if (pixel >= 0xffff0100) {
            PCDeviceBackend.force_software = 1;
            PCDeviceHardwareVertexProcessingPC = 1;
        } else if (vendor == 0x8086) {
            PCDeviceBackend.force_software = 1;
            PCDeviceHardwareVertexProcessingPC = 1;
            if (device == 0x2572 || device == 0x3582 || device == 0x2562 || device == 0x3577)
                PcIntelShaderFallback = 1;
        }
    }
    if (vendor == 0x10de) {
        switch (PCDeviceBackend.identifier.device_id) {
            case 0x311:
            case 0x312:
            case 0x314:
            case 0x321:
            case 0x322:
            case 0x323:
            case 0x326:
            case 0x341:
            case 0x342:
            case 0x343:
            case 0x344:
                if (original_class > 1)
                    PCDeviceHardwareVertexProcessingPC = 1;
                break;
        }
    } else if (vendor == 0x8086) {
        if (device == 0x2582 || device == 0x2772 || device == 0x29b2)
            PCDeviceHardwareVertexProcessingPC = 1;
    }
    switch (PcWindowOptions.forceshadermodel) {
        case 1:
            PCDeviceHardwareVertexProcessingPC = 1;
            break;
        case 2:
            PCDeviceHardwareVertexProcessingPC = 2;
            break;
        case 3:
            PCDeviceHardwareVertexProcessingPC = 4;
            break;
    }
    if (!original_class) {
        switch (NuLanguageGet()) {
            case 2:
                PCGraphicsFatalDiagnostic(
                    &PCNativeWarningDebugState,
                    "Impossible de lancer ce jeu sur votre ordinateur.\nUne carte graphique compatible avec Pixel "
                    "Shaders v.1.1 est n\351cessaire.\nLe jeu va maintenant \352tre ferm\351.");
                break;
            case 3:
                PCGraphicsFatalDiagnostic(
                    &PCNativeWarningDebugState,
                    "Lo sentimos, pero tu ordenador no puede ejecutar este juego.\nSe necesita una tarjeta gr\341fica "
                    "compatible con Pixel Shaders v1.1.\nEl juego finalizar\341 ahora.");
                break;
            case 4:
                PCGraphicsFatalDiagnostic(
                    &PCNativeWarningDebugState,
                    "Das Spiel kann auf diesem System leider nicht ausgef\374hrt werden.\nDu brauchst eine Pixel "
                    "Shader v1.1-kompatible Grafikkarte.\nDas Spiel wird nun beendet.");
                break;
            case 5:
                PCGraphicsFatalDiagnostic(
                    &PCNativeWarningDebugState,
                    "Purtroppo, il tuo sistema non \350 in grado di eseguire il gioco.\n\310 necessaria una scheda "
                    "video che supporti Pixel Shaders v1.1.\nIl gioco verr\340 chiuso.");
                break;
            case 6:
                PCGraphicsFatalDiagnostic(&PCNativeWarningDebugState,
                                          "Helaas is je pc niet geschikt voor dit spel.\nJe hebt een grafische kaart "
                                          "met Pixel Shaders v1.1-ondersteuning nodig.\nHet spel wordt nu afgesloten.");
                break;
            case 8:
                PCGraphicsFatalDiagnostic(
                    &PCNativeWarningDebugState,
                    "Beklager, men din maskine kan ikke k\370re dette spil.\nDen skal have et grafikkort der "
                    "underst\370tter Pixel Shaders v1.1.\nSpillet vil nu blive lukket.");
                break;
            default:
                PCGraphicsFatalDiagnostic(&PCNativeWarningDebugState,
                                          "Sorry, but your machine is unable to run this game.\nIt requires a graphics "
                                          "card which supports Pixel Shaders v1.1.\nThe game will now exit.");
                break;
        }
    }
    return 1;
}

// Uncompiled PC reconstruction draft. Append to input_runtime.cpp only after shared contract review.
extern "C" i32 __cdecl PcInputDeadzonePc(i32 port) {
    if ((u32)(port - 10) <= 3)
        return 0;
    return ((PC_INPUT_RUNTIME_PC *)nu_input_state)->devices[port].deadzone;
}

extern "C" i32 __cdecl NuPadReadOutputsPc(i32 port, u8 *lx, u8 *ly, u8 *rx, u8 *ry, u8 *l1, u8 *l2, u8 *r1, u8 *r2,
                                          u32 *buttons, u8 *unknown, i32 *flags) {
    if (nu_input_state == NULL)
        return 0;
    return ((PC_INPUT_RUNTIME_PC *)nu_input_state)
        ->ReadSamplePc(port, lx, ly, rx, ry, l1, l2, r1, r2, buttons, unknown, flags);
}

i32 PC_INPUT_RUNTIME_PC::ReadSamplePc(i32 port, u8 *lx, u8 *ly, u8 *rx, u8 *ry, u8 *l1, u8 *l2, u8 *r1, u8 *r2,
                                      u32 *buttons, u8 *unknown, i32 *flags) {
    i32 valid = 0;
    if ((u32)(port - 10) <= 3 && !devices[port].action_keys_active)
        return 0;
    if (!keyboard_polled) {
        if (nu_keyboard_owner.unknown_0004 > 0) {
            nu_keyboard_owner.unknown_0008 = (f32)((f64)nu_keyboard_owner.unknown_0008 - NuTimeGetFrameTime());
            if (nu_keyboard_owner.unknown_0008 <= 0.0f) {
                nu_keyboard_owner.unknown_0008 += nu_keyboard_owner.unknown_0000;
                --nu_keyboard_owner.unknown_0004;
                nu_keyboard_owner.RescanDevicesPc();
            }
        }
        if (!keyboard_polled) {
            keyboard_polled = 1;
            keyboard_state = NuKeyboardPoll(&nu_keyboard_owner);
        }
    }
    UpdateMousePc();
    u32 mask = 1u << port;
    PC_INPUT_SAMPLE_PC *sample = &devices[port].sample;
    if (!(read_mask & mask)) {
        read_mask |= mask;
        valid = SampleDevicePc(port, &raw_sample);
        if ((action_mask & mask) && valid)
            CaptureBindingPc(port);
        TranslateBindingsPc(port, sample, valid, &raw_sample, &mouse, &keyboard_state);
        if ((port == 10 || port == 12) && !keyboard_ui_override)
            ApplyMouseAxisPc(sample);
        if (keyboard_ui_override && valid) {
            if ((u32)(port - 10) <= 3)
                sample->buttons = 0;
            i32 linked = TextInputLinkedDevicePc(TextInputLastDeviceIndexPc);
            if (linked == port || (linked == -1 && (u32)(port - 10) <= 3))
                sample->buttons = CombineUiButtonsPc(port, sample->buttons);
        }
    }
    *buttons = sample->buttons;
    if (!(nu_keyboard_device_flags & 4) && SoundVoiceEnabledGate) {
        *lx = sample->analog[0];
        *ly = sample->analog[1];
        *rx = sample->analog[2];
        *ry = sample->analog[3];
        *l1 = sample->analog[4];
        *l2 = sample->analog[5];
        *r1 = sample->analog[6];
        *r2 = sample->analog[7];
    } else {
        *lx = *ly = *rx = *ry = 128;
        *l1 = *l2 = *r1 = *r2 = 0;
        *buttons = 0;
    }
    *unknown = 0;
    *flags = 0;
    return valid;
}

u32 PC_INPUT_RUNTIME_PC::CombineUiButtonsPc(i32 port, u32 buttons) {
    if ((u32)(port - 10) <= 3)
        buttons = 0;
    const u8 *keys = nu_keyboard_pages + (i32)(i8)nu_keyboard_page_index * 256;
    if (keys[0x1c])
        buttons |= 0x40;
    if (mouse.buttons[0])
        buttons |= 0x40;
    if (keys[0x9c])
        buttons |= 0x40;
    if (keys[0x39])
        buttons |= 0x40;
    if (mouse.buttons[2])
        buttons |= 0x40;
    if (!suppress_key_1) {
        if ((u32)(port - 10) <= 3)
            buttons &= ~0x800u;
        if (keys[1])
            buttons |= 0x20;
    }
    if (keys[0xe])
        buttons |= 0x20;
    if (mouse.buttons[1])
        buttons |= 0x20;
    if (keys[0x3b])
        buttons |= 0x80;
    if (keys[0x3c])
        buttons |= 0x10;
    if (keys[0xcd])
        buttons |= 0x2000;
    if (keys[0xcb])
        buttons |= 0x8000;
    if (keys[0xd0])
        buttons |= 0x4000;
    if (mouse.delta[2] < 0.0f)
        buttons |= 0x4000;
    if (keys[0xc8])
        buttons |= 0x1000;
    if (mouse.delta[2] > 0.0f)
        buttons |= 0x1000;
    return buttons;
}

struct PC_DI_JOYSTICK_STATE_PC {
    i32 axes[8];
    i32 pov[4];
    u8 buttons[128];
    i32 extended_axes[24];
};
STATIC_ASSERT(sizeof(PC_DI_JOYSTICK_STATE_PC) == 0x110);
STATIC_ASSERT(offsetof(PC_DI_JOYSTICK_STATE_PC, pov) == 0x20);
STATIC_ASSERT(offsetof(PC_DI_JOYSTICK_STATE_PC, buttons) == 0x30);
STATIC_ASSERT(offsetof(PC_DI_JOYSTICK_STATE_PC, extended_axes) == 0xb0);
extern const u8 PcInputPovBitsPc[8];

i32 PC_INPUT_RUNTIME_PC::SampleDevicePc(i32 port, PC_INPUT_RAW_SAMPLE_PC *sample) {
    memset(sample->axes, 0, 64);
    memset(sample->buttons, 0, 132);
    if (port < 0)
        return 0;
    if ((u32)(port - 10) <= 3) {
        if (nu_keyboard_owner.poll_status == 0)
            return 0;
        return (i8)devices[port].action_keys_active;
    }
    if (port < 4) {
        PC_INPUT_DEVICE_PC *record = &devices[port];
        record->unknown_29 = 0;
        if (record->kind != 6)
            return 0;
        PC_XINPUT_STATE_PC state;
        if (XInputGetState(port, (XINPUT_STATE *)&state) != 0)
            return 0;
        if (!record->capabilities_valid) {
            PC_XINPUT_CAPABILITIES_PC caps;
            record->capabilities_valid = XInputGetCapabilities(port, 1, (XINPUT_CAPABILITIES *)&caps) == 0;
            if (record->capabilities_valid)
                InitXInputCaps(&caps, record);
        }
        u32 bits = state.gamepad.buttons;
        sample->buttons[7] = (u8)((bits >> 4) & 1);
        sample->buttons[6] = (u8)((bits >> 5) & 1);
        record->unknown_29 = sample->buttons[6];
        sample->buttons[128] = (u8)(bits & 1);
        sample->buttons[130] = (u8)((bits >> 1) & 1);
        sample->buttons[131] = (u8)((bits >> 2) & 1);
        sample->buttons[129] = (u8)((bits >> 3) & 1);
        sample->buttons[3] = (u8)((bits >> 15) & 1);
        sample->buttons[1] = (u8)((bits >> 13) & 1);
        sample->buttons[0] = (u8)((bits >> 12) & 1);
        sample->buttons[5] = (u8)((bits >> 9) & 1);
        sample->buttons[2] = (u8)((bits >> 14) & 1);
        sample->buttons[4] = (u8)((bits >> 8) & 1);
        sample->buttons[8] = (u8)((bits >> 6) & 1);
        sample->buttons[9] = (u8)((bits >> 7) & 1);
        sample->axes[0] = state.gamepad.thumb_lx;
        if (state.gamepad.thumb_ly == -32768)
            state.gamepad.thumb_ly = -32767;
        sample->axes[1] = (i16)-state.gamepad.thumb_ly;
        sample->axes[3] = state.gamepad.thumb_rx;
        if (state.gamepad.thumb_ry == -32768)
            state.gamepad.thumb_ry = -32767;
        sample->axes[4] = (i16)-state.gamepad.thumb_ry;
        sample->axes[6] = (i16)((u16)state.gamepad.left_trigger << 7);
        sample->axes[7] = (i16)((u16)state.gamepad.right_trigger << 7);
        NormalizeAxisPairPc(&sample->axes[0], &sample->axes[1]);
        NormalizeAxisPairPc(&sample->axes[3], &sample->axes[4]);
    } else {
        NuKeyboardDevice_PC *device = devices[port].device;
        if (device == NULL)
            return 0;
        if (device->methods->poll(device) < 0) {
            device->methods->acquire(device);
            return 0;
        }
        PC_DI_JOYSTICK_STATE_PC state;
        if (device->methods->get_state(device, 0x110, &state) != 0)
            return 0;
        for (i32 axis = 0; axis < 8; ++axis)
            sample->axes[axis] = (i16)state.axes[axis];
        for (i32 axis = 0; axis < 24; ++axis)
            sample->axes[axis + 8] = (i16)state.extended_axes[axis];
        PCByteCopy_candidate(sample->buttons, state.buttons, devices[port].button_count);
        if (state.pov[3] >= 0) {
            u8 direction = PcInputPovBitsPc[state.pov[3] / 4500];
            for (i32 bit = 0; bit < 4; ++bit)
                sample->buttons[128 + bit] = (u8)((direction >> bit) & 1);
        } else
            memset(sample->buttons + 128, 0, 4);
    }
    return 1;
}

// Uncompiled drafts retaining native mouse accumulation and cursor behavior.
extern u32 PcMouseButtonsMaskPc, PcMousePressedMaskPc;
extern i32 PcMouseCurrentAnglePc, PcMouseTargetAnglePc;
extern f32 PcMouseRadiusPc, PcMouseInterpolationPc;
extern "C" f32 __cdecl NuPow(f32, f32);
extern "C" f32 __cdecl PcInputAspectRatioPc(void);

i32 PC_INPUT_RUNTIME_PC::UpdateMousePc() {
    if ((nu_keyboard_device_flags & 1) || mouse_polled)
        return 0;
    i32 button_count =
        nu_keyboard_owner.mouse_axes && nu_keyboard_owner.mouse_buttons ? nu_keyboard_owner.mouse_buttons : 0;
    mouse_polled = 1;
    const PC_DI_MOUSE_STATE_PC *state = nu_keyboard_owner.PollMousePc();
    for (i32 axis = 0; axis < 3; ++axis)
        mouse.previous[axis] = mouse.current[axis];
    if (state == NULL) {
        memset(mouse.buttons, 0, 16);
        for (i32 axis = 0; axis < 3; ++axis) {
            mouse.delta[axis] = 0;
            mouse.accumulated[axis] = 0;
            mouse.previous[axis] = mouse.current[axis];
        }
        return 0;
    }
    f32 multiplier = (f32)devices[10].alternate_deadzone * 2.0f;
    multiplier *= multiplier;
    for (i32 axis = 0; axis < 3; ++axis)
        mouse.current[axis] = (i16)state->axes[axis];
    if (first_mouse) {
        first_mouse = 0;
        for (i32 axis = 0; axis < 3; ++axis) {
            mouse.accumulated[axis] = 0;
            mouse.previous[axis] = mouse.current[axis];
        }
        mouse.position[0] = mouse.position[1] = 0.5f;
    }
    for (i32 axis = 0; axis < 3; ++axis) {
        f32 change = (f32)((i32)mouse.current[axis] - mouse.previous[axis]);
        if (change > 32767.0)
            change = (f32)(change - 65536.0);
        if (change < -32768.0f)
            change = (f32)(change + 65536.0);
        mouse.delta[axis] = axis < 2 ? multiplier * change : change;
    }
    mouse.accumulated[0] = (f32)(mouse.accumulated[0] + mouse.delta[0] * 0.00390625);
    mouse.accumulated[1] = (f32)(mouse.accumulated[1] + mouse.delta[1] * 0.00390625);
    mouse.accumulated[2] += mouse.delta[2];
    f32 reciprocal = (f32)(1.0f / PcInputAspectRatioPc());
    f32 x = (f32)(mouse.position[0] + (mouse.delta[0] * reciprocal) * 0.00000762939453125);
    f32 y = (f32)(mouse.position[1] + mouse.delta[1] * 0.00000762939453125);
    if (x < 0.0f)
        x = 0.0f;
    else if (x > 1.0f)
        x = 1.0f;
    if (y < 0.0f)
        y = 0.0f;
    else if (y > 1.0f)
        y = 1.0f;
    mouse.position[0] = x;
    mouse.position[1] = y;
    u32 previous = PcMouseButtonsMaskPc;
    PcMouseButtonsMaskPc = 0;
    u32 mask = 0;
    for (i32 button = 0; button < button_count; ++button)
        if (mouse.buttons[button])
            mask |= 1u << button;
    PcMouseButtonsMaskPc = mask;
    PcMousePressedMaskPc = (mask ^ previous) & mask;
    memset(mouse.buttons, 0, 8);
    PCByteCopy_candidate(mouse.buttons, state->buttons, button_count);
    return 1;
}

void PC_INPUT_RUNTIME_PC::ApplyMouseAxisPc(PC_INPUT_SAMPLE_PC *sample) {
    (void)NuTimeGetFrameTime();
    PC_INPUT_RUNTIME_PC *runtime = (PC_INPUT_RUNTIME_PC *)nu_input_state;
    f32 goal = 0.0f;
    f32 x = (f32)(runtime->mouse.delta[0] * -4.0);
    f32 y = (f32)(runtime->mouse.delta[1] * -4.0);
    f32 square = x * x + y * y;
    if (square > 100000.0f) {
        PcMouseTargetAnglePc = NuAtan2D(y, x);
        i32 difference = PcMouseTargetAnglePc - PcMouseCurrentAnglePc;
        if (difference > 32768)
            PcMouseCurrentAnglePc += 65536;
        else if (difference < -32768)
            PcMouseCurrentAnglePc -= 65536;
    }
    f32 elapsed = NuTimeGetFrameTime();
    f32 exponent = (f32)(elapsed * 4.0);
    PcMouseInterpolationPc = (f32)(1.0f - 1.0f / NuPow(2.0f, exponent));
    i32 difference = PcMouseTargetAnglePc - PcMouseCurrentAnglePc;
    f32 movement = PcMouseInterpolationPc * (f32)difference;
    f32 angle = movement + (f32)PcMouseCurrentAnglePc;
    PcMouseCurrentAnglePc = (i32)angle;
    difference = PcMouseTargetAnglePc - PcMouseCurrentAnglePc;
    if (difference > 32768)
        difference -= 65536;
    else if (difference < -32768)
        difference += 65536;
    if ((difference < 0 ? -difference : difference) > 100)
        goal = 0.3f;
    if (sample->analog[1] < 112)
        goal = 0.7f;
    if (sample->analog[1] > 144)
        goal = 1.0f;
    f32 radius_delta = goal - PcMouseRadiusPc;
    radius_delta *= PcMouseInterpolationPc;
    PcMouseRadiusPc += radius_delta;
    i32 cosine = ((PcMouseCurrentAnglePc + 16384) >> 1) & 32767;
    i32 sine = (PcMouseCurrentAnglePc >> 1) & 32767;
    f32 fx = -NuTrigTable[cosine] * PcMouseRadiusPc;
    fx += 1.0f;
    f32 fy = -NuTrigTable[sine] * PcMouseRadiusPc;
    fy += 1.0f;
    sample->analog[0] = (u8)(i32)(fy * 127.5);
    f32 scaled_x = (f32)(fx * 127.5);
    sample->analog[1] = (u8)(i32)scaled_x;
}

// Uncompiled PC control-flow reconstruction. All indexed records require native initialized domains.
extern u8 PcMouseAxisModesPc[3];
extern f32 PcMouseAxisIdlePc[3];

i32 PC_INPUT_RUNTIME_PC::TranslateBindingsPc(i32 port, PC_INPUT_SAMPLE_PC *out, i32 valid, PC_INPUT_RAW_SAMPLE_PC *raw,
                                             PC_INPUT_MOUSE_RUNTIME_PC *motion, u8 **keys) {
    i32 step;
    if (ramp_seconds == 0.0f)
        step = 32768;
    else {
        step = (i32)(NuTimeGetFrameTime() * (32768.0 / ramp_seconds));
        if (step == 0)
            step = 1;
    }
    if (port >= 10 && port < 14 && keyboard_ui_override)
        step = 32768;
    i16 sums[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    u32 buttons = 0;
    for (i32 n = 0; n < shutdown_state; ++n) {
        PC_INPUT_BINDING_PC *binding = &devices[port].bindings[n];
        PC_INPUT_COMMAND_PC *command = &commands[n];
        i16 *retained = (i16 *)&devices[port].reset_pairs[n].first;
        i32 index = (i16)binding->value;
        u8 pressed = 0;
        u8 digital = 1;
        if (index >= 0) {
            i32 value;
            switch (binding->kind) {
                case 4:
                    pressed = motion->buttons[index];
                    break;
                case 5:
                case 6:
                    if (valid) {
                        if (PcMouseAxisModesPc[index] == 0) {
                            value = (i32)motion->delta[index];
                            if (binding->kind == 6)
                                value = -value;
                            pressed = value > 0;
                            value *= 8;
                        } else {
                            if (PcMouseAxisModesPc[index] == 2) {
                                if (motion->accumulated[index] > 256.0f)
                                    motion->accumulated[index] = 256.0f;
                                if (motion->accumulated[index] < -256.0f)
                                    motion->accumulated[index] = -256.0f;
                                i32 moved = (i32)motion->delta[index];
                                if ((moved < 0 ? -moved : moved) < 5)
                                    PcMouseAxisIdlePc[index] += NuTimeGetFrameTime();
                                else
                                    PcMouseAxisIdlePc[index] = 0.0f;
                                if (PcMouseAxisIdlePc[index] > 0.5f) {
                                    f32 accumulated = motion->accumulated[index];
                                    if (accumulated != 0.0f) {
                                        if (accumulated > 0.0f && accumulated < 128.0f)
                                            accumulated -= 1.0f;
                                        else if (accumulated < 0.0f && accumulated > -128.0f)
                                            accumulated += 1.0f;
                                        else
                                            accumulated = (f32)(accumulated - accumulated * 0.0078125);
                                        motion->accumulated[index] = accumulated;
                                    }
                                }
                            }
                            value = (i32)motion->accumulated[index];
                            if (binding->kind == 6)
                                value = -value;
                            if (value >= 32)
                                value = ((value - 31) * 32512) / 96;
                            else if (value < -32)
                                value = ((value + 32) * 32512) / 96;
                            else
                                value = 0;
                        }
                        if (value > 32767)
                            value = 32767;
                        if (value < -32767)
                            value = -32767;
                        if (command->active == 2 || command->active == 3)
                            *retained = value > 0 ? (i16)value : 0;
                        else
                            *retained = value < 0 ? (i16)value : 0;
                        digital = 0;
                    }
                    break;
                case 7:
                    if (!suppress_key_1 || index != 1)
                        pressed = (*keys)[index];
                    break;
                case 1:
                    if (valid)
                        pressed = raw->buttons[index];
                    break;
                case 2:
                case 3:
                    if (valid) {
                        value = raw->axes[index];
                        if (binding->kind == 3)
                            value = -value;
                        if (value > 32767)
                            value = 32767;
                        if (command->active == 2 || command->active == 3)
                            *retained = value > 0 ? (i16)value : 0;
                        else
                            *retained = value < 0 ? (i16)value : 0;
                        i32 magnitude = *retained < 0 ? -*retained : *retained;
                        pressed = magnitude > axis_threshold;
                        digital = 0;
                    }
                    break;
            }
            if (command->button >= 0) {
                if (pressed || (!digital && (*retained > axis_threshold || *retained < -axis_threshold)))
                    buttons |= 1u << command->button;
            }
        }
        if (command->axis >= 0) {
            if (digital) {
                i32 value = *retained;
                if (pressed) {
                    if (command->active == 2 || command->active == 3) {
                        value += step;
                        if (value > 32767)
                            value = 32767;
                    } else {
                        value -= step;
                        if (value < -32768)
                            value = -32768;
                    }
                } else if (value < 0) {
                    value += step;
                    if (value > 0)
                        value = 0;
                } else {
                    value -= step;
                    if (value < 0)
                        value = 0;
                }
                *retained = (i16)value;
            }
            sums[command->axis] = (i16)((u16)sums[command->axis] + (u16)*retained);
        }
    }
    for (i32 axis = 0; axis < 4; ++axis)
        out->analog[axis] = (u8)(((i32)sums[axis] + 32768) >> 8);
    for (i32 axis = 4; axis < 8; ++axis)
        out->analog[axis] = (u8)((i32)sums[axis] >> 7);
    out->buttons = buttons;
    return 1;
}

void PC_INPUT_RUNTIME_PC::CaptureBindingPc(i32 port) {
    u32 mask = 1u << port;
    if (!(action_mask & mask) || (completed_mask & mask))
        return;
    completed_mask |= mask;
    action_mask &= ~mask;
    if ((u32)(port - 10) <= 3) {
        if (capture_available) {
            captures[port] = captures[10];
            return;
        }
        capture_available = 1;
        i32 key_count = nu_keyboard_owner.key_count;
        i32 groups = (key_count + 3) / 4;
        const u32 *words = (const u32 *)keyboard_state;
        for (i32 group = 0; group < groups; ++group) {
            if (!words[group])
                continue;
            for (i32 key = group * 4; key < key_count && key < group * 4 + 4; ++key)
                if (keyboard_state[key]) {
                    captures[10].kind[0] = 7;
                    captures[10].value[0] = (i16)key;
                    group = groups;
                    break;
                }
        }
        if (nu_keyboard_owner.mouse_axes && nu_keyboard_owner.mouse_buttons && nu_keyboard_owner.mouse_axes >= 2 &&
            nu_keyboard_owner.mouse_buttons >= 2) {
            for (i32 button = 0; button < 8; ++button)
                if (mouse.buttons[button]) {
                    captures[10].kind[1] = 4;
                    captures[10].value[1] = (i16)button;
                    break;
                }
            i32 x = (i32)mouse.accumulated[0];
            i32 y = (i32)mouse.accumulated[1];
            i32 z = (i32)mouse.accumulated[2];
            i32 ax = x < 0 ? -x : x, ay = y < 0 ? -y : y;
            if (ax > ay) {
                if (ax > motion_threshold) {
                    captures[10].kind[2] = x > 0 ? 5 : 6;
                    captures[10].value[2] = 0;
                }
            } else if (ay > motion_threshold) {
                captures[10].kind[2] = y > 0 ? 5 : 6;
                captures[10].value[2] = 1;
            }
            if (z) {
                captures[10].kind[2] = z > 0 ? 5 : 6;
                captures[10].value[2] = 2;
            }
        }
        return;
    }
    for (i32 axis = 0; axis < 32; ++axis) {
        if (raw_sample.axes[axis] > 16384 || raw_sample.axes[axis] < -16384) {
            captures[port].kind[2] = raw_sample.axes[axis] > 16384 ? 2 : 3;
            captures[port].value[2] = (i16)axis;
            break;
        }
    }
    for (i32 button = 0; button < devices[port].button_count; ++button)
        if (raw_sample.buttons[button]) {
            captures[port].kind[0] = 1;
            captures[port].value[0] = (i16)button;
            break;
        }
    for (i32 button = 128; button <= 131; ++button)
        if (raw_sample.buttons[button]) {
            captures[port].kind[0] = 1;
            captures[port].value[0] = (i16)button;
            break;
        }
}

// Native input_runtime.cpp member.
void PC_INPUT_RUNTIME_PC::NormalizeAxisPairPc(i16 *x, i16 *y) {
    f32 vx = (f32)*x, vy = (f32)*y;
    f32 ax = vx < 0 ? -vx : vx, ay = vy < 0 ? -vy : vy;
    f32 square = ax * ax + ay * ay;
    f32 magnitude = NuFsqrt(square);
    f32 ratio;
    if (ax > ay)
        ratio = (f32)(32767.0 / ax);
    else if (ay != 0.0f)
        ratio = (f32)(32767.0 / ay);
    else
        ratio = 1.0f;
    f32 scale = ratio * magnitude;
    scale = (f32)(scale * 0.000030517578125);
    vx = scale * vx;
    vy = scale * vy;
    if (vx < -32767.0)
        vx = -32767.0f;
    if (vx > 32767.0)
        vx = 32767.0f;
    if (vy < -32767.0)
        vy = -32767.0f;
    if (vy > 32767.0)
        vy = 32767.0f;
    *x = (i16)(i32)vx;
    *y = (i16)(i32)vy;
}

// Uncompiled native input_runtime.cpp lifecycle cohort.
void PC_INPUT_RUNTIME_PC::ServicePendingCapturePc() {
    if (action_mask == 0)
        return;
    if (!keyboard_polled) {
        keyboard_polled = 1;
        keyboard_state = NuKeyboardPoll(&nu_keyboard_owner);
    }
    UpdateMousePc();
    for (i32 port = 0; port < 14; ++port) {
        u32 mask = 1u << port;
        // Native 6d87a7 JNE skips SET bits: preserve this condition, even though
        // CaptureBindingPc then sees a clear bit and returns without recording.
        if (!(action_mask & mask)) {
            read_mask |= mask;
            action_mask &= ~mask;
            i32 valid = SampleDevicePc(port, &raw_sample);
            if (valid || (u32)(port - 10) <= 3)
                CaptureBindingPc(port);
        }
    }
}
void PC_INPUT_RUNTIME_PC::AdvanceCaptureRecordsPc() {
    for (i32 port = 0; port < 14; ++port) {
        staged_capture[port] = captures[port];
        captures[port].kind[2] = 0;
        captures[port].kind[0] = 0;
        captures[port].kind[1] = 0;
        if (mouse.port_activity[port]) {
            --mouse.port_activity[port];
            if (staged_capture[port].kind[2] || staged_capture[port].kind[0] || staged_capture[port].kind[1])
                mouse.port_activity[port] = 5;
        }
    }
    completed_mask = 0;
}
extern "C" void __cdecl PcInputEndFramePc(void) {
    PC_INPUT_RUNTIME_PC *runtime = (PC_INPUT_RUNTIME_PC *)nu_input_state;
    runtime->ServicePendingCapturePc();
    runtime->AdvanceCaptureRecordsPc();
    runtime->read_mask = 0;
    runtime->keyboard_polled = 0;
    runtime->mouse_polled = 0;
    runtime->capture_available = 0;
}

// Append to input_runtime.cpp; declarations/mappings in B-contract.json.
#include <string.h>

void PC_INPUT_RUNTIME_PC::CopyBindingsPc(i32 port, i32 to_auxiliary, i32 bank) {
    if (port < 0)
        return;
    if (to_auxiliary) {
        auxiliary_ports[bank] = port;
        PCByteCopy_candidate(auxiliary_bindings[bank], devices[port].bindings, 0x80);
    } else
        PCByteCopy_candidate(devices[port].bindings, auxiliary_bindings[bank], 0x80);
}
extern "C" void __cdecl PcInputCopyBindingsPc(i32 port, i32 to_auxiliary, i32 bank) {
    ((PC_INPUT_RUNTIME_PC *)nu_input_state)->CopyBindingsPc(port, to_auxiliary, bank);
}
extern "C" void __cdecl PcInputResetMouseMotionPc() {
    PC_INPUT_RUNTIME_PC *runtime = (PC_INPUT_RUNTIME_PC *)nu_input_state;
    runtime->mouse.accumulated[2] = 0.0f;
    runtime->mouse.accumulated[1] = 0.0f;
    runtime->mouse.accumulated[0] = 0.0f;
}
extern "C" char *__cdecl PcInputDeviceNamePc(i32 port) {
    return ((PC_INPUT_RUNTIME_PC *)nu_input_state)->devices[port].name;
}
extern "C" void __cdecl PcInputSuppressKeyOnePc(i32 value) {
    ((PC_INPUT_RUNTIME_PC *)nu_input_state)->suppress_key_1 = (u8)value;
}
extern "C" void __cdecl PcInputSetMouseFieldPc(i32 value) {
    ((PC_INPUT_RUNTIME_PC *)nu_input_state)->mouse.field_58 = value;
}
extern "C" void __cdecl PcInputSetCapturedPortPc(i32 port) {
    ((PC_INPUT_RUNTIME_PC *)nu_input_state)->captured_port = port;
}
extern "C" i32 __cdecl PcInputCapturedPortPc() {
    return ((PC_INPUT_RUNTIME_PC *)nu_input_state)->captured_port;
}
extern "C" void __cdecl PcInputSetBlockedKeysPc(const char *keys) {
    PC_INPUT_RUNTIME_PC *runtime = (PC_INPUT_RUNTIME_PC *)nu_input_state;
    runtime->blocked_keys[0] = 0;
    if (keys != NULL)
        NuStrCpy(runtime->blocked_keys, keys);
}
extern "C" i32 __cdecl PcInputIsMouseDevicePc(i32 port) {
    return port == 10 || port == 12;
}
extern "C" i32 __cdecl PcInputIsJoystickDevicePc(i32 port) {
    return (u32)port <= 9;
}
extern "C" i32 __cdecl PcInputMixConfigurationHashPc(i32 previous, i32 value) {
    return (i32)(((u32)previous * 2 + (previous < 0)) ^ (u32)value ^ 0x12345678);
}
i32 PC_INPUT_RUNTIME_PC::HashAuxiliaryBindingsPc() {
    i32 hash = 0;
    for (i32 bank = 0; bank < 4; ++bank)
        for (i32 action = 0; action < 32; ++action) {
            hash = (i32)(((u32)hash * 2 + (hash < 0)) ^ (i32)(i16)auxiliary_bindings[bank][action].value ^ 0x12345678);
            hash = (i32)(((u32)hash * 2 + (hash < 0)) ^ auxiliary_bindings[bank][action].kind ^ 0x12345678);
        }
    return hash;
}
extern "C" i32 __cdecl PcInputHashAuxiliaryBindingsPc() {
    return ((PC_INPUT_RUNTIME_PC *)nu_input_state)->HashAuxiliaryBindingsPc();
}
extern "C" void __cdecl PcInputInstallConfigPc(PC_INPUT_CONFIG_PC *config, i32 port, i32 key, i32 auxiliary, i32 bank) {
    ((PC_INPUT_RUNTIME_PC *)nu_input_state)->InstallConfig(config, port, key, auxiliary, bank);
}
extern "C" i32 __cdecl PcInputBindingPresentPc(i32 port, i32 action, i32 auxiliary, i32 bank) {
    PC_INPUT_RUNTIME_PC *runtime = (PC_INPUT_RUNTIME_PC *)nu_input_state;
    return auxiliary ? runtime->auxiliary_bindings[bank][action].kind != 0
                     : runtime->devices[port].bindings[action].kind != 0;
}
extern "C" PC_INPUT_CONFIG_PC *__cdecl PcInputFindConfigPc(u8 mask, i32 port, i32 key) {
    return ((PC_INPUT_RUNTIME_PC *)nu_input_state)->FindConfig(mask, port, key);
}
i32 PC_INPUT_RUNTIME_PC::BindingAvailablePc(i32 bank, i32 bank_count, i32 kind, i32 value) {
    if ((u32)(auxiliary_ports[bank] - 10) > 3)
        return 1;
    if (kind == 7)
        for (const char *key = blocked_keys; *key; ++key)
            if ((i32)(i8)*key == value)
                return 0;
    for (i32 other = 0; other < bank_count; ++other) {
        if (other == bank || auxiliary_ports[other] == -1 || (u32)(auxiliary_ports[other] - 10) > 3)
            continue;
        if (PcInputActionOrder[0] >= 0) {
            for (i32 entry = 0; PcInputActionOrder[entry] >= 0; ++entry) {
                i32 action = PcInputActionOrder[entry];
                if ((i32)(i16)auxiliary_bindings[other][action].value == value &&
                    auxiliary_bindings[other][action].kind == kind)
                    return 0;
            }
        } else {
            for (i32 action = 0; action < 32; ++action)
                if ((i32)(i16)auxiliary_bindings[other][action].value == value &&
                    auxiliary_bindings[other][action].kind == kind)
                    return 0;
        }
    }
    return 1;
}
extern "C" i32 __cdecl PcInputBindingAvailablePc(i32 bank, i32 bank_count, i32 kind, i32 value) {
    return ((PC_INPUT_RUNTIME_PC *)nu_input_state)->BindingAvailablePc(bank, bank_count, kind, value);
}
void PC_INPUT_RUNTIME_PC::ClearOtherBankConflictsPc(i32 bank, i32 bank_count) {
    if ((u32)(auxiliary_ports[bank] - 10) > 3)
        return;
    for (i32 other = 0; other < bank_count; ++other) {
        if (other == bank || (u32)(auxiliary_ports[other] - 10) > 3)
            continue;
        for (i32 action = 0; action < 32; ++action)
            if (auxiliary_bindings[bank][action].value == auxiliary_bindings[other][action].value &&
                auxiliary_bindings[bank][action].kind == auxiliary_bindings[other][action].kind) {
                auxiliary_bindings[other][action].value = 0;
                auxiliary_bindings[other][action].kind = 0;
            }
    }
}
extern "C" void __cdecl PcInputClearOtherBankConflictsPc(i32 bank, i32 bank_count) {
    ((PC_INPUT_RUNTIME_PC *)nu_input_state)->ClearOtherBankConflictsPc(bank, bank_count);
}
extern "C" i32 __cdecl PcInputResolveAuxiliaryConflictsPc(i32 bank, i32 bank_count) {
    return ((PC_INPUT_RUNTIME_PC *)nu_input_state)->ResolveAuxiliaryConflictsPc(bank, bank_count);
}
extern "C" i32 __cdecl PcInputAlternateDeadzonePc(i32 port) {
    if ((u32)(port - 10) > 3)
        return 0;
    return ((PC_INPUT_RUNTIME_PC *)nu_input_state)->devices[port].alternate_deadzone;
}
extern "C" void __cdecl PcInputSetDeadzonePc(i32 port, i32 value) {
    ((PC_INPUT_RUNTIME_PC *)nu_input_state)->devices[port].deadzone = (u32)(port - 10) > 3 ? (u8)value : 0;
}
extern "C" void __cdecl PcInputSetAlternateDeadzonePc(i32 port, i32 value) {
    ((PC_INPUT_RUNTIME_PC *)nu_input_state)->devices[port].alternate_deadzone = (u32)(port - 10) <= 3 ? (u8)value : 0;
}
extern "C" PC_INPUT_COMMAND_FILTER_PC __cdecl PcInputGetCommandFilterPc() {
    return PcInputCommandFilterPc;
}

void PC_INPUT_RUNTIME_PC::AssignBindingPc(i32 action, i32 port, i32 kind, i32 value, i32 unused, i32 auxiliary,
                                          i32 bank, u32 excluded) {
    PC_INPUT_BINDING_PC *bindings = auxiliary ? auxiliary_bindings[bank] : devices[port].bindings;
    i32 simple = (u32)port <= 3;
    i32 active = (i8)commands[action].active;
    i32 positive = kind == 5 || kind == 6 ? 5 : 2;
    i32 negative = positive + 1;
    if (!(PcInputAxisRemapInitialized & 1)) {
        PcInputAxisRemapInitialized |= 1;
        PcInputAxisRemap[10] = PcInputAxisRemap[11] = (u8)positive;
        PcInputAxisRemap[12] = PcInputAxisRemap[13] = (u8)negative;
        PcInputAxisRemap[14] = PcInputAxisRemap[15] = 0;
    }
    if (kind != positive && kind != negative) {
        for (i32 i = 0; i < 32; ++i)
            if (bindings[i].kind == kind && (i32)(i16)bindings[i].value == value && !(excluded & (1u << i))) {
                bindings[i].kind = 0;
                bindings[i].value = 0;
            }
        bindings[action].value = (u16)value;
        bindings[action].kind = (u8)kind;
        return;
    }
    if (active == 3) {
        bindings[action].kind = (u8)kind;
        bindings[action].value = (u16)value;
        for (i32 i = 0; i < 32; ++i) {
            i32 other_kind = bindings[i].kind;
            if (i != action && (other_kind == positive || other_kind == negative) &&
                (i32)(i16)bindings[i].value == value) {
                i32 index = (kind == positive ? 4 : 0) + (other_kind == positive ? 2 : 0);
                if (commands[i].active == 2)
                    ++index;
                if (commands[i].active == 3)
                    index += 8;
                bindings[i].kind = PcInputAxisRemap[index];
                bindings[i].value = bindings[i].kind ? (u16)value : 0;
            }
        }
        return;
    }
    kind = ((kind == positive && active == 2) || (kind == negative && active == 1)) ? positive : negative;
    for (i32 i = 0; i < 32; ++i)
        if ((bindings[i].kind == positive || bindings[i].kind == negative) && (i32)(i16)bindings[i].value == value &&
            !(excluded & (1u << i))) {
            bindings[i].kind = 0;
            bindings[i].value = 0;
        }
    i32 axis = commands[action].axis;
    if (axis < 0)
        return;
    if (simple && (value == 6 || value == 7)) {
        bindings[action].value = (u16)value;
        bindings[action].kind = (u8)kind;
        return;
    }
    if (active == 2 || active == 1)
        for (i32 i = 0; i < 32; ++i)
            if ((commands[i].active == 2 || commands[i].active == 1) && commands[i].axis == axis) {
                bindings[i].kind = (u8)kind;
                bindings[i].value = (u16)value;
            }
}
extern "C" void __cdecl PcInputAssignBindingPc(i32 action, i32 port, i32 kind, i32 value, i32 unused, i32 auxiliary,
                                               i32 bank, u32 excluded) {
    ((PC_INPUT_RUNTIME_PC *)nu_input_state)
        ->AssignBindingPc(action, port, kind, value, unused, auxiliary, bank, excluded);
}

// Append to input_runtime.cpp. Extend config unknown_44 with metadata named in B-layout.txt.
void PC_INPUT_RUNTIME_PC::ExportConfigPc(i32 port, PC_INPUT_CONFIG_PC *config, i32 auxiliary, i32 bank, i32 key) {
    PC_INPUT_BINDING_PC *bindings = auxiliary ? auxiliary_bindings[bank] : devices[port].bindings;
    for (i32 i = 0; i < 4; ++i) {
        config->axes[i][0] = config->unknown_axes[i][0] = 0;
        config->axes[i][1] = config->axes[i][2] = 255;
        config->unknown_axes[i][1] = config->unknown_axes[i][2] = 255;
    }
    for (i32 i = 0; i < 16; ++i)
        config->buttons[i][0] = config->buttons[i][1] = 0;
    for (i32 action = 0; action < shutdown_state; ++action) {
        i32 kind = bindings[action].kind;
        if (!kind)
            continue;
        i32 active = (i8)commands[action].active;
        i32 axis = commands[action].axis;
        i32 button = commands[action].button;
        if (axis >= 0) {
            u8 *triplet = config->all_axes[axis];
            if (kind == 1 || kind == 7) {
                triplet[0] = kind == 1 ? 11 : 10;
                triplet[active == 2 || active == 3 ? 2 : 1] = (u8)bindings[action].value;
            } else if (kind == 2 || kind == 3 || kind == 5 || kind == 6) {
                triplet[1] = triplet[2] = (u8)bindings[action].value;
                i32 positive = kind == 5 || kind == 6 ? 5 : 2;
                i32 negative = positive + 1;
                if (active == 3)
                    triplet[0] = (u8)kind;
                else if ((active == 2 && kind == positive) || (active == 1 && kind == negative))
                    triplet[0] = (u8)positive;
                else
                    triplet[0] = (u8)negative;
            } else if (kind == 12 || kind == 13) {
                triplet[0] = (u8)kind;
                triplet[1] = triplet[2] = (u8)bindings[action].value;
            }
        }
        if (button >= 0 && (kind >= 1 && kind <= 7 || kind == 12 || kind == 13)) {
            config->buttons[button][0] = (u8)kind;
            config->buttons[button][1] = (u8)bindings[action].value;
        }
    }
    for (i32 i = 0; i < 4; ++i) {
        if (config->axes[i][1] == 255 || config->axes[i][2] == 255)
            config->axes[i][0] = config->axes[i][1] = config->axes[i][2] = 0;
        if (config->unknown_axes[i][1] == 255 || config->unknown_axes[i][2] == 255)
            config->unknown_axes[i][0] = config->unknown_axes[i][1] = config->unknown_axes[i][2] = 0;
    }
    memset(config->name, 0, 64);
    PCByteCopy_candidate(config->name, devices[port].name, NuStrLen(devices[port].name));
    config->product_id = devices[port].product.Data1;
    config->guid_data2 = devices[port].product.Data2;
    config->guid_data3 = devices[port].product.Data3;
    for (i32 i = 0; i < 8; ++i)
        config->guid_data4[i] = devices[port].product.Data4[i];
    config->button_count = devices[port].button_count;
    config->pov_count = (i8)devices[port].pov_count;
    config->axis_count = (i8)devices[port].axis_count;
    config->deadzone = devices[port].deadzone;
    config->unknown_5b = 0;
    config->alternate_deadzone = devices[port].alternate_deadzone;
    config->unknown_5d = 0;
    config->key = (i16)key;
}
void PC_INPUT_RUNTIME_PC::PromoteConfigPc(PC_INPUT_CONFIG_PC *records, i32 count, PC_INPUT_CONFIG_PC *config) {
    i32 index = 0;
    for (; index < count; ++index)
        if (records[index].product_id == config->product_id && records[index].key == config->key)
            break;
    if (index == count)
        index = count - 1;
    if (index > 0)
        PCByteCopy_candidate(records + 1, records, index * sizeof(*records));
    PCByteCopy_candidate(records, config, sizeof(*records));
}
extern "C" void __cdecl PcInputStageAuxiliaryConfigPc(i32 port, i32 bank, i32 key) {
    PC_INPUT_CONFIG_PC config;
    PC_INPUT_RUNTIME_PC *runtime = (PC_INPUT_RUNTIME_PC *)nu_input_state;
    runtime->ExportConfigPc(port, &config, 1, bank, key);
    runtime->PromoteConfigPc(runtime->primary, 64, &config);
}

// Append input_runtime.cpp. Native strings/table definitions and mappings in B-save-data.json.
#include <stdio.h>
extern u8 PcInputWriteTextProfiles;
extern const char *PcInputBindingKindNames[14];
extern const char PcInputProfileHeader[], PcInputProfileGuidChar[], PcInputProfileGuidEnd[], PcInputProfileNumbers[];
extern const char PcInputProfileTab[], PcInputProfileAxis[], PcInputProfileAxesEnd[], PcInputProfileUnknownAxesEnd[];
extern const char PcInputProfileButton[], PcInputProfileButtonsEnd0[], PcInputProfileButtonsEnd1[];
extern const char PcInputProfileButtonsEnd2[], PcInputProfileButtonsEnd3[], PcInputProfileFileFormat[];

void PC_INPUT_RUNTIME_PC::SanitizeConfigNamePc(const char *name, char *destination) {
    PCByteCopy_candidate(destination, name, 63);
    destination[63] = 0;
    char *out = destination;
    for (char *in = destination; *in; ++in) {
        char c = *in;
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_')
            *out++ = c;
    }
    *out = 0;
}
void PC_INPUT_RUNTIME_PC::FormatConfigTextPc(char *destination, PC_INPUT_CONFIG_PC *config) {
    char text[256], part[256], character[4];
    sprintf(text, PcInputProfileHeader, config->name, config->product_id, config->guid_data2, config->guid_data3);
    for (i32 i = 0; i < 8; ++i) {
        u32 c = config->guid_data4[i];
        if (c < 32)
            sprintf(part, "%d", c);
        else {
            character[0] = (char)c;
            character[1] = 0;
            sprintf(part, PcInputProfileGuidChar, character);
        }
        strcat(text, part);
        strcat(text, i < 7 ? "," : PcInputProfileGuidEnd);
    }
    NuStrCpy(destination, text);
    sprintf(text, PcInputProfileNumbers, config->button_count, config->pov_count, config->axis_count,
            (i16)((u16)config->deadzone | ((u16)config->unknown_5b << 8)),
            (i16)((u16)config->alternate_deadzone | ((u16)config->unknown_5d << 8)), config->key);
    NuStrCat(destination, text);
    NuStrCpy(text, PcInputProfileTab);
    for (i32 i = 0; i < 4; ++i) {
        sprintf(part, PcInputProfileAxis, PcInputBindingKindNames[config->axes[i][0]], config->axes[i][1],
                config->axes[i][2]);
        NuStrCat(text, part);
    }
    NuStrCat(text, PcInputProfileAxesEnd);
    NuStrCat(destination, text);
    NuStrCpy(text, PcInputProfileTab);
    for (i32 i = 0; i < 4; ++i) {
        sprintf(part, PcInputProfileAxis, PcInputBindingKindNames[config->unknown_axes[i][0]],
                config->unknown_axes[i][1], config->unknown_axes[i][2]);
        NuStrCat(text, part);
    }
    NuStrCat(text, PcInputProfileUnknownAxesEnd);
    NuStrCat(destination, text);
    NuStrCpy(text, PcInputProfileTab);
    for (i32 i = 0; i < 4; ++i) {
        sprintf(part, PcInputProfileButton, PcInputBindingKindNames[config->buttons[i][0]], config->buttons[i][1]);
        NuStrCat(text, part);
    }
    NuStrCat(text, PcInputProfileButtonsEnd0);
    NuStrCat(destination, text);
    NuStrCpy(text, PcInputProfileTab);
    for (i32 i = 4; i < 8; ++i) {
        sprintf(part, PcInputProfileButton, PcInputBindingKindNames[config->buttons[i][0]], config->buttons[i][1]);
        NuStrCat(text, part);
    }
    NuStrCat(text, PcInputProfileButtonsEnd1);
    NuStrCat(destination, text);
    NuStrCpy(text, PcInputProfileTab);
    for (i32 i = 8; i < 12; ++i) {
        sprintf(part, PcInputProfileButton, PcInputBindingKindNames[config->buttons[i][0]], config->buttons[i][1]);
        NuStrCat(text, part);
    }
    NuStrCat(text, PcInputProfileButtonsEnd2);
    NuStrCat(destination, text);
    NuStrCpy(text, PcInputProfileTab);
    for (i32 i = 12; i < 16; ++i) {
        sprintf(part, PcInputProfileButton, PcInputBindingKindNames[config->buttons[i][0]], config->buttons[i][1]);
        NuStrCat(text, part);
    }
    NuStrCat(text, PcInputProfileButtonsEnd3);
    NuStrCat(destination, text);
}
void PC_INPUT_RUNTIME_PC::WriteConfigTextPc(const char *directory, PC_INPUT_CONFIG_PC *config) {
    char name[64], path[1024], text[2036];
    if (!config->name[0])
        return;
    SanitizeConfigNamePc(config->name, name);
    sprintf(path, PcInputProfileFileFormat, directory, name, config->key);
    FILE *file = PcOpenPlatformFile(path, "", 0, "w");
    if (file != NULL) {
        FormatConfigTextPc(text, config);
        fwrite(text, NuStrLen(text), 1, file);
        fclose(file);
    }
}
void PC_INPUT_RUNTIME_PC::WriteConfigurationPc(PC_INPUT_CONFIG_PC *records, i32 count, const char *directory) {
    char path[1024];
    NuStrCpy(path, directory);
    NuStrCat(path, "Mappings.dat");
    FILE *file = PcOpenPlatformFile(path, "", 0, "wb");
    if (file != NULL) {
        u32 word = 0x4d415056;
        fwrite(&word, 4, 1, file);
        word = 19;
        fwrite(&word, 4, 1, file);
        fwrite(records, count * sizeof(*records), 1, file);
        fclose(file);
    }
    for (i32 i = 0; i < count; ++i)
        if (records[i].name[0] && PcInputWriteTextProfiles)
            WriteConfigTextPc(directory, &records[i]);
}
void PC_INPUT_RUNTIME_PC::SaveConfigurationPc() {
    char directory[1024];
    const char *company = PcCompanyName;
    const char *product = PcProductName;
    char *folder = PcGetUserFolder(company, product);
    sprintf(directory, "%s\\%s\\%s\\", folder, company, product);
    WriteConfigurationPc(primary, 64, directory);
}
extern "C" void __cdecl PcInputSaveConfigurationPc() {
    ((PC_INPUT_RUNTIME_PC *)nu_input_state)->SaveConfigurationPc();
}

u8 PcInputAxisRemap[16];
u32 PcInputAxisRemapInitialized;
extern const char PcInputProfileHeader[21] = "\t\"%s\",\t0x%x,%d,%d, {";
extern const char PcInputProfileGuidChar[5] = "'%s'";
extern const char PcInputProfileGuidEnd[17] = "}, //Guid stuff\n";
extern const char PcInputProfileNumbers[43] = "\t%d,%d,%d,%d,%d,%d,0,0,\t//NumButtons etc.\n";
extern const char PcInputProfileTab[2] = "\t";
extern const char PcInputProfileAxis[11] = "%s,%d,%d, ";
extern const char PcInputProfileAxesEnd[25] = "//analogue thumb sticks\n";
extern const char PcInputProfileUnknownAxesEnd[29] = "//analogue shoulder buttons\n";
extern const char PcInputProfileButton[8] = "%s,%d, ";
extern const char PcInputProfileButtonsEnd0[15] = "//l2,r2,l1,r1\n";
extern const char PcInputProfileButtonsEnd1[26] = "//rup,rright,rdown,rleft\n";
extern const char PcInputProfileButtonsEnd2[22] = "//select,l3,r3,start\n";
extern const char PcInputProfileButtonsEnd3[26] = "//lup,lright,ldown,lleft\n";
extern const char PcInputProfileFileFormat[13] = "%s%s_v%d.txt";
extern const char PcInputBindingKindName0[7] = "NOTSET";
extern const char PcInputBindingKindName1[11] = "PADDIGITAL";
extern const char PcInputBindingKindName2[15] = "PADANALOGUEPOS";
extern const char PcInputBindingKindName3[15] = "PADANALOGUENEG";
extern const char PcInputBindingKindName4[13] = "MOUSEDIGITAL";
extern const char PcInputBindingKindName5[13] = "MOUSEMOVEPOS";
extern const char PcInputBindingKindName6[13] = "MOUSEMOVENEG";
extern const char PcInputBindingKindName7[11] = "KEYDIGITAL";
extern const char PcInputBindingKindName8[14] = "KEYDIGITALPOS";
extern const char PcInputBindingKindName9[14] = "KEYDIGITALNEG";
extern const char PcInputBindingKindName10[16] = "KEYANALOGUEPAIR";
extern const char PcInputBindingKindName11[16] = "PADANALOGUEPAIR";
extern const char PcInputBindingKindName12[13] = "FORCEDMOUSEX";
extern const char PcInputBindingKindName13[13] = "FORCEDMOUSEY";
const char *PcInputBindingKindNames[14] = {PcInputBindingKindName0,  PcInputBindingKindName1,  PcInputBindingKindName2,
                                           PcInputBindingKindName3,  PcInputBindingKindName4,  PcInputBindingKindName5,
                                           PcInputBindingKindName6,  PcInputBindingKindName7,  PcInputBindingKindName8,
                                           PcInputBindingKindName9,  PcInputBindingKindName10, PcInputBindingKindName11,
                                           PcInputBindingKindName12, PcInputBindingKindName13};
u8 PcInputWriteTextProfiles = 0;
