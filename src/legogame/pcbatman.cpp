#include "gameapi/edtools/edui.h"
#include "gameframework/saveload.h"
#include "globals.h"
#include "legoapi/audio/audio.h"
#include "legoapi/menus/core/panel.h"
#include "legoapi/misc/buffers.h"
#include "legoapi/misc/gamemain.h"
#include "legoapi/world/levels.h"
#include "legogame/game.h"
#include "legogame/pcbatman.h"
#include "nu2api/PCInput.h"
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/nu3d/nutimebar_plain.h"
#include "nu2api/nufile/nufile.h"
#include "nu2api/nuwin32/d3dCore.h"
#include "nu2api/nuwin32/pcapi.h"

extern "C" void PcbatmanNuError(void) {
}

extern "C" {
    i32 SUPERBUFFERSIZE = 0x09600000;
    i32 CHARACTERBUFFERSIZE = 0x02800000;
}

extern "C" {
    VARIPTR original_permbuffer_base = {0};
}

/* Allocates the permanent arena (InitOnce does the same inline). */
extern "C" void fun_004010c0(void) {
    permbuffer_base.void_ptr = NuAlloc(SUPERBUFFERSIZE, ".\\pcbatman.cpp", 0x112);
    superbuffer_end.addr = permbuffer_base.addr + (u32)SUPERBUFFERSIZE;
    original_permbuffer_base = permbuffer_base;
}

extern "C" {
    static void StartupParseArguments(i32 count, char **arguments) {
        while (count > 0) {
            StartupParseArgument(count, arguments);
            count--;
            arguments++;
        }
        StartupGameFlag1 = 1;
        StartupGameFlag0 = 0;
    }
}

extern "C" {
    i32 InitOnceFileMode = 1;
    const char *StartupDatNames[7] = {
        "GAME.DAT", "Episode_I.DAT", "Episode_II.DAT", "Episode_III.DAT", "Episode_IV.DAT", "Episode_V.DAT", "Episode_VI.DAT",
    };
    const char *SaveIconName = "lego.ico";
    char SaveUnicodeName[20] = "LEGO Star Wars Saga";
}

extern "C" {
    i32 InitOnceSelector34 = 0;
    char InitOnceCurrentDirectory[1] = {0};
    i32 InitOnceDvd = 0;
    i32 InitOnceHostFs = 0;
    i32 InitOnceEject = 0;
}
i32 app_tbplayerset = 0;
i32 app_tbaiset = 0;
i32 app_tbdrawset = 0;
i32 app_tbgameset = 0;
extern "C" {
    FRAME_PAD **Game_NuPad = 0;
    FRAME_PAD *Game_NuPad_Store[2] = {0};
}

extern "C" void InitOnce(i32 argc, char **argv) {
    PcNativeSelectLanguage("LUCASARTS\\LEGOSTARWARSSAGA");
    permbuffer_base.void_ptr = NuAlloc(SUPERBUFFERSIZE, ".\\pcbatman.cpp", 0x112);
    superbuffer_end.addr = permbuffer_base.addr + (u32)SUPERBUFFERSIZE;
    original_permbuffer_base = permbuffer_base;
    /* Native uses the permanent arena without an allocation-failure branch here. */
    InitGameBeforeConfig();
    StartupParseArguments(argc - 1, argv + 1);
    NuFileSetCurrentDirectory(InitOnceCurrentDirectory);
    NuRndrConfigureLeaf(1.0f, 0.5f, 0);
    Game_NuPad = &Game_NuPad_Store[0];
    i16 first[] = {0x1e, 0x20, 0x11, 0x1f, -1, -1, -1,   -1, -1, -1, 0x1d, 0x39, 0x25,
                   0x24, 0x16, 0x23, -1,   -1, -1, 0x1c, -1, -1, -1, -1,   -1};
    i16 third[] = {0xcb, 0xcd, 0xc8, 0xd0, -1, -1, -1,   -1, -1, -1, 0x9d, 0x52, 0x51,
                   0x50, 0x4c, 0x4f, -1,   -1, -1, 0x9c, -1, -1, -1, -1,   -1};
    i8 order[] = {0, 1, 2, 3, 12, 13, 14, 15, 16, 18, 23, 4, 5, 6, 7, 8, 10, 11, 9, -1};
    InitPadDescriptors((const u16 *)first, NULL, (const u16 *)third, NULL, order);

    NuInitHardware(&permbuffer_base, &superbuffer_end, 0,
                   NUHW_HOSTFS(InitOnceHostFs),                                       //
                   NUHW_DAT_NAMES(StartupDatNames),                                   //
                   NUHW_IOPRESET(InitOnceFileMode),                                   //
                   NUHW_EJECTDISC(InitOnceEject),                                     //
                   NUHW_STREAMSIZE(0x500000),                                         //
                   NUHW_VIDEOMODE(PAL ? 8 : 0),                                       //
                   NUHW_VIDEORES(512, PAL ? 256 : 224),                               //
                   NUHW_MAXTEXID(0x1000),                                             //
                   NUHW_GLASSRPLANE(1),                                               //
                   NUHW_CDDVDMODE(&InitOnceDvd),                                      //
                   NUHW_SOUND_OR_NOSOUND(NOSOUND, 0, 0x640, 0, 0),                    //
                   NUHW_REVERB(5, 0x1fff),                                            //
                   NUHW_PAD0(Game_NuPad),                                             //
                   NUHW_PAD1(Game_NuPad + 1),                                         //
                   NUHW_PC_WINDOWTITLE("LEGO\xae Star Wars\x99: The Complete Saga"),  //
                   NUHW_PC_TITLE("LEGO\xae Star Wars\x99: The Complete Saga"),        //
                   NUHW_PC_REGISTRYKEY("LUCASARTS\\LEGOSTARWARSSAGA"),                //
                   NUHW_PC_SAVEFOLDERPUBLISHER("LucasArts"),                          //
                   NUHW_PC_SAVEFOLDERGAMETITLE("LEGO Star Wars - The Complete Saga"), //
                   NUHW_PC_WINDOWED(InitOnceSelector34),                              //
                   NUHW_SETUP_0X46(1),                                                //
                   NUHW_SETUP_0X47(1),                                                //
                   NUHW_SETUP_0X49(1),                                                //
                   NUHW_SETUP_0X4B(1),                                                //
                   NUHW_END);

    NuTimeBarInit();
    pNuCam = NuCameraCreate();
    InitPanel(Game.options_save.widescreen);
    app_tbgameset = NuTimeBarCreateSet(NULL);
    app_tbplayerset = NuTimeBarCreateSet(NULL);
    app_tbaiset = NuTimeBarCreateSet(NULL);
    app_tbdrawset = NuTimeBarCreateSet(NULL);
    Game.options_save.sound_volume = (u8)PcWindowOptions.sfxvolume;
    Game.options_save.music_volume = (u8)PcWindowOptions.musicvolume;
    Game.options_save.master_volume = (u8)PcWindowOptions.mastervolume;
    GameSetSoundVolume(&Game.options_save);
    GameSetMusicVolume(&Game.options_save);
    saveloadInit(&permbuffer_base, permbuffer_end, 0x7e4c, SaveProductCode, SaveIconName, SaveUnicodeName, 4);
}
