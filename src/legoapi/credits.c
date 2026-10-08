#include "saga.h"
#include <stdio.h>

struct credit_record_pc_s {
    char *text;
    f32 x;
    f32 y;
    f32 scale;
    u8 colour[4];
    u8 alignment;
    i8 type;
    u8 reserved[2];
};
typedef struct credit_type_pc_s {
    char *name;
    f32 scale;
    f32 width;
    u8 default_colour[4];
    u8 colour[4];
    u8 reserved[3];
    i8 type;
} CREDIT_TYPE_PC;
STATIC_ASSERT(sizeof(CREDIT_RECORD_PC) == 24);
STATIC_ASSERT(offsetof(CREDIT_RECORD_PC, text) == 0);
STATIC_ASSERT(offsetof(CREDIT_RECORD_PC, x) == 4);
STATIC_ASSERT(offsetof(CREDIT_RECORD_PC, y) == 8);
STATIC_ASSERT(offsetof(CREDIT_RECORD_PC, scale) == 12);
STATIC_ASSERT(offsetof(CREDIT_RECORD_PC, colour) == 16);
STATIC_ASSERT(offsetof(CREDIT_RECORD_PC, alignment) == 20);
STATIC_ASSERT(offsetof(CREDIT_RECORD_PC, type) == 21);
STATIC_ASSERT(sizeof(CREDIT_TYPE_PC) == 24);
STATIC_ASSERT(offsetof(CREDIT_TYPE_PC, name) == 0);
STATIC_ASSERT(offsetof(CREDIT_TYPE_PC, scale) == 4);
STATIC_ASSERT(offsetof(CREDIT_TYPE_PC, width) == 8);
STATIC_ASSERT(offsetof(CREDIT_TYPE_PC, default_colour) == 12);
STATIC_ASSERT(offsetof(CREDIT_TYPE_PC, colour) == 16);
STATIC_ASSERT(offsetof(CREDIT_TYPE_PC, type) == 23);

extern CREDIT_RECORD_PC *CreditList;
extern f32 CreditsAlpha;
extern f32 CreditsTime;
extern f32 CreditsFinishedTime;
extern i32 CreditsFlag;
static f32 CreditsDurationPc = 120.0f;
static f32 CreditsSizePc;
static i32 CreditCountPc;
static CREDIT_TYPE_PC CreditTypePc[5] = {
    {"heading", 0.4f, 1.65f, {255, 255, 255, 128}, {0, 0, 0, 0}, {0, 0, 0}, 0},
    {"company", 0.6f, 1.65f, {255, 255, 255, 128}, {0, 0, 0, 0}, {0, 0, 0}, 0},
    {"title", 0.35f, 0.825f, {255, 255, 255, 128}, {0, 0, 0, 0}, {0, 0, 0}, 0},
    {"name", 0.4f, 0.825f, {255, 255, 255, 128}, {0, 0, 0, 0}, {0, 0, 0}, 0},
    {"credit", 0.45f, 1.65f, {255, 255, 255, 128}, {0, 0, 0, 0}, {0, 0, 0}, 0},
};

static CREDIT_TYPE_PC *CreditsFindTypePc(char *word) {
    i32 index;
    for (index = 0; index < 5; ++index) {
        if (NuStrICmp(CreditTypePc[index].name, word) == 0)
            return &CreditTypePc[index];
    }
    return NULL;
}

static void CreditsReadColourPc(NUFPAR_PC *parser, u8 *colour, i32 maximum) {
    f32 value;
    if (NuFParGetWord(parser)) {
        value = NuAToF(parser->word_buffer_ptr);
        if (value > 1.0f)
            value = 1.0f;
        else if (value < 0.0f)
            value = 0.0f;
        if (maximum > 255)
            maximum = 255;
        else if (maximum < 0)
            maximum = 0;
        *colour = (u8)(i32)(value * maximum);
    }
}

