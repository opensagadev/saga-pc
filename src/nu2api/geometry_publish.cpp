#include "saga.h"

struct PC_GEOMETRY_PUBLISH_FORMAT_RECORD_PC {
    u32 unknown_00;
    i32 element_count;
    u8 unknown_08[0x70];
};
struct PC_GEOMETRY_PUBLISH_SKIN_REF_PC {
    u32 unknown_00;
    i32 matrix_count;
    u8 unknown_08[0x14];
};
struct PC_GEOMETRY_PUBLISH_FORMAT_PC {
    u32 kind;
    u32 record_count;
    u32 skin_ref_count;
    PC_GEOMETRY_PUBLISH_FORMAT_RECORD_PC *records;
    PC_GEOMETRY_PUBLISH_SKIN_REF_PC *skin_refs;
};
struct PC_GEOMETRY_PUBLISH_RECORD_PC {
    PC_GEOMETRY_PUBLISH_FORMAT_PC *format;
    i32 element_count;
    u32 unknown_08;
    PC_GEOMETRY_SORT_ITEM_PC *sort_items;
    u32 unknown_10;
    PC_BUFFER_RESOURCE_PC *vertex_buffer;
    PC_BUFFER_RESOURCE_PC *strip_vertex_buffer;
    PC_BUFFER_RESOURCE_PC *strip_index_buffer;
    u32 frame_stamp;
    u32 unknown_24;
};
struct PC_GEOMETRY_PUBLISH_SORT_ITEM_PC {
    u32 output_index;
    f32 distance_squared;
    u32 unknown_08;
};
STATIC_ASSERT(sizeof(PC_GEOMETRY_PUBLISH_FORMAT_RECORD_PC) == 0x78);
STATIC_ASSERT(offsetof(PC_GEOMETRY_PUBLISH_FORMAT_RECORD_PC, element_count) == 4);
STATIC_ASSERT(sizeof(PC_GEOMETRY_PUBLISH_SKIN_REF_PC) == 0x1c);
STATIC_ASSERT(offsetof(PC_GEOMETRY_PUBLISH_SKIN_REF_PC, matrix_count) == 4);
STATIC_ASSERT(offsetof(PC_GEOMETRY_PUBLISH_FORMAT_PC, kind) == 0);
STATIC_ASSERT(offsetof(PC_GEOMETRY_PUBLISH_FORMAT_PC, record_count) == 4);
STATIC_ASSERT(offsetof(PC_GEOMETRY_PUBLISH_FORMAT_PC, skin_ref_count) == 8);
STATIC_ASSERT(offsetof(PC_GEOMETRY_PUBLISH_FORMAT_PC, records) == 0xc);
STATIC_ASSERT(offsetof(PC_GEOMETRY_PUBLISH_FORMAT_PC, skin_refs) == 0x10);
STATIC_ASSERT(sizeof(PC_GEOMETRY_PUBLISH_RECORD_PC) == 0x28);
STATIC_ASSERT(offsetof(PC_GEOMETRY_PUBLISH_RECORD_PC, format) == 0);
STATIC_ASSERT(offsetof(PC_GEOMETRY_PUBLISH_RECORD_PC, element_count) == 4);
STATIC_ASSERT(offsetof(PC_GEOMETRY_PUBLISH_RECORD_PC, unknown_08) == 8);
STATIC_ASSERT(offsetof(PC_GEOMETRY_PUBLISH_RECORD_PC, sort_items) == 0xc);
STATIC_ASSERT(offsetof(PC_GEOMETRY_PUBLISH_RECORD_PC, unknown_10) == 0x10);
STATIC_ASSERT(offsetof(PC_GEOMETRY_PUBLISH_RECORD_PC, vertex_buffer) == 0x14);
STATIC_ASSERT(offsetof(PC_GEOMETRY_PUBLISH_RECORD_PC, strip_vertex_buffer) == 0x18);
STATIC_ASSERT(offsetof(PC_GEOMETRY_PUBLISH_RECORD_PC, strip_index_buffer) == 0x1c);
STATIC_ASSERT(offsetof(PC_GEOMETRY_PUBLISH_RECORD_PC, frame_stamp) == 0x20);
STATIC_ASSERT(sizeof(PC_GEOMETRY_PUBLISH_SORT_ITEM_PC) == 0xc);
STATIC_ASSERT(offsetof(PC_GEOMETRY_PUBLISH_SORT_ITEM_PC, output_index) == 0);

