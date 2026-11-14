#include "saga.h"

struct PC_GEOMETRY_PUBLISH_FORMAT_RECORD_PC {
    u32 unknown_00;      // 0x00
    i32 element_count;   // 0x04
    u8 unknown_08[0x70]; // 0x08
}; // size 0x78

struct PC_GEOMETRY_PUBLISH_SKIN_REF_PC {
    u32 unknown_00;      // 0x00
    i32 matrix_count;    // 0x04
    u8 unknown_08[0x14]; // 0x08
}; // size 0x1c

struct PC_GEOMETRY_PUBLISH_FORMAT_PC {
    u32 kind;                                      // 0x00
    u32 record_count;                              // 0x04
    u32 skin_ref_count;                            // 0x08
    PC_GEOMETRY_PUBLISH_FORMAT_RECORD_PC *records; // 0x0c
    PC_GEOMETRY_PUBLISH_SKIN_REF_PC *skin_refs;    // 0x10
}; // size 0x14

struct PC_GEOMETRY_PUBLISH_RECORD_PC {
    PC_GEOMETRY_PUBLISH_FORMAT_PC *format;      // 0x00
    i32 element_count;                          // 0x04
    u32 unknown_08;                             // 0x08
    PC_GEOMETRY_SORT_ITEM_PC *sort_items;       // 0x0c
    u32 unknown_10;                             // 0x10
    PC_BUFFER_RESOURCE_PC *vertex_buffer;       // 0x14
    PC_BUFFER_RESOURCE_PC *strip_vertex_buffer; // 0x18
    PC_BUFFER_RESOURCE_PC *strip_index_buffer;  // 0x1c
    u32 frame_stamp;                            // 0x20
    u32 unknown_24;                             // 0x24
}; // size 0x28

struct PC_GEOMETRY_PUBLISH_SORT_ITEM_PC {
    u32 output_index;     // 0x0
    f32 distance_squared; // 0x4
    u32 unknown_08;       // 0x8
}; // size 0xc

extern "C" i32 PcGeometryBackendAvailablePc(void) {
    return PCDeviceHardwareVertexProcessingPC >= 4;
}

extern "C" i32 PcRegisterDynamicGeometryPc(PC_GEOMETRY_PUBLISH_FORMAT_PC *format) {
    if (!PcGeometryBackendAvailablePc()) {
        return 0;
    }
    if (format == NULL || format->kind != 1 || format->record_count == 0) {
        return 0;
    }
    u32 total = 0;
    for (u32 i = 0; i < format->record_count; ++i) {
        total += (u32)format->records[i].element_count;
    }
    i32 count = (i32)total;
    if (count == 0) {
        return 0;
    }

    PC_GEOMETRY_PUBLISH_RECORD_PC *old = PcGeometryRegistryPc;
    PcGeometryRegistryPc = (PC_GEOMETRY_PUBLISH_RECORD_PC *)NuAlloc(
        (i32)(((u32)PcGeometryRegistryCountPc + 1) * sizeof(PC_GEOMETRY_PUBLISH_RECORD_PC)),
        "d:\\projects\\game_saga\\nu2api.saga\\nu3d\\nuinstsurfgeom_gen.cpp", 0x7c);
    for (i32 i = 0; i < PcGeometryRegistryCountPc; ++i) {
        PcGeometryRegistryPc[i] = old[i];
    }
    if (old != NULL) {
        NuFree(old, "d:\\projects\\game_saga\\nu2api.saga\\nu3d\\nuinstsurfgeom_gen.cpp", 0x81);
    }
    ++PcGeometryRegistryCountPc;
    PC_GEOMETRY_PUBLISH_RECORD_PC *record = &PcGeometryRegistryPc[PcGeometryRegistryCountPc - 1];
    record->format = format;
    record->element_count = count;
    record->frame_stamp = 0;
    record->unknown_08 = 0;
    record->unknown_10 = 0;
    if (!PcInitializeDynamicGeometryBuffersPc((PC_DYNAMIC_GEOMETRY_PC *)record, count)) {
        return 0;
    }
    record->sort_items =
        (PC_GEOMETRY_SORT_ITEM_PC *)NuAlloc((i32)((u32)count * sizeof(PC_GEOMETRY_PUBLISH_SORT_ITEM_PC)),
                                            "d:\\projects\\game_saga\\nu2api.saga\\nu3d\\nuinstsurfgeom_gen.cpp", 0x91);
    for (i32 i = 0; i < count; ++i) {
        ((PC_GEOMETRY_PUBLISH_SORT_ITEM_PC *)record->sort_items)[i].output_index = (u32)i;
    }

    i32 maximum = 0;
    for (u32 i = 0; i < format->skin_ref_count; ++i) {
        if (format->skin_refs[i].matrix_count > maximum) {
            maximum = format->skin_refs[i].matrix_count;
        }
    }
    if (PcGeometryMatrixCapacityPc < maximum || PcGeometryTransformsPc == NULL) {
        PcGeometryMatrixCapacityPc = maximum;
        void *raw = NuAlloc((i32)(((u32)maximum + 2) * sizeof(NUMTX)),
                            "d:\\projects\\game_saga\\nu2api.saga\\nu3d\\nuinstsurfgeom_gen.cpp", 0x9d);
        PcGeometryTransformsPc = (PC_GEOMETRY_BASIS_PC *)(((u32)raw + 0x7f) & 0xffffff00);
        if (PcGeometryTransformsPc == NULL) {
            PcGeometryMatrixCapacityPc = -1;
        }
    }
    if (PcGeometryOutputCapacityPc <= count || PcGeometryOutputPc == NULL) {
        if (PcGeometryOutputPc != NULL) {
            NuFree(PcGeometryOutputPc, "d:\\projects\\game_saga\\nu2api.saga\\nu3d\\nuinstsurfgeom_gen.cpp", 0xaa);
        }
        PcGeometryOutputCapacityPc = count;
        void *raw = NuAlloc((i32)((u32)count * 0x50 + 0x80),
                            "d:\\projects\\game_saga\\nu2api.saga\\nu3d\\nuinstsurfgeom_gen.cpp", 0xae);
        PcGeometryOutputPc = (PC_GEOMETRY_OUTPUT_PC *)(((u32)raw + 0x7f) & 0xffffff00);
        if (PcGeometryOutputPc == NULL) {
            PcGeometryOutputCapacityPc = -1;
        }
    }
    return 1;
}

