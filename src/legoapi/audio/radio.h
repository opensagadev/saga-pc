#ifndef SAGA_LEGOAPI_AUDIO_RADIO_H
#define SAGA_LEGOAPI_AUDIO_RADIO_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

#ifdef __cplusplus
extern "C" {
#endif
    void PlayRadio(char *arg0, char *arg1, i32 arg2);
    void ResetRadios(void);
    extern u8 RadioEntriesPc[0xa0];
#ifdef __cplusplus
}
#endif

#endif
