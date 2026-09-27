#include "saga.h"

NUMTX *__cdecl NuSpecialGetMtx(NUSPECIALHANDLE *special) {
    if (special->display_special != NULL) {
        return special->display_special;
    }
    return special->special;
}

void __cdecl NuSpecialSetMtx(NUSPECIALHANDLE *special, const NUMTX *matrix) {
    if (special->display_special != NULL) {
        *special->display_special = *matrix;
    } else {
        *special->special = *matrix;
    }
}

i32 __cdecl NuSpecialExistsFn(NUSPECIALHANDLE *special) {
    if (special != NULL) {
        if (special->special != NULL || special->display_special != NULL) {
            return 1;
        }
    }
    return 0;
}
