#include "gameapi/edtools/edui.h"
#include "nu2api/PCInput.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/nu3d/nudlist.h"
#include "nu2api/nucore/NuSpeedBlurFilter.h"
#include "nu2api/nuinstsurfgeom_gen.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/numath/nuvec4.h"
#include "nu2api/nustencilshadows_PC.h"

struct PC_GEOMETRY_PUBLISH_FORMAT_RECORD { // size 0x78
    u32 unknown_00;                        // 0x00
    i32 element_count;                     // 0x04
    u8 unknown_08[0x70];                   // 0x08
};

struct PC_GEOMETRY_PUBLISH_SKIN_REF { // size 0x1c
    u32 unknown_00;                   // 0x00
    i32 matrix_count;                 // 0x04
    u8 unknown_08[0x14];              // 0x08
};

struct PC_GEOMETRY_PUBLISH_FORMAT {             // size 0x14
    u32 kind;                                   // 0x00
    u32 record_count;                           // 0x04
    u32 skin_ref_count;                         // 0x08
    PC_GEOMETRY_PUBLISH_FORMAT_RECORD *records; // 0x0c
    PC_GEOMETRY_PUBLISH_SKIN_REF *skin_refs;    // 0x10
};

struct PC_GEOMETRY_PUBLISH_RECORD {          // size 0x28
    PC_GEOMETRY_PUBLISH_FORMAT *format;      // 0x00
    i32 element_count;                       // 0x04
    u32 sort_item_count;                     // 0x08
    PC_GEOMETRY_SORT_ITEM *sort_items;       // 0x0c
    u32 strip_count;                         // 0x10
    PC_BUFFER_RESOURCE *vertex_buffer;       // 0x14
    PC_BUFFER_RESOURCE *strip_vertex_buffer; // 0x18
    PC_BUFFER_RESOURCE *strip_index_buffer;  // 0x1c
    u32 frame_stamp;                         // 0x20
    u32 unknown_24;                          // 0x24
};

/* The engine's C++ vector and matrix wrappers (the editor registers its types as "NuVec" and
 * "NuMtx"); this unit keeps the out-of-line copies of their members. */
struct NuMtx : NUMTX {
    NuMtx();
    NuMtx Inverse();
    NuMtx Multiply(NuMtx &other);
};

struct NuVec : NUVEC {
    NuVec();
    NuVec(f32 value);
    NuVec(f32 x, f32 y, f32 z);
    NuVec(const NuVec &other);
    NuVec(const NUVEC &other);
    NuVec &operator=(const NuVec &other);
    NuVec TransformH(NuMtx &matrix);
    NuVec operator/(const NuVec &other) const;
    NuVec operator-() const;
    f32 LengthSquared() const;
};

struct PC_GEOMETRY_PUBLISH_SORT_ITEM { // size 0xc
    u32 output_index;                  // 0x0
    f32 distance_squared;              // 0x4
    u32 unknown_08;                    // 0x8
};

extern "C" i32 PcGeometryBackendAvailable(void) {
    return PCDeviceHardwareVertexProcessingPC >= 4;
}

