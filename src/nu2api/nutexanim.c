#include "gameapi/edtools/edui.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nu3d/nuqfnt.h"
#include "nu2api/nu3d/nurndr.h"
#include "nu2api/nu3d/nutexanm.h"
#include "nu2api/nucore/nucore_includes.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nucore/nuthread.h"
#include "nu2api/nudir_gen.h"
#include "nu2api/nufile/nufile.h"
#include "nu2api/nufile/nufilepak.h"
#include "nu2api/nufile/nufpar.h"
#include "nu2api/nufpar_gen.h"
#include "nu2api/numath/numaths.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nurand.h"
#include "nu2api/numath/nuvec.h"
#include <string.h>
static i32 nutexanm_critical_section = -1;
static PC_TEXTURE_ANIM_LIST *ntal_free = 0;
i32 NuTexAnimMissingPakDiagnostic = 0;

void NuTexAnimAddList(void *animation) {
    PC_TEXTURE_ANIM_LIST *node;
    if (animation == NULL) {
        return;
    }
    NuThreadCriticalSectionBegin(nutexanm_critical_section);
    if (ntal_free != NULL) {
        node = ntal_free;
        if (node->next == NULL) {
            NuSevereWarning("Ran out of texture anim slots!");
        }
        ntal_free = ntal_free->next;
        node->previous = NULL;
        node->next = ntal_first;
        node->first = animation;
        if (node->next != NULL) {
            node->next->previous = node;
        }
        ntal_first = node;
    }
    NuThreadCriticalSectionEnd(nutexanm_critical_section);
}

void NuTexAnimRemoveList(void *animation) {
    PC_TEXTURE_ANIM_LIST *node;
    NuThreadCriticalSectionBegin(nutexanm_critical_section);
    for (node = ntal_first; node != NULL; node = node->next) {
        if (node->first == animation) {
            if (node->next != NULL) {
                node->next->previous = node->previous;
            }
            if (node->previous != NULL) {
                node->previous->next = node->next;
            } else {
                ntal_first = node->next;
            }
            node->next = ntal_free;
            ntal_free = node;
            NuThreadEnableThreadSwap();
            NuThreadCriticalSectionEnd(nutexanm_critical_section);
            return;
        }
    }
    NuThreadCriticalSectionEnd(nutexanm_critical_section);
}

PC_TEXTURE_ANIM_ENV *NuTexAnimEnvCreate(VARIPTR *buffer, NUMTL *material, u16 *texture_ids, void *program) {
    PC_TEXTURE_ANIM_ENV *environment;
    if (buffer != NULL) {
        environment = (PC_TEXTURE_ANIM_ENV *)((buffer->addr + 3) & ~3u);
        buffer->void_ptr = environment + 1;
    } else {
        environment = NuAlloc(sizeof(PC_TEXTURE_ANIM_ENV), "d:\\projects\\game_saga\\nu2api.saga\\nu3d\\nutexanm.c", 548);
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
        if (buffer != NULL) {
            environment->flags &= ~1u;
        } else {
            environment->flags |= 1;
        }
    }
    return environment;
}

static u32 nta_sig_off = 0;
NURAND texanim_rand = {0};
static u32 nta_sig_on = 0;
NUVPCURRENT vpDevice = {0};
NUFILE nudlist_field_958 = 0;
static i32 nta_iframetime = 0;
static u16 nta_script_mask = 0;
void *GHGTextureProgramsPC = 0;
PC_TEXTURE_ANIM_LIST *ntal_first = 0;
static u32 nta_sig_old = 0;
CAMERA_SAVED_STATE CameraSavedStates[16] = {{0}};
i32 NuQFntCSModeStack[16] = {0};
NUPORTAL_FRUSTUM **NuPortalFrusta = 0;

void NuTexAnimSetSignals(u32 signals) {
    u32 changed = nta_sig_old ^ signals;
    nta_sig_on = changed & signals;
    nta_sig_off = changed & ~signals;
    nta_sig_old = signals;
}

void NuTexAnimSetMask(i32 mask) {
    script_mask = (u16)mask;
}

static i32 TextureAnimationConditionPrivate(i32 condition, i32 texture, i32 value) {
    switch (condition) {
        case TEXANIM_CMP_EQ:
            if (texture == value) {
                return 1;
            }
            break;
        case TEXANIM_CMP_LT:
            if (texture < value) {
                return 1;
            }
            break;
        case TEXANIM_CMP_GT:
            if (texture > value) {
                return 1;
            }
            break;
        case TEXANIM_CMP_LE:
            if (texture <= value) {
                return 1;
            }
            break;
        case TEXANIM_CMP_GE:
            if (texture >= value) {
                return 1;
            }
            break;
        case TEXANIM_CMP_NE:
            if (texture != value) {
                return 1;
            }
            break;
    }
    return 0;
}

