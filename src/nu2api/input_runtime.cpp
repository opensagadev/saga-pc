#include "gameapi/edtools/edui.h"
#include "legoapi/items/collect/bolts.h"
#include "legoapi/world/levels.h"
#include "nu2api/PCInput.h"
#include "nu2api/huffdecode.h"
#include "nu2api/nucore/nuapi.h"
#include "nu2api/nucore/nukeyboard.h"
#include "nu2api/nucore/nupad.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nucore/nutime.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/nuwin32/d3dApiCalls.h"
#include "nu2api/nuwin32/d3dCore.h"
#include "nu2api/nuwin32/pcapi.h"
#include "nu2api/platform_files.h"
#include <stdio.h>
#include <string.h>

extern "C" {
    u8 PcCapsConfigured = 0;
    const char *dat_02974d58[256] = {0};
    TEXT_INPUT_LABELS *TextInputAlternateLabels = 0;
    u8 PcMouseAxisModes[3] = {0};
    PC_INPUT_RUNTIME *nu_input_state = 0;
    i8 PcInputActionOrder[20] = {0};
    TEXT_INPUT_LABELS *TextInputDefaultLabels = 0;
    u32 mbuttons = 0;
}
static u32 PcMousePressedMask = 0;
extern "C" {
    u8 nu_keyboard_pages[0x200] = {0};
    char dat_029753c0[512] = {0};
    u8 nu_keyboard_page_index = 0;
}
void (*PcInputStateInitializedCallback)(PC_INPUT_RUNTIME *) = NULL;
extern "C" {
    i32 (*PcInputCommandFilter)(i32, i32) = NULL;
    f32 PcMouseAxisIdle[3] = {0};
    void (*TextInputNameCallback)(i32, char *) = 0;
    i32 PcMouseCurrentAngle = 0;
    i32 PcMouseTargetAngle = 0;
    f32 PcMouseRadius = 0;
    i8 TextInputLastDeviceIndex = 0;
    f32 PcMouseInterpolation = 0;
    PC_NATIVE_DEBUG_STATE PCNativeWarningDebugState;
}

extern "C" u32 fun_006d58c0(void) {
    return PcMousePressedMask;
}

extern "C" u32 fun_006d58f0(i32 button) {
    if (button >= 0 && button < 4u) {
        return (1 << button) & PcMousePressedMask;
    }
    return 0;
}

i32 PC_INPUT_RUNTIME::ResolveAuxiliaryConflicts(i32 auxiliary_index, i32 bank_count) {
    if ((u32)(auxiliary_ports[auxiliary_index] - 10) > 3) {
        return 0;
    }
    PC_INPUT_BINDING *bindings = auxiliary_bindings[auxiliary_index];
    i32 missing = 0;
    PC_INPUT_BINDING *peer_bindings = auxiliary_bindings[0];
    i32 *peer_port = auxiliary_ports;
    for (i32 bank = 0; bank < bank_count; ++bank, peer_bindings += 32, ++peer_port) {
        if (bank == auxiliary_index || (u32)(*peer_port - 10) > 3) {
            continue;
        }
        if (PcInputActionOrder[0] >= 0) {
            i8 *first_order = PcInputActionOrder;
            for (i32 first = *first_order; first >= 0; first = *++first_order) {
                PC_INPUT_BINDING *first_binding = &bindings[first];
                if (!first_binding->kind) {
                    continue;
                }
                i8 *second_order = PcInputActionOrder;
                for (i32 second = *second_order; second >= 0; second = *++second_order) {
                    PC_INPUT_BINDING *selected_binding = &bindings[second];
                    PC_INPUT_BINDING *peer_binding = &peer_bindings[second];
                    if (selected_binding->kind && selected_binding->value != 255 && first_binding->value == peer_binding->value &&
                        first_binding->kind == peer_binding->kind) {
                        first_binding->value = 0;
                        first_binding->kind = PC_BINDING_NONE;
                    }
                }
            }
        } else {
            PC_INPUT_BINDING *first_binding = bindings;
            for (i32 first = 0; first < 32; ++first, ++first_binding) {
                if (!first_binding->kind) {
                    continue;
                }
                PC_INPUT_BINDING *selected_binding = bindings;
                PC_INPUT_BINDING *peer_binding = peer_bindings;
                for (i32 second = 0; second < 32; ++second, ++selected_binding, ++peer_binding) {
                    if (selected_binding->kind && first_binding->value == peer_binding->value &&
                        first_binding->kind == peer_binding->kind) {
                        first_binding->value = 0;
                        first_binding->kind = PC_BINDING_NONE;
                    }
                }
            }
        }
    }
    for (i32 index = 0; PcInputActionOrder[index] >= 0; ++index) {
        i32 command = PcInputActionOrder[index];
        if ((PcInputCommandFilter == NULL || PcInputCommandFilter(auxiliary_ports[auxiliary_index], command)) &&
            bindings[command].kind == PC_BINDING_NONE) {
            missing = 1;
            break;
        }
    }
    return missing;
}

i32 PC_INPUT_RUNTIME::SelectConfiguration(i32 port, i32 key, i32 auxiliary_index) {
    PC_INPUT_CONFIG *config = NULL;
    for (i32 attempt = 0; attempt < 3; ++attempt) {
        config = FindConfig(3, port, key);
        if (config != NULL) {
            break;
        }
        if (++key == 3) {
            key = 0;
        }
    }
    InstallConfig(config, port, key, auxiliary_index >= 0, auxiliary_index);
    if (auxiliary_index >= 0 && (u32)(port - 10) <= 3) {
        for (i32 device = 0; device < 3; ++device) {
            (void)NuPadGetPort(device);
        }
        if (ResolveAuxiliaryConflicts(auxiliary_index, 4)) {
            config = NULL;
        }
    }
    return config != NULL;
}

u8 PC_INPUT_RUNTIME::InstallDropInConfiguration(i32 port, i32 player_index) {
    for (i32 bank = 0; bank < 3; ++bank) {
        if (bank == player_index) {
            continue;
        }
        i32 linked_port = NuPadGetPort(bank);
        if (linked_port < 0) {
            continue;
        }
        auxiliary_ports[bank] = linked_port;
        PCByteCopy(auxiliary_bindings[bank], devices[linked_port].bindings, 0x80);
    }
    i32 key = player_index;
    PC_INPUT_CONFIG *config = NULL;
    for (i32 attempt = 0; attempt < 3; ++attempt) {
        config = FindConfig(3, port, key);
        if (config != NULL) {
            break;
        }
        if (++key == 3) {
            key = 0;
        }
    }
    if (config == NULL) {
        return 0;
    }
    InstallConfig(config, port, player_index, 1, player_index);
    return ResolveAuxiliaryConflicts(player_index, 3) == 0;
}

i32 PC_INPUT_RUNTIME::fun_006d7410(i32 key, i32 port) {
    return SelectConfiguration(port, key, key);
}

extern "C" u8 PcInput_MappingIsGood(i32 port, i32 player_index) {
    return nu_input_state->InstallDropInConfiguration(port, player_index);
}

extern "C" i32 PcInput_ScanForAnyInput(u32 excluded, u8 kinds, i32 *kind, i32 *value, i32 skip_active) {
    PC_INPUT_RUNTIME *runtime = nu_input_state;
    runtime->action_mask = (u32)-1;
    for (i32 port = 0; port < 14; ++port) {
        if ((skip_active && runtime->mouse.port_activity[port]) || (excluded & (1u << port))) {
            continue;
        }
        PC_INPUT_CAPTURE *capture = &runtime->staged_capture[port];
        i32 selected = -1;
        if ((u32)(port - 10) <= 3) {
            if ((kinds & 16) && capture->kind[0]) {
                selected = 0;
            } else if ((kinds & 8) && capture->kind[1]) {
                selected = 1;
            } else if ((kinds & 4) && capture->kind[2]) {
                selected = 2;
            }
        } else {
            if ((kinds & 2) && capture->kind[0]) {
                selected = 0;
            } else if ((kinds & 1) && capture->kind[2]) {
                selected = 2;
            }
        }
        if (selected < 0) {
            continue;
        }
        i32 selected_kind = (i8)capture->kind[selected];
        if (!selected_kind) {
            return -1;
        }
        if (kind != NULL) {
            *kind = selected_kind;
        }
        if (value != NULL) {
            *value = capture->value[selected];
        }
        return port;
    }
    return -1;
}

extern "C" i32 PcInput_RemapVirtualKeyboard(i32 port, i32 player) {
    if (port < 10) {
        return port;
    }
    if (port >= 14) {
        return port;
    }
    if (player != nu_input_state->captured_port) {
        ++port;
    }
    if (player >= 0) {
        port += player * 2;
    }
    return port;
}

extern "C" u8 PcInput_SafeGetMapping(i32 port, i32 key, i32 auxiliary_index) {
    return nu_input_state->SelectConfiguration(port, key, auxiliary_index);
}

extern "C" void PcInput_SetIsUsedCallBack(i32 (*filter)(i32, i32)) {
    PcInputCommandFilter = filter;
}

typedef struct NuKeyboardInitKeyRecord { // size 0x6
    u8 unknown_00;                       // 0x0
    u8 unknown_01;                       // 0x1
    u8 unknown_02;                       // 0x2
    u8 unknown_03;                       // 0x3
    u16 unknown_04;                      // 0x4
} NuKeyboardInitKeyRecord;

typedef struct NuKeyboardInitTriplet { // size 0xa
    u8 unknown_00;                     // 0x0
    u8 unknown_01;                     // 0x1
    u8 unknown_02;                     // 0x2
    u8 unknown_03_to_09[7];            // 0x3
} NuKeyboardInitTriplet;

