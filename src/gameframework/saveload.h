#ifndef SAGA_GAMEFRAMEWORK_SAVELOAD_H
#define SAGA_GAMEFRAMEWORK_SAVELOAD_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "legoapi/menus/screens/gamemenuall.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"

#ifdef __cplusplus
extern "C" {
#endif
    extern i32 memcard_autosavestarted;
    extern f32 memcard_autosavepredelay;
    extern f32 memcard_autosavepostdelay;
    extern void (*memcard_drawasiconfn)(void);
    extern i32 saveload_autosave;
    void saveloadInit(VARIPTR *buffer, VARIPTR end, i32 size, const char *product, const char *icon, const char *title,
                      i32 flags);
    extern i32 saveload_status;

    void PCOptionsBuildFolder(char *);
    extern i32 SAVESLOTS;
    extern i32 saveload_cardtype;
    extern i32 saveload_cardformatted;
    extern i32 memcard_autosaveneeded;
    extern i32 memcard_autosaveinprogress;
    extern void *memcard_savedata;
    extern void *memcard_savedatabuffer;
    extern i32 memcard_savedatasize;
    extern i32 memcard_autosave;
    extern i32 saveload_error;
    extern i32 saveload_cardchanged;
    extern i32 saveload_savepresent;
    extern i32 saveload_slotused[];
    void SaveSystemInitialise(i32, SAVE_HASH_FN_PC, void *, i32, i32, SAVE_VOID_FN_PC);
    i32 ChecksumSaveData(void *, i32);
    extern i32 saveload_freespace;
    extern i32 SAVESIZE_ADDITIONAL;
    extern i32 memcard_autosaveenabled;
    extern i32 saveload_filecorrupt;
    extern i32 saveload_slotused[6];
    extern u8 code_test[16];
    extern char id_test[17];
    i32 saveloadCheckCardFreeSpace(i32 slot);
    i32 saveloadCheckCardType(void);
    i32 saveloadCheckSlotsUsed(void);
    i32 saveloadDeleteSlot(i32 slot);
    i32 saveloadSaveSlot(i32 slot, void *buffer, i32 size);
    i32 saveloadLoadSlot(i32 slot, void *buffer, i32 size);
    void saveloadASLoad(i32 slot, void *buffer, i32 size);
    void saveloadASCallEachFrame(void);
#ifdef __cplusplus
}
#endif

#endif
