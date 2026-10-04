#include "saga.h"

void __cdecl NuTexAnimAddList(void *animation) {
    PC_TEXTURE_ANIM_LIST *node;
    if (animation == NULL)
        return;
    PCThreadCriticalSectionEnter_candidate(g_texAnimCriticalSection);
    node = ntal_free;
    if (node != NULL) {
        if (node->next == NULL)
            NuSevereWarning(NuTextureAnimationSlotsWarning);
        ntal_free = ntal_free->next;
        node->next = ntal_first;
        node->previous = NULL;
        node->first = animation;
        if (ntal_first != NULL)
            ntal_first->previous = node;
        ntal_first = node;
    }
    PCThreadCriticalSectionLeave_candidate(g_texAnimCriticalSection);
}

void __cdecl NuTexAnimRemoveList(void *animation) {
    PC_TEXTURE_ANIM_LIST *node;
    PCThreadCriticalSectionEnter_candidate(g_texAnimCriticalSection);
    for (node = ntal_first; node != NULL; node = node->next) {
        if (node->first == animation) {
            if (node->next != NULL)
                node->next->previous = node->previous;
            if (node->previous != NULL)
                node->previous->next = node->next;
            else
                ntal_first = node->next;
            node->next = ntal_free;
            ntal_free = node;
            NuThreadEnableThreadSwap();
            break;
        }
    }
    PCThreadCriticalSectionLeave_candidate(g_texAnimCriticalSection);
}

PC_TEXTURE_ANIM_ENV *__cdecl NuTexAnimEnvCreate(VARIPTR *buffer, NUMTL *material, u16 *texture_ids, void *program) {
    PC_TEXTURE_ANIM_ENV *environment;
    if (buffer != NULL) {
        environment = (PC_TEXTURE_ANIM_ENV *)((buffer->addr + 3) & ~3u);
        buffer->void_ptr = environment + 1;
    } else {
        environment = (PC_TEXTURE_ANIM_ENV *)NuAlloc(sizeof(PC_TEXTURE_ANIM_ENV), NuTextureAnimationSourcePathPC, 548);
    }
    if (environment != NULL) {
        environment->program = program;
        environment->instruction_index = 0;
        environment->loop_depth = 0;
        environment->call_depth = 0;
        environment->wait_base = 0;
        environment->wait_random = 0;
        environment->wait_remaining = 0;
        environment->material = material;
        environment->texture_ids = texture_ids;
        environment->texture_index = 0;
        if (buffer != NULL)
            environment->flags &= ~1u;
        else
            environment->flags |= 1;
    }
    return environment;
}

#include <string.h>
typedef struct TEXTURE_ANIM_PROGRAM_PC TEXTURE_ANIM_PROGRAM_PC;
struct TEXTURE_ANIM_PROGRAM_PC {
    TEXTURE_ANIM_PROGRAM_PC *next, *previous;
    char name[32];
    i32 on_signal[32], off_signal[32];
    u32 on_mask, off_mask;
    i16 label_ids[32], label_offsets[32];
    i32 label_count;
    i16 instruction_count;
    u16 flags;
    u16 mask;
    i16 instructions[1];
};
STATIC_ASSERT(sizeof(TEXTURE_ANIM_PROGRAM_PC) == 0x1bc);
STATIC_ASSERT(offsetof(TEXTURE_ANIM_PROGRAM_PC, next) == 0);
STATIC_ASSERT(offsetof(TEXTURE_ANIM_PROGRAM_PC, previous) == 4);
STATIC_ASSERT(offsetof(TEXTURE_ANIM_PROGRAM_PC, name) == 8);
STATIC_ASSERT(offsetof(TEXTURE_ANIM_PROGRAM_PC, on_signal) == 0x28);
STATIC_ASSERT(offsetof(TEXTURE_ANIM_PROGRAM_PC, off_signal) == 0xa8);
STATIC_ASSERT(offsetof(TEXTURE_ANIM_PROGRAM_PC, on_mask) == 0x128);
STATIC_ASSERT(offsetof(TEXTURE_ANIM_PROGRAM_PC, off_mask) == 0x12c);
STATIC_ASSERT(offsetof(TEXTURE_ANIM_PROGRAM_PC, label_ids) == 0x130);
STATIC_ASSERT(offsetof(TEXTURE_ANIM_PROGRAM_PC, label_offsets) == 0x170);
STATIC_ASSERT(offsetof(TEXTURE_ANIM_PROGRAM_PC, label_count) == 0x1b0);
STATIC_ASSERT(offsetof(TEXTURE_ANIM_PROGRAM_PC, instruction_count) == 0x1b4);
STATIC_ASSERT(offsetof(TEXTURE_ANIM_PROGRAM_PC, flags) == 0x1b6);
STATIC_ASSERT(offsetof(TEXTURE_ANIM_PROGRAM_PC, mask) == 0x1b8);
STATIC_ASSERT(offsetof(TEXTURE_ANIM_PROGRAM_PC, instructions) == 0x1ba);
typedef struct TEXTURE_ANIMATION_PC TEXTURE_ANIMATION_PC;
struct TEXTURE_ANIMATION_PC {
    TEXTURE_ANIMATION_PC *next, *previous;
    u16 *texture_ids;
    i16 texture_count;
    u8 flags, reserved_0f;
    NUMTL *material;
    PC_TEXTURE_ANIM_ENV *environment;
    char *name, *program_name;
};
STATIC_ASSERT(sizeof(TEXTURE_ANIMATION_PC) == 0x20);
STATIC_ASSERT(offsetof(TEXTURE_ANIMATION_PC, next) == 0);
STATIC_ASSERT(offsetof(TEXTURE_ANIMATION_PC, previous) == 4);
STATIC_ASSERT(offsetof(TEXTURE_ANIMATION_PC, texture_ids) == 8);
STATIC_ASSERT(offsetof(TEXTURE_ANIMATION_PC, texture_count) == 0xc);
STATIC_ASSERT(offsetof(TEXTURE_ANIMATION_PC, flags) == 0xe);
STATIC_ASSERT(offsetof(TEXTURE_ANIMATION_PC, material) == 0x10);
STATIC_ASSERT(offsetof(TEXTURE_ANIMATION_PC, environment) == 0x14);
STATIC_ASSERT(offsetof(TEXTURE_ANIMATION_PC, name) == 0x18);
typedef struct TEXTURE_ANIM_MATERIAL_PC {
    u8 unknown_00[0x74];
    i16 texture_id;
    u8 unknown_76[0x42];
    i32 diffuse_texture_id;
} TEXTURE_ANIM_MATERIAL_PC;
STATIC_ASSERT(offsetof(TEXTURE_ANIM_MATERIAL_PC, texture_id) == 0x74);
STATIC_ASSERT(offsetof(TEXTURE_ANIM_MATERIAL_PC, diffuse_texture_id) == 0xb8);

