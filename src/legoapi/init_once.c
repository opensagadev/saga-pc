#include "saga.h"

void __cdecl InitOnce(i32 argc, char **argv) {
    (void)argc;
    (void)argv;

    /* PC performs early configuration and permanent-buffer allocation first. */
    STUBBED_NAMED(0x006dc890);
    STUBBED_NAMED(0x004eef40); /* permanent allocation; its allocator closure is unresolved */
    STUBBED_NAMED(0x00428b90);
    STUBBED_NAMED(0x00401100);
    STUBBED_NAMED(0x004f6930);
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

    STUBBED_NAMED(0x00547b20);
    STUBBED_NAMED(0x00547ab0);
    STUBBED_NAMED(0x00543e80);
}
