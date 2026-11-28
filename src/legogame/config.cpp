#include "legoapi/characters/core/character.h"
#include "legoapi/characters/core/charconfig.h"
#include "legoapi/characters/motion/move.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nufile/nufpar.h"

static void CC_variant(NUFPAR *parser) {
    if (NuFParGetWord(parser) == 0) {
        return;
    }
    if (NuStrICmp(parser->word_buffer_ptr, "stormtrooper") == 0) {
        charconfig.runtime->variant = CHARVARIANT_STORMTROOPER;
    } else if (NuStrICmp(parser->word_buffer_ptr, "fett") == 0) {
        charconfig.runtime->variant = CHARVARIANT_FETT;
    } else if (NuStrICmp(parser->word_buffer_ptr, "macewindu") == 0) {
        charconfig.runtime->variant = CHARVARIANT_MACEWINDU;
    } else if (NuStrICmp(parser->word_buffer_ptr, "battledroid") == 0) {
        charconfig.runtime->variant = CHARVARIANT_BATTLEDROID;
    } else if (NuStrICmp(parser->word_buffer_ptr, "hoverdroid") == 0) {
        charconfig.runtime->variant = CHARVARIANT_HOVERDROID;
    } else if (NuStrICmp(parser->word_buffer_ptr, "walker2legs") == 0) {
        charconfig.runtime->variant = CHARVARIANT_WALKER_2LEGS;
    } else if (NuStrICmp(parser->word_buffer_ptr, "walker4legs") == 0) {
        charconfig.runtime->variant = CHARVARIANT_WALKER_4LEGS;
    } else if (NuStrICmp(parser->word_buffer_ptr, "wookiee") == 0) {
        charconfig.runtime->variant = CHARVARIANT_WOOKIEE;
    } else if (NuStrICmp(parser->word_buffer_ptr, "obiwankenobi") == 0) {
        charconfig.runtime->variant = CHARVARIANT_OBIWANKENOBI;
    } else if (NuStrICmp(parser->word_buffer_ptr, "leia") == 0) {
        charconfig.runtime->variant = CHARVARIANT_LEIA;
    } else if (NuStrICmp(parser->word_buffer_ptr, "clone") == 0) {
        charconfig.runtime->variant = CHARVARIANT_CLONE;
    } else if (NuStrICmp(parser->word_buffer_ptr, "rebel") == 0) {
        charconfig.runtime->variant = CHARVARIANT_REBEL;
    } else if (NuStrICmp(parser->word_buffer_ptr, "tie") == 0) {
        charconfig.runtime->variant = CHARVARIANT_TIE;
    } else if (NuStrICmp(parser->word_buffer_ptr, "lando") == 0) {
        charconfig.runtime->variant = CHARVARIANT_LANDO;
    } else if (NuStrICmp(parser->word_buffer_ptr, "weirdo") == 0) {
        charconfig.runtime->variant = CHARVARIANT_WEIRDO;
    } else if (NuStrICmp(parser->word_buffer_ptr, "luke") == 0) {
        charconfig.runtime->variant = CHARVARIANT_LUKE;
    } else if (NuStrICmp(parser->word_buffer_ptr, "hansolo") == 0) {
        charconfig.runtime->variant = CHARVARIANT_HANSOLO;
    } else if (NuStrICmp(parser->word_buffer_ptr, "padme") == 0) {
        charconfig.runtime->variant = CHARVARIANT_PADME;
    } else if (NuStrICmp(parser->word_buffer_ptr, "critter") == 0) {
        charconfig.runtime->variant = CHARVARIANT_CRITTER;
    } else if (NuStrICmp(parser->word_buffer_ptr, "naboostarfighter") == 0) {
        charconfig.runtime->variant = CHARVARIANT_NABOOSTARFIGHTER;
    } else if (NuStrICmp(parser->word_buffer_ptr, "pod") == 0) {
        charconfig.runtime->variant = CHARVARIANT_POD;
    } else if (NuStrICmp(parser->word_buffer_ptr, "republicgunship") == 0) {
        charconfig.runtime->variant = CHARVARIANT_REPUBLICGUNSHIP;
    } else if (NuStrICmp(parser->word_buffer_ptr, "henchman") == 0) {
        charconfig.runtime->variant = CHARVARIANT_HENCHMAN;
    }
}

extern "C" NUFPCOMJMP ConfigChar_GameKeywords[2] = {{"variant", CC_variant}, {NULL, NULL}};
