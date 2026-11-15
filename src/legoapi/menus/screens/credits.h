#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/world/world.h"

typedef struct credit_record_s CREDIT_RECORD;
#ifdef __cplusplus
extern "C" {
#endif
    extern CREDIT_RECORD *CreditList;

    void Credits_Load(WORLDINFO *, VARIPTR *, VARIPTR *);
#ifdef __cplusplus
}
#endif
