#pragma once

#include "nu2api/nucore/common.h"

/* PC_WINDOW_OPTIONS.aspect_mode (the "Widescreen" option), PC_DEVICE_BACKEND.active_aspect_mode: the aspect
 * ratio PcInputAspectRatio and PCDeviceBackend_GetActiveAspect return for each. */
#define PC_ASPECT_4_3 0
#define PC_ASPECT_16_9 1
#define PC_ASPECT_16_10 2
#define PC_ASPECT_AUTO 3 /* the display's (PCDeviceBackend.active_aspect) */

typedef struct PC_WINDOW_OPTIONS { // size 0x460
    i32 context;                   // 0x000
    char file_name[1024];          // 0x004
    i32 device_width;              // 0x404
    i32 device_height;             // 0x408
    i32 client_width;              // 0x40c
    i32 client_height;             // 0x410
    i32 window_x;                  // 0x414
    i32 window_y;                  // 0x418
    i32 refresh_rate;              // 0x41c
    i32 multisample;               // 0x420
    i32 aspect_mode;               // 0x424
    i32 sfxvolume;                 // 0x428
    i32 musicvolume;               // 0x42c
    i32 mastervolume;              // 0x430
    u8 musicenabled;               // 0x434
    u8 unknown_435[3];             // 0x435
    i32 mousesensitivity;          // 0x438
    u8 inverty;                    // 0x43c
    u8 vibration;                  // 0x43d
    u8 unknown_43e[2];             // 0x43e
    i32 bloommethod;               // 0x440
    i32 desiredbloommethod;        // 0x444
    u8 bloomenabled;               // 0x448
    u8 unknown_449[3];             // 0x449
    i32 shadowmethod;              // 0x44c
    i32 desiredshadowmethod;       // 0x450
    u8 shadowsenabled;             // 0x454
    u8 speedblursupported;         // 0x455
    u8 speedblurenabled;           // 0x456
    u8 usehires;                   // 0x457
    u8 usehirespending;            // 0x458
    u8 option459;                  // 0x459
    u8 unknown_45a[2];             // 0x45a
    i32 forceshadermodel;          // 0x45c
#ifdef __cplusplus
    PC_WINDOW_OPTIONS();
    void SetWidth(i32 value);
    void SetHeight(i32 value);
    void SetDeviceWidth(i32 value);
    void SetDeviceHeight(i32 value);
    void SetRefresh(i32 value);
    void SetMultisample(i32 value);
    void SetSfxVolume(i32 value);
    void SetMusicVolume(i32 value);
    void SetMasterVolume(i32 value);
    void SetMusicEnabled(u8 value);
    void SetBloomMethod(i32 value);
    void SetShadowMethod(i32 value);
    void SetSpeedBlurSupported(u8 value);
    void Save();
    void Defaults();
    void Validate();
    void InitializePath();
    void Load();
    void SetDesiredBloomMethod(i32 value);
    void SetDesiredShadowMethod(i32 value);
    void SetSpeedBlurEnabled(u8 value);
    void SetUseHires(u8 value);
    void SetOption459(u8 value);
    void SetWindowX(i32 value);
    void SetWindowY(i32 value);
    i32 GetDeviceWidth();
    i32 GetDeviceHeight();
    i32 GetRefresh();
    i32 GetMultisample();
    i32 GetBloomMethod();
    i32 GetDesiredBloomMethod();
    i32 GetDesiredShadowMethod();
    u8 GetSpeedBlurSupported();
    i32 GetClientWidth();
    i32 GetClientHeight();
    i32 GetWindowX();
    i32 GetWindowY();
    void SetMouseSensitivity(i32 value);
    void SetInvertY(u8 value);
    void SetVibration(u8 value);
    void SetForceShaderModel(i32 value);

    void SetAspectMode(i32);
    void SetShadowsEnabled(u8);
    void SetBloomEnabled(u8);
    void SetUseHiresPending(u8);
    u8 fun_00509030();
    u8 fun_00509040();
    u8 fun_00509050();
    u8 fun_00509060();
    u8 fun_00509070();
    u8 fun_00509080();
#endif
} PC_WINDOW_OPTIONS;

#ifdef __cplusplus
extern "C" {
#endif
    void StartupOpenDats(const char **names, i32 count, VARIPTR *arena);
    void PcRegisterSaveFolder(const char *company, const char *product, const char *parameter, i32 personal);
    i32 PublishWindow(HWND, i32);
#ifdef __cplusplus
    i32 PCDeviceIsAbsent(void);
#ifdef __cplusplus
}
#endif
#endif
/* NuInitHardware's variable arguments: a list of tags, each followed by the values the tag takes, ended by
 * NUAPI_SETUP_END. The parser reads the tags it knows and skips the values of the others. The names follow the
 * engine's own enum (build/reference); this build inserts two tags at 24 and 25 and two at 44 and 45, and adds
 * the tags from 61 on. Names marked "guessed" have no reference name and come from what the parser does. */