extern "C" i32 fun_005fbd40(PC_GEOMETRY_PUBLISH_RECORD_PC *record);

extern "C" i32 PcUnregisterDynamicGeometryPc(PC_GEOMETRY_PUBLISH_FORMAT_PC *format) {
    for (i32 i = 0; i < PcGeometryRegistryCountPc; ++i) {
        if (format == PcGeometryRegistryPc[i].format) {
            fun_005fbd40(&PcGeometryRegistryPc[i]);
            if (PcGeometryRegistryCountPc != 0) {
                PC_GEOMETRY_PUBLISH_RECORD_PC *old = PcGeometryRegistryPc;
                PcGeometryRegistryPc = (PC_GEOMETRY_PUBLISH_RECORD_PC *)NuAlloc(
                    ((PcGeometryRegistryCountPc - 1) * sizeof(PC_GEOMETRY_PUBLISH_RECORD_PC)),
                    "d:\\projects\\game_saga\\nu2api.saga\\nu3d\\nuinstsurfgeom_gen.cpp", 0xd0);
                i32 count = 0;
                for (i32 j = 0; j < PcGeometryRegistryCountPc; ++j) {
                    if (format != old[j].format) {
                        PcGeometryRegistryPc[count++] = old[j];
                    }
                }
                if (old != NULL) {
                    NuFree(old, "d:\\projects\\game_saga\\nu2api.saga\\nu3d\\nuinstsurfgeom_gen.cpp", 0xdf);
                }
                --PcGeometryRegistryCountPc;
                return 1;
            }
            NuFree(PcGeometryRegistryPc, "d:\\projects\\game_saga\\nu2api.saga\\nu3d\\nuinstsurfgeom_gen.cpp", 0xc8);
            PcGeometryRegistryPc = NULL;
            PcGeometryRegistryCountPc = 0;
        }
    }
    return 0;
}

extern "C" NUDISPLAYLIST *fun_005c6680(void) {
    return &PCDisplayList2dPc;
}

extern "C" VARIPTR *fun_005c6690(void) {
    NuDisplayListStreamCursor->addr = (NuDisplayListStreamCursor->addr + 15) & ~15u;
    return NuDisplayListStreamCursor;
}

extern "C" void fun_005c6820(void) {
}

extern "C" NUVEC *fun_005c6950(NUVEC *out, const NUVEC *a, const NUVEC *b) {
    out->x = a->y * b->z - a->z * b->y;
    out->y = a->z * b->x - a->x * b->z;
    out->z = b->y * a->x - b->x * a->y;
    return out;
}

extern "C" f32 fun_005c6990(const NUVEC *a, const NUVEC *b) {
    return a->y * b->y + a->x * b->x + a->z * b->z;
}

extern "C" f32 fun_005c6b30(const NUVEC4 *plane, const NUVEC *point) {
    return fun_005c6990((const NUVEC *)plane, point) + plane->w;
}