static void TextureAnimationCrossReferencePrivate(i32 label, PC_TEXTURE_ANIM_ENV *current) {
    PC_TEXTURE_ANIM_LIST *list;
    for (list = ntal_first; list != NULL; list = list->next) {
        TEXTURE_ANIMATION *animation;
        for (animation = list->first; animation != NULL; animation = animation->next) {
            PC_TEXTURE_ANIM_ENV *other = animation->environment;
            TEXTURE_ANIM_PROGRAM *program;
            i32 i;
            if (other == NULL || other == current || other->program == NULL) {
                continue;
            }
            program = other->program;
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

void NuTexAnimEnvProc(PC_TEXTURE_ANIM_ENV *environment) {
    TEXTURE_ANIM_PROGRAM *program = environment->program;
    NUMTL *material;
    i32 i;
    if (program == NULL || (program->mask & nta_script_mask) == 0) {
        return;
    }
    material = environment->material;
    if ((nta_sig_off & program->off_mask) != 0) {
        for (i = 0; i < 32; ++i) {
            if ((nta_sig_off & (program->off_mask & (1U << i))) != 0) {
                environment->instruction_index = program->off_signal[i];
                environment->wait_remaining = 0;
                environment->call_depth = 0;
                environment->loop_depth = 0;
                break;
            }
        }
    }
    if ((nta_sig_on & program->on_mask) != 0) {
        for (i = 0; i < 32; ++i) {
            if ((nta_sig_on & (program->on_mask & (1U << i))) != 0) {
                environment->instruction_index = program->on_signal[i];
                environment->wait_remaining = 0;
                environment->call_depth = 0;
                environment->loop_depth = 0;
                break;
            }
        }
    }
    if (environment->wait_remaining != 0) {
        environment->wait_remaining -= nta_iframetime;
    }
    if (environment->wait_remaining > 0) {
        return;
    }
    for (;;) {
        switch (program->instructions[environment->instruction_index]) {
            case TEXANIM_OP_RATE:
                environment->wait_base = program->instructions[environment->instruction_index + 1];
                environment->wait_random = program->instructions[environment->instruction_index + 2];
                environment->instruction_index += 3;
                continue;
            case TEXANIM_OP_GOTO:
                environment->instruction_index = program->instructions[environment->instruction_index + 1];
                continue;
            case TEXANIM_OP_BTEX:
                if (TextureAnimationConditionPrivate(program->instructions[environment->instruction_index + 1], environment->texture_index,
                                                     program->instructions[environment->instruction_index + 2])) {
                    environment->instruction_index = program->instructions[environment->instruction_index + 3];
                } else {
                    environment->instruction_index += 4;
                }
                continue;
            case TEXANIM_OP_GOSUB:
                if (environment->call_depth >= 16) {
                    ((void (*)(i32))AnimationDiagnosticNoOp)(environment->instruction_index);
                }
                environment->return_stack[environment->call_depth++] = environment->instruction_index + 2;
                environment->instruction_index = program->instructions[environment->instruction_index + 1];
                continue;
            case TEXANIM_OP_RET:
                if (environment->call_depth == 0) {
                    ((void (*)(i32))AnimationDiagnosticNoOp)(environment->instruction_index);
                }
                environment->instruction_index = environment->return_stack[--environment->call_depth];
                continue;
            case TEXANIM_OP_REPEAT:
                if (environment->loop_depth >= 16) {
                    ((void (*)(i32))AnimationDiagnosticNoOp)(environment->instruction_index);
                }
                environment->loop_counts[environment->loop_depth] = program->instructions[environment->instruction_index + 1];
                if (program->instructions[environment->instruction_index + 2] != 0) {
                    i32 *loop_count = &environment->loop_counts[environment->loop_depth];
                    i32 random = NuRand(&texanim_rand);
                    *loop_count += random % program->instructions[environment->instruction_index + 2];
                }
                environment->instruction_index += 3;
                environment->loop_starts[environment->loop_depth++] = environment->instruction_index;
                continue;
            case TEXANIM_OP_REPEND:
                if (environment->loop_depth == 0) {
                    ((void (*)(i32))AnimationDiagnosticNoOp)(environment->instruction_index);
                }
                if (environment->loop_counts[environment->loop_depth - 1] == 0) {
                    --environment->loop_depth;
                    ++environment->instruction_index;
                } else {
                    environment->instruction_index = environment->loop_starts[environment->loop_depth - 1];
                    --environment->loop_counts[environment->loop_depth - 1];
                }
                continue;
            case TEXANIM_OP_UNTILTEX:
                if (environment->loop_depth == 0) {
                    ((void (*)(i32))AnimationDiagnosticNoOp)(environment->instruction_index);
                }
                if (TextureAnimationConditionPrivate(program->instructions[environment->instruction_index + 1], environment->texture_index,
                                                     program->instructions[environment->instruction_index + 2]) ||
                    environment->loop_counts[environment->loop_depth - 1] == 0) {
                    environment->instruction_index += 3;
                    --environment->loop_depth;
                } else {
                    environment->instruction_index = environment->loop_starts[environment->loop_depth - 1];
                    --environment->loop_counts[environment->loop_depth - 1];
                }
                continue;
            case TEXANIM_OP_XREF:
                TextureAnimationCrossReferencePrivate(program->instructions[environment->instruction_index + 1], environment);
                environment->instruction_index += 2;
                continue;
            case TEXANIM_OP_TEX:
                environment->texture_index = program->instructions[environment->instruction_index + 1];
                material->texture_id = environment->texture_ids[environment->texture_index];
                material->shader.diffuse_map_ids[0] = environment->texture_ids[environment->texture_index] & 0x7fff;
                environment->instruction_index += 2;
                environment->wait_remaining += environment->wait_base;
                if (environment->wait_random != 0) {
                    environment->wait_remaining += NuRand(&texanim_rand) % environment->wait_random;
                }
                if (environment->wait_remaining < 0) {
                    environment->wait_remaining = 0;
                }
                return;
            case TEXANIM_OP_TEXR: {
                i32 random = NuRand(&texanim_rand);
                environment->texture_index = random % program->instructions[environment->instruction_index + 1];
                material->texture_id = environment->texture_ids[environment->texture_index];
                material->shader.diffuse_map_ids[0] = environment->texture_ids[environment->texture_index] & 0x7fff;
                environment->instruction_index += 2;
                environment->wait_remaining += environment->wait_base;
                if (environment->wait_random != 0) {
                    environment->wait_remaining += NuRand(&texanim_rand) % environment->wait_random;
                }
                if (environment->wait_remaining < 0) {
                    environment->wait_remaining = 0;
                }
                return;
            }
            case TEXANIM_OP_TEXADJ:
                environment->texture_index =
                    MIN(program->instructions[environment->instruction_index + 3],
                        MAX(program->instructions[environment->instruction_index + 2],
                            program->instructions[environment->instruction_index + 1] + environment->texture_index));
                material->texture_id = environment->texture_ids[environment->texture_index];
                material->shader.diffuse_map_ids[0] = environment->texture_ids[environment->texture_index] & 0x7fff;
                environment->instruction_index += 4;
                environment->wait_remaining += environment->wait_base;
                if (environment->wait_random != 0) {
                    environment->wait_remaining += NuRand(&texanim_rand) % environment->wait_random;
                }
                if (environment->wait_remaining < 0) {
                    environment->wait_remaining = 0;
                }
                return;
            case TEXANIM_OP_TEXADJR: {
                i32 range = program->instructions[environment->instruction_index + 2] -
                            program->instructions[environment->instruction_index + 1] + 1;
                i32 texture = program->instructions[environment->instruction_index + 1] + NuRand(&texanim_rand) % range;
                environment->texture_index =
                    MIN(program->instructions[environment->instruction_index + 4],
                        MAX(program->instructions[environment->instruction_index + 3], environment->texture_index + texture));
                material->texture_id = environment->texture_ids[environment->texture_index];
                material->shader.diffuse_map_ids[0] = environment->texture_ids[environment->texture_index] & 0x7fff;
                environment->instruction_index += 5;
                environment->wait_remaining += environment->wait_base;
                if (environment->wait_random != 0) {
                    environment->wait_remaining += NuRand(&texanim_rand) % environment->wait_random;
                }
                if (environment->wait_remaining < 0) {
                    environment->wait_remaining = 0;
                }
                return;
            }
            case TEXANIM_OP_WAIT:
                environment->wait_remaining += program->instructions[environment->instruction_index + 1];
                if (program->instructions[environment->instruction_index + 2] != 0) {
                    environment->wait_remaining += NuRand(&texanim_rand) % program->instructions[environment->instruction_index + 2];
                }
                environment->instruction_index += 3;
                if (environment->wait_remaining < 0) {
                    environment->wait_remaining = 0;
                }
                return;
            case TEXANIM_OP_END:
                if (environment->wait_remaining < 0) {
                    environment->wait_remaining = 0;
                }
                return;
            default:
                continue;
        }
    }
}

void NuTexAnimProcessList(TEXTURE_ANIMATION *animation) {
    while (animation != NULL) {
        if (animation->environment != NULL) {
            NuTexAnimEnvProc(animation->environment);
        }
        animation = animation->next;
    }
}

void NuTexAnimResetList(TEXTURE_ANIMATION *animation) {
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

void NuTexAnimInitMutex(void) {
    nutexanm_critical_section = PCThreadCriticalSectionCreate_candidate();
}

void NuTexAnimProcess(f32 frame_time) {
    PC_TEXTURE_ANIM_LIST *list;
    NuThreadCriticalSectionBegin(nutexanm_critical_section);
    nta_iframetime = (i32)(frame_time * 4096.0);
    nta_script_mask = script_mask;
    for (list = ntal_first; list != NULL; list = list->next) {
        TEXTURE_ANIMATION *animation = list->first;
        while (animation != NULL) {
            if (animation->environment != NULL) {
                NuTexAnimEnvProc(animation->environment);
            }
            animation = animation->next;
        }
    }
    NuThreadCriticalSectionEnd(nutexanm_critical_section);
}

void NuTexAnimRestart(void) {
    PC_TEXTURE_ANIM_LIST *list;
    for (list = ntal_first; list != NULL; list = list->next) {
        NuTexAnimResetList(list->first);
    }
}

TEXTURE_ANIMATION *NuTexAnimFind(TEXTURE_ANIMATION *animation, const char *name) {
    while (animation != NULL) {
        if (NuStrICmp(animation->name, name) == 0) {
            return animation;
        }
        animation = animation->next;
    }
    return NULL;
}

void NuTexAnimDestroy(TEXTURE_ANIMATION *animation) {
    if (animation->environment != NULL && (animation->environment->flags & 1) != 0) {
        NuFree(animation->environment, "d:\\projects\\game_saga\\nu2api.saga\\nu3d\\nutexanm.c", 520);
    }
    if (animation->flags != 0) {
        NuFree(animation, "d:\\projects\\game_saga\\nu2api.saga\\nu3d\\nutexanm.c", 1690);
    }
}

static TEXTURE_ANIM_PROGRAM *TextureAnimationParseProgramPrivate = 0;
static char TextureAnimationLabelsPrivate[64][21] = {0};
char PCDisplayCaptureBuffer[0x1000] = {0};
NUCAMERAREFLECT global_reflect = {0};
f32 zy = 0;
VARIPTR *NuDisplayListStreamCursor = 0;
static i32 TextureAnimationLabelCountPrivate = 0;
NUMTX VisibilityAlternateWorldMatrix = {0};
NUMTX VisibilityAlternatePlaneMatrix = {0};
VARIPTR fstack_top = {0};
NUVEC NuRndrDebBase = {0};
struct PC_SPECIAL_SHADOW_LIGHT *PCSpecialShadowLights[4] = {0};
static char TextureAnimationExternalLabelsPrivate[256][21] = {0};
static i32 xdeflabtabcnt = 0;
static i32 TextureAnimationLabelOffsetsPrivate[64] = {0};

static PC_TEXTURE_ANIM_LIST TextureAnimationListBuffer[64] = {0};

void NuTexAnimProgSysInit(void) {
    i32 i;
    GHGTextureProgramsPC = NULL;
    for (i = 0; i < 63; i++) {
        TextureAnimationListBuffer[i].next = &TextureAnimationListBuffer[i + 1];
    }
    TextureAnimationListBuffer[63].next = NULL;
    ntal_free = TextureAnimationListBuffer;
    ntal_first = NULL;
    xdeflabtabcnt = 0;
    nta_sig_old = 0;
    nta_sig_off = 0;
    nta_sig_on = 0;
    texanim_rand.value = 0;
}

TEXTURE_ANIM_PROGRAM *NuTexAnimProgFind(const char *name) {
    TEXTURE_ANIM_PROGRAM *program;
    for (program = GHGTextureProgramsPC; program != NULL; program = program->next) {
        if (NuStrICmp(name, program->name) == 0) {
            break;
        }
    }
    return program;
}

void NuTexAnimProgDestroy(TEXTURE_ANIM_PROGRAM *program) {
    if (program->previous != NULL) {
        program->previous->next = program->next;
    } else {
        GHGTextureProgramsPC = program->next;
    }
    if (program->next != NULL) {
        program->next->previous = program->previous;
    }
    if ((program->flags & 1) != 0) {
        NuFree(program, "d:\\projects\\game_saga\\nu2api.saga\\nu3d\\nutexanm.c", 337);
    }
}

void NuTexAnimProgRelease(void) {
    GHGTextureProgramsPC = NULL;
}

static i32 TextureAnimationLabelIndexPrivate(char *name) {
    i32 i;
    if (strlen(name) >= 21) {
        name[20] = 0;
    }
    for (i = 0; i < TextureAnimationLabelCountPrivate; ++i) {
        if (NuStrICmp(TextureAnimationLabelsPrivate[i], name) == 0) {
            return i;
        }
    }
    if (TextureAnimationLabelCountPrivate >= 64) {
        AnimationDiagnosticNoOp();
    }
    NuStrCpy(TextureAnimationLabelsPrivate[TextureAnimationLabelCountPrivate++], name);
    return TextureAnimationLabelCountPrivate - 1;
}

static i32 TextureAnimationExternalLabelIndexPrivate(char *name) {
    i32 i;
    if (strlen(name) >= 21) {
        name[20] = 0;
    }
    for (i = 0; i < xdeflabtabcnt; ++i) {
        if (NuStrICmp(TextureAnimationExternalLabelsPrivate[i], name) == 0) {
            return i;
        }
    }
    if (xdeflabtabcnt >= 256) {
        AnimationDiagnosticNoOp();
    }
    NuStrCpy(TextureAnimationExternalLabelsPrivate[xdeflabtabcnt++], name);
    return xdeflabtabcnt - 1;
}

static i32 TextureAnimationReadComparisonPrivate(NUFPAR *parser) {
    char *word;
    NuFParGetWord(parser);
    word = parser->word_buffer_ptr;
    switch (word[0]) {
        case '<':
            switch (word[1]) {
                case '=':
                    return TEXANIM_CMP_LE;
                case '>':
                    return TEXANIM_CMP_NE;
            }
            return TEXANIM_CMP_LT;
        case '>':
            return word[1] == '=' ? TEXANIM_CMP_GE : TEXANIM_CMP_GT;
        case '!':
            return TEXANIM_CMP_NE;
        case '=':
            return TEXANIM_CMP_EQ;
        default:
            ((void (*)(const char *, i32))AnimationDiagnosticNoOp)(word, parser->line_number);
            return TEXANIM_CMP_EQ;
    }
}

static void TextureAnimationScriptMaskPrivate(NUFPAR *parser) {
    TextureAnimationParseProgramPrivate->mask = (u16)NuFParGetInt(parser);
}

static void TextureAnimationTexPrivate(NUFPAR *parser) {
    i32 value = NuFParGetInt(parser);
    TEXTURE_ANIM_PROGRAM *p = TextureAnimationParseProgramPrivate;
    p->instructions[p->instruction_count++] = TEXANIM_OP_TEX;
    p->instructions[p->instruction_count++] = (i16)value;
}

static void TextureAnimationTexRandomPrivate(NUFPAR *parser) {
    i32 value = NuFParGetInt(parser);
    TEXTURE_ANIM_PROGRAM *p = TextureAnimationParseProgramPrivate;
    p->instructions[p->instruction_count++] = TEXANIM_OP_TEXR;
    p->instructions[p->instruction_count++] = (i16)value;
}

static void TextureAnimationTexAdjustPrivate(NUFPAR *parser) {
    i32 amount = NuFParGetInt(parser);
    i32 lower = NuFParGetInt(parser);
    i32 upper = NuFParGetInt(parser);
    TEXTURE_ANIM_PROGRAM *p = TextureAnimationParseProgramPrivate;
    p->instructions[p->instruction_count++] = TEXANIM_OP_TEXADJ;
    p->instructions[p->instruction_count++] = (i16)amount;
    p->instructions[p->instruction_count++] = (i16)lower;
    p->instructions[p->instruction_count++] = (i16)upper;
}

static void TextureAnimationTexAdjustRandomPrivate(NUFPAR *parser) {
    i32 lower_adjust = NuFParGetInt(parser);
    i32 upper_adjust = NuFParGetInt(parser);
    i32 lower = NuFParGetInt(parser);
    i32 upper = NuFParGetInt(parser);
    TEXTURE_ANIM_PROGRAM *p = TextureAnimationParseProgramPrivate;
    p->instructions[p->instruction_count++] = TEXANIM_OP_TEXADJR;
    p->instructions[p->instruction_count++] = (i16)lower_adjust;
    p->instructions[p->instruction_count++] = (i16)upper_adjust;
    p->instructions[p->instruction_count++] = (i16)lower;
    p->instructions[p->instruction_count++] = (i16)upper;
}

static void TextureAnimationWaitPrivate(NUFPAR *parser) {
    i32 value = NuFParGetInt(parser);
    i32 random = NuFParGetInt(parser);
    TEXTURE_ANIM_PROGRAM *p = TextureAnimationParseProgramPrivate;
    p->instructions[p->instruction_count++] = TEXANIM_OP_WAIT;
    p->instructions[p->instruction_count++] = (i16)(i32)(value * (f64)(1.0f / 60.0f) * 4096.0);
    p->instructions[p->instruction_count++] = (i16)(i32)(random * (f64)(1.0f / 60.0f) * 4096.0);
}

static void TextureAnimationRatePrivate(NUFPAR *parser) {
    i32 value = NuFParGetInt(parser);
    i32 random = NuFParGetInt(parser);
    TEXTURE_ANIM_PROGRAM *p = TextureAnimationParseProgramPrivate;
    p->instructions[p->instruction_count++] = TEXANIM_OP_RATE;
    p->instructions[p->instruction_count++] = (i16)(i32)(value * (f64)(1.0f / 60.0f) * 4096.0);
    p->instructions[p->instruction_count++] = (i16)(i32)(random * (f64)(1.0f / 60.0f) * 4096.0);
}

static void TextureAnimationOnPrivate(NUFPAR *parser) {
    i32 signal = NuFParGetInt(parser);
    TEXTURE_ANIM_PROGRAM *p = TextureAnimationParseProgramPrivate;
    p->on_signal[signal] = p->instruction_count;
    p->on_mask |= 1u << signal;
}

static void TextureAnimationOffPrivate(NUFPAR *parser) {
    i32 signal = NuFParGetInt(parser);
    TEXTURE_ANIM_PROGRAM *p = TextureAnimationParseProgramPrivate;
    p->off_signal[signal] = p->instruction_count;
    p->off_mask |= 1u << signal;
}

static void TextureAnimationLabelPrivate(NUFPAR *parser) {
    i32 label;
    NuFParGetWord(parser);
    label = TextureAnimationLabelIndexPrivate(parser->word_buffer_ptr);
    TextureAnimationLabelOffsetsPrivate[label] = TextureAnimationParseProgramPrivate->instruction_count;
}

static void TextureAnimationGotoPrivate(NUFPAR *parser) {
    i32 label;
    TEXTURE_ANIM_PROGRAM *p;
    NuFParGetWord(parser);
    label = TextureAnimationLabelIndexPrivate(parser->word_buffer_ptr);
    p = TextureAnimationParseProgramPrivate;
    p->instructions[p->instruction_count++] = TEXANIM_OP_GOTO;
    p->instructions[p->instruction_count++] = (i16)label;
}

static void TextureAnimationGosubPrivate(NUFPAR *parser) {
    i32 label;
    TEXTURE_ANIM_PROGRAM *p;
    NuFParGetWord(parser);
    label = TextureAnimationLabelIndexPrivate(parser->word_buffer_ptr);
    p = TextureAnimationParseProgramPrivate;
    p->instructions[p->instruction_count++] = TEXANIM_OP_GOSUB;
    p->instructions[p->instruction_count++] = (i16)label;
}

static void TextureAnimationBranchPrivate(NUFPAR *parser) {
    i32 comparison = TextureAnimationReadComparisonPrivate(parser);
    i32 value = NuFParGetInt(parser);
    i32 label;
    TEXTURE_ANIM_PROGRAM *p;
    NuFParGetWord(parser);
    label = TextureAnimationLabelIndexPrivate(parser->word_buffer_ptr);
    p = TextureAnimationParseProgramPrivate;
    p->instructions[p->instruction_count++] = TEXANIM_OP_BTEX;
    p->instructions[p->instruction_count++] = (i16)comparison;
    p->instructions[p->instruction_count++] = (i16)value;
    p->instructions[p->instruction_count++] = (i16)label;
}

static void TextureAnimationReturnPrivate(NUFPAR *parser) {
    TextureAnimationParseProgramPrivate->instructions[TextureAnimationParseProgramPrivate->instruction_count++] = TEXANIM_OP_RET;
}

static void TextureAnimationRepeatPrivate(NUFPAR *parser) {
    i32 value = NuFParGetInt(parser);
    i32 random;
    TEXTURE_ANIM_PROGRAM *p;
    if (value == 0) {
        value = 0x7fffffff;
    }
    random = NuFParGetInt(parser);
    p = TextureAnimationParseProgramPrivate;
    p->instructions[p->instruction_count++] = TEXANIM_OP_REPEAT;
    p->instructions[p->instruction_count++] = (i16)value;
    p->instructions[p->instruction_count++] = (i16)random;
}

static void TextureAnimationRepeatEndPrivate(NUFPAR *parser) {
    TextureAnimationParseProgramPrivate->instructions[TextureAnimationParseProgramPrivate->instruction_count++] = TEXANIM_OP_REPEND;
}

static void TextureAnimationUntilPrivate(NUFPAR *parser) {
    i32 comparison = TextureAnimationReadComparisonPrivate(parser);
    i32 value = NuFParGetInt(parser);
    TEXTURE_ANIM_PROGRAM *p = TextureAnimationParseProgramPrivate;
    p->instructions[p->instruction_count++] = TEXANIM_OP_UNTILTEX;
    p->instructions[p->instruction_count++] = (i16)comparison;
    p->instructions[p->instruction_count++] = (i16)value;
}

static void TextureAnimationEndPrivate(NUFPAR *parser) {
    TextureAnimationParseProgramPrivate->instructions[TextureAnimationParseProgramPrivate->instruction_count++] = TEXANIM_OP_END;
}

static void TextureAnimationScriptNamePrivate(NUFPAR *parser) {
    NuFParGetWord(parser);
    parser->word_buffer_ptr[32] = 0;
    strcpy(TextureAnimationParseProgramPrivate->name, parser->word_buffer_ptr);
}

static void TextureAnimationExternalDefinitionPrivate(NUFPAR *parser) {
    i32 label;
    TEXTURE_ANIM_PROGRAM *p;
    NuFParGetWord(parser);
    label = TextureAnimationExternalLabelIndexPrivate(parser->word_buffer_ptr);
    p = TextureAnimationParseProgramPrivate;
    p->label_ids[p->label_count] = (i16)label;
    p->label_offsets[p->label_count] = p->instruction_count;
    p->label_count++;
}

static void TextureAnimationExternalReferencePrivate(NUFPAR *parser) {
    i32 label;
    TEXTURE_ANIM_PROGRAM *p;
    NuFParGetWord(parser);
    label = TextureAnimationExternalLabelIndexPrivate(parser->word_buffer_ptr);
    p = TextureAnimationParseProgramPrivate;
    p->instructions[p->instruction_count++] = TEXANIM_OP_XREF;
    p->instructions[p->instruction_count++] = (i16)label;
}

u16 script_mask = 0xffff;
static NUFPCOMJMP TextureAnimationCommandsPrivate[] = {{"scriptmask", TextureAnimationScriptMaskPrivate},
                                                       {"tex", TextureAnimationTexPrivate},
                                                       {"texr", TextureAnimationTexRandomPrivate},
                                                       {"texadj", TextureAnimationTexAdjustPrivate},
                                                       {"texadjr", TextureAnimationTexAdjustRandomPrivate},
                                                       {"wait", TextureAnimationWaitPrivate},
                                                       {"rate", TextureAnimationRatePrivate},
                                                       {"on", TextureAnimationOnPrivate},
                                                       {"off", TextureAnimationOffPrivate},
                                                       {"label", TextureAnimationLabelPrivate},
                                                       {"goto", TextureAnimationGotoPrivate},
                                                       {"btex", TextureAnimationBranchPrivate},
                                                       {"gosub", TextureAnimationGosubPrivate},
                                                       {"ret", TextureAnimationReturnPrivate},
                                                       {"repeat", TextureAnimationRepeatPrivate},
                                                       {"repend", TextureAnimationRepeatEndPrivate},
                                                       {"untiltex", TextureAnimationUntilPrivate},
                                                       {"end", TextureAnimationEndPrivate},
                                                       {"scriptname", TextureAnimationScriptNamePrivate},
                                                       {"xdef", TextureAnimationExternalDefinitionPrivate},
                                                       {"xref", TextureAnimationExternalReferencePrivate},
                                                       {NULL, NULL}};
i32 nutexanim_usepakfile = 1;

void NuTexAnimProgInit(TEXTURE_ANIM_PROGRAM *program) {
    i32 i;
    if (program == NULL) {
        return;
    }
    for (i = 0; i < 32; ++i) {
        program->off_signal[i] = -1;
        program->on_signal[i] = -1;
    }
    program->instruction_count = 0;
    program->on_mask = 0;
    program->off_mask = 0;
    program->name[0] = 0;
    program->label_count = 0;
    *(i16 *)&program->flags &= ~1;
    program->mask = 1;
}

TEXTURE_ANIM_PROGRAM *NuTexAnimProgCreate(VARIPTR *buffer, i32 extra_bytes, const char *name) {
    TEXTURE_ANIM_PROGRAM *program;
    if (buffer != NULL) {
        program = (TEXTURE_ANIM_PROGRAM *)buffer->addr;
        buffer->addr += sizeof(*program) + extra_bytes;
    } else {
        program = NuAlloc(sizeof(*program) + extra_bytes, "d:\\projects\\game_saga\\nu2api.saga\\nu3d\\nutexanm.c", 386);
    }
    if (program != NULL) {
        NuTexAnimProgInit(program);
        if (buffer == NULL) {
            program->flags |= 1;
        }
        if (name != NULL) {
            NuStrCpy(program->name, name);
        }
    }
    return program;
}

void NuTexAnimProgWrite(char *path, TEXTURE_ANIM_PROGRAM *program) {
    NUFILE file = NuFileOpen(path, 1);
    if (file != 0) {
        NuFileWrite(file, program, sizeof(*program) + program->instruction_count * 2);
        NuFileClose(file);
    }
}

TEXTURE_ANIM_PROGRAM *NuTexAnimProgRead(VARIPTR *buffer, char *path) {
    TEXTURE_ANIM_PROGRAM *program = NULL;
    TEXTURE_ANIM_PROGRAM *head;
    NUFILE file = NuFileOpen(path, 0);
    if (file != 0) {
        u32 size = NuFileOpenSize(file);
        if (buffer != NULL) {
            program = (TEXTURE_ANIM_PROGRAM *)((buffer->addr + 3) & ~3u);
            buffer->addr = (u32)program + size;
        } else {
            program = NuAlloc(size, "d:\\projects\\game_saga\\nu2api.saga\\nu3d\\nutexanm.c", 444);
        }
        NuFileRead(file, program, size);
        NuFileClose(file);
        head = GHGTextureProgramsPC;
        program->next = head;
        if (head != NULL) {
            head->previous = program;
        }
        program->previous = NULL;
        GHGTextureProgramsPC = program;
        if (buffer != NULL) {
            program->flags &= ~1u;
        } else {
            program->flags |= 1;
        }
    }
    return program;
}

void NuTexAnimProgAssembleEnd(TEXTURE_ANIM_PROGRAM *program) {
    i16 index = 0;
    while (index < program->instruction_count) {
        switch (program->instructions[index]) {
            case TEXANIM_OP_TEX:
                index += 2;
                break;
            case TEXANIM_OP_TEXR:
                index += 2;
                break;
            case TEXANIM_OP_TEXADJ:
                index += 4;
                break;
            case TEXANIM_OP_TEXADJR:
                index += 5;
                break;
            case TEXANIM_OP_WAIT:
                index += 3;
                break;
            case TEXANIM_OP_RATE:
                index += 3;
                break;
            case TEXANIM_OP_GOTO:
                program->instructions[index + 1] = (i16)TextureAnimationLabelOffsetsPrivate[program->instructions[index + 1]];
                index += 2;
                break;
            case TEXANIM_OP_GOSUB:
                program->instructions[index + 1] = (i16)TextureAnimationLabelOffsetsPrivate[program->instructions[index + 1]];
                index += 2;
                break;
            case TEXANIM_OP_BTEX:
                program->instructions[index + 3] = (i16)TextureAnimationLabelOffsetsPrivate[program->instructions[index + 3]];
                index += 4;
                break;
            case TEXANIM_OP_RET:
                ++index;
                break;
            case TEXANIM_OP_REPEAT:
                index += 3;
                break;
            case TEXANIM_OP_REPEND:
                ++index;
                break;
            case TEXANIM_OP_UNTILTEX:
                index += 3;
                break;
            case TEXANIM_OP_END:
                ++index;
                break;
            case TEXANIM_OP_XREF:
                index += 2;
                break;
        }
    }
}

void NuTexAnimEnvDestroy(PC_TEXTURE_ANIM_ENV *environment) {
    if ((environment->flags & 1) != 0) {
        NuFree(environment, "d:\\projects\\game_saga\\nu2api.saga\\nu3d\\nutexanm.c", 520);
    }
}

TEXTURE_ANIM_PROGRAM *NuTexAnimProgParseFile(NUFILE file, VARIPTR *buffer, VARIPTR end, i32 fps) {
    TEXTURE_ANIM_PROGRAM *program;
    TEXTURE_ANIM_PROGRAM *head;
    NUFPAR *parser;
    i32 length;
    TEXTURE_ANIM_PROGRAM *result;
    if (buffer != NULL) {
        program = (TEXTURE_ANIM_PROGRAM *)((buffer->addr + 3) & ~3u);
    } else {
        program = NuAlloc(1024, "d:\\projects\\game_saga\\nu2api.saga\\nu3d\\nutexanm.c", 962);
        end.addr = (u32)program + 1023;
    }
    memset(TextureAnimationLabelsPrivate, 0, sizeof(TextureAnimationLabelsPrivate));
    TextureAnimationLabelCountPrivate = 0;
    result = NULL;
    parser = NuFParOpen(file);
    if (parser != NULL) {
        NuFParPushCom(parser, TextureAnimationCommandsPrivate);
        if ((u32)(program + 1) >= end.addr) {
            AnimationDiagnosticNoOp();
        }
        NuTexAnimProgInit(program);
        TextureAnimationParseProgramPrivate = program;
        while (NuFParGetLine(parser) != 0) {
            length = NuFParGetWord(parser);
            if (length != 0 && NuFParInterpretWord(parser) == 0 && parser->word_buffer_ptr[0] != 0 &&
                parser->word_buffer_ptr[length - 1] == ':') {
                i32 label;
                parser->word_buffer_ptr[length - 1] = 0;
                label = TextureAnimationLabelIndexPrivate(parser->word_buffer_ptr);
                TextureAnimationLabelOffsetsPrivate[label] = TextureAnimationParseProgramPrivate->instruction_count;
            }
            if ((u32)(program->instructions + program->instruction_count) >= end.addr) {
                AnimationDiagnosticNoOp();
            }
        }
        if (buffer != NULL) {
            buffer->addr = (u32)(program->instructions + program->instruction_count);
        }
        NuFreeFPar(parser);
        NuTexAnimProgAssembleEnd(program);
        head = GHGTextureProgramsPC;
        program->next = head;
        if (head != NULL) {
            head->previous = program;
        }
        program->previous = NULL;
        GHGTextureProgramsPC = program;
        result = program;
    }
    return result;
}

TEXTURE_ANIM_PROGRAM *NuTexAnimProgReadScript(char *path, VARIPTR *buffer, VARIPTR end, i32 fps) {
    TEXTURE_ANIM_PROGRAM *program = NULL;
    NUFILE file = NuFileOpen(path, 0);
    if (file != 0) {
        program = NuTexAnimProgParseFile(file, buffer, end, fps);
        NuFileClose(file);
    }
    return program;
}

void NuTexAnimProgReadCFG(char *path, VARIPTR *buffer, VARIPTR end) {
    char filename[128];
    VARIPTR cursor;
    VARIPTR list;
    void *pak;
    void *data;
    i32 size;
    i32 item;
    NUFILE file;
    NUFPAR *parser;
    char *name;
    if (nutexanim_usepakfile != 0) {
        NuStrCpy(filename, path);
        name = NuStrRChr(filename, '.');
        if (name != NULL) {
            *name = '\0';
        }
        NuStrCat(filename, ".pak");
        size = (i32)NuFileSize(filename);
        if (size == 0) {
            return;
        }
        list.addr = (end.addr - size) & ~15u;
        cursor = list;
        pak = NuFilePakLoad(filename, &cursor, end, 16);
        if (pak != NULL) {
            name = NuStrRChr(path, '/');
            if (name == NULL && (name = NuStrRChr(path, '\\')) == NULL) {
                name = path;
            } else {
                ++name;
            }
            NuStrCpy(filename, name);
            name = NuStrRChr(filename, '.');
            if (name != NULL) {
                *name = '\0';
            }
            NuStrCat(filename, ".cfg");
            item = NuFilePakGetItem(pak, filename);
            if (item == 0) {
                ((void (*)(const void *))AnimationDiagnosticNoOp)(filename);
            }
            NuFilePakGetItemInfo(pak, item, &data, &size);
            file = NuMemFileOpen(data, size, 0);
            if (file != 0) {
                parser = NuFParOpen(file);
                if (parser != NULL) {
                    *--list.char_ptr = '\0';
                    while (NuFParGetLine(parser)) {
                        NuFParGetWord(parser);
                        if (parser->word_buffer_ptr[0] == '\0') {
                            ((void (*)(const void *))AnimationDiagnosticNoOp)((void *)parser->line_number);
                        }
                        name = NuStrRChr(parser->word_buffer_ptr, '/');
                        if (name == NULL && (name = NuStrRChr(parser->word_buffer_ptr, '\\')) == NULL) {
                            name = parser->word_buffer_ptr;
                        } else {
                            ++name;
                        }
                        list.char_ptr -= NuStrLen(name) + 1;
                        NuStrCpy(list.char_ptr, name);
                    }
                    NuFreeFPar(parser);
                }
                NuFileClose(file);
            }
            for (cursor = list; *cursor.char_ptr != '\0'; cursor.char_ptr += NuStrLen(cursor.char_ptr) + 1) {
                item = NuFilePakGetItem(pak, cursor.char_ptr);
                if (item == 0) {
                    ((void (*)(const void *))AnimationDiagnosticNoOp)(cursor.char_ptr);
                }
                NuFilePakGetItemInfo(pak, item, &data, &size);
                file = NuMemFileOpen(data, size, 0);
                if (file != 0) {
                    if (NuTexAnimProgParseFile(file, buffer, list, 0) == NULL) {
                        PCSpecialEmptyHandler(cursor.char_ptr);
                    }
                    NuFileClose(file);
                }
            }
            return;
        }
        if (NuTexAnimMissingPakDiagnostic != 0) {
            ((void (*)(const void *))AnimationDiagnosticNoOp)(path);
        } else {
            PCSpecialEmptyHandler(path);
        }
        buffer->addr = (buffer->addr + 15) & ~15u;
    }
    *end.char_ptr = '\0';
    list.char_ptr = end.char_ptr - 1;
    parser = NuFParCreate(path);
    if (parser != NULL) {
        *list.char_ptr = '\0';
        while (NuFParGetLine(parser)) {
            NuFParGetWord(parser);
            if (parser->word_buffer_ptr[0] == '\0') {
                ((void (*)(const void *))AnimationDiagnosticNoOp)((void *)parser->line_number);
            }
            list.char_ptr -= NuStrLen(parser->word_buffer_ptr) + 1;
            NuStrCpy(list.char_ptr, parser->word_buffer_ptr);
        }
        NuFParDestroy(parser);
    }
    for (cursor = list; *cursor.char_ptr != '\0'; cursor.char_ptr += NuStrLen(cursor.char_ptr) + 1) {
        if (NuTexAnimProgReadScript(cursor.char_ptr, buffer, list, 0) == NULL) {
            PCSpecialEmptyHandler(cursor.char_ptr);
        }
    }
    buffer->addr = (buffer->addr + 15) & ~15u;
}

TEXTURE_ANIMATION *NuTexAnimCreate(VARIPTR *buffer, TEXTURE_ANIM_PROGRAM *program, NUMTL *material, u16 *texture_ids, i32 texture_count) {
    TEXTURE_ANIMATION *animation;
    if (buffer != NULL) {
        animation = (TEXTURE_ANIMATION *)((buffer->addr + 3) & ~3u);
        buffer->addr = (u32)(animation + 1) + texture_count * 2;
    } else {
        animation = NuAlloc(sizeof(*animation) + texture_count * 2, "d:\\projects\\game_saga\\nu2api.saga\\nu3d\\nutexanm.c", 1720);
    }
    animation->previous = NULL;
    animation->next = NULL;
    animation->texture_ids = (u16 *)(animation + 1);
    animation->texture_count = (i16)texture_count;
    animation->material = material;
    animation->name = NULL;
    memcpy(animation->texture_ids, texture_ids, texture_count * 2);
    animation->environment = NuTexAnimEnvCreate(buffer, material, animation->texture_ids, program);
    return animation;
}

void NuTexAnimProcessEx(f32 frame_time, u16 mask) {
    PC_TEXTURE_ANIM_LIST *list;
    AnimationDiagnosticNoOp();
    nta_iframetime = (i32)(frame_time * 4096.0);
    nta_script_mask = mask;
    for (list = ntal_first; list != NULL; list = list->next) {
        TEXTURE_ANIMATION *animation = list->first;
        while (animation != NULL) {
            if (animation->environment != NULL) {
                NuTexAnimEnvProc(animation->environment);
            }
            animation = animation->next;
        }
    }
}

void NuTexAnimEnvReset(PC_TEXTURE_ANIM_ENV *environment) {
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
