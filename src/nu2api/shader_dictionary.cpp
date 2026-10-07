#include "saga.h"
#include <stdio.h>
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

bool ShaderCompilerDictionaryPc::EvaluateCondition(ShaderConditionNodePc *node, i32 *result) {
    while (strcmp(node->token, "defined") != 0 && strcmp(node->token, "\x80") == 0)
        node = node->left;
    if (strcmp(node->token, "defined") == 0) {
        *result = FindDefinition(node->left->left->token) != NULL;
        return true;
    }
    const char *token = node->token;
    if (strcmp(token, "!") == 0) {
        if (!EvaluateCondition(node->left, result))
            return false;
        *result = *result == 0;
        return true;
    }
    i32 left, right;
    if (strcmp(token, "&&") == 0) {
        if (!EvaluateCondition(node->left, &left) || !EvaluateCondition(node->right, &right))
            return false;
        *result = left != 0 && right != 0;
        return true;
    }
    if (strcmp(token, "||") == 0) {
        if (EvaluateCondition(node->left, &left) && left != 0) {
            *result = left;
            return true;
        }
        if (!EvaluateCondition(node->right, &right))
            return false;
        *result = right;
        return true;
    }
    if (strcmp(token, "==") == 0 || strcmp(token, "<=") == 0 || strcmp(token, ">=") == 0 || strcmp(token, "<") == 0 ||
        strcmp(token, ">") == 0 || strcmp(token, "!=") == 0) {
        if (!EvaluateCondition(node->left, &left) || !EvaluateCondition(node->right, &right))
            return false;
        if (strcmp(token, "==") == 0)
            *result = left == right;
        else if (strcmp(token, "<=") == 0)
            *result = left <= right;
        else if (strcmp(token, ">=") == 0)
            *result = left >= right;
        else if (strcmp(token, "<") == 0)
            *result = left < right;
        else if (strcmp(token, ">") == 0)
            *result = left > right;
        else
            *result = left != right;
        return true;
    }
    ShaderMacroDefinitionPc *definition = FindDefinition(token);
    const char *value;
    if (definition != NULL) {
        for (;;) {
            ShaderMacroDefinitionPc *next = FindDefinition(definition->value);
            if (next == NULL)
                break;
            definition = next;
        }
        value = definition->value;
    } else {
        value = node->token;
    }
    if (value == NULL || !IsNumericToken(value))
        return false;
    *result = NuAToI(const_cast<char *>(value));
    return true;
}

i32 ShaderCompilerDictionaryPc::EvaluateValue(ShaderConditionNodePc *node) {
    i32 value;
    return EvaluateCondition(node, &value) ? value : 0;
}

ShaderSourceNodePc::ShaderSourceNodePc(u32 input_flags, i32 input_type, i32 input_array_count, const char *input_name,
                                       i32 input_semantic, const char *input_custom_semantic, i32 input_vertex_register,
                                       i32 input_pixel_register) {
    flags = input_flags;
    type = input_type;
    array_count = input_array_count;
    name = input_name;
    semantic = input_semantic;
    custom_semantic = input_custom_semantic;
    vertex_register = input_vertex_register;
    pixel_register = input_pixel_register;
    expansion_state = 0;
}
const char *ShaderCompilerDictionaryPc::RenderASTName(ShaderSourceNodePc *node) {
    register_buffer.length = 0;
    switch (node->flags) {
        case 0x1004:
            register_buffer.AppendText("USE_");
            register_buffer << node->name;
            break;
        case 0x2002:
        case 0x3008:
            register_buffer.AppendText("USE_");
            register_buffer.AppendText(node->name);
            break;
        case 0x2010:
            register_buffer.AppendText("USE_attribute_");
            register_buffer.AppendText(node->name);
            break;
        case 0x1020:
        case 0x2020:
        case 0x3020:
        case 0x5020:
        case 0x6020:
        case 0x7020:
            register_buffer.AppendText("USE_uniform_");
            register_buffer.AppendText(node->name);
            break;
    }
    if (register_buffer.length > 0 && register_buffer.data[register_buffer.length - 1] != 0) {
        i32 required = register_buffer.length + 1;
        if (required > register_buffer.capacity)
            register_buffer.Resize((3 * required) / 2);
        register_buffer.data[register_buffer.length++] = 0;
    }
    return reinterpret_cast<const char *>(register_buffer.data);
}
ShaderSourceNodePc *ShaderCompilerDictionaryPc::FindASTExact(const char *name, u32 flags) {
    i32 count = definition_count;
    for (i32 i = 0; i < count; ++i) {
        ShaderSourceNodePc *node = definitions[i];
        if (node->flags == flags && strcmp(node->name, name) == 0)
            return node;
    }
    return NULL;
}
ShaderSourceNodePc *ShaderCompilerDictionaryPc::FindASTUniform(const char *name) {
    i32 count = definition_count;
    for (i32 i = 0; i < count; ++i) {
        ShaderSourceNodePc *node = definitions[i];
        if ((node->flags & 0x4020) == 0x4020 && strcmp(node->name, name) == 0)
            return node;
    }
    return NULL;
}

