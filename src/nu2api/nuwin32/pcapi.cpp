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
#include "nu2api/nufile/nufpar.h"
#include "nu2api/pc_options.h"
#include "nu2api/platform_files.h"
#include <stdio.h>
#include <shlobj.h>
#include <string.h>

extern "C" {
    extern u32 NuHwParameter4f;
}
extern "C" i32 PcNativeSelectLanguage(const char *configuration_name);

u32 PC_DEVICE_BACKEND::GetShaderFlags() {
    return shader_flags;
}

u32 PC_DEVICE_BACKEND::GetRenderTargetFlags() {
    return render_target_flags;
}

void PC_DEVICE_BACKEND::SetRequestedWindowed(u8 windowed) {
    requested_windowed = windowed;
}

extern "C" i32 PcOptionsGetHardwareVertexProcessing(void) {
    return PCDeviceHardwareVertexProcessingPC;
}

extern "C" i32 fun_006dbb20(void) {
    return 0;
}

extern "C" {
    const char *NuHwSoundDfsName = "";
    const char *NuHwDfsName = "";
    HINSTANCE PCInputWindow = 0;
    i32 NuHwShowCommand = 0;
    u32 NuHwParameter19 = 0;
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
            case NUAPI_SETUP_ANIMATIONCOUNT:
                animation_count = va_arg(args, i32);
                break;
            case NUAPI_SETUP_IOPRESET:
                file_mode = va_arg(args, i32);
                break;
            case NUAPI_SETUP_EJECTDISC:
                eject = va_arg(args, i32);
                break;
            case NUAPI_SETUP_HOSTFS:
                hostfs = va_arg(args, i32);
                break;
            case NUAPI_SETUP_CDDVDMODE:
            case NUAPI_SETUP_GLASSRPLANE:
            case NUAPI_SETUP_TEXFILTERMODE:
            case NUAPI_SETUP_ENABLE_PROFILE:
            case NUAPI_SETUP_FLICKERFIX:
            case NUAPI_SETUP_SWAPMODE:
            case NUAPI_SETUP_ZMODE:
            case NUAPI_SETUP_IOPSTEREOSIZE:
            case NUAPI_SETUP_IOPMONOSIZE:
            case NUAPI_SETUP_IOPDATASIZE:
            case NUAPI_SETUP_FRAMEBUFFERDEPTH:
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
            case NUAPI_SETUP_DFSNAME:
                NuHwDfsName = va_arg(args, const char *);
                break;
            case NUAPI_SETUP_SFXDFSNAME:
                NuHwSoundDfsName = va_arg(args, const char *);
                break;
            case NUAPI_SETUP_DATNAMES:
                dat_names = va_arg(args, const char **);
                dat_count = va_arg(args, i32);
                break;
            case NUAPI_SETUP_PARAMETER19:
                NuHwParameter19 = va_arg(args, u32);
                break;
            case NUAPI_SETUP_STREAMSIZE:
                stream_size = va_arg(args, i32);
                break;
            case NUAPI_SETUP_TB:
            case NUAPI_SETUP_SCREENPOS:
            case NUAPI_SETUP_FILEPARSER:
            case 53:
                va_arg(args, i32);
                va_arg(args, i32);
                break;
            case NUAPI_SETUP_SOUND:
                audio_enabled = 1;
                audio_gate = va_arg(args, i32);
                audio_buffer_size = va_arg(args, i32);
                audio_names = va_arg(args, void *);
                audio_parameter = va_arg(args, void *);
                break;
            case NUAPI_SETUP_NOSOUND:
                audio_enabled = 0;
                va_arg(args, i32);
                va_arg(args, i32);
                va_arg(args, i32);
                va_arg(args, i32);
                break;
            case NUAPI_SETUP_SOUNDSTREAM:
                va_arg(args, i32);
                va_arg(args, i32);
                break;
            case NUAPI_SETUP_STREAMCHANCOUNT:
                video_parameter = va_arg(args, i32);
                break;
            case NUAPI_SETUP_REVERB:
                audio_channels = va_arg(args, i32);
                audio_capacity = va_arg(args, i32);
                break;
            case NUAPI_SETUP_PAD0:
                pads[0] = va_arg(args, void **);
                break;
            case NUAPI_SETUP_PAD1:
                pads[1] = va_arg(args, void **);
                break;
            case NUAPI_SETUP_PAD2:
                pads[2] = va_arg(args, void **);
                break;
            case NUAPI_SETUP_PAD3:
                pads[3] = va_arg(args, void **);
                break;
            case NUAPI_SETUP_VIDEOMODE: {
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
            case NUAPI_SETUP_VIDEORES:
                va_arg(args, i32);
                video_height = va_arg(args, i32);
                break;
            case NUAPI_SETUP_MULTIPURPOSEBUFFER:
                va_arg(args, i32);
                va_arg(args, i32);
                va_arg(args, i32);
                break;
            case NUAPI_SETUP_MAXTEXID:
                texture_count = va_arg(args, i32);
                break;
            case NUAPI_SETUP_MAXMTL:
                material_count = va_arg(args, i32);
                break;
            case NUAPI_SETUP_PC_WINDOWTITLE:
                parameter_2e = va_arg(args, const char *);
                break;
            case NUAPI_SETUP_PC_REGISTRYKEY:
                config_name = va_arg(args, const char *);
                break;
            case NUAPI_SETUP_PC_TITLE:
                parameter_2d = va_arg(args, void *);
                break;
            case NUAPI_SETUP_PC_SAVEFOLDERPUBLISHER:
                parameter_30 = va_arg(args, void *);
                break;
            case NUAPI_SETUP_PC_SAVEFOLDERGAMETITLE:
                parameter_31 = va_arg(args, void *);
                break;
            case NUAPI_SETUP_PC_SAVEMETADATA:
                parameter_32 = va_arg(args, void *);
                break;
            case NUAPI_SETUP_PARAMETER33:
                parameter_33 = va_arg(args, void *);
                break;
            case NUAPI_SETUP_PC_WINDOWED:
                show = va_arg(args, i32);
                break;
            case NUAPI_SETUP_PC_WINDOW:
                supplied_window = va_arg(args, HWND);
                break;
            case 56:
            case 57:
            case 58:
                va_arg(args, i32);
                continue;
            case NUAPI_SETUP_MAXGAMEPADS:
                NuPadSetMaxGamePads(va_arg(args, i32));
                break;
            case NUAPI_SETUP_OCCLUSIONCAPACITY:
                occlusion_capacity = va_arg(args, u32);
                break;
            case NUAPI_SETUP_PARAMETER4F:
                NuHwParameter4f = va_arg(args, u32);
                /* fall through */
            case NUAPI_SETUP_ICON:
                icon = va_arg(args, u32);
                break;
            default:
                break;
        }
    } while (selector != NUAPI_SETUP_END);
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

extern "C" i32 NuKeyboardDevicePresent(i32 index) {
    return nu_keyboard_owner.device_present[index];
}
