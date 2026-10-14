#include "saga.h"
#include <stdio.h>

extern i32 SAVESLOTS;
extern f32 memcard_autosavecanceldelay;
extern i32 saveload_cardtype;
extern i32 saveload_cardformatted;
extern i32 saveload_autosave;
extern i32 MenuLoadOccurred;
extern i32 MenuSaveOccurred;
extern i32 memcard_slot;
extern i32 memcard_loadneeded;
extern i32 memcard_loadstarted;
extern i32 memcard_loadfailed;
extern i32 memcard_loadcorrupt;
extern i32 memcard_saveneeded;
extern i32 memcard_savestarted;
extern i32 memcard_savefailed;
extern i32 memcard_autosaveneeded;
extern i32 memcard_autosavestarted;
extern f32 memcard_autosavepredelay;
extern f32 memcard_autosavepostdelay;
extern i32 memcard_autosaveinprogress;
extern i32 memcard_autosavedisabled;
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
extern void *memcard_savedata;
extern void *memcard_savedatabuffer;
extern i32 memcard_savedatasize;
extern i32 memcard_autosave;
extern SAVE_HASH_FN_PC memcard_hashfn;
extern SAVE_VOID_FN_PC memcard_drawasiconfn;
extern void *memcard_headerdata;
extern void *memcard_headerdatabuffer;
extern i32 memcard_headerdatasize;
extern i32 memcard_slotsused;
extern SAVE_VOID_FN_PC savesuccessfn;
extern i32 saveload_status;
extern i32 saveload_error;
extern i32 saveload_cardchanged;
extern i32 saveload_savepresent;
extern i32 saveload_autosavedisabled;
extern i32 saveload_slotused[];

void __cdecl SaveSystemInitialise(i32 slots, SAVE_HASH_FN_PC hashfn, void *save, i32 save_size, i32 autosave,
                                  SAVE_VOID_FN_PC draw_save_icon) {
    SAVESLOTS = slots;
    if (slots >= 6)
        SAVESLOTS = 6;
    memcard_hashfn = hashfn;
    memcard_savedata = save;
    memcard_savedatasize = save_size;
    memcard_savedatabuffer =
        NuAlloc(save_size + 4, "d:\\projects\\game_saga\\gameapi.saga\\gui\\gamemenu\\apisave.c", 0x6d);
    memcard_autosave = autosave;
    memcard_drawasiconfn = draw_save_icon;
}

void __cdecl SaveSystemInitialiseEx(i32 slots, SAVE_HASH_FN_PC hashfn, void *save, i32 save_size, void *header,
                                    i32 header_size, i32 autosave, SAVE_VOID_FN_PC draw_save_icon) {
    memcard_hashfn = hashfn;
    memcard_savedata = save;
    memcard_savedatasize = save_size;
    memcard_savedatabuffer =
        NuAlloc(save_size + 4, "d:\\projects\\game_saga\\gameapi.saga\\gui\\gamemenu\\apisave.c", 0x7f);
    memcard_autosave = autosave;
    memcard_drawasiconfn = draw_save_icon;
    memcard_headerdata = header;
    memcard_headerdatasize = header_size;
    memcard_headerdatabuffer =
        NuAlloc(header_size + 4, "d:\\projects\\game_saga\\gameapi.saga\\gui\\gamemenu\\apisave.c", 0x84);
    if (header == NULL) {
        SAVESLOTS = slots < 6 ? slots : 6;
    } else {
        SAVESLOTS = slots < 5 ? slots : 5;
    }
}

void __cdecl SetSaveSuccessFn(SAVE_VOID_FN_PC callback) {
    savesuccessfn = callback;
}

i32 __cdecl ChecksumSaveData(void *buffer, i32 size) {
    i32 count = size / 4;
    i32 index;
    u32 sum = 0x5c0999;
    for (index = 0; index < count; ++index)
        sum += ((u32 *)buffer)[index];
    return (i32)sum;
}

