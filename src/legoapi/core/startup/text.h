#ifndef SAGA_LEGOAPI_CORE_STARTUP_TEXT_H
#define SAGA_LEGOAPI_CORE_STARTUP_TEXT_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: legoapi/items/base/apiobject.h (not all of it earlier in the shim). */

#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"

#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void Text_InitDefaultStrings(void);
    void Text_LoadStrings(VARIPTR *buffer, VARIPTR *buffer_end);
    extern const char Text_CrossCodeword[6];
    void Text_DecodeButtons(char *source, char *destination);
    extern i32 (*Tag_NoHiddenIconFn)(GAMEOBJECT *);
    extern const char *LEGOASCII_UP;
#ifdef __cplusplus
}
#endif

#endif
