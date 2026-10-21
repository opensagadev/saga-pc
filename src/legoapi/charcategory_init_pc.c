#include "saga.h"

STATIC_ASSERT(sizeof(CHAR_CATEGORY_RECORD_PC) == 12);
STATIC_ASSERT(offsetof(CHAR_CATEGORY_RECORD_PC, name) == 0);
STATIC_ASSERT(offsetof(CHAR_CATEGORY_RECORD_PC, model_flags) == 4);
STATIC_ASSERT(offsetof(CHAR_CATEGORY_RECORD_PC, game_flags) == 8);
extern char AreaFixUpName_Jedi[8];
extern const char PCContextName_Teleport[9];
extern const char PCContextName_ZipUp[6];

#pragma const_seg(".rdata$StartupCharText_SuperJumpPc")
const char StartupCharText_SuperJumpPc[10] = "SuperJump";
#pragma const_seg()
#pragma const_seg(".rdata$StartupCharText_SuperStrengthPc")
const char StartupCharText_SuperStrengthPc[14] = "SuperStrength";
#pragma const_seg()
#pragma const_seg(".rdata$StartupCharText_TransformatronPc")
const char StartupCharText_TransformatronPc[15] = "Transformatron";
#pragma const_seg()
#pragma const_seg(".rdata$StartupCharText_BypassSecurityPc")
const char StartupCharText_BypassSecurityPc[15] = "BypassSecurity";
#pragma const_seg()
#pragma const_seg(".rdata$StartupCharText_HazardProtectionPc")
const char StartupCharText_HazardProtectionPc[17] = "HazardProtection";
#pragma const_seg()
#pragma const_seg(".rdata$StartupCharText_JediBaddiePc")
const char StartupCharText_JediBaddiePc[11] = "JediBaddie";
#pragma const_seg()
#pragma const_seg(".rdata$StartupCharText_BountyHunterPc")
const char StartupCharText_BountyHunterPc[13] = "BountyHunter";
#pragma const_seg()
#pragma const_seg(".rdata$StartupCharText_HighJumpPc")
const char StartupCharText_HighJumpPc[9] = "HighJump";
#pragma const_seg()
#pragma const_seg(".rdata$StartupCharText_AstromechPc")
const char StartupCharText_AstromechPc[10] = "Astromech";
#pragma const_seg()
#pragma const_seg(".rdata$StartupCharText_ProtocolPc")
const char StartupCharText_ProtocolPc[9] = "Protocol";
#pragma const_seg()
#pragma const_seg(".rdata$StartupCharText_walker_2legsPc")
const char StartupCharText_walker_2legsPc[13] = "walker_2legs";
#pragma const_seg()
#pragma const_seg(".rdata$StartupCharText_walker_4legsPc")
const char StartupCharText_walker_4legsPc[13] = "walker_4legs";
#pragma const_seg()

#pragma data_seg(".data$StartupCharCategoriesDefaultPc")
CHAR_CATEGORY_RECORD_PC StartupCharCategoriesDefaultPc[6] = {
    {(char *)StartupCharText_SuperJumpPc, 0x0u, 0x400000u},
    {(char *)StartupCharText_SuperStrengthPc, 0x0u, 0x800000u},
    {(char *)StartupCharText_TransformatronPc, 0x0u, 0x1000000u},
    {(char *)StartupCharText_BypassSecurityPc, 0x0u, 0x2000000u},
    {(char *)StartupCharText_HazardProtectionPc, 0x0u, 0x4000000u},
    {NULL, 0x0u, 0x0u},
};
#pragma data_seg()

#pragma data_seg(".data$StartupCharCategoriesLswPc")
CHAR_CATEGORY_RECORD_PC StartupCharCategoriesLswPc[10] = {
    {(char *)AreaFixUpName_Jedi, 0x8u, 0x0u},
    {(char *)StartupCharText_JediBaddiePc, 0xcu, 0x0u},
    {(char *)StartupCharText_BountyHunterPc, 0x1100080u, 0x0u},
    {(char *)PCContextName_Teleport, 0x40000u, 0x0u},
    {(char *)StartupCharText_HighJumpPc, 0x0u, 0x400000u},
    {(char *)StartupCharText_AstromechPc, 0x40u, 0x0u},
    {(char *)StartupCharText_ProtocolPc, 0x20u, 0x0u},
    {(char *)PCContextName_ZipUp, 0x100080u, 0x0u},
    {"Blaster", 0x80u, 0x0u},
    {NULL, 0x0u, 0x0u},
};
#pragma data_seg()

#pragma data_seg(".data$StartupCharVariantNamesPc")
char *StartupCharVariantNamesPc[23] = {
    "weirdo",
    "stormtrooper",
    "fett",
    "macewindu",
    "battledroid",
    "wookiee",
    "obiwankenobi",
    "leia",
    "clone",
    "rebel",
    "tie",
    "lando",
    "luke",
    "hansolo",
    "padme",
    (char *)StartupCharText_walker_2legsPc,
    (char *)StartupCharText_walker_4legsPc,
    "hoverdroid",
    "critter",
    "naboostarfighter",
    "henchman",
    NULL,
    NULL,
};
#pragma data_seg()

void __cdecl CharCategories_Init(CHAR_CATEGORY_RECORD_PC *categories) {
    CharCategory = categories;
    CHARCATEGORYCOUNT = 0;
    if (categories != NULL) {
        while (categories->name != NULL) {
            ++CHARCATEGORYCOUNT;
            ++categories;
        }
    }
}
