#include "saga.h"
#include <stdio.h>
void __thiscall PC_WINDOW_OPTIONS_PC::SetWidth(i32 value) {
    client_width = value;
}
void __thiscall PC_WINDOW_OPTIONS_PC::SetHeight(i32 value) {
    client_height = value;
}
void __thiscall PC_WINDOW_OPTIONS_PC::SetDeviceWidth(i32 value) {
    device_width = value;
}
void __thiscall PC_WINDOW_OPTIONS_PC::SetDeviceHeight(i32 value) {
    device_height = value;
}
void __thiscall PC_WINDOW_OPTIONS_PC::SetRefresh(i32 value) {
    refresh_rate = value;
}
void __thiscall PC_WINDOW_OPTIONS_PC::SetMultisample(i32 value) {
    multisample = value;
}
void __thiscall PC_WINDOW_OPTIONS_PC::Save() {
    FILE *file = fopen(file_name, "w");
    if (file != NULL) {
        fprintf(file, "FileVersion            %d\n", PCOptionsFileVersion);
        fprintf(file, "ScreenWidth            %d\n", device_width);
        fprintf(file, "ScreenHeight           %d\n", device_height);
        fprintf(file, "WindowWidth            %d\n", client_width);
        fprintf(file, "WindowHeight           %d\n", client_height);
        fprintf(file, "WindowLeft             %d\n", window_x);
        fprintf(file, "WindowTop              %d\n", window_y);
        fprintf(file, "ScreenRefreshRate      %d\n", refresh_rate);
        fprintf(file, "FSAA                   %d\n", multisample);
        fprintf(file, "Widescreen             %d\n", aspect_mode);
        fprintf(file, "SFXVolume              %d\n", sfxvolume);
        fprintf(file, "MusicVolume            %d\n", musicvolume);
        fprintf(file, "MasterVolume           %d\n", mastervolume);
        fprintf(file, "MusicEnabled           %d\n", musicenabled != 0);
        fprintf(file, "MouseSensitivity       %d\n", mousesensitivity);
        fprintf(file, "InvertY                %d\n", inverty);
        fprintf(file, "Vibration              %d\n", vibration);
        fprintf(file, "BloomMethod            %d\n", bloommethod);
        fprintf(file, "DesiredBloomMethod     %d\n", desiredbloommethod);
        fprintf(file, "BloomEnabled           %d\n", bloomenabled != 0);
        fprintf(file, "ShadowMethod           %d\n", shadowmethod);
        fprintf(file, "DesiredShadowMethod    %d\n", desiredshadowmethod);
        fprintf(file, "ShadowsEnabled         %d\n", shadowsenabled != 0);
        fprintf(file, "SpeedBlurSupported     %d\n", speedblursupported != 0);
        fprintf(file, "SpeedBlurEnabled       %d\n", speedblurenabled != 0);
        fprintf(file, "UseHires               %d\n", usehires != 0);
        fprintf(file, "UseHiresPending        %d\n", usehirespending != 0);
        fprintf(file, "ForceShaderModel       %d\n", forceshadermodel);
        fclose(file);
    }
}

void __thiscall PC_WINDOW_OPTIONS_PC::SetBloomMethod(i32 value) {
    bloommethod = value;
}
void __thiscall PC_WINDOW_OPTIONS_PC::SetShadowMethod(i32 value) {
    shadowmethod = value;
}
void __thiscall PC_WINDOW_OPTIONS_PC::SetSpeedBlurSupported(u8 value) {
    speedblursupported = value;
}