extern "C" {
    extern i32 PcGeometryRegistryCountPc;
    extern PC_GEOMETRY_PUBLISH_RECORD_PC *PcGeometryRegistryPc;
    extern i32 PcGeometryMatrixCapacityPc;
    extern i32 PcGeometryOutputCapacityPc;
}

extern "C" i32 __cdecl PcGeometryBackendAvailablePc(void) {
    return PCDeviceHardwareVertexProcessingPC >= 4;
}

extern "C" i32 __cdecl PcRegisterDynamicGeometryPc(PC_GEOMETRY_PUBLISH_FORMAT_PC *format) {
    if (!PcGeometryBackendAvailablePc())
        return 0;
    if (format == NULL || format->kind != 1 || format->record_count == 0)
        return 0;
    u32 total = 0;
    for (u32 i = 0; i < format->record_count; ++i)
        total += (u32)format->records[i].element_count;
    i32 count = (i32)total;
    if (count == 0)
        return 0;

    PC_GEOMETRY_PUBLISH_RECORD_PC *old = PcGeometryRegistryPc;
    PcGeometryRegistryPc = (PC_GEOMETRY_PUBLISH_RECORD_PC *)NuAlloc(
        (i32)(((u32)PcGeometryRegistryCountPc + 1) * sizeof(PC_GEOMETRY_PUBLISH_RECORD_PC)),
        "d:\\projects\\game_saga\\nu2api.saga\\nu3d\\nuinstsurfgeom_gen.cpp", 0x7c);
    for (i32 i = 0; i < PcGeometryRegistryCountPc; ++i)
        PcGeometryRegistryPc[i] = old[i];
    if (old != NULL)
        NuFree(old, "d:\\projects\\game_saga\\nu2api.saga\\nu3d\\nuinstsurfgeom_gen.cpp", 0x81);
    ++PcGeometryRegistryCountPc;
    PC_GEOMETRY_PUBLISH_RECORD_PC *record = &PcGeometryRegistryPc[PcGeometryRegistryCountPc - 1];
    record->format = format;
    record->element_count = count;
    record->frame_stamp = 0;
    record->unknown_08 = 0;
    record->unknown_10 = 0;
    if (!PcInitializeDynamicGeometryBuffersPc((PC_DYNAMIC_GEOMETRY_PC *)record, count))
        return 0;
    record->sort_items =
        (PC_GEOMETRY_SORT_ITEM_PC *)NuAlloc((i32)((u32)count * sizeof(PC_GEOMETRY_PUBLISH_SORT_ITEM_PC)),
                                            "d:\\projects\\game_saga\\nu2api.saga\\nu3d\\nuinstsurfgeom_gen.cpp", 0x91);
    for (i32 i = 0; i < count; ++i)
        ((PC_GEOMETRY_PUBLISH_SORT_ITEM_PC *)record->sort_items)[i].output_index = (u32)i;

    i32 maximum = 0;
    for (u32 i = 0; i < format->skin_ref_count; ++i)
        if (maximum < format->skin_refs[i].matrix_count)
            maximum = format->skin_refs[i].matrix_count;
    if (PcGeometryMatrixCapacityPc < maximum || PcGeometryTransformsPc == NULL) {
        PcGeometryMatrixCapacityPc = maximum;
        void *raw = NuAlloc((i32)(((u32)maximum + 2) * sizeof(NUMTX)),
                            "d:\\projects\\game_saga\\nu2api.saga\\nu3d\\nuinstsurfgeom_gen.cpp", 0x9d);
        PcGeometryTransformsPc = (PC_GEOMETRY_BASIS_PC *)(((u32)raw + 0x7f) & 0xffffff00);
        if (PcGeometryTransformsPc == NULL)
            PcGeometryMatrixCapacityPc = -1;
    }
    if (PcGeometryOutputCapacityPc <= count || PcGeometryOutputPc == NULL) {
        if (PcGeometryOutputPc != NULL)
            NuFree(PcGeometryOutputPc, "d:\\projects\\game_saga\\nu2api.saga\\nu3d\\nuinstsurfgeom_gen.cpp", 0xaa);
        PcGeometryOutputCapacityPc = count;
        void *raw = NuAlloc((i32)((u32)count * 0x50 + 0x80),
                            "d:\\projects\\game_saga\\nu2api.saga\\nu3d\\nuinstsurfgeom_gen.cpp", 0xae);
        PcGeometryOutputPc = (PC_GEOMETRY_OUTPUT_PC *)(((u32)raw + 0x7f) & 0xffffff00);
        if (PcGeometryOutputPc == NULL)
            PcGeometryOutputCapacityPc = -1;
    }
    return 1;
}