enum NUAPI_SETUP {
    NUAPI_SETUP_END = 0,
    NUAPI_SETUP_NGC_LICENSE_TEXTURE = 1,
    NUAPI_SETUP_NGC_SFX_PAKFILE = 2,
    NUAPI_SETUP_NGC_FIFO_SIZE = 3,
    NUAPI_SETUP_HOSTFS = 4,
    NUAPI_SETUP_CDDVDMODE = 5,
    NUAPI_SETUP_IOPRESET = 6, /* the file mode passed to NuFileInitEx */
    NUAPI_SETUP_EJECTDISC = 7,
    NUAPI_SETUP_STREAMSIZE = 8,
    NUAPI_SETUP_TB = 9,
    NUAPI_SETUP_SOUND = 10,  /* gate, buffer size, names, parameter */
    NUAPI_SETUP_REVERB = 11, /* two values, kept as the audio channels and capacity */
    NUAPI_SETUP_ADDITIONALDATASTREAMS = 12,
    NUAPI_SETUP_MULTITAP = 13,
    NUAPI_SETUP_PAD0 = 14,
    NUAPI_SETUP_PAD1 = 15,
    NUAPI_SETUP_PAD2 = 16,
    NUAPI_SETUP_PAD3 = 17,
    NUAPI_SETUP_VIDEOMODE = 18, /* PAL modes 2 and 8 make a 224-line height 256 */
    NUAPI_SETUP_MULTIPURPOSEBUFFER = 19,
    NUAPI_SETUP_SCREENPOS = 20,
    NUAPI_SETUP_GLASSRPLANE = 21,
    NUAPI_SETUP_DFSNAME = 22,
    NUAPI_SETUP_SFXDFSNAME = 23,
    NUAPI_SETUP_DATNAMES = 24,    /* guessed: names, count */
    NUAPI_SETUP_PARAMETER19 = 25, /* guessed: stored in NuHwParameter19 */
    NUAPI_SETUP_MAXTEXID = 26,
    NUAPI_SETUP_TEXFILTERMODE = 27,
    NUAPI_SETUP_NOSOUND = 28, /* four ignored values */
    NUAPI_SETUP_BSDEMO = 29,
    NUAPI_SETUP_FILEPARSER = 30,
    NUAPI_SETUP_ENABLE_PROFILE = 31,
    NUAPI_SETUP_FLICKERFIX = 32,
    NUAPI_SETUP_VIDEORES = 33, /* the width (ignored on PC), then the height */
    NUAPI_SETUP_SWAPMODE = 34,
    NUAPI_SETUP_SOUNDSTREAM = 35,
    NUAPI_SETUP_STREAMCHANCOUNT = 36,
    NUAPI_SETUP_MAXMTL = 37,
    NUAPI_SETUP_ZMODE = 38,
    NUAPI_SETUP_IOPSTEREOSIZE = 39,
    NUAPI_SETUP_IOPMONOSIZE = 40,
    NUAPI_SETUP_IOPDATASIZE = 41,
    NUAPI_SETUP_FRAMEBUFFERDEPTH = 42,
    NUAPI_SETUP_DEBUGTEXCAHECOLOUR = 43,
    NUAPI_SETUP_PC_TITLE = 45, /* guessed: the default title (PcSetDefaultParameter) */
    NUAPI_SETUP_PC_WINDOWTITLE = 46,
    NUAPI_SETUP_PC_REGISTRYKEY = 47,
    NUAPI_SETUP_PC_SAVEFOLDERPUBLISHER = 48,
    NUAPI_SETUP_PC_SAVEFOLDERGAMETITLE = 49,
    NUAPI_SETUP_PC_SAVEMETADATA = 50, /* guessed: PcRegisterSaveMetadata */
    NUAPI_SETUP_PARAMETER33 = 51,     /* guessed: NuHwSetParameter33 */
    NUAPI_SETUP_PC_WINDOWED = 52,     /* windowed when the value is 1 */
    NUAPI_SETUP_PC_WINDOW = 60,       /* guessed: a window the game draws into */
    NUAPI_SETUP_ANIMATIONCOUNT = 68,  /* guessed */
    NUAPI_SETUP_0x46 = 70,
    NUAPI_SETUP_0x47 = 71,
    NUAPI_SETUP_0x49 = 73,
    NUAPI_SETUP_0x4b = 75,
    NUAPI_SETUP_OCCLUSIONCAPACITY = 76, /* guessed */
    NUAPI_SETUP_PARAMETER4F = 79,       /* guessed: stored in NuHwParameter4f */
    NUAPI_SETUP_MAXGAMEPADS = 82,       /* guessed */
    NUAPI_SETUP_ICON = 83               /* guessed */
};

