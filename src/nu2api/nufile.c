#include "saga.h"

void __cdecl PCByteCopy_candidate(void *destination, const void *source, u32 count) {
    u8 *dst = (u8 *)destination;
    const u8 *src = (const u8 *)source;

    if (dst < src) {
        while (count != 0) {
            *dst++ = *src++;
            --count;
        }
    } else {
        dst += count;
        src += count;
        while (count != 0) {
            *--dst = *--src;
            --count;
        }
    }
}

u32 __fastcall PCFilePakChecksum_candidate(const u8 *data, i32 count) {
    u32 sum = 0x12345678;

    while (count != 0) {
        --count;
        sum += *data++;
    }

    return sum;
}

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
