#include "saga.h"

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

extern "C" void __cdecl InitOnce(i32 argc, char **argv) {
    u16 first[] = {0x1e, 0x20, 0x11, 0x1f,   0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0x1d,   0x39,  0x25,
                   0x24, 0x16, 0x23, 0xffff, 0xffff, 0xffff, 0x1c,   0xffff, 0xffff, 0xffff, 0xffff, 0xffff};
    u16 third[] = {0xcb, 0xcd, 0xc8, 0xd0,   0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0x9d,   0x52,  0x51,
                   0x50, 0x4c, 0x4f, 0xffff, 0xffff, 0xffff, 0x9c,   0xffff, 0xffff, 0xffff, 0xffff, 0xffff};
    i8 order[] = {0, 1, 2, 3, 12, 13, 14, 15, 16, 18, 23, 4, 5, 6, 7, 8, 10, 11, 9, -1};
    STATIC_ASSERT(sizeof(first) == 50);
    STATIC_ASSERT(sizeof(third) == 50);
    STATIC_ASSERT(sizeof(order) == 20);
    STUBBED_NAMED(0x006dc890); /* (configuration-key75152c), 4011fe */
    original_permbuffer_base.void_ptr = NuAlloc(SUPERBUFFERSIZE, InitOnceAllocSource, 0x112);
    permbuffer_base = original_permbuffer_base;
    superbuffer_end.addr = original_permbuffer_base.addr + (u32)SUPERBUFFERSIZE;
    /* Native uses the permanent arena without an allocation-failure branch here. */
    STUBBED_NAMED(0x00428b90);
    StartupParseArguments(argc - 1, argv + 1);
    NuFileSetCurrentDirectory(InitOnceCurrentDirectory);
    STUBBED_NAMED(0x00602930); /* (1.0f, .5f, 0), 401268 */
    Game_NuPad = &Game_NuPad_Store[0];
    InitPadDescriptorsPc(first, NULL, third, NULL, order);
    NuInitHardwarePc(&permbuffer_base, &superbuffer_end, 0, 4, InitOnceHostFsPc, 24, StartupDatNames, 7, 6,
                     InitOnceFileModePc, 7, InitOnceEjectPc, 8, 0x500000, 18, PAL ? 8 : 0, 33, 512, PAL ? 256 : 224, 26,
                     0x1000, 21, 1, 5, &InitOnceDvdPc, NOSOUND ? 28 : 10, 0, 0x640, 0, 0, 11, 5, 0x1fff, 14, Game_NuPad,
                     15, Game_NuPad + 1, 46, InitOnceWindowTitle, 45, InitOnceWindowTitle, 47, InitOnceEmptyConfigPc,
                     48, InitOnceCompanyPc, 49, InitOnceProductPc, 52, InitOnceSelector34Pc, 70, 1, 71, 1, 73, 1, 75, 1,
                     0);
    NuTimeBarInit();
    pNuCam = NuCameraCreate();
    InitPanel((i32)Game.options_save.widescreen);
    app_tbgameset = NuTimeBarCreateSet(NULL);
    app_tbplayerset = NuTimeBarCreateSet(NULL);
    app_tbaiset = NuTimeBarCreateSet(NULL);
    app_tbdrawset = NuTimeBarCreateSet(NULL);
    Game.options_save.sound_volume = (u8)PcWindowOptions.sfxvolume;
    Game.options_save.music_volume = (u8)PcWindowOptions.musicvolume;
    Game.options_save.master_volume = (u8)PcWindowOptions.mastervolume;
    GameSetSoundVolume(&Game.options_save);
    GameSetMusicVolume(&Game.options_save);
    saveloadInit(&permbuffer_base, permbuffer_end, 0x7e4c, SaveProductCodePc, SaveIconNamePc, SaveUnicodeNamePc, 4);
}
