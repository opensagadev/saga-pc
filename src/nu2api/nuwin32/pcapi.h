/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

typedef struct PC_WINDOW_OPTIONS_PC { // size 0x460
    i32 context;                      // 0x000
    char file_name[1024];             // 0x004
    i32 device_width;                 // 0x404
    i32 device_height;                // 0x408
    i32 client_width;                 // 0x40c
    i32 client_height;                // 0x410
    i32 window_x;                     // 0x414
    i32 window_y;                     // 0x418
    i32 refresh_rate;                 // 0x41c
    i32 multisample;                  // 0x420
    i32 aspect_mode;                  // 0x424
    i32 sfxvolume;                    // 0x428
    i32 musicvolume;                  // 0x42c
    i32 mastervolume;                 // 0x430
    u8 musicenabled;                  // 0x434
    u8 unknown_435[3];                // 0x435
    i32 mousesensitivity;             // 0x438
    u8 inverty;                       // 0x43c
    u8 vibration;                     // 0x43d
    u8 unknown_43e[2];                // 0x43e
    i32 bloommethod;                  // 0x440
    i32 desiredbloommethod;           // 0x444
    u8 bloomenabled;                  // 0x448
    u8 unknown_449[3];                // 0x449
    i32 shadowmethod;                 // 0x44c
    i32 desiredshadowmethod;          // 0x450
    u8 shadowsenabled;                // 0x454
    u8 speedblursupported;            // 0x455
    u8 speedblurenabled;              // 0x456
    u8 usehires;                      // 0x457
    u8 usehirespending;               // 0x458
    u8 option459;                     // 0x459
    u8 unknown_45a[2];                // 0x45a
    i32 forceshadermodel;             // 0x45c
#ifdef __cplusplus
    PC_WINDOW_OPTIONS_PC();
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
    i32 fun_006dc710();
    i32 fun_006dc720();
    i32 fun_006dc730();
    i32 fun_006dc740();
    void SetMouseSensitivity(i32 value);
    void SetInvertY(u8 value);
    void SetVibration(u8 value);
    void SetForceShaderModel(i32 value);

    void SetAspectModePc(i32);
    void SetShadowsEnabledPc(u8);
    void SetBloomEnabledPc(u8);
    void SetUseHiresPendingPc(u8);
    u8 fun_00509030();
    u8 fun_00509040();
    u8 fun_00509050();
    u8 fun_00509060();
    u8 fun_00509070();
    u8 fun_00509080();
#endif
} PC_WINDOW_OPTIONS_PC;

#ifdef __cplusplus
extern "C" {
#endif
    void StartupOpenDats(const char **names, i32 count, VARIPTR *arena);
    void PcRegisterSaveFolderPc(const char *company, const char *product, const char *parameter, i32 personal);
    i32 PublishWindowPc(HWND, i32);
#ifdef __cplusplus
    i32 PCDeviceIsAbsent(void);
#ifdef __cplusplus
}
#endif
#endif
#ifdef __cplusplus
extern "C" {
#endif
    extern const char NuHwDefaultConfigNamePc[10];
    extern const char *NuHwSoundDfsName;
    extern const char *NuHwDfsName;
    extern i32 NuHwShowCommand;
    extern u32 NuHwParameter19;
    extern u32 NuHwParameter4f;
    i32 PublishWindowPc(HWND, i32);
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
#ifdef __cplusplus
}
#endif