extern "C" i32 PcRegisterDynamicGeometry(PC_GEOMETRY_PUBLISH_FORMAT *format) {
    if (!PcGeometryBackendAvailable()) {
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

    PC_GEOMETRY_PUBLISH_RECORD *old = PcGeometryRegistry;
    PcGeometryRegistry =
        (PC_GEOMETRY_PUBLISH_RECORD *)NuAlloc((i32)(((u32)PcGeometryRegistryCount + 1) * sizeof(PC_GEOMETRY_PUBLISH_RECORD)),
                                              "d:\\projects\\game_saga\\nu2api.saga\\nu3d\\nuinstsurfgeom_gen.cpp", 0x7c);
    for (i32 i = 0; i < PcGeometryRegistryCount; ++i) {
        PcGeometryRegistry[i] = old[i];
    }
    if (old != NULL) {
        NuFree(old, "d:\\projects\\game_saga\\nu2api.saga\\nu3d\\nuinstsurfgeom_gen.cpp", 0x81);
    }
    ++PcGeometryRegistryCount;
    PC_GEOMETRY_PUBLISH_RECORD *record = &PcGeometryRegistry[PcGeometryRegistryCount - 1];
    record->format = format;
    record->element_count = count;
    record->frame_stamp = 0;
    record->sort_item_count = 0;
    record->strip_count = 0;
    if (!PcInitializeDynamicGeometryBuffers((PC_DYNAMIC_GEOMETRY *)record, count)) {
        return 0;
    }
    record->sort_items = (PC_GEOMETRY_SORT_ITEM *)NuAlloc((i32)((u32)count * sizeof(PC_GEOMETRY_PUBLISH_SORT_ITEM)),
                                                          "d:\\projects\\game_saga\\nu2api.saga\\nu3d\\nuinstsurfgeom_gen.cpp", 0x91);
    for (i32 i = 0; i < count; ++i) {
        ((PC_GEOMETRY_PUBLISH_SORT_ITEM *)record->sort_items)[i].output_index = (u32)i;
    }

    i32 maximum = 0;
    for (u32 i = 0; i < format->skin_ref_count; ++i) {
        if (format->skin_refs[i].matrix_count > maximum) {
            maximum = format->skin_refs[i].matrix_count;
        }
    }
    if (PcGeometryMatrixCapacity < maximum || PcGeometryTransforms == NULL) {
        PcGeometryMatrixCapacity = maximum;
        void *raw =
            NuAlloc((i32)(((u32)maximum + 2) * sizeof(NUMTX)), "d:\\projects\\game_saga\\nu2api.saga\\nu3d\\nuinstsurfgeom_gen.cpp", 0x9d);
        PcGeometryTransforms = (PC_GEOMETRY_BASIS_MATRIX *)(((u32)raw + 0x7f) & 0xffffff00);
        if (PcGeometryTransforms == NULL) {
            PcGeometryMatrixCapacity = -1;
        }
    }
    if (PcGeometryOutputCapacity <= count || PcGeometryOutput == NULL) {
        if (PcGeometryOutput != NULL) {
            NuFree(PcGeometryOutput, "d:\\projects\\game_saga\\nu2api.saga\\nu3d\\nuinstsurfgeom_gen.cpp", 0xaa);
        }
        PcGeometryOutputCapacity = count;
        void *raw = NuAlloc((i32)((u32)count * 0x50 + 0x80), "d:\\projects\\game_saga\\nu2api.saga\\nu3d\\nuinstsurfgeom_gen.cpp", 0xae);
        PcGeometryOutput = (PC_GEOMETRY_OUTPUT *)(((u32)raw + 0x7f) & 0xffffff00);
        if (PcGeometryOutput == NULL) {
            PcGeometryOutputCapacity = -1;
        }
    }
    return 1;
}

extern "C" i32 PCDynamicGeometryRelease(PC_GEOMETRY_PUBLISH_RECORD *record);

extern "C" i32 PcUnregisterDynamicGeometry(PC_GEOMETRY_PUBLISH_FORMAT *format) {
    for (i32 i = 0; i < PcGeometryRegistryCount; ++i) {
        if (format == PcGeometryRegistry[i].format) {
            PCDynamicGeometryRelease(&PcGeometryRegistry[i]);
            if (PcGeometryRegistryCount != 0) {
                PC_GEOMETRY_PUBLISH_RECORD *old = PcGeometryRegistry;
                PcGeometryRegistry =
                    (PC_GEOMETRY_PUBLISH_RECORD *)NuAlloc(((PcGeometryRegistryCount - 1) * sizeof(PC_GEOMETRY_PUBLISH_RECORD)),
                                                          "d:\\projects\\game_saga\\nu2api.saga\\nu3d\\nuinstsurfgeom_gen.cpp", 0xd0);
                i32 count = 0;
                for (i32 j = 0; j < PcGeometryRegistryCount; ++j) {
                    if (format != old[j].format) {
                        PcGeometryRegistry[count++] = old[j];
                    }
                }
                if (old != NULL) {
                    NuFree(old, "d:\\projects\\game_saga\\nu2api.saga\\nu3d\\nuinstsurfgeom_gen.cpp", 0xdf);
                }
                --PcGeometryRegistryCount;
                return 1;
            }
            NuFree(PcGeometryRegistry, "d:\\projects\\game_saga\\nu2api.saga\\nu3d\\nuinstsurfgeom_gen.cpp", 0xc8);
            PcGeometryRegistry = NULL;
            PcGeometryRegistryCount = 0;
        }
    }
    return 0;
}

extern "C" PC_GEOMETRY_PUBLISH_RECORD *PcGeometryRegistryFind(PC_GEOMETRY_PUBLISH_FORMAT *format) {
    for (i32 i = 0; i < PcGeometryRegistryCount; ++i) {
        if (format == PcGeometryRegistry[i].format) {
            return &PcGeometryRegistry[i];
        }
    }
    return NULL;
}

extern "C" NUDISPLAYLIST *NuDisplayList2dGet(void) {
    return &PCDisplayList2d;
}

extern "C" VARIPTR *NuDisplayListStreamAlign16(void) {
    NuDisplayListStreamCursor->addr = (NuDisplayListStreamCursor->addr + 15) & ~15u;
    return NuDisplayListStreamCursor;
}

extern "C" void fun_005c6820(void) {
}

NuVec::NuVec() {
}

NuVec::NuVec(f32 value) {
    x = y = z = value;
}

NuVec::NuVec(f32 x, f32 y, f32 z) {
    this->x = x;
    this->y = y;
    this->z = z;
}

NuVec::NuVec(const NuVec &other) : NUVEC(other) {
}

NuVec::NuVec(const NUVEC &other) : NUVEC(other) {
}

NuVec &NuVec::operator=(const NuVec &other) {
    NUVEC::operator=(other);
    return *this;
}

NuVec NuVec::TransformH(NuMtx &matrix) {
    NuVec result;
    NuVecMtxTransformH(&result, this, &matrix);
    return result;
}

NuVec NuVec::operator/(const NuVec &other) const {
    return NuVec(x / other.x, y / other.y, z / other.z);
}

NuVec NuVec::operator-() const {
    return NuVec(-x, -y, -z);
}

extern "C" NUVEC *NuGeometryVecCross(NUVEC *out, const NUVEC *a, const NUVEC *b) {
    out->x = a->y * b->z - a->z * b->y;
    out->y = a->z * b->x - a->x * b->z;
    out->z = b->y * a->x - b->x * a->y;
    return out;
}

extern "C" f32 NuGeometryVecDot(const NUVEC *a, const NUVEC *b) {
    return a->y * b->y + a->x * b->x + a->z * b->z;
}

f32 NuVec::LengthSquared() const {
    return x * x + y * y + z * z;
}

NuMtx::NuMtx() {
}

NuMtx NuMtx::Inverse() {
    NuMtx result;
    NuMtxInvH(&result, this);
    return result;
}

NuMtx NuMtx::Multiply(NuMtx &other) {
    NuMtx result;
    NuMtxMulH(&result, this, &other);
    return result;
}

extern "C" f32 NuGeometryPlaneDistance(const NUVEC4 *plane, const NUVEC *point) {
    return NuGeometryVecDot(&plane->xyz, point) + plane->w;
}