void UpdateSaveSlots(void) {
    const f32 elapsed = NuTimeGetFrameTime();
    if (memcard_autosavecanceldelay > 0.0f)
        memcard_autosavecanceldelay -= elapsed;
#define ADVANCE_SAVE_DELAY(message, result)                                                                            \
    if (message > 0.0f) {                                                                                              \
        message -= elapsed;                                                                                            \
    } else if (result > 0.0f) {                                                                                        \
        result -= elapsed;                                                                                             \
    }
    ADVANCE_SAVE_DELAY(memcard_loadmessage_delay, memcard_loadresult_delay);
    ADVANCE_SAVE_DELAY(memcard_savemessage_delay, memcard_saveresult_delay);
    if (memcard_autosavepredelay > 0.0f) {
        memcard_autosavepredelay -= elapsed;
    } else if (memcard_autosavepostdelay > 0.0f) {
        memcard_autosavepostdelay -= elapsed;
        if (memcard_autosavepostdelay <= 0.0f)
            memcard_autosaveinprogress = 0;
    }
    ADVANCE_SAVE_DELAY(memcard_formatmessage_delay, memcard_formatresult_delay);
    ADVANCE_SAVE_DELAY(memcard_createmessage_delay, memcard_createresult_delay);
    ADVANCE_SAVE_DELAY(memcard_deletemessage_delay, memcard_deleteresult_delay);
    ADVANCE_SAVE_DELAY(memcard_message_delay, memcard_result_delay);
#undef ADVANCE_SAVE_DELAY

    if (saveload_cardtype != 2)
        memcard_justformatted = 0;
    if (saveload_autosavedisabled != 0) {
        if (memcard_autosavestarted != 0) {
            memcard_savefailed = 1;
            memcard_saveresult_delay = 1.5f;
        }
        memcard_autosavestarted = 0;
        memcard_autosavepredelay = 0.0f;
        memcard_autosaveinprogress = 0;
        memcard_autosaveneeded = 0;
        memcard_autosavepostdelay = 0.0f;
        memcard_autosavedisabled = 1;
        saveload_autosavedisabled = 0;
    }

    if (memcard_loadneeded != 0) {
        if (saveload_status == 1 && saveload_cardtype == 2 && saveload_cardformatted != 0) {
            memcard_loadmessage_delay = 1.5f;
            if (memcard_savedatabuffer == NULL || memcard_savedatasize == 0) {
                memcard_loadneeded = 0;
                memcard_loadstarted = 0;
                memcard_loadfailed = 1;
                return;
            }
            STUBBED_NAMED("0x0050bc20: saveloadASLoad at 0x00543d90 (void, 3 arguments)");
            memcard_loadneeded = 0;
            memcard_loadcorrupt = 0;
            if (saveload_status == 1) {
                memcard_loadstarted = 0;
                memcard_loadfailed = 1;
            } else {
                memcard_loadstarted = 1;
                memcard_loadfailed = 0;
            }
        }
    } else if (memcard_loadstarted != 0 && saveload_status == 1) {
        memcard_loadstarted = 0;
        if (saveload_error == 0) {
            if (ChecksumSaveData(memcard_savedatabuffer, memcard_savedatasize) !=
                *(i32 *)((char *)memcard_savedatabuffer + memcard_savedatasize)) {
                memcard_loadcorrupt = 1;
                saveload_autosave = -1;
            } else {
                PCByteCopy_candidate(memcard_savedata, memcard_savedatabuffer, memcard_savedatasize);
                MenuLoadOccurred = 1;
            }
        } else {
            memcard_loadfailed = 1;
            saveload_autosave = -1;
        }
    }

    if (memcard_autosaveneeded != 0) {
        memcard_autosaveneeded = 0;
        if (saveload_autosave != -1) {
            memcard_autosavepredelay = 1.0f;
            memcard_autosavestarted = 1;
            memcard_autosaveinprogress = 1;
        }
    } else if (memcard_autosavestarted != 0 && memcard_saveneeded == 0 && memcard_savestarted == 0 &&
               memcard_autosavepredelay <= 0.0f) {
        memcard_saveneeded = 1;
        memcard_slot = saveload_autosave;
    }

    if (memcard_saveneeded != 0) {
        if (saveload_status == 1 && saveload_cardtype == 2 && saveload_cardformatted != 0) {
            i32 save_hash;
            memcard_savemessage_delay = 1.5f;
            if (memcard_savedatabuffer == NULL || memcard_savedatasize == 0) {
                memcard_saveneeded = 0;
                memcard_savestarted = 0;
                memcard_savefailed = 1;
                return;
            }
            save_hash = memcard_hashfn != NULL ? memcard_hashfn() : -1;
            if (memcard_slot != -1) {
                PCByteCopy_candidate(memcard_savedatabuffer, memcard_savedata, memcard_savedatasize);
                *(i32 *)((char *)memcard_savedatabuffer + memcard_savedatasize) =
                    ChecksumSaveData(memcard_savedatabuffer, memcard_savedatasize);
                STUBBED_NAMED("0x0050be2e: saveloadASSave at 0x00543d40 (void, 4 arguments)");
                (void)save_hash;
            }
            memcard_saveneeded = 0;
            memcard_savestarted = 1;
            memcard_savefailed = 0;
            memcard_justformatted = 0;
        }
    } else if (memcard_savestarted != 0 && saveload_status == 1) {
        memcard_savestarted = 0;
        if (saveload_error == 0) {
            MenuSaveOccurred = 1;
            if (savesuccessfn != NULL)
                savesuccessfn();
            if (memcard_autosavestarted != 0) {
                memcard_autosavestarted = 0;
                memcard_autosavepostdelay = 1.0f;
            }
        } else {
            memcard_savefailed = 1;
            if (saveload_autosave != -1) {
                saveload_autosave = -1;
                memcard_saveresult_delay = 1.5f;
                saveload_autosavedisabled = 1;
                memcard_autosavedisabled = 1;
            }
            memcard_autosavestarted = 0;
            memcard_autosaveinprogress = 0;
        }
    }

    if (memcard_deleteneeded != 0) {
        if (saveload_status == 1 && saveload_cardtype == 2 && saveload_cardformatted != 0) {
            STUBBED_NAMED("0x0050bee7: saveloadASDelete at 0x00543e30 (void, 1 argument)");
            memcard_deleteneeded = 0;
            memcard_deletemessage_delay = 1.5f;
            memcard_deletestarted = 1;
            memcard_deletefailed = 0;
        }
    } else if (memcard_deletestarted != 0 && saveload_status == 1) {
        memcard_deletefailed = saveload_error != 0;
        memcard_deletestarted = 0;
    }
    if (memcard_formatme != 0) {
        if (saveload_status == 1 && saveload_cardtype == 2) {
            STUBBED_NAMED("0x0050bf8b: saveloadASFormat at 0x00543e20 (void)");
            memcard_formatme = 0;
            memcard_formatmessage_delay = 1.5f;
            memcard_formatting = 1;
            memcard_formatfailed = 0;
            memcard_justformatted = 0;
        }
    } else if (memcard_formatting != 0 && saveload_status == 3) {
        memcard_formatting = 0;
        if (saveload_error == 0)
            memcard_justformatted = 1;
        else
            memcard_formatfailed = 1;
    }
}

void __cdecl loadsaveCallEachFrame(void) {
    STUBBED_NAMED("0x005114a0: saveloadASCallEachFrame at 0x00543de0 (void)");
    if (saveload_cardchanged != 0)
        memcard_cardchanged = 1;
    UpdateSaveSlots();
    memcard_slotsused = 0;
    if (saveload_savepresent != 0) {
        i32 slot;
        i32 used = 0;
        for (slot = 0; slot < SAVESLOTS; ++slot) {
            if (saveload_slotused[slot] != 0)
                ++used;
        }
        memcard_slotsused = used;
    }
}

void Text3D(char *text, f32 x, f32 y, f32 z, f32 x_scale, f32 y_scale, f32 z_scale, u32 alignment, u8 red, u8 green,
            u8 blue) {
    Text3DEx(text, x, y, z, x_scale, y_scale, z_scale, alignment, red, green, blue, 128);
}

typedef struct SAVE_MENU_VIEW_PC {
    f32 menu_time;
    f32 time_04;
    i16 selected_column;
    i16 selected_row;
    i16 first_column;
    i16 first_row;
    i16 last_column;
    i16 last_row;
    i16 menu;
    u8 unknown_16[0x26];
    i32 previous_item;
    u8 unknown_40[0x24];
    i32 confirm_pressed;
    i32 cancel_pressed;
    u8 unknown_6c[0x20];
    i32 draw_item;
    f32 item_scale;
    f32 draw_x;
    f32 draw_y;
    f32 draw_z;
    u8 unknown_a0[0x3c];
    i32 state;
} SAVE_MENU_VIEW_PC;
STATIC_ASSERT(sizeof(SAVE_MENU_VIEW_PC) == sizeof(MENU));
STATIC_ASSERT(offsetof(SAVE_MENU_VIEW_PC, menu_time) == 0);
STATIC_ASSERT(offsetof(SAVE_MENU_VIEW_PC, time_04) == 4);
STATIC_ASSERT(offsetof(SAVE_MENU_VIEW_PC, selected_column) == 8);
STATIC_ASSERT(offsetof(SAVE_MENU_VIEW_PC, selected_row) == 0xa);
STATIC_ASSERT(offsetof(SAVE_MENU_VIEW_PC, first_column) == 0xc);
STATIC_ASSERT(offsetof(SAVE_MENU_VIEW_PC, last_column) == 0x10);
STATIC_ASSERT(offsetof(SAVE_MENU_VIEW_PC, last_row) == 0x12);
STATIC_ASSERT(offsetof(SAVE_MENU_VIEW_PC, menu) == 0x14);
STATIC_ASSERT(offsetof(SAVE_MENU_VIEW_PC, previous_item) == 0x3c);
STATIC_ASSERT(offsetof(SAVE_MENU_VIEW_PC, confirm_pressed) == 0x64);
STATIC_ASSERT(offsetof(SAVE_MENU_VIEW_PC, cancel_pressed) == 0x68);
STATIC_ASSERT(offsetof(SAVE_MENU_VIEW_PC, state) == 0xdc);
STATIC_ASSERT(offsetof(MENUFNINFO, cancel_prompt) == 0x1a);

