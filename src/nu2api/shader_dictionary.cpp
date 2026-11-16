#include "gamelib/util/CRC16.h"
#include "gamelib/util/gamelib_util_types.h"
#include "legoapi/items/collect/bolts.h"
#include "nu2api/PCInput.h"
#include "nu2api/nu3d/ShaderManager.h"
#include "nu2api/nu3d/nushader.h"
#include "nu2api/nucore/nustring.h"
#include <stdio.h>
#include <new>
// Repeats its header, but deleting it changes ShaderCompilerDictionary::EmitAttribute (declaration noise).
extern "C" void ShaderLexicalClassesInit(void);

ShaderCompilerDictionary::ShaderCompilerDictionary(VirtualStackAllocator &parent, i32 input_mode)
    : scratch(parent, 0x3000), output_memory(parent, 0x4000), register_buffer(parent, 0x100) {
    active_output = NULL;
    ShaderLexicalClassesInit();
    mode = input_mode;
    program_count = 0;
    definition_count = 0;
    register_block_count = 0;
    emit_definitions = 1;
    output_suppressed = 0;
    unknown_966 = 0;
    profile = PC_SHADER_MODEL_3_0;
    first_version = 8;
    second_version = 2;
    force_mode = false;
    switch (mode) {
        case 1:
            emit_semantics = 0;
            emit_registers = 0;
            emit_centroid = 0;
            register_format = ": c%d";
            profile_name = "";
            break;
        default:
            emit_semantics = 1;
            emit_registers = 1;
            emit_centroid = 1;
            register_format = ": register(c%d)";
            profile_name = "_centroid";
            break;
    }
}

ShaderCompilerDictionary::~ShaderCompilerDictionary() {
}

void ShaderCompilerDictionary::SetProfile(i32 input_profile, i32 first, i32 second) {
    profile = input_profile;
    first_version = first;
    second_version = second;
    if (mode == 0) {
        if (profile == PC_SHADER_MODEL_2_0 || profile == PC_SHADER_MODEL_2_X || profile == PC_SHADER_MODEL_3_0) {
            profile_name = "_centroid";
        } else {
            profile_name = "";
        }
    }
}

extern "C" i32 fun_00746520(u32 flags) {
    return (flags >> 5) & 1;
}

extern "C" bool fun_00746530(u32 flags) {
    return (flags & 0x4020) == 0x4020;
}

extern "C" i32 fun_00746550(u32 flags) {
    return (flags >> 13) & 1;
}

extern "C" i32 fun_00746560(u32 flags) {
    return (flags >> 12) & 1;
}

PC_SHADER_MATERIAL_FILTER_VIEW::PC_SHADER_MATERIAL_FILTER_VIEW(const NUSHADERMTLDESC *descriptor, i32 flags, i32 parameter) {
    Initialize(descriptor, NULL, flags, parameter);
}

void ShaderCompilerDictionary::SetForceMode(bool force) {
    force_mode = force;
}

bool ShaderCompilerDictionary::GetForceMode() const {
    return force_mode;
}

i32 ShaderCompilerDictionary::GetProfile() const {
    return profile;
}

void ShaderCompilerDictionary::AddRegisterBlock(const char *name, i32 count) {
    ShaderRegisterBlock *block = &register_blocks[register_block_count++];
    PCByteCopy(block->name, name, NuStrLen(name) + 1);
    block->register_count = count;
}

i32 ShaderCompilerDictionary::FindRegisterBlock(const char *name) {
    ShaderRegisterBlock *cursor = register_blocks;
    ShaderRegisterBlock *end = register_blocks + register_block_count;
    for (; cursor < end; ++cursor) {
        if (strcmp(name, cursor->name) == 0) {
            return cursor->register_count;
        }
    }
    return 0;
}

ShaderMacroDefinition *ShaderCompilerDictionary::FindDefinition(const char *name) {
    i32 count = program_count;
    for (i32 i = 0; i < count; ++i) {
        if (strcmp(programs[i]->name, name) == 0) {
            return programs[i];
        }
    }
    return NULL;
}

char *ShaderCloneArenaString(const char *source, VirtualStackAllocator *arena);

