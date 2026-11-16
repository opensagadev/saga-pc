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

extern "C" {
    void NuRndrDwaInit(void);
    void PcCreateCursorMaterial(void);
}

extern "C" void PcRegisterSaveMetadata(const void *);

extern "C" {
    extern char dat_007516c0[];
    const char *NuHwSoundDfsName = dat_007516c0;
    const char *NuHwDfsName = dat_007516c0;
    HINSTANCE PCInputWindow = 0;
    i32 NuHwShowCommand = 0;
}

/* Incomplete entry-connected reconstruction. Every skipped native call remains
 * marked at its actual site; this is not a complete dispatcher implementation. */
extern "C" i32 NuInitHardware(VARIPTR *buffer, VARIPTR *end, i32 allocate, ...) {
    i32 animation_count = 160;
    i32 dat_count = 0;
    i32 hostfs = 0;
    i32 file_mode = 1;
    i32 eject = 0;
    i32 stream_size = 0x200000;
    i32 texture_count = 0x800;
    i32 material_count = 0x400;
    i32 video_height = 224;
    i32 video_parameter = 3;
    i32 show = 1;
    i32 audio_enabled = 0;
    i32 audio_gate = 0;
    i32 audio_buffer_size = 0;
    i32 audio_channels = 0;
    i32 audio_capacity = 0;
    void *audio_names = NULL;
    void *audio_parameter = NULL;
    void **pads[4] = {NULL, NULL, NULL, NULL};
    const char **dat_names = NULL;
    const char *parameter_2e = NuHwDefaultParameter;
    const char *config_name = "TTalesKey";
    void *parameter_2d = NULL;
    void *parameter_30 = NULL;
    void *parameter_31 = NULL;
    void *parameter_32 = NULL;
    void *parameter_33 = NULL;
    u32 occlusion_capacity = 0;
    HWND supplied_window = NULL;
    u32 icon = 0;
    i32 selector;
    va_list args;
    NuAPIInit();
    if (allocate) {
        buffer->void_ptr = NuAlloc(0x4000000, ".\\pcapi.cpp", 0xb2);
        end->addr = buffer->addr + 0x4000000;
    }
    PcInitializeStartupCriticalSection();
    NuRndrConfigureLeaf(1.0f, 0.5f, 0);
    va_start(args, allocate);
    do {
        selector = va_arg(args, i32);
        switch (selector) {
            case NUHW_ANIMATION_COUNT:
                animation_count = va_arg(args, i32);
                break;
            case NUHW_FILE_MODE:
                file_mode = va_arg(args, i32);
                break;
            case NUHW_EJECT:
                eject = va_arg(args, i32);
                break;
            case NUHW_HOSTFS:
                hostfs = va_arg(args, i32);
                break;
            case 5:
            case 21:
            case 27:
            case 31:
            case 32:
            case 34:
            case 38:
            case 39:
            case 40:
            case 41:
            case 42:
            case 54:
            case 55:
            case 59:
            case 69:
            case 70:
            case 71:
            case 72:
            case 73:
            case 75:
            case 78:
                va_arg(args, i32);
                break;
            case NUHW_DFS_NAME:
                NuHwDfsName = va_arg(args, const char *);
                break;
            case NUHW_SOUND_DFS_NAME:
                NuHwSoundDfsName = va_arg(args, const char *);
                break;
            case NUHW_DAT_NAMES:
                dat_names = va_arg(args, const char **);
                dat_count = va_arg(args, i32);
                break;
            case 25:
                NuHwParameter19 = va_arg(args, u32);
                break;
            case NUHW_STREAM_SIZE:
                stream_size = va_arg(args, i32);
                break;
            case 9:
            case 20:
            case 30:
            case 53:
                va_arg(args, i32);
                va_arg(args, i32);
                break;
            case NUHW_AUDIO:
                audio_enabled = 1;
                audio_gate = va_arg(args, i32);
                audio_buffer_size = va_arg(args, i32);
                audio_names = va_arg(args, void *);
                audio_parameter = va_arg(args, void *);
                break;
            case NUHW_NO_AUDIO:
                audio_enabled = 0;
                va_arg(args, i32);
                va_arg(args, i32);
                va_arg(args, i32);
                va_arg(args, i32);
                break;
            case 35:
                va_arg(args, i32);
                va_arg(args, i32);
                break;
            case 36:
                video_parameter = va_arg(args, i32);
                break;
            case NUHW_AUDIO_CHANNELS:
                audio_channels = va_arg(args, i32);
                audio_capacity = va_arg(args, i32);
                break;
            case NUHW_PAD0:
                pads[0] = va_arg(args, void **);
                break;
            case NUHW_PAD1:
                pads[1] = va_arg(args, void **);
                break;
            case NUHW_PAD2:
                pads[2] = va_arg(args, void **);
                break;
            case NUHW_PAD3:
                pads[3] = va_arg(args, void **);
                break;
            case NUHW_VIDEO_MODE: {
                i32 mode = va_arg(args, i32);
                if (video_height == 224) {
                    switch (mode) {
                        case 2:
                        case 8:
                            video_height = 256;
                            break;
                    }
                }
                break;
            }
            case NUHW_VIDEO_HEIGHT:
                va_arg(args, i32);
                video_height = va_arg(args, i32);
                break;
            case 19:
                va_arg(args, i32);
                va_arg(args, i32);
                va_arg(args, i32);
                break;
            case NUHW_TEXTURE_COUNT:
                texture_count = va_arg(args, i32);
                break;
            case NUHW_MATERIAL_COUNT:
                material_count = va_arg(args, i32);
                break;
            case 46:
                parameter_2e = va_arg(args, const char *);
                break;
            case NUHW_CONFIG_NAME:
                config_name = va_arg(args, const char *);
                break;
            case 45:
                parameter_2d = va_arg(args, void *);
                break;
            case NUHW_SAVE_FOLDER_COMPANY:
                parameter_30 = va_arg(args, void *);
                break;
            case NUHW_SAVE_FOLDER_GAME:
                parameter_31 = va_arg(args, void *);
                break;
            case 50:
                parameter_32 = va_arg(args, void *);
                break;
            case 51:
                parameter_33 = va_arg(args, void *);
                break;
            case NUHW_SHOW_COMMAND:
                show = va_arg(args, i32);
                break;
            case NUHW_WINDOW:
                supplied_window = va_arg(args, HWND);
                break;
            case 56:
            case 57:
            case 58:
                va_arg(args, i32);
                continue;
            case NUHW_MAX_GAME_PADS:
                NuPadSetMaxGamePads(va_arg(args, i32));
                break;
            case NUHW_OCCLUSION_CAPACITY:
                occlusion_capacity = va_arg(args, u32);
                break;
            case 79:
                NuHwParameter4f = va_arg(args, u32);
                /* fall through */
            case NUHW_ICON:
                icon = va_arg(args, u32);
                break;
            default:
                break;
        }
    } while (selector != NUHW_END);
    va_end(args);
    if (parameter_30 != NULL && parameter_31 != NULL) {
        PcRegisterSaveFolder((const char *)parameter_30, (const char *)parameter_31, parameter_2e, 0);
    }
    PCDeviceBackend.InitializeFactory();
    PcWindowOptions.InitializePath();
    PcWindowOptions.Load();
    PcNativeSelectLanguage(config_name);
    PcConfigureInputGraphicsCaps();
    if (parameter_32) {
        PcRegisterSaveMetadata(parameter_32);
    }
    if (parameter_2d) {
        PcSetDefaultParameter((const char *)parameter_2d);
    }
    if (parameter_33) {
        NuHwSetParameter33(parameter_33);
    }
    PCDeviceBackend.requested_windowed = show == 1;
    NuFileInitEx(hostfs, file_mode, eject);
    NuFileExtInit(!NuFileUseHiresPC());
    PcWindowOptions.SetBloomMethod(2);
    if (PCDeviceHardwareVertexProcessingPC < 4) {
        PcWindowOptions.SetUseHires(0);
        PcWindowOptions.SetOption459(0);
        PcWindowOptions.Save();
    }
    /* The two device-specific tables choose this same low-capability path.
     * Their exact finite ID sets are retained in vendor-protocol-native.json. */
    if (PCDeviceBackend.identifier.vendor == 0x10de) {
        switch ((i32)PCDeviceBackend.identifier.device_id) {
            case 0xf3:
            case 0x14f:
            case 0x161:
            case 0x162:
            case 0x163:
            case 0x1df:
            case 0x221:
            case 0x7e1:
                PcWindowOptions.SetUseHires(0);
                PcWindowOptions.SetOption459(0);
                PcWindowOptions.Save();
                break;
            default:
                break;
        }
    } else if (PCDeviceBackend.identifier.vendor == 0x1002) {
        switch (PCDeviceBackend.identifier.device_id) {
            case 0x7142:
            case 0x7146:
            case 0x714d:
            case 0x714e:
            case 0x715e:
            case 0x7162:
            case 0x7166:
            case 0x716d:
            case 0x716e:
            case 0x717e:
            case 0x7180:
            case 0x7183:
            case 0x7187:
            case 0x718f:
            case 0x71a0:
            case 0x71a3:
            case 0x71a7:
            case 0x71af:
            case 0x71c3:
            case 0x71e3:
                PcWindowOptions.SetUseHires(0);
                PcWindowOptions.SetOption459(0);
                PcWindowOptions.Save();
                break;
            default:
                break;
        }
    }
    if (PcWindowOptions.usehires) {
        PCDeviceBackend.requested_multisample = 0;
        PcWindowOptions.SetDesiredShadowMethod(2);
        PcWindowOptions.SetDesiredBloomMethod(2);
    } else {
        PcWindowOptions.SetDesiredShadowMethod(1);
        PcWindowOptions.SetDesiredBloomMethod(1);
    }
    PcWindowOptions.SetSpeedBlurEnabled(1);
    NuFileInitEx(hostfs, file_mode, eject);
    if (supplied_window == NULL) {
        if (show == 0) {
            NuHwShowCommand = 0;
        }
        if (!CreateGameWindow(PCInputWindow, NuHwShowCommand, (u16)icon)) {
            exit(1);
        }
    } else {
        if (!PublishWindow(supplied_window, NuHwShowCommand)) {
            exit(1);
        }
    }
    NuPrimInit(buffer, *end);
    PCGraphicsInitRenderTexturePool(buffer, *end);
    NuKeyboardPlatformStartup();
    nuapi.video_aspect = PcWindowOptions.aspect_mode == PC_ASPECT_AUTO ? PCDeviceBackend.active_aspect_mode : PcWindowOptions.aspect_mode;
    SetCursor(NULL);
    if (!PCDeviceIsAbsent()) {
        PCDeviceShowCursor(0);
    }
    NuRndrDwaInit();
    NuLgtInitializeState();
    NuTrigInit();
    NuVpInit();
    NuDisplayListInit(buffer, (i32)end->addr);
    NuShaderManagerInit(buffer, *end);
    if ((NuHwDfsName && *NuHwDfsName) || (NuHwSoundDfsName && *NuHwSoundDfsName)) {
        i32 count = dat_count;
        if (NuHwDfsName && *NuHwDfsName) {
            ++count;
        }
        if (NuHwSoundDfsName && *NuHwSoundDfsName) {
            ++count;
        }
        const char **names = (const char **)NuAlloc(count * 4, ".\\pcapi.cpp", 0x2a0);
        if (NuHwDfsName && *NuHwDfsName) {
            PCByteCopy(names, &NuHwDfsName, 4);
        }
        if (NuHwSoundDfsName && *NuHwSoundDfsName) {
            PCByteCopy(names + 1, &NuHwSoundDfsName, 4);
        }
        dat_names = names;
        dat_count = count;
        /* Native does not copy original dat_names into the expanded allocation. */
    }
    StartupOpenDats(dat_names, dat_count, buffer);
    NuTexInitEx(buffer, texture_count);
    NuMtlInitEx(buffer, material_count);
    NuRndrInitEx(stream_size, buffer, *end);
    NuRndrInitStartupMaterials();
    NuGobjRegistryInit();
    NuRndrInitLeaf(buffer, *end);
    NuInitDebrisRenderer(buffer, *end);
    PCGraphicsInitializeFramebuffers(PCDeviceBackend.render_target_flags, 0, buffer, *end);
    NuPostEffectInit(PCDeviceBackend.shader_flags, buffer, *end);
    NuRndrBufferDescriptorsInit(buffer, *end);
    PcResetGeometryOriginState();
    NuQFntInit(buffer, *end);
    PcInitializePerformanceTiming();
    NuTimeBarInit();
    NuAnimInit(animation_count, buffer, *end);
    NuTexAnimInitMutex();
    if (audio_enabled) {
        NuSound3InitV(buffer, *end, video_parameter, 1, (SFX_FILE_INFO *)audio_gate, 2, audio_buffer_size, 3, audio_channels, 4,
                      audio_capacity, 0);
    }
    if (audio_gate && audio_names && audio_parameter) {
        NuSound3SetRequestTable((u16 *)audio_names, (i32)audio_parameter);
    }
    NuSound3Update();
    NuPrimSetCoordinateSystem(0);
    NuQFntSetCoordinateSystem(NUQFNT_CS_SIXTEENTHS);
    NuPadInit();
    if (pads[0]) {
        *pads[0] = NuPadOpen(0, 0);
    }
    if (pads[1]) {
        *pads[1] = NuPadOpen(1, 0);
    }
    if (pads[2]) {
        *pads[2] = NuPadOpen(2, 0);
    }
    if (pads[3]) {
        *pads[3] = NuPadOpen(3, 0);
    }
    NuMtlInitOverride(512, buffer, end);
    NuPadAutoConfigure(1);
    NuOcclusionManagerInit(occlusion_capacity, buffer, *end);
    PcCreateCursorMaterial();
    NuRndrBeginScene(1);
    NuRndrClear(0xb00, 0, 1.0f);
    NuRndrEndScene();
    NuRndrSwapScreen(0);
    bgProcInit();
    return 1;
}

extern "C" i32 fun_006dc6b0(i32 index) {
    return nu_keyboard_owner.unknown_0010_to_0020[index];
}

extern "C" void fun_006dc6f0(void) {
    NuPrimSetCoordinateSystem(NuPrimCoordSystemStack[--NuPrimCSPos]);
}

i32 PC_WINDOW_OPTIONS::fun_006dc710() {
    return client_width;
}

i32 PC_WINDOW_OPTIONS::fun_006dc720() {
    return client_height;
}

i32 PC_WINDOW_OPTIONS::fun_006dc730() {
    return window_x;
}

i32 PC_WINDOW_OPTIONS::fun_006dc740() {
    return window_y;
}

i32 PC_DEVICE_BACKEND::fun_006dc750() {
    return parameters.windowed == 1;
}

extern "C" {
    u32 NuHwParameter19 = 0;
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