extern i32 Menu_DisableCancel;
extern i32 Menu_InHeaderSave;
extern i32 MenuKeyboardAlwaysPc;
extern i32 MenuKeyboardSecondaryPc;
extern i32 Menu_InWarningFlow;
extern i32 MenuASCancelFinished;
extern i32 MenuCardWarningState;
extern i32 Menu_Overwriteflag;
extern i32 MenuLoadStarted;
extern i32 saveload_freespace;
extern i32 SAVESIZE_ADDITIONAL;
extern i32 MENUSFX_MENUSELECT;
extern i32 MENUSFX_MENUNOENTRY;
extern i32 memcard_autosaveenabled;
void __cdecl BackupMenu(void);
void __cdecl BackupMenuNoFn(void);

void __cdecl MenuEnterSave(MENU *);
void __cdecl MenuEnterLoad(MENU *);

void __cdecl MenuEnterSaveConfirm(MENU *menu) {
    i32 slot = Menu_InHeaderSave != 0 ? SAVESLOTS : memcard_slot;
    (void)menu;
    Menu_Overwriteflag = saveload_slotused[slot] != 0;
}

void __cdecl MenuEnterAutoSaveWarning(MENU *menu) {
    (void)menu;
    memcard_autosaveenabled = 1;
    memcard_autosavedisabled = 0;
}

void __cdecl MenuEnterAutoSaveCancel(MENU *menu) {
    if (MenuASCancelFinished == 0) {
        memcard_autosaveenabled = 0;
        memcard_autosavedisabled = 0;
        ((SAVE_MENU_VIEW_PC *)menu)->last_row = 0;
    }
}

void __cdecl MenuEnterCardWarning(MENU *menu) {
    (void)menu;
    MenuCardWarningState = 0;
    memcard_cardchanged = 0;
}

void __cdecl MenuEnterHeaderSave(MENU *menu) {
    (void)menu;
    Menu_InHeaderSave = 1;
    BackupMenuNoFn();
    NewMenu(1000, -1, -1);
}

void __cdecl MenuEnterStartNewGame(MENU *menu) {
    (void)menu;
    memcard_autosaveenabled = 0;
    saveload_autosave = -1;
    BackupMenu();
}

void __cdecl MenuExitSave(MENU *menu) {
    (void)menu;
    MenuKeyboardAlwaysPc = 0;
}

void __cdecl MenuExitLoad(MENU *menu) {
    (void)menu;
    MenuKeyboardSecondaryPc = 0;
}

void __cdecl MenuExitCardWarning(MENU *menu) {
    (void)menu;
    Menu_InWarningFlow = 0;
}

void __cdecl MenuUpdateSave(MENU *menu) {
    SAVE_MENU_VIEW_PC *view = (SAVE_MENU_VIEW_PC *)menu;
    i32 slot;
    MenuInfo[menu->menu].cancel_prompt = Menu_DisableCancel == 0;
    if (memcard_cardchanged != 0 || saveload_cardtype != 2) {
        MenuEnterSave(menu);
    }
    slot = menu->selected_column - view->first_column;
    if (menu->confirm_pressed != 0) {
        MenuSFX = MENUSFX_MENUSELECT;
        if (menu->selected_row == 0) {
            if (saveload_slotused[slot] == 0 && saveload_freespace < SAVESIZE_ADDITIONAL) {
                MenuSFX = MENUSFX_MENUNOENTRY;
            } else {
                memcard_slot = slot;
                NewMenu(1008, 1, -1);
            }
        } else if (menu->selected_row == view->last_row && Menu_DisableCancel == 0) {
            NewMenu(1016, 1, -1);
        }
    } else if (menu->cancel_pressed != 0 && Menu_DisableCancel == 0) {
        MenuSFX = MENUSFX_MENUSELECT;
        NewMenu(1016, 1, -1);
    }
}

void __cdecl MenuUpdateInsertCard(MENU *menu) {
    if (menu->cancel_pressed != 0) {
        MenuSFX = MENUSFX_MENUSELECT;
        BackupMenuNoFn();
        NewMenu(1016, 1, -1);
    }
    if (menu->confirm_pressed != 0 && menu->selected_row == ((SAVE_MENU_VIEW_PC *)menu)->last_row) {
        MenuSFX = MENUSFX_MENUSELECT;
        BackupMenuNoFn();
        NewMenu(1016, 1, -1);
    }
}

void __cdecl MenuUpdateNoMemoryCard(MENU *menu) {
    if (menu->confirm_pressed != 0) {
        MenuSFX = MENUSFX_MENUSELECT;
        if (menu->selected_row == 0) {
            BackupMenu();
        } else {
            BackupMenuNoFn();
            NewMenu(1017, 1, -1);
        }
    }
    if (menu->cancel_pressed != 0) {
        MenuSFX = MENUSFX_MENUSELECT;
        BackupMenuNoFn();
        NewMenu(1017, 1, -1);
    }
}

void __cdecl MenuUpdateNotEnoughSpace(MENU *menu) {
    if (menu->cancel_pressed != 0) {
        MenuSFX = MENUSFX_MENUSELECT;
        BackupMenuNoFn();
        NewMenu(1016, 1, -1);
    }
}

void __cdecl MenuUpdateNoData(MENU *menu) {
    if (menu->confirm_pressed != 0) {
        MenuSFX = MENUSFX_MENUSELECT;
        BackupMenuNoFn();
        BackupMenu();
    }
    if (menu->cancel_pressed != 0) {
        MenuSFX = MENUSFX_MENUSELECT;
        BackupMenuNoFn();
        BackupMenu();
    }
}

void __cdecl MenuUpdateFormatConfirm(MENU *menu) {
    if (menu->confirm_pressed != 0) {
        MenuSFX = MENUSFX_MENUSELECT;
    } else if (menu->cancel_pressed != 0 && MenuInfo[GameMenu[GameMenuLevel - 1].menu].id != 1019) {
        MenuSFX = MENUSFX_MENUSELECT;
        BackupMenuNoFn();
        NewMenu(1018, 1, -1);
    }
}

void __cdecl MenuUpdateFileCorrupt(MENU *menu) {
    if (MenuInfo[GameMenu[GameMenuLevel - 1].menu].id != 1019)
        MenuInfo[menu->menu].cancel_prompt = 1;
    else
        MenuInfo[menu->menu].cancel_prompt = 0;
    if (menu->confirm_pressed != 0) {
        MenuSFX = MENUSFX_MENUSELECT;
        if (MenuInfo[GameMenu[GameMenuLevel - 1].menu].id == 1000) {
            BackupMenuNoFn();
            NewMenu(1016, 1, -1);
        } else if (MenuInfo[GameMenu[GameMenuLevel - 1].menu].id == 1019) {
            MenuCardWarningState = 3;
            memcard_cardchanged = 0;
            BackupMenuNoFn();
        } else {
            BackupMenuNoFn();
            NewMenu(1017, 1, -1);
        }
    } else if (menu->cancel_pressed != 0 && MenuInfo[GameMenu[GameMenuLevel - 1].menu].id != 1019) {
        MenuSFX = MENUSFX_MENUSELECT;
        if (MenuInfo[GameMenu[GameMenuLevel - 1].menu].id == 1000) {
            BackupMenuNoFn();
            NewMenu(1016, 1, -1);
        } else {
            BackupMenuNoFn();
            NewMenu(1017, 1, -1);
        }
    }
}