static void CreditsInitCreditPc(CREDIT_RECORD_PC *credit, char *text, f32 x, f32 y, f32 scale, u8 red, u8 green,
                                u8 blue, u8 alpha, u8 alignment, VARIPTR *buffer, VARIPTR *end) {
    char *copy = NULL;
    i32 length = NuStrLen(text);
    if (length != 0) {
        copy = (char *)GameBufferAlloc(buffer, end, length + 1);
        NuStrCpy(copy, text);
    }
    credit->text = copy;
    credit->x = x;
    credit->y = y;
    credit->scale = scale;
    credit->colour[0] = red;
    credit->colour[1] = green;
    credit->colour[2] = blue;
    credit->colour[3] = alpha;
    credit->alignment = alignment;
}

void __cdecl Credits_Load(WORLDINFO *world, VARIPTR *buffer, VARIPTR *end) {
    char filename[256];
    CREDIT_RECORD_PC *credit;
    CREDIT_RECORD_PC *previous = NULL;
    NUFPAR_PC *parser;
    CREDIT_TYPE_PC *type;
    f32 y = 0.0f;
    i32 index;
    i32 component;
    CreditsSizePc = 0.0f;
    CreditsDurationPc = 120.0f;
    for (index = 0; index < 5; ++index) {
        for (component = 0; component < 4; ++component)
            CreditTypePc[index].colour[component] = CreditTypePc[index].default_colour[component];
        CreditTypePc[index].type = (i8)index;
    }
    NuStrCpy(filename, "stuff\\text\\");
    NuStrCat(filename, "english");
    NuStrCat(filename, "_credits.txt");
    CreditList = (CREDIT_RECORD_PC *)GameBufferAlloc(buffer, end, 24000);
    CreditCountPc = 0;
    if (CreditList == NULL)
        return;
    parser = NuFParCreate(filename);
    if (parser == NULL)
        return;
    credit = CreditList;
    while (NuFParGetLine(parser)) {
        if (NuFParGetWord(parser)) {
            if (NuStrICmp(parser->word_buffer_ptr, "duration") == 0) {
                if (NuFParGetWord(parser) && CreditsDurationPc == 120.0) {
                    f32 duration = NuAToF(parser->word_buffer_ptr);
                    if (duration > 0.0f)
                        CreditsDurationPc = duration;
                }
            } else {
                type = CreditsFindTypePc(parser->word_buffer_ptr);
                if (type != NULL && NuFParGetWord(parser)) {
                    if (NuStrICmp(parser->word_buffer_ptr, "colour") == 0) {
                        CreditsReadColourPc(parser, &type->colour[0], 255);
                        CreditsReadColourPc(parser, &type->colour[1], 255);
                        CreditsReadColourPc(parser, &type->colour[2], 255);
                        CreditsReadColourPc(parser, &type->colour[3], 128);
                    } else if (CreditCountPc < 1000) {
                        credit->type = type->type;
                        switch (type->type) {
                            case 0:
                                if (previous == NULL || previous->type != 1)
                                    y = (f32)(y + CreditTypePc[0].scale / CreditTypePc[1].scale * (f64)0.2f);
                                CreditsInitCreditPc(credit, parser->word_buffer_ptr, 0.0f, y, type->scale,
                                                    type->colour[0], type->colour[1], type->colour[2], type->colour[3],
                                                    1, buffer, end);
                                y = (f32)(y + CreditTypePc[0].scale / CreditTypePc[1].scale * (f64)0.2f);
                                y = (f32)(y + (f64)0.1f);
                                break;
                            case 1:
                                if (previous != NULL && previous->type != 0)
                                    y = (f32)(y + CreditTypePc[1].scale / CreditTypePc[1].scale * (f64)0.2f);
                                CreditsInitCreditPc(credit, parser->word_buffer_ptr, 0.0f, y, type->scale,
                                                    type->colour[0], type->colour[1], type->colour[2], type->colour[3],
                                                    1, buffer, end);
                                y = (f32)(y + CreditTypePc[1].scale / CreditTypePc[1].scale * (f64)0.2f);
                                break;
                            case 2:
                                CreditsInitCreditPc(credit, parser->word_buffer_ptr, -0.015f, y, type->scale,
                                                    type->colour[0], type->colour[1], type->colour[2], type->colour[3],
                                                    9, buffer, end);
                                break;
                            case 3:
                                CreditsInitCreditPc(credit, parser->word_buffer_ptr, 0.015f, y, type->scale,
                                                    type->colour[0], type->colour[1], type->colour[2], type->colour[3],
                                                    3, buffer, end);
                                y = (f32)(y + CreditTypePc[3].scale / CreditTypePc[1].scale * (f64)0.2f);
                                break;
                            case 4:
                                CreditsInitCreditPc(credit, parser->word_buffer_ptr, 0.0f, y, type->scale,
                                                    type->colour[0], type->colour[1], type->colour[2], type->colour[3],
                                                    1, buffer, end);
                                y = (f32)(y + CreditTypePc[4].scale / CreditTypePc[1].scale * (f64)0.2f);
                                break;
                        }
                        previous = credit;
                        ++credit;
                        ++CreditCountPc;
                    }
                }
            }
        }
        CreditsSizePc = (f32)(y + (f64)2.2f);
    }
    NuFParDestroy(parser);
}

