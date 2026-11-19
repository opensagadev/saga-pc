#pragma once

#include "nu2api/nucore/common.h"

#include "gameapi/gui/apimenu.h"
#include "legogame/game.h"
#include "nu2api/nu3d/nuqfnt.h"

typedef struct MENU_VIDEO_OPTIONS { // size 0x14
    i32 aspect_mode;                // 0x00
    i32 shadows_enabled;            // 0x04
    i32 bloom_enabled;              // 0x08
    i32 speed_blur_enabled;         // 0x0c
    i32 hires_enabled;              // 0x10
#ifdef __cplusplus
    u8 Changed();
    void LoadFromWindowOptions();
    void Apply();
#endif
} MENU_VIDEO_OPTIONS;

#ifdef __cplusplus
extern "C" {
#endif

    void Text3DStringEncodeFont(u8 *source, u16 *destination, VUFNT *font);
    void TextDecodeCodeword(char *source, char *destination);
    void TextDecode(char *source, u8 *destination);
    f32 TextPrintSubstring(u8 *text, f32 x, f32 y, f32 z, f32 x_scale, f32 y_scale, i32 colour, i32 button_font);
    VUFNT *LoadGameFont(char *, char *, VARIPTR *, VARIPTR *, i32);
    VUFNT *LoadButtonFont(char *, char *, VARIPTR *, VARIPTR *, i32);
    void DrawMenuButtonPromptsEx(i32, i32, i32, i32, u32, u32, u32, u32);

    void DrawMenuButtonPrompts(i32, i32, i32, u32, u32, u32, u32);
    i32 FindNextBreak(u8 *, i32);
    i32 FindNearestBreak(u8 *, i32);
    void DrawMenu(i32);
    extern void (*headerdrawfn)(void);
    extern void (*predrawfn)(MENU *);
    extern i32 MenuSFX;
    extern i32 header_r;
    extern i32 header_g;
    extern i32 header_b;
    void MenuEnterNewGame(MENU *menu);
    extern const char *MenuSettingsStrings[39];
    extern i32 MenuSettingsStringsInitialized;
    extern i32 startnewgame_initiated;
    extern i32 startnewgame;
    i32 SplitTextFindNextWS(u8 *text, i32 position);
    void FixUpButtonsInFont(VUFNT *game_font, VUFNT *button_font);
    extern i32 MENUSFX_MENUMOVE;
    extern i32 Menu_InSaveFlow;
    extern i32 Menu_InLoadFlow;

    void MenuRepeat(i32 *, i32 *, f32 *, u8 *, f32, f32);
    typedef i32 (*SAVE_HASH_FN)(void);
    typedef void (*SAVE_VOID_FN)(void);
    extern f32 memcard_autosavecanceldelay;
    extern i32 memcard_slot;
    extern i32 memcard_loadneeded;
    extern i32 memcard_loadstarted;
    extern i32 memcard_loadfailed;
    extern i32 memcard_loadcorrupt;
    extern i32 memcard_saveneeded;
    extern i32 memcard_savestarted;
    extern i32 memcard_savefailed;
    extern i32 memcard_deleteneeded;
    extern i32 memcard_deletestarted;
    extern i32 memcard_deletefailed;
    extern i32 memcard_formatme;
    extern i32 memcard_formatting;
    extern i32 memcard_formatfailed;
    extern i32 memcard_justformatted;
    extern f32 memcard_loadmessage_delay;
    extern f32 memcard_savemessage_delay;
    extern f32 memcard_formatmessage_delay;
    extern f32 memcard_createmessage_delay;
    extern f32 memcard_deletemessage_delay;
    extern f32 memcard_loadresult_delay;
    extern f32 memcard_saveresult_delay;
    extern f32 memcard_formatresult_delay;
    extern f32 memcard_createresult_delay;
    extern f32 memcard_deleteresult_delay;
    extern f32 memcard_message_delay;
    extern f32 memcard_result_delay;
    extern i32 memcard_cardchanged;
    extern SAVE_HASH_FN memcard_hashfn;
    extern void *memcard_headerdata;
    extern void *memcard_headerdatabuffer;
    extern i32 memcard_headerdatasize;
    extern i32 memcard_slotsused;
    extern SAVE_VOID_FN savesuccessfn;
    extern i32 saveload_autosavedisabled;
    void UpdateSaveSlots(void);
    void loadsaveCallEachFrame(void);
    extern i32 Menu_DisableCancel;
    extern i32 Menu_InHeaderSave;
    extern i32 Menu_InWarningFlow;
    extern i32 MenuASCancelFinished;
    extern i32 MenuCardWarningState;
    extern i32 Menu_Overwriteflag;
    extern i32 MenuLoadStarted;
    extern i32 MENUSFX_MENUSELECT;
    extern i32 MENUSFX_MENUNOENTRY;
    void MenuEnterSaveConfirm(MENU *);
    void MenuEnterAutoSaveWarning(MENU *);
    void MenuEnterAutoSaveCancel(MENU *);
    void MenuEnterCardWarning(MENU *);
    void MenuEnterHeaderSave(MENU *);
    void MenuEnterStartNewGame(MENU *);
    void MenuExitSave(MENU *);
    void MenuExitLoad(MENU *);
    void MenuExitCardWarning(MENU *);
    void MenuUpdateSave(MENU *);
    void MenuUpdateInsertCard(MENU *);
    void MenuUpdateNoMemoryCard(MENU *);
    void MenuUpdateNotEnoughSpace(MENU *);
    void MenuUpdateNoData(MENU *);
    void MenuUpdateFormatConfirm(MENU *);
    void MenuUpdateFormatting(MENU *);
    void MenuUpdateDeleteConfirm(MENU *);
    void MenuUpdateDeleting(MENU *);
    void MenuUpdateSaveConfirm(MENU *);
    void MenuUpdateSaving(MENU *);
    void MenuUpdateAutoSaveWarning(MENU *);
    void MenuUpdateLoad(MENU *);
    void MenuUpdateLoadConfirm(MENU *);
    void MenuUpdateLoading(MENU *);
    void MenuUpdateSaveCancel(MENU *);
    void MenuUpdateLoadCancel(MENU *);
    void MenuUpdateFormatCancel(MENU *);
    extern i32 Menu_LastFlow;
    extern i32 SAVESIZE;
    void MenuEnterSave(MENU *);
    void MenuEnterLoad(MENU *);
    extern i32 MENUSFX_MENUBACK;
    void BackupMenuNoFn(void);
    void MenuEnterInsertCard(MENU *);
    void MenuDrawInsertCard(MENU *);
    void MenuDrawFormatConfirm(MENU *);
    void MenuDrawNotEnoughSpace(MENU *);
    void MenuDrawSaving(MENU *);
    void MenuDrawDeleting(MENU *);
    void MenuDrawAutoSaveWarning(MENU *);
    void MenuDrawSave(MENU *);
    void MenuDrawLoad(MENU *);
    void MenuDrawNoMemoryCard(MENU *);
    void MenuDrawDeleteConfirm(MENU *);
    void MenuDrawSaveConfirm(MENU *);
    void MenuDrawLoadConfirm(MENU *);
    void MenuDrawSaveCancel(MENU *);
    void MenuDrawLoadCancel(MENU *);
    void MenuDrawFormatCancel(MENU *);
    void MenuDrawFileCorrupt(MENU *);
    extern f32 MENUBOTY;
    extern char *apiGameName;
    extern char *apitxt_YES;
    extern char *apitxt_NO;
    extern char *apitxt_OK;
    extern char *apitxt_CANCEL;
    extern char *apitxt_RETRY;
    extern char *apitxt_SLOT;
    extern char *apitxt_PRESENT;
    extern char *apitxt_EMPTY;
    extern char *apitxt_NOSPACE;
    extern char *apitxt_SAVEGAME;
    extern char *apitxt_LOADGAME;
    extern char *apitxt_DELETEGAME;
    extern char *apitxt_GAMESAVED;
    extern char *apitxt_GAMELOADED;
    extern char *apitxt_DELETING;
    extern char *apitxt_FORMATTING;
    extern char *apitxt_CORRUPTLOAD;
    extern char *apitxt_CONFIRMLOAD;
    extern char *apitxt_CONFIRMSAVE;
    extern char *apitxt_CONFIRMOVERWRITE;
    extern char *apitxt_DOYOUWANTTOABORT;
    extern char *apitxt_DOYOUWANTTOABORTLOAD;
    extern char *apitxt_DOYOUWANTTOABORTFORMAT;
    extern char *apitxt_NEWSAVE;
    extern char *apitxt_CONFIRMDELETE;
    extern char *apitxt_GAMESAVING;
    extern char *apitxt_GAMELOADING;
    extern char *apitxt_FAILEDTOLOAD;
    extern char *apitxt_FAILEDTOSAVE;
    extern char *apitxt_FAILEDTOOVERWRITE;
    extern char *apitxt_NODATAAVAILABLE;
    extern char *apitxt_AUTOSAVE_ENABLED;
    extern char *apitxt_FORMATTINGCOMPLETE;
    extern char *apitxt_DELETECOMPLETE;
    extern char *apitxt_NOTENOUGHSPACE;
    extern i32 newgamefade;
    void MenuRegisterSoundFX(i32 move, i32 select, i32 back, i32 no_entry);
    extern MENU_VIDEO_OPTIONS MenuVideoOptions;
    extern i32 MenuAppliedAspect;
#ifdef __cplusplus
    bool MenuCanChangeMultisample(void);
    bool MenuCanChangeShadows(void);
#ifdef __cplusplus
}
#endif
#endif
#ifdef __cplusplus
extern "C" {
#endif
    void MenuEnterVideo(MENU *);
    void MenuDrawVideo(MENU *);
    void MenuExitVideo(MENU *);
    void MenuEnterEffects(MENU *);
    void MenuDrawEffects(MENU *);
    void MenuExitEffects(MENU *);
    extern f32 MenuVideoConfirmDuration;
    extern i32 MenuVideoConfirmWait;
    void MenuEnterVideoConfirm(MENU *);
    void MenuDrawVideoConfirm(MENU *);
    void MenuExitVideoConfirm(MENU *);
    void MenuDrawNotice(char *, u32, u32, u32);
    void MenuEnterHiresNotice(MENU *);
    void MenuDrawHiresNotice(MENU *);
    void MenuExitHiresNotice(MENU *);
    void MenuEnterQuit(MENU *);
    void MenuDrawQuit(MENU *);
    void MenuExitQuit(MENU *);
    void MenuDrawAutoSaveCancel(MENU *);
    void MenuUpdateAutoSaveCancel(MENU *);

    i32 MenuInMemoryCard(void);
    void MakeMenuPacket(void);
    extern i32 shop_save_done;
    extern GAMESAVE TempGame; /* 856c38, native0x7e4c owner. */
    extern f32 BlipL[2];
    extern f32 uprepeattime[2];
    extern u8 uprepeatcount[2];
    extern u8 leftrepeatcount[2];
    void fun_00509470(MENU *);
    void fun_00509480(MENU *);
    void fun_00509490(MENU *);
    void fun_005094a0(MENU *);
#ifdef __cplusplus
}
#endif