#include <shlobj.h>
#include <string.h>
extern const char *PcCompanyName;
extern const char *PcProductName;
extern i32 PcUsePersonalFolder;
extern char PcUserFolder[260];
extern "C" void __cdecl PCOptionsBuildFolder(char *destination) {
    HRESULT result =
        SHGetFolderPathA(NULL, PcUsePersonalFolder != 0 ? CSIDL_PERSONAL : CSIDL_LOCAL_APPDATA, NULL, 0, PcUserFolder);
    const char *source = result < 0 ? NULL : PcUserFolder;
    strcpy(destination, source);
    strcat(destination, "\\");
    strcat(destination, PcCompanyName);
    strcat(destination, "\\");
    strcat(destination, PcProductName);
    DWORD attributes = GetFileAttributesA(destination);
    strcat(destination, "\\");
    if (attributes == INVALID_FILE_ATTRIBUTES)
        SHCreateDirectoryExA(NULL, destination, NULL);
}
void __thiscall PC_WINDOW_OPTIONS_PC::Defaults() {
    context = -1;
    device_width = PCDeviceBackend.desktop_mode.width <= 1024u ? PCDeviceBackend.desktop_mode.width : 1024u;
    device_height = PCDeviceBackend.desktop_mode.width > 1024u ? 768u : PCDeviceBackend.desktop_mode.height;
    client_width = 1024;
    client_height = 768;
    window_x = window_y = 0;
    refresh_rate = PCDeviceBackend.desktop_mode.refresh_rate;
    sfxvolume = mousesensitivity = 10;
    musicvolume = mastervolume = 8;
    bloommethod = desiredbloommethod = shadowmethod = desiredshadowmethod = 2;
    multisample = 0;
    aspect_mode = 3;
    musicenabled = bloomenabled = shadowsenabled = speedblursupported = speedblurenabled = 1;
    usehires = usehirespending = option459 = 1;
    inverty = vibration = 0;
    forceshadermodel = 0;
}
void __thiscall PC_WINDOW_OPTIONS_PC::Validate() {
    if (device_width <= 0)
        device_width = PCDeviceBackend.desktop_mode.width;
    if (device_height <= 0)
        device_height = PCDeviceBackend.desktop_mode.height;
    if (client_width <= 0)
        client_width = device_width;
    if (client_height <= 0)
        client_height = device_height;
    if (refresh_rate < 0)
        refresh_rate = PCDeviceBackend.desktop_mode.refresh_rate;
    if (multisample < 0 || multisample >= 5)
        multisample = 0;
    if (aspect_mode < 0 || aspect_mode > 3)
        aspect_mode = 3;
    if (sfxvolume < 0 || sfxvolume > 10)
        sfxvolume = 10;
    if (musicvolume < 0 || musicvolume > 10)
        musicvolume = 8;
    if (mastervolume < 0 || mastervolume > 10)
        mastervolume = 8;
    if (bloommethod < 0 || bloommethod >= 3)
        bloommethod = 2;
    if (desiredbloommethod < 0 || desiredbloommethod >= 3)
        desiredbloommethod = 2;
    if (shadowmethod < 0 || shadowmethod >= 3)
        shadowmethod = 2;
    if (desiredshadowmethod < 0 || desiredshadowmethod >= 3)
        desiredshadowmethod = 2;
    if (forceshadermodel < 0 || forceshadermodel > 3)
        forceshadermodel = 0;
}
void __thiscall PC_WINDOW_OPTIONS_PC::InitializePath() {
    PCOptionsBuildFolder(file_name);
    strcat(file_name, "pcconfig.txt");
    Defaults();
}

static void __cdecl PCOptionsReadFileVersion(NUFPAR_PC *parser) {
    PcWindowOptions.context = NuFParGetIntRDP(parser);
}

static void __cdecl PCOptionsReadScreenWidth(NUFPAR_PC *parser) {
    PcWindowOptions.device_width = NuFParGetIntRDP(parser);
}

static void __cdecl PCOptionsReadScreenHeight(NUFPAR_PC *parser) {
    PcWindowOptions.device_height = NuFParGetIntRDP(parser);
}

static void __cdecl PCOptionsReadWindowWidth(NUFPAR_PC *parser) {
    PcWindowOptions.client_width = NuFParGetIntRDP(parser);
}

