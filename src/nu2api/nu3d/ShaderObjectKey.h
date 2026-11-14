/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "decomp.h"
#include "gamelib/util/CRC16.h"
#include "gamelib/util/gamelib_util_types.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    extern u32 ShaderProgramHashPC[4];
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus

class ShaderBuilderGen { // size 0xbd0
  public:
    char save_directory[0x100];                    // 0x004
    char include_directories[0x100];               // 0x104
    u32 unknown_204;                               // 0x204
    void *parsed_source;                           // 0x208
    ShaderCompilerDictionaryPc dictionary;         // 0x20c
    ShaderCompilerBufferAllocatorPc source_buffer; // 0xbb0
    VirtualStackAllocator parser_memory;           // 0xbc0
    ShaderBuilderGen(VirtualStackAllocator &, i32);
    virtual ~ShaderBuilderGen();
    virtual void SetIncludeDirectories(const char *);
    virtual void SetSaveDirectory(const char *);
    virtual void SetProfile(i32, i32, i32);
    virtual void SetForceMode(bool);
    virtual void Prepare(const PC_SHADER_FILTER_DESC_VIEW *, ShaderCompilerDictionaryPc *);
    virtual void Configure(PC_SHADER_MATERIAL_FILTER_VIEW *, ShaderCompilerDictionaryPc *);
    bool LoadSource(const char *, i32);
    void *ParseSource(const char *, i32, ShaderCompilerBufferAllocatorPc *, i32, VirtualStackAllocator *);
    void SaveFile(const char *suffix, const char *name, const void *data, i32 size);
    void *ParseFile(const char *name, ShaderCompilerBufferAllocatorPc *buffer, i32 prefix,
                    VirtualStackAllocator *memory);
    bool LoadFile(const char *name);
    i32 CompileEffect(const char *source, i32 length, void *output, i32 unused);
    void Generate(PC_SHADER_MATERIAL_FILTER_VIEW *, ShaderCompilerBufferAllocatorPc *,
                  ShaderCompilerBufferAllocatorPc *);
    void fun_00745270(const char *name, i32 count);
    bool fun_00745290();
    i32 fun_007452a0();
};

typedef ShaderBuilderGen ShaderCompilerBasePc;

class HLSLShaderBuilder : public ShaderBuilderGen { // size 0xbd0
  public:
    HLSLShaderBuilder(VirtualStackAllocator &);

    virtual ~HLSLShaderBuilder() {
    }

    virtual void Prepare(const PC_SHADER_FILTER_DESC_VIEW *, ShaderCompilerDictionaryPc *);
    virtual void Configure(PC_SHADER_MATERIAL_FILTER_VIEW *, ShaderCompilerDictionaryPc *);
};

typedef HLSLShaderBuilder ShaderCompilerHLSLPc;
#endif