void __cdecl MenuUpdateFormatting(MENU *menu) {
    (void)menu;
    if (memcard_formatme != 0)
        memcard_formatmessage_delay = 1.5f;
    if (memcard_formatme != 0 || memcard_formatting != 0 || memcard_formatmessage_delay > 0.0f) {
        memcard_formatresult_delay = 1.5f;
        if (memcard_formatme != 0 && memcard_cardchanged != 0) {
            memcard_formatme = 0;
            memcard_formatfailed = 1;
        }
    } else if (memcard_formatresult_delay <= 0.0f) {
        BackupMenu();
    }
}

void __cdecl MenuUpdateDeleteConfirm(MENU *menu) {
    if (menu->confirm_pressed != 0) {
        MenuSFX = MENUSFX_MENUSELECT;
        if (menu->selected_row == 1) {
            BackupMenu();
        } else {
            memcard_slot = -1;
            memcard_deleteneeded = 1;
            memcard_deletestarted = 0;
            BackupMenuNoFn();
            BackupMenuNoFn();
            NewMenu(1015, 0, -1);
        }
    } else if (menu->cancel_pressed != 0) {
        MenuSFX = MENUSFX_MENUSELECT;
        BackupMenu();
    }
}

void __cdecl MenuUpdateDeleting(MENU *menu) {
    (void)menu;
    if (memcard_deleteneeded != 0 || memcard_deletemessage_delay > 0.0f) {
        memcard_deleteresult_delay = 1.5f;
        if (memcard_deleteneeded != 0 && memcard_cardchanged != 0) {
            memcard_deleteneeded = 0;
            memcard_deletefailed = 1;
        }
    } else if (memcard_deleteresult_delay <= 0.0f) {
        BackupMenu();
    }
}

void __cdecl MenuUpdateSaveConfirm(MENU *menu) {
    if (menu->confirm_pressed != 0) {
        MenuSFX = MENUSFX_MENUSELECT;
        if (menu->selected_row == 1) {
            BackupMenu();
        } else {
            memcard_saveneeded = 1;
            memcard_savestarted = 0;
            memcard_savefailed = 0;
            BackupMenuNoFn();
            NewMenu(1009, 0, -1);
        }
    } else if (menu->cancel_pressed != 0) {
        BackupMenu();
        MenuSFX = MENUSFX_MENUSELECT;
    }
}

void __cdecl MenuUpdateSaving(MENU *menu) {
    (void)menu;
    if (memcard_saveneeded != 0)
        memcard_savemessage_delay = 1.5f;
    if (memcard_saveneeded != 0 || memcard_savestarted != 0 || memcard_savemessage_delay > 0.0f) {
        memcard_saveresult_delay = 1.5f;
        if (memcard_saveneeded != 0 && (memcard_cardchanged != 0 || saveload_cardtype == -1)) {
            memcard_saveneeded = 0;
            memcard_savefailed = 1;
        }
    } else if (memcard_saveresult_delay <= 0.0f) {
        if (memcard_savefailed != 0) {
            BackupMenu();
        } else if (memcard_autosave != 0 && saveload_autosave != -1) {
            BackupMenuNoFn();
            BackupMenuNoFn();
            NewMenu(1010, 1, -1);
        } else {
            BackupMenuNoFn();
            BackupMenu();
        }
    }
}

void __cdecl MenuUpdateAutoSaveWarning(MENU *menu) {
    if (memcard_cardchanged != 0)
        BackupMenu();
    if (menu->confirm_pressed != 0) {
        MenuSFX = MENUSFX_MENUSELECT;
        BackupMenu();
    }
}

void __cdecl MenuUpdateLoad(MENU *menu) {
    if (memcard_cardchanged != 0 || saveload_cardtype != 2) {
        MenuEnterLoad(menu);
        return;
    }
    if (menu->confirm_pressed != 0) {
        MenuSFX = MENUSFX_MENUSELECT;
        if (menu->selected_row == 0) {
            memcard_slot = menu->selected_column - ((SAVE_MENU_VIEW_PC *)menu)->first_column;
            if (saveload_slotused[memcard_slot] != 0)
                NewMenu(1013, 1, -1);
            else
                MenuSFX = MENUSFX_MENUNOENTRY;
        } else if (menu->selected_row == ((SAVE_MENU_VIEW_PC *)menu)->last_row) {
            NewMenu(1017, 1, -1);
        }
    } else if (menu->cancel_pressed != 0) {
        NewMenu(1017, 1, -1);
        MenuSFX = MENUSFX_MENUSELECT;
    }
}

void __cdecl MenuUpdateLoadConfirm(MENU *menu) {
    if (menu->confirm_pressed != 0) {
        MenuSFX = MENUSFX_MENUSELECT;
        if (menu->selected_row == 1) {
            BackupMenu();
        } else {
            MenuLoadStarted = 1;
            memcard_loadneeded = 1;
            memcard_loadstarted = 0;
            memcard_loadfailed = 0;
            memcard_loadcorrupt = 0;
            BackupMenuNoFn();
            NewMenu(1014, 0, -1);
        }
    } else if (menu->cancel_pressed != 0) {
        MenuSFX = MENUSFX_MENUSELECT;
        BackupMenu();
    }
}

void __cdecl MenuUpdateLoading(MENU *menu) {
    (void)menu;
    if (memcard_loadneeded != 0)
        memcard_loadmessage_delay = 1.5f;
    if (memcard_loadneeded != 0 || memcard_loadstarted != 0 || memcard_loadmessage_delay > 0.0f) {
        memcard_loadresult_delay = 1.5f;
        if (memcard_loadneeded != 0 && (memcard_cardchanged != 0 || saveload_cardtype == -1)) {
            memcard_loadneeded = 0;
            memcard_loadfailed = 1;
        }
    } else if (memcard_loadresult_delay <= 0.0f) {
        if (memcard_loadfailed == 0 && memcard_loadcorrupt == 0) {
            if (memcard_autosave != 0 && saveload_autosave != -1) {
                BackupMenuNoFn();
                BackupMenuNoFn();
                NewMenu(1010, 1, -1);
                return;
            }
            BackupMenuNoFn();
        }
        BackupMenu();
    }
}

void __cdecl MenuUpdateSaveCancel(MENU *menu) {
    if (menu->confirm_pressed != 0) {
        if (menu->selected_row == 0)
            BackupMenuNoFn();
        BackupMenu();
        MenuSFX = MENUSFX_MENUSELECT;
    } else if (menu->cancel_pressed != 0) {
        BackupMenu();
        MenuSFX = MENUSFX_MENUSELECT;
    }
}

void __cdecl MenuUpdateLoadCancel(MENU *menu) {
    if (menu->confirm_pressed != 0) {
        if (menu->selected_row == 0)
            BackupMenuNoFn();
        BackupMenu();
        MenuSFX = MENUSFX_MENUSELECT;
    } else if (menu->cancel_pressed != 0) {
        BackupMenu();
        MenuSFX = MENUSFX_MENUSELECT;
    }
}

