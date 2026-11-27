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
}
extern "C" {
    PC_NATIVE_DEBUG_STATE PCNativeWarningDebugState;
}

extern "C" void fun_006d3b20(void) {
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

extern "C" void fun_006d4d60(void) {
}

// Append to input_runtime.cpp; declarations/mappings in B-contract.json.

// Append input_runtime.cpp. Native strings/table definitions and mappings in B-save-data.json.
