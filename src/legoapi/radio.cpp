#include "saga.h"
#include <string.h>

struct RADIO_ENTRY_PC {
    GIZMOBLOWUP_PC *blowup;
    NUSPECIALHANDLE special;
    f32 time;
};
STATIC_ASSERT(sizeof(RADIO_ENTRY_PC) == 20);
STATIC_ASSERT(offsetof(RADIO_ENTRY_PC, blowup) == 0);
STATIC_ASSERT(offsetof(RADIO_ENTRY_PC, special) == 4);
STATIC_ASSERT(offsetof(RADIO_ENTRY_PC, time) == 0x10);

f32 RadioLifetimePc = 20.0f;

void __cdecl ResetRadios(void) {
    memset(RadioEntriesPc, 0, sizeof(RadioEntriesPc));
    radios_playing = 0;
}

f32 RadioPhasePeriodPc = 0.5f;
f32 RadioScaleFactorPc = 0.1f;

extern "C" void __cdecl UpdateRadios(void) {
    if (radios_playing == 0)
        return;
    radios_playing = 0;
    RADIO_ENTRY_PC *pool = reinterpret_cast<RADIO_ENTRY_PC *>(RadioEntriesPc);
    for (i32 index = 0; index < 8; ++index) {
        RADIO_ENTRY_PC *radio = &pool[index];
        if (!(radio->time > 0.0f))
            continue;
        f32 remaining = radio->time - FRAMETIME;
        radio->time = remaining;
        if (remaining < 0.0f)
            radio->time = 0.0f;
        else
            radios_playing = 1;
        f32 phase = NuFmod(radio->time, RadioPhasePeriodPc);
        i32 angle = (i32)((f64)phase * 65536.0);
        if (radio->blowup != NULL) {
            GizmoBlowupUpdateMatrix(radio->blowup);
            NUVEC scale;
            scale.x = (f32)((f64)NuTrigTable[(angle >> 1) & 0x7fff] * (f64)RadioScaleFactorPc + 1.0);
            scale.y = scale.z = scale.x;
            NuMtxPreScale(&radio->blowup->transform, &scale);
            radio->blowup->status_flags |= 0x10000;
            PlaySfx("swdisco", &radio->blowup->position);
        } else {
            if (NuSpecialExistsFn(&radio->special) == 0)
                continue;
            NUMTX *source = NuSpecialGetMtx(&radio->special);
            NUMTX *matrix = NuSpecialGetDrawMtx(&radio->special);
            *matrix = *source;
            if (angle != 0) {
                NUVEC scale;
                scale.x = (f32)((f64)NuTrigTable[(angle >> 1) & 0x7fff] * (f64)RadioScaleFactorPc + 1.0);
                scale.y = scale.z = scale.x;
                NuMtxPreScale(matrix, &scale);
                NuSpecialUpdate(&radio->special);
            }
            PlaySfx("swdisco", &matrix->translation);
        }
    }
}

void __cdecl PlayRadio(char *special_name, char *blowup_name, i32 play) {
    RADIO_ENTRY_PC *pool = reinterpret_cast<RADIO_ENTRY_PC *>(RadioEntriesPc);
    if (special_name != NULL || blowup_name != NULL) {
        if (play != 0) {
            for (i32 i = 0; i < 8; ++i) {
                RADIO_ENTRY_PC *radio = &pool[i];
                if (radio->time <= 0.0f) {
                    if (blowup_name != NULL) {
                        GIZMO_PC *gizmo =
                            (GIZMO_PC *)GizmoFindByName(WORLD->gizmo_sys, blowup_gizmotype_id, blowup_name);
                        if (gizmo == NULL)
                            return;
                        radio->blowup = (GIZMOBLOWUP_PC *)gizmo->object;
                    } else if (special_name == NULL ||
                               NuSpecialFind(WORLD->current_gscn, &radio->special, special_name, 1) == 0) {
                        return;
                    }
                    radio->time = RadioLifetimePc;
                    radios_playing = 1;
                    return;
                }
            }
            return;
        }
        for (i32 i = 0; i < 8; ++i) {
            RADIO_ENTRY_PC *radio = &pool[i];
            if (blowup_name != NULL) {
                if (radio->blowup != NULL && NuStrICmp(radio->blowup->name, blowup_name) == 0) {
                    GizmoBlowupUpdateMatrix(radio->blowup);
                    radio->blowup->status_flags |= 0x10000;
                    memset(radio, 0, sizeof(*radio));
                    return;
                }
                if (special_name != NULL) {
                    char *name = NuSpecialGetName(&radio->special);
                    if (name != NULL && NuStrICmp(name, special_name) == 0) {
                        NUMTX *source = NuSpecialGetMtx(&radio->special);
                        NUMTX *instance = NuSpecialGetInstanceMtx(&radio->special);
                        *instance = *source;
                        NuSpecialUpdate(&radio->special);
                        memset(radio, 0, sizeof(*radio));
                        return;
                    }
                }
            }
        }
    }
}