void __cdecl Credits_Init(WORLDINFO *world) {
    CreditsAlpha = 1.0f;
    CreditsTime = 0.0f;
    CreditsFinishedTime = 0.0f;
    CreditsFlag = 0;
    if (LastLData != STATUS_LDATA)
        BackDrop_ResetColours();
}

void __cdecl Credits_GetInfo(f32 *duration, i32 *flag, f32 *alpha) {
    if (duration != NULL)
        *duration = CreditsDurationPc;
    if (flag != NULL)
        *flag = CreditsFlag;
    if (alpha != NULL)
        *alpha = CreditsAlpha;
}

typedef struct credits_completion_reader_pc_s {
    u16 points;
    u8 unknown_02;
    u8 flags;
} CREDITS_COMPLETION_READER_PC;
STATIC_ASSERT(sizeof(CREDITS_COMPLETION_READER_PC) == 4);
STATIC_ASSERT(offsetof(CREDITS_COMPLETION_READER_PC, points) == 0);
STATIC_ASSERT(offsetof(CREDITS_COMPLETION_READER_PC, flags) == 3);

static void CreditsDrawListPc(CREDIT_RECORD_PC *credit, f32 time) {
    char percentage[32];
    f32 x;
    i32 alpha;
    i32 points;
    if (CreditsFlag == 0) {
        f32 offset = time / CreditsDurationPc * CreditsSizePc;
        while (credit->text != NULL) {
            if (offset >= credit->y && offset <= credit->y + (f64)2.2f) {
                alpha = (i32)(credit->colour[3] * CreditsAlpha);
                SmartTextEx(credit->text, credit->x, (f32)(-1.0 - (credit->y - offset)), 1.0f, credit->scale,
                            credit->scale, credit->scale, credit->alignment, credit->colour[0], credit->colour[1],
                            credit->colour[2], CreditTypePc[credit->type].width, 1, NULL, 0, (u32)alpha);
            }
            ++credit;
        }
    } else {
        CREDITS_COMPLETION_READER_PC *completion = (CREDITS_COMPLETION_READER_PC *)CustomiserCompletionSavePc;
        points = completion != NULL ? completion->points : 0;
        sprintf(percentage, "%.1f%%", (f64)(points * 100) / HubCompletionPointsPc);
        x = 0.0f;
        if (CreditsFlag == 1)
            x = (f32)(CreditsAlpha * -(f64)0.1f + (f64)0.1f);
        else if (CreditsFlag == 3)
            x = (f32)(0.0 - (1.0 - CreditsAlpha) * (f64)0.1f);
        alpha = (i32)((i32)(CreditsAlpha * 128.0) * 0.25);
        SmartTextEx(percentage, x, 0.0f, 1.0f, 1.75f, 1.75f, 1.75f, 0, 255, 191, 0, 1.7f, 1, NULL, 0, (u32)alpha);
    }
}

void __cdecl Credits_DrawPanel(WORLDINFO *world) {
    CreditsDrawListPc(CreditList, CreditsTime);
}