static u32 TextureAnimationSignalsOffPc, TextureAnimationSignalsOnPc;
static i32 TextureAnimationFrameTimePc;
static u16 TextureAnimationActiveMaskPc;
extern NURAND TextureAnimationRandomPc;
static u32 TextureAnimationPreviousSignalsPc;
extern u16 TextureAnimationScriptMaskPc;

void __cdecl NuTexAnimSetSignals(u32 signals) {
    u32 changed = TextureAnimationPreviousSignalsPc ^ signals;
    TextureAnimationSignalsOnPc = changed & signals;
    TextureAnimationSignalsOffPc = changed & ~signals;
    TextureAnimationPreviousSignalsPc = signals;
}

void __cdecl NuTexAnimSetMask(i32 mask) {
    TextureAnimationScriptMaskPc = (u16)mask;
}

static i32 TextureAnimationConditionPrivatePc(i32 condition, i32 texture, i32 value) {
    switch (condition) {
        case 0:
            return texture == value;
        case 1:
            return texture < value;
        case 2:
            return texture > value;
        case 3:
            return texture <= value;
        case 4:
            return texture >= value;
        case 5:
            return texture != value;
        default:
            return 0;
    }
}

static void TextureAnimationCrossReferencePrivatePc(i32 label, PC_TEXTURE_ANIM_ENV *current) {
    PC_TEXTURE_ANIM_LIST *list;
    for (list = ntal_first; list != NULL; list = list->next) {
        TEXTURE_ANIMATION_PC *animation;
        for (animation = (TEXTURE_ANIMATION_PC *)list->first; animation != NULL; animation = animation->next) {
            PC_TEXTURE_ANIM_ENV *other = animation->environment;
            TEXTURE_ANIM_PROGRAM_PC *program;
            i32 i;
            if (other == NULL || other == current || other->program == NULL)
                continue;
            program = (TEXTURE_ANIM_PROGRAM_PC *)other->program;
            for (i = 0; i < program->label_count; ++i) {
                if (program->label_ids[i] == label) {
                    other->instruction_index = program->label_offsets[i];
                    other->wait_remaining = 0;
                    other->call_depth = 0;
                    other->loop_depth = 0;
                    break;
                }
            }
        }
    }
}

