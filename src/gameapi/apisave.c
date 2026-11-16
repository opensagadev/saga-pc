#include "batman.h"
#include "gameapi/apisave.h"
#include "gameapi/apitext.h"
#include "gameapi/edtools/edui.h"
#include "gameapi/gui/apimenu.h"
#include "gameapi/gui/apimenu_internal.h"
#include "gameframework/saveload.h"
#include "globals.h"
#include "legoapi/audio/sfx.h"
#include "legoapi/items/collect/bolts.h"
#include "legoapi/menus/core/text.h"
#include "legoapi/menus/screens/gamemenuall.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nu3d/nuqfnt.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nucore/nutime.h"
#include <stdio.h>
#include "legoapi/audio/audio.h"
#include "legoapi/characters/core/players.h"
#include "legogame/game.h"
#include "nu2api/nuwin32/pcapi.h"

extern i32 saveload_slotused[];

i32 MenuSFX = -1;
u8 MENUENTRYG = 127;
u8 MENUENTRYB = 255;
u8 MENUFLASH0G = 255;
u8 MENUFLASH0B = 255;
u8 MENUFLASH1R = 127;
u8 MENUFLASH1B = 255;
char *apitxt_YES = "Yes";
char *apitxt_NO = "No";
char *apitxt_GAMESAVING = "Saving game...";
char *apitxt_GAMELOADING = "Loading game...";
char *apitxt_FAILEDTOLOAD = "Failed to Load";
char *apitxt_FAILEDTOSAVE = "Failed to Save";
char *apitxt_FAILEDTOOVERWRITE = "Failed to Overwrite";
char *apitxt_NODATAAVAILABLE = "No %s data present";
char *apitxt_AUTOSAVE_ENABLED = "Autosave will now use Game %i";
char *apitxt_FORMATTINGCOMPLETE = "Formatting complete";
char *apitxt_DELETECOMPLETE = "Delete complete";
char *apitxt_NOTENOUGHSPACE = "Insufficient drive space to save games.";
i32 SAVESLOTS = 6;
f32 memcard_autosavecanceldelay = 1.5f;

void SaveSystemInitialise(i32 slots, SAVE_HASH_FN hashfn, void *save, i32 save_size, i32 autosave, SAVE_VOID_FN draw_save_icon) {
    SAVESLOTS = slots;
    if (slots >= 6) {
        SAVESLOTS = 6;
    }
    memcard_hashfn = hashfn;
    memcard_savedata = save;
    memcard_savedatasize = save_size;
    memcard_savedatabuffer = NuAlloc(save_size + 4, "d:\\projects\\game_saga\\gameapi.saga\\gui\\gamemenu\\apisave.c", 0x6d);
    memcard_autosave = autosave;
    memcard_drawasiconfn = draw_save_icon;
}

void SaveSystemInitialiseEx(i32 slots, SAVE_HASH_FN hashfn, void *save, i32 save_size, void *header, i32 header_size, i32 autosave,
                            SAVE_VOID_FN draw_save_icon) {
    memcard_hashfn = hashfn;
    memcard_savedata = save;
    memcard_savedatasize = save_size;
    memcard_savedatabuffer = NuAlloc(save_size + 4, "d:\\projects\\game_saga\\gameapi.saga\\gui\\gamemenu\\apisave.c", 0x7f);
    memcard_autosave = autosave;
    memcard_drawasiconfn = draw_save_icon;
    memcard_headerdata = header;
    memcard_headerdatasize = header_size;
    memcard_headerdatabuffer = NuAlloc(header_size + 4, "d:\\projects\\game_saga\\gameapi.saga\\gui\\gamemenu\\apisave.c", 0x84);
    if (header != NULL) {
        SAVESLOTS = slots < 5 ? slots : 5;
    } else {
        SAVESLOTS = slots < 6 ? slots : 6;
    }
}

void SetSaveSuccessFn(SAVE_VOID_FN callback) {
    savesuccessfn = callback;
}

i32 ChecksumSaveData(void *buffer, i32 size) {
    u32 sum;
    i32 i;
    sum = 0x5c0999;
    for (i = 0; i < size / 4; i++) {
        sum += ((u32 *)buffer)[i];
    }
    return (i32)sum;
}

/* Counts down a message delay, then the result delay that follows it. */
static void AdvanceSaveDelay(f32 *message, f32 *result, f32 elapsed) {
    if (*message > 0.0f) {
        *message -= elapsed;
    } else if (*result > 0.0f) {
        *result -= elapsed;
    }
}

void UpdateSaveSlots(void) {
    const f32 elapsed = NuTimeGetFrameTime();
    if (memcard_autosavecanceldelay > 0.0f) {
        memcard_autosavecanceldelay -= elapsed;
    }
    AdvanceSaveDelay(&memcard_loadmessage_delay, &memcard_loadresult_delay, elapsed);
    AdvanceSaveDelay(&memcard_savemessage_delay, &memcard_saveresult_delay, elapsed);
    if (memcard_autosavepredelay > 0.0f) {
        memcard_autosavepredelay -= elapsed;
    } else if (memcard_autosavepostdelay > 0.0f) {
        memcard_autosavepostdelay -= elapsed;
        if (memcard_autosavepostdelay <= 0.0f) {
            memcard_autosaveinprogress = 0;
        }
    }
    AdvanceSaveDelay(&memcard_formatmessage_delay, &memcard_formatresult_delay, elapsed);
    AdvanceSaveDelay(&memcard_createmessage_delay, &memcard_createresult_delay, elapsed);
    AdvanceSaveDelay(&memcard_deletemessage_delay, &memcard_deleteresult_delay, elapsed);
    AdvanceSaveDelay(&memcard_message_delay, &memcard_result_delay, elapsed);

    if (saveload_cardtype != 2) {
        memcard_justformatted = 0;
    }
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
            saveloadASLoad(memcard_slot, memcard_savedatabuffer, memcard_savedatasize + 4);
            memcard_loadneeded = 0;
            memcard_loadcorrupt = 0;
            if (saveload_status != 1) {
                memcard_loadstarted = 1;
                memcard_loadfailed = 0;
            } else {
                memcard_loadstarted = 0;
                memcard_loadfailed = 1;
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
                PCByteCopy(memcard_savedata, memcard_savedatabuffer, memcard_savedatasize);
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
    } else if (memcard_autosavestarted != 0 && memcard_saveneeded == 0 && memcard_savestarted == 0 && memcard_autosavepredelay <= 0.0f) {
        memcard_slot = saveload_autosave;
        memcard_saveneeded = 1;
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
                PCByteCopy(memcard_savedatabuffer, memcard_savedata, memcard_savedatasize);
                *(i32 *)((char *)memcard_savedatabuffer + memcard_savedatasize) =
                    ChecksumSaveData(memcard_savedatabuffer, memcard_savedatasize);
                saveloadASSave(memcard_slot, memcard_savedatabuffer, memcard_savedatasize + 4, (u32)save_hash);
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
            if (savesuccessfn != NULL) {
                savesuccessfn();
            }
            if (memcard_autosavestarted != 0) {
                memcard_autosavestarted = 0;
                memcard_autosavepostdelay = 1.0f;
            }
        } else {
            memcard_savefailed = 1;
            if (saveload_autosave != -1) {
                saveload_autosavedisabled = 1;
                memcard_saveresult_delay = 1.5f;
                saveload_autosave = -1;
                memcard_autosavedisabled = 1;
            }
            memcard_autosavestarted = 0;
            memcard_autosaveinprogress = 0;
        }
    }

    if (memcard_deleteneeded != 0) {
        if (saveload_status == 1 && saveload_cardtype == 2 && saveload_cardformatted != 0) {
            saveloadASDelete(memcard_slot);
            memcard_deleteneeded = 0;
            memcard_deletemessage_delay = 1.5f;
            memcard_deletestarted = 1;
            memcard_deletefailed = 0;
        }
    } else if (memcard_deletestarted != 0 && saveload_status == 1) {
        memcard_deletestarted = 0;
        memcard_deletefailed = 0;
        if (saveload_error != 0) {
            memcard_deletefailed = 1;
        }
    }
    if (memcard_formatme != 0) {
        if (saveload_status == 1 && saveload_cardtype == 2) {
            saveloadASFormat();
            memcard_formatme = 0;
            memcard_formatmessage_delay = 1.5f;
            memcard_formatting = 1;
            memcard_formatfailed = 0;
            memcard_justformatted = 0;
        }
    } else if (memcard_formatting != 0 && saveload_status == 3) {
        memcard_formatting = 0;
        if (saveload_error == 0) {
            memcard_justformatted = 1;
        } else {
            memcard_formatfailed = 1;
        }
    }
}