// noinline: the original calls AddDefinition out of line from ShaderCompilerDictionary::EmitVertexInput; VC8 would
// inline it there.
__declspec(noinline) void ShaderCompilerDictionary::AddDefinition(const char *name, const char *value, i32 emit) {
    ShaderMacroDefinition *definition = FindDefinition(name);
    if (definition != NULL) {
        const char *stored_value;
        if (value != NULL) {
            stored_value = ShaderCloneArenaString(value, &scratch);
        } else {
            stored_value = NULL;
        }
        definition->value = stored_value;
        definition->emit = (u8)emit;
    } else {
        char *stored_name = ShaderCloneArenaString(name, &scratch);
        const char *stored_value;
        if (value != NULL) {
            stored_value = ShaderCloneArenaString(value, &scratch);
        } else {
            stored_value = NULL;
        }
        ShaderMacroDefinition *storage = reinterpret_cast<ShaderMacroDefinition *>(scratch.cursor);
        scratch.cursor += sizeof(ShaderMacroDefinition);
        if (storage != NULL) {
            storage->name = stored_name;
            storage->value = stored_value;
            storage->emit = (u8)emit;
            definition = storage;
        } else {
            definition = NULL;
        }
        programs[program_count++] = definition;
    }
}

void ShaderCompilerDictionary::BindOutputBuffer(ShaderCompilerBufferAllocator *buffer) {
    ShaderMacroDefinition **cursor = programs;
    ShaderMacroDefinition **end = programs + program_count;
    for (; cursor != end; ++cursor) {
        ShaderMacroDefinition *definition = *cursor;
        if (definition->emit != 0) {
            const char *value = definition->value != NULL ? definition->value : "";
            buffer->AppendText("#define ");
            buffer->AppendText(definition->name);
            buffer->AppendText(" ");
            buffer->AppendText(value);
            buffer->AppendText("\r\n");
        }
    }
}

bool ShaderCompilerDictionary::EvaluateCondition(ShaderConditionNode *node, i32 *result) {
    for (;;) {
        if (strcmp(node->token, "defined") == 0) {
            *result = FindDefinition(node->left->left->token) != NULL;
            return true;
        }
        if (strcmp(node->token, "\x80") != 0) {
            break;
        }
        node = node->left;
    }
    const char *token = node->token;
    if (strcmp(token, "!") == 0) {
        if (!EvaluateCondition(node->left, result)) {
            return false;
        }
        *result = *result == 0;
        return true;
    }
    i32 left;
    i32 right;
    if (strcmp(token, "&&") == 0) {
        if (!EvaluateCondition(node->left, &left) || !EvaluateCondition(node->right, &right)) {
            return false;
        }
        *result = left != 0 && right != 0;
        return true;
    }
    if (strcmp(token, "||") == 0) {
        if (EvaluateCondition(node->left, &left) && left != 0) {
            *result = left;
            return true;
        }
        if (!EvaluateCondition(node->right, &right)) {
            return false;
        }
        *result = right;
        return true;
    }
    if (strcmp(token, "==") == 0) {
        if (!EvaluateCondition(node->left, &left) || !EvaluateCondition(node->right, &right)) {
            return false;
        }
        *result = left == right;
        return true;
    }
    if (strcmp(token, "<=") == 0) {
        if (!EvaluateCondition(node->left, &left) || !EvaluateCondition(node->right, &right)) {
            return false;
        }
        *result = left <= right;
        return true;
    }
    if (strcmp(token, ">=") == 0) {
        if (!EvaluateCondition(node->left, &left) || !EvaluateCondition(node->right, &right)) {
            return false;
        }
        *result = left >= right;
        return true;
    }
    if (strcmp(token, "<") == 0) {
        if (!EvaluateCondition(node->left, &left) || !EvaluateCondition(node->right, &right)) {
            return false;
        }
        *result = left < right;
        return true;
    }
    if (strcmp(token, ">") == 0) {
        if (!EvaluateCondition(node->left, &left) || !EvaluateCondition(node->right, &right)) {
            return false;
        }
        *result = left > right;
        return true;
    }
    if (strcmp(token, "!=") == 0) {
        if (!EvaluateCondition(node->left, &left) || !EvaluateCondition(node->right, &right)) {
            return false;
        }
        *result = left != right;
        return true;
    }
    ShaderMacroDefinition *definition = FindDefinition(token);
    const char *value;
    if (definition != NULL) {
        ShaderMacroDefinition *next;
        do {
            next = FindDefinition(definition->value);
            if (next != NULL) {
                definition = next;
            }
        } while (next != NULL);
        value = definition->value;
    } else {
        // This input is reloaded after the definition lookup.
        value = reinterpret_cast<const volatile ShaderConditionNode *>(node)->token;
    }
    if (value == NULL || !IsNumericToken(value)) {
        return false;
    }
    *result = NuAToI(const_cast<char *>(value));
    return true;
}

