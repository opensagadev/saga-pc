#include "saga.h"

extern void *debris_cutoff_camera_vec;
extern i32 debris_render_group;

i32 __cdecl LookupDebrisEffectPageIgnore(char *name, i8 page, i32 ignore) {
    i32 i;
    u8 requested_page = (u8)page;

    if (name == NULL) {
        return -1;
    }

    if (requested_page < 8 && edpp_page_used[requested_page] != 0) {
        for (i = 1; i < debinf_count; ++i) {
            DEBRIS_INFO_PC *effect = debtab[i];
            if (i != ignore && effect != NULL && effect->page == requested_page && NuStrICmp(effect->name, name) == 0) {
                return i;
            }
        }
    }

    for (i = 1; i < debinf_count; ++i) {
        DEBRIS_INFO_PC *effect = debtab[i];
        if (i != ignore && effect != NULL &&
            ((effect->page == 0 && edpp_page_used[0] != 0) || (effect->page == 1 && edpp_page_used[1] != 0)) &&
            NuStrICmp(effect->name, name) == 0) {
            return i;
        }
    }

    for (i = 1; i < debinf_count; ++i) {
        DEBRIS_INFO_PC *effect = debtab[i];
        if (i != ignore && effect != NULL && edpp_page_used[(i8)effect->page] != 0 &&
            NuStrICmp(effect->name, name) == 0) {
            return i;
        }
    }
    return -1;
}

i32 __cdecl LookupDebrisEffectPage(char *name, i8 page) {
    return LookupDebrisEffectPageIgnore(name, page, 0);
}

i32 __cdecl LookupDebrisEffectPageOnly(char *name, i8 page) {
    i32 i;
    u8 requested_page = (u8)page;

    if (name == NULL) {
        return -1;
    }

    if (requested_page < 8 && edpp_page_used[requested_page] != 0) {
        for (i = 1; i < debinf_count; ++i) {
            DEBRIS_INFO_PC *effect = debtab[i];
            if (effect != NULL && effect->page == requested_page && NuStrICmp(effect->name, name) == 0) {
                return i;
            }
        }
    }

    for (i = 1; i < debinf_count; ++i) {
        DEBRIS_INFO_PC *effect = debtab[i];
        if (effect != NULL &&
            ((effect->page == 0 && edpp_page_used[0] != 0) || (effect->page == 1 && edpp_page_used[1] != 0)) &&
            NuStrICmp(effect->name, name) == 0) {
            return i;
        }
    }
    return -1;
}

void __cdecl DebrisTypeStatusAlwaysOff(i32 type) {
    if (type != -1) {
        debtab[type]->status = 0;
    }
}

void __cdecl DebrisTypeStatusAlwaysOn(i32 type) {
    if (type != -1) {
        debtab[type]->status = 2;
    }
}

void DebrisSetSeed(i32 seed) {
    debrisseed = (u32)seed;
}

u32 DebrisGetSeed(void) {
    return debrisseed;
}

void DebrisSetTimeIncrement(f32 increment) {
    debris_time_increment = increment;
}

void DebrisSetCutSceneMode(i32 enabled) {
    DebrisCutSceneMode = enabled;
}

void DebrisRegisterCutoffCameraVec(void *position) {
    debris_cutoff_camera_vec = position;
}

void DebrisSetRenderGroup(i32 group) {
    debris_render_group = group;
}

i32 __cdecl PartDebrisCreatePart_candidate(i32 id, NUVEC *position, i32 flag) {
    STUBBED_NAMED("PartDebrisCreatePart_candidate: part template lookup and spawn");
    return -1;
}
