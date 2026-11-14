#ifndef SAGA_LEGOAPI_MENUS_SCREENS_CREDITS_H
#define SAGA_LEGOAPI_MENUS_SCREENS_CREDITS_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

#include "legoapi/world/world.h"

typedef struct credit_record_pc_s CREDIT_RECORD_PC;
#ifdef __cplusplus
extern "C" {
#endif
    extern CREDIT_RECORD_PC *CreditList;

    void Credits_Load(WORLDINFO *, VARIPTR *, VARIPTR *);
#ifdef __cplusplus
}
#endif

#endif