#define NUHW_END NUAPI_SETUP_END
#define NUHW_HOSTFS(func) NUAPI_SETUP_HOSTFS, func
#define NUHW_CDDVDMODE(func) NUAPI_SETUP_CDDVDMODE, func
#define NUHW_IOPRESET(func) NUAPI_SETUP_IOPRESET, func
#define NUHW_EJECTDISC(func) NUAPI_SETUP_EJECTDISC, func
#define NUHW_STREAMSIZE(size) NUAPI_SETUP_STREAMSIZE, size
#define NUHW_SOUND(gate, buffer_size, names, parameter) NUAPI_SETUP_SOUND, gate, buffer_size, names, parameter
#define NUHW_NOSOUND(a, b, c, d) NUAPI_SETUP_NOSOUND, a, b, c, d
/* NUHW_SOUND, or NUHW_NOSOUND with the same four values when the game runs without sound. */
#define NUHW_SOUND_OR_NOSOUND(no_sound, gate, buffer_size, names, parameter)                                                               \
    (no_sound) ? NUAPI_SETUP_NOSOUND : NUAPI_SETUP_SOUND, gate, buffer_size, names, parameter
#define NUHW_REVERB(a, b) NUAPI_SETUP_REVERB, a, b
#define NUHW_PAD0(pad) NUAPI_SETUP_PAD0, pad
#define NUHW_PAD1(pad) NUAPI_SETUP_PAD1, pad
#define NUHW_PAD2(pad) NUAPI_SETUP_PAD2, pad
#define NUHW_PAD3(pad) NUAPI_SETUP_PAD3, pad
#define NUHW_VIDEOMODE(mode) NUAPI_SETUP_VIDEOMODE, mode
#define NUHW_GLASSRPLANE(value) NUAPI_SETUP_GLASSRPLANE, value
#define NUHW_DFSNAME(name) NUAPI_SETUP_DFSNAME, name
#define NUHW_SFXDFSNAME(name) NUAPI_SETUP_SFXDFSNAME, name
#define NUHW_DAT_NAMES(names) NUAPI_SETUP_DATNAMES, names, (sizeof(names) / sizeof(names[0])) /* names, count */
#define NUHW_MAXTEXID(count) NUAPI_SETUP_MAXTEXID, count
#define NUHW_VIDEORES(width, height) NUAPI_SETUP_VIDEORES, width, height
#define NUHW_MAXMTL(count) NUAPI_SETUP_MAXMTL, count
#define NUHW_PC_TITLE(title) NUAPI_SETUP_PC_TITLE, title
#define NUHW_PC_WINDOWTITLE(title) NUAPI_SETUP_PC_WINDOWTITLE, title
#define NUHW_PC_REGISTRYKEY(key) NUAPI_SETUP_PC_REGISTRYKEY, key
#define NUHW_PC_SAVEFOLDERPUBLISHER(name) NUAPI_SETUP_PC_SAVEFOLDERPUBLISHER, name
#define NUHW_PC_SAVEFOLDERGAMETITLE(name) NUAPI_SETUP_PC_SAVEFOLDERGAMETITLE, name
#define NUHW_PC_WINDOWED(windowed) NUAPI_SETUP_PC_WINDOWED, windowed
#define NUHW_PC_WINDOW(window) NUAPI_SETUP_PC_WINDOW, window
#define NUHW_ANIMATIONCOUNT(count) NUAPI_SETUP_ANIMATIONCOUNT, count
#define NUHW_OCCLUSIONCAPACITY(capacity) NUAPI_SETUP_OCCLUSIONCAPACITY, capacity
#define NUHW_MAXGAMEPADS(count) NUAPI_SETUP_MAXGAMEPADS, count
#define NUHW_ICON(icon) NUAPI_SETUP_ICON, icon
#define NUHW_SETUP_0x46(value) NUAPI_SETUP_0x46, value
#define NUHW_SETUP_0x47(value) NUAPI_SETUP_0x47, value
#define NUHW_SETUP_0x49(value) NUAPI_SETUP_0x49, value
#define NUHW_SETUP_0x4b(value) NUAPI_SETUP_0x4b, value

#ifdef __cplusplus
extern "C" {
#endif
    extern const char NuHwDefaultConfigName[10];
    extern const char *NuHwSoundDfsName;
    extern const char *NuHwDfsName;
    extern i32 NuHwShowCommand;
    extern u32 NuHwParameter19;
    extern u32 NuHwParameter4f;
    i32 PublishWindow(HWND, i32);
    i32 PCDeviceIsAbsent(void);
    i32 NuInitHardware(VARIPTR *, VARIPTR *, i32, ...);
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus

#endif
#ifdef __cplusplus
extern "C" {
#endif
    i32 PcNativeSelectLanguage(const char *);
    extern u32 PcUseUserGeoRegion;
    void NuHwSetParameter33(void *value);
    void PcSetDefaultParameter(const char *);
#ifdef __cplusplus
}
#endif