void __cdecl MenuUpdateFormatCancel(MENU *menu) {
    if (menu->confirm_pressed != 0) {
        MenuSFX = MENUSFX_MENUSELECT;
        if (menu->selected_row == 0) {
            i32 parent_id = MenuInfo[GameMenu[GameMenuLevel - 1].menu].id;
            if (parent_id == 1000) {
                BackupMenuNoFn();
                NewMenu(1016, 1, -1);
            } else if (parent_id == 1012) {
                BackupMenuNoFn();
                NewMenu(1017, 1, -1);
            } else {
                BackupMenu();
            }
        } else {
            BackupMenuNoFn();
            NewMenu(1006, 1, -1);
        }
    } else if (menu->cancel_pressed != 0) {
        MenuSFX = MENUSFX_MENUSELECT;
        BackupMenuNoFn();
        NewMenu(1006, 1, -1);
    }
}

STATIC_ASSERT(offsetof(SAVE_MENU_VIEW_PC, first_row) == 0xe);

static i32 SaveMenuLastSlotPc;
static i32 SaveMenuSlideLeftPc;
static i32 SaveMenuSlideRightPc;
extern i32 Menu_LastFlow;
extern i32 saveload_filecorrupt;
extern i32 SAVESIZE;
extern u8 MENUENTRYR, MENUENTRYG, MENUENTRYB;
extern u8 MENUFLASH0R, MENUFLASH0G, MENUFLASH0B;
extern u8 MENUFLASH1R, MENUFLASH1G, MENUFLASH1B;

void __cdecl MenuEnterLoad(MENU *menu) {
    SAVE_MENU_VIEW_PC *view = (SAVE_MENU_VIEW_PC *)menu;
    i32 last_column = SAVESLOTS;
    memcard_cardchanged = 0;
    if (last_column > 6)
        last_column = memcard_slotsused;
    view->last_column = (i16)(last_column - 1);
    view->state = 2;
    view->last_row = 1;
    SaveMenuLastSlotPc = SaveMenuSlideLeftPc = SaveMenuSlideRightPc = 0;
    MenuKeyboardSecondaryPc = 1;
    Menu_LastFlow = 3;
    MenuLoadOccurred = 0;
    MenuLoadStarted = 0;
    if (saveload_filecorrupt != 0)
        NewMenu(1020, -1, -1);
    else if (saveload_cardtype != 2)
        NewMenu(1004, -1, -1);
    else if (saveload_savepresent == 0 || saveload_cardformatted == 0)
        NewMenu(1005, -1, -1);
}

void __cdecl MenuEnterSave(MENU *menu) {
    SAVE_MENU_VIEW_PC *view = (SAVE_MENU_VIEW_PC *)menu;
    MenuSaveOccurred = 0;
    memcard_cardchanged = 0;
    Menu_DisableCancel = 0;
    MenuKeyboardAlwaysPc = 1;
    Menu_LastFlow = 2;
    if (SAVESLOTS <= 6) {
        view->last_column = (i16)(SAVESLOTS - 1);
    } else {
        view->last_column = (i16)memcard_slotsused;
        if (SAVESLOTS == memcard_slotsused || saveload_freespace < SAVESIZE_ADDITIONAL)
            --view->last_column;
    }
    view->state = 2;
    SaveMenuLastSlotPc = SaveMenuSlideLeftPc = SaveMenuSlideRightPc = 0;
    NuStrCpy(MenuHeader, apitxt_SAVEGAME);
    header_r = MENUHEADERR;
    header_g = MENUHEADERG;
    header_b = MENUHEADERB;
    view->last_row = 1;
    if (saveload_cardtype != 2)
        NewMenu(1002, -1, -1);
    else if (saveload_filecorrupt != 0)
        NewMenu(1020, -1, -1);
    else if (saveload_cardformatted == 0)
        NewMenu(1006, 1, -1);
    else if (saveload_savepresent == 0 && saveload_freespace < SAVESIZE)
        NewMenu(1003, -1, -1);
    else if (Menu_InHeaderSave != 0) {
        memcard_slot = -1;
        NewMenu(1008, 1, -1);
    }
}

void __cdecl APIMenuDrawMemCardSlots(MENU *menu, f32 y) {
    SAVE_MENU_VIEW_PC *view = (SAVE_MENU_VIEW_PC *)menu;
    i32 count = SAVESLOTS;
    i32 first, last, slot, highlight;
    i32 red, green, blue;
    f32 x, scale;
    if (SaveMenuSlideRightPc > 0)
        --SaveMenuSlideRightPc;
    if (SaveMenuSlideLeftPc > 0)
        --SaveMenuSlideLeftPc;
    if (count > 3) {
        if (view->selected_column != SaveMenuLastSlotPc) {
            if (view->selected_column < SaveMenuLastSlotPc)
                SaveMenuSlideRightPc = 10;
            else
                SaveMenuSlideLeftPc = 10;
            SaveMenuLastSlotPc = view->selected_column;
        }
        first = view->selected_column - 1;
        if (SaveMenuSlideLeftPc != 0)
            --first;
        last = view->selected_column + 1;
        if (SaveMenuSlideRightPc != 0)
            ++last;
    } else {
        first = 0;
        last = count - 1;
    }
    if (count > 6)
        count = memcard_slotsused;
    if (SAVESLOTS > 3)
        x = 0.0 - (view->selected_column - first) * 0.5 - SaveMenuSlideRightPc * 0.5 / 10.0 +
            SaveMenuSlideLeftPc * 0.5 / 10.0;
    else
        x = (SAVESLOTS - 1) * -0.25;
    for (slot = first; slot <= last; ++slot, x += 0.5) {
        highlight = view->selected_row == view->first_row && view->selected_column == slot &&
                    SaveMenuSlideLeftPc == 0 && SaveMenuSlideRightPc == 0;
        if (slot < 0 || slot > view->last_column)
            continue;
        if (slot < count) {
            drawslotinfofn(x, y, highlight, slot);
        } else {
            if (view->selected_column == count && view->selected_row == 0) {
                if (menu_flash != 0) {
                    red = MENUFLASH0R;
                    green = MENUFLASH0G;
                    blue = MENUFLASH0B;
                } else {
                    red = MENUFLASH1R;
                    green = MENUFLASH1G;
                    blue = MENUFLASH1B;
                }
                if (menu_pulsate > 0.0f) {
                    red = (i32)(MENUFLASH0R * (f64)menu_pulsate + MENUFLASH1R * (1.0 - menu_pulsate));
                    green = (i32)(MENUFLASH0G * (f64)menu_pulsate + MENUFLASH1G * (1.0 - menu_pulsate));
                    blue = (i32)(MENUFLASH0B * (f64)menu_pulsate + MENUFLASH1B * (1.0 - menu_pulsate));
                }
            } else {
                red = MENUENTRYR;
                green = MENUENTRYG;
                blue = MENUENTRYB;
            }
            MenuText3DEx(apitxt_NEWSAVE, x, y, 1.0f, (f32)(MenuTextScalePc * (f64)0.85f),
                         (f32)(MenuTextScalePc * (f64)0.85f), MenuTextScalePc, 0, (u8)red, (u8)green, (u8)blue,
                         (u8)MenuA);
        }
    }
    if (view->selected_row == 0) {
        if (menu_flash != 0) {
            red = MENUFLASH0R;
            green = MENUFLASH0G;
            blue = MENUFLASH0B;
        } else {
            red = MENUFLASH1R;
            green = MENUFLASH1G;
            blue = MENUFLASH1B;
        }
        if (menu_pulsate > 0.0f) {
            red = (i32)(MENUFLASH0R * (f64)menu_pulsate + MENUFLASH1R * (1.0 - menu_pulsate));
            green = (i32)(MENUFLASH0G * (f64)menu_pulsate + MENUFLASH1G * (1.0 - menu_pulsate));
            blue = (i32)(MENUFLASH0B * (f64)menu_pulsate + MENUFLASH1B * (1.0 - menu_pulsate));
        }
    } else {
        red = MENUENTRYR;
        green = MENUENTRYG;
        blue = MENUENTRYB;
    }
    scale = MenuTextScalePc + MenuTextScalePc;
    if (SaveMenuSlideLeftPc == 0 && SaveMenuSlideRightPc == 0) {
        if (first > 0)
            MenuText3DEx("<", -0.8f, y, 1.0f, scale, scale, scale, 0, (u8)red, (u8)green, (u8)blue, (u8)MenuA);
        if (view->last_column > last)
            MenuText3DEx(">", 0.8f, y, 1.0f, scale, scale, scale, 0, (u8)red, (u8)green, (u8)blue, (u8)MenuA);
    }
}

