/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

typedef struct PC_WINDOW_OPTIONS_PC {
    i32 context;
    char file_name[1024];
    i32 device_width;
    i32 device_height;
    i32 client_width;
    i32 client_height;
    i32 window_x;
    i32 window_y;
    i32 refresh_rate;
    i32 multisample;
    i32 aspect_mode;
    i32 sfxvolume;
    i32 musicvolume;
    i32 mastervolume;
    u8 musicenabled;
    u8 unknown_435[3];
    i32 mousesensitivity;
    u8 inverty;
    u8 vibration;
    u8 unknown_43e[2];
    i32 bloommethod;
    i32 desiredbloommethod;
    u8 bloomenabled;
    u8 unknown_449[3];
    i32 shadowmethod;
    i32 desiredshadowmethod;
    u8 shadowsenabled;
    u8 speedblursupported;
    u8 speedblurenabled;
    u8 usehires;
    u8 usehirespending;
    u8 option459;
    u8 unknown_45a[2];
    i32 forceshadermodel;
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

STATIC_ASSERT(sizeof(PC_WINDOW_OPTIONS_PC) == 0x460);
STATIC_ASSERT(offsetof(PC_WINDOW_OPTIONS_PC, context) == 0);
STATIC_ASSERT(offsetof(PC_WINDOW_OPTIONS_PC, file_name) == 4);
STATIC_ASSERT(offsetof(PC_WINDOW_OPTIONS_PC, device_width) == 0x404);
STATIC_ASSERT(offsetof(PC_WINDOW_OPTIONS_PC, device_height) == 0x408);
STATIC_ASSERT(offsetof(PC_WINDOW_OPTIONS_PC, client_width) == 0x40c);
STATIC_ASSERT(offsetof(PC_WINDOW_OPTIONS_PC, client_height) == 0x410);
STATIC_ASSERT(offsetof(PC_WINDOW_OPTIONS_PC, window_x) == 0x414);
STATIC_ASSERT(offsetof(PC_WINDOW_OPTIONS_PC, window_y) == 0x418);
STATIC_ASSERT(offsetof(PC_WINDOW_OPTIONS_PC, refresh_rate) == 0x41c);
STATIC_ASSERT(offsetof(PC_WINDOW_OPTIONS_PC, multisample) == 0x420);
STATIC_ASSERT(offsetof(PC_WINDOW_OPTIONS_PC, aspect_mode) == 0x424);
STATIC_ASSERT(offsetof(PC_WINDOW_OPTIONS_PC, sfxvolume) == 0x428);
STATIC_ASSERT(offsetof(PC_WINDOW_OPTIONS_PC, musicvolume) == 0x42c);
STATIC_ASSERT(offsetof(PC_WINDOW_OPTIONS_PC, mastervolume) == 0x430);
STATIC_ASSERT(offsetof(PC_WINDOW_OPTIONS_PC, musicenabled) == 0x434);
STATIC_ASSERT(offsetof(PC_WINDOW_OPTIONS_PC, mousesensitivity) == 0x438);
STATIC_ASSERT(offsetof(PC_WINDOW_OPTIONS_PC, inverty) == 0x43c);
STATIC_ASSERT(offsetof(PC_WINDOW_OPTIONS_PC, vibration) == 0x43d);
STATIC_ASSERT(offsetof(PC_WINDOW_OPTIONS_PC, bloommethod) == 0x440);
STATIC_ASSERT(offsetof(PC_WINDOW_OPTIONS_PC, desiredbloommethod) == 0x444);
STATIC_ASSERT(offsetof(PC_WINDOW_OPTIONS_PC, bloomenabled) == 0x448);
STATIC_ASSERT(offsetof(PC_WINDOW_OPTIONS_PC, shadowmethod) == 0x44c);
STATIC_ASSERT(offsetof(PC_WINDOW_OPTIONS_PC, desiredshadowmethod) == 0x450);
STATIC_ASSERT(offsetof(PC_WINDOW_OPTIONS_PC, shadowsenabled) == 0x454);
STATIC_ASSERT(offsetof(PC_WINDOW_OPTIONS_PC, speedblursupported) == 0x455);
STATIC_ASSERT(offsetof(PC_WINDOW_OPTIONS_PC, speedblurenabled) == 0x456);
STATIC_ASSERT(offsetof(PC_WINDOW_OPTIONS_PC, usehires) == 0x457);
STATIC_ASSERT(offsetof(PC_WINDOW_OPTIONS_PC, usehirespending) == 0x458);
STATIC_ASSERT(offsetof(PC_WINDOW_OPTIONS_PC, option459) == 0x459);
STATIC_ASSERT(offsetof(PC_WINDOW_OPTIONS_PC, forceshadermodel) == 0x45c);
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
    extern const char *NuHwSoundDfsNamePc;
    extern const char *NuHwDfsNamePc;
    extern i32 NuHwShowCommandPc;
    extern u32 NuHwParameter19Pc;
    extern u32 NuHwParameter4fPc;
    i32 PublishWindowPc(HWND, i32);
    i32 PCDeviceIsAbsent(void);
    i32 NuInitHardwarePc(VARIPTR *, VARIPTR *, i32, ...);
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
STATIC_ASSERT(offsetof(PC_WINDOW_OPTIONS_PC, usehires) == 0x457);

#endif
#ifdef __cplusplus
extern "C" {
#endif
    i32 PcNativeSelectLanguage(const char *);
    extern u32 PcUseUserGeoRegionPc;
#ifdef __cplusplus
}
#endif