void __cdecl NuTexAnimEnvProc(PC_TEXTURE_ANIM_ENV *environment) {
    TEXTURE_ANIM_PROGRAM_PC *program = (TEXTURE_ANIM_PROGRAM_PC *)environment->program;
    TEXTURE_ANIM_MATERIAL_PC *material;
    u32 signals;
    i32 i;
    if (program == NULL || (program->mask & TextureAnimationActiveMaskPc) == 0)
        return;
    material = (TEXTURE_ANIM_MATERIAL_PC *)environment->material;
    signals = TextureAnimationSignalsOffPc & program->off_mask;
    if (signals != 0) {
        for (i = 0; i < 32; ++i) {
            if ((signals & (1U << i)) != 0) {
                environment->wait_remaining = 0;
                environment->instruction_index = program->off_signal[i];
                environment->call_depth = 0;
                environment->loop_depth = 0;
                break;
            }
        }
    }
    signals = TextureAnimationSignalsOnPc & program->on_mask;
    if (signals != 0) {
        for (i = 0; i < 32; ++i) {
            if ((signals & (1U << i)) != 0) {
                environment->wait_remaining = 0;
                environment->instruction_index = program->on_signal[i];
                environment->call_depth = 0;
                environment->loop_depth = 0;
                break;
            }
        }
    }
    if (environment->wait_remaining != 0)
        environment->wait_remaining -= TextureAnimationFrameTimePc;
    if (environment->wait_remaining > 0)
        return;
    for (;;) {
        i16 *instruction = program->instructions + environment->instruction_index;
        switch (instruction[0]) {
            case 0:
                environment->texture_index = instruction[1];
                environment->instruction_index += 2;
                break;
            case 1:
                environment->texture_index = NuRand(&TextureAnimationRandomPc) % instruction[1];
                environment->instruction_index += 2;
                break;
            case 2: {
                i32 texture = environment->texture_index + instruction[1];
                if (texture < instruction[2])
                    texture = instruction[2];
                if (texture > instruction[3])
                    texture = instruction[3];
                environment->texture_index = texture;
                environment->instruction_index += 4;
                break;
            }
            case 3: {
                i32 texture = NuRand(&TextureAnimationRandomPc) % (instruction[2] - instruction[1] + 1);
                texture += instruction[1] + environment->texture_index;
                if (texture < instruction[3])
                    texture = instruction[3];
                if (texture > instruction[4])
                    texture = instruction[4];
                environment->texture_index = texture;
                environment->instruction_index += 5;
                break;
            }
            case 4:
                environment->wait_remaining += instruction[1];
                if (instruction[2] != 0)
                    environment->wait_remaining += NuRand(&TextureAnimationRandomPc) % instruction[2];
                environment->instruction_index += 3;
                if (environment->wait_remaining < 0)
                    environment->wait_remaining = 0;
                return;
            case 7:
                environment->wait_base = instruction[1];
                environment->wait_random = instruction[2];
                environment->instruction_index += 3;
                continue;
            case 9:
                environment->instruction_index = instruction[1];
                continue;
            case 10:
                if (environment->call_depth >= 16)
                    ((void(__cdecl *)(i32))AnimationDiagnosticNoOpPc)(environment->instruction_index);
                environment->return_stack[environment->call_depth++] = environment->instruction_index + 2;
                environment->instruction_index = instruction[1];
                continue;
            case 11:
                if (TextureAnimationConditionPrivatePc(instruction[1], environment->texture_index, instruction[2]))
                    environment->instruction_index = instruction[3];
                else
                    environment->instruction_index += 4;
                continue;
            case 12:
                if (environment->call_depth == 0)
                    ((void(__cdecl *)(i32))AnimationDiagnosticNoOpPc)(environment->instruction_index);
                environment->instruction_index = environment->return_stack[--environment->call_depth];
                continue;
            case 13:
                if (environment->loop_depth >= 16)
                    ((void(__cdecl *)(i32))AnimationDiagnosticNoOpPc)(environment->instruction_index);
                environment->loop_counts[environment->loop_depth] = instruction[1];
                if (instruction[2] != 0)
                    environment->loop_counts[environment->loop_depth] +=
                        NuRand(&TextureAnimationRandomPc) % instruction[2];
                environment->instruction_index += 3;
                environment->loop_starts[environment->loop_depth++] = environment->instruction_index;
                continue;
            case 14:
                if (environment->loop_depth == 0)
                    ((void(__cdecl *)(i32))AnimationDiagnosticNoOpPc)(environment->instruction_index);
                if (environment->loop_counts[environment->loop_depth - 1] == 0) {
                    --environment->loop_depth;
                    ++environment->instruction_index;
                } else {
                    environment->instruction_index = environment->loop_starts[environment->loop_depth - 1];
                    --environment->loop_counts[environment->loop_depth - 1];
                }
                continue;
            case 15:
                if (environment->loop_depth == 0)
                    ((void(__cdecl *)(i32))AnimationDiagnosticNoOpPc)(environment->instruction_index);
                if (TextureAnimationConditionPrivatePc(instruction[1], environment->texture_index, instruction[2]) ||
                    environment->loop_counts[environment->loop_depth - 1] == 0) {
                    --environment->loop_depth;
                    environment->instruction_index += 3;
                } else {
                    --environment->loop_counts[environment->loop_depth - 1];
                    environment->instruction_index = environment->loop_starts[environment->loop_depth - 1];
                }
                continue;
            case 16:
                if (environment->wait_remaining < 0)
                    environment->wait_remaining = 0;
                return;
            case 17:
                TextureAnimationCrossReferencePrivatePc(instruction[1], environment);
                environment->instruction_index += 2;
                continue;
            default:
                continue;
        }
        material->texture_id = environment->texture_ids[environment->texture_index];
        material->diffuse_texture_id = environment->texture_ids[environment->texture_index] & 0x7fff;
        environment->wait_remaining += environment->wait_base;
        if (environment->wait_random != 0)
            environment->wait_remaining += NuRand(&TextureAnimationRandomPc) % environment->wait_random;
        if (environment->wait_remaining < 0)
            environment->wait_remaining = 0;
        return;
    }
}

