#ifndef SAGA_LEGOAPI_GIZMOS_DOOR_SPINNER_H
#define SAGA_LEGOAPI_GIZMOS_DOOR_SPINNER_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: legoapi/legoapi_types.h (not all of it earlier in the shim). */

#include "nu2api/nucore/fixed_width.h"

#ifndef SAGA_SHIM
#include "legoapi/legoapi_types.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    extern i32 spinner_gizmotype_id;
    extern i16 GizSpinnerGDeb_Fail[3];
    extern i32 LEGOTHINGSSCENE_TER_SPINBASE;
    extern i32 LEGOTHINGSSCENE_TER_SPINARM;
    void GizSpinner_Update(struct GIZSPINNER_s *spinner);
#ifdef __cplusplus
}
#endif

#endif
