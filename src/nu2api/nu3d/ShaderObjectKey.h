#pragma once

#include "nu2api/nucore/common.h"

#include "gamelib/util/CRC16.h"
#include "gamelib/util/gamelib_util_types.h"
#include "nu2api/nu3d/numtl.h"

struct PC_SHADER_MATERIAL_FILTER;

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
    char save_directory[0x100];                  // 0x004
    char include_directories[0x100];             // 0x104
    u32 unknown_204;                             // 0x204
    void *parsed_source;                         // 0x208
    ShaderCompilerDictionary dictionary;         // 0x20c
    ShaderCompilerBufferAllocator source_buffer; // 0xbb0
    VirtualStackAllocator parser_memory;         // 0xbc0
    ShaderBuilderGen(VirtualStackAllocator &, i32);
    virtual ~ShaderBuilderGen();
    virtual void SetIncludeDirectories(const char *);
    virtual void SetSaveDirectory(const char *);
    virtual void SetProfile(i32, i32, i32);
    virtual void SetForceMode(bool);
    virtual void Prepare(const NUSHADERMTLDESC *, ShaderCompilerDictionary *);
    virtual void Configure(struct PC_SHADER_MATERIAL_FILTER *, ShaderCompilerDictionary *);
    bool LoadSource(const char *, i32);
    void *ParseSource(const char *, i32, ShaderCompilerBufferAllocator *, i32, VirtualStackAllocator *);
    void SaveFile(const char *suffix, const char *name, const void *data, i32 size);
    void *ParseFile(const char *name, ShaderCompilerBufferAllocator *buffer, i32 prefix, VirtualStackAllocator *memory);
    bool LoadFile(const char *name);
    i32 CompileEffect(const char *source, i32 length, void *output, i32 unused);
    void Generate(struct PC_SHADER_MATERIAL_FILTER *, ShaderCompilerBufferAllocator *, ShaderCompilerBufferAllocator *);
    void fun_00745270(const char *name, i32 count);
    bool GetDictionaryForceMode();
    i32 GetDictionaryProfile();
};

typedef ShaderBuilderGen ShaderCompilerBase;

class HLSLShaderBuilder : public ShaderBuilderGen { // size 0xbd0
  public:
    HLSLShaderBuilder(VirtualStackAllocator &);

    virtual ~HLSLShaderBuilder() {
    }

    virtual void Prepare(const NUSHADERMTLDESC *, ShaderCompilerDictionary *);
    virtual void Configure(struct PC_SHADER_MATERIAL_FILTER *, ShaderCompilerDictionary *);
};

typedef HLSLShaderBuilder ShaderCompilerHLSLPc;
#endif