void loadsaveCallEachFrame(void) {
    saveloadASCallEachFrame();
    if (saveload_cardchanged != 0) {
        memcard_cardchanged = 1;
    }
    UpdateSaveSlots();
    memcard_slotsused = 0;
    if (saveload_savepresent != 0) {
        i32 i;
        for (i = 0; i < SAVESLOTS; ++i) {
            if (saveload_slotused[i] != 0) {
                ++memcard_slotsused;
            }
        }
    }
}

void Text3D(char *text, f32 x, f32 y, f32 z, f32 x_scale, f32 y_scale, f32 z_scale, u32 alignment, u8 red, u8 green, u8 blue) {
    Text3DEx(text, x, y, z, x_scale, y_scale, z_scale, alignment, red, green, blue, 128);
}

void MenuEnterSaveConfirm(MENU *menu) {
    i32 slot = Menu_InHeaderSave != 0 ? SAVESLOTS : memcard_slot;
    Menu_Overwriteflag = saveload_slotused[slot] != 0;
}

void MenuEnterAutoSaveWarning(MENU *menu) {
    memcard_autosaveenabled = 1;
    memcard_autosavedisabled = 0;
}

void MenuEnterAutoSaveCancel(MENU *menu) {
    if (MenuASCancelFinished == 0) {
        memcard_autosaveenabled = 0;
        memcard_autosavedisabled = 0;
        menu->last_row = 0;
    }
}

void MenuEnterCardWarning(MENU *menu) {
    MenuCardWarningState = 0;
    memcard_cardchanged = 0;
}

void MenuEnterHeaderSave(MENU *menu) {
    Menu_InHeaderSave = 1;
    BackupMenuNoFn();
    NewMenu(MENU_SAVE, -1, -1);
}

void MenuEnterStartNewGame(MENU *menu) {
    memcard_autosaveenabled = 0;
    saveload_autosave = -1;
    BackupMenu();
}

void MenuExitSave(MENU *menu) {
    Menu_InSaveFlow = 0;
}

void MenuExitLoad(MENU *menu) {
    Menu_InLoadFlow = 0;
}

void MenuExitCardWarning(MENU *menu) {
    Menu_InWarningFlow = 0;
}

void fun_0050a7a0(MENU *menu) {
}

void fun_0050a7b0(MENU *menu) {
}

void fun_0050a7c0(MENU *menu) {
}

void fun_0050a7d0(MENU *menu) {
}

void fun_0050a7e0(MENU *menu) {
}

void MenuDrawDoNotRemoveCard(MENU *menu) {
}

i32 SAVESIZE_ADDITIONAL = 3;
i32 saveload_cardtype = 2;
i32 saveload_freespace = 2048;
i32 saveload_cardformatted = 1;
i32 saveload_autosave = -1;

void MenuUpdateSave(MENU *menu) {
    i32 slot;
    if (Menu_DisableCancel != 0) {
        MenuInfo[menu->menu].cancel_prompt = 0;
    } else {
        MenuInfo[menu->menu].cancel_prompt = 1;
    }
    if (memcard_cardchanged != 0 || saveload_cardtype != 2) {
        MenuEnterSave(menu);
    }
    slot = menu->selected_column - menu->first_column;
    if (menu->confirm_pressed != 0) {
        MenuSFX = MENUSFX_MENUSELECT;
        if (menu->selected_row == 0) {
            if (saveload_slotused[slot] == 0 && saveload_freespace < SAVESIZE_ADDITIONAL) {
                MenuSFX = MENUSFX_MENUNOENTRY;
            } else {
                memcard_slot = slot;
                NewMenu(MENU_SAVE_CONFIRM, 1, -1);
            }
        } else if (menu->selected_row == menu->last_row && Menu_DisableCancel == 0) {
            NewMenu(MENU_SAVE_CANCEL, 1, -1);
        }
    } else if (menu->cancel_pressed != 0 && Menu_DisableCancel == 0) {
        MenuSFX = MENUSFX_MENUSELECT;
        NewMenu(MENU_SAVE_CANCEL, 1, -1);
    }
}

void MenuUpdateDoNotRemoveCard(MENU *menu) {
    if (menu->menu_time > 2.0f && saveload_status == 1) {
        BackupMenu();
    }
}

void MenuUpdateInsertCard(MENU *menu) {
    if (menu->cancel_pressed != 0) {
        MenuSFX = MENUSFX_MENUSELECT;
        BackupMenuNoFn();
        NewMenu(MENU_SAVE_CANCEL, 1, -1);
    }
    if (menu->confirm_pressed != 0 && menu->selected_row == menu->last_row) {
        MenuSFX = MENUSFX_MENUSELECT;
        BackupMenuNoFn();
        NewMenu(MENU_SAVE_CANCEL, 1, -1);
    }
}

void MenuUpdateNoMemoryCard(MENU *menu) {
    if (menu->confirm_pressed != 0) {
        MenuSFX = MENUSFX_MENUSELECT;
        if (menu->selected_row == 0) {
            BackupMenu();
        } else {
            BackupMenuNoFn();
            NewMenu(MENU_LOAD_CANCEL, 1, -1);
        }
    }
    if (menu->cancel_pressed != 0) {
        MenuSFX = MENUSFX_MENUSELECT;
        BackupMenuNoFn();
        NewMenu(MENU_LOAD_CANCEL, 1, -1);
    }
}

void MenuUpdateNotEnoughSpace(MENU *menu) {
    if (menu->cancel_pressed != 0) {
        MenuSFX = MENUSFX_MENUSELECT;
        BackupMenuNoFn();
        NewMenu(MENU_SAVE_CANCEL, 1, -1);
    }
}

