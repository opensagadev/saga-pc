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

typedef struct terrain_dynamic_light_pc_s {
    u8 unknown_0[0x59];
    u8 flags;
} TERRAIN_DYNAMIC_LIGHT_PC;
STATIC_ASSERT(offsetof(TERRAIN_DYNAMIC_LIGHT_PC, flags) == 0x59);
NULNKHDR *__cdecl NuLstGetByIdx(NULSTHDR *, i32);
i32 __cdecl rtlDynamicEnable(i32 id, i32 enabled) {
    TERRAIN_DYNAMIC_LIGHT_PC *light;
    i32 previous;
    if (rtl_dynamic_pool == NULL || id < 0 || id >= rtl_dynamic_max)
        return 0;
    light = (TERRAIN_DYNAMIC_LIGHT_PC *)NuLstGetByIdx(rtl_dynamic_pool, id);
    if (light == NULL)
        return 0;
    previous = (light->flags & 1) == 0;
    light->flags = (light->flags & ~1) | (enabled == 0);
    return previous;
}
