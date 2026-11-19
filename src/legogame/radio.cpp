#include "gameapi/edptl.h"
#include "globals.h"
#include "legoapi/audio/radio.h"
#include "legoapi/gizmo/base/gizmo.h"
#include "legoapi/gizmo/object/gizmoblowups.h"
#include "legoapi/gizmos/object/newblowup.h"
#include "legoapi/misc/gamemain.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/nuanimation_gen.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"
#include <string.h>

struct RADIO_ENTRY {      // size 0x14
    GIZMOBLOWUP *blowup;  // 0x00
    nuhspecial_s special; // 0x04
    f32 time;             // 0x10
};

f32 RadioLifetime = 20.0f;

extern "C" {
    i32 radios_playing = 0;
    u8 RadioEntries[0xa0] = {0};
}

void ResetRadios(void) {
    memset(RadioEntries, 0, sizeof(RadioEntries));
    radios_playing = 0;
}

f32 RadioPhasePeriod = 0.5f;
f32 RadioScaleFactor = 0.1f;

extern "C" void UpdateRadios(void) {
    if (radios_playing == 0) {
        return;
    }
    radios_playing = 0;
    RADIO_ENTRY *pool = reinterpret_cast<RADIO_ENTRY *>(RadioEntries);
    RADIO_ENTRY *radio;
    i32 index;
    NUVEC scale;
    for (index = 0, radio = pool; index < 8; ++index, ++radio) {
        if (!(radio->time > 0.0f)) {
            continue;
        }
        f32 remaining = radio->time - FRAMETIME;
        radio->time = remaining;
        if (remaining < 0.0f) {
            radio->time = 0.0f;
        } else {
            radios_playing = 1;
        }
        f32 phase = NuFmod(radio->time, RadioPhasePeriod);
        i32 angle = (i32)((f64)phase * 65536.0);
        if (radio->blowup != NULL) {
            GizmoBlowupUpdateMatrix(radio->blowup);
            scale.x = (f32)((f64)NuTrigTable[(angle >> 1) & 0x7fff] * (f64)RadioScaleFactor + 1.0);
            scale.y = scale.z = scale.x;
            NuMtxPreScale(&radio->blowup->transform, &scale);
            radio->blowup->status_flags |= 0x10000;
            PlaySfx("swdisco", &radio->blowup->position);
        } else {
            if (NuSpecialExistsFn(&radio->special) == 0) {
                continue;
            }
            NUMTX *source = NuSpecialGetMtx(&radio->special);
            NUMTX *matrix = NuSpecialGetDrawMtx(&radio->special);
            *matrix = *source;
            if (angle != 0) {
                scale.x = (f32)((f64)NuTrigTable[(angle >> 1) & 0x7fff] * (f64)RadioScaleFactor + 1.0);
                scale.y = scale.z = scale.x;
                NuMtxPreScale(matrix, &scale);
                NuSpecialUpdate(&radio->special);
            }
            PlaySfx("swdisco", &matrix->translation);
        }
    }
}

void PlayRadio(char *special_name, char *blowup_name, i32 play) {
    RADIO_ENTRY *pool = reinterpret_cast<RADIO_ENTRY *>(RadioEntries);
    if (special_name != NULL || blowup_name != NULL) {
        if (play != 0) {
            RADIO_ENTRY *radio;
            i32 i;
            for (i = 0, radio = pool; i < 8; ++i, ++radio) {
                if (radio->time <= 0.0f) {
                    if (blowup_name != NULL) {
                        GIZMO *gizmo = GizmoFindByName(WORLD->gizmo_sys, blowup_gizmotype_id, blowup_name);
                        if (gizmo == NULL) {
                            return;
                        }
                        radio->blowup = gizmo->blowup;
                    } else if (special_name == NULL || NuSpecialFind(WORLD->current_gscn, &radio->special, special_name, 1) == 0) {
                        return;
                    }
                    radio->time = RadioLifetime;
                    radios_playing = 1;
                    return;
                }
            }
            return;
        }
        RADIO_ENTRY *radio;
        i32 i;
        for (i = 0, radio = pool; i < 8; ++i, ++radio) {
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
