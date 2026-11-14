/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

/* saga.h includes this header once per part (SAGA_SHIM), so `#pragma once` holds for direct includes only; the
 * condition and the _PART selector go away with the shim. */
#ifndef SAGA_SHIM
#pragma once
#endif
#ifndef SAGA_NU2API_NU3D_NUSHADEROBJECT_H_PART
#define SAGA_NU2API_NU3D_NUSHADEROBJECT_H_PART 1
#endif

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "decomp.h"
#endif

/* 3 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_NU2API_NU3D_NUSHADEROBJECT_H_PART == 1 && !defined(SAGA_NU2API_NU3D_NUSHADEROBJECT_H_STOP)

struct numtl_s;

#ifdef __cplusplus
extern "C" {
#endif
    void PCShaderNativeUnInit(PC_SHADER_OBJECT_FULL_VIEW *);

    typedef struct PC_SHADER_PARAMETER_LOCAL_PC PC_SHADER_PARAMETER_LOCAL_PC;

    struct PC_SHADER_PARAMETER_LOCAL_PC { // size 0xc
        i16 register_index;               // 0x0
        u16 unknown_02;                   // 0x2

        union { // size 0x4
            u32 metadata;

            struct {               // size 0x4
                u8 register_count; // 0x4
                u8 array_count;    // 0x5
                u8 semantic;       // 0x6
                u8 type_stage;     // 0x7
            };
        }; // 0x4

        PC_SHADER_PARAMETER_LOCAL_PC *next; // 0x8
#ifdef __cplusplus
        void SetScalarValues(i32 first_element, i32 count, const f32 *values);
        void SetVectorValues(i32 first_element, i32 count, const f32 *values);
        void SetMatrixValues(i32 first_element, i32 count, const f32 *values);
        void SetTransposedMatrixValues(i32 first_element, i32 count, const f32 *values);
        void SetScalar(f32 value);
        void SetVector(const f32 *values);
        void SetVectorElement(i32 index, const f32 *values);
        void SetFloat2(const f32 *values);
        void SetFloat3(const f32 *values);
        void SetValues(i32 first_element, i32 count, const f32 *values);
        i32 fun_00742180();
        PC_SHADER_PARAMETER_LOCAL_PC *fun_00742190();
        void fun_007421a0(PC_SHADER_PARAMETER_LOCAL_PC *parameter);
        PC_SHADER_PARAMETER_LOCAL_PC *fun_007421b0();
        void fun_007424a0(i32 first_element, const f32 *values);
        void fun_00742720(i32 index, const f32 *values);
        i32 fun_00743c90();
        void fun_00743e10(i32 first_element, i32 count, const f32 *values);
#endif
    };

    typedef struct D3DX_CONSTANT_DESCRIPTION_PC { // size 0x30
        const char *name;                         // 0x00
        i32 register_set;                         // 0x04
        UINT register_index;                      // 0x08
        UINT register_count;                      // 0x0c
        i32 class_kind;                           // 0x10
        i32 type_kind;                            // 0x14
        UINT rows;                                // 0x18
        UINT columns;                             // 0x1c
        UINT elements;                            // 0x20
        UINT struct_members;                      // 0x24
        UINT bytes;                               // 0x28
        const void *default_value;                // 0x2c
    } D3DX_CONSTANT_DESCRIPTION_PC;

    typedef struct D3DX_CONSTANT_TABLE_METHODS_PC {                                      // size 0x28
        HRESULT(__stdcall *query_interface)(ID3DXConstantTable *, const IID *, void **); // 0x00
        ULONG(__stdcall *add_ref)(ID3DXConstantTable *);                                 // 0x04
        ULONG(__stdcall *release)(ID3DXConstantTable *);                                 // 0x08
        void *(__stdcall *get_buffer_pointer)(ID3DXConstantTable *);                     // 0x0c
        DWORD(__stdcall *get_buffer_size)(ID3DXConstantTable *);                         // 0x10
        HRESULT(__stdcall *get_description)(ID3DXConstantTable *, void *);               // 0x14
        HRESULT(__stdcall *get_constant_description)                                     // 0x18
        (ID3DXConstantTable *, const char *, D3DX_CONSTANT_DESCRIPTION_PC *, UINT *);
        UINT(__stdcall *get_sampler_index)(ID3DXConstantTable *, const char *);                          // 0x1c
        const char *(__stdcall *get_constant)(ID3DXConstantTable *, const char *, UINT);                 // 0x20
        const char *(__stdcall *get_constant_by_name)(ID3DXConstantTable *, const char *, const char *); // 0x24
    } D3DX_CONSTANT_TABLE_METHODS_PC;

    struct ID3DXConstantTable {
        const D3DX_CONSTANT_TABLE_METHODS_PC *methods;
    };

    extern const f32 PCSamplerBiasMinimum;
    extern const f32 PCSamplerBiasMaximum;
    extern f32 PCShaderMaterialSamplerBiasOffsetPc;
#ifdef __cplusplus
}
#endif
#undef SAGA_NU2API_NU3D_NUSHADEROBJECT_H_PART
#define SAGA_NU2API_NU3D_NUSHADEROBJECT_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_NU2API_NU3D_NUSHADEROBJECT_H_STOP
#endif
#endif

#if SAGA_NU2API_NU3D_NUSHADEROBJECT_H_PART == 2 && !defined(SAGA_NU2API_NU3D_NUSHADEROBJECT_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    void PCShaderProbeHLSLConstants(PC_SHADER_OBJECT_FULL_VIEW *, ID3DXConstantTable *, ID3DXConstantTable *);
    void PCShaderObjectInitHLSLPc(PC_SHADER_OBJECT_FULL_VIEW *, const u32 *, i32, const char *, i32, const char *, i32,
                                  i32);
#ifdef __cplusplus
}
#endif
#undef SAGA_NU2API_NU3D_NUSHADEROBJECT_H_PART
#define SAGA_NU2API_NU3D_NUSHADEROBJECT_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_NU2API_NU3D_NUSHADEROBJECT_H_STOP
#endif
#endif

#if SAGA_NU2API_NU3D_NUSHADEROBJECT_H_PART == 3 && !defined(SAGA_NU2API_NU3D_NUSHADEROBJECT_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    void PCShaderSetMaterialParametersPc(PC_SHADER_OBJECT_FULL_VIEW *, struct numtl_s *);
#ifdef __cplusplus
}
#endif
#undef SAGA_NU2API_NU3D_NUSHADEROBJECT_H_PART
#define SAGA_NU2API_NU3D_NUSHADEROBJECT_H_PART 4
#ifdef SAGA_SHIM
#define SAGA_NU2API_NU3D_NUSHADEROBJECT_H_STOP
#endif
#endif

#undef SAGA_NU2API_NU3D_NUSHADEROBJECT_H_STOP
