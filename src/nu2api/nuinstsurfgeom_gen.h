#pragma once

#include "nu2api/nucore/common.h"

#ifdef __cplusplus
extern "C" {
#endif
    extern struct PC_GEOMETRY_BASIS *PcGeometryTransforms;
    extern struct PC_GEOMETRY_OUTPUT *PcGeometryOutput;
#ifdef __cplusplus
}
#endif

typedef struct PC_GEOMETRY_PUBLISH_FORMAT PC_GEOMETRY_PUBLISH_FORMAT;
typedef struct PC_GEOMETRY_PUBLISH_RECORD PC_GEOMETRY_PUBLISH_RECORD;
#ifdef __cplusplus
extern "C" {
#endif
    i32 PcRegisterDynamicGeometry(PC_GEOMETRY_PUBLISH_FORMAT *);
    extern i32 PcGeometryRegistryCount;
    extern PC_GEOMETRY_PUBLISH_RECORD *PcGeometryRegistry;
    extern i32 PcGeometryMatrixCapacity;
    extern i32 PcGeometryOutputCapacity;
#ifdef __cplusplus
}
#endif