void __cdecl NuTexAnimProcessList(TEXTURE_ANIMATION_PC *animation) {
    while (animation != NULL) {
        if (animation->environment != NULL)
            NuTexAnimEnvProc(animation->environment);
        animation = animation->next;
    }
}

void __cdecl NuTexAnimResetList(TEXTURE_ANIMATION_PC *animation) {
    while (animation != NULL) {
        PC_TEXTURE_ANIM_ENV *environment = animation->environment;
        if (environment != NULL) {
            environment->instruction_index = 0;
            environment->loop_depth = 0;
            environment->call_depth = 0;
            environment->wait_base = 0;
            environment->wait_random = 0;
            environment->wait_remaining = 0;
            environment->texture_index = 0;
        }
        animation = animation->next;
    }
}

void __cdecl NuTexAnimInitMutex(void) {
    g_texAnimCriticalSection = PCThreadCriticalSectionCreate_candidate();
}

void __cdecl NuTexAnimProcess(f32 frame_time) {
    PC_TEXTURE_ANIM_LIST *list;
    PCThreadCriticalSectionEnter_candidate(g_texAnimCriticalSection);
    TextureAnimationFrameTimePc = (i32)(frame_time * 4096.0);
    TextureAnimationActiveMaskPc = TextureAnimationScriptMaskPc;
    for (list = ntal_first; list != NULL; list = list->next) {
        TEXTURE_ANIMATION_PC *animation = (TEXTURE_ANIMATION_PC *)list->first;
        while (animation != NULL) {
            if (animation->environment != NULL)
                NuTexAnimEnvProc(animation->environment);
            animation = animation->next;
        }
    }
    PCThreadCriticalSectionLeave_candidate(g_texAnimCriticalSection);
}

void __cdecl NuTexAnimRestart(void) {
    PC_TEXTURE_ANIM_LIST *list;
    for (list = ntal_first; list != NULL; list = list->next)
        NuTexAnimResetList((TEXTURE_ANIMATION_PC *)list->first);
}

TEXTURE_ANIMATION_PC *__cdecl NuTexAnimFind(TEXTURE_ANIMATION_PC *animation, const char *name) {
    while (animation != NULL) {
        if (NuStrICmp(animation->name, name) == 0)
            return animation;
        animation = animation->next;
    }
    return NULL;
}

void __cdecl NuTexAnimDestroy(TEXTURE_ANIMATION_PC *animation) {
    if (animation->environment != NULL && (animation->environment->flags & 1) != 0)
        NuFree(animation->environment, NuTextureAnimationSourcePathPC, 520);
    if (animation->flags != 0)
        NuFree(animation, NuTextureAnimationSourcePathPC, 1690);
}

static TEXTURE_ANIM_PROGRAM_PC *TextureAnimationParseProgramPrivatePc;
static char TextureAnimationLabelsPrivatePc[64][21];
static i32 TextureAnimationLabelCountPrivatePc;
static char TextureAnimationExternalLabelsPrivatePc[256][21];
static i32 TextureAnimationExternalLabelCountPrivatePc;
static i32 TextureAnimationLabelOffsetsPrivatePc[64];

TEXTURE_ANIM_PROGRAM_PC *__cdecl NuTexAnimProgFind(const char *name) {
    TEXTURE_ANIM_PROGRAM_PC *program;
    for (program = (TEXTURE_ANIM_PROGRAM_PC *)GHGTextureProgramsPC; program != NULL; program = program->next) {
        if (NuStrICmp(name, program->name) == 0)
            return program;
    }
    return NULL;
}

void __cdecl NuTexAnimProgDestroy(TEXTURE_ANIM_PROGRAM_PC *program) {
    if (program->previous != NULL)
        program->previous->next = program->next;
    else
        GHGTextureProgramsPC = program->next;
    if (program->next != NULL)
        program->next->previous = program->previous;
    if ((program->flags & 1) != 0)
        NuFree(program, NuTextureAnimationSourcePathPC, 337);
}

void __cdecl NuTexAnimProgRelease(void) {
    GHGTextureProgramsPC = NULL;
}

static i32 __cdecl TextureAnimationLabelIndexPrivatePc(char *name) {
    i32 i;
    if (strlen(name) >= 21)
        name[20] = 0;
    for (i = 0; i < TextureAnimationLabelCountPrivatePc; ++i) {
        if (NuStrICmp(TextureAnimationLabelsPrivatePc[i], name) == 0)
            return i;
    }
    if (TextureAnimationLabelCountPrivatePc >= 64)
        AnimationDiagnosticNoOpPc();
    NuStrCpy(TextureAnimationLabelsPrivatePc[TextureAnimationLabelCountPrivatePc++], name);
    return TextureAnimationLabelCountPrivatePc - 1;
}

