/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

#ifdef __cplusplus
extern "C" {
#endif
    extern struct PC_GEOMETRY_BASIS_PC *PcGeometryTransformsPc;
    extern struct PC_GEOMETRY_OUTPUT_PC *PcGeometryOutputPc;
#ifdef __cplusplus
}
#endif

typedef struct PC_GEOMETRY_PUBLISH_FORMAT_PC PC_GEOMETRY_PUBLISH_FORMAT_PC;
typedef struct PC_GEOMETRY_PUBLISH_RECORD_PC PC_GEOMETRY_PUBLISH_RECORD_PC;
#ifdef __cplusplus
extern "C" {
#endif
    i32 PcRegisterDynamicGeometryPc(PC_GEOMETRY_PUBLISH_FORMAT_PC *);
    extern i32 PcGeometryRegistryCountPc;
    extern PC_GEOMETRY_PUBLISH_RECORD_PC *PcGeometryRegistryPc;
    extern i32 PcGeometryMatrixCapacityPc;
    extern i32 PcGeometryOutputCapacityPc;
#ifdef __cplusplus
}
#endif
