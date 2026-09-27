#include "saga.h"

NUDATHDR *__cdecl NuDatSet(NUDATHDR *header) {
    NUDATHDR *dat = curr_dat;
    curr_dat = header;
    return dat;
}

NUDATHDR *__cdecl NuDatGet(void) {
    return curr_dat;
}
