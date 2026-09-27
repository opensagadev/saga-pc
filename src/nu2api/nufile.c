#include "saga.h"

NUDATHDR *__cdecl NuDatOpenEx(const char *filepath, VARIPTR *buf, i32 *unused, i16 mode) {
    (void)filepath;
    (void)buf;
    (void)unused;
    (void)mode;

    /* PC NuDatOpenEx begins by opening the archive through NuFileOpenDF. */
    STUBBED_NAMED(NuFileOpenDF);
    return NULL;
}

NUDATHDR *__cdecl NuDatOpen(const char *filepath, VARIPTR *buf, i32 *unused) {
    return NuDatOpenEx(filepath, buf, unused, 0);
}

NUDATHDR *__cdecl NuDatSet(NUDATHDR *header) {
    NUDATHDR *dat = curr_dat;
    curr_dat = header;
    return dat;
}

NUDATHDR *__cdecl NuDatGet(void) {
    return curr_dat;
}
