#ifndef SAGA_NU2API_NU3D_ANDROID_NUSHADEROBJECT_H_PART
#define SAGA_NU2API_NU3D_ANDROID_NUSHADEROBJECT_H_PART 1
#endif

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: decomp.h, nu2api/nu3d/numtl.h, nu2api/nucore/common.h, nu2api/nucore/fixed_width.h
 * (not all of it earlier in the shim). */

#ifndef SAGA_SHIM
#include "decomp.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"
#endif
/* 3 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_NU2API_NU3D_ANDROID_NUSHADEROBJECT_H_PART == 1 && !defined(SAGA_NU2API_NU3D_ANDROID_NUSHADEROBJECT_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    void PCShaderNativeUnInit(PC_SHADER_OBJECT_FULL_VIEW *);

    typedef struct PC_SHADER_PARAMETER_LOCAL_PC PC_SHADER_PARAMETER_LOCAL_PC;

    struct PC_SHADER_PARAMETER_LOCAL_PC {
        i16 register_index;
        u16 unknown_02;

        union {
            u32 metadata;

            struct {
                u8 register_count;
                u8 array_count;
                u8 semantic;
                u8 type_stage;
            };
        };

        PC_SHADER_PARAMETER_LOCAL_PC *next;
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

    STATIC_ASSERT(sizeof(PC_SHADER_PARAMETER_LOCAL_PC) == 12);
    STATIC_ASSERT(offsetof(PC_SHADER_PARAMETER_LOCAL_PC, register_index) == 0);
    STATIC_ASSERT(offsetof(PC_SHADER_PARAMETER_LOCAL_PC, metadata) == 4);
    STATIC_ASSERT(offsetof(PC_SHADER_PARAMETER_LOCAL_PC, register_count) == 4);
    STATIC_ASSERT(offsetof(PC_SHADER_PARAMETER_LOCAL_PC, array_count) == 5);
    STATIC_ASSERT(offsetof(PC_SHADER_PARAMETER_LOCAL_PC, semantic) == 6);
    STATIC_ASSERT(offsetof(PC_SHADER_PARAMETER_LOCAL_PC, type_stage) == 7);
    STATIC_ASSERT(offsetof(PC_SHADER_PARAMETER_LOCAL_PC, next) == 8);

    typedef struct D3DX_CONSTANT_DESCRIPTION_PC {
        const char *name;
        i32 register_set;
        UINT register_index;
        UINT register_count;
        i32 class_kind;
        i32 type_kind;
        UINT rows;
        UINT columns;
        UINT elements;
        UINT struct_members;
        UINT bytes;
        const void *default_value;
    } D3DX_CONSTANT_DESCRIPTION_PC;

    STATIC_ASSERT(sizeof(D3DX_CONSTANT_DESCRIPTION_PC) == 48);
    STATIC_ASSERT(offsetof(D3DX_CONSTANT_DESCRIPTION_PC, register_index) == 8);
    STATIC_ASSERT(offsetof(D3DX_CONSTANT_DESCRIPTION_PC, register_count) == 12);
    STATIC_ASSERT(offsetof(D3DX_CONSTANT_DESCRIPTION_PC, class_kind) == 16);
    STATIC_ASSERT(offsetof(D3DX_CONSTANT_DESCRIPTION_PC, type_kind) == 20);
    STATIC_ASSERT(offsetof(D3DX_CONSTANT_DESCRIPTION_PC, elements) == 32);

    typedef struct D3DX_CONSTANT_TABLE_METHODS_PC {
        HRESULT(__stdcall *query_interface)(ID3DXConstantTable *, const IID *, void **);
        ULONG(__stdcall *add_ref)(ID3DXConstantTable *);
        ULONG(__stdcall *release)(ID3DXConstantTable *);
        void *(__stdcall *get_buffer_pointer)(ID3DXConstantTable *);
        DWORD(__stdcall *get_buffer_size)(ID3DXConstantTable *);
        HRESULT(__stdcall *get_description)(ID3DXConstantTable *, void *);
        HRESULT(__stdcall *get_constant_description)
        (ID3DXConstantTable *, const char *, D3DX_CONSTANT_DESCRIPTION_PC *, UINT *);
        UINT(__stdcall *get_sampler_index)(ID3DXConstantTable *, const char *);
        const char *(__stdcall *get_constant)(ID3DXConstantTable *, const char *, UINT);
        const char *(__stdcall *get_constant_by_name)(ID3DXConstantTable *, const char *, const char *);
    } D3DX_CONSTANT_TABLE_METHODS_PC;

    struct ID3DXConstantTable {
        const D3DX_CONSTANT_TABLE_METHODS_PC *methods;
    };

    STATIC_ASSERT(offsetof(D3DX_CONSTANT_TABLE_METHODS_PC, get_constant_description) == 24);
    STATIC_ASSERT(offsetof(D3DX_CONSTANT_TABLE_METHODS_PC, get_constant_by_name) == 36);
    extern const f32 PCSamplerBiasMinimum;
    extern const f32 PCSamplerBiasMaximum;
    extern f32 PCShaderMaterialSamplerBiasOffsetPc;
#ifdef __cplusplus
}
#endif
#undef SAGA_NU2API_NU3D_ANDROID_NUSHADEROBJECT_H_PART
#define SAGA_NU2API_NU3D_ANDROID_NUSHADEROBJECT_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_NU2API_NU3D_ANDROID_NUSHADEROBJECT_H_STOP
#endif
#endif

#if SAGA_NU2API_NU3D_ANDROID_NUSHADEROBJECT_H_PART == 2 && !defined(SAGA_NU2API_NU3D_ANDROID_NUSHADEROBJECT_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    void PCShaderProbeHLSLConstants(PC_SHADER_OBJECT_FULL_VIEW *, ID3DXConstantTable *, ID3DXConstantTable *);
    void PCShaderObjectInitHLSLPc(PC_SHADER_OBJECT_FULL_VIEW *, const u32 *, i32, const char *, i32, const char *, i32,
                                  i32);
#ifdef __cplusplus
}
#endif
#undef SAGA_NU2API_NU3D_ANDROID_NUSHADEROBJECT_H_PART
#define SAGA_NU2API_NU3D_ANDROID_NUSHADEROBJECT_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_NU2API_NU3D_ANDROID_NUSHADEROBJECT_H_STOP
#endif
#endif

#if SAGA_NU2API_NU3D_ANDROID_NUSHADEROBJECT_H_PART == 3 && !defined(SAGA_NU2API_NU3D_ANDROID_NUSHADEROBJECT_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    void PCShaderSetMaterialParametersPc(PC_SHADER_OBJECT_FULL_VIEW *, NUMTL *);
#ifdef __cplusplus
}
#endif
#undef SAGA_NU2API_NU3D_ANDROID_NUSHADEROBJECT_H_PART
#define SAGA_NU2API_NU3D_ANDROID_NUSHADEROBJECT_H_PART 4
#ifdef SAGA_SHIM
#define SAGA_NU2API_NU3D_ANDROID_NUSHADEROBJECT_H_STOP
#endif
#endif

#undef SAGA_NU2API_NU3D_ANDROID_NUSHADEROBJECT_H_STOP
