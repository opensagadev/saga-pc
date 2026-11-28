#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/items/objects/objectsall.h"

#ifdef __cplusplus
extern "C" {
#endif
    extern PARTTYPE_LOOKUP *edpart_nearest_type;
    void EdFileWriteUnsignedInt(u32 value);
    extern struct parttype_lookup_s *edpart_nearest_type;
#ifdef __cplusplus
}
#endif
