#include "saga.h"

/* Incomplete entry-connected reconstruction. Every skipped native call remains
 * marked at its actual site; this is not a complete dispatcher implementation. */
extern "C" i32 __cdecl NuInitHardwarePc(VARIPTR *buffer, VARIPTR *end, i32 allocate, ...) {
    i32 animation_count = 160, dat_count = 0, hostfs = 0, file_mode = 1, eject = 0;
    i32 stream_size = 0x200000, texture_count = 0x800, material_count = 0x400;
    i32 video_height = 224, video_parameter = 3, show = 1, audio_enabled = 0;
    i32 audio_gate = 0, audio_buffer_size = 0, audio_channels = 0, audio_capacity = 0;
    void *audio_names = NULL, *audio_parameter = NULL;
    void **pads[4] = {NULL, NULL, NULL, NULL};
    const char **dat_names = NULL;
    const char *parameter_2e = NuHwDefaultParameterPc;
    const char *config_name = NuHwDefaultConfigNamePc;
    void *parameter_2d = NULL, *parameter_30 = NULL, *parameter_31 = NULL;
    void *parameter_32 = NULL, *parameter_33 = NULL, *parameter_4c = NULL;
    HWND supplied_window = NULL;
    u32 icon = 0;
    i32 selector;
    va_list args;
    NuAPIInit();
    if (allocate) {
        buffer->void_ptr = NuAlloc(0x4000000, NuHwAllocSourcePc, 0xb2);
        end->addr = buffer->addr + 0x4000000;
    }
    STUBBED_NAMED(0x004ff100); /* native call 6dbc30 */
    STUBBED_NAMED(0x00602930); /* (1.0f, .5f, 0), call 6dbc49 */
    va_start(args, allocate);
    do {
        selector = va_arg(args, i32);
        switch (selector) {
            case 4:
                hostfs = va_arg(args, i32);
                break;
            case 6:
                file_mode = va_arg(args, i32);
                break;
            case 7:
                eject = va_arg(args, i32);
                break;
            case 8:
                stream_size = va_arg(args, i32);
                break;
            case 9:
            case 20:
            case 30:
            case 53:
                va_arg(args, i32);
                va_arg(args, i32);
                break;
            case 10:
                audio_gate = va_arg(args, i32);
                audio_buffer_size = va_arg(args, i32);
                audio_names = va_arg(args, void *);
                audio_parameter = va_arg(args, void *);
                audio_enabled = 1;
                break;
            case 11:
                audio_channels = va_arg(args, i32);
                audio_capacity = va_arg(args, i32);
                break;
            case 14:
                pads[0] = va_arg(args, void **);
                break;
            case 15:
                pads[1] = va_arg(args, void **);
                break;
            case 16:
                pads[2] = va_arg(args, void **);
                break;
            case 17:
                pads[3] = va_arg(args, void **);
                break;
            case 18: {
                i32 mode = va_arg(args, i32);
                if (video_height == 224 && (mode == 2 || mode == 8))
                    video_height = 256;
                break;
            }
            case 19:
                va_arg(args, i32);
                va_arg(args, i32);
                va_arg(args, i32);
                break;
            case 22:
                NuHwDfsNamePc = va_arg(args, const char *);
                break;
            case 23:
                NuHwSoundDfsNamePc = va_arg(args, const char *);
                break;
            case 24:
                dat_names = va_arg(args, const char **);
                dat_count = va_arg(args, i32);
                break;
            case 25:
                NuHwParameter19Pc = va_arg(args, u32);
                break;
            case 26:
                texture_count = va_arg(args, i32);
                break;
            case 28:
                audio_enabled = 0;
                va_arg(args, i32);
                va_arg(args, i32);
                va_arg(args, i32);
                va_arg(args, i32);
                break;
            case 33:
                va_arg(args, i32);
                video_height = va_arg(args, i32);
                break;
            case 35:
                va_arg(args, i32);
                va_arg(args, i32);
                break;
            case 36:
                video_parameter = va_arg(args, i32);
                break;
            case 37:
                material_count = va_arg(args, i32);
                break;
            case 45:
                parameter_2d = va_arg(args, void *);
                break;
            case 46:
                parameter_2e = va_arg(args, const char *);
                break;
            case 47:
                config_name = va_arg(args, const char *);
                break;
            case 48:
                parameter_30 = va_arg(args, void *);
                break;
            case 49:
                parameter_31 = va_arg(args, void *);
                break;
            case 50:
                parameter_32 = va_arg(args, void *);
                break;
            case 51:
                parameter_33 = va_arg(args, void *);
                break;
            case 52:
                show = va_arg(args, i32);
                break;
            case 60:
                supplied_window = va_arg(args, HWND);
                break;
            case 68:
                animation_count = va_arg(args, i32);
                break;
            case 76:
                parameter_4c = va_arg(args, void *);
                break;
            case 79:
                NuHwParameter4fPc = va_arg(args, u32);
                icon = va_arg(args, u32);
                break;
            case 82:
                NuPadSetMaxGamePads(va_arg(args, i32));
                break;
            case 83:
                icon = va_arg(args, u32);
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
            case 56:
            case 57:
            case 58:
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
            default:
                break;
        }
    } while (selector != 0);
    va_end(args);
    if (parameter_30 != NULL && parameter_31 != NULL) {
        STUBBED_NAMED(0x006ddd50); /* (parameter_30, parameter_31, parameter_2e, 0), 6dbf16 */
    }
    PCDeviceBackend.InitializeFactory();
    PcWindowOptions.InitializePath();
    PcWindowOptions.Load();
    PcNativeSelectLanguage(config_name);
    PcConfigureInputGraphicsCaps();
    if (parameter_32) {
        STUBBED_NAMED(0x006ddd90);
    }
    if (parameter_2d) {
        STUBBED_NAMED(0x006ddd40);
    }
    if (parameter_33) {
        STUBBED_NAMED(0x006dddb0);
    }
    PCDeviceBackend.requested_windowed = (u8)(show == 1);
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
    switch (PCDeviceBackend.identifier.vendor) {
        case 0x10de:
            switch (PCDeviceBackend.identifier.device_id) {
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
            break;
        case 0x1002:
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
            break;
        default:
            break;
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
        if (show == 0)
            NuHwShowCommandPc = 0;
        if (!CreateWindowPc(PCInputWindow_candidate, NuHwShowCommandPc, (u16)icon))
            exit(1);
    } else {
        if (!PublishWindowPc(supplied_window, NuHwShowCommandPc))
            exit(1);
    }
    NuPrimInit(buffer, *end);
    PCGraphicsInitRenderTexturePool(buffer, *end);
    NuKeyboardPlatformStartup_candidate();
    nuapi.video_aspect =
        PcWindowOptions.aspect_mode == 3 ? PCDeviceBackend.active_aspect_mode : PcWindowOptions.aspect_mode;
    SetCursor(NULL);
    if (!PCDeviceIsAbsent()) {
        STUBBED_NAMED(0x006e57d0); /* (0), 6dc167 */
    }
    STUBBED_NAMED(0x005fe3f0);
    STUBBED_NAMED(0x00609060);
    NuTrigInit();
    STUBBED_NAMED(0x00607bc0);
    NuDisplayListInit(buffer, (i32)end->addr);
    NuShaderManagerInit(buffer, *end);
    if ((NuHwDfsNamePc && *NuHwDfsNamePc) || (NuHwSoundDfsNamePc && *NuHwSoundDfsNamePc)) {
        i32 count = dat_count;
        if (NuHwDfsNamePc && *NuHwDfsNamePc)
            ++count;
        if (NuHwSoundDfsNamePc && *NuHwSoundDfsNamePc)
            ++count;
        const char **names = (const char **)NuAlloc(count * 4, NuHwAllocSourcePc, 0x2a0);
        if (NuHwDfsNamePc && *NuHwDfsNamePc)
            PCByteCopy_candidate(names, &NuHwDfsNamePc, 4);
        if (NuHwSoundDfsNamePc && *NuHwSoundDfsNamePc)
            PCByteCopy_candidate(names + 1, &NuHwSoundDfsNamePc, 4);
        dat_names = names;
        dat_count = count;
        /* Native does not copy original dat_names into the expanded allocation. */
    }
    StartupOpenDats(dat_names, dat_count, buffer);
    NuTexInitEx(buffer, texture_count);
    NuMtlInitEx(buffer, material_count);
    NuRndrInitEx(stream_size, buffer, *end);
    STUBBED_NAMED(0x00601a30);
    STUBBED_NAMED(0x0060cb00);
    STUBBED_NAMED(0x005d16a0); /* (buffer, *end) */
    NuInitDebrisRenderer(buffer, *end);
    PCGraphicsInitializeFramebuffers((u8)PCDeviceBackend.render_target_flags, 0, buffer, *end);
    STUBBED_NAMED(0x005f22a0); /* (PCDeviceBackend.shader_flags, buffer, *end) */
    NuRndrBufferDescriptorsInitPc(buffer, *end);
    PcResetGeometryOriginStatePc();
    STUBBED_NAMED(0x005eaa80); /* (buffer, *end) */
    STUBBED_NAMED(0x004ffbb0);
    NuTimeBarInit();
    NuAnimInit(animation_count, buffer, *end);
    NuTexAnimInitMutex();
    if (audio_enabled) {
        STUBBED_NAMED(0x006951f0); /* (buffer,*end,video_parameter,1,audio_gate,2,
                                     audio_buffer_size,3,audio_channels,4,audio_capacity,0) */
    }
    if (audio_gate && audio_names && audio_parameter) {
        STUBBED_NAMED(0x00698710);
    }
    NuSound3Update();
    NuPrimSetCoordinateSystem(0);
    NuQFntSetCoordinateSystem(1);
    NuPadInit();
    if (pads[0])
        *pads[0] = NuPadOpen(0, 0);
    if (pads[1])
        *pads[1] = NuPadOpen(1, 0);
    if (pads[2])
        *pads[2] = NuPadOpen(2, 0);
    if (pads[3])
        *pads[3] = NuPadOpen(3, 0);
    NuMtlInitOverride(512, buffer, end);
    STUBBED_NAMED(0x006daea0); /* (1) */
    STUBBED_NAMED(0x005f2800); /* (parameter_4c, buffer, *end) */
    STUBBED_NAMED(0x006ddec0);
    NuRndrBeginScene(1);
    NuRndrClear(0xb00, 0, 1.0f);
    NuRndrEndScene();
    NuRndrSwapScreen(0);
    bgProcInit();
    return 1;
}

extern "C" void __cdecl NuKeyboardCreateWindowState_candidate(HWND window, HINSTANCE instance) {
    PCFrameCriticalSectionIndex_candidate = PCThreadCriticalSectionCreate_candidate();
    PCDeviceBackend.InitializeAdapter();
    nu_keyboard_owner.Setup(window, instance);
    STUBBED_NAMED(0x00600bc0); /* native call6df0a8, actual separate RET leaf */
    PCNativeWarningDebugState.Initialize();
}

extern "C" {
    extern u32 PcUseUserGeoRegionPc;
}

extern "C" i32 __cdecl PcNativeSelectLanguage(const char *configuration_name) {
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
    if (NuHwParameter4fPc == 0 && language == 0x409)
        language = 0x809;
    NuRegionSet(3);
    switch (language) {
        case 0x405:
            NuLanguageSet(10);
            break;
        case 0x406:
            NuLanguageSet(8);
            break;
        case 0x407:
        case 0x807:
        case 0xc07:
        case 0x1007:
        case 0x1407:
            NuLanguageSet(4);
            break;
        case 0x408:
            NuLanguageSet(9);
            break;
        case 0x409:
            NuLanguageSet(18);
            NuRegionSet(2);
            break;
        case 0x40a:
        case 0xc0a:
        case 0x100a:
        case 0x140a:
        case 0x180a:
        case 0x200a:
        case 0x240a:
        case 0x280a:
        case 0x2c0a:
        case 0x300a:
        case 0x340a:
        case 0x380a:
        case 0x3c0a:
        case 0x400a:
        case 0x440a:
        case 0x480a:
        case 0x4c0a:
        case 0x500a:
            NuLanguageSet(3);
            break;
        case 0x40b:
            NuLanguageSet(17);
            break;
        case 0x40c:
        case 0x80c:
        case 0x100c:
        case 0x140c:
        case 0x180c:
            NuLanguageSet(2);
            break;
        case 0x40e:
            NuLanguageSet(20);
            break;
        case 0x410:
        case 0x810:
            NuLanguageSet(5);
            break;
        case 0x411:
            NuLanguageSet(0);
            NuRegionSet(1);
            break;
        case 0x412:
            NuLanguageSet(13);
            break;
        case 0x413:
        case 0x813:
            NuLanguageSet(6);
            break;
        case 0x414:
            NuLanguageSet(15);
            break;
        case 0x415:
            NuLanguageSet(11);
            break;
        case 0x416:
            NuLanguageSet(16);
            break;
        case 0x419:
            NuLanguageSet(12);
            break;
        case 0x41d:
            NuLanguageSet(14);
            break;
        case 0x809:
        case 0xc09:
        case 0x1009:
        case 0x1409:
        case 0x1809:
        case 0x1c09:
        case 0x2009:
        case 0x2409:
        case 0x2809:
        case 0x2c09:
        case 0x3009:
        case 0x3409:
            NuLanguageSet(1);
            break;
        case 0x80a:
            NuLanguageSet(3);
            NuRegionSet(2);
            break;
        case 0x816:
            NuLanguageSet(7);
            break;
        case 0xc0c:
            NuLanguageSet(2);
            NuRegionSet(2);
            break;
        default:
            switch (language & 0x3ff) {
                case 6:
                    return 8;
                case 7:
                    return 4;
                case 9:
                    return 1;
                case 10:
                    return 3;
                case 12:
                    return 2;
                case 16:
                    return 5;
                case 21:
                    return 11;
                case 25:
                    return 12;
                default:
                    return 1;
            }
    }
    if (PcUseUserGeoRegionPc != 0) {
        switch (GetUserGeoID((GEOCLASS)16)) {
            case 0x27:
            case 0xa6:
            case 0xf4:
                NuRegionSet(2);
                break;
            case 0x7a:
                NuRegionSet(1);
                break;
            default:
                NuRegionSet(3);
                break;
        }
    }
    return NuLanguageGet();
}

extern "C" void __cdecl PCDeviceResetSamplerDefaultsPc(void) {
    u32 stage = 0;
    if (PCDeviceBackend.caps.sampler_count_pc == 0)
        return;
    do {
        PCDeviceSetSamplerState(stage, 5, 2);
        PCDeviceSetSamplerState(stage, 6, 2);
        PCDeviceSetSamplerState(stage, 7, 2);
        PCDeviceSetSamplerState(stage, 9, 0);
        ++stage;
    } while (stage != PCDeviceBackend.caps.sampler_count_pc);
}
