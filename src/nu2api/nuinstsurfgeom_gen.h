#pragma once

#include "nu2api/nucore/common.h"
#include "nu2api/numath/numtx.h"

struct PC_BUFFER_RESOURCE;
struct PC_GEOMETRY_INPUT;
struct PC_GEOMETRY_GROUP;
struct PC_GEOMETRY_BASIS_INPUT_RECORD;
struct PC_GEOMETRY_MORPH_DELTA;

struct PC_GEOMETRY_SORT_ITEM { // size 0xc
    u32 output_index;          // 0x0
    f32 distance_squared;      // 0x4
    u32 unknown_08;            // 0x8
};
struct PC_GEOMETRY_OUTPUT;
struct PC_GEOMETRY_MORPH_WEIGHTS;

struct PC_GEOMETRY_MESH {               // size 0x78
    u32 skin_id;                        // 0x00
    i32 element_count;                  // 0x04
    u32 group_count;                    // 0x08
    struct PC_GEOMETRY_INPUT *elements; // 0x0c
    struct PC_GEOMETRY_GROUP *groups;   // 0x10
    u32 unknown_14;                     // 0x14
    i32 strip_segments;                 // 0x18
    u8 unknown_1c[0x5c];                // 0x1c
};

struct PC_GEOMETRY_BASIS_SOURCE {                   // size 0x1c
    u32 skin_id;                                    // 0x00
    i32 record_count;                               // 0x04
    u32 unknown_08;                                 // 0x08
    i32 morph_count;                                // 0x0c
    u32 unknown_10;                                 // 0x10
    struct PC_GEOMETRY_BASIS_INPUT_RECORD *records; // 0x14
    struct PC_GEOMETRY_MORPH_DELTA *morph_deltas;   // 0x18
};

struct PC_GEOMETRY_FORMAT {                     // size 0x14
    u32 kind;                                   // 0x00
    u32 record_count;                           // 0x04
    u32 skin_ref_count;                         // 0x08
    struct PC_GEOMETRY_MESH *records;           // 0x0c
    struct PC_GEOMETRY_BASIS_SOURCE *skin_refs; // 0x10
};

struct PC_DYNAMIC_GEOMETRY {                        // size 0x28
    struct PC_GEOMETRY_FORMAT *format;              // 0x00
    i32 element_count;                              // 0x04
    u32 matrix_count;                               // 0x08
    struct PC_GEOMETRY_SORT_ITEM *sort_items;       // 0x0c
    NUMTX *matrices;                                // 0x10
    struct PC_BUFFER_RESOURCE *vertex_buffer;       // 0x14
    struct PC_BUFFER_RESOURCE *strip_vertex_buffer; // 0x18
    struct PC_BUFFER_RESOURCE *strip_index_buffer;  // 0x1c
    i32 frame_stamp;                                // 0x20
    u32 unknown_24;                                 // 0x24
};

#ifdef __cplusplus
extern "C" {
#endif
    extern struct PC_GEOMETRY_BASIS_MATRIX *PcGeometryTransforms;
    extern struct PC_GEOMETRY_OUTPUT *PcGeometryOutput;
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
    i32 PcRegisterDynamicGeometry(struct PC_GEOMETRY_FORMAT *);
    struct PC_DYNAMIC_GEOMETRY *PcGeometryRegistryFind(struct PC_GEOMETRY_FORMAT *);
    i32 PCDynamicGeometryLock(struct PC_DYNAMIC_GEOMETRY *, void **);
    i32 PCDynamicGeometryUnlock(struct PC_DYNAMIC_GEOMETRY *);
    i32 PCDynamicGeometryRelease(struct PC_DYNAMIC_GEOMETRY *);
    i32 PCDynamicGeometryCopyOutput(struct PC_DYNAMIC_GEOMETRY *, struct PC_GEOMETRY_OUTPUT *, i32);
    i32 PCGeometrySortItems(struct PC_GEOMETRY_SORT_ITEM *, struct PC_GEOMETRY_OUTPUT *, i32, i32);
    extern i32 PcGeometryRegistryCount;
    extern struct PC_DYNAMIC_GEOMETRY *PcGeometryRegistry;
    extern i32 PcGeometryMatrixCapacity;
    extern i32 PcGeometryOutputCapacity;
#ifdef __cplusplus
}
#endif