// Native type-name table minimum22 entries; original declaration visibility remains qualified.
static const char *ShaderCompilerTypeNamesPc[22] = {
    "undefined", "float",     "float",     "float2",    "float3",      "float4",  "float3x3", "float4x4",
    "half",      "half",      "half2",     "half3",     "half4",       "half3x3", "half4x4",  "bool",
    "int",       "sampler1D", "sampler2D", "sampler3D", "samplerCUBE", "hook"};
const char *ShaderSourceNodePc::SemanticName() {
    switch (semantic) {
        case 0:
            return custom_semantic;
        case 1:
            return "DEFAULT";
        case 2:
            return "POSITION";
        case 3:
            return "POSITION1";
        case 4:
            return "VPOS";
        case 5:
            return "NORMAL";
        case 6:
            return "TANGENT";
        case 7:
            return "BINORMAL";
        case 8:
            return "TANGENT1";
        case 9:
            return "COLORX";
        case 10:
            return "COLOR0";
        case 11:
            return "COLOR1";
        case 12:
            return "FOG";
        case 13:
            return "TEXCOORDX";
        case 14:
            return "TEXCOORD0";
        case 15:
            return "TEXCOORD1";
        case 16:
            return "TEXCOORD2";
        case 17:
            return "TEXCOORD3";
        case 20:
            return "TEXCOORD6";
        case 21:
            return "TEXCOORD7";
        case 22:
            return "BLENDINDICES0";
        case 23:
            return "BLENDWEIGHT0";
        case 24:
            return "INDEX";
        default:
            return "INVALID";
    }
}
void ShaderCompilerDictionaryPc::EmitType(ShaderCompilerBufferAllocatorPc *output) {
    for (i32 i = 0; i < definition_count; ++i) {
        ShaderSourceNodePc *node = definitions[i];
        if (node->flags == 0x2002 && node->expansion_state > 0) {
            const char *alias = RenderASTName(node);
            output->AppendText("#define ");
            output->AppendText(alias);
            output->AppendText("\r\n");
            AddDefinition(alias, NULL, 0);
            output->AppendText(ShaderCompilerTypeNamesPc[node->type]);
            output->AppendText(NuBlankStringValue);
            output->AppendText(node->name);
            output->AppendText(";\r\n");
        }
    }
}
void ShaderCompilerDictionaryPc::EmitRegister(ShaderCompilerBufferAllocatorPc *output) {
    for (i32 i = 0; i < definition_count; ++i) {
        ShaderSourceNodePc *node = definitions[i];
        if (node->flags == 0x1004 && node->expansion_state > 0) {
            const char *alias = RenderASTName(node);
            output->AppendText("#define ");
            output->AppendText(alias);
            output->AppendText("\r\n");
            AddDefinition(alias, NULL, 0);
            output->AppendText(ShaderCompilerTypeNamesPc[node->type]);
            output->AppendText(NuBlankStringValue);
            output->AppendText(node->name);
            if (node->semantic != 1) {
                output->AppendText(":");
                output->AppendText(node->SemanticName());
            }
            output->AppendText(";\r\n");
        }
    }
}
void ShaderCompilerDictionaryPc::EmitVertexInput(ShaderCompilerBufferAllocatorPc *output) {
    output->AppendText("struct VertexInput { \r\n");
    for (i32 i = 0; i < definition_count; ++i) {
        ShaderSourceNodePc *node = definitions[i];
        if (node->flags == 0x2010 && node->expansion_state > 0) {
            char array_suffix[64];
            if (node->array_count >= 0)
                sprintf(array_suffix, "[%d] ", node->array_count);
            else
                array_suffix[0] = 0;
            const char *alias = RenderASTName(node);
            output->AppendText("\t#define ");
            output->AppendText(alias);
            output->AppendText("\r\n");
            AddDefinition(alias, NULL, 0);
            const char *type_name = ShaderCompilerTypeNamesPc[node->type];
            output->AppendText("\t");
            output->AppendText(type_name);
            output->AppendText(NuBlankStringValue);
            output->AppendText(node->name);
            output->AppendText(array_suffix);
            output->AppendText(": ");
            output->AppendText(node->SemanticName());
            output->AppendText(";\r\n");
        }
    }
    output->AppendText("};\r\n");
}

