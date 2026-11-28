#include "gameapi/edtools/edui.h"
#include "legoapi/items/collect/bolts.h"
#include "legoapi/render/core/gameliball.h"
#include "nu2api/PCInput.h"
#include "nu2api/bgproc_pc.h"
#include "nu2api/huffdecode.h"
#include "nu2api/nu3d/nudlist.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nu3d/nuocclusion.h"
#include "nu2api/nu3d/nupostfilter.h"
#include "nu2api/nu3d/nuprim.h"
#include "nu2api/nu3d/nuptl.h"
#include "nu2api/nu3d/nuqfnt.h"
#include "nu2api/nu3d/nurndr.h"
#include "nu2api/nu3d/nushader_plain.h"
#include "nu2api/nu3d/nutex.h"
#include "nu2api/nu3d/nutexanm.h"
#include "nu2api/nu3d/nutimebar_plain.h"
#include "nu2api/nu3d/nuvport.h"
#include "nu2api/nucamera_dlist.h"
#include "nu2api/nucore/bgproc.h"
#include "nu2api/nucore/nuanim3.h"
#include "nu2api/nucore/nuapi.h"
#include "nu2api/nucore/nucore_includes.h"
#include "nu2api/nucore/nupad.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nudir_gen.h"
#include "nu2api/nufile/nufile.h"
#include "nu2api/nugobj.h"
#include "nu2api/nuhgo_dlist.h"
#include "nu2api/numainfilter_gen.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/nuoctree_gen.h"
#include "nu2api/nuonline_gen.h"
#include "nu2api/nusound/nusound.h"
#include "nu2api/nustencilshadows_PC.h"
#include "nu2api/nuwin32/d3dCalls.h"
#include "nu2api/nuwin32/d3dCore.h"
#include "nu2api/nuwin32/nu2api_windows.h"
#include "nu2api/nuwin32/pcapi.h"
#include "nu2api/save_folder.h"
#include "gameframework/saveload.h"

extern "C" void NuPrimPopCoordinateSystem(void) {
    NuPrimSetCoordinateSystem(NuPrimCoordSystemStack[--NuPrimCSPos]);
}

i32 PC_WINDOW_OPTIONS::GetClientWidth() {
    return client_width;
}

i32 PC_WINDOW_OPTIONS::GetClientHeight() {
    return client_height;
}

i32 PC_WINDOW_OPTIONS::GetWindowX() {
    return window_x;
}

i32 PC_WINDOW_OPTIONS::GetWindowY() {
    return window_y;
}

i32 PC_DEVICE_BACKEND::IsWindowed() {
    return parameters.windowed == 1;
}

extern "C" {
    u32 NuHwParameter4f = 0;
    u32 PcUseUserGeoRegion = 0;
}

