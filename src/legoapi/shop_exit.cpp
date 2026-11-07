#include "saga.h"
#include <string.h>

extern "C" {
    extern i32 HubForceShopSavePc;
    i32 __cdecl UpdateShop(void *);
    void __cdecl MenuRememberCursor(MENU *);
    void __cdecl NuMainStatusHelperPc(STATUSPACKET_s *);
    void __cdecl Text_FillInExtendedSaveInfo(void);
    void __cdecl Hub_ClearStats(void);
    i32 __cdecl TriggerAutoSave(void);

    void __cdecl EndShopMenu(i32) {
        LevLock[0] = 1;
        LevTime[0] = 0.6f;
        DrawCoinTotalTime = 2.0f;
        MenuRememberCursor(&GameMenu[GameMenuLevel]);
        MenuReset();
        ReCalculateCompletionPoints();
    }

    void __cdecl MenuUpdateShop(MENU *menu) {
        if (shop_save_done != 0) {
            EndShopMenu(0);
            shop_save_done = 0;
            return;
        }
        if (UpdateShop(menu) == 0)
            return;
        GameAudio_PlaySfx(0x31, NULL, 0, 0);
        SHOPACTIVE = 0;
        ShopNameAlpha = 0.0f;
        shop_quit = 1;
        if (HubForceShopSavePc == 0) {
            TempGame.game_time = Game.game_time;
            if (memcmp(&TempGame, &Game, sizeof(Game)) == 0) {
                EndShopMenu(0);
                return;
            }
        }
        HubForceShopSavePc = 0;
        NuMainStatusHelperPc(NULL);
        Hub_ClearStats();
        Text_FillInExtendedSaveInfo();
        if (TriggerAutoSave() != 0) {
            EndShopMenu(0);
            return;
        }
        shop_save_done = 1;
        NewMenu(1000, -1, -1);
    }
}
