#include "saga.h"

void __cdecl InitOnce(i32 argc, char **argv) {

    /* PC performs early configuration and permanent-buffer allocation first. */
    STUBBED_NAMED(0x006dc890);
    original_permbuffer_base.void_ptr = NuAlloc(SUPERBUFFERSIZE, InitOnceAllocSource, 0x112);
    permbuffer_base.void_ptr = original_permbuffer_base.void_ptr;
    superbuffer_end.addr = permbuffer_base.addr + (u32)SUPERBUFFERSIZE;
    if (permbuffer_base.void_ptr == NULL) {
        STUBBED_NAMED("InitOnce: permanent allocation failed");
        return;
    }
    STUBBED_NAMED(0x00428b90);
    StartupParseArguments(argc - 1, argv != NULL ? argv + 1 : NULL);
    NuFileSetCurrentDirectory(InitOnceCurrentDirectory);
    STUBBED_NAMED(0x00602930);

    /* PC installs the first pad table before hardware setup. */
    Game_NuPad = &Game_NuPad_Store[0];
    STUBBED_NAMED(0x006d62a0); /* pad/setup descriptor preparation */
    STUBBED_NAMED(0x006dbb30); /* hardware/setup varargs dispatcher */

    /* The camera and time-bar helpers depend on the unresolved PC allocator. */
    STUBBED_NAMED(0x005ede60);
    STUBBED_NAMED(0x005d5330); /* camera creation */
    InitPanel((i32)Game.options_save.widescreen);
    STUBBED_NAMED(0x005d8270); /* first time-bar set */
    STUBBED_NAMED(0x005d8270); /* second time-bar set */
    STUBBED_NAMED(0x005d8270); /* third time-bar set */
    STUBBED_NAMED(0x005d8270); /* fourth time-bar set */

    /* Three independent low bytes are copied into the PC option record. */
    Game.options_save.sound_volume = (u8)InitOnceOptionInput0;
    Game.options_save.music_volume = (u8)InitOnceOptionInput1;
    Game.options_save.master_volume = (u8)InitOnceOptionInput2;

    GameSetSoundVolume(&Game.options_save);
    GameSetMusicVolume(&Game.options_save);
    STUBBED_NAMED(0x00543e80);
}