STATIC_ASSERT(offsetof(SAVE_MENU_VIEW_PC, draw_item) == 0x8c);
STATIC_ASSERT(offsetof(SAVE_MENU_VIEW_PC, item_scale) == 0x90);
STATIC_ASSERT(offsetof(SAVE_MENU_VIEW_PC, draw_x) == 0x94);
STATIC_ASSERT(offsetof(SAVE_MENU_VIEW_PC, draw_y) == 0x98);
STATIC_ASSERT(offsetof(SAVE_MENU_VIEW_PC, draw_z) == 0x9c);
extern f32 MENUBOTY;
extern u8 MENUWARNR, MENUWARNG, MENUWARNB;
void __cdecl DrawMenuEntryEx(MENU *, char *, i32);
void __cdecl MenuDrawSave(MENU *menu) {
    NuStrCpy(MenuHeader, apitxt_SAVEGAME);
    header_r = MENUHEADERR;
    header_g = MENUHEADERG;
    header_b = MENUHEADERB;
    if (drawslotsfn != NULL && MenuAlpha > (f64)0.2f && MenuStopDraw == 0)
        drawslotsfn(menu, -0.3f);
    ++((SAVE_MENU_VIEW_PC *)menu)->draw_item;
    if (Menu_DisableCancel == 0) {
        ((SAVE_MENU_VIEW_PC *)menu)->draw_y = MENUBOTY;
        DrawMenuEntryEx(menu, apitxt_CANCEL, (u8)MenuA);
    }
}

void __cdecl MenuDrawLoad(MENU *menu) {
    NuStrCpy(MenuHeader, apitxt_LOADGAME);
    header_r = MENUHEADERR;
    header_g = MENUHEADERG;
    header_b = MENUHEADERB;
    if (drawslotsfn != NULL && MenuAlpha > (f64)0.2f && MenuStopDraw == 0)
        drawslotsfn(menu, -0.2f);
    ++((SAVE_MENU_VIEW_PC *)menu)->draw_item;
    ((SAVE_MENU_VIEW_PC *)menu)->draw_y = MENUBOTY;
    DrawMenuEntryEx(menu, apitxt_CANCEL, (u8)MenuA);
}

void __cdecl MenuDrawNoMemoryCard(MENU *menu) {
    ((SAVE_MENU_VIEW_PC *)menu)->draw_y = MENUBOTY - MENUDY;
    DrawMenuEntryEx(menu, apitxt_RETRY, (u8)MenuA);
    DrawMenuEntryEx(menu, apitxt_CANCEL, (u8)MenuA);
}

void __cdecl MenuDrawDeleteConfirm(MENU *menu) {
    MenuSmartTextEx(apitxt_CONFIRMDELETE, 0.0f, 0.0f, 1.0f, MenuTextScalePc, MenuTextScalePc, MenuTextScalePc, 0,
                    MENUNORMALR, MENUNORMALG, MENUNORMALB, 1.2f, 2, NULL, 0, MenuA);
    ((SAVE_MENU_VIEW_PC *)menu)->draw_y = MENUBOTY - MENUDY;
    DrawMenuEntryEx(menu, apitxt_YES, (u8)MenuA);
    DrawMenuEntryEx(menu, apitxt_NO, (u8)MenuA);
}

void __cdecl MenuDrawSaveConfirm(MENU *menu) {
    NuStrCpy(MenuHeader, apitxt_SAVEGAME);
    header_r = MENUHEADERR;
    header_g = MENUHEADERG;
    header_b = MENUHEADERB;
    if (Menu_Overwriteflag != 0)
        MenuSmartTextEx(apitxt_CONFIRMOVERWRITE, 0.0f, -0.3f, 1.0f, MenuTextScalePc, MenuTextScalePc, MenuTextScalePc,
                        0, MENUNORMALR, MENUNORMALG, MENUNORMALB, 1.2f, 2, NULL, 0, MenuA);
    else
        MenuSmartTextEx(apitxt_CONFIRMSAVE, 0.0f, -0.3f, 1.0f, MenuTextScalePc, MenuTextScalePc, MenuTextScalePc, 0,
                        MENUNORMALR, MENUNORMALG, MENUNORMALB, 1.2f, 2, NULL, 0, MenuA);
    ((SAVE_MENU_VIEW_PC *)menu)->draw_y = MENUBOTY - MENUDY;
    DrawMenuEntryEx(menu, apitxt_YES, (u8)MenuA);
    DrawMenuEntryEx(menu, apitxt_NO, (u8)MenuA);
}

void __cdecl MenuDrawSaveCancel(MENU *menu) {
    NuStrCpy(MenuHeader, apitxt_SAVEGAME);
    header_r = MENUHEADERR;
    header_g = MENUHEADERG;
    header_b = MENUHEADERB;
    MenuSmartTextEx(apitxt_DOYOUWANTTOABORT, 0.0f, -0.3f, 1.0f, MenuTextScalePc, MenuTextScalePc, MenuTextScalePc, 0,
                    MENUNORMALR, MENUNORMALG, MENUNORMALB, 1.5f, 2, NULL, 0, MenuA);
    ((SAVE_MENU_VIEW_PC *)menu)->draw_y = MENUBOTY - MENUDY;
    DrawMenuEntryEx(menu, apitxt_YES, (u8)MenuA);
    DrawMenuEntryEx(menu, apitxt_NO, (u8)MenuA);
}

