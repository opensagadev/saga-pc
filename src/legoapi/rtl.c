#include "saga.h"
i32 __cdecl rtlResetDynamic(void) {
    if (rtl_dynamic_pool != NULL) {
        NULNKHDR *entry = NuLstGetNext(rtl_dynamic_pool, NULL);
        while (entry != NULL) {
            NULNKHDR *next = NuLstGetNext(rtl_dynamic_pool, entry);
            NuLstFree(entry);
            entry = next;
        }
        rtl_dynamic_cnt = 0;
    }
    return rtl_dynamic_max;
}
