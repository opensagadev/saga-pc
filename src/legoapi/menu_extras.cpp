#include "saga.h"
#include <stdio.h>

extern "C" {
    void __cdecl GameDrawMenuEntry(MENU *, char *);
    extern CHEAT_PC ExtrasCheatsPc[46];
    extern f32 ExtrasFadeStartPc, ExtrasFadeEndPc;
    extern i16 TextIdPc88074c, TextIdPc880708, TextIdPc880764;
    extern const char ExtrasNameFormatPc[], ExtrasLockedNumberFormatPc[];

    void __cdecl MenuDrawSelectLanguage(MENU *menu) {
        for (i32 i = 0; i < LANGUAGECOUNT; ++i)
            GameDrawMenuEntry(menu, Text_LanguageList[i].localized_name);
    }

    void __cdecl MenuUpdateSelectLanguage(MENU *menu) {
        if (menu->confirm_pressed || menu->time_04 >= 10.0f) {
            Text_SetLanguage(Text_LanguageList[menu->selected_item].language);
            BackupMenu();
        }
    }
}

struct EXTRAS_AREA_SAVE_READER_PC {
    u8 unknown0[6];
    u8 red_brick_unlocked;
    u8 unknown7[5];
};
struct EXTRAS_SAVE_READER_PC {
    u8 unknown0[0x782c];
    EXTRAS_AREA_SAVE_READER_PC areas[72];
    u8 unknown7b8c[0x74];
    u32 unlocked[2];
};
STATIC_ASSERT(sizeof(EXTRAS_AREA_SAVE_READER_PC) == 12);
STATIC_ASSERT(offsetof(EXTRAS_AREA_SAVE_READER_PC, red_brick_unlocked) == 6);
STATIC_ASSERT(offsetof(EXTRAS_SAVE_READER_PC, unlocked) == 0x7c00);

extern "C" void __cdecl MenuUpdateExtras(MENU *menu) {
    EXTRAS_SAVE_READER_PC *save = (EXTRAS_SAVE_READER_PC *)&Game;
    if (menu->confirm_pressed) {
        i32 index = (menu->selected_item + 44) % 45;
        if (!(save->unlocked[index / 32] & (1u << (index & 31))) || (BonusArea && Cheat_CheckFlags(index, 0x200000))) {
            GameAudio_PlaySfx(0x32, NULL, 0, 0);
            return;
        }
        Cheat_SetOn(index, !Cheat_IsOn(index), 1);
        GameAudio_PlaySfx(0x30, NULL, 0, 0);
    } else if (menu->cancel_pressed) {
        BackupMenu();
        MenuSFX = GameAudio_GetSfxId(0x31);
    }
}

extern "C" void __cdecl MenuDrawExtras(MENU *menu) {
    char text[256], number[64];
    EXTRAS_SAVE_READER_PC *save = (EXTRAS_SAVE_READER_PC *)&Game;
    menu->draw_y = -menu->selected_row * MENUDY * menu->item_scale;
    for (i32 i = 0; i < 45; ++i) {
        i32 index = (i + 44) % 45;
        CHEAT_PC *cheat = &ExtrasCheatsPc[index];
        f32 alpha = 0.5f;
        if (index >= 8 && !(save->unlocked[index / 32] & (1u << (index & 31))) && cheat->area_index != -1 &&
            !save->areas[cheat->area_index].red_brick_unlocked) {
            NuStrCpy(text, TTab[TextIdPc88074c]);
            sprintf(number, ExtrasLockedNumberFormatPc, index - 7);
            NuStrCat(text, number);
            NuStrCat(text, TTab[TextIdPc880708]);
        } else {
            if (save->unlocked[index / 32] & (1u << (index & 31))) {
                alpha = 1.0f;
                NuStrCpy(number, TTab[Cheat_IsOn(index) ? tON : tOFF]);
                if (BonusArea && Cheat_CheckFlags(index, 0x200000))
                    alpha = 0.5f;
            } else {
                NuStrCpy(number, TTab[TextIdPc880764]);
            }
            sprintf(text, ExtrasNameFormatPc, TTab[*cheat->text_id], number);
        }
        dme_sy = menu->item_scale;
        if (index >= 8)
            dme_rgb = 1;
        if (index == 44) {
            dme_r = 0xbf;
            dme_g = 0;
            dme_b = 0x7f;
        } else {
            dme_r = 0xdf;
            dme_g = 0x3f;
            dme_b = 0;
        }
        union {
            f32 value;
            u32 bits;
        } absolute;
        absolute.value = menu->draw_y;
        absolute.bits &= 0x7fffffff;
        f32 distance = absolute.value;
        if (distance > ExtrasFadeStartPc) {
            if (distance > ExtrasFadeEndPc)
                alpha = 0.0f;
            else
                alpha *= 1.0f - (distance - ExtrasFadeStartPc) / (ExtrasFadeEndPc - ExtrasFadeStartPc);
        }
        if (Paused) {
            dme_align = PauseMenus_Align;
            menu->draw_x = PauseMenus_X;
        }
        DrawMenuEntryEx(menu, text, (u8)(i32)(MenuA * alpha));
    }
}