void __cdecl MenuDrawLoadCancel(MENU *menu) {
    NuStrCpy(MenuHeader, apitxt_LOADGAME);
    header_r = MENUHEADERR;
    header_g = MENUHEADERG;
    header_b = MENUHEADERB;
    MenuSmartTextEx(apitxt_DOYOUWANTTOABORTLOAD, 0.0f, -0.3f, 1.0f, MenuTextScalePc, MenuTextScalePc, MenuTextScalePc,
                    0, MENUNORMALR, MENUNORMALG, MENUNORMALB, 1.5f, 2, NULL, 0, MenuA);
    ((SAVE_MENU_VIEW_PC *)menu)->draw_y = MENUBOTY - MENUDY;
    DrawMenuEntryEx(menu, apitxt_YES, (u8)MenuA);
    DrawMenuEntryEx(menu, apitxt_NO, (u8)MenuA);
}

void __cdecl MenuDrawFormatCancel(MENU *menu) {
    MenuSmartTextEx(apitxt_DOYOUWANTTOABORTFORMAT, 0.0f, -0.3f, 1.0f, MenuTextScalePc, MenuTextScalePc, MenuTextScalePc,
                    0, MENUNORMALR, MENUNORMALG, MENUNORMALB, 1.5f, 2, NULL, 0, MenuA);
    ((SAVE_MENU_VIEW_PC *)menu)->draw_y = MENUBOTY - MENUDY;
    DrawMenuEntryEx(menu, apitxt_YES, (u8)MenuA);
    DrawMenuEntryEx(menu, apitxt_NO, (u8)MenuA);
}

void __cdecl MenuDrawLoadConfirm(MENU *menu) {
    NuStrCpy(MenuHeader, apitxt_LOADGAME);
    header_r = MENUHEADERR;
    header_g = MENUHEADERG;
    header_b = MENUHEADERB;
    MenuText3DEx(apitxt_CONFIRMLOAD, 0.0f, -0.3f, 1.0f, MenuTextScalePc, MenuTextScalePc, MenuTextScalePc, 0,
                 MENUNORMALR, MENUNORMALG, MENUNORMALB, (u8)MenuA);
    ((SAVE_MENU_VIEW_PC *)menu)->draw_y = MENUBOTY - MENUDY;
    DrawMenuEntryEx(menu, apitxt_YES, (u8)MenuA);
    DrawMenuEntryEx(menu, apitxt_NO, (u8)MenuA);
}

void __cdecl MenuDrawFileCorrupt(MENU *menu) {
    char message[1024];
    (void)menu;
    sprintf(message, apitxt_CORRUPTLOAD);
    MenuSmartTextEx(message, 0.0f, 0.0f, 1.0f, MenuTextScalePc, MenuTextScalePc, MenuTextScalePc, 0, MENUNORMALR,
                    MENUNORMALG, MENUNORMALB, 1.3f, 3, NULL, 0, MenuA);
}

void __cdecl APIMenuDrawGameState(f32 x, f32 y, i32 highlight, i32 slot) {
    char label[32];
    i32 red, green, blue;
    if (highlight != 0) {
        if (menu_flash != 0) {
            red = MENUFLASH0R;
            green = MENUFLASH0G;
            blue = MENUFLASH0B;
        } else {
            red = MENUFLASH1R;
            green = MENUFLASH1G;
            blue = MENUFLASH1B;
        }
        if (menu_pulsate > 0.0f) {
            red = (i32)(MENUFLASH0R * (f64)menu_pulsate + MENUFLASH1R * (1.0 - menu_pulsate));
            green = (i32)(MENUFLASH0G * (f64)menu_pulsate + MENUFLASH1G * (1.0 - menu_pulsate));
            blue = (i32)(MENUFLASH0B * (f64)menu_pulsate + MENUFLASH1B * (1.0 - menu_pulsate));
        }
    } else {
        red = MENUFLASH1R;
        green = MENUFLASH1G;
        blue = MENUFLASH1B;
    }
    sprintf(label, "%s %i", apitxt_SLOT, slot + 1);
    MenuText3DEx(label, x, y, 1.0f, MenuTextScalePc, MenuTextScalePc, MenuTextScalePc, 0, (u8)red, (u8)green, (u8)blue,
                 (u8)MenuA);
    if (saveload_slotused[slot] != 0) {
        MenuSmartTextEx(apitxt_PRESENT, x, (f32)(y + MenuTextScalePc * 0.5), 1.0f, MenuTextScalePc, MenuTextScalePc,
                        MenuTextScalePc, 0, MENUNORMALR, MENUNORMALG, MENUNORMALB, 0.4f, 2, NULL, 0, MenuA);
    } else if (saveload_freespace >= SAVESIZE_ADDITIONAL) {
        MenuSmartTextEx(apitxt_EMPTY, x, (f32)(y + MenuTextScalePc * 0.5), 1.0f, MenuTextScalePc, MenuTextScalePc,
                        MenuTextScalePc, 0, MENUNORMALR, MENUNORMALG, MENUNORMALB, 0.4f, 2, NULL, 0, MenuA);
    } else {
        MenuSmartTextEx(apitxt_NOSPACE, x, (f32)(y + MenuTextScalePc * 0.5), 1.0f, MenuTextScalePc, MenuTextScalePc,
                        MenuTextScalePc, 0, MENUWARNR, MENUWARNG, MENUWARNB, 0.4f, 2, NULL, 0, MenuA);
    }
}

static void MenuDrawNoDataMessagePc(void) {
    char message[1024];
    f32 scale = MenuTextScalePc;
    sprintf(message, apitxt_NODATAAVAILABLE, apiGameName);
    MenuSmartTextEx(message, 0.0f, -0.4f, 1.0f, scale, scale, scale, 0, MENUNORMALR, MENUNORMALG, MENUNORMALB, 1.5f, 3,
                    NULL, 0, MenuA);
}

void __cdecl MenuEnterNoMemoryCard(MENU *menu) {
    (void)menu;
}

void __cdecl MenuDrawNoData(MENU *menu) {
    MenuDrawNoDataMessagePc();
    ((SAVE_MENU_VIEW_PC *)menu)->draw_y = MENUBOTY;
    DrawMenuEntryEx(menu, apitxt_OK, (u8)MenuA);
}

