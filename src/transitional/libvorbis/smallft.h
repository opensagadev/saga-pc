/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct {
        int n;
        float *trigcache;
        int *splitcache;
    } drft_lookup;

    void drft_forward(drft_lookup *, float *);
    void drft_backward(drft_lookup *, float *);
    void drft_init(drft_lookup *, int);
    void drft_clear(drft_lookup *);
#ifdef __cplusplus
}
#endif