void MenuUpdateNoData(MENU *menu) {
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

void MenuUpdateFormatConfirm(MENU *menu) {
    if (menu->confirm_pressed != 0) {
        MenuSFX = MENUSFX_MENUSELECT;
    } else if (menu->cancel_pressed != 0 && MenuInfo[GameMenu[GameMenuLevel - 1].menu].id != MENU_CARD_WARNING) {
        MenuSFX = MENUSFX_MENUSELECT;
        BackupMenuNoFn();
        NewMenu(MENU_FORMAT_CANCEL, 1, -1);
    }
}

void MenuUpdateFileCorrupt(MENU *menu) {
    i32 cancel;
    if (MenuInfo[GameMenu[GameMenuLevel - 1].menu].id == MENU_CARD_WARNING) {
        MenuInfo[menu->menu].cancel_prompt = 0;
    } else {
        MenuInfo[menu->menu].cancel_prompt = 1;
    }
    cancel = 0;
    if (menu->confirm_pressed != 0) {
        MenuSFX = MENUSFX_MENUSELECT;
        cancel = 1;
    } else if (menu->cancel_pressed != 0 && MenuInfo[GameMenu[GameMenuLevel - 1].menu].id != MENU_CARD_WARNING) {
        MenuSFX = MENUSFX_MENUSELECT;
        cancel = 1;
    }
    if (cancel != 0) {
        if (MenuInfo[GameMenu[GameMenuLevel - 1].menu].id == MENU_SAVE) {
            BackupMenuNoFn();
            NewMenu(MENU_SAVE_CANCEL, 1, -1);
        } else if (MenuInfo[GameMenu[GameMenuLevel - 1].menu].id == MENU_CARD_WARNING) {
            MenuCardWarningState = 3;
            memcard_cardchanged = 0;
            BackupMenuNoFn();
        } else {
            BackupMenuNoFn();
            NewMenu(MENU_LOAD_CANCEL, 1, -1);
        }
    }
}

void MenuUpdateFormatting(MENU *menu) {
    if (memcard_formatme != 0) {
        memcard_formatmessage_delay = 1.5f;
    }
    if (memcard_formatme != 0 || memcard_formatting != 0 || memcard_formatmessage_delay > 0.0f) {
        memcard_formatresult_delay = 1.5f;
        if (memcard_formatme != 0 && memcard_cardchanged != 0) {
            memcard_formatme = 0;
            memcard_formatfailed = 1;
        }
    }
    if (memcard_formatresult_delay <= 0.0f) {
        BackupMenu();
    }
}

void MenuUpdateDeleteConfirm(MENU *menu) {
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
            NewMenu(MENU_DELETING, 0, -1);
        }
    } else if (menu->cancel_pressed != 0) {
        MenuSFX = MENUSFX_MENUSELECT;
        BackupMenu();
    }
}

void MenuUpdateDeleting(MENU *menu) {
    if (memcard_deleteneeded != 0 || memcard_deletemessage_delay > 0.0f) {
        memcard_deleteresult_delay = 1.5f;
        if (memcard_deleteneeded != 0 && memcard_cardchanged != 0) {
            memcard_deleteneeded = 0;
            memcard_deletefailed = 1;
        }
    }
    if (memcard_deleteresult_delay <= 0.0f) {
        BackupMenu();
    }
}

void MenuUpdateSaveConfirm(MENU *menu) {
    if (menu->confirm_pressed != 0) {
        MenuSFX = MENUSFX_MENUSELECT;
        if (menu->selected_row == 1) {
            BackupMenu();
        } else {
            memcard_saveneeded = 1;
            memcard_savestarted = 0;
            memcard_savefailed = 0;
            BackupMenuNoFn();
            NewMenu(MENU_SAVING, 0, -1);
        }
    } else if (menu->cancel_pressed != 0) {
        BackupMenu();
        MenuSFX = MENUSFX_MENUSELECT;
    }
}

void MenuUpdateSaving(MENU *menu) {
    if (memcard_saveneeded != 0) {
        memcard_savemessage_delay = 1.5f;
    }
    if (memcard_saveneeded != 0 || memcard_savestarted != 0 || memcard_savemessage_delay > 0.0f) {
        memcard_saveresult_delay = 1.5f;
        if (memcard_saveneeded != 0 && (memcard_cardchanged != 0 || saveload_cardtype == -1)) {
            memcard_saveneeded = 0;
            memcard_savefailed = 1;
        }
    }
    if (memcard_saveresult_delay <= 0.0f) {
        if (memcard_savefailed != 0) {
            BackupMenu();
        } else if (memcard_autosave != 0 && saveload_autosave != -1) {
            BackupMenuNoFn();
            BackupMenuNoFn();
            NewMenu(MENU_AUTO_SAVE_WARNING, 1, -1);
        } else {
            BackupMenuNoFn();
            BackupMenu();
        }
    }
}

void MenuUpdateAutoSaveWarning(MENU *menu) {
    if (memcard_cardchanged != 0) {
        BackupMenu();
    }
    if (menu->confirm_pressed != 0) {
        MenuSFX = MENUSFX_MENUSELECT;
        BackupMenu();
    }
}

void MenuUpdateLoad(MENU *menu) {
    if (memcard_cardchanged != 0 || saveload_cardtype != 2) {
        MenuEnterLoad(menu);
        return;
    }
    if (menu->confirm_pressed != 0) {
        MenuSFX = MENUSFX_MENUSELECT;
        if (menu->selected_row == 0) {
            memcard_slot = menu->selected_column - menu->first_column;
            if (saveload_slotused[memcard_slot] != 0) {
                NewMenu(MENU_LOAD_CONFIRM, 1, -1);
            } else {
                MenuSFX = MENUSFX_MENUNOENTRY;
            }
        } else if (menu->selected_row == menu->last_row) {
            NewMenu(MENU_LOAD_CANCEL, 1, -1);
        }
    } else if (menu->cancel_pressed != 0) {
        NewMenu(MENU_LOAD_CANCEL, 1, -1);
        MenuSFX = MENUSFX_MENUSELECT;
    }
}

void MenuUpdateLoadConfirm(MENU *menu) {
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
            NewMenu(MENU_LOADING, 0, -1);
        }
    } else if (menu->cancel_pressed != 0) {
        MenuSFX = MENUSFX_MENUSELECT;
        BackupMenu();
    }
}

void MenuUpdateLoading(MENU *menu) {
    if (memcard_loadneeded != 0) {
        memcard_loadmessage_delay = 1.5f;
    }
    if (memcard_loadneeded != 0 || memcard_loadstarted != 0 || memcard_loadmessage_delay > 0.0f) {
        memcard_loadresult_delay = 1.5f;
        if (memcard_loadneeded != 0 && (memcard_cardchanged != 0 || saveload_cardtype == -1)) {
            memcard_loadneeded = 0;
            memcard_loadfailed = 1;
        }
    }
    if (memcard_loadresult_delay <= 0.0f) {
        if (memcard_loadfailed == 0 && memcard_loadcorrupt == 0) {
            if (memcard_autosave != 0 && saveload_autosave != -1) {
                BackupMenuNoFn();
                BackupMenuNoFn();
                NewMenu(MENU_AUTO_SAVE_WARNING, 1, -1);
                return;
            }
            BackupMenuNoFn();
        }
        BackupMenu();
    }
}

void MenuUpdateSaveCancel(MENU *menu) {
    if (menu->confirm_pressed != 0) {
        if (menu->selected_row == 0) {
            BackupMenuNoFn();
            BackupMenu();
            MenuSFX = MENUSFX_MENUSELECT;
        } else {
            BackupMenu();
            MenuSFX = MENUSFX_MENUSELECT;
        }
    } else if (menu->cancel_pressed != 0) {
        BackupMenu();
        MenuSFX = MENUSFX_MENUSELECT;
    }
}