static void MenuDrawLoadingMessagePc(void) {
    f32 scale = MenuTextScalePc;
    MenuSmartTextEx(apitxt_GAMELOADING, 0.0f, -0.4f, 1.0f, scale, scale, scale, 0, MENUNORMALR, MENUNORMALG,
                    MENUNORMALB, 1.5f, 2, NULL, 0, MenuA);
}
static void MenuDrawLoadedMessagePc(void) {
    f32 scale = MenuTextScalePc;
    MenuSmartTextEx(apitxt_GAMELOADED, 0.0f, -0.4f, 1.0f, scale, scale, scale, 0, MENUNORMALR, MENUNORMALG, MENUNORMALB,
                    1.5f, 2, NULL, 0, MenuA);
}
static void MenuDrawLoadFailedMessagePc(void) {
    f32 scale = MenuTextScalePc;
    MenuSmartTextEx(apitxt_FAILEDTOLOAD, 0.0f, 0.0f, 1.0f, scale, scale, scale, 0, MENUNORMALR, MENUNORMALG,
                    MENUNORMALB, 1.5f, 3, NULL, 0, MenuA);
}
static void MenuDrawLoadCorruptMessagePc(void) {
    f32 scale = MenuTextScalePc;
    MenuSmartTextEx(apitxt_CORRUPTLOAD, 0.0f, 0.0f, 1.0f, scale, scale, scale, 0, MENUNORMALR, MENUNORMALG, MENUNORMALB,
                    1.5f, 3, NULL, 0, MenuA);
}
static i32 MenuLoadResultShownPc;
void __cdecl MenuDrawLoading(MENU *menu) {
    (void)menu;
    NuStrCpy(MenuHeader, apitxt_LOADGAME);
    header_r = MENUHEADERR;
    header_g = MENUHEADERG;
    header_b = MENUHEADERB;
    if (memcard_loadneeded != 0 || memcard_loadstarted != 0 || memcard_loadmessage_delay > 0.0f) {
        MenuLoadResultShownPc = 0;
        MenuDrawLoadingMessagePc();
    } else {
        if (MenuLoadResultShownPc == 0) {
            MenuAlpha = 0.0f;
            MenuA = 0;
            MenuLoadResultShownPc = 1;
        }
        if (memcard_loadfailed != 0)
            MenuDrawLoadFailedMessagePc();
        else if (memcard_loadcorrupt != 0)
            MenuDrawLoadCorruptMessagePc();
        else
            MenuDrawLoadedMessagePc();
    }
}

void __cdecl MenuEnterInsertCard(MENU *menu) {
    (void)menu;
}

void __cdecl MenuDrawInsertCard(MENU *menu) {
    char message[1024];
    (void)menu;
    (void)message;
}

void __cdecl MenuDrawFormatConfirm(MENU *menu) {
    (void)menu;
}

static void MenuDrawNotEnoughSpaceMessagePc(void) {
    f32 scale = MenuTextScalePc;
    MenuSmartTextEx(apitxt_NOTENOUGHSPACE, 0.0f, 0.3f, 1.0f, scale, scale, scale, 0, MENUNORMALR, MENUNORMALG,
                    MENUNORMALB, 1.5f, 3, NULL, 0, MenuA);
}

static void MenuDrawSavingMessagePc(void) {
    f32 scale = MenuTextScalePc;
    MenuSmartTextEx(apitxt_GAMESAVING, 0.0f, -0.2f, 1.0f, scale, scale, scale, 0, MENUNORMALR, MENUNORMALG, MENUNORMALB,
                    1.5f, 2, NULL, 0, MenuA);
}

static void MenuDrawSavedMessagePc(void) {
    f32 scale = MenuTextScalePc;
    MenuSmartTextEx(apitxt_GAMESAVED, 0.0f, -0.2f, 1.0f, scale, scale, scale, 0, MENUNORMALR, MENUNORMALG, MENUNORMALB,
                    1.5f, 2, NULL, 0, MenuA);
}

static void MenuDrawDeletingMessagePc(void) {
    f32 scale = MenuTextScalePc;
    MenuSmartTextEx(apitxt_DELETING, 0.0f, 0.2f, 1.0f, scale, scale, scale, 0, MENUNORMALR, MENUNORMALG, MENUNORMALB,
                    1.5f, 2, NULL, 0, MenuA);
}

static void MenuDrawDeletedMessagePc(void) {
    f32 scale = MenuTextScalePc;
    MenuSmartTextEx(apitxt_DELETECOMPLETE, 0.0f, 0.0f, 1.0f, scale, scale, scale, 0, MENUNORMALR, MENUNORMALG,
                    MENUNORMALB, 1.5f, 3, NULL, 0, MenuA);
}

static void MenuDrawSaveFailedMessagePc(void) {
    f32 scale = MenuTextScalePc;
    if (Menu_Overwriteflag != 0)
        MenuSmartTextEx(apitxt_FAILEDTOOVERWRITE, 0.0f, 0.0f, 1.0f, scale, scale, scale, 0, MENUNORMALR, MENUNORMALG,
                        MENUNORMALB, 1.5f, 3, NULL, 0, MenuA);
    else
        MenuSmartTextEx(apitxt_FAILEDTOSAVE, 0.0f, 0.0f, 1.0f, scale, scale, scale, 0, MENUNORMALR, MENUNORMALG,
                        MENUNORMALB, 1.5f, 3, NULL, 0, MenuA);
}

static void MenuDrawAutoSaveWarningMessagePc(void) {
    char message[1024];
    f32 scale = MenuTextScalePc;
    if (memcard_drawasiconfn != NULL)
        memcard_drawasiconfn();
    sprintf(message, apitxt_AUTOSAVE_ENABLED, saveload_autosave + 1);
    MenuSmartTextEx(message, 0.0f, 0.45f, 1.0f, scale, scale, scale, 0, MENUNORMALR, MENUNORMALG, MENUNORMALB, 1.5f, 3,
                    NULL, 0, MenuA);
}

void __cdecl MenuDrawNotEnoughSpace(MENU *menu) {
    char message[1024];
    (void)menu;
    (void)message;
    MenuDrawNotEnoughSpaceMessagePc();
}

static i32 MenuSaveResultShownPc;
void __cdecl MenuDrawSaving(MENU *menu) {
    (void)menu;
    NuStrCpy(MenuHeader, apitxt_SAVEGAME);
    header_r = MENUHEADERR;
    header_g = MENUHEADERG;
    header_b = MENUHEADERB;
    if (memcard_saveneeded != 0 || memcard_savestarted != 0 || memcard_savemessage_delay > 0.0f) {
        MenuSaveResultShownPc = 0;
        MenuDrawSavingMessagePc();
    } else {
        if (MenuSaveResultShownPc == 0) {
            MenuAlpha = 0.0f;
            MenuA = 0;
            MenuSaveResultShownPc = 1;
        }
        if (memcard_savefailed != 0)
            MenuDrawSaveFailedMessagePc();
        else
            MenuDrawSavedMessagePc();
    }
}

static i32 MenuDeleteResultShownPc;
void __cdecl MenuDrawDeleting(MENU *menu) {
    (void)menu;
    NuStrCpy(MenuHeader, apitxt_DELETEGAME);
    header_r = MENUHEADERR;
    header_g = MENUHEADERG;
    header_b = MENUHEADERB;
    if (memcard_deleteneeded != 0 || memcard_deletestarted != 0 || memcard_deletemessage_delay > 0.0f) {
        MenuDeleteResultShownPc = 0;
        MenuDrawDeletingMessagePc();
    } else {
        if (MenuDeleteResultShownPc == 0) {
            MenuAlpha = 0.0f;
            MenuA = 0;
            MenuDeleteResultShownPc = 1;
        }
        if (memcard_deletefailed == 0)
            MenuDrawDeletedMessagePc();
    }
}

void __cdecl MenuDrawAutoSaveWarning(MENU *menu) {
    MenuDrawAutoSaveWarningMessagePc();
    ((SAVE_MENU_VIEW_PC *)menu)->draw_y = MENUBOTY;
    DrawMenuEntryEx(menu, apitxt_OK, (u8)MenuA);
}