static void __cdecl PCOptionsReadWindowHeight(NUFPAR_PC *parser) {
    PcWindowOptions.client_height = NuFParGetIntRDP(parser);
}

static void __cdecl PCOptionsReadWindowLeft(NUFPAR_PC *parser) {
    PcWindowOptions.window_x = NuFParGetIntRDP(parser);
}

static void __cdecl PCOptionsReadWindowTop(NUFPAR_PC *parser) {
    PcWindowOptions.window_y = NuFParGetIntRDP(parser);
}

static void __cdecl PCOptionsReadScreenRefreshRate(NUFPAR_PC *parser) {
    PcWindowOptions.refresh_rate = NuFParGetIntRDP(parser);
}

static void __cdecl PCOptionsReadFSAA(NUFPAR_PC *parser) {
    PcWindowOptions.multisample = NuFParGetIntRDP(parser);
}

static void __cdecl PCOptionsReadWidescreen(NUFPAR_PC *parser) {
    PcWindowOptions.aspect_mode = NuFParGetIntRDP(parser);
}

static void __cdecl PCOptionsReadSFXVolume(NUFPAR_PC *parser) {
    PcWindowOptions.sfxvolume = NuFParGetIntRDP(parser);
}

static void __cdecl PCOptionsReadMusicVolume(NUFPAR_PC *parser) {
    PcWindowOptions.musicvolume = NuFParGetIntRDP(parser);
}

static void __cdecl PCOptionsReadMasterVolume(NUFPAR_PC *parser) {
    PcWindowOptions.mastervolume = NuFParGetIntRDP(parser);
}

static void __cdecl PCOptionsReadMouseSensitivity(NUFPAR_PC *parser) {
    PcWindowOptions.mousesensitivity = NuFParGetIntRDP(parser);
}

static void __cdecl PCOptionsReadInvertY(NUFPAR_PC *parser) {
    PcWindowOptions.inverty = NuFParGetIntRDP(parser) > 0;
}

static void __cdecl PCOptionsReadVibration(NUFPAR_PC *parser) {
    PcWindowOptions.vibration = NuFParGetIntRDP(parser) > 0;
}

static void __cdecl PCOptionsReadBloomMethod(NUFPAR_PC *parser) {
    PcWindowOptions.bloommethod = NuFParGetIntRDP(parser);
}

static void __cdecl PCOptionsReadDesiredBloomMethod(NUFPAR_PC *parser) {
    PcWindowOptions.desiredbloommethod = NuFParGetIntRDP(parser);
}

static void __cdecl PCOptionsReadBloomEnabled(NUFPAR_PC *parser) {
    PcWindowOptions.bloomenabled = NuFParGetIntRDP(parser) > 0;
}

static void __cdecl PCOptionsReadShadowMethod(NUFPAR_PC *parser) {
    PcWindowOptions.shadowmethod = NuFParGetIntRDP(parser);
}

static void __cdecl PCOptionsReadDesiredShadowMethod(NUFPAR_PC *parser) {
    PcWindowOptions.desiredshadowmethod = NuFParGetIntRDP(parser);
}

static void __cdecl PCOptionsReadShadowsEnabled(NUFPAR_PC *parser) {
    PcWindowOptions.shadowsenabled = NuFParGetIntRDP(parser) > 0;
}

static void __cdecl PCOptionsReadSpeedBlurSupported(NUFPAR_PC *parser) {
    PcWindowOptions.speedblursupported = NuFParGetIntRDP(parser) > 0;
}

static void __cdecl PCOptionsReadSpeedBlurEnabled(NUFPAR_PC *parser) {
    PcWindowOptions.speedblurenabled = NuFParGetIntRDP(parser) > 0;
}

static void __cdecl PCOptionsReadUseHires(NUFPAR_PC *parser) {
    PcWindowOptions.usehires = NuFParGetIntRDP(parser) > 0;
}