void MenuUpdateLoadCancel(MENU *menu) {
    if (menu->confirm_pressed != 0) {
        if (menu->selected_row == 0) {
            BackupMenuNoFn();
            BackupMenu();
            MenuSFX = MENUSFX_MENUSELECT;
        } else {
            BackupMenu();
            MenuSFX = MENUSFX_MENUSELECT;
        }
    } else if (menu->cancel_pressed != 0) {
        BackupMenu();
        MenuSFX = MENUSFX_MENUSELECT;
    }
}

void MenuUpdateFormatCancel(MENU *menu) {
    if (menu->confirm_pressed != 0) {
        if (menu->selected_row == 0) {
            MenuSFX = MENUSFX_MENUSELECT;
            if (MenuInfo[GameMenu[GameMenuLevel - 1].menu].id == MENU_SAVE) {
                BackupMenuNoFn();
                NewMenu(MENU_SAVE_CANCEL, 1, -1);
            } else if (MenuInfo[GameMenu[GameMenuLevel - 1].menu].id == MENU_LOAD) {
                BackupMenuNoFn();
                NewMenu(MENU_LOAD_CANCEL, 1, -1);
            } else {
                BackupMenu();
            }
        } else {
            MenuSFX = MENUSFX_MENUSELECT;
            BackupMenuNoFn();
            NewMenu(MENU_FORMAT_CONFIRM, 1, -1);
        }
    } else if (menu->cancel_pressed != 0) {
        MenuSFX = MENUSFX_MENUSELECT;
        BackupMenuNoFn();
        NewMenu(MENU_FORMAT_CONFIRM, 1, -1);
    }
}

void MenuUpdateCardWarning(MENU *menu) {
    menu->previous_item = MenuCardWarningState;
    switch (MenuCardWarningState) {
        case 7:
            if (menu->confirm_pressed != 0) {
                MenuCardWarningState = 3;
                MenuSFX = MENUSFX_MENUSELECT;
                BackupMenu();
                return;
            }
            break;
        case 0:
            if (saveload_status == 1) {
                Menu_InWarningFlow = 1;
                Menu_LastFlow = 1;
                MenuCardWarningState = 3;
                BackupMenu();
                return;
            }
            break;
    }
    if (MenuCardWarningState == 3) {
        BackupMenu();
        return;
    }
    if (MenuCardWarningState != menu->previous_item) {
        if (MenuCardWarningState != 0) {
            MenuAlpha = 0.0f;
            MenuA = 0;
        }
        menu->time_04 = 0.0f;
    }
}

i32 fun_0050b300(void) {
    return memcard_slot;
}

i32 menu_flash = 0;
i32 header_g = 0;
i32 header_b = 0;
char MenuHeader[64] = {0};
i32 header_r = 0;
MENU GameMenu[10] = {0};
i32 MenuStopDraw = 0;
i32 MenuLoadOccurred = 0;
i32 MenuLoadStarted = 0;
i32 MenuSaveOccurred = 0;
i32 Menu_Overwriteflag = 0;
i32 MenuCardWarningState = 0;
i32 MenuASCancelFinished = 0;
i32 MenuValidated = 0;
i32 Menu_InSaveFlow = 0;
i32 Menu_InLoadFlow = 0;
i32 Menu_InWarningFlow = 0;
i32 Menu_InHeaderSave = 0;
i32 Menu_LastFlow = 0;
f32 MenuAlpha = 0;
i32 MenuA = 0;
f32 menu_pulsate = 0.0f;
i32 Menu_DisableCancel = 0;
static i32 lastslot = 0;
static i32 slideleft = 0;
static i32 slideright = 0;
NUMTL *MenuFadeMtl = 0;
i32 CurrentMenuId = 0;
i32 MenuSuppressUpHeld = 0;
i32 MenuSuppressDownHeld = 0;
i32 MenuSuppressLeftHeld = 0;
i32 MenuSuppressRightHeld = 0;
f32 sfx_wait = 0.0f;
u8 MENUNORMALB = 0;
u8 MENUENTRYR = 0;
u8 MENUHEADERR = 0;
u8 MENUWARNG = 0;
u8 MENUWARNB = 0;
u8 MENUFLASH0R = 0;
u8 MENUFLASH1G = 0;
i32 dme_rgb = 0;
i32 dme_align = 0;
i32 memcard_slot = 0;
i32 memcard_loadneeded = 0;
i32 memcard_loadstarted = 0;
i32 memcard_loadfailed = 0;
i32 memcard_loadcorrupt = 0;
i32 memcard_saveneeded = 0;
i32 memcard_savestarted = 0;
i32 memcard_savefailed = 0;
i32 memcard_autosaveneeded = 0;
i32 memcard_autosavestarted = 0;
f32 memcard_autosavepredelay = 0;
f32 memcard_autosavepostdelay = 0;
i32 memcard_autosaveinprogress = 0;
i32 memcard_autosavedisabled = 0;
i32 memcard_deleteneeded = 0;
i32 memcard_deletestarted = 0;
i32 memcard_deletefailed = 0;
i32 memcard_formatme = 0;
i32 memcard_formatting = 0;
i32 memcard_formatfailed = 0;
i32 memcard_justformatted = 0;
f32 memcard_loadmessage_delay = 0.0f;
f32 memcard_savemessage_delay = 0.0f;
f32 memcard_formatmessage_delay = 0.0f;
f32 memcard_createmessage_delay = 0.0f;
f32 memcard_deletemessage_delay = 0.0f;
f32 memcard_loadresult_delay = 0.0f;
f32 memcard_saveresult_delay = 0.0f;
f32 memcard_formatresult_delay = 0.0f;
f32 memcard_createresult_delay = 0.0f;
f32 memcard_deleteresult_delay = 0.0f;
f32 memcard_message_delay = 0.0f;
f32 memcard_result_delay = 0.0f;
i32 memcard_autosaveenabled = 0;
i32 memcard_cardchanged = 0;
void *memcard_savedata = NULL;
void *memcard_savedatabuffer = NULL;
i32 memcard_savedatasize = 0;
i32 memcard_autosave = 0;
SAVE_HASH_FN memcard_hashfn = NULL;
void (*memcard_drawasiconfn)(void) = 0;
void *memcard_headerdata = NULL;
void *memcard_headerdatabuffer = NULL;
i32 memcard_headerdatasize = 0;
i32 memcard_slotsused = 0;
SAVE_VOID_FN savesuccessfn = NULL;
VUFNT *QFont2D = 0;
VUFNT *QFont2DLower = 0;
VUFNT *QFont2DZ = NULL;
VUFNT *QFont3D = NULL;
VUFNT *QFont3DZ = 0;
VUFNT *SmartTextFont = 0;
VUFNT *QFont2DButtons = 0;
i32 create_qfont2dlower = 0;
i32 create_qfont2dz = 0;
i32 create_qfont3d = 0;
i32 create_qfont3dz = 0;
f32 TextCrawlTime = 0.0f;
i32 followon_line = 0;
i32 TextPrintMode = 0;
void (*buttonmapfn)(char *, char *) = 0;
i32 smarttext_fwn = 0;
i32 MessageMtlInit = 0;