static i32 __cdecl TextureAnimationExternalLabelIndexPrivatePc(char *name) {
    i32 i;
    if (strlen(name) >= 21)
        name[20] = 0;
    for (i = 0; i < TextureAnimationExternalLabelCountPrivatePc; ++i) {
        if (NuStrICmp(TextureAnimationExternalLabelsPrivatePc[i], name) == 0)
            return i;
    }
    if (TextureAnimationExternalLabelCountPrivatePc >= 256)
        AnimationDiagnosticNoOpPc();
    NuStrCpy(TextureAnimationExternalLabelsPrivatePc[TextureAnimationExternalLabelCountPrivatePc++], name);
    return TextureAnimationExternalLabelCountPrivatePc - 1;
}

static i32 __cdecl TextureAnimationReadComparisonPrivatePc(NUFPAR_PC *parser) {
    char *word;
    NuFParGetWord(parser);
    word = parser->word_buffer_ptr;
    switch (word[0]) {
        case '<':
            if (word[1] == '=')
                return 3;
            if (word[1] == '>')
                return 5;
            return 1;
        case '>':
            return word[1] == '=' ? 4 : 2;
        case '!':
            return 5;
        case '=':
            return 0;
        default:
            ((void(__cdecl *)(const char *, i32))AnimationDiagnosticNoOpPc)(word, parser->line_number);
            return 0;
    }
}