typedef struct NuKeyboardInitLayout {                // size 0x68b4
    u8 cleared_0000_to_26ff[0x2700];                 // 0x0000
    u8 cleared_2700_to_4dff[0x2700];                 // 0x2700
    i32 unknown_4e00;                                // 0x4e00
    i32 unknown_4e04;                                // 0x4e04
    i32 unknown_4e08;                                // 0x4e08
    i32 unknown_4e0c;                                // 0x4e0c
    u8 keyboard_polled;                              // 0x4e10
    u8 unknown_4e11;                                 // 0x4e11
    u8 unknown_4e12;                                 // 0x4e12
    u8 unknown_4e13;                                 // 0x4e13
    u8 unknown_4e14_to_4e33[0x20];                   // 0x4e14
    u8 cleared_4e34_to_62fb[0x14c8];                 // 0x4e34
    u8 cleared_62fc_to_6343[0x48];                   // 0x62fc
    u8 unknown_6344;                                 // 0x6344
    u8 unknown_6345_to_6352[14];                     // 0x6345
    u8 unknown_6353;                                 // 0x6353
    i32 unknown_6354;                                // 0x6354
    u8 cleared_6358_to_6557[0x200];                  // 0x6358
    u8 unknown_6558_to_6567[0x10];                   // 0x6558
    i32 unknown_6568;                                // 0x6568
    i32 unknown_656c;                                // 0x656c
    f32 unknown_6570;                                // 0x6570
    u8 unknown_6574_to_6575[2];                      // 0x6574
    NuKeyboardInitTriplet unknown_6576_triplets[14]; // 0x6576
    u8 unknown_6602_to_6603[2];                      // 0x6602
    i32 unknown_6604;                                // 0x6604
    i32 unknown_6608;                                // 0x6608
    NuKeyboardInitTriplet unknown_660c_triplets[14]; // 0x660c
    u8 unknown_6698_to_6719[0x82];                   // 0x6698
    u8 unknown_671a;                                 // 0x671a
    u8 unknown_671b;                                 // 0x671b
    u8 cleared_671c_to_67df[0xc4];                   // 0x671c
    u8 unknown_67e0;                                 // 0x67e0
    u8 unknown_67e1;                                 // 0x67e1
    u8 unknown_67e2_to_67e3[2];                      // 0x67e2
    i32 unknown_67e4;                                // 0x67e4
    i32 unknown_67e8;                                // 0x67e8
    i32 unknown_67ec;                                // 0x67ec
    NuKeyboardInitKeyRecord key_records[32];         // 0x67f0
    u8 *keyboard_state;                              // 0x68b0
} NuKeyboardInitLayout;

void PC_INPUT_RUNTIME::ReadConfiguration(PC_INPUT_CONFIG *records, const char *directory) {
    char path[1024];
    u32 version = 0;
    u32 magic;
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
        } else {
            fseek(file, 0, SEEK_SET);
        }
        if (version == 19) {
            fread(records, bytes, 1, file);
        }
        fclose(file);
    }
}

void PC_INPUT_RUNTIME::InitConfiguration() {
    char directory[1024];
    /* Retail passes only the folder; the two missing %s read the company and product arguments that
       PcGetUserFolder left on the stack (cdecl cleanup is deferred). */
    sprintf(directory, "%s\\%s\\%s\\", PcGetUserFolder(PcCompanyName, PcProductName));
    memset(primary, 0, sizeof(primary));
    ReadConfiguration(primary, directory);
}

void PC_INPUT_RUNTIME::fun_006d3b10(i32) {
}

extern "C" void fun_006d3b20(void) {
}

extern "C" {
    char **TextInputKeyNames = TextInputKeyNameTable;
    extern const u8 PcInputPovBits[8] = {1, 3, 2, 6, 4, 12, 8, 9};
}

PC_INPUT_RUNTIME::PC_INPUT_RUNTIME() {
    NuKeyboardInitLayout *layout = (NuKeyboardInitLayout *)this;
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
    for (i32 i = 0; i < 14; ++i) {
        for (i32 j = 0; j < 32; ++j) {
            devices[i].reset_pairs[j].first = 0;
            devices[i].reset_pairs[j].second = 0;
        }
    }
    layout->unknown_6570 = PCKeyboardDefaultValue;
    layout->unknown_6568 = 0x1fff;
    layout->unknown_656c = 32;
    layout->unknown_4e04 = 0;
    layout->keyboard_polled = 0;
    layout->unknown_4e11 = 0;
    layout->unknown_67e1 = 0;
    InitializeDefaultRecords();
    if (PcInputStateInitializedCallback) {
        PcInputStateInitializedCallback(this);
    }
    InitKeyboardProfiles();
    switch (NuLanguageGet()) {
        case NULANGUAGE_JAPANESE:
            TextInputKeyNames = dat_00821910;
            TextInputDefaultLabels = &dat_00820560;
            TextInputAlternateLabels = &dat_00820560;
            break;
        case NULANGUAGE_ENGLISH:
            TextInputKeyNames = TextInputKeyNameTable;
            TextInputDefaultLabels = &dat_00820480;
            TextInputAlternateLabels = &dat_00820480;
            break;
        case NULANGUAGE_FRENCH:
            TextInputKeyNames = dat_00822080;
            TextInputDefaultLabels = &dat_00820aa0;
            TextInputAlternateLabels = &dat_00820aa0;
            break;
        case NULANGUAGE_SPANISH:
            TextInputKeyNames = dat_00822ae8;
            TextInputDefaultLabels = &dat_00820d40;
            TextInputAlternateLabels = &dat_00820d40;
            break;
        case NULANGUAGE_GERMAN:
            TextInputKeyNames = dat_00822770;
            TextInputDefaultLabels = &dat_00820c60;
            TextInputAlternateLabels = &dat_00820c60;
            break;
        case NULANGUAGE_ITALIAN:
            TextInputKeyNames = dat_008223f8;
            TextInputDefaultLabels = &dat_00820b80;
            TextInputAlternateLabels = &dat_00820b80;
            break;
        case NULANGUAGE_DUTCH:
            TextInputKeyNames = dat_008231d8;
            TextInputDefaultLabels = &dat_00820640;
            TextInputAlternateLabels = &dat_00820640;
            break;
        case NULANGUAGE_PORTUGUESE:
            TextInputKeyNames = dat_008240b8;
            TextInputDefaultLabels = &dat_008209c0;
            TextInputAlternateLabels = &dat_008209c0;
            break;
        case NULANGUAGE_DANISH:
            TextInputKeyNames = dat_00822e60;
            TextInputDefaultLabels = &dat_00820e20;
            TextInputAlternateLabels = &dat_00820e20;
            break;
        case NULANGUAGE_CZECH:
            TextInputKeyNames = dat_00821cc8;
            TextInputDefaultLabels = &dat_008210c0;
            TextInputAlternateLabels = &dat_008210c0;
            break;
        case NULANGUAGE_POLISH:
            TextInputKeyNames = dat_00823590;
            TextInputDefaultLabels = &dat_00820f00;
            TextInputAlternateLabels = &dat_00820f00;
            break;
        case NULANGUAGE_RUSSIAN:
            TextInputKeyNames = dat_00823948;
            TextInputDefaultLabels = &dat_00820fe0;
            TextInputAlternateLabels = &dat_00820fe0;
            break;
        case NULANGUAGE_SWEDISH:
            TextInputKeyNames = dat_00824828;
            TextInputDefaultLabels = &dat_00820800;
            TextInputAlternateLabels = &dat_00820800;
            break;
        case NULANGUAGE_NORWEGIAN:
            TextInputKeyNames = dat_00824470;
            TextInputDefaultLabels = &dat_00820720;
            TextInputAlternateLabels = &dat_00820720;
            break;
        case NULANGUAGE_BRAZILIAN_PORTUGUESE:
            TextInputKeyNames = dat_00823d00;
            TextInputDefaultLabels = &dat_008209c0;
            TextInputAlternateLabels = &dat_008209c0;
            break;
        case NULANGUAGE_FINNISH:
            TextInputKeyNames = dat_00824be0;
            TextInputDefaultLabels = &dat_008208e0;
            TextInputAlternateLabels = &dat_008208e0;
            break;
        default:
            TextInputKeyNames = dat_00821558;
            TextInputDefaultLabels = &dat_00820480;
            TextInputAlternateLabels = &dat_00820480;
            break;
    }
}