extern "C" i32 PcNativeSelectLanguage(const char *configuration_name) {
    char path[256];
    DWORD language;
    DWORD bytes = 4;
    HKEY key;

    NuStrCpy(path, "SOFTWARE\\");
    NuStrCat(path, configuration_name);
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, path, 0, KEY_QUERY_VALUE, &key) == ERROR_SUCCESS) {
        RegQueryValueExA(key, "LanguageVal", NULL, NULL, (LPBYTE)&language, &bytes);
        RegCloseKey(key);
    } else {
        language = GetUserDefaultLangID();
    }
    if (NuHwParameter4f == 0 && language == MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US)) {
        language = MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_UK);
    }
    NuRegionSet(3);
    switch (language) {
        case MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US):
            NuLanguageSet(NULANGUAGE_US_ENGLISH);
            NuRegionSet(2);
            break;
        case MAKELANGID(LANG_FINNISH, SUBLANG_DEFAULT):
            NuLanguageSet(NULANGUAGE_FINNISH);
            break;
        case MAKELANGID(LANG_DANISH, SUBLANG_DEFAULT):
            NuLanguageSet(NULANGUAGE_DANISH);
            break;
        case MAKELANGID(LANG_HUNGARIAN, SUBLANG_DEFAULT):
            NuLanguageSet(NULANGUAGE_HUNGARIAN);
            break;
        case MAKELANGID(LANG_POLISH, SUBLANG_DEFAULT):
            NuLanguageSet(NULANGUAGE_POLISH);
            break;
        case MAKELANGID(LANG_RUSSIAN, SUBLANG_DEFAULT):
            NuLanguageSet(NULANGUAGE_RUSSIAN);
            break;
        case MAKELANGID(LANG_GREEK, SUBLANG_DEFAULT):
            NuLanguageSet(NULANGUAGE_GREEK);
            break;
        case MAKELANGID(LANG_CZECH, SUBLANG_DEFAULT):
            NuLanguageSet(NULANGUAGE_CZECH);
            break;
        case MAKELANGID(LANG_JAPANESE, SUBLANG_DEFAULT):
            NuLanguageSet(NULANGUAGE_JAPANESE);
            NuRegionSet(1);
            break;
        case MAKELANGID(LANG_KOREAN, SUBLANG_KOREAN):
            NuLanguageSet(NULANGUAGE_KOREAN);
            break;
        case MAKELANGID(LANG_SWEDISH, SUBLANG_SWEDISH):
            NuLanguageSet(NULANGUAGE_SWEDISH);
            break;
        case MAKELANGID(LANG_NORWEGIAN, SUBLANG_NORWEGIAN_BOKMAL):
            NuLanguageSet(NULANGUAGE_NORWEGIAN);
            break;
        case MAKELANGID(LANG_PORTUGUESE, SUBLANG_PORTUGUESE_BRAZILIAN):
            NuLanguageSet(NULANGUAGE_BRAZILIAN_PORTUGUESE);
            break;
        case MAKELANGID(LANG_ITALIAN, SUBLANG_ITALIAN):
        case MAKELANGID(LANG_ITALIAN, SUBLANG_ITALIAN_SWISS):
            NuLanguageSet(NULANGUAGE_ITALIAN);
            break;
        case MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_MEXICAN):
            NuLanguageSet(NULANGUAGE_SPANISH);
            NuRegionSet(2);
            break;
        case MAKELANGID(LANG_DUTCH, SUBLANG_DUTCH):
        case MAKELANGID(LANG_DUTCH, SUBLANG_DUTCH_BELGIAN):
            NuLanguageSet(NULANGUAGE_DUTCH);
            break;
        case MAKELANGID(LANG_PORTUGUESE, SUBLANG_PORTUGUESE):
            NuLanguageSet(NULANGUAGE_PORTUGUESE);
            break;
        case MAKELANGID(LANG_FRENCH, SUBLANG_FRENCH_CANADIAN):
            NuLanguageSet(NULANGUAGE_FRENCH);
            NuRegionSet(2);
            break;
        case MAKELANGID(LANG_FRENCH, SUBLANG_FRENCH):
        case MAKELANGID(LANG_FRENCH, SUBLANG_FRENCH_BELGIAN):
        case MAKELANGID(LANG_FRENCH, SUBLANG_FRENCH_SWISS):
        case MAKELANGID(LANG_FRENCH, SUBLANG_FRENCH_LUXEMBOURG):
        case MAKELANGID(LANG_FRENCH, SUBLANG_FRENCH_MONACO):
            NuLanguageSet(NULANGUAGE_FRENCH);
            break;
        case MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN):
        case MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN_SWISS):
        case MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN_AUSTRIAN):
        case MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN_LUXEMBOURG):
        case MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN_LIECHTENSTEIN):
            NuLanguageSet(NULANGUAGE_GERMAN);
            break;
        case MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_UK):
        case MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_AUS):
        case MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_CAN):
        case MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_NZ):
        case MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_EIRE):
        case MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_SOUTH_AFRICA):
        case MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_JAMAICA):
        case MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_CARIBBEAN):
        case MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_BELIZE):
        case MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_TRINIDAD):
        case MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_ZIMBABWE):
        case MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_PHILIPPINES):
            NuLanguageSet(NULANGUAGE_ENGLISH);
            break;
        case MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH):
        case MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_MODERN):
        case MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_GUATEMALA):
        case MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_COSTA_RICA):
        case MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_PANAMA):
        case MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_DOMINICAN_REPUBLIC):
        case MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_VENEZUELA):
        case MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_COLOMBIA):
        case MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_PERU):
        case MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_ARGENTINA):
        case MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_ECUADOR):
        case MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_CHILE):
        case MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_URUGUAY):
        case MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_PARAGUAY):
        case MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_BOLIVIA):
        case MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_EL_SALVADOR):
        case MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_HONDURAS):
        case MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_NICARAGUA):
        case MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_PUERTO_RICO):
            NuLanguageSet(NULANGUAGE_SPANISH);
            break;
        default:
            switch (language & 0x3ff) {
                case 9:
                    return 1;
                case 12:
                    return 2;
                case 16:
                    return 5;
                case 7:
                    return 4;
                case 10:
                    return 3;
                case 6:
                    return 8;
                case 25:
                    return 12;
                case 21:
                    return 11;
                default:
                    return 1;
            }
    }
    if (PcUseUserGeoRegion != 0) {
        switch (GetUserGeoID((GEOCLASS)16)) {
            case 0x27:
            case 0xa6:
            case 0xf4:
                NuRegionSet(2);
                break;
            case 0x7a:
                NuRegionSet(1);
                break;
            case 0xf2:
                NuRegionSet(3);
                break;
            default:
                NuRegionSet(3);
                break;
        }
    }
    return NuLanguageGet();
}
