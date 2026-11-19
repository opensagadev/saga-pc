#pragma once

#include "nu2api/nucore/common.h"

#ifdef __cplusplus
extern "C" {
#endif
    extern struct PC_GEOMETRY_BASIS_MATRIX *PcGeometryTransforms;
    extern struct PC_GEOMETRY_OUTPUT *PcGeometryOutput;
#ifdef __cplusplus
}
#endif

struct PC_GEOMETRY_PUBLISH_FORMAT;
struct PC_GEOMETRY_PUBLISH_RECORD;
#ifdef __cplusplus
extern "C" {
#endif
    i32 PcRegisterDynamicGeometry(struct PC_GEOMETRY_PUBLISH_FORMAT *);
    extern i32 PcGeometryRegistryCount;
    extern struct PC_GEOMETRY_PUBLISH_RECORD *PcGeometryRegistry;
    extern i32 PcGeometryMatrixCapacity;
    extern i32 PcGeometryOutputCapacity;
#ifdef __cplusplus
}
#endif
