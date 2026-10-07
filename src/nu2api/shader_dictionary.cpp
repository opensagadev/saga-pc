#include "saga.h"
extern "C" void __cdecl ShaderLexicalClassesInitPc(void);
ShaderCompilerDictionaryPc::ShaderCompilerDictionaryPc(VirtualStackAllocator &parent, i32 input_mode)
    : scratch(parent, 0x3000), output_memory(parent, 0x4000), register_buffer(parent, 0x100) {
    unknown_99c = 0;
    ShaderLexicalClassesInitPc();
    mode = input_mode;
    program_count = 0;
    definition_count = 0;
    register_block_count = 0;
    emit_definitions = 1;
    unknown_965 = 0;
    unknown_966 = 0;
    profile = 4;
    first_version = 8;
    second_version = 2;
    force_mode = false;
    if (mode == 1) {
        emit_semantics = 0;
        emit_registers = 0;
        emit_centroid = 0;
        register_format = ": c%d";
        profile_name = "";
    } else {
        emit_semantics = 1;
        emit_registers = 1;
        emit_centroid = 1;
        register_format = ": register(c%d)";
        profile_name = "_centroid";
    }
}
ShaderCompilerDictionaryPc::~ShaderCompilerDictionaryPc() {
}
void ShaderCompilerDictionaryPc::SetProfile(i32 input_profile, i32 first, i32 second) {
    first_version = first;
    profile = input_profile;
    second_version = second;
    if (mode == 0)
        profile_name = profile == 2 || profile == 3 || profile == 4 ? "_centroid" : "";
}
void ShaderCompilerDictionaryPc::AddRegisterBlock(const char *name, i32 count) {
    ShaderRegisterBlockPc *block = &register_blocks[register_block_count++];
    PCByteCopy_candidate(block->name, name, NuStrLen(name) + 1);
    block->register_count = count;
}
i32 ShaderCompilerDictionaryPc::FindRegisterBlock(const char *name) {
    for (i32 i = 0; i < register_block_count; ++i)
        if (strcmp(register_blocks[i].name, name) == 0)
            return register_blocks[i].register_count;
    return 0;
}

ShaderMacroDefinitionPc *ShaderCompilerDictionaryPc::FindDefinition(const char *name) {
    i32 count = program_count;
    for (i32 i = 0; i < count; ++i)
        if (strcmp(programs[i]->name, name) == 0)
            return programs[i];
    return NULL;
}
void ShaderCompilerDictionaryPc::AddDefinition(const char *name, const char *value, i32 emit) {
    ShaderMacroDefinitionPc *definition = FindDefinition(name);
    if (definition != NULL) {
        const char *stored_value = NULL;
        if (value != NULL) {
            i32 bytes = (i32)strlen(value) + 1;
            char *copy = reinterpret_cast<char *>(scratch.cursor);
            scratch.cursor += bytes;
            memcpy(copy, value, bytes);
            stored_value = copy;
        }
        definition->value = stored_value;
        definition->emit = (u8)emit;
    } else {
        i32 bytes = (i32)strlen(name) + 1;
        char *stored_name = reinterpret_cast<char *>(scratch.cursor);
        scratch.cursor += bytes;
        memcpy(stored_name, name, bytes);
        const char *stored_value = NULL;
        if (value != NULL) {
            bytes = (i32)strlen(value) + 1;
            char *copy = reinterpret_cast<char *>(scratch.cursor);
            scratch.cursor += bytes;
            memcpy(copy, value, bytes);
            stored_value = copy;
        }
        definition = reinterpret_cast<ShaderMacroDefinitionPc *>(scratch.cursor);
        scratch.cursor += sizeof(ShaderMacroDefinitionPc);
        if (definition != NULL) {
            definition->name = stored_name;
            definition->value = stored_value;
            definition->emit = (u8)emit;
        }
        programs[program_count++] = definition;
    }
}
void ShaderCompilerDictionaryPc::BindOutputBuffer(ShaderCompilerBufferAllocatorPc *buffer) {
    ShaderMacroDefinitionPc **cursor = programs;
    ShaderMacroDefinitionPc **end = programs + program_count;
    while (cursor != end) {
        ShaderMacroDefinitionPc *definition = *cursor++;
        if (definition->emit != 0) {
            const char *value = definition->value != NULL ? definition->value : "";
            buffer->AppendText("#define ");
            buffer->AppendText(definition->name);
            buffer->AppendText(NuBlankStringValue);
            buffer->AppendText(value);
            buffer->AppendText("\r\n");
        }
    }
}
