#include "saga.h"
#include <stdio.h>

struct credit_record_pc_s { // size 0x18
    char *text;             // 0x00
    f32 x;                  // 0x04
    f32 y;                  // 0x08
    f32 scale;              // 0x0c
    u8 colour[4];           // 0x10
    u8 alignment;           // 0x14
    i8 type;                // 0x15
    u8 reserved[2];         // 0x16
};

typedef struct credit_type_pc_s { // size 0x18
    char *name;                   // 0x00
    f32 scale;                    // 0x04
    f32 width;                    // 0x08
    u8 default_colour[4];         // 0x0c
    u8 colour[4];                 // 0x10
    u8 reserved[3];               // 0x14
    i8 type;                      // 0x17
} CREDIT_TYPE_PC;

static f32 Credits_Duration = 120.0f;
static f32 CreditsSize = 0;
static i32 CreditCount = 0;
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
        if (NuStrICmp(CreditTypePc[index].name, word) == 0) {
            return &CreditTypePc[index];
        }
    }
    return NULL;
}

static void CreditsReadColourPc(NUFPAR_PC *parser, u8 *colour, i32 maximum) {
    f32 value;
    if (NuFParGetWord(parser)) {
        value = NuAToF(parser->word_buffer_ptr);
        if (value > 1.0f) {
            value = 1.0f;
        } else if (value < 0.0f) {
            value = 0.0f;
        }
        if (maximum > 255) {
            maximum = 255;
        } else if (maximum < 0) {
            maximum = 0;
        }
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

void Credits_Load(WORLDINFO *world, VARIPTR *buffer, VARIPTR *end) {
    char filename[256];
    CREDIT_RECORD_PC *credit;
    CREDIT_RECORD_PC *previous;
    NUFPAR_PC *parser;
    CREDIT_TYPE_PC *type;
    f32 y;
    i32 index;
    CreditsSize = 0.0f;
    Credits_Duration = 120.0f;
    for (index = 0; index < 5; ++index) {
        CreditTypePc[index].type = (i8)index;
        CreditTypePc[index].colour[0] = CreditTypePc[index].default_colour[0];
        CreditTypePc[index].colour[1] = CreditTypePc[index].default_colour[1];
        CreditTypePc[index].colour[2] = CreditTypePc[index].default_colour[2];
        CreditTypePc[index].colour[3] = CreditTypePc[index].default_colour[3];
    }
    NuStrCpy(filename, "stuff\\text\\");
    NuStrCat(filename, "english");
    NuStrCat(filename, "_credits.txt");
    CreditList = (CREDIT_RECORD_PC *)GameBufferAlloc(buffer, end, 24000);
    credit = CreditList;
    CreditCount = 0;
    previous = NULL;
    if (credit == NULL) {
        return;
    }
    parser = NuFParCreate(filename);
    if (parser == NULL) {
        return;
    }
    y = 0.0f;
    while (NuFParGetLine(parser)) {
        if (NuFParGetWord(parser)) {
            if (NuStrICmp(parser->word_buffer_ptr, "duration") == 0) {
                if (NuFParGetWord(parser) && Credits_Duration == 120.0) {
                    f32 duration = NuAToF(parser->word_buffer_ptr);
                    if (duration > 0.0f) {
                        Credits_Duration = duration;
                    }
                }
            } else {
                type = CreditsFindTypePc(parser->word_buffer_ptr);
                if (type != NULL && NuFParGetWord(parser)) {
                    if (NuStrICmp(parser->word_buffer_ptr, "colour") == 0) {
                        CreditsReadColourPc(parser, &type->colour[0], 255);
                        CreditsReadColourPc(parser, &type->colour[1], 255);
                        CreditsReadColourPc(parser, &type->colour[2], 255);
                        CreditsReadColourPc(parser, &type->colour[3], 128);
                    } else if (CreditCount < 1000) {
                        credit->type = type->type;
                        switch (credit->type) {
                            case 0:
                                if (previous == NULL || previous->type != 1) {
                                    y = (f32)(y + CreditTypePc[0].scale / CreditTypePc[1].scale * (f64)0.2f);
                                }
                                CreditsInitCreditPc(credit, parser->word_buffer_ptr, 0.0f, y, type->scale,
                                                    type->colour[0], type->colour[1], type->colour[2], type->colour[3],
                                                    1, buffer, end);
                                y = (f32)(y + CreditTypePc[0].scale / CreditTypePc[1].scale * (f64)0.2f);
                                y = (f32)(y + (f64)0.1f);
                                break;
                            case 1:
                                if (previous != NULL && previous->type != 0) {
                                    y = (f32)(y + CreditTypePc[1].scale / CreditTypePc[1].scale * (f64)0.2f);
                                }
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
                        ++CreditCount;
                    }
                }
            }
        }
        CreditsSize = (f32)(y + (f64)2.2f);
    }
    NuFParDestroy(parser);
}

void Credits_Init(WORLDINFO *world) {
    CreditsAlpha = 1.0f;
    CreditsTime = 0.0f;
    CreditsFinishedTime = 0.0f;
    CreditsFlag = 0;
    if (LastLData != STATUS_LDATA) {
        BackDrop_ResetColours();
    }
}

void Credits_GetInfo(f32 *duration, i32 *flag, f32 *alpha) {
    if (duration != NULL) {
        *duration = Credits_Duration;
    }
    if (flag != NULL) {
        *flag = CreditsFlag;
    }
    if (alpha != NULL) {
        *alpha = CreditsAlpha;
    }
}

extern "C" void fun_0059d750() {
}

typedef struct credits_completion_reader_pc_s { // size 0x4
    u16 points;                                 // 0x0
    u8 unknown_02;                              // 0x2
    u8 flags;                                   // 0x3
} CREDITS_COMPLETION_READER_PC;

static void CreditsDrawListPc(CREDIT_RECORD_PC *credit, f32 time) {
    char percentage[32];
    f32 x;
    i32 alpha;
    i32 points;
    if (CreditsFlag == 0) {
        f32 offset = time / Credits_Duration * CreditsSize;
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
        if (CreditsFlag == 1) {
            x = (f32)((f64)0.1f + (x - (f64)0.1f) * CreditsAlpha);
        } else if (CreditsFlag == 3) {
            x = (f32)(x - (1.0 - CreditsAlpha) * (f64)0.1f);
        }
        alpha = (i32)((i32)(CreditsAlpha * 128.0) * 0.25);
        SmartTextEx(percentage, x, 0.0f, 1.0f, 1.75f, 1.75f, 1.75f, 0, 255, 191, 0, 1.7f, 1, NULL, 0, (u32)alpha);
    }
}

void Credits_DrawPanel(WORLDINFO *world) {
    CreditsDrawListPc(CreditList, CreditsTime);
}

void legoSetMusicVolume(f32);
extern "C" void MusicClearAll(void);
extern "C" void SoundKillAll(void);

extern "C" void Credits_UpdateMenu(MENU *menu) {
    f32 volume = Game_OptionsSave != NULL ? GameSetMusicVolume(Game_OptionsSave) : 1.0f;
    f32 previous_time = CreditsTime;
    if (CreditsFlag != 0) {
        f32 progress;
        legoSetMusicVolume(0.0f);
        CreditsTime += FRAMETIME;
        switch (CreditsFlag) {
            case 1:
                if (previous_time < (f64)0.1f && CreditsTime >= (f64)0.1f) {
                    music_man.StopAll(0);
                    MusicClearAll();
                    SoundKillAll();
                }
                if (CreditsTime >= 1.0f) {
                    CreditsFlag = 2;
                    GameAudio_PlaySfx(0x2b, NULL, 0, 0);
                    progress = 1.0f;
                } else {
                    progress = CreditsTime;
                }
                break;
            case 2:
                if (CreditsTime >= 3.0f) {
                    CreditsFlag = 3;
                    CreditsTime = 0.0f;
                }
                progress = 1.0f;
                break;
            default:
                if (CreditsTime >= 1.1f) {
                    FADETYPE fade_type;
                    NewLData = HUB_LDATA;
                    fade_type.type = FADE_TYPE_STILL;
                    FadeSys.SetFade(fade_type, 0);
                }
                progress = 1.0f - CreditsTime;
                if (progress < 0.0f) {
                    progress = 0.0f;
                }
                break;
        }
        CreditsAlpha = NuTrigTable[((i32)((f64)progress * 16384.0) >> 1) & 0x7fff];
    } else {
        if (FadeSys.fade == 0.0f) {
            CreditsTime += FRAMETIME;
        }
        if (CreditsFinishedTime > 0.0f) {
            CreditsFinishedTime += FRAMETIME;
            if (CreditsFinishedTime >= 1.1f) {
                CreditsFlag = 1;
                CreditsAlpha = 0.0f;
                CreditsTime = 0.0f;
            }
            if (CreditsAlpha > 0.0f) {
                CreditsAlpha -= FRAMETIME;
                if (CreditsAlpha < 0.0f) {
                    CreditsAlpha = 0.0f;
                }
            }
        } else if ((menu->buttons_pressed & GAMEPAD_SKIP) != 0 || CreditsTime >= Credits_Duration) {
            CreditsFinishedTime = 0.001f;
        }
        legoSetMusicVolume((f32)((f64)CreditsAlpha * volume));
    }
}