static void __cdecl TextureAnimationScriptMaskPrivatePc(NUFPAR_PC *parser) {
    TextureAnimationParseProgramPrivatePc->mask = (u16)NuFParGetInt(parser);
}
static void __cdecl TextureAnimationTexPrivatePc(NUFPAR_PC *parser) {
    i32 value = NuFParGetInt(parser);
    TEXTURE_ANIM_PROGRAM_PC *p = TextureAnimationParseProgramPrivatePc;
    p->instructions[p->instruction_count++] = 0;
    p->instructions[p->instruction_count++] = (i16)value;
}
static void __cdecl TextureAnimationTexRandomPrivatePc(NUFPAR_PC *parser) {
    i32 value = NuFParGetInt(parser);
    TEXTURE_ANIM_PROGRAM_PC *p = TextureAnimationParseProgramPrivatePc;
    p->instructions[p->instruction_count++] = 1;
    p->instructions[p->instruction_count++] = (i16)value;
}
static void __cdecl TextureAnimationTexAdjustPrivatePc(NUFPAR_PC *parser) {
    i32 amount = NuFParGetInt(parser);
    i32 lower = NuFParGetInt(parser);
    i32 upper = NuFParGetInt(parser);
    TEXTURE_ANIM_PROGRAM_PC *p = TextureAnimationParseProgramPrivatePc;
    p->instructions[p->instruction_count++] = 2;
    p->instructions[p->instruction_count++] = (i16)amount;
    p->instructions[p->instruction_count++] = (i16)lower;
    p->instructions[p->instruction_count++] = (i16)upper;
}
static void __cdecl TextureAnimationTexAdjustRandomPrivatePc(NUFPAR_PC *parser) {
    i32 lower_adjust = NuFParGetInt(parser);
    i32 upper_adjust = NuFParGetInt(parser);
    i32 lower = NuFParGetInt(parser);
    i32 upper = NuFParGetInt(parser);
    TEXTURE_ANIM_PROGRAM_PC *p = TextureAnimationParseProgramPrivatePc;
    p->instructions[p->instruction_count++] = 3;
    p->instructions[p->instruction_count++] = (i16)lower_adjust;
    p->instructions[p->instruction_count++] = (i16)upper_adjust;
    p->instructions[p->instruction_count++] = (i16)lower;
    p->instructions[p->instruction_count++] = (i16)upper;
}
static void __cdecl TextureAnimationWaitPrivatePc(NUFPAR_PC *parser) {
    i32 value = NuFParGetInt(parser);
    i32 random = NuFParGetInt(parser);
    TEXTURE_ANIM_PROGRAM_PC *p = TextureAnimationParseProgramPrivatePc;
    p->instructions[p->instruction_count++] = 4;
    p->instructions[p->instruction_count++] = (i16)(i32)(value * (f64)(1.0f / 60.0f) * 4096.0);
    p->instructions[p->instruction_count++] = (i16)(i32)(random * (f64)(1.0f / 60.0f) * 4096.0);
}
static void __cdecl TextureAnimationRatePrivatePc(NUFPAR_PC *parser) {
    i32 value = NuFParGetInt(parser);
    i32 random = NuFParGetInt(parser);
    TEXTURE_ANIM_PROGRAM_PC *p = TextureAnimationParseProgramPrivatePc;
    p->instructions[p->instruction_count++] = 7;
    p->instructions[p->instruction_count++] = (i16)(i32)(value * (f64)(1.0f / 60.0f) * 4096.0);
    p->instructions[p->instruction_count++] = (i16)(i32)(random * (f64)(1.0f / 60.0f) * 4096.0);
}
static void __cdecl TextureAnimationOnPrivatePc(NUFPAR_PC *parser) {
    i32 signal = NuFParGetInt(parser);
    TEXTURE_ANIM_PROGRAM_PC *p = TextureAnimationParseProgramPrivatePc;
    p->on_signal[signal] = p->instruction_count;
    p->on_mask |= 1u << signal;
}
static void __cdecl TextureAnimationOffPrivatePc(NUFPAR_PC *parser) {
    i32 signal = NuFParGetInt(parser);
    TEXTURE_ANIM_PROGRAM_PC *p = TextureAnimationParseProgramPrivatePc;
    p->off_signal[signal] = p->instruction_count;
    p->off_mask |= 1u << signal;
}
static void __cdecl TextureAnimationLabelPrivatePc(NUFPAR_PC *parser) {
    i32 label;
    NuFParGetWord(parser);
    label = TextureAnimationLabelIndexPrivatePc(parser->word_buffer_ptr);
    TextureAnimationLabelOffsetsPrivatePc[label] = TextureAnimationParseProgramPrivatePc->instruction_count;
}
static void __cdecl TextureAnimationGotoPrivatePc(NUFPAR_PC *parser) {
    i32 label;
    TEXTURE_ANIM_PROGRAM_PC *p;
    NuFParGetWord(parser);
    label = TextureAnimationLabelIndexPrivatePc(parser->word_buffer_ptr);
    p = TextureAnimationParseProgramPrivatePc;
    p->instructions[p->instruction_count++] = 9;
    p->instructions[p->instruction_count++] = (i16)label;
}
static void __cdecl TextureAnimationGosubPrivatePc(NUFPAR_PC *parser) {
    i32 label;
    TEXTURE_ANIM_PROGRAM_PC *p;
    NuFParGetWord(parser);
    label = TextureAnimationLabelIndexPrivatePc(parser->word_buffer_ptr);
    p = TextureAnimationParseProgramPrivatePc;
    p->instructions[p->instruction_count++] = 10;
    p->instructions[p->instruction_count++] = (i16)label;
}
static void __cdecl TextureAnimationBranchPrivatePc(NUFPAR_PC *parser) {
    i32 comparison = TextureAnimationReadComparisonPrivatePc(parser);
    i32 value = NuFParGetInt(parser);
    i32 label;
    TEXTURE_ANIM_PROGRAM_PC *p;
    NuFParGetWord(parser);
    label = TextureAnimationLabelIndexPrivatePc(parser->word_buffer_ptr);
    p = TextureAnimationParseProgramPrivatePc;
    p->instructions[p->instruction_count++] = 11;
    p->instructions[p->instruction_count++] = (i16)comparison;
    p->instructions[p->instruction_count++] = (i16)value;
    p->instructions[p->instruction_count++] = (i16)label;
}
static void __cdecl TextureAnimationReturnPrivatePc(NUFPAR_PC *parser) {
    (void)parser;
    TextureAnimationParseProgramPrivatePc->instructions[TextureAnimationParseProgramPrivatePc->instruction_count++] =
        12;
}
static void __cdecl TextureAnimationRepeatPrivatePc(NUFPAR_PC *parser) {
    i32 value = NuFParGetInt(parser);
    i32 random = NuFParGetInt(parser);
    TEXTURE_ANIM_PROGRAM_PC *p = TextureAnimationParseProgramPrivatePc;
    if (value == 0)
        value = -1;
    p->instructions[p->instruction_count++] = 13;
    p->instructions[p->instruction_count++] = (i16)value;
    p->instructions[p->instruction_count++] = (i16)random;
}
static void __cdecl TextureAnimationRepeatEndPrivatePc(NUFPAR_PC *parser) {
    (void)parser;
    TextureAnimationParseProgramPrivatePc->instructions[TextureAnimationParseProgramPrivatePc->instruction_count++] =
        14;
}
static void __cdecl TextureAnimationUntilPrivatePc(NUFPAR_PC *parser) {
    i32 comparison = TextureAnimationReadComparisonPrivatePc(parser);
    i32 value = NuFParGetInt(parser);
    TEXTURE_ANIM_PROGRAM_PC *p = TextureAnimationParseProgramPrivatePc;
    p->instructions[p->instruction_count++] = 15;
    p->instructions[p->instruction_count++] = (i16)comparison;
    p->instructions[p->instruction_count++] = (i16)value;
}
static void __cdecl TextureAnimationEndPrivatePc(NUFPAR_PC *parser) {
    (void)parser;
    TextureAnimationParseProgramPrivatePc->instructions[TextureAnimationParseProgramPrivatePc->instruction_count++] =
        16;
}
static void __cdecl TextureAnimationScriptNamePrivatePc(NUFPAR_PC *parser) {
    NuFParGetWord(parser);
    parser->word_buffer_ptr[32] = 0;
    NuStrCpy(TextureAnimationParseProgramPrivatePc->name, parser->word_buffer_ptr);
}
static void __cdecl TextureAnimationExternalDefinitionPrivatePc(NUFPAR_PC *parser) {
    i32 label, index;
    TEXTURE_ANIM_PROGRAM_PC *p;
    NuFParGetWord(parser);
    label = TextureAnimationExternalLabelIndexPrivatePc(parser->word_buffer_ptr);
    p = TextureAnimationParseProgramPrivatePc;
    index = p->label_count;
    p->label_ids[index] = (i16)label;
    p->label_offsets[index] = p->instruction_count;
    p->label_count = index + 1;
}
static void __cdecl TextureAnimationExternalReferencePrivatePc(NUFPAR_PC *parser) {
    i32 label;
    TEXTURE_ANIM_PROGRAM_PC *p;
    NuFParGetWord(parser);
    label = TextureAnimationExternalLabelIndexPrivatePc(parser->word_buffer_ptr);
    p = TextureAnimationParseProgramPrivatePc;
    p->instructions[p->instruction_count++] = 17;
    p->instructions[p->instruction_count++] = (i16)label;
}

