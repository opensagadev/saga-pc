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
#include "legoapi/core/input/gamepads.h"
#include "editor/aieditor_state.h"
#include "nu2api/nucore/nustring.h"

extern "C" {
    i32 CHARACTERBUFFERSIZE = 0x02800000;
}

extern "C" void PcbatmanNuError(void) {
}

extern "C" {
    i32 SUPERBUFFERSIZE = 0x09600000;
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

    void PcControlPlaySound(i32 sound) {
        switch (sound) {
            case 6:
                GameAudio_PlaySfx(0x32, NULL, 0, 0);
                break;
            case 0:
                GameAudio_PlaySfx(0x31, NULL, 0, 0);
                break;
            case 1:
            case 2:
                GameAudio_PlaySfx(0x30, NULL, 0, 0);
                break;
            case 3:
            case 4:
            case 5:
                GameAudio_PlaySfx(0x2f, NULL, 0, 0);
                break;
        }
    }
}

i32 PcInputCommandAllowed(i32 port, i32 command) {
    if (PcInput_PortIsKeyboard(port) && ((command >= 4 && command <= 7) || (u32)(command - 8) <= 3)) {
        return 0;
    }
    return 1;
}

extern "C" {
    i32 InitOnceFileMode = 1;
    const char *StartupDatNames[7] = {
        "GAME.DAT", "Episode_I.DAT", "Episode_II.DAT", "Episode_III.DAT", "Episode_IV.DAT", "Episode_V.DAT", "Episode_VI.DAT",
    };
    const char *SaveIconName = "lego.ico";
    char SaveUnicodeName[20] = "LEGO Star Wars Saga";
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
    Game.options_save.sound_volume = (u8)PcWindowOptions.GetSfxVolume();
    Game.options_save.music_volume = (u8)PcWindowOptions.GetMusicVolume();
    Game.options_save.master_volume = (u8)PcWindowOptions.GetMasterVolume();
    GameSetSoundVolume(&Game.options_save);
    GameSetMusicVolume(&Game.options_save);
    saveloadInit(&permbuffer_base, permbuffer_end, 0x7e4c, SaveProductCode, SaveIconName, SaveUnicodeName, 4);
}

extern "C" i32 Text_ExpandButtonString(char *command, char *output) {
    if (NuStrICmp(command, "[TAG]") == 0 || NuStrICmp(command, "[TRIANGLE]") == 0 || NuStrICmp(command, "[T]") == 0) {
        NuStrCpy(output, TextButtonString(12, TTab[816], TTab[817]));
        return 1;
    } else if (NuStrICmp(command, "[ACTION]") == 0 || NuStrICmp(command, "[SQUARE]") == 0 || NuStrICmp(command, "[S]") == 0) {
        NuStrCpy(output, TextButtonString(15, TTab[816], TTab[817]));
        return 1;
    } else if (NuStrICmp(command, "[SPECIAL]") == 0 || NuStrICmp(command, "[CIRCLE]") == 0 || NuStrICmp(command, "[O]") == 0) {
        NuStrCpy(output, TextButtonString(13, TTab[816], TTab[817]));
        return 1;
    } else if (NuStrICmp(command, "[JUMP]") == 0 || NuStrICmp(command, "[CROSS]") == 0 || NuStrICmp(command, "[X]") == 0) {
        NuStrCpy(output, TextButtonString(14, TTab[816], TTab[817]));
        return 1;
    } else if (NuStrICmp(command, "[TOGGLELEFT]") == 0) {
        NuStrCpy(output, TextButtonString(16, TTab[816], TTab[817]));
        return 1;
    } else if (NuStrICmp(command, "[TOGGLERIGHT]") == 0) {
        NuStrCpy(output, TextButtonString(18, TTab[816], TTab[817]));
        return 1;
    }
    return 0;
}

/* Empty hook; NuMainRenderWorld calls it after SetDepthOfField. */
void NuMainRenderLeaf(void) {
}

u32 GAMEPAD_TAG = PAD_BUTTON_TRIANGLE;
u32 GAMEPAD_MENUCANCEL = PAD_BUTTON_TRIANGLE;

void GamePad_InitButtons(void) {
    GAMEPAD_JUMP = PAD_BUTTON_CROSS;
    GAMEPAD_ACTION = PAD_BUTTON_SQUARE;
    GAMEPAD_SPECIAL = PAD_BUTTON_CIRCLE;
    GAMEPAD_TAG = PAD_BUTTON_TRIANGLE;
    GAMEPAD_SKIP = PAD_BUTTON_CROSS | PAD_BUTTON_START;
    GAMEPAD_MENUSELECT = PAD_BUTTON_CROSS;
    GAMEPAD_MENUCANCEL = PAD_BUTTON_CIRCLE | PAD_BUTTON_SELECT;
}