static const i32 ShaderCompilerTypeComponentsPc[22] = {-1, 1, 1, 2,  3, 4, 9,  16, 1,  1,  2,
                                                       3,  4, 9, 16, 1, 1, -1, -1, -1, -1, -1};
static const char *ShaderRegisterComponentsPc(i32 first, i32 count) {
    switch (first) {
        case 0:
            switch (count) {
                case 1:
                    return ".x";
                case 2:
                    return ".xy";
                case 3:
                    return ".xyz";
                case 4:
                    return ".xyzw";
                default:
                    return "????";
            }
        case 1:
            switch (count) {
                case 1:
                    return ".y";
                case 2:
                    return ".yz";
                case 3:
                    return ".yzw";
                default:
                    return "????";
            }
        case 2:
            switch (count) {
                case 1:
                    return ".z";
                case 2:
                    return ".zw";
                default:
                    return "????";
            }
        case 3:
            return count == 1 ? ".w" : "????";
        default:
            return "??????";
    }
}
void ShaderCompilerDictionaryPc::EmitAttribute(ShaderCompilerBufferAllocatorPc *output) {
    i32 allocated = 0;
    i32 colors = 0;
    bool overflow = false;
    i32 used[16] = {0};
    const char *names[16] = {"texcoord0",  "texcoord1",  "texcoord2",  "texcoord3", "texcoord4",  "texcoord5",
                             "texcoord6",  "texcoord7",  "texcoord8",  "texcoord9", "texcoord10", "texcoord11",
                             "texcoord12", "texcoord13", "texcoord14", "texcoord15"};
    output->AppendText("struct VertexOutput { \r\n");
    for (i32 i = 0; i < definition_count; ++i) {
        ShaderSourceNodePc *node = definitions[i];
        if (node->flags != 0x3008 || node->expansion_state <= 0)
            continue;
        const char *alias = RenderASTName(node);
        output->AppendText("\t#define ");
        output->AppendText(alias);
        output->AppendText("\r\n");
        AddDefinition(alias, NULL, 0);
        switch (node->semantic) {
            case 12:
                output->AppendText("\t#define ");
                output->AppendText(node->name);
                output->AppendText(" vout._");
                output->AppendText(node->name);
                output->AppendText("\r\n");
                output->AppendText("\tfloat _");
                output->AppendText(node->name);
                output->AppendText(": FOG;\r\n");
                AddDefinition(node->name, node->name, 0);
                ++allocated;
                break;
            case 9:
                if (colors < second_version) {
                    output->AppendText("\t#define ");
                    output->AppendText(node->name);
                    output->AppendText(" vout._");
                    output->AppendText(node->name);
                    output->AppendText("\r\n");
                    const char *type_name = ShaderCompilerTypeNamesPc[node->type];
                    output->AppendText("\t");
                    output->AppendText(type_name);
                    output->AppendText(" _");
                    output->AppendText(node->name);
                    output->AppendText(": COLOR");
                    output->AppendInt(colors++);
                    output->AppendText(";\r\n");
                    AddDefinition(node->name, node->name, 0);
                } else {
                    goto allocate_first;
                }
                break;
            case 13: {
            allocate_first:
                i32 slot = -1;
                for (i32 j = 0; j < first_version; ++j) {
                    if (used[j] == 0) {
                        used[j] += ShaderCompilerTypeComponentsPc[node->type];
                        ++allocated;
                        slot = j;
                        break;
                    }
                }
                i32 count = ShaderCompilerTypeComponentsPc[node->type];
                output->AppendText("\t#define ");
                output->AppendText(node->name);
                output->AppendText(" vout.");
                output->AppendText(names[slot]);
                output->AppendText(ShaderRegisterComponentsPc(used[slot] - count, count));
                output->AppendText("\r\n");
                AddDefinition(node->name, node->name, 0);
                break;
            }
            case 14:
            case 15: {
                i32 slot = node->semantic - 14;
                i32 count = ShaderCompilerTypeComponentsPc[node->type];
                used[slot] += count;
                ++allocated;
                output->AppendText("\t#define ");
                output->AppendText(node->name);
                output->AppendText(" vout.");
                output->AppendText(names[slot]);
                output->AppendText(ShaderRegisterComponentsPc(used[slot] - count, count));
                output->AppendText("\r\n");
                AddDefinition(node->name, node->name, 0);
                break;
            }
            case 2:
                output->AppendText("\t#define ");
                output->AppendText(node->name);
                output->AppendText(" vout._");
                output->AppendText(node->name);
                output->AppendText("\r\n");
                output->AppendText("\tfloat4 _");
                output->AppendText(node->name);
                output->AppendText(": POSITION;\r\n");
                AddDefinition(node->name, node->name, 0);
                break;
            case 4:
                output->AppendText("\t#define ");
                output->AppendText(node->name);
                output->AppendText(" vout._");
                output->AppendText(node->name);
                output->AppendText("\r\n");
                output->AppendText("\tfloat4 _");
                output->AppendText(node->name);
                output->AppendText(": VPOS;\r\n");
                AddDefinition(node->name, node->name, 0);
                break;
        }
    }
    for (i32 i = 0; i < definition_count; ++i) {
        ShaderSourceNodePc *node = definitions[i];
        if (node->flags != 0x3008 || node->expansion_state <= 0 || node->semantic != 1)
            continue;
        i32 slot = -1;
        if (!overflow) {
            for (i32 j = 0; j < first_version; ++j) {
                if (used[j] == 0) {
                    used[j] += ShaderCompilerTypeComponentsPc[node->type];
                    ++allocated;
                    slot = j;
                    break;
                }
            }
            overflow = slot == -1 || allocated >= 11;
        }
        if (slot == -1) {
            i32 count = ShaderCompilerTypeComponentsPc[node->type];
            for (i32 j = 0; j < 16; ++j) {
                if (4 - used[j] >= count) {
                    used[j] += count;
                    slot = j;
                    break;
                }
            }
        }
        i32 count = ShaderCompilerTypeComponentsPc[node->type];
        output->AppendText("\t#define ");
        output->AppendText(node->name);
        output->AppendText(" vout.");
        const char *alias = names[slot];
        output->AppendText(alias);
        output->AppendText(ShaderRegisterComponentsPc(used[slot] - count, count));
        output->AppendText("\r\n");
        AddDefinition(node->name, alias, 0);
    }
    const char *type_names[4] = {"float1", "float2", "float3", "float4"};
    for (i32 i = 0; i < 16; ++i) {
        i32 count = used[i];
        if (count > 0) {
            output->AppendText("\t");
            output->AppendText(type_names[count - 1]);
            output->AppendText(" texcoord");
            output->AppendInt(i);
            output->AppendText(": TEXCOORD");
            output->AppendInt(i);
            output->AppendText(profile_name);
            output->AppendText(";\r\n");
        }
    }
    output->AppendText("};\r\n");
}
void ShaderCompilerDictionaryPc::AddASTDefinition(ShaderSourceNodePc *node) {
    i32 count = definition_count;
    i32 insertion = count;
    for (i32 i = 0; i < count; ++i) {
        if (ShaderCompilerTypeComponentsPc[definitions[i]->type] < ShaderCompilerTypeComponentsPc[node->type]) {
            insertion = i;
            break;
        }
    }
    for (i32 i = count; i > insertion; --i)
        definitions[i] = definitions[i - 1];
    definitions[insertion] = node;
    ++definition_count;
}