static NUFPCOMJMP TextureAnimationCommandsPrivatePc[] = {{"scriptmask", TextureAnimationScriptMaskPrivatePc},
                                                         {"tex", TextureAnimationTexPrivatePc},
                                                         {"texr", TextureAnimationTexRandomPrivatePc},
                                                         {"texadj", TextureAnimationTexAdjustPrivatePc},
                                                         {"texadjr", TextureAnimationTexAdjustRandomPrivatePc},
                                                         {"wait", TextureAnimationWaitPrivatePc},
                                                         {"rate", TextureAnimationRatePrivatePc},
                                                         {"on", TextureAnimationOnPrivatePc},
                                                         {"off", TextureAnimationOffPrivatePc},
                                                         {"label", TextureAnimationLabelPrivatePc},
                                                         {"goto", TextureAnimationGotoPrivatePc},
                                                         {"btex", TextureAnimationBranchPrivatePc},
                                                         {"gosub", TextureAnimationGosubPrivatePc},
                                                         {"ret", TextureAnimationReturnPrivatePc},
                                                         {"repeat", TextureAnimationRepeatPrivatePc},
                                                         {"repend", TextureAnimationRepeatEndPrivatePc},
                                                         {"untiltex", TextureAnimationUntilPrivatePc},
                                                         {"end", TextureAnimationEndPrivatePc},
                                                         {"scriptname", TextureAnimationScriptNamePrivatePc},
                                                         {"xdef", TextureAnimationExternalDefinitionPrivatePc},
                                                         {"xref", TextureAnimationExternalReferencePrivatePc},
                                                         {NULL, NULL}};

void __cdecl NuTexAnimProgInit(TEXTURE_ANIM_PROGRAM_PC *program) {
    i32 i;
    if (program == NULL)
        return;
    for (i = 0; i < 32; ++i) {
        program->off_signal[i] = -1;
        program->on_signal[i] = -1;
    }
    program->instruction_count = 0;
    program->on_mask = 0;
    program->off_mask = 0;
    program->name[0] = 0;
    program->label_count = 0;
    program->flags &= ~1u;
    program->mask = 1;
}

TEXTURE_ANIM_PROGRAM_PC *__cdecl NuTexAnimProgCreate(VARIPTR *buffer, i32 extra_bytes, const char *name) {
    TEXTURE_ANIM_PROGRAM_PC *program;
    if (buffer != NULL) {
        program = (TEXTURE_ANIM_PROGRAM_PC *)buffer->addr;
        buffer->addr += sizeof(*program) + extra_bytes;
    } else {
        program =
            (TEXTURE_ANIM_PROGRAM_PC *)NuAlloc(sizeof(*program) + extra_bytes, NuTextureAnimationSourcePathPC, 386);
    }
    if (program != NULL) {
        NuTexAnimProgInit(program);
        if (buffer == NULL)
            program->flags |= 1;
        if (name != NULL)
            NuStrCpy(program->name, name);
    }
    return program;
}

void __cdecl NuTexAnimProgWrite(char *path, TEXTURE_ANIM_PROGRAM_PC *program) {
    NUFILE file = NuFileOpen(path, 1);
    if (file != 0) {
        NuFileWrite(file, program, sizeof(*program) + program->instruction_count * 2);
        NuFileClose(file);
    }
}

TEXTURE_ANIM_PROGRAM_PC *__cdecl NuTexAnimProgRead(VARIPTR *buffer, char *path) {
    NUFILE file = NuFileOpen(path, 0);
    TEXTURE_ANIM_PROGRAM_PC *program = NULL;
    if (file != 0) {
        u32 size = NuFileOpenSize(file);
        if (buffer != NULL) {
            program = (TEXTURE_ANIM_PROGRAM_PC *)((buffer->addr + 3) & ~3u);
            buffer->addr = (u32)program + size;
        } else {
            program = (TEXTURE_ANIM_PROGRAM_PC *)NuAlloc(size, NuTextureAnimationSourcePathPC, 444);
        }
        NuFileRead(file, program, size);
        NuFileClose(file);
        program->next = (TEXTURE_ANIM_PROGRAM_PC *)GHGTextureProgramsPC;
        if (program->next != NULL)
            program->next->previous = program;
        program->previous = NULL;
        GHGTextureProgramsPC = program;
        if (buffer != NULL)
            program->flags &= ~1u;
        else
            program->flags |= 1;
    }
    return program;
}