void MenuEnterLoad(MENU *menu) {
    i32 last_column = SAVESLOTS;
    memcard_cardchanged = 0;
    if (last_column > 6) {
        last_column = memcard_slotsused;
    }
    menu->last_column = (i16)(last_column - 1);
    menu->state = 2;
    menu->last_row = 1;
    slideright = slideleft = lastslot = 0;
    Menu_InLoadFlow = 1;
    Menu_LastFlow = 3;
    MenuLoadOccurred = 0;
    MenuLoadStarted = 0;
    if (saveload_filecorrupt != 0) {
        NewMenu(MENU_FILE_CORRUPT, -1, -1);
    } else if (saveload_cardtype != 2) {
        NewMenu(MENU_NO_MEMORY_CARD, -1, -1);
    } else if (saveload_savepresent == 0 || saveload_cardformatted == 0) {
        NewMenu(MENU_NO_DATA, -1, -1);
    }
}

i32 SAVESIZE = 50;

void MenuEnterSave(MENU *menu) {
    MenuSaveOccurred = 0;
    memcard_cardchanged = 0;
    Menu_DisableCancel = 0;
    Menu_InSaveFlow = 1;
    Menu_LastFlow = 2;
    if (SAVESLOTS <= 6) {
        menu->last_column = (i16)(SAVESLOTS - 1);
    } else {
        menu->last_column = (i16)memcard_slotsused;
        if (memcard_slotsused == SAVESLOTS || saveload_freespace < SAVESIZE_ADDITIONAL) {
            --menu->last_column;
        }
    }
    menu->state = 2;
    slideright = slideleft = lastslot = 0;
    NuStrCpy(MenuHeader, apitxt_SAVEGAME);
    menu->last_row = 1;
    header_r = MENUHEADERR;
    header_g = MENUHEADERG;
    header_b = MENUHEADERB;
    if (saveload_cardtype != 2) {
        NewMenu(MENU_INSERT_CARD, -1, -1);
    } else if (saveload_filecorrupt != 0) {
        NewMenu(MENU_FILE_CORRUPT, -1, -1);
    } else if (saveload_cardformatted == 0) {
        NewMenu(MENU_FORMAT_CONFIRM, 1, -1);
    } else if (saveload_savepresent == 0 && saveload_freespace < SAVESIZE) {
        NewMenu(MENU_NOT_ENOUGH_SPACE, -1, -1);
    } else if (Menu_InHeaderSave != 0) {
        memcard_slot = -1;
        NewMenu(MENU_SAVE_CONFIRM, 1, -1);
    }
}

