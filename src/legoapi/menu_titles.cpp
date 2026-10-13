#include "saga.h"

void __cdecl legoSetMusicVolume(f32);

extern "C" {
    void __cdecl MusicClearAll(void);
    void __cdecl SoundKillAll(void);
    void __cdecl GamePlayMusic(LEVELDATA_PC *, i32, OPTIONSSAVE *);
    i32 __cdecl Movie_Play(char *, VARIPTR *, VARIPTR *, f32, i32(__cdecl *)(void), f32);
    extern char *Trailer[6];
}

static i32 MenuUpdateTitleFadePc(MENU *menu) {
    f32 music_volume = 0.0f;
    if (Game.options_save.music_enabled)
        music_volume = GameGetMusicVolume(&TempOptions);
    if (newgamefade == 0) {
        legoSetMusicVolume(music_volume);
        return 0;
    }
    if (newgamefade == 1) {
        if (newgamewait > 0.0f)
            newgamewait -= FRAMETIME;
        else {
            newgamealpha += FRAMETIME * 2.0f;
            if (newgamealpha > 1.0f) {
                newgamealpha = 1.0f;
                newgamefade = 0;
            }
        }
    } else if (newgamewait > 0.0f) {
        newgamewait -= FRAMETIME;
        music_volume = 0.0f;
        if (newgamewait <= 0.0f) {
            if (!NOSOUND) {
                music_man.StopAll(0);
                MusicClearAll();
                SoundKillAll();
            }
            Movie_Play(Trailer[0], &characterbuffer_ptr, &characterbuffer_end, DEFAULTFRAMETIME, NULL,
                       (f32)Game.options_save.music_volume / 10.0f);
            FinishLoop(2);
            ResetTimer(&GameTimer, 0.0f);
            menu->menu_time = menu->time_04 = 0.0f;
            newgamealpha = 0.0f;
            newgamefade = 1;
            newgamewait = 0.5f;
            if (!NOSOUND && !NOMUSIC) {
                music_man.StopAll(0);
                GamePlayMusic(WORLD->current_level, 0, &Game.options_save);
            }
        }
    } else if (newgamefade == 2) {
        newgamealpha -= FRAMETIME * 2.0f;
        if (newgamealpha <= 0.0f) {
            newgamealpha = 0.0f;
            newgamewait = 0.1f;
        }
        music_volume *= newgamealpha;
    }
    legoSetMusicVolume(music_volume);
    return 1;
}

extern "C" void __cdecl MenuUpdateTitles(MENU *menu) {
    i32 kind, value;
    if (MenuUpdateTitleFadePc(menu))
        return;
    if (menu->menu_time >= 30.0) {
        newgamefade = 2;
        return;
    }
    if (!(GameTimer.time_elapsed > 3.0f))
        return;
    TextInputSetLastDevicePc(0);
    i32 port = PcInputSelectPressedDevicePc(0, 26, &kind, &value, 1);
    if (port < 0)
        return;
    port = PcInputMapKeyboardPortPc(port, 0);
    NuPadMapPlayerToPort(0, port);
    NuPadSetValid(port, 1);
    TextInputSetLastDevicePc(0);
    if (PcInputSelectConfigurationPc(port, 0, -1))
        NewMenu(1, saveload_savepresent != 0, -1);
    else
        NewMenu(22, 0, -1);
}