void __cdecl NuTexAnimProgAssembleEnd(TEXTURE_ANIM_PROGRAM_PC *program) {
    i16 index = 0;
    while (index < program->instruction_count) {
        switch (program->instructions[index]) {
            case 0:
            case 1:
            case 17:
                index += 2;
                break;
            case 3:
                index += 5;
                break;
            case 4:
            case 7:
            case 13:
            case 15:
                index += 3;
                break;
            case 9:
            case 10:
                program->instructions[index + 1] =
                    (i16)TextureAnimationLabelOffsetsPrivatePc[program->instructions[index + 1]];
                index += 2;
                break;
            case 11:
                program->instructions[index + 3] =
                    (i16)TextureAnimationLabelOffsetsPrivatePc[program->instructions[index + 3]];
                index += 4;
                break;
            case 2:
                index += 4;
                break;
            case 12:
            case 14:
            case 16:
                ++index;
                break;
        }
    }
}

TEXTURE_ANIM_PROGRAM_PC *__cdecl NuTexAnimProgParseFile(NUFILE file, VARIPTR *buffer, VARIPTR end, i32 fps) {
    TEXTURE_ANIM_PROGRAM_PC *program;
    NUFPAR_PC *parser;
    i32 length;
    (void)fps;
    if (buffer != NULL)
        program = (TEXTURE_ANIM_PROGRAM_PC *)((buffer->addr + 3) & ~3u);
    else {
        program = (TEXTURE_ANIM_PROGRAM_PC *)NuAlloc(1024, NuTextureAnimationSourcePathPC, 962);
        end.addr = (u32)program + 1023;
    }
    TextureAnimationLabelCountPrivatePc = 0;
    memset(TextureAnimationLabelsPrivatePc, 0, sizeof(TextureAnimationLabelsPrivatePc));
    parser = NuFParOpen(file);
    if (parser == NULL)
        return NULL;
    NuFParPushCom(parser, TextureAnimationCommandsPrivatePc);
    if ((u32)(program + 1) >= end.addr)
        AnimationDiagnosticNoOpPc();
    NuTexAnimProgInit(program);
    TextureAnimationParseProgramPrivatePc = program;
    while (NuFParGetLine(parser) != 0) {
        length = NuFParGetWord(parser);
        if (length != 0 && NuFParInterpretWord(parser) == 0 && parser->word_buffer_ptr[0] != 0 &&
            parser->word_buffer_ptr[length - 1] == ':') {
            i32 label;
            parser->word_buffer_ptr[length - 1] = 0;
            label = TextureAnimationLabelIndexPrivatePc(parser->word_buffer_ptr);
            TextureAnimationLabelOffsetsPrivatePc[label] = TextureAnimationParseProgramPrivatePc->instruction_count;
        }
        if ((u32)(program->instructions + program->instruction_count) >= end.addr)
            AnimationDiagnosticNoOpPc();
    }
    if (buffer != NULL)
        buffer->addr = (u32)(program->instructions + program->instruction_count);
    NuFreeFPar(parser);
    NuTexAnimProgAssembleEnd(program);
    program->next = (TEXTURE_ANIM_PROGRAM_PC *)GHGTextureProgramsPC;
    if (program->next != NULL)
        program->next->previous = program;
    program->previous = NULL;
    GHGTextureProgramsPC = program;
    return program;
}

TEXTURE_ANIM_PROGRAM_PC *__cdecl NuTexAnimProgReadScript(char *path, VARIPTR *buffer, VARIPTR end, i32 fps) {
    NUFILE file = NuFileOpen(path, 0);
    TEXTURE_ANIM_PROGRAM_PC *program = NULL;
    if (file != 0) {
        program = NuTexAnimProgParseFile(file, buffer, end, fps);
        NuFileClose(file);
    }
    return program;
}

TEXTURE_ANIMATION_PC *__cdecl NuTexAnimCreate(VARIPTR *buffer, TEXTURE_ANIM_PROGRAM_PC *program, NUMTL *material,
                                              u16 *texture_ids, i32 texture_count) {
    TEXTURE_ANIMATION_PC *animation;
    if (buffer != NULL) {
        animation = (TEXTURE_ANIMATION_PC *)((buffer->addr + 3) & ~3u);
        buffer->addr = (u32)(animation + 1) + texture_count * 2;
    } else {
        animation = (TEXTURE_ANIMATION_PC *)NuAlloc(sizeof(*animation) + texture_count * 2,
                                                    NuTextureAnimationSourcePathPC, 1720);
    }
    animation->texture_count = (i16)texture_count;
    animation->material = material;
    animation->previous = NULL;
    animation->next = NULL;
    animation->name = NULL;
    animation->texture_ids = (u16 *)(animation + 1);
    memcpy(animation->texture_ids, texture_ids, texture_count * 2);
    animation->environment = NuTexAnimEnvCreate(buffer, material, animation->texture_ids, program);
    return animation;
}

void __cdecl NuTexAnimEnvReset(PC_TEXTURE_ANIM_ENV *environment) {
    if (environment != NULL) {
        environment->instruction_index = 0;
        environment->loop_depth = 0;
        environment->call_depth = 0;
        environment->wait_base = 0;
        environment->wait_random = 0;
        environment->wait_remaining = 0;
        environment->texture_index = 0;
    }
}