i32 ShaderCompilerDictionary::EvaluateValue(ShaderConditionNode *node) {
    i32 value;
    return EvaluateCondition(node, &value) ? value : 0;
}

ShaderSourceNode::ShaderSourceNode(u32 input_flags, i32 input_type, i32 input_array_count, const char *input_name, i32 input_semantic,
                                   const char *input_custom_semantic, i32 input_vertex_register, i32 input_pixel_register) {
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

const char *ShaderCompilerDictionary::RenderASTName(ShaderSourceNode *node) {
    register_buffer.length = 0;
    switch ((i32)node->flags) {
        case 0x1004:
            register_buffer.AppendText("USE_");
            register_buffer << node->name;
            break;
        case 0x2002:
            register_buffer.AppendText("USE_");
            register_buffer.AppendText(node->name);
            break;
        case 0x2010:
            register_buffer.AppendText("USE_attribute_");
            register_buffer.AppendText(node->name);
            break;
        case 0x3008:
            register_buffer.AppendText("USE_");
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
    ShaderCompilerBufferAllocator *buffer = &register_buffer;
    if (buffer->length > 0 && buffer->data[buffer->length - 1] != 0) {
        if (buffer->length + 1 > buffer->capacity) {
            buffer->Resize((3 * (buffer->length + 1)) / 2);
        }
        buffer->data[buffer->length++] = 0;
    }
    return reinterpret_cast<const char *>(buffer->data);
}

ShaderSourceNode *ShaderCompilerDictionary::FindASTExact(const char *name, u32 flags) {
    i32 count = definition_count;
    for (i32 i = 0; i < count; ++i) {
        ShaderSourceNode *node = definitions[i];
        if (node->flags == flags && strcmp(node->name, name) == 0) {
            return node;
        }
    }
    return NULL;
}

ShaderSourceNode *ShaderCompilerDictionary::FindASTUniform(const char *name) {
    i32 count = definition_count;
    for (i32 i = 0; i < count; ++i) {
        ShaderSourceNode *node = definitions[i];
        if ((node->flags & 0x4020) == 0x4020 && strcmp(node->name, name) == 0) {
            return node;
        }
    }
    return NULL;
}

static const char *ShaderCompilerTypeNames[22] = {
    "undefined", "float", "float",   "float2",  "float3", "float4", "float3x3",  "float4x4",  "half",      "half",        "half2",
    "half3",     "half4", "half3x3", "half4x4", "bool",   "int",    "sampler1D", "sampler2D", "sampler3D", "samplerCUBE", "hook"};

const char *ShaderSourceNode::SemanticName() {
    switch (semantic) {
        case SHADER_SEMANTIC_CUSTOM:
            return custom_semantic;
        case SHADER_SEMANTIC_DEFAULT:
            return "DEFAULT";
        case SHADER_SEMANTIC_POSITION:
            return "POSITION";
        case SHADER_SEMANTIC_POSITION1:
            return "POSITION1";
        case SHADER_SEMANTIC_VPOS:
            return "VPOS";
        case SHADER_SEMANTIC_NORMAL:
            return "NORMAL";
        case SHADER_SEMANTIC_TANGENT:
            return "TANGENT";
        case SHADER_SEMANTIC_BINORMAL:
            return "BINORMAL";
        case SHADER_SEMANTIC_TANGENT1:
            return "TANGENT1";
        case SHADER_SEMANTIC_COLORX:
            return "COLORX";
        case SHADER_SEMANTIC_COLOR0:
            return "COLOR0";
        case SHADER_SEMANTIC_COLOR1:
            return "COLOR1";
        case SHADER_SEMANTIC_FOG:
            return "FOG";
        case SHADER_SEMANTIC_TEXCOORDX:
            return "TEXCOORDX";
        case SHADER_SEMANTIC_TEXCOORD0:
            return "TEXCOORD0";
        case SHADER_SEMANTIC_TEXCOORD1:
            return "TEXCOORD1";
        case SHADER_SEMANTIC_TEXCOORD2:
            return "TEXCOORD2";
        case SHADER_SEMANTIC_TEXCOORD3:
            return "TEXCOORD3";
        case SHADER_SEMANTIC_TEXCOORD6:
            return "TEXCOORD6";
        case SHADER_SEMANTIC_TEXCOORD7:
            return "TEXCOORD7";
        case SHADER_SEMANTIC_BLENDINDICES0:
            return "BLENDINDICES0";
        case SHADER_SEMANTIC_BLENDWEIGHT0:
            return "BLENDWEIGHT0";
        case SHADER_SEMANTIC_INDEX:
            return "INDEX";
        default:
            return "INVALID";
    }
}

void ShaderCompilerDictionary::EmitType(ShaderCompilerBufferAllocator *output) {
    for (i32 i = 0; i < definition_count; ++i) {
        ShaderSourceNode *node = definitions[i];
        if (node->flags == 0x2002 && node->expansion_state > 0) {
            const char *alias = RenderASTName(node);
            output->AppendText("#define ");
            output->AppendText(alias);
            output->AppendText("\r\n");
            AddDefinition(alias, NULL, 0);
            output->AppendText(ShaderCompilerTypeNames[node->type]);
            output->AppendText(" ");
            output->AppendText(node->name);
            output->AppendText(";\r\n");
        }
    }
}

void ShaderCompilerDictionary::EmitRegister(ShaderCompilerBufferAllocator *output) {
    for (i32 i = 0; i < definition_count; ++i) {
        ShaderSourceNode *node = definitions[i];
        if (node->flags == 0x1004 && node->expansion_state > 0) {
            const char *alias = RenderASTName(node);
            output->AppendText("#define ");
            output->AppendText(alias);
            output->AppendText("\r\n");
            AddDefinition(alias, NULL, 0);
            output->AppendText(ShaderCompilerTypeNames[node->type]);
            output->AppendText(" ");
            output->AppendText(node->name);
            if (node->semantic != SHADER_SEMANTIC_DEFAULT) {
                output->AppendText(":");
                output->AppendText(node->SemanticName());
            }
            output->AppendText(";\r\n");
        }
    }
}

void ShaderCompilerDictionary::EmitVertexInput(ShaderCompilerBufferAllocator *output) {
    output->AppendText("struct VertexInput { \r\n");
    for (i32 i = 0; i < definition_count; ++i) {
        ShaderSourceNode *node = definitions[i];
        if (node->flags == 0x2010 && node->expansion_state > 0) {
            char array_suffix[64];
            if (node->array_count >= 0) {
                sprintf(array_suffix, "[%d] ", node->array_count);
            } else {
                array_suffix[0] = 0;
            }
            const char *alias = RenderASTName(node);
            output->AppendText("\t#define ");
            output->AppendText(alias);
            output->AppendText("\r\n");
            AddDefinition(alias, NULL, 0);
            const char *type_name = ShaderCompilerTypeNames[node->type];
            output->AppendText("\t");
            output->AppendText(type_name);
            output->AppendText(" ");
            output->AppendText(node->name);
            output->AppendText(array_suffix);
            output->AppendText(": ");
            output->AppendText(node->SemanticName());
            output->AppendText(";\r\n");
        }
    }
    output->AppendText("};\r\n");
}

static const i32 ShaderCompilerTypeComponents[22] = {-1, 1, 1, 2, 3, 4, 9, 16, 1, 1, 2, 3, 4, 9, 16, 1, 1, -1, -1, -1, -1, -1};

const char *ShaderRegisterComponents(i32 first, i32 count) {
    switch (first) {
        case 0:
            if (count == 1) {
                return ".x";
            }
            if (count == 2) {
                return ".xy";
            }
            if (count == 3) {
                return ".xyz";
            }
            if (count == 4) {
                return ".xyzw";
            }
            return "????";
        case 1:
            if (count == 1) {
                return ".y";
            }
            if (count == 2) {
                return ".yz";
            }
            if (count == 3) {
                return ".yzw";
            }
            return "????";
        case 2:
            if (count == 1) {
                return ".z";
            }
            if (count == 2) {
                return ".zw";
            }
            return "????";
        case 3:
            if (count == 1) {
                return ".w";
            }
            return "????";
        default:
            return "??????";
    }
}

void ShaderCompilerDictionary::EmitAttribute(ShaderCompilerBufferAllocator *output) {
    i32 allocated = 0;
    i32 colors = 0;
    bool overflow = false;
    i32 used[16] = {0};
    const char *names[16] = {"texcoord0", "texcoord1", "texcoord2",  "texcoord3",  "texcoord4",  "texcoord5",  "texcoord6",  "texcoord7",
                             "texcoord8", "texcoord9", "texcoord10", "texcoord11", "texcoord12", "texcoord13", "texcoord14", "texcoord15"};
    output->AppendText("struct VertexOutput { \r\n");
    for (i32 i = 0; i < definition_count; ++i) {
        ShaderSourceNode *node = definitions[i];
        if (node->flags != 0x3008 || node->expansion_state <= 0) {
            continue;
        }
        const char *alias = RenderASTName(node);
        output->AppendText("\t#define ");
        output->AppendText(alias);
        output->AppendText("\r\n");
        AddDefinition(alias, NULL, 0);
        switch (node->semantic) {
            case SHADER_SEMANTIC_FOG:
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
            case SHADER_SEMANTIC_COLORX:
                if (colors < second_version) {
                    output->AppendText("\t#define ");
                    output->AppendText(node->name);
                    output->AppendText(" vout._");
                    output->AppendText(node->name);
                    output->AppendText("\r\n");
                    const char *type_name = ShaderCompilerTypeNames[node->type];
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
            case SHADER_SEMANTIC_TEXCOORDX: {
            allocate_first:
                i32 slot = -1;
                for (i32 j = 0; j < first_version; ++j) {
                    if (used[j] == 0) {
                        used[j] += ShaderCompilerTypeComponents[node->type];
                        ++allocated;
                        slot = j;
                        break;
                    }
                }
                i32 count = ShaderCompilerTypeComponents[node->type];
                output->AppendText("\t#define ");
                output->AppendText(node->name);
                output->AppendText(" vout.");
                output->AppendText(names[slot]);
                output->AppendText(ShaderRegisterComponents(used[slot] - count, count));
                output->AppendText("\r\n");
                AddDefinition(node->name, node->name, 0);
                break;
            }
            case SHADER_SEMANTIC_TEXCOORD0:
            case SHADER_SEMANTIC_TEXCOORD1: {
                i32 slot = node->semantic - 14;
                i32 count = ShaderCompilerTypeComponents[node->type];
                used[slot] += count;
                ++allocated;
                output->AppendText("\t#define ");
                output->AppendText(node->name);
                output->AppendText(" vout.");
                output->AppendText(names[slot]);
                output->AppendText(ShaderRegisterComponents(used[slot] - count, count));
                output->AppendText("\r\n");
                AddDefinition(node->name, node->name, 0);
                break;
            }
            case SHADER_SEMANTIC_POSITION:
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
            case SHADER_SEMANTIC_VPOS:
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
        ShaderSourceNode *node = definitions[i];
        if (node->flags != 0x3008 || node->expansion_state <= 0 || node->semantic != SHADER_SEMANTIC_DEFAULT) {
            continue;
        }
        i32 slot = -1;
        if (!overflow) {
            for (i32 j = 0; j < first_version; ++j) {
                if (used[j] == 0) {
                    used[j] += ShaderCompilerTypeComponents[node->type];
                    ++allocated;
                    slot = j;
                    break;
                }
            }
            overflow = slot == -1 || allocated >= 11;
        }
        if (slot == -1) {
            i32 count = ShaderCompilerTypeComponents[node->type];
            for (i32 j = 0; j < 16; ++j) {
                if (4 - used[j] >= count) {
                    used[j] += count;
                    slot = j;
                    break;
                }
            }
        }
        i32 count = ShaderCompilerTypeComponents[node->type];
        output->AppendText("\t#define ");
        output->AppendText(node->name);
        output->AppendText(" vout.");
        const char *alias = names[slot];
        output->AppendText(alias);
        output->AppendText(ShaderRegisterComponents(used[slot] - count, count));
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

ShaderCompilerBufferAllocator &ShaderCompilerBufferAllocator::operator<<(i32 value) {
    AppendInt(value);
    return *this;
}

ShaderMacroDefinition::ShaderMacroDefinition(const char *name, const char *value, u8 emit) : name(name), value(value), emit(emit) {
}

i32 ShaderSourceNode::GetComponentCount() {
    return ShaderCompilerTypeComponents[type];
}

const char *ShaderSourceNode::GetTypeName() {
    return ShaderCompilerTypeNames[type];
}

void ShaderRegisterBlock::fun_00746760(const char *block_name, i32 count) {
    PCByteCopy(name, block_name, NuStrLen(block_name) + 1);
    register_count = count;
}

void ShaderCompilerDictionary::fun_00746940(ShaderConditionNode *node, char *text) {
    if (node->left != NULL) {
        fun_00746940(node->left, text);
    }
    strcat(text, node->token);
    if (node->right != NULL) {
        fun_00746940(node->right, text);
    }
}

void ShaderCompilerDictionary::fun_00746bf0(const char *name) {
    i32 i;
    for (i = 0; i < program_count; ++i) {
        if (strcmp(programs[i]->name, name) == 0) {
            break;
        }
    }
    if (i < program_count - 1) {
        programs[i] = programs[program_count - 1];
    }
    program_count--;
}

void ShaderCompilerDictionary::AddASTDefinition(ShaderSourceNode *node) {
    i32 i;
    for (i = 0; i < definition_count; ++i) {
        if (ShaderCompilerTypeComponents[definitions[i]->type] < ShaderCompilerTypeComponents[node->type]) {
            for (i32 j = definition_count; j > i; --j) {
                definitions[j] = definitions[j - 1];
            }
            definitions[i] = node;
            ++definition_count;
            return;
        }
    }
    definitions[definition_count] = node;
    ++definition_count;
}

void ShaderCompilerDictionary::EmitProgram(ShaderCompilerBufferAllocator *output) {
    for (i32 i = 0; i < definition_count; ++i) {
        ShaderSourceNode *node = definitions[i];
        if (!(emit_centroid != 0 && (node->flags & 0x4020) == 0x4020)) {
            bool uniform = (node->flags >> 5) & 1;
            if (!uniform || node->expansion_state <= 0 || FindDefinition(node->name) != NULL) {
                continue;
            }
        }
        bool unavailable = output_stage == 0 ? (node->flags & 0x2000) == 0 : (node->flags & 0x1000) == 0;
        const char *alias = RenderASTName(node);
        output->AppendText("#define ");
        output->AppendText(alias);
        output->AppendText("\r\n");
        AddDefinition(alias, NULL, 0);
        if (node->type == 21) {
            continue;
        }
        const char *shared = !unavailable && emit_semantics != 0 && (node->flags & 0x4020) == 0x4020 ? "shared " : "";
        char array_suffix[64];
        if (node->array_count >= 0) {
            sprintf(array_suffix, "[%d] ", node->array_count);
        } else if (node->array_count == -2) {
            sprintf(array_suffix, "[%d] ", FindRegisterBlock(node->name));
        } else {
            array_suffix[0] = 0;
        }
        const char *semantic_separator;
        const char *semantic_name;
        if (emit_registers != 0) {
            semantic_separator = ": ";
            semantic_name = node->SemanticName();
        } else {
            semantic_separator = "";
            semantic_name = "";
        }
        char register_suffix[64];
        bool uniform = (node->flags >> 5) & 1;
        if (!unavailable && uniform) {
            i32 index = output_stage == 0 ? node->vertex_register : node->pixel_register;
            ShaderUniformRecord *record = NuShaderUniformGetByString(node->name);
            if (record != NULL) {
                index = record->register_index;
            }
            if (index >= 0 && (profile == PC_SHADER_MODEL_3_0 || profile == 5)) {
                sprintf(register_suffix, register_format, index);
            } else {
                register_suffix[0] = 0;
            }
        } else {
            register_suffix[0] = 0;
        }
        const char *type_name = ShaderCompilerTypeNames[node->type];
        output->AppendText(shared);
        output->AppendText("uniform ");
        output->AppendText(type_name);
        output->AppendText(" ");
        output->AppendText(node->name);
        output->AppendText(array_suffix);
        output->AppendText(semantic_separator);
        output->AppendText(semantic_name);
        output->AppendText(register_suffix);
        output->AppendText(";\r\n");
    }
}

ShaderCompilerBufferAllocator &ShaderCompilerBufferAllocator::operator<<(const char *source) {
    AppendText(source);
    return *this;
}

char *ShaderCloneArenaString(const char *source, VirtualStackAllocator *arena) {
    i32 length = (i32)strlen(source);
    char *result = reinterpret_cast<char *>(arena->cursor);
    arena->cursor += length + 1;
    memcpy(result, source, length + 1);
    return result;
}

void ShaderCompilerDictionary::Expand(void *input, ShaderCompilerBufferAllocator *output) {
    ShaderCompilerBufferAllocator *saved_output = active_output;
    active_output = output;
    ShaderExpansionNode *node = static_cast<ShaderExpansionNode *>(input);
    while (node != NULL) {
        switch (node->kind) {
            case SHADER_NODE_BRANCH: {
                i32 value;
                ShaderConditionNode *condition = node->data.branch.condition;
                if (EvaluateCondition(condition, &value) && value != 0) {
                    Expand(node->data.branch.if_true, output);
                } else if (node->data.branch.if_false != NULL) {
                    Expand(node->data.branch.if_false, output);
                }
                break;
            }
            case SHADER_NODE_MACRO:
                AddDefinition(node->data.definition.name, node->data.definition.value, 0);
                if (output_suppressed == 0 && output != NULL) {
                    const char *value = node->data.definition.value != NULL ? node->data.definition.value : "";
                    output->AppendText("#define ");
                    output->AppendText(node->data.definition.name);
                    output->AppendText(" ");
                    output->AppendText(value);
                    output->AppendText("\r\n");
                }
                break;
            case SHADER_NODE_UNDEF:
                if (output != NULL) {
                    output->AppendText("#undef ");
                    output->AppendText(node->data.definition.name);
                    output->AppendText("\r\n");
                }
                break;
            case SHADER_NODE_DECLARATION:
                if (FindASTExact(node->data.declaration.name, node->data.declaration.flags) == NULL) {
                    VirtualStackAllocator *arena = &output_memory;
                    const char *name = ShaderCloneArenaString(node->data.declaration.name, arena);
                    const char *custom = node->data.declaration.custom_semantic != NULL
                                             ? ShaderCloneArenaString(node->data.declaration.custom_semantic, arena)
                                             : NULL;
                    ShaderSourceNode *storage = reinterpret_cast<ShaderSourceNode *>(arena->cursor);
                    arena->cursor += sizeof(ShaderSourceNode);
                    AddASTDefinition(new (storage)
                                         ShaderSourceNode(node->data.declaration.flags, node->data.declaration.type,
                                                          node->data.declaration.array_count, name, node->data.declaration.semantic, custom,
                                                          node->data.declaration.vertex_register, node->data.declaration.pixel_register));
                }
                break;
            case SHADER_NODE_REFERENCE: {
                const char *name = node->data.reference.name;
                for (i32 i = 0; i < node->data.reference.alias_depth; ++i) {
                    name = FindDefinition(name)->value;
                }
                ShaderSourceNode *definition = FindASTExact(name, node->data.reference.flags);
                bool uniform = (node->data.reference.flags >> 5) & 1;
                if (definition == NULL && uniform) {
                    definition = FindASTUniform(name);
                }
                if (definition != NULL) {
                    ++definition->expansion_state;
                    const char *rendered = RenderASTName(definition);
                    AddDefinition(rendered, NULL, 0);
                }
                break;
            }
            case SHADER_NODE_TOGGLE:
                output_suppressed = node->data.toggle.enabled == 0;
                break;
            case SHADER_NODE_EMIT:
                if (output != NULL) {
                    u32 flags = node->data.emit.flags;
                    bool program = (flags >> 5) & 1;
                    if (program) {
                        EmitProgram(output);
                    } else if (flags == 0x2010) {
                        EmitVertexInput(output);
                    } else if (flags == 0x3008) {
                        EmitAttribute(output);
                    } else if (flags == 0x2002) {
                        EmitType(output);
                    } else if (flags == 0x1004) {
                        EmitRegister(output);
                    }
                }
                break;
            case SHADER_NODE_TEXT:
                if (output != NULL) {
                    output->AppendBytes(node->data.text.bytes, node->data.text.count);
                }
                break;
        }
        node = node->next;
    }
    active_output = saved_output;
}