static void __cdecl PCOptionsReadUseHiresPending(NUFPAR_PC *parser) {
    PcWindowOptions.usehirespending = NuFParGetIntRDP(parser) > 0;
}

static void __cdecl PCOptionsReadForceShaderModel(NUFPAR_PC *parser) {
    PcWindowOptions.forceshadermodel = NuFParGetIntRDP(parser);
}

static NUFPCOMJMP PCOptionsCommands[28] = {{"FileVersion", PCOptionsReadFileVersion},
                                           {"ScreenWidth", PCOptionsReadScreenWidth},
                                           {"ScreenHeight", PCOptionsReadScreenHeight},
                                           {"WindowWidth", PCOptionsReadWindowWidth},
                                           {"WindowHeight", PCOptionsReadWindowHeight},
                                           {"WindowLeft", PCOptionsReadWindowLeft},
                                           {"WindowTop", PCOptionsReadWindowTop},
                                           {"ScreenRefreshRate", PCOptionsReadScreenRefreshRate},
                                           {"FSAA", PCOptionsReadFSAA},
                                           {"Widescreen", PCOptionsReadWidescreen},
                                           {"SFXVolume", PCOptionsReadSFXVolume},
                                           {"MusicVolume", PCOptionsReadMusicVolume},
                                           {"MasterVolume", PCOptionsReadMasterVolume},
                                           {"MouseSensitivity", PCOptionsReadMouseSensitivity},
                                           {"InvertY", PCOptionsReadInvertY},
                                           {"Vibration", PCOptionsReadVibration},
                                           {"BloomMethod", PCOptionsReadBloomMethod},
                                           {"DesiredBloomMethod", PCOptionsReadDesiredBloomMethod},
                                           {"BloomEnabled", PCOptionsReadBloomEnabled},
                                           {"ShadowMethod", PCOptionsReadShadowMethod},
                                           {"DesiredShadowMethod", PCOptionsReadDesiredShadowMethod},
                                           {"ShadowsEnabled", PCOptionsReadShadowsEnabled},
                                           {"SpeedBlurSupported", PCOptionsReadSpeedBlurSupported},
                                           {"SpeedBlurEnabled", PCOptionsReadSpeedBlurEnabled},
                                           {"UseHires", PCOptionsReadUseHires},
                                           {"UseHiresPending", PCOptionsReadUseHiresPending},
                                           {"ForceShaderModel", PCOptionsReadForceShaderModel},
                                           {NULL, NULL}};

void __thiscall PC_WINDOW_OPTIONS_PC::Load() {
    char fallback[260];
    NUFPAR_PC *parser = NuFParCreate(file_name);
    if (parser == NULL) {
        NuFileGetCurrentDirectory(fallback);
        strcat(fallback, "pcconfig.txt");
        parser = NuFParCreate(fallback);
        if (parser == NULL) {
            Defaults();
            Save();
            return;
        }
    }
    NuFParPushCom(parser, PCOptionsCommands);
    NuFParGetLine(parser);
    while (*parser->line_buffer_ptr != 0) {
        NuFParGetWord(parser);
        NuFParInterpretWord(parser);
        NuFParGetLine(parser);
    }
    NuFParDestroy(parser);
    if (usehirespending != usehires)
        usehires = usehirespending;
    Validate();
}

void __thiscall PC_WINDOW_OPTIONS_PC::SetDesiredBloomMethod(i32 value) {
    desiredbloommethod = value;
}
void __thiscall PC_WINDOW_OPTIONS_PC::SetDesiredShadowMethod(i32 value) {
    desiredshadowmethod = value;
}
void __thiscall PC_WINDOW_OPTIONS_PC::SetSpeedBlurEnabled(u8 value) {
    speedblurenabled = value;
}
void __thiscall PC_WINDOW_OPTIONS_PC::SetUseHires(u8 value) {
    usehires = value;
    usehirespending = value;
}
void __thiscall PC_WINDOW_OPTIONS_PC::SetOption459(u8 value) {
    option459 = value;
}
