#include "saga.h"


typedef struct CHARCONFIG_PC {
    CHARACTERDATA_PC *character;
    PLAYERCHARACTERCONFIG_PC *runtime;
    GAMECHARACTERLAYER_PC *layer_scratch;
    union {
        u32 flags;
        struct {
            u32 flags_low : 6;
            u32 permanent : 1;
            u32 flags_high : 25;
        };
    };
    u8 named_layers;
    u8 unknown_11[3];
    i32 effect_count;
    struct CHARACTER_EFFECT_CONFIG_PC *effect_scratch;
    i16 character_id;
    u16 animation_count;
    VARIPTR *arena;
    VARIPTR *arena_end;
    char animation_names[100][40];
} CHARCONFIG_PC;
extern CHARCONFIG_PC charconfig;
typedef struct charconfig_variant_reader_pc_s {
    u8 unknown_000[0x116];
    i8 variant_type;
    u8 unknown_117[9];
} CHARCONFIG_VARIANT_READER_PC;

static void CC_variant(NUFPAR_PC *parser) {
    if (NuFParGetWord(parser) == 0)
        return;
    if (NuStrICmp(parser->word_buffer_ptr, "stormtrooper") == 0)
        ((CHARCONFIG_VARIANT_READER_PC *)charconfig.runtime)->variant_type = 1;
    else if (NuStrICmp(parser->word_buffer_ptr, "fett") == 0)
        ((CHARCONFIG_VARIANT_READER_PC *)charconfig.runtime)->variant_type = 2;
    else if (NuStrICmp(parser->word_buffer_ptr, "macewindu") == 0)
        ((CHARCONFIG_VARIANT_READER_PC *)charconfig.runtime)->variant_type = 3;
    else if (NuStrICmp(parser->word_buffer_ptr, "battledroid") == 0)
        ((CHARCONFIG_VARIANT_READER_PC *)charconfig.runtime)->variant_type = 4;
    else if (NuStrICmp(parser->word_buffer_ptr, "hoverdroid") == 0)
        ((CHARCONFIG_VARIANT_READER_PC *)charconfig.runtime)->variant_type = 17;
    else if (NuStrICmp(parser->word_buffer_ptr, "walker2legs") == 0)
        ((CHARCONFIG_VARIANT_READER_PC *)charconfig.runtime)->variant_type = 15;
    else if (NuStrICmp(parser->word_buffer_ptr, "walker4legs") == 0)
        ((CHARCONFIG_VARIANT_READER_PC *)charconfig.runtime)->variant_type = 16;
    else if (NuStrICmp(parser->word_buffer_ptr, "wookiee") == 0)
        ((CHARCONFIG_VARIANT_READER_PC *)charconfig.runtime)->variant_type = 5;
    else if (NuStrICmp(parser->word_buffer_ptr, "obiwankenobi") == 0)
        ((CHARCONFIG_VARIANT_READER_PC *)charconfig.runtime)->variant_type = 6;
    else if (NuStrICmp(parser->word_buffer_ptr, "leia") == 0)
        ((CHARCONFIG_VARIANT_READER_PC *)charconfig.runtime)->variant_type = 7;
    else if (NuStrICmp(parser->word_buffer_ptr, "clone") == 0)
        ((CHARCONFIG_VARIANT_READER_PC *)charconfig.runtime)->variant_type = 8;
    else if (NuStrICmp(parser->word_buffer_ptr, "rebel") == 0)
        ((CHARCONFIG_VARIANT_READER_PC *)charconfig.runtime)->variant_type = 9;
    else if (NuStrICmp(parser->word_buffer_ptr, "tie") == 0)
        ((CHARCONFIG_VARIANT_READER_PC *)charconfig.runtime)->variant_type = 10;
    else if (NuStrICmp(parser->word_buffer_ptr, "lando") == 0)
        ((CHARCONFIG_VARIANT_READER_PC *)charconfig.runtime)->variant_type = 11;
    else if (NuStrICmp(parser->word_buffer_ptr, "weirdo") == 0)
        ((CHARCONFIG_VARIANT_READER_PC *)charconfig.runtime)->variant_type = 0;
    else if (NuStrICmp(parser->word_buffer_ptr, "luke") == 0)
        ((CHARCONFIG_VARIANT_READER_PC *)charconfig.runtime)->variant_type = 12;
    else if (NuStrICmp(parser->word_buffer_ptr, "hansolo") == 0)
        ((CHARCONFIG_VARIANT_READER_PC *)charconfig.runtime)->variant_type = 13;
    else if (NuStrICmp(parser->word_buffer_ptr, "padme") == 0)
        ((CHARCONFIG_VARIANT_READER_PC *)charconfig.runtime)->variant_type = 14;
    else if (NuStrICmp(parser->word_buffer_ptr, "critter") == 0)
        ((CHARCONFIG_VARIANT_READER_PC *)charconfig.runtime)->variant_type = 18;
    else if (NuStrICmp(parser->word_buffer_ptr, "naboostarfighter") == 0)
        ((CHARCONFIG_VARIANT_READER_PC *)charconfig.runtime)->variant_type = 19;
    else if (NuStrICmp(parser->word_buffer_ptr, "pod") == 0)
        ((CHARCONFIG_VARIANT_READER_PC *)charconfig.runtime)->variant_type = 20;
    else if (NuStrICmp(parser->word_buffer_ptr, "republicgunship") == 0)
        ((CHARCONFIG_VARIANT_READER_PC *)charconfig.runtime)->variant_type = 21;
    else if (NuStrICmp(parser->word_buffer_ptr, "henchman") == 0)
        ((CHARCONFIG_VARIANT_READER_PC *)charconfig.runtime)->variant_type = 22;
}

NUFPCOMJMP ConfigChar_GameKeywords[2] = {{"variant", CC_variant}, {NULL, NULL}};