extern "C" i32 PcConfigureInputGraphicsCaps() {
    if (PcCapsConfigured) {
        return 1;
    }
    u32 vertex = PCDeviceBackend.caps.vertex_shader_version;
    u32 pixel = PCDeviceBackend.caps.pixel_shader_version;
    PcCapsConfigured = 1;
    if (vertex >= 0xfffe0101 && pixel >= 0xffff0101) {
        PCDeviceHardwareVertexProcessingPC = PC_SHADER_MODEL_1_1;
    }
    if (vertex >= 0xfffe0200) {
        if (pixel >= 0xffff0200) {
            PCDeviceHardwareVertexProcessingPC = PC_SHADER_MODEL_2_0;
        }
        if (vertex > 0xfffe0200 && pixel > 0xffff0200) {
            PCDeviceHardwareVertexProcessingPC = PC_SHADER_MODEL_2_X;
        }
    }
    if (vertex >= 0xfffe0300 && pixel >= 0xffff0300) {
        PCDeviceHardwareVertexProcessingPC = PC_SHADER_MODEL_3_0;
    }
    u32 device = PCDeviceBackend.identifier.device_id;
    u32 vendor = PCDeviceBackend.identifier.vendor;
    i32 original_class = PCDeviceHardwareVertexProcessingPC;
    if (!original_class) {
        if (pixel >= 0xffff0100) {
            PCDeviceBackend.force_software = 1;
            PCDeviceHardwareVertexProcessingPC = PC_SHADER_MODEL_1_1;
        } else if (vendor == 0x8086) {
            PCDeviceBackend.force_software = 1;
            PCDeviceHardwareVertexProcessingPC = PC_SHADER_MODEL_1_1;
            if (device == 0x2572 || device == 0x3582 || device == 0x2562 || device == 0x3577) {
                PcIntelShaderFallback = 1;
            }
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
                if (original_class > 1) {
                    PCDeviceHardwareVertexProcessingPC = PC_SHADER_MODEL_1_1;
                }
                break;
        }
    } else if (vendor == 0x8086) {
        if (device == 0x2582 || device == 0x2772 || device == 0x29b2) {
            PCDeviceHardwareVertexProcessingPC = PC_SHADER_MODEL_1_1;
        }
    }
    switch (PcWindowOptions.forceshadermodel) {
        case 1:
            PCDeviceHardwareVertexProcessingPC = PC_SHADER_MODEL_1_1;
            break;
        case 2:
            PCDeviceHardwareVertexProcessingPC = PC_SHADER_MODEL_2_0;
            break;
        case 3:
            PCDeviceHardwareVertexProcessingPC = PC_SHADER_MODEL_3_0;
            break;
    }
    if (!original_class) {
        switch (NuLanguageGet()) {
            case NULANGUAGE_FRENCH:
                PCGraphicsFatalDiagnostic(&PCNativeWarningDebugState,
                                          "Impossible de lancer ce jeu sur votre ordinateur.\nUne carte graphique compatible avec Pixel "
                                          "Shaders v.1.1 est n\351cessaire.\nLe jeu va maintenant \352tre ferm\351.");
                break;
            case NULANGUAGE_SPANISH:
                PCGraphicsFatalDiagnostic(
                    &PCNativeWarningDebugState,
                    "Lo sentimos, pero tu ordenador no puede ejecutar este juego.\nSe necesita una tarjeta gr\341fica "
                    "compatible con Pixel Shaders v1.1.\nEl juego finalizar\341 ahora.");
                break;
            case NULANGUAGE_GERMAN:
                PCGraphicsFatalDiagnostic(&PCNativeWarningDebugState,
                                          "Das Spiel kann auf diesem System leider nicht ausgef\374hrt werden.\nDu brauchst eine Pixel "
                                          "Shader v1.1-kompatible Grafikkarte.\nDas Spiel wird nun beendet.");
                break;
            case NULANGUAGE_ITALIAN:
                PCGraphicsFatalDiagnostic(&PCNativeWarningDebugState,
                                          "Purtroppo, il tuo sistema non \350 in grado di eseguire il gioco.\n\310 necessaria una scheda "
                                          "video che supporti Pixel Shaders v1.1.\nIl gioco verr\340 chiuso.");
                break;
            case NULANGUAGE_DUTCH:
                PCGraphicsFatalDiagnostic(&PCNativeWarningDebugState,
                                          "Helaas is je pc niet geschikt voor dit spel.\nJe hebt een grafische kaart "
                                          "met Pixel Shaders v1.1-ondersteuning nodig.\nHet spel wordt nu afgesloten.");
                break;
            case NULANGUAGE_DANISH:
                PCGraphicsFatalDiagnostic(&PCNativeWarningDebugState,
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

extern "C" void NuKeyboardRead(void) {
    nu_input_state->PollKeyboard();
}

// Uncompiled PC reconstruction draft. Append to input_runtime.cpp only after shared contract review.
extern "C" i32 NuPadGetDeadzoneByPortPS(i32 port) {
    if ((u32)(port - 10) <= 3) {
        return 0;
    }
    return nu_input_state->devices[port].deadzone;
}

extern "C" i32 NuPadReadOutputs(i32 port, u8 *lx, u8 *ly, u8 *rx, u8 *ry, u8 *l1, u8 *l2, u8 *r1, u8 *r2, u32 *buttons, u8 *unknown,
                                i32 *flags) {
    if (nu_input_state != NULL) {
        return nu_input_state->ReadSample(port, lx, ly, rx, ry, l1, l2, r1, r2, buttons, unknown, flags);
    }
    return 0;
}

i32 PC_INPUT_RUNTIME::ReadSample(i32 port, u8 *lx, u8 *ly, u8 *rx, u8 *ry, u8 *l1, u8 *l2, u8 *r1, u8 *r2, u32 *buttons, u8 *unknown,
                                 i32 *flags) {
    i32 valid = 0;
    if ((u32)(port - 10) <= 3 && !devices[port].action_keys_active) {
        return 0;
    }
    if (!keyboard_polled) {
        if (nu_keyboard_owner.unknown_0004 > 0) {
            nu_keyboard_owner.unknown_0008 = (f32)((f64)nu_keyboard_owner.unknown_0008 - NuTimeGetFrameTime());
            if (nu_keyboard_owner.unknown_0008 <= 0.0f) {
                nu_keyboard_owner.unknown_0008 += nu_keyboard_owner.unknown_0000;
                --nu_keyboard_owner.unknown_0004;
                nu_keyboard_owner.RescanDevices();
            }
        }
        if (!keyboard_polled) {
            keyboard_polled = 1;
            keyboard_state = nu_keyboard_owner.PollKeyboard();
        }
    }
    UpdateMouse();
    u32 mask = 1u << port;
    PC_INPUT_SAMPLE *sample = &devices[port].sample;
    if (!(read_mask & mask)) {
        read_mask |= mask;
        valid = SampleDevice(port, &raw_sample);
        if ((action_mask & mask) && valid) {
            CaptureBinding(port);
        }
        TranslateBindings(port, sample, valid, &raw_sample, &mouse, &keyboard_state);
        if ((port == 10 || port == 12) && !keyboard_ui_override) {
            ApplyMouseAxis(sample);
        }
        if (keyboard_ui_override && valid) {
            if ((u32)(port - 10) <= 3) {
                sample->buttons = 0;
            }
            i32 linked = NuPadGetPort(TextInputLastDeviceIndex);
            if (linked == port || (linked == -1 && (u32)(port - 10) <= 3)) {
                sample->buttons = CombineUiButtons(port, sample->buttons);
            }
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

u32 PC_INPUT_RUNTIME::CombineUiButtons(i32 port, u32 buttons) {
    if ((u32)(port - 10) <= 3) {
        buttons = 0;
    }
    const u8 *keys = nu_keyboard_pages + (i8)nu_keyboard_page_index * 256;
    if (keys[0x1c]) {
        buttons |= PAD_BUTTON_CROSS;
    }
    if (mouse.buttons[0]) {
        buttons |= PAD_BUTTON_CROSS;
    }
    if (keys[0x9c]) {
        buttons |= PAD_BUTTON_CROSS;
    }
    if (keys[0x39]) {
        buttons |= PAD_BUTTON_CROSS;
    }
    if (mouse.buttons[2]) {
        buttons |= PAD_BUTTON_CROSS;
    }
    if (!suppress_key_1) {
        if ((u32)(port - 10) <= 3) {
            buttons &= ~PAD_BUTTON_START;
        }
        if (keys[1]) {
            buttons |= PAD_BUTTON_CIRCLE;
        }
    }
    if (keys[0xe]) {
        buttons |= PAD_BUTTON_CIRCLE;
    }
    if (mouse.buttons[1]) {
        buttons |= PAD_BUTTON_CIRCLE;
    }
    if (keys[0x3b]) {
        buttons |= PAD_BUTTON_SQUARE;
    }
    if (keys[0x3c]) {
        buttons |= PAD_BUTTON_TRIANGLE;
    }
    if (keys[0xcd]) {
        buttons |= PAD_BUTTON_RIGHT;
    }
    if (keys[0xcb]) {
        buttons |= PAD_BUTTON_LEFT;
    }
    if (keys[0xd0]) {
        buttons |= PAD_BUTTON_DOWN;
    }
    if (mouse.delta[2] < 0.0f) {
        buttons |= PAD_BUTTON_DOWN;
    }
    if (keys[0xc8]) {
        buttons |= PAD_BUTTON_UP;
    }
    if (mouse.delta[2] > 0.0f) {
        buttons |= PAD_BUTTON_UP;
    }
    return buttons;
}

struct PC_DI_JOYSTICK_STATE { // size 0x110
    i32 axes[8];              // 0x000
    i32 pov[4];               // 0x020
    u8 buttons[128];          // 0x030
    i32 extended_axes[24];    // 0x0b0
};

i32 PC_INPUT_RUNTIME::SampleDevice(i32 port, PC_INPUT_RAW_SAMPLE *sample) {
    memset(sample->axes, 0, sizeof(sample->axes));
    memset(sample->buttons, 0, sizeof(sample->buttons));
    if (port < 0) {
        return 0;
    }
    if ((u32)(port - 10) <= 3) {
        if (nu_keyboard_owner.poll_status == 0) {
            return 0;
        }
        return (i8)devices[port].action_keys_active;
    }
    if (port < 4) {
        PC_INPUT_DEVICE *record = &devices[port];
        record->unknown_29 = 0;
        if (record->kind != PC_INPUT_DEVICE_XINPUT) {
            return 0;
        }
        PC_XINPUT_STATE state;
        if (XInputGetState(port, (XINPUT_STATE *)&state) != 0) {
            return 0;
        }
        if (!record->capabilities_valid) {
            PC_XINPUT_CAPABILITIES caps;
            record->capabilities_valid = XInputGetCapabilities(port, 1, (XINPUT_CAPABILITIES *)&caps) == 0;
            if (record->capabilities_valid) {
                InitXInputCaps(&caps, record);
            }
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
        if (state.gamepad.thumb_ly == -32768) {
            state.gamepad.thumb_ly = -32767;
        }
        sample->axes[1] = (i16)-state.gamepad.thumb_ly;
        sample->axes[3] = state.gamepad.thumb_rx;
        if (state.gamepad.thumb_ry == -32768) {
            state.gamepad.thumb_ry = -32767;
        }
        sample->axes[4] = (i16)-state.gamepad.thumb_ry;
        sample->axes[6] = (i16)((u16)state.gamepad.left_trigger << 7);
        sample->axes[7] = (i16)((u16)state.gamepad.right_trigger << 7);
        NormalizeAxisPair(&sample->axes[0], &sample->axes[1]);
        NormalizeAxisPair(&sample->axes[3], &sample->axes[4]);
    } else {
        NuKeyboardDevice *device = devices[port].device;
        if (device == NULL) {
            return 0;
        }
        if (device->methods->poll(device) < 0) {
            device->methods->acquire(device);
            return 0;
        }
        PC_DI_JOYSTICK_STATE state;
        if (device->methods->get_state(device, 0x110, &state) != 0) {
            return 0;
        }
        for (i32 axis = 0; axis < 8; ++axis) {
            sample->axes[axis] = (i16)state.axes[axis];
        }
        for (i32 axis = 0; axis < 24; ++axis) {
            sample->axes[axis + 8] = (i16)state.extended_axes[axis];
        }
        PCByteCopy(sample->buttons, state.buttons, devices[port].button_count);
        if (state.pov[3] >= 0) {
            u8 direction = PcInputPovBits[state.pov[3] / 4500];
            for (i32 bit = 0; bit < 4; ++bit) {
                sample->buttons[128 + bit] = (u8)((direction >> bit) & 1);
            }
        } else {
            memset(sample->buttons + 128, 0, 4);
        }
    }
    return 1;
}

// Uncompiled drafts retaining native mouse accumulation and cursor behavior.
extern u32 mbuttons;

i32 PC_INPUT_RUNTIME::UpdateMouse() {
    if (!(nu_keyboard_device_flags & 1) && !mouse_polled) {
        i32 button_count = nu_keyboard_owner.mouse_axes && nu_keyboard_owner.mouse_buttons ? nu_keyboard_owner.mouse_buttons : 0;
        mouse_polled = 1;
        const PC_DI_MOUSE_STATE *state = nu_keyboard_owner.PollMouse();
        for (i32 axis = 0; axis < 3; ++axis) {
            mouse.previous[axis] = mouse.current[axis];
        }
        if (state != NULL) {
            f32 multiplier = (f32)devices[10].alternate_deadzone * 2.0f;
            multiplier *= multiplier;
            for (i32 axis = 0; axis < 3; ++axis) {
                mouse.current[axis] = (i16)state->axes[axis];
            }
            if (first_mouse) {
                first_mouse = 0;
                for (i32 axis = 0; axis < 3; ++axis) {
                    mouse.accumulated[axis] = 0;
                    mouse.previous[axis] = mouse.current[axis];
                }
                mouse.position[0] = 0.5f;
                mouse.position[1] = 0.5f;
            }
            for (i32 axis = 0; axis < 3; ++axis) {
                f32 change = (f32)(mouse.current[axis] - mouse.previous[axis]);
                if (change > 32767.0) {
                    change = (f32)(change - 65536.0);
                }
                if (change < -32768.0f) {
                    change = (f32)(change + 65536.0);
                }
                mouse.delta[axis] = axis < 2 ? multiplier * change : change;
            }
            mouse.accumulated[0] += mouse.delta[0] * 0.00390625;
            mouse.accumulated[1] += mouse.delta[1] * 0.00390625;
            mouse.accumulated[2] += mouse.delta[2];
            f32 reciprocal = 1.0f / PcInputAspectRatio();
            mouse.position[0] += mouse.delta[0] * reciprocal * 7.62939453125e-6;
            mouse.position[1] += mouse.delta[1] * 7.62939453125e-6;
            if (mouse.position[0] < 0.0f) {
                mouse.position[0] = 0.0f;
            } else if (mouse.position[0] > 1.0f) {
                mouse.position[0] = 1.0f;
            }
            if (mouse.position[1] < 0.0f) {
                mouse.position[1] = 0.0f;
            } else if (mouse.position[1] > 1.0f) {
                mouse.position[1] = 1.0f;
            }
            u32 previous = mbuttons;
            mbuttons = 0;
            u32 mask = 0;
            for (i32 button = 0; button < button_count; ++button) {
                if (mouse.buttons[button]) {
                    mask |= 1u << button;
                }
            }
            mbuttons = mask;
            PcMousePressedMask = (mask ^ previous) & mask;
            memset(mouse.buttons, 0, 8);
            PCByteCopy(mouse.buttons, state->buttons, button_count);
            return 1;
        }
        memset(mouse.buttons, 0, sizeof(mouse.buttons));
        mouse.accumulated[0] = mouse.accumulated[1] = mouse.accumulated[2] = mouse.delta[0] = mouse.delta[1] = mouse.delta[2] = 0.0f;
        for (i32 axis = 0; axis < 3; ++axis) {
            mouse.previous[axis] = mouse.current[axis];
        }
    }
    return 0;
}

void PC_INPUT_RUNTIME::ApplyMouseAxis(PC_INPUT_SAMPLE *sample) {
    (void)NuTimeGetFrameTime();
    PC_INPUT_RUNTIME *runtime = nu_input_state;
    f32 goal = 0.0f;
    f32 x = (f32)(runtime->mouse.delta[0] * -4.0);
    f32 y = (f32)(runtime->mouse.delta[1] * -4.0);
    f32 square = x * x + y * y;
    if (square > 100000.0f) {
        PcMouseTargetAngle = NuAtan2D(y, x);
        i32 difference = PcMouseTargetAngle - PcMouseCurrentAngle;
        if (difference > 32768) {
            PcMouseCurrentAngle += 65536;
        } else if (difference < -32768) {
            PcMouseCurrentAngle -= 65536;
        }
    }
    f32 elapsed = NuTimeGetFrameTime();
    f32 exponent = (f32)(elapsed * 4.0);
    PcMouseInterpolation = 1.0f - 1.0f / NuPow(2.0f, exponent);
    i32 difference = PcMouseTargetAngle - PcMouseCurrentAngle;
    f32 movement = PcMouseInterpolation * (f32)difference;
    f32 angle = movement + (f32)PcMouseCurrentAngle;
    PcMouseCurrentAngle = (i32)angle;
    difference = PcMouseTargetAngle - PcMouseCurrentAngle;
    if (difference > 32768) {
        difference -= 65536;
    } else if (difference < -32768) {
        difference += 65536;
    }
    if ((difference < 0 ? -difference : difference) > 100) {
        goal = 0.3f;
    }
    if (sample->analog[1] < 112) {
        goal = 0.7f;
    }
    if (sample->analog[1] > 144) {
        goal = 1.0f;
    }
    f32 radius_delta = goal - PcMouseRadius;
    radius_delta *= PcMouseInterpolation;
    PcMouseRadius += radius_delta;
    i32 cosine = ((PcMouseCurrentAngle + 16384) >> 1) & 32767;
    i32 sine = (PcMouseCurrentAngle >> 1) & 32767;
    f32 fx = -NuTrigTable[cosine] * PcMouseRadius;
    fx += 1.0f;
    f32 fy = -NuTrigTable[sine] * PcMouseRadius;
    fy += 1.0f;
    sample->analog[0] = (u8)(i32)(fy * 127.5);
    f32 scaled_x = (f32)(fx * 127.5);
    sample->analog[1] = (u8)(i32)scaled_x;
}

// Uncompiled PC control-flow reconstruction. All indexed records require native initialized domains.
extern u8 PcMouseAxisModes[3];

i32 PC_INPUT_RUNTIME::TranslateBindings(i32 port, PC_INPUT_SAMPLE *out, i32 valid, PC_INPUT_RAW_SAMPLE *raw, PC_INPUT_MOUSE_RUNTIME *motion,
                                        u8 **keys) {
    i32 step;
    if (ramp_seconds == 0.0f) {
        step = 32768;
    } else {
        step = (i32)(NuTimeGetFrameTime() * (32768.0 / ramp_seconds));
        if (step == 0) {
            step = 1;
        }
    }
    if (port >= 10 && port < 14 && keyboard_ui_override) {
        step = 32768;
    }
    i16 sums[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    u32 buttons = 0;
    for (i32 n = 0; n < shutdown_state; ++n) {
        PC_INPUT_BINDING *binding = &devices[port].bindings[n];
        PC_INPUT_COMMAND *command = &commands[n];
        i16 *retained = (i16 *)&devices[port].reset_pairs[n].first;
        i32 index = binding->value;
        u8 pressed = 0;
        u8 digital = 1;
        if (index >= 0) {
            i32 value;
            switch (binding->kind) {
                case PC_BINDING_MOUSE_BUTTON:
                    pressed = motion->buttons[index];
                    break;
                case PC_BINDING_MOUSE_AXIS:
                case PC_BINDING_MOUSE_AXIS_REVERSED:
                    if (valid) {
                        if (PcMouseAxisModes[index] == 0) {
                            value = (i32)motion->delta[index];
                            if (binding->kind == PC_BINDING_MOUSE_AXIS_REVERSED) {
                                value = -value;
                            }
                            pressed = value > 0;
                            value *= 8;
                        } else {
                            if (PcMouseAxisModes[index] == 2) {
                                if (motion->accumulated[index] > 256.0f) {
                                    motion->accumulated[index] = 256.0f;
                                }
                                if (motion->accumulated[index] < -256.0f) {
                                    motion->accumulated[index] = -256.0f;
                                }
                                i32 moved = (i32)motion->delta[index];
                                if ((moved < 0 ? -moved : moved) < 5) {
                                    PcMouseAxisIdle[index] += NuTimeGetFrameTime();
                                } else {
                                    PcMouseAxisIdle[index] = 0.0f;
                                }
                                if (PcMouseAxisIdle[index] > 0.5f) {
                                    f32 accumulated = motion->accumulated[index];
                                    if (accumulated != 0.0f) {
                                        if (accumulated > 0.0f && accumulated < 128.0f) {
                                            accumulated -= 1.0f;
                                        } else if (accumulated < 0.0f && accumulated > -128.0f) {
                                            accumulated += 1.0f;
                                        } else {
                                            accumulated = (f32)(accumulated - accumulated * 0.0078125);
                                        }
                                        motion->accumulated[index] = accumulated;
                                    }
                                }
                            }
                            value = (i32)motion->accumulated[index];
                            if (binding->kind == PC_BINDING_MOUSE_AXIS_REVERSED) {
                                value = -value;
                            }
                            if (value >= 32) {
                                value = ((value - 31) * 32512) / 96;
                            } else if (value < -32) {
                                value = ((value + 32) * 32512) / 96;
                            } else {
                                value = 0;
                            }
                        }
                        if (value > 32767) {
                            value = 32767;
                        }
                        if (value < -32767) {
                            value = -32767;
                        }
                        if (command->active == 2 || command->active == 3) {
                            *retained = value > 0 ? (i16)value : 0;
                        } else {
                            *retained = value < 0 ? (i16)value : 0;
                        }
                        digital = 0;
                    }
                    break;
                case PC_BINDING_KEY:
                    if (!suppress_key_1 || index != 1) {
                        pressed = (*keys)[index];
                    }
                    break;
                case PC_BINDING_JOYSTICK_BUTTON:
                    if (valid) {
                        pressed = raw->buttons[index];
                    }
                    break;
                case PC_BINDING_JOYSTICK_AXIS:
                case PC_BINDING_JOYSTICK_AXIS_REVERSED:
                    if (valid) {
                        value = raw->axes[index];
                        if (binding->kind == PC_BINDING_JOYSTICK_AXIS_REVERSED) {
                            value = -value;
                        }
                        if (value > 32767) {
                            value = 32767;
                        }
                        if (command->active == 2 || command->active == 3) {
                            *retained = value > 0 ? (i16)value : 0;
                        } else {
                            *retained = value < 0 ? (i16)value : 0;
                        }
                        i32 magnitude = *retained < 0 ? -*retained : *retained;
                        pressed = magnitude > axis_threshold;
                        digital = 0;
                    }
                    break;
            }
            if (command->button >= 0) {
                if (pressed || (!digital && (*retained > axis_threshold || *retained < -axis_threshold))) {
                    buttons |= 1u << command->button;
                }
            }
        }
        if (command->axis >= 0) {
            if (digital) {
                i32 value = *retained;
                if (pressed) {
                    if (command->active == 2 || command->active == 3) {
                        value += step;
                        if (value > 32767) {
                            value = 32767;
                        }
                    } else {
                        value -= step;
                        if (value < -32768) {
                            value = -32768;
                        }
                    }
                } else if (value < 0) {
                    value += step;
                    if (value > 0) {
                        value = 0;
                    }
                } else {
                    value -= step;
                    if (value < 0) {
                        value = 0;
                    }
                }
                *retained = (i16)value;
            }
            sums[command->axis] = (i16)((u16)sums[command->axis] + (u16)*retained);
        }
    }
    for (i32 axis = 0; axis < 4; ++axis) {
        out->analog[axis] = (u8)((sums[axis] + 32768) >> 8);
    }
    for (i32 axis = 4; axis < 8; ++axis) {
        out->analog[axis] = (u8)(sums[axis] >> 7);
    }
    out->buttons = buttons;
    return 1;
}

u32 PC_INPUT_RUNTIME::fun_006d82d0(i32, u32 buttons, i32 key, i32 mouse_button, i32 axis, i32 direction, u32 bit) {
    if (key >= 0 && nu_keyboard_pages[(i8)nu_keyboard_page_index * 256 + key]) {
        buttons |= bit;
    }
    if (mouse_button >= 0 && mouse.buttons[mouse_button]) {
        buttons |= bit;
    }
    if (axis >= 0) {
        if ((direction > 0 && mouse.delta[axis] > 0.0f) || (direction < 0 && mouse.delta[axis] < 0.0f)) {
            buttons |= bit;
        }
    }
    return buttons;
}

void PC_INPUT_RUNTIME::CaptureBinding(i32 port) {
    u32 mask = 1u << port;
    if (!(action_mask & mask) || (completed_mask & mask)) {
        return;
    }
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
            if (words[group] != 0) {
                i32 key = group * 4;
                for (i32 lane = 0; lane < 4; ++lane, ++key) {
                    if (key >= key_count) {
                        break;
                    }
                    if (keyboard_state[key]) {
                        captures[10].kind[0] = PC_BINDING_KEY;
                        captures[10].value[0] = (i16)key;
                        break;
                    }
                }
                break;
            }
        }
        if (nu_keyboard_owner.mouse_axes && nu_keyboard_owner.mouse_buttons && nu_keyboard_owner.mouse_axes >= 2 &&
            nu_keyboard_owner.mouse_buttons >= 2) {
            for (i32 button = 0; button < 8; ++button) {
                if (mouse.buttons[button]) {
                    captures[10].kind[1] = PC_BINDING_MOUSE_BUTTON;
                    captures[10].value[1] = (i16)button;
                    break;
                }
            }
            i32 x = (i32)mouse.accumulated[0];
            i32 y = (i32)mouse.accumulated[1];
            i32 z = (i32)mouse.accumulated[2];
            if (abs(x) > abs(y)) {
                if (abs(x) > motion_threshold) {
                    if (x > 0) {
                        captures[10].kind[2] = PC_BINDING_MOUSE_AXIS;
                        captures[10].value[2] = 0;
                    } else {
                        captures[10].kind[2] = PC_BINDING_MOUSE_AXIS_REVERSED;
                        captures[10].value[2] = 0;
                    }
                }
            } else if (abs(y) > motion_threshold) {
                if (y > 0) {
                    captures[10].kind[2] = PC_BINDING_MOUSE_AXIS;
                    captures[10].value[2] = 1;
                } else {
                    captures[10].kind[2] = PC_BINDING_MOUSE_AXIS_REVERSED;
                    captures[10].value[2] = 1;
                }
            }
            if (z) {
                if (z > 0) {
                    captures[10].kind[2] = PC_BINDING_MOUSE_AXIS;
                    captures[10].value[2] = 2;
                } else {
                    captures[10].kind[2] = PC_BINDING_MOUSE_AXIS_REVERSED;
                    captures[10].value[2] = 2;
                }
            }
        }
        return;
    }
    for (i32 axis = 0; axis < 32; ++axis) {
        if (raw_sample.axes[axis] > 16384) {
            captures[port].kind[2] = PC_BINDING_JOYSTICK_AXIS;
            captures[port].value[2] = (i16)axis;
            break;
        }
        if (raw_sample.axes[axis] < -16384) {
            captures[port].kind[2] = PC_BINDING_JOYSTICK_AXIS_REVERSED;
            captures[port].value[2] = (i16)axis;
            break;
        }
    }
    for (i32 button = 0; button < devices[port].button_count; ++button) {
        if (raw_sample.buttons[button]) {
            captures[port].kind[0] = PC_BINDING_JOYSTICK_BUTTON;
            captures[port].value[0] = (i16)button;
            break;
        }
    }
    for (i32 button = 128; button <= 131; ++button) {
        if (raw_sample.buttons[button]) {
            captures[port].kind[0] = PC_BINDING_JOYSTICK_BUTTON;
            captures[port].value[0] = (i16)button;
            break;
        }
    }
}

// Native input_runtime.cpp member.
void PC_INPUT_RUNTIME::NormalizeAxisPair(i16 *x, i16 *y) {
    f32 vx = (f32)*x;
    f32 vy = (f32)*y;
    f32 ax = vx < 0 ? -vx : vx;
    f32 ay = vy < 0 ? -vy : vy;
    f32 square = ax * ax + ay * ay;
    f32 magnitude = NuFsqrt(square);
    f32 ratio;
    if (ax > ay) {
        ratio = (f32)(32767.0 / ax);
    } else if (ay != 0.0f) {
        ratio = (f32)(32767.0 / ay);
    } else {
        ratio = 1.0f;
    }
    f32 scale = ratio * magnitude;
    scale = (f32)(scale * 0.000030517578125);
    vx = scale * vx;
    vy = scale * vy;
    if (vx < -32767.0) {
        vx = -32767.0f;
    }
    if (vx > 32767.0) {
        vx = 32767.0f;
    }
    if (vy < -32767.0) {
        vy = -32767.0f;
    }
    if (vy > 32767.0) {
        vy = 32767.0f;
    }
    *x = (i16)(i32)vx;
    *y = (i16)(i32)vy;
}

i32 PC_INPUT_RUNTIME::fun_006d49f0(i32 button) {
    for (i32 index = 0; index < shutdown_state; ++index) {
        if (commands[index].active == 3 && commands[index].button == button) {
            return index;
        }
    }
    return -1;
}

i32 PC_INPUT_RUNTIME::fun_006d4a30(i32 axis, i32 active) {
    for (i32 index = 0; index < shutdown_state; ++index) {
        if (commands[index].active == active && commands[index].axis == axis) {
            return index;
        }
    }
    return -1;
}

// Uncompiled native input_runtime.cpp lifecycle cohort.
void PC_INPUT_RUNTIME::ServicePendingCapture() {
    if (action_mask == 0) {
        return;
    }
    if (!keyboard_polled) {
        keyboard_polled = 1;
        keyboard_state = nu_keyboard_owner.PollKeyboard();
    }
    UpdateMouse();
    for (i32 port = 0; port < 14; ++port) {
        u32 mask = 1u << port;
        // Native 6d87a7 JNE skips SET bits: preserve this condition, even though
        // CaptureBinding then sees a clear bit and returns without recording.
        if (!(action_mask & mask)) {
            read_mask |= mask;
            action_mask &= ~mask;
            i32 valid = SampleDevice(port, &raw_sample);
            if (valid || (u32)(port - 10) <= 3) {
                CaptureBinding(port);
            }
        }
    }
}

void PC_INPUT_RUNTIME::AdvanceCaptureRecords() {
    for (i32 port = 0; port < 14; ++port) {
        staged_capture[port] = captures[port];
        captures[port].kind[2] = PC_BINDING_NONE;
        captures[port].kind[0] = PC_BINDING_NONE;
        captures[port].kind[1] = PC_BINDING_NONE;
        if (mouse.port_activity[port]) {
            --mouse.port_activity[port];
            if (staged_capture[port].kind[2] || staged_capture[port].kind[0] || staged_capture[port].kind[1]) {
                mouse.port_activity[port] = 5;
            }
        }
    }
    completed_mask = 0;
}

extern "C" const char *dat_02974d58[256];
extern "C" char dat_029753c0[512];

void PC_INPUT_RUNTIME::fun_006d4c90(u32 layout_id) {
    char *text = dat_029753c0;
    char name[16];
    sprintf(name, "%08.8i32x", layout_id);
    HKL layout = LoadKeyboardLayoutA(name, 1);
    for (i32 key = 0; key < 255; ++key) {
        dat_02974d58[key] = "?";
        u32 character = MapVirtualKeyExA(key, 3, layout);
        if (character >= 0x20 && character <= 0x5a) {
            dat_02974d58[key] = text;
            text[0] = character;
            text[1] = 0;
            text += 2;
        }
    }
}

void PC_INPUT_RUNTIME::fun_006d4d20(i32 value) {
    i32 previous = keyboard_ui_override;
    keyboard_ui_override = value;
    if (previous != value) {
        nu_keyboard_repeat_countdown = 6;
    }
}

i32 PC_INPUT_RUNTIME::fun_006d4d50() {
    return keyboard_ui_override;
}

extern "C" void fun_006d4d60(void) {
}

void PC_INPUT_RUNTIME::fun_006d4d70() {
    mouse.accumulated[0] = mouse.accumulated[1] = mouse.accumulated[2] = 0.0f;
}

void PC_INPUT_RUNTIME::ResetInputRecords() {
    for (i32 i = 0; i < 32; ++i) {
        commands[i].active = 0;
        commands[i].button = -1;
        commands[i].axis = -1;
        commands[i].unknown_04 = 0;
    }
    shutdown_state = 0;
}

void PC_INPUT_RUNTIME::fun_006d4e20(i32 value) {
    mouse.unknown_58 = value;
}

i32 PC_INPUT_RUNTIME::GetInputRecordCount() {
    return shutdown_state;
}

i32 PC_INPUT_RUNTIME::fun_006d4e40() {
    return unknown_67e8;
}

i32 PC_INPUT_RUNTIME::fun_006d4e50() {
    return unknown_67ec;
}

void PC_INPUT_RUNTIME::InsertInputRecord(i32 index, i32 unknown_00, i32 field_01, i32 unknown_02, u16 unknown_04) {
    if ((u32)index > 31) {
        return;
    }
    for (i32 i = 0; i < 32; ++i) {
        if (unknown_00 == commands[i].active) {
            if (field_01 >= 0 && field_01 == commands[i].axis) {
                return;
            }
            if (unknown_02 >= 0 && unknown_02 == commands[i].button) {
                return;
            }
        }
    }
    commands[index].active = (i8)unknown_00;
    commands[index].axis = (i8)field_01;
    commands[index].button = (i8)unknown_02;
    commands[index].unknown_04 = unknown_04;
}

i32 PC_INPUT_RUNTIME::fun_006d4ef0(i32 action, i32 *active, i32 *axis, i32 *button, i32 *value) {
    if (action < 0 || action >= 32) {
        return 0;
    }
    if (active != NULL) {
        *active = commands[action].active;
    }
    if (axis != NULL) {
        *axis = commands[action].axis;
    }
    if (button != NULL) {
        *button = commands[action].button;
    }
    if (value != NULL) {
        *value = commands[action].unknown_04;
    }
    return 1;
}

i32 PC_INPUT_RUNTIME::fun_006d4f60(i32 active, i32 code) {
    i32 button;
    i32 axis;
    i32 mode;
    for (i32 action = 0; action < 32; ++action) {
        fun_006d4ef0(action, &mode, &axis, &button, NULL);
        if (mode == active) {
            if (active == 3) {
                if (button == code) {
                    return action;
                }
            } else if ((active == 1 || active == 2) && axis == code) {
                return action;
            }
        }
    }
    return -1;
}

void PC_INPUT_RUNTIME::fun_006d4fc0(const char *keys) {
    blocked_keys[0] = 0;
    if (keys != NULL) {
        NuStrCpy(blocked_keys, keys);
    }
}

i32 PC_INPUT_RUNTIME::fun_006d4fe0(i32 port, i32 kind, i32 value) {
    for (i32 action = 0; action < 32; ++action) {
        if (devices[port].bindings[action].kind == kind && devices[port].bindings[action].value == value) {
            return 1;
        }
    }
    return 0;
}

/* Original source name unknown. PlayersDropInOut calls it rather than folding the constant. */
extern "C" i32 PcInput_GetFirstKeyboardPort(void) {
    return 10;
}

extern "C" void fun_006d5050(const i8 *order) {
    i8 *destination;
    i32 value;
    PcInputActionOrder[0] = -1;
    if (order != NULL) {
        destination = PcInputActionOrder;
        do {
            value = *order++;
            *destination++ = value;
        } while (value >= 0);
    }
}

extern "C" void PcInput_SetStringUCGfxRemapFn(void (*callback)(i32, char *)) {
    TextInputNameCallback = callback;
}

i32 PC_INPUT_RUNTIME::fun_006d4de0(i32 port, i32 action, i32 auxiliary, i32 bank) {
    if (auxiliary) {
        return auxiliary_bindings[bank][action].kind != PC_BINDING_NONE;
    }
    return devices[port].bindings[action].kind != PC_BINDING_NONE;
}

extern "C" i32 fun_006d5d40(i32 port, i32 kind, i32 value) {
    return nu_input_state->fun_006d4fe0(port, kind, value);
}

extern "C" void PcInputEndFrame(void) {
    nu_input_state->EndFrame();
}

// Append to input_runtime.cpp; declarations/mappings in B-contract.json.

void PC_INPUT_RUNTIME::CopyBindings(i32 port, i32 to_auxiliary, i32 bank) {
    if (port < 0) {
        return;
    }
    if (to_auxiliary) {
        auxiliary_ports[bank] = port;
        PCByteCopy(auxiliary_bindings[bank], devices[port].bindings, 0x80);
    } else {
        PCByteCopy(devices[port].bindings, auxiliary_bindings[bank], 0x80);
    }
}

extern "C" void PcInputCopyBindings(i32 port, i32 to_auxiliary, i32 bank) {
    nu_input_state->CopyBindings(port, to_auxiliary, bank);
}

extern "C" f32 fun_006d5b00(void) {
    PC_INPUT_RUNTIME *runtime = nu_input_state;
    return runtime->mouse.accumulated[0];
}

extern "C" f32 fun_006d5b10(void) {
    PC_INPUT_RUNTIME *runtime = nu_input_state;
    return runtime->mouse.accumulated[1];
}

extern "C" f32 fun_006d5b20(void) {
    PC_INPUT_RUNTIME *runtime = nu_input_state;
    return runtime->mouse.accumulated[2];
}

extern "C" void PcInputResetMouseMotion() {
    PC_INPUT_RUNTIME *runtime = nu_input_state;
    runtime->mouse.accumulated[2] = 0.0f;
    runtime->mouse.accumulated[1] = 0.0f;
    runtime->mouse.accumulated[0] = 0.0f;
}

extern "C" i32 fun_006d5dd0(i32 player) {
    i32 port = NuPadGetPort(player);
    if (port == 10 || port == 12) {
        return 1;
    }
    return 0;
}

extern "C" i32 fun_006d5df0(i32 player) {
    i32 port = NuPadGetPort(player);
    if (port == 10 || port == 12) {
        return 1;
    }
    return 0;
}

extern "C" i32 fun_006d5e10(i32 player) {
    i32 port = NuPadGetPort(player);
    return port >= 10 && port <= 13;
}

extern "C" void fun_006d5e30(void (*callback)(PC_INPUT_RUNTIME *)) {
    PcInputStateInitializedCallback = callback;
}

extern "C" void fun_006d5e40(u32 *buttons, u32 *axes, i32 bank) {
    PC_INPUT_RUNTIME *runtime = nu_input_state;
    u32 button_mask = 0;
    u32 axis_mask = 0;
    for (i32 action = 0; action < 32; ++action) {
        i32 active;
        i32 axis;
        i32 button;
        if (runtime->fun_006d4ef0(action, &active, &axis, &button, NULL) && active != 0 &&
            runtime->auxiliary_bindings[bank][action].kind != PC_BINDING_NONE) {
            if (button >= 0) {
                button_mask |= 1 << button;
            }
            if (axis >= 0) {
                axis_mask |= 1 << axis;
            }
        }
    }
    *buttons = button_mask;
    *axes = axis_mask;
}

extern "C" u32 fun_006d5ed0(i32 bank) {
    PC_INPUT_RUNTIME *runtime = nu_input_state;
    u32 mask = 0;
    for (i32 action = 0; action < 32; ++action) {
        i32 active;
        if (runtime->fun_006d4ef0(action, &active, NULL, NULL, NULL) && active != 0 &&
            runtime->auxiliary_bindings[bank][action].kind != PC_BINDING_NONE) {
            mask |= 1 << action;
        }
    }
    return mask;
}

extern "C" i32 fun_006d5f20(u32 buttons, u32 axes) {
    i32 action;
    i32 active;
    i32 axis;
    i32 button;
    i32 value;
    if (buttons == 0 && axes == 0) {
        return -2;
    }
    if (buttons != 0 && axes != 0) {
        return -3;
    }
    if (buttons != 0) {
        for (action = 0; action < 32; ++action) {
            if (nu_input_state->fun_006d4ef0(action, &active, &axis, &button, &value) && button >= 0 && (buttons & (1 << button))) {
                if (buttons & ~(1 << button)) {
                    return -4;
                }
                return action;
            }
        }
    } else {
        for (action = 0; action < 32; ++action) {
            if (nu_input_state->fun_006d4ef0(action, NULL, &axis, NULL, NULL) && axis >= 0 && (axes & (1 << axis))) {
                if (axes & ~(1 << axis)) {
                    return -5;
                }
                return action;
            }
        }
    }
    return -1;
}

extern "C" char *PcInputDeviceName(i32 port) {
    return nu_input_state->devices[port].name;
}

extern "C" void PcInputSuppressKeyOne(i32 value) {
    nu_input_state->suppress_key_1 = (u8)value;
}

extern "C" void PcInputSetMouseField(i32 value) {
    nu_input_state->mouse.unknown_58 = value;
}

extern "C" void PcInputSetCapturedPort(i32 port) {
    nu_input_state->captured_port = port;
}

extern "C" i32 PcInputCapturedPort() {
    return nu_input_state->captured_port;
}

extern "C" void PcInputSetBlockedKeys(const char *keys) {
    PC_INPUT_RUNTIME *runtime = nu_input_state;
    runtime->blocked_keys[0] = 0;
    if (keys != NULL) {
        NuStrCpy(runtime->blocked_keys, keys);
    }
}

extern "C" i32 fun_006d6760(void) {
    PC_INPUT_RUNTIME *state = nu_input_state;
    i32 i;
    for (i = 0; i <= 3; ++i) {
        if (state->devices[i].kind == PC_INPUT_DEVICE_XINPUT && state->devices[i].unknown_29 != 0) {
            return 1;
        }
    }
    return 0;
}

extern "C" i32 PcInputIsMouseDevice(i32 port) {
    return port == 10 || port == 12;
}

extern "C" i32 PcInputIsJoystickDevice(i32 port) {
    return (u32)port <= 9;
}

extern "C" i32 PcInputMixConfigurationHash(i32 previous, i32 value) {
    if (previous & 0x80000000) {
        return (previous * 2 + 1) ^ value ^ 0x12345678;
    }
    return (previous * 2) ^ value ^ 0x12345678;
}

i32 PC_INPUT_RUNTIME::HashAuxiliaryBindings() {
    i32 hash = 0;
    for (i32 bank = 0; bank < 4; ++bank) {
        for (i32 action = 0; action < 32; ++action) {
            hash = ((hash & 0x80000000) ? hash * 2 + 1 : hash * 2) ^ auxiliary_bindings[bank][action].value ^ 0x12345678;
            hash = ((hash & 0x80000000) ? hash * 2 + 1 : hash * 2) ^ auxiliary_bindings[bank][action].kind ^ 0x12345678;
        }
    }
    return hash;
}

i32 PC_INPUT_RUNTIME::fun_006d38b0(PC_INPUT_CONFIG *config) {
    for (i32 index = 0; index < 64; ++index) {
        if (config->product_id == primary[index].product_id && config->key == primary[index].key) {
            return index;
        }
    }
    return -1;
}

extern "C" i32 PcInputHashAuxiliaryBindings() {
    return nu_input_state->HashAuxiliaryBindings();
}

extern "C" void PcInputInstallConfig(PC_INPUT_CONFIG *config, i32 port, i32 key, i32 auxiliary, i32 bank) {
    nu_input_state->InstallConfig(config, port, key, auxiliary, bank);
}

extern "C" i32 PcInputBindingPresent(i32 port, i32 action, i32 auxiliary, i32 bank) {
    return auxiliary ? nu_input_state->auxiliary_bindings[bank][action].kind != PC_BINDING_NONE
                     : nu_input_state->devices[port].bindings[action].kind != PC_BINDING_NONE;
}

extern "C" PC_INPUT_CONFIG *PcInputFindConfig(u8 mask, i32 port, i32 key) {
    return nu_input_state->FindConfig(mask, port, key);
}

i32 PC_INPUT_RUNTIME::BindingAvailable(i32 bank, i32 bank_count, i32 kind, i32 value) {
    i32 port;
    const char *key;
    i32 blocked;
    const i8 *order;
    PC_INPUT_BINDING *bindings;
    i32 other;
    i32 action;

    port = auxiliary_ports[bank];
    if (port < 10 || port > 13) {
        return 1;
    }
    if (kind == PC_BINDING_KEY) {
        key = blocked_keys;
        while ((blocked = *key++) != 0) {
            if (value == blocked) {
                return 0;
            }
        }
    }
    bindings = auxiliary_bindings[0];
    for (other = 0; other < bank_count; ++other, bindings += 32) {
        if (other != bank) {
            port = auxiliary_ports[other];
            if (port != -1 && port >= 10 && port <= 13) {
                if (PcInputActionOrder[0] >= 0) {
                    order = PcInputActionOrder;
                    while ((action = *order++) >= 0) {
                        if (value == bindings[action].value && kind == bindings[action].kind) {
                            return 0;
                        }
                    }
                } else {
                    for (action = 0; action < 32; ++action) {
                        if (value == bindings[action].value && kind == bindings[action].kind) {
                            return 0;
                        }
                    }
                }
            }
        }
    }
    return 1;
}

extern "C" i32 PcInputBindingAvailable(i32 bank, i32 bank_count, i32 kind, i32 value) {
    return nu_input_state->BindingAvailable(bank, bank_count, kind, value);
}

void PC_INPUT_RUNTIME::ClearOtherBankConflicts(i32 bank, i32 bank_count) {
    PC_INPUT_BINDING *source;
    PC_INPUT_BINDING *target;
    i32 port;
    i32 other;
    i32 action;

    port = auxiliary_ports[bank];
    if (port < 10 || port > 13) {
        return;
    }
    source = auxiliary_bindings[bank];
    for (other = 0; other < bank_count; ++other) {
        if (other != bank) {
            port = auxiliary_ports[other];
            if (port >= 10 && port <= 13) {
                target = auxiliary_bindings[other];
                for (action = 0; action < 32; ++action) {
                    if (source[action].value == target[action].value && source[action].kind == target[action].kind) {
                        target[action].value = 0;
                        target[action].kind = PC_BINDING_NONE;
                    }
                }
            }
        }
    }
}

extern "C" void PcInputClearOtherBankConflicts(i32 bank, i32 bank_count) {
    nu_input_state->ClearOtherBankConflicts(bank, bank_count);
}

extern "C" i32 PcInputResolveAuxiliaryConflicts(i32 bank, i32 bank_count) {
    return nu_input_state->ResolveAuxiliaryConflicts(bank, bank_count);
}

void PC_INPUT_RUNTIME::InitializeDefaultRecords() {
    ResetInputRecords();
    AddInputRecord(1, 0, -1, 0);
    AddInputRecord(2, 0, -1, 0);
    AddInputRecord(1, 1, -1, 0);
    AddInputRecord(2, 1, -1, 0);
    AddInputRecord(1, 2, -1, 0);
    AddInputRecord(2, 2, -1, 0);
    AddInputRecord(1, 3, -1, 0);
    AddInputRecord(2, 3, -1, 0);
    AddInputRecord(3, -1, 12, 0);
    AddInputRecord(3, -1, 13, 0);
    AddInputRecord(3, -1, 14, 0);
    AddInputRecord(3, -1, 15, 0);
    AddInputRecord(3, -1, 4, 0);
    AddInputRecord(3, -1, 5, 0);
    AddInputRecord(3, -1, 6, 0);
    AddInputRecord(3, -1, 7, 0);
    AddInputRecord(2, 4, 2, 0);
    AddInputRecord(2, 5, 0, 0);
    AddInputRecord(2, 6, 3, 0);
    AddInputRecord(2, 7, 1, 0);
    AddInputRecord(3, -1, 9, 0);
    AddInputRecord(3, -1, 10, 0);
    AddInputRecord(3, -1, 8, 0);
    AddInputRecord(3, -1, 11, 0);
}

i32 PC_INPUT_RUNTIME::fun_006d99e0(i32 port, u8 mask) {
    return FindConfig(mask, port, 0) != NULL;
}

extern "C" i32 PcInputAlternateDeadzone(i32 port) {
    if ((u32)(port - 10) > 3) {
        return 0;
    }
    return nu_input_state->devices[port].alternate_deadzone;
}

extern "C" void PcInputSetDeadzone(i32 port, i32 value) {
    PC_INPUT_DEVICE *device = &nu_input_state->devices[port];
    if ((u32)(port - 10) > 3) {
        device->deadzone = value;
    } else {
        device->deadzone = 0;
    }
}

extern "C" void PcInputSetAlternateDeadzone(i32 port, i32 value) {
    PC_INPUT_DEVICE *device = &nu_input_state->devices[port];
    if ((u32)(port - 10) <= 3) {
        device->alternate_deadzone = value;
    } else {
        device->alternate_deadzone = 0;
    }
}

extern "C" void NuPadSetMotorsPS(i32 port, i32 motor0, i32 motor1) {
    PC_INPUT_DEVICE *device = &nu_input_state->devices[port];
    if (port >= 0 && port < 4) {
        XINPUT_VIBRATION vibration;
        vibration.wLeftMotorSpeed = device->left_trigger * motor1 / 256;
        if (motor0 != 0) {
            motor0 = 1;
        }
        vibration.wRightMotorSpeed = device->right_trigger * motor0;
        XInputSetState(port, &vibration);
    }
}

extern "C" i32 fun_006d9490(i32 player) {
    i32 port = NuPadGetPort(player);
    return port >= 0 && port <= 3;
}

extern "C" PC_INPUT_COMMAND_FILTER PcInputGetCommandFilter() {
    return PcInputCommandFilter;
}

i32 PC_INPUT_RUNTIME::fun_006d6db0(i32 port, i32 index) {
    if (port >= 10 && port <= 13) {
        if (index != captured_port) {
            ++port;
        }
        if (index >= 0) {
            port += index * 2;
        }
    }
    return port;
}

void PC_INPUT_RUNTIME::AddInputRecord(i32 unknown_00, i32 field_01, i32 unknown_02, u16 unknown_04) {
    if (shutdown_state < 32) {
        InsertInputRecord(shutdown_state, unknown_00, field_01, unknown_02, unknown_04);
        ++shutdown_state;
    }
}

void PC_INPUT_RUNTIME::AssignBinding(i32 action, i32 port, i32 kind, i32 value, i32 unused, i32 auxiliary, i32 bank, u32 excluded) {
    PC_INPUT_BINDING *bindings = auxiliary ? auxiliary_bindings[bank] : devices[port].bindings;
    i32 simple = (u32)port <= 3;
    i32 active = commands[action].active;
    i32 positive;
    i32 negative;
    if (kind == PC_BINDING_MOUSE_AXIS || kind == PC_BINDING_MOUSE_AXIS_REVERSED) {
        positive = PC_BINDING_MOUSE_AXIS;
        negative = PC_BINDING_MOUSE_AXIS_REVERSED;
    } else {
        positive = PC_BINDING_JOYSTICK_AXIS;
        negative = PC_BINDING_JOYSTICK_AXIS_REVERSED;
    }
    if (!(PcInputAxisRemapInitialized & 1)) {
        PcInputAxisRemapInitialized |= 1;
        PcInputAxisRemap[10] = (u8)positive;
        PcInputAxisRemap[11] = (u8)positive;
        PcInputAxisRemap[12] = (u8)negative;
        PcInputAxisRemap[13] = (u8)negative;
        PcInputAxisRemap[14] = 0;
        PcInputAxisRemap[15] = 0;
    }
    if (kind != positive && kind != negative) {
        for (i32 i = 0; i < 32; ++i) {
            if (bindings[i].kind == kind && bindings[i].value == value && !(excluded & (1u << i))) {
                bindings[i].kind = PC_BINDING_NONE;
                bindings[i].value = 0;
            }
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
            if (i != action && (other_kind == positive || other_kind == negative) && bindings[i].value == value) {
                i32 index = 0;
                if (kind == positive) {
                    index = 4;
                }
                if (other_kind == positive) {
                    index += 2;
                }
                if (commands[i].active == 2) {
                    ++index;
                }
                if (commands[i].active == 3) {
                    index += 8;
                }
                u8 remapped_kind = PcInputAxisRemap[index];
                bindings[i].kind = remapped_kind;
                bindings[i].value = (u16)(remapped_kind ? value : 0);
            }
        }
        return;
    }
    if (kind == positive && active == 2) {
        kind = positive;
    } else if (kind == negative && active == 1) {
        kind = positive;
    } else {
        kind = negative;
    }
    for (i32 i = 0; i < 32; ++i) {
        if ((bindings[i].kind == positive || bindings[i].kind == negative) && bindings[i].value == value && !(excluded & (1u << i))) {
            bindings[i].kind = PC_BINDING_NONE;
            bindings[i].value = 0;
        }
    }
    i32 axis = commands[action].axis;
    if (axis < 0) {
        return;
    }
    if (simple && (value == 6 || value == 7)) {
        bindings[action].value = (u16)value;
        bindings[action].kind = (u8)kind;
        return;
    }
    if (active == 2 || active == 1) {
        for (i32 i = 0; i < 32; ++i) {
            if ((commands[i].active == 2 || commands[i].active == 1) && commands[i].axis == axis) {
                bindings[i].kind = (u8)kind;
                bindings[i].value = (u16)value;
            }
        }
    }
}

extern "C" void PcInputAssignBinding(i32 action, i32 port, i32 kind, i32 value, i32 unused, i32 auxiliary, i32 bank, u32 excluded) {
    nu_input_state->AssignBinding(action, port, kind, value, unused, auxiliary, bank, excluded);
}

extern "C" void fun_006d9250(i32 button, i32 port, i32 kind, i32 value, i32 unused, i32 auxiliary, i32 bank, u32 excluded) {
    PC_INPUT_RUNTIME *runtime = nu_input_state;
    i32 command_button;
    for (i32 action = 0; action < 32; ++action) {
        runtime->fun_006d4ef0(action, NULL, NULL, &command_button, NULL);
        if (command_button == button) {
            runtime->AssignBinding(action, port, kind, value, unused, auxiliary, bank, excluded);
            return;
        }
    }
}

extern "C" void fun_006d92c0(i32 axis, i32 active, i32 port, i32 kind, i32 value, i32 unused, i32 auxiliary, i32 bank, u32 excluded) {
    PC_INPUT_RUNTIME *runtime = nu_input_state;
    i32 command_active;
    i32 command_axis;
    for (i32 action = 0; action < 32; ++action) {
        runtime->fun_006d4ef0(action, &command_active, &command_axis, NULL, NULL);
        if (command_axis == axis && command_active == active) {
            runtime->AssignBinding(action, port, kind, value, unused, auxiliary, bank, excluded);
            return;
        }
    }
}

// Append to input_runtime.cpp. Extend config unknown_44 with metadata named in B-layout.txt.
struct PC_INPUT_EXPORT_METADATA { // size 0xc
    i16 button_count;             // 0x0
    i16 pov_count;                // 0x2
    i16 axis_count;               // 0x4
    u16 deadzone_word;            // 0x6
    u16 alternate_deadzone_word;  // 0x8
    i16 key;                      // 0xa
};

void PC_INPUT_RUNTIME::ExportConfig(i32 port, PC_INPUT_CONFIG *config, i32 auxiliary, i32 bank, i32 key) {
    PC_INPUT_BINDING *bindings = auxiliary ? auxiliary_bindings[bank] : devices[port].bindings;
    for (i32 i = 0; i < 8; ++i) {
        config->all_axes[i][0] = 0;
        config->all_axes[i][1] = 255;
        config->all_axes[i][2] = 255;
    }
    for (i32 i = 0; i < 16; ++i) {
        config->buttons[i][0] = 0;
        config->buttons[i][1] = 0;
    }
    for (i32 action = 0; action < shutdown_state; ++action) {
        i32 button = commands[action].button;
        i32 active = commands[action].active;
        i32 axis = commands[action].axis;
        i32 kind = bindings[action].kind;
        if (!kind) {
            continue;
        }
        if (axis >= 0) {
            u8 *triplet = config->all_axes[axis];
            if (kind == 12 || kind == 13) {
                triplet[0] = (u8)kind;
                u8 value = (u8)bindings[action].value;
                triplet[1] = value;
                triplet[2] = value;
            } else if (kind == PC_BINDING_JOYSTICK_AXIS || kind == PC_BINDING_JOYSTICK_AXIS_REVERSED) {
                triplet[1] = (u8)bindings[action].value;
                triplet[2] = (u8)bindings[action].value;
                if (active == 3) {
                    triplet[0] = (u8)kind;
                } else if ((active == 2 && kind == PC_BINDING_JOYSTICK_AXIS) ||
                           (active == 1 && kind == PC_BINDING_JOYSTICK_AXIS_REVERSED)) {
                    triplet[0] = 2;
                } else {
                    triplet[0] = 3;
                }
            } else if (kind == PC_BINDING_MOUSE_AXIS || kind == PC_BINDING_MOUSE_AXIS_REVERSED) {
                triplet[1] = (u8)bindings[action].value;
                triplet[2] = (u8)bindings[action].value;
                if (active == 3) {
                    triplet[0] = (u8)kind;
                } else if ((active == 2 && kind == PC_BINDING_MOUSE_AXIS) || (active == 1 && kind == PC_BINDING_MOUSE_AXIS_REVERSED)) {
                    triplet[0] = 5;
                } else {
                    triplet[0] = 6;
                }
            } else if (kind == PC_BINDING_JOYSTICK_BUTTON) {
                triplet[0] = 11;
                if (active == 2 || active == 3) {
                    triplet[2] = (u8)bindings[action].value;
                } else {
                    triplet[1] = (u8)bindings[action].value;
                }
            } else if (kind == PC_BINDING_KEY) {
                triplet[0] = 10;
                if (active == 2 || active == 3) {
                    triplet[2] = (u8)bindings[action].value;
                } else {
                    triplet[1] = (u8)bindings[action].value;
                }
            }
        }
        if (button >= 0 && (kind >= 1 && kind <= 7 || kind == 12 || kind == 13)) {
            config->buttons[button][0] = (u8)kind;
            config->buttons[button][1] = (u8)bindings[action].value;
        }
    }
    for (i32 i = 0; i < 8; ++i) {
        if (config->all_axes[i][1] == 255 || config->all_axes[i][2] == 255) {
            config->all_axes[i][0] = 0;
            config->all_axes[i][2] = 0;
            config->all_axes[i][1] = 0;
        }
    }
    memset(config->name, 0, sizeof(config->name));
    PCByteCopy(config->name, devices[port].name, NuStrLen(devices[port].name));
    config->product_id = devices[port].product.Data1;
    config->guid_data2 = devices[port].product.Data2;
    config->guid_data3 = devices[port].product.Data3;
    for (i32 i = 0; i < 8; ++i) {
        config->guid_data4[i] = devices[port].product.Data4[i];
    }
    PC_INPUT_EXPORT_METADATA *metadata = (PC_INPUT_EXPORT_METADATA *)&config->button_count;
    metadata->button_count = devices[port].button_count;
    metadata->pov_count = devices[port].pov_count;
    metadata->axis_count = devices[port].axis_count;
    u16 deadzone = devices[port].deadzone;
    i16 saved_key = (i16)key;
    metadata->deadzone_word = deadzone;
    metadata->alternate_deadzone_word = devices[port].alternate_deadzone;
    metadata->key = saved_key;
}

void PC_INPUT_RUNTIME::PromoteConfig(PC_INPUT_CONFIG *records, i32 count, PC_INPUT_CONFIG *config) {
    i32 index = 0;
    for (; index < count; ++index) {
        if (config->product_id == records[index].product_id && records[index].key == config->key) {
            break;
        }
    }
    if (index == count) {
        index = count - 1;
    }
    if (index > 0) {
        PCByteCopy(records + 1, records, index * sizeof(*records));
    }
    PCByteCopy(records, config, sizeof(*records));
}

void PC_INPUT_RUNTIME::fun_006d3970(PC_INPUT_CONFIG *config) {
    PromoteConfig(primary, 64, config);
}

void PC_INPUT_RUNTIME::fun_006d3980(i32) {
}

extern "C" void PcInputStageAuxiliaryConfig(i32 port, i32 bank, i32 key) {
    PC_INPUT_CONFIG config;
    PC_INPUT_RUNTIME *runtime = nu_input_state;
    runtime->ExportConfig(port, &config, 1, bank, key);
    runtime->PromoteConfig(runtime->primary, 64, &config);
}

// Append input_runtime.cpp. Native strings/table definitions and mappings in B-save-data.json.

void PC_INPUT_RUNTIME::SanitizeConfigName(const char *name, char *destination) {
    PCByteCopy(destination, name, 63);
    destination[63] = 0;
    char *out = destination;
    for (char *in = destination; *in; ++in) {
        char c = *in;
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_') {
            *out++ = c;
        }
    }
    *out = 0;
}

void PC_INPUT_RUNTIME::FormatConfigText(char *destination, PC_INPUT_CONFIG *config) {
    char text[256];
    char part[256];
    char character[4];
    sprintf(text, "\t\"%s\",\t0x%x,%d,%d, {", config->name, config->product_id, config->guid_data2, config->guid_data3);
    for (i32 i = 0; i < 8; ++i) {
        u32 c = config->guid_data4[i];
        if (c < 32) {
            sprintf(part, "%d", c);
        } else {
            character[0] = (char)c;
            character[1] = 0;
            sprintf(part, "'%s'", character);
        }
        strcat(text, part);
        strcat(text, i < 7 ? "," : "}, //Guid stuff\n");
    }
    NuStrCpy(destination, text);
    sprintf(text, "\t%d,%d,%d,%d,%d,%d,0,0,\t//NumButtons etc.\n", config->button_count, config->pov_count, config->axis_count,
            (i16)((u16)config->deadzone | ((u16)config->unknown_5b << 8)),
            (i16)((u16)config->alternate_deadzone | ((u16)config->unknown_5d << 8)), config->key);
    NuStrCat(destination, text);
    NuStrCpy(text, "\t");
    for (i32 i = 0; i < 4; ++i) {
        sprintf(part, "%s,%d,%d, ", PcInputBindingKindNames[config->axes[i][0]], config->axes[i][1], config->axes[i][2]);
        NuStrCat(text, part);
    }
    NuStrCat(text, "//analogue thumb sticks\n");
    NuStrCat(destination, text);
    NuStrCpy(text, "\t");
    for (i32 i = 0; i < 4; ++i) {
        sprintf(part, "%s,%d,%d, ", PcInputBindingKindNames[config->unknown_axes[i][0]], config->unknown_axes[i][1],
                config->unknown_axes[i][2]);
        NuStrCat(text, part);
    }
    NuStrCat(text, "//analogue shoulder buttons\n");
    NuStrCat(destination, text);
    NuStrCpy(text, "\t");
    for (i32 i = 0; i < 4; ++i) {
        sprintf(part, "%s,%d, ", PcInputBindingKindNames[config->buttons[i][0]], config->buttons[i][1]);
        NuStrCat(text, part);
    }
    NuStrCat(text, "//l2,r2,l1,r1\n");
    NuStrCat(destination, text);
    NuStrCpy(text, "\t");
    for (i32 i = 4; i < 8; ++i) {
        sprintf(part, "%s,%d, ", PcInputBindingKindNames[config->buttons[i][0]], config->buttons[i][1]);
        NuStrCat(text, part);
    }
    NuStrCat(text, "//rup,rright,rdown,rleft\n");
    NuStrCat(destination, text);
    NuStrCpy(text, "\t");
    for (i32 i = 8; i < 12; ++i) {
        sprintf(part, "%s,%d, ", PcInputBindingKindNames[config->buttons[i][0]], config->buttons[i][1]);
        NuStrCat(text, part);
    }
    NuStrCat(text, "//select,l3,r3,start\n");
    NuStrCat(destination, text);
    NuStrCpy(text, "\t");
    for (i32 i = 12; i < 16; ++i) {
        sprintf(part, "%s,%d, ", PcInputBindingKindNames[config->buttons[i][0]], config->buttons[i][1]);
        NuStrCat(text, part);
    }
    NuStrCat(text, "//lup,lright,ldown,lleft\n");
    NuStrCat(destination, text);
}

void PC_INPUT_RUNTIME::WriteConfigText(const char *directory, PC_INPUT_CONFIG *config) {
    char name[64];
    char path[1024];
    char text[2036];
    if (!config->name[0]) {
        return;
    }
    SanitizeConfigName(config->name, name);
    sprintf(path, "%s%s_v%d.txt", directory, name, config->key);
    FILE *file = PcOpenPlatformFile(path, "", 0, "w");
    if (file != NULL) {
        FormatConfigText(text, config);
        fwrite(text, NuStrLen(text), 1, file);
        fclose(file);
    }
}

void PC_INPUT_RUNTIME::fun_006d6eb0(i32 port, PC_INPUT_CONFIG *config) {
    ExportConfig(port, config, 0, 0, 0);
}

PC_INPUT_CONFIG *PC_INPUT_RUNTIME::fun_006d6ed0(u8 mask, i32 port) {
    return FindConfig(mask, port, 0);
}

void PC_INPUT_RUNTIME::fun_006d6ef0(u8 mask, i32 port, i32 key, i32 auxiliary, i32 auxiliary_port, i32 fallback) {
    PC_INPUT_CONFIG *config = FindConfig(mask, port, key);
    if (config == NULL) {
        if (!fallback) {
            return;
        }
        config = FindConfig(mask, port, -1);
        if (config == NULL) {
            return;
        }
    }
    InstallConfig(config, port, config->key, auxiliary, auxiliary_port);
}

void PC_INPUT_RUNTIME::WriteConfiguration(PC_INPUT_CONFIG *records, i32 count, const char *directory) {
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
    for (i32 i = 0; i < count; ++i) {
        if (records[i].name[0] && PcInputWriteTextProfiles) {
            WriteConfigText(directory, &records[i]);
        }
    }
}

void PC_INPUT_RUNTIME::SaveConfiguration() {
    char directory[1024];
    /* Retail passes only the folder; the two missing %s read the company and product arguments that
       PcGetUserFolder left on the stack (cdecl cleanup is deferred). */
    sprintf(directory, "%s\\%s\\%s\\", PcGetUserFolder(PcCompanyName, PcProductName));
    WriteConfiguration(primary, 64, directory);
}

extern "C" void PcInputSaveConfiguration() {
    nu_input_state->SaveConfiguration();
}

u8 PcInputAxisRemap[16] = {0};
u32 PcInputAxisRemapInitialized = 0;
const char *PcInputBindingKindNames[14] = {"NOTSET",          "PADDIGITAL",      "PADANALOGUEPOS", "PADANALOGUENEG", "MOUSEDIGITAL",
                                           "MOUSEMOVEPOS",    "MOUSEMOVENEG",    "KEYDIGITAL",     "KEYDIGITALPOS",  "KEYDIGITALNEG",
                                           "KEYANALOGUEPAIR", "PADANALOGUEPAIR", "FORCEDMOUSEX",   "FORCEDMOUSEY"};
u8 PcInputWriteTextProfiles = 0;
