#include "saga.h"

void __cdecl InitOnce(i32 argc, char **argv) {
    (void)argc;
    (void)argv;

    /* PC calls into startup diagnostics, arena setup, and game config here. */
    STUBBED_NAMED(0x006dc890);
    STUBBED_NAMED(0x004eef40); /* allocates the permanent buffer */
    STUBBED_NAMED(0x00428b90);
    STUBBED_NAMED(0x00401100);
    STUBBED_NAMED(0x004f6930);
    STUBBED_NAMED(0x00602930);

    /* PC installs the first game pad before passing its setup table onward. */
    Game_NuPad = &Game_NuPad_Store[0];
    STUBBED_NAMED(0x006d62a0); /* pad/setup descriptor preparation */
    STUBBED_NAMED(0x006dbb30); /* hardware/setup varargs dispatcher */

    /* The PC startup continues through camera, panel and timing initialization. */
    STUBBED_NAMED(0x005ede60);
    STUBBED_NAMED(0x005d5330); /* Android-correlated camera creation */
    InitPanel((i32)InitOncePanelOption);
    STUBBED_NAMED(0x005d8270); /* first time-bar set */
    STUBBED_NAMED(0x005d8270); /* second time-bar set */
    STUBBED_NAMED(0x005d8270); /* third time-bar set */
    STUBBED_NAMED(0x005d8270); /* fourth time-bar set */

    STUBBED_NAMED(0x00547b20);
    STUBBED_NAMED(0x00547ab0);
    STUBBED_NAMED(0x00543e80);
}