void APIMenuDrawMemCardSlots(MENU *menu, f32 y) {
    i32 count;
    i32 first;
    i32 last;
    i32 slot;
    i32 highlight;
    i32 red;
    i32 green;
    i32 blue;
    f32 x;
    f32 scale;
    if (slideright > 0) {
        --slideright;
    }
    if (slideleft > 0) {
        --slideleft;
    }
    if (SAVESLOTS > 3) {
        if (menu->selected_column != lastslot) {
            if (lastslot > menu->selected_column) {
                slideright = 10;
            } else {
                slideleft = 10;
            }
            lastslot = menu->selected_column;
        }
        first = menu->selected_column - 1;
        if (slideleft != 0) {
            --first;
        }
        last = menu->selected_column + 1;
        if (slideright != 0) {
            ++last;
        }
    } else {
        first = 0;
        last = SAVESLOTS - 1;
    }
    if (SAVESLOTS <= 6) {
        count = SAVESLOTS;
    } else {
        count = memcard_slotsused;
    }
    slot = first;
    if (SAVESLOTS <= 3) {
        x = (SAVESLOTS - 1) * -0.25;
    } else {
        x = 0.0 - (menu->selected_column - first) * 0.5 - slideright * 0.5 / 10.0 + slideleft * 0.5 / 10.0;
    }
    for (; slot <= last; ++slot, x += 0.5) {
        highlight = menu->selected_row == menu->first_row && menu->selected_column == slot && slideleft == 0 && slideright == 0;
        if (slot < 0 || slot > menu->last_column) {
            continue;
        }
        if (slot < count) {
            drawslotinfofn(x, y, highlight, slot);
        } else {
            if (menu->selected_column == count && menu->selected_row == 0) {
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
            MenuText3DEx(apitxt_NEWSAVE, x, y, 1.0f, (f32)(MenuTextScale * (f64)0.85f), (f32)(MenuTextScale * (f64)0.85f), MenuTextScale, 0,
                         red, green, blue, (u8)MenuA);
        }
    }
    if (menu->selected_row == 0) {
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
    scale = MenuTextScale + MenuTextScale;
    if (slideleft == 0 && slideright == 0) {
        if (first > 0) {
            MenuText3DEx("<", -0.8f, y, 1.0f, scale, scale, scale, 0, red, green, blue, (u8)MenuA);
        }
        if (last < menu->last_column) {
            MenuText3DEx(">", 0.8f, y, 1.0f, scale, scale, scale, 0, red, green, blue, (u8)MenuA);
        }
    }
}

void fun_005108c0(MENU *menu) {
    /* Like MenuDrawCardWarning: the text is not drawn, its buffer keeps the security cookie. */
    char text[8];
    if (0) {
        sprintf(text, "%d", menu->menu);
    }
}

void fun_005108e0(MENU *menu) {
}

void MenuDrawSave(MENU *menu) {
    NuStrCpy(MenuHeader, apitxt_SAVEGAME);
    header_r = MENUHEADERR;
    header_g = MENUHEADERG;
    header_b = MENUHEADERB;
    if (drawslotsfn != NULL && MenuAlpha > (f64)0.2f && MenuStopDraw == 0) {
        drawslotsfn(menu, -0.3f);
    }
    ++menu->draw_item;
    if (Menu_DisableCancel == 0) {
        menu->draw_y = MENUBOTY;
        DrawMenuEntryEx(menu, apitxt_CANCEL, (u8)MenuA);
    }
}

void MenuDrawLoad(MENU *menu) {
    NuStrCpy(MenuHeader, apitxt_LOADGAME);
    header_r = MENUHEADERR;
    header_g = MENUHEADERG;
    header_b = MENUHEADERB;
    if (drawslotsfn != NULL && MenuAlpha > (f64)0.2f && MenuStopDraw == 0) {
        drawslotsfn(menu, -0.2f);
    }
    ++menu->draw_item;
    menu->draw_y = MENUBOTY;
    DrawMenuEntryEx(menu, apitxt_CANCEL, (u8)MenuA);
}

void MenuDrawNoMemoryCard(MENU *menu) {
    menu->draw_y = MENUBOTY - MENUDY;
    DrawMenuEntryEx(menu, apitxt_RETRY, (u8)MenuA);
    DrawMenuEntryEx(menu, apitxt_CANCEL, (u8)MenuA);
}

void MenuDrawDeleteConfirm(MENU *menu) {
    MenuSmartTextEx(apitxt_CONFIRMDELETE, 0.0f, 0.0f, 1.0f, MenuTextScale, MenuTextScale, MenuTextScale, 0, MENUNORMALR, MENUNORMALG,
                    MENUNORMALB, 1.2f, 2, NULL, 0, MenuA);
    menu->draw_y = MENUBOTY - MENUDY;
    DrawMenuEntryEx(menu, apitxt_YES, (u8)MenuA);
    DrawMenuEntryEx(menu, apitxt_NO, (u8)MenuA);
}

void MenuDrawSaveConfirm(MENU *menu) {
    NuStrCpy(MenuHeader, apitxt_SAVEGAME);
    header_r = MENUHEADERR;
    header_g = MENUHEADERG;
    header_b = MENUHEADERB;
    if (Menu_Overwriteflag != 0) {
        MenuSmartTextEx(apitxt_CONFIRMOVERWRITE, 0.0f, -0.3f, 1.0f, MenuTextScale, MenuTextScale, MenuTextScale, 0, MENUNORMALR,
                        MENUNORMALG, MENUNORMALB, 1.2f, 2, NULL, 0, MenuA);
    } else {
        MenuSmartTextEx(apitxt_CONFIRMSAVE, 0.0f, -0.3f, 1.0f, MenuTextScale, MenuTextScale, MenuTextScale, 0, MENUNORMALR, MENUNORMALG,
                        MENUNORMALB, 1.2f, 2, NULL, 0, MenuA);
    }
    menu->draw_y = MENUBOTY - MENUDY;
    DrawMenuEntryEx(menu, apitxt_YES, (u8)MenuA);
    DrawMenuEntryEx(menu, apitxt_NO, (u8)MenuA);
}

void MenuDrawSaveCancel(MENU *menu) {
    NuStrCpy(MenuHeader, apitxt_SAVEGAME);
    header_r = MENUHEADERR;
    header_g = MENUHEADERG;
    header_b = MENUHEADERB;
    MenuSmartTextEx(apitxt_DOYOUWANTTOABORT, 0.0f, -0.3f, 1.0f, MenuTextScale, MenuTextScale, MenuTextScale, 0, MENUNORMALR, MENUNORMALG,
                    MENUNORMALB, 1.5f, 2, NULL, 0, MenuA);
    menu->draw_y = MENUBOTY - MENUDY;
    DrawMenuEntryEx(menu, apitxt_YES, (u8)MenuA);
    DrawMenuEntryEx(menu, apitxt_NO, (u8)MenuA);
}

void MenuDrawLoadCancel(MENU *menu) {
    NuStrCpy(MenuHeader, apitxt_LOADGAME);
    header_r = MENUHEADERR;
    header_g = MENUHEADERG;
    header_b = MENUHEADERB;
    MenuSmartTextEx(apitxt_DOYOUWANTTOABORTLOAD, 0.0f, -0.3f, 1.0f, MenuTextScale, MenuTextScale, MenuTextScale, 0, MENUNORMALR,
                    MENUNORMALG, MENUNORMALB, 1.5f, 2, NULL, 0, MenuA);
    menu->draw_y = MENUBOTY - MENUDY;
    DrawMenuEntryEx(menu, apitxt_YES, (u8)MenuA);
    DrawMenuEntryEx(menu, apitxt_NO, (u8)MenuA);
}

void MenuDrawFormatCancel(MENU *menu) {
    MenuSmartTextEx(apitxt_DOYOUWANTTOABORTFORMAT, 0.0f, -0.3f, 1.0f, MenuTextScale, MenuTextScale, MenuTextScale, 0, MENUNORMALR,
                    MENUNORMALG, MENUNORMALB, 1.5f, 2, NULL, 0, MenuA);
    menu->draw_y = MENUBOTY - MENUDY;
    DrawMenuEntryEx(menu, apitxt_YES, (u8)MenuA);
    DrawMenuEntryEx(menu, apitxt_NO, (u8)MenuA);
}

void MenuDrawCardWarning(MENU *menu) {
    /* The shipping build draws no warning text; the buffer it was formatted in still gives the
       function its security cookie. */
    char text[8];
    if (0) {
        sprintf(text, "%d", menu->menu);
    }
}

void MenuDrawLoadConfirm(MENU *menu) {
    NuStrCpy(MenuHeader, apitxt_LOADGAME);
    header_r = MENUHEADERR;
    header_g = MENUHEADERG;
    header_b = MENUHEADERB;
    MenuText3DEx(apitxt_CONFIRMLOAD, 0.0f, -0.3f, 1.0f, MenuTextScale, MenuTextScale, MenuTextScale, 0, MENUNORMALR, MENUNORMALG,
                 MENUNORMALB, (u8)MenuA);
    menu->draw_y = MENUBOTY - MENUDY;
    DrawMenuEntryEx(menu, apitxt_YES, (u8)MenuA);
    DrawMenuEntryEx(menu, apitxt_NO, (u8)MenuA);
}

void MenuDrawFileCorrupt(MENU *menu) {
    char message[1024];
    sprintf(message, apitxt_CORRUPTLOAD);
    MenuSmartTextEx(message, 0.0f, 0.0f, 1.0f, MenuTextScale, MenuTextScale, MenuTextScale, 0, MENUNORMALR, MENUNORMALG, MENUNORMALB, 1.3f,
                    3, NULL, 0, MenuA);
}

void APIMenuDrawGameState(f32 x, f32 y, i32 highlight, i32 slot) {
    char label[32];
    i32 red;
    i32 green;
    i32 blue;
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
    MenuText3DEx(label, x, y, 1.0f, MenuTextScale, MenuTextScale, MenuTextScale, 0, red, green, blue, (u8)MenuA);
    if (saveload_slotused[slot] != 0) {
        MenuSmartTextEx(apitxt_PRESENT, x, (f32)(y + MenuTextScale * 0.5), 1.0f, MenuTextScale, MenuTextScale, MenuTextScale, 0,
                        MENUNORMALR, MENUNORMALG, MENUNORMALB, 0.4f, 2, NULL, 0, MenuA);
    } else if (saveload_freespace >= SAVESIZE_ADDITIONAL) {
        MenuSmartTextEx(apitxt_EMPTY, x, (f32)(y + MenuTextScale * 0.5), 1.0f, MenuTextScale, MenuTextScale, MenuTextScale, 0, MENUNORMALR,
                        MENUNORMALG, MENUNORMALB, 0.4f, 2, NULL, 0, MenuA);
    } else {
        MenuSmartTextEx(apitxt_NOSPACE, x, (f32)(y + MenuTextScale * 0.5), 1.0f, MenuTextScale, MenuTextScale, MenuTextScale, 0, MENUWARNR,
                        MENUWARNG, MENUWARNB, 0.4f, 2, NULL, 0, MenuA);
    }
}

static void Draw_NODATAAVAILABLE(void) {
    char message[1024];
    f32 scale = MenuTextScale;
    sprintf(message, apitxt_NODATAAVAILABLE, apiGameName);
    MenuSmartTextEx(message, 0.0f, -0.4f, 1.0f, scale, scale, scale, 0, MENUNORMALR, MENUNORMALG, MENUNORMALB, 1.5f, 3, NULL, 0, MenuA);
}

void MenuEnterNoMemoryCard(MENU *menu) {
}

void MenuDrawNoData(MENU *menu) {
    Draw_NODATAAVAILABLE();
    menu->draw_y = MENUBOTY;
    DrawMenuEntryEx(menu, apitxt_CANCEL, (u8)MenuA);
}

static void MenuDrawLoadingMessage(void) {
    f32 scale = MenuTextScale;
    MenuSmartTextEx(apitxt_GAMELOADING, 0.0f, -0.4f, 1.0f, scale, scale, scale, 0, MENUNORMALR, MENUNORMALG, MENUNORMALB, 1.5f, 2, NULL, 0,
                    MenuA);
}

static void MenuDrawLoadedMessage(void) {
    f32 scale = MenuTextScale;
    MenuSmartTextEx(apitxt_GAMELOADED, 0.0f, -0.4f, 1.0f, scale, scale, scale, 0, MENUNORMALR, MENUNORMALG, MENUNORMALB, 1.5f, 2, NULL, 0,
                    MenuA);
}

static void MenuDrawLoadFailedMessage(void) {
    f32 scale = MenuTextScale;
    MenuSmartTextEx(apitxt_FAILEDTOLOAD, 0.0f, 0.0f, 1.0f, scale, scale, scale, 0, MENUNORMALR, MENUNORMALG, MENUNORMALB, 1.5f, 3, NULL, 0,
                    MenuA);
}

static void MenuDrawLoadCorruptMessage(void) {
    f32 scale = MenuTextScale;
    MenuSmartTextEx(apitxt_CORRUPTLOAD, 0.0f, 0.0f, 1.0f, scale, scale, scale, 0, MENUNORMALR, MENUNORMALG, MENUNORMALB, 1.5f, 3, NULL, 0,
                    MenuA);
}

static void MenuDrawFormattingMessage(void) {
    f32 scale = MenuTextScale;
    MenuSmartTextEx(apitxt_FORMATTING, 0.0f, 0.2f, 1.0f, scale, scale, scale, 0, MENUNORMALR, MENUNORMALG, MENUNORMALB, 1.5f, 2, NULL, 0,
                    MenuA);
}

static void MenuDrawFormattedMessage(void) {
    f32 scale = MenuTextScale;
    MenuSmartTextEx(apitxt_FORMATTINGCOMPLETE, 0.0f, 0.0f, 1.0f, scale, scale, scale, 0, MENUNORMALR, MENUNORMALG, MENUNORMALB, 1.5f, 3,
                    NULL, 0, MenuA);
}

static i32 MenuLoadResultShown = 0;
i32 saveload_error = 0;
i32 saveload_cardchanged = 0;
i32 saveload_autosavedisabled = 0;
i32 saveload_filecorrupt = 0;

void MenuDrawLoading(MENU *menu) {
    NuStrCpy(MenuHeader, apitxt_LOADGAME);
    header_r = MENUHEADERR;
    header_g = MENUHEADERG;
    header_b = MENUHEADERB;
    if (memcard_loadneeded != 0 || memcard_loadstarted != 0 || memcard_loadmessage_delay > 0.0f) {
        MenuLoadResultShown = 0;
        MenuDrawLoadingMessage();
    } else if (memcard_loadfailed != 0) {
        if (MenuLoadResultShown == 0) {
            MenuAlpha = 0.0f;
            MenuA = 0;
            MenuLoadResultShown = 1;
        }
        MenuDrawLoadFailedMessage();
    } else if (memcard_loadcorrupt != 0) {
        if (MenuLoadResultShown == 0) {
            MenuAlpha = 0.0f;
            MenuA = 0;
            MenuLoadResultShown = 1;
        }
        MenuDrawLoadCorruptMessage();
    } else {
        if (MenuLoadResultShown == 0) {
            MenuAlpha = 0.0f;
            MenuA = 0;
            MenuLoadResultShown = 1;
        }
        MenuDrawLoadedMessage();
    }
}

void MenuEnterInsertCard(MENU *menu) {
}

void MenuDrawInsertCard(MENU *menu) {
    char message[1024];
    if (0) {
        sprintf(message, "");
    }
}

void MenuDrawFormatConfirm(MENU *menu) {
}

static void MenuDrawNotEnoughSpaceMessage(void) {
    f32 scale = MenuTextScale;
    MenuSmartTextEx(apitxt_NOTENOUGHSPACE, 0.0f, 0.3f, 1.0f, scale, scale, scale, 0, MENUNORMALR, MENUNORMALG, MENUNORMALB, 1.5f, 3, NULL,
                    0, MenuA);
}

static void MenuDrawSavingMessage(void) {
    f32 scale = MenuTextScale;
    MenuSmartTextEx(apitxt_GAMESAVING, 0.0f, -0.2f, 1.0f, scale, scale, scale, 0, MENUNORMALR, MENUNORMALG, MENUNORMALB, 1.5f, 2, NULL, 0,
                    MenuA);
}

static void MenuDrawSavedMessage(void) {
    f32 scale = MenuTextScale;
    MenuSmartTextEx(apitxt_GAMESAVED, 0.0f, -0.2f, 1.0f, scale, scale, scale, 0, MENUNORMALR, MENUNORMALG, MENUNORMALB, 1.5f, 2, NULL, 0,
                    MenuA);
}

static void MenuDrawDeletingMessage(void) {
    f32 scale = MenuTextScale;
    MenuSmartTextEx(apitxt_DELETING, 0.0f, 0.2f, 1.0f, scale, scale, scale, 0, MENUNORMALR, MENUNORMALG, MENUNORMALB, 1.5f, 2, NULL, 0,
                    MenuA);
}

static void MenuDrawDeletedMessage(void) {
    f32 scale = MenuTextScale;
    MenuSmartTextEx(apitxt_DELETECOMPLETE, 0.0f, 0.0f, 1.0f, scale, scale, scale, 0, MENUNORMALR, MENUNORMALG, MENUNORMALB, 1.5f, 3, NULL,
                    0, MenuA);
}

static void MenuDrawSaveFailedMessage(void) {
    f32 scale = MenuTextScale;
    if (Menu_Overwriteflag != 0) {
        MenuSmartTextEx(apitxt_FAILEDTOOVERWRITE, 0.0f, 0.0f, 1.0f, scale, scale, scale, 0, MENUNORMALR, MENUNORMALG, MENUNORMALB, 1.5f, 3,
                        NULL, 0, MenuA);
    } else {
        MenuSmartTextEx(apitxt_FAILEDTOSAVE, 0.0f, 0.0f, 1.0f, scale, scale, scale, 0, MENUNORMALR, MENUNORMALG, MENUNORMALB, 1.5f, 3, NULL,
                        0, MenuA);
    }
}

static void Draw_AUTOSAVEWARNING(void) {
    char message[1024];
    f32 scale = MenuTextScale;
    if (memcard_drawasiconfn != NULL) {
        memcard_drawasiconfn();
    }
    sprintf(message, apitxt_AUTOSAVE_ENABLED, saveload_autosave + 1);
    MenuSmartTextEx(message, 0.0f, 0.45f, 1.0f, scale, scale, scale, 0, MENUNORMALR, MENUNORMALG, MENUNORMALB, 1.5f, 3, NULL, 0, MenuA);
}

void MenuDrawNotEnoughSpace(MENU *menu) {
    char message[1024];
    if (0) {
        sprintf(message, "");
    }
    MenuDrawNotEnoughSpaceMessage();
}

static i32 MenuFormattingResultShown = 0;

void MenuDrawFormatting(MENU *menu) {
    if (memcard_formatting != 0 || memcard_formatme != 0 || memcard_formatmessage_delay > 0.0f) {
        MenuFormattingResultShown = 0;
        MenuDrawFormattingMessage();
        return;
    }
    if (memcard_formatfailed != 0) {
        if (MenuFormattingResultShown == 0) {
            MenuAlpha = 0.0f;
            MenuA = 0;
            MenuFormattingResultShown = 1;
        }
        return;
    }
    if (MenuFormattingResultShown == 0) {
        MenuAlpha = 0.0f;
        MenuA = 0;
        MenuFormattingResultShown = 1;
    }
    MenuDrawFormattedMessage();
}

static i32 MenuSaveResultShown = 0;

void MenuDrawSaving(MENU *menu) {
    NuStrCpy(MenuHeader, apitxt_SAVEGAME);
    header_r = MENUHEADERR;
    header_g = MENUHEADERG;
    header_b = MENUHEADERB;
    if (memcard_saveneeded != 0 || memcard_savestarted != 0 || memcard_savemessage_delay > 0.0f) {
        MenuSaveResultShown = 0;
        MenuDrawSavingMessage();
    } else if (memcard_savefailed != 0) {
        if (MenuSaveResultShown == 0) {
            MenuAlpha = 0.0f;
            MenuA = 0;
            MenuSaveResultShown = 1;
        }
        MenuDrawSaveFailedMessage();
    } else {
        if (MenuSaveResultShown == 0) {
            MenuAlpha = 0.0f;
            MenuA = 0;
            MenuSaveResultShown = 1;
        }
        MenuDrawSavedMessage();
    }
}

static i32 MenuDeleteResultShown = 0;

void MenuDrawDeleting(MENU *menu) {
    NuStrCpy(MenuHeader, apitxt_DELETEGAME);
    header_r = MENUHEADERR;
    header_g = MENUHEADERG;
    header_b = MENUHEADERB;
    if (memcard_deleteneeded != 0 || memcard_deletestarted != 0 || memcard_deletemessage_delay > 0.0f) {
        MenuDeleteResultShown = 0;
        MenuDrawDeletingMessage();
    } else if (memcard_deletefailed != 0) {
        if (MenuDeleteResultShown == 0) {
            MenuAlpha = 0.0f;
            MenuA = 0;
            MenuDeleteResultShown = 1;
        }
    } else {
        if (MenuDeleteResultShown == 0) {
            MenuAlpha = 0.0f;
            MenuA = 0;
            MenuDeleteResultShown = 1;
        }
        MenuDrawDeletedMessage();
    }
}

void MenuDrawAutoSaveWarning(MENU *menu) {
    Draw_AUTOSAVEWARNING();
    menu->draw_y = MENUBOTY;
    DrawMenuEntryEx(menu, apitxt_OK, (u8)MenuA);
}

MENUFNINFO MenuInfo[100] = {
    {MENU_SAVE, MenuEnterSave, MenuDrawSave, MenuUpdateSave, MenuExitSave, -1, -1, 0, 1, 1, 0},
    {MENU_DO_NOT_REMOVE_CARD, NULL, MenuDrawDoNotRemoveCard, MenuUpdateDoNotRemoveCard, NULL, -1, -1, 0, 0, 0, 0},
    {MENU_INSERT_CARD, MenuEnterInsertCard, MenuDrawInsertCard, MenuUpdateInsertCard, NULL, -1, -1, 0, 1, 1, 0},
    {MENU_NO_MEMORY_CARD, MenuEnterNoMemoryCard, MenuDrawNoMemoryCard, MenuUpdateNoMemoryCard, NULL, -1, -1, 0, 1, 1, 0},
    {MENU_NOT_ENOUGH_SPACE, NULL, MenuDrawNotEnoughSpace, MenuUpdateNotEnoughSpace, NULL, -1, -1, 0, 1, 1, 0},
    {MENU_NO_DATA, NULL, MenuDrawNoData, MenuUpdateNoData, NULL, -1, -1, 0, 1, 1, 0},
    {MENU_FORMAT_CONFIRM, NULL, MenuDrawFormatConfirm, MenuUpdateFormatConfirm, NULL, -1, -1, 0, 1, 1, 0},
    {MENU_FORMATTING, NULL, MenuDrawFormatting, MenuUpdateFormatting, NULL, -1, -1, 0, 0, 0, 0},
    {MENU_SAVE_CONFIRM, MenuEnterSaveConfirm, MenuDrawSaveConfirm, MenuUpdateSaveConfirm, NULL, -1, -1, 0, 1, 1, 0},
    {MENU_SAVING, NULL, MenuDrawSaving, MenuUpdateSaving, NULL, -1, -1, 0, 0, 0, 0},
    {MENU_AUTO_SAVE_WARNING, MenuEnterAutoSaveWarning, MenuDrawAutoSaveWarning, MenuUpdateAutoSaveWarning, NULL, -1, -1, 0, 1, 0, 0},
    {MENU_AUTO_SAVE_CANCEL, MenuEnterAutoSaveCancel, MenuDrawAutoSaveCancel, MenuUpdateAutoSaveCancel, NULL, -1, -1, 0, 1, 1, 0},
    {MENU_LOAD, MenuEnterLoad, MenuDrawLoad, MenuUpdateLoad, MenuExitLoad, -1, -1, 0, 1, 1, 0},
    {MENU_LOAD_CONFIRM, NULL, MenuDrawLoadConfirm, MenuUpdateLoadConfirm, NULL, -1, -1, 0, 1, 1, 0},
    {MENU_LOADING, NULL, MenuDrawLoading, MenuUpdateLoading, NULL, -1, -1, 0, 0, 0, 0},
    {MENU_DELETING, NULL, MenuDrawDeleting, MenuUpdateDeleting, NULL, -1, -1, 0, 0, 0, 0},
    {MENU_SAVE_CANCEL, NULL, MenuDrawSaveCancel, MenuUpdateSaveCancel, NULL, -1, -1, 0, 1, 1, 0},
    {MENU_LOAD_CANCEL, NULL, MenuDrawLoadCancel, MenuUpdateLoadCancel, NULL, -1, -1, 0, 1, 1, 0},
    {MENU_FORMAT_CANCEL, NULL, MenuDrawFormatCancel, MenuUpdateFormatCancel, NULL, -1, -1, 0, 1, 1, 0},
    {MENU_CARD_WARNING, MenuEnterCardWarning, MenuDrawCardWarning, MenuUpdateCardWarning, MenuExitCardWarning, -1, -1, 0, 1, 0, 0},
    {MENU_FILE_CORRUPT, NULL, MenuDrawFileCorrupt, MenuUpdateFileCorrupt, NULL, -1, -1, 0, 1, 1, 0},
    {MENU_HEADER_SAVE, MenuEnterHeaderSave, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
    {MENU_START_NEW_GAME, MenuEnterStartNewGame, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
    {MENU_DELETE_CONFIRM, NULL, MenuDrawDeleteConfirm, MenuUpdateDeleteConfirm, NULL, -1, -1, 0, 1, 1, 0},
    {1024, fun_00509470, fun_00509480, fun_00509490, fun_005094a0, -1, -1, 0, 0, 0, 0},
    {MENU_VIDEO, MenuEnterVideo, MenuDrawVideo, MenuUpdateVideo, MenuExitVideo, -1, -1, 0, 0, 0, 0},
    {MENU_EFFECTS, MenuEnterEffects, MenuDrawEffects, MenuUpdateEffects, MenuExitEffects, -1, -1, 0, 0, 0, 0},
    {MENU_VIDEO_CONFIRM, MenuEnterVideoConfirm, MenuDrawVideoConfirm, MenuUpdateVideoConfirm, MenuExitVideoConfirm, -1, -1, 0, 0, 0, 0},
    {MENU_HIRES_NOTICE, MenuEnterHiresNotice, MenuDrawHiresNotice, MenuUpdateHiresNotice, MenuExitHiresNotice, -1, -1, 0, 0, 0, 0},
    {MENU_QUIT, MenuEnterQuit, MenuDrawQuit, MenuUpdateQuit, MenuExitQuit, -1, -1, 0, 0, 0, 0},
    {1030, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
};
