#include "saga.h"
#include <string.h>

/* ThingManager's unit (retail 0x633500-0x633df0). */

STATIC_ASSERT(sizeof(BaseThing) == 0x10);
STATIC_ASSERT(offsetof(BaseThing, id) == 0x4);
STATIC_ASSERT(offsetof(BaseThing, flags) == 0x8);
STATIC_ASSERT(offsetof(BaseThing, profiling) == 0xc);

BaseThing::BaseThing() {
    id = 0;
    flags = 0;
    profiling = 0;
}

void BaseThing::ProcessEvenWhenPaused(ThingProcessData *) {
}

void BaseThing::Display(ThingRenderData *) {
}

BaseThing::~BaseThing() {
}

STATIC_ASSERT(sizeof(ThingManager) == 0x24);
STATIC_ASSERT(offsetof(ThingManager, things) == 0x4);
STATIC_ASSERT(offsetof(ThingManager, max_things) == 0x8);
STATIC_ASSERT(offsetof(ThingManager, count) == 0xc);
STATIC_ASSERT(offsetof(ThingManager, permanent_count) == 0x10);
STATIC_ASSERT(offsetof(ThingManager, pending) == 0x14);
STATIC_ASSERT(offsetof(ThingManager, timebar) == 0x18);
STATIC_ASSERT(offsetof(ThingManager, timing_thing) == 0x1c);
STATIC_ASSERT(offsetof(ThingManager, timing_state) == 0x20);
STATIC_ASSERT(sizeof(ThingProcessData) == 0x10);
STATIC_ASSERT(offsetof(ThingProcessData, paused) == 0x4);
STATIC_ASSERT(offsetof(ThingProcessData, pads) == 0x8);
STATIC_ASSERT(offsetof(ThingProcessData, pad_count) == 0xc);

/* Bit numbers of BaseThing::flags; a set bit opts the thing out of a pass. */
enum {
    THING_SKIP_REMOVE_DEPENDANCIES = 0,
    THING_SKIP_ENTER_LEVEL = 1,
    THING_SKIP_EXIT_LEVEL = 2,
    THING_SKIP_RESET = 3,
    THING_SKIP_PROCESS = 4,
    THING_SKIP_PROCESS_EVEN_WHEN_PAUSED = 5,
    THING_SKIP_PROCESS_ONLY_WHEN_PAUSED = 6,
    THING_SKIP_RENDER = 7,
    THING_SKIP_DISPLAY = 8,
    THING_SKIP_EFFECTS = 9
};
#define THING_TAKES_PASS(thing, bit) ((~((thing)->flags >> (bit)) & 1) != 0)

extern "C" {
    ThingManager *theThingManager = NULL;
    extern const char MenuPcString76b274[4];
    void __cdecl NuFntSet(i32 font);
    void __cdecl NuFntScale(i32 x, i32 y);
}

/* Editor "Timings" menu: per-pass time bars, globally or per thing. */
static eduimenu_s *edTimingMenu = NULL;
static u32 EdAttr[4] = {0x80000000, 0x80ff0000, 0x80808080, 0x80404040};

/* The thing array comes from the permanent buffer; without room the manager
 * never accepts a thing. */
ThingManager::ThingManager(i32 max_things) {
    MEMORYMANAGER *memory = &theMemoryManager;
    u32 bytes = max_things * sizeof(BaseThing *);
    void *array = NULL;
    if (bytes < *memory->end_anchor - *memory->start_anchor) {
        *memory->start_anchor = (*memory->start_anchor + 15) & ~15;
        array = (void *)*memory->start_anchor;
        *memory->start_anchor += bytes;
        memset(array, 0, bytes);
        memory->unknown_14 += bytes;
        memory->size -= bytes;
        memory->unknown_10 = *memory->start_anchor;
    }
    things = static_cast<BaseThing **>(array);
    this->max_things = max_things;
    timebar = NuTimeBarCreateSet(NULL);
    theThingManager = this;
}

/* Appends at count; a pending AddThingAfterThis reservation is folded in. */
void ThingManager::AddThing(BaseThing *thing) {
    if (thing != NULL && count < max_things) {
        things[count++] = thing;
    }
    count += pending;
    pending = 0;
}

/* Reserves the slot after the tail; the next AddThing folds it into count. */
BaseThing *ThingManager::AddThingAfterThis(BaseThing *thing) {
    if (thing != NULL) {
        pending++;
        i32 index = count + pending;
        if (index < max_things)
            things[index] = thing;
        return thing;
    }
    return NULL;
}

/* Sets (clear == 0) or clears the flag bits of the first thing with this id. */
void ThingManager::EnableActions(u32 id, u32 flags, i32 clear) {
    for (i32 i = 0; i < count; i++) {
        if (things[i] != NULL && things[i]->id == id) {
            if (clear != 0)
                things[i]->flags &= ~flags;
            else
                things[i]->flags |= flags;
            return;
        }
    }
}

void ThingManager::RemoveTemporaryThings() {
    for (i32 i = count - 1; i >= permanent_count; i--) {
        delete things[i];
        things[i] = NULL;
    }
    count = permanent_count;
}

i32 ThingManager::RemoveDependanciesThings(ThingRemoveData *data) {
    i32 removed = 1;
    for (i32 i = 0; i < count; i++) {
        if (things[i] != NULL && THING_TAKES_PASS(things[i], THING_SKIP_REMOVE_DEPENDANCIES))
            removed &= things[i]->RemoveDependancies(data);
    }
    return removed;
}

void ThingManager::ResetThings(ThingResetData *data) {
    for (i32 i = 0; i < count; i++) {
        if (things[i] != NULL && THING_TAKES_PASS(things[i], THING_SKIP_RESET)) {
            if (things[i]->profiling != 0)
                _NuTimeBarSlotBegin(timebar, 4, "Res");
            things[i]->Reset(data);
            if (things[i]->profiling != 0)
                _NuTimeBarSlotEnd(timebar, 4);
        }
    }
}

void ThingManager::EnterLevelThings(ThingLevelData *data) {
    for (i32 i = 0; i < count; i++) {
        if (things[i] != NULL && THING_TAKES_PASS(things[i], THING_SKIP_ENTER_LEVEL))
            things[i]->EnterLevel(data);
    }
}

void ThingManager::ExitLevelThings(ThingLevelData *data) {
    for (i32 i = 0; i < count; i++) {
        if (things[i] != NULL && THING_TAKES_PASS(things[i], THING_SKIP_EXIT_LEVEL))
            things[i]->ExitLevel(data);
    }
}

/* ProcessEvenWhenPaused always runs, then Process or ProcessOnlyWhenPaused. */
void ThingManager::ProcessThings(ThingProcessData *data) {
    i32 i;
    for (i = 0; i < count; i++) {
        if (things[i] != NULL && THING_TAKES_PASS(things[i], THING_SKIP_PROCESS_EVEN_WHEN_PAUSED)) {
            if (things[i]->profiling != 0)
                _NuTimeBarSlotBegin(timebar, 0, "PROC");
            things[i]->ProcessEvenWhenPaused(data);
            if (things[i]->profiling != 0)
                _NuTimeBarSlotEnd(timebar, 0);
        }
    }
    if (data->paused != 0) {
        for (i = 0; i < count; i++) {
            if (things[i] != NULL && THING_TAKES_PASS(things[i], THING_SKIP_PROCESS_ONLY_WHEN_PAUSED)) {
                if (things[i]->profiling != 0)
                    _NuTimeBarSlotBegin(timebar, 0, "PROC");
                things[i]->ProcessOnlyWhenPaused(data);
                if (things[i]->profiling != 0)
                    _NuTimeBarSlotEnd(timebar, 0);
            }
        }
    } else {
        for (i = 0; i < count; i++) {
            if (things[i] != NULL && THING_TAKES_PASS(things[i], THING_SKIP_PROCESS)) {
                if (things[i]->profiling != 0)
                    _NuTimeBarSlotBegin(timebar, 0, "PROC");
                things[i]->Process(data);
                if (things[i]->profiling != 0)
                    _NuTimeBarSlotEnd(timebar, 0);
            }
        }
    }
}

void ThingManager::RenderThings(ThingRenderData *data) {
    for (i32 i = 0; i < count; i++) {
        if (things[i] != NULL && THING_TAKES_PASS(things[i], THING_SKIP_RENDER)) {
            if (things[i]->profiling != 0)
                _NuTimeBarSlotBegin(timebar, 1, "Rnd");
            things[i]->Render(data);
            if (things[i]->profiling != 0)
                _NuTimeBarSlotEnd(timebar, 1);
        }
    }
}

void ThingManager::DisplayThings(ThingRenderData *data) {
    for (i32 i = 0; i < count; i++) {
        if (things[i] != NULL && THING_TAKES_PASS(things[i], THING_SKIP_DISPLAY)) {
            if (things[i]->profiling != 0)
                _NuTimeBarSlotBegin(timebar, 3, "Dis");
            things[i]->Display(data);
            if (things[i]->profiling != 0)
                _NuTimeBarSlotEnd(timebar, 3);
        }
    }
}

void ThingManager::EffectsThings(ThingRenderData *data) {
    for (i32 i = 0; i < count; i++) {
        if (things[i] != NULL && THING_TAKES_PASS(things[i], THING_SKIP_EFFECTS)) {
            if (things[i]->profiling != 0)
                _NuTimeBarSlotBegin(timebar, 5, "Fx");
            things[i]->Effects(data);
            if (things[i]->profiling != 0)
                _NuTimeBarSlotEnd(timebar, 5);
        }
    }
}

void ThingManager::cbEdTrackCancel(eduimenu_s *menu, eduimenu_s *) {
    eduiMenuDestroy(menu);
    edTimingMenu = NULL;
}

/* Items 0-3 pick Off / System / No Things / All Things; the toggles after
 * them flip one thing's time bar. */
void ThingManager::cbEdTimingSelect(eduimenu_s *menu, eduiitem_s *item, u32) {
    theThingManager->timing_state = item->data;
    theThingManager->timing_thing = item->data - 3;
    if ((u32)item->data < 4) {
        i32 enabled = item->data == 3;
        for (i32 i = 0; i < theThingManager->count; i++)
            theThingManager->things[i]->profiling = enabled;
        if ((u32)item->data < 2) {
            eduiMenuDestroy(menu);
            edTimingMenu = NULL;
        } else {
            i32 index = 0;
            for (eduiitem_s *entry = menu->first->next; entry != NULL; entry = entry->next) {
                if (index++ > 2)
                    entry->highlighted = enabled;
                else
                    entry->highlighted = 0;
            }
        }
        NuTimeBarEnable(item->data != 0);
    } else {
        theThingManager->things[item->data - 4]->profiling ^= 1;
    }
}

void ThingManager::edTimingInit() {
    theThingManager->timing_state = 0;
}

void ThingManager::edTimingEnter() {
    edTimingMenu = eduiMenuCreate(30, 30, 250, 320, NULL, (void *)cbEdTrackCancel, "Timings");
    if (edTimingMenu != NULL) {
        eduiMenuAddItem(edTimingMenu,
                        eduiItemSelCreate(0, EdAttr, 0, 1, (void *)cbEdTimingSelect, (char *)MenuPcString76b274));
        eduiMenuAddItem(edTimingMenu, eduiItemSelCreate(1, EdAttr, 0, 2, (void *)cbEdTimingSelect, "System"));
        eduiMenuAddItem(edTimingMenu, eduiItemSelCreate(2, EdAttr, 0, 3, (void *)cbEdTimingSelect, "No Things"));
        eduiMenuAddItem(edTimingMenu, eduiItemSelCreate(3, EdAttr, 0, 4, (void *)cbEdTimingSelect, "All Things"));
        for (i32 i = 0; i < theThingManager->count; i++) {
            BaseThing *thing = theThingManager->things[i];
            i32 selected = thing->profiling;
            eduiMenuAddItem(edTimingMenu, eduiItemToggleCreate(i + 4, EdAttr, selected, i + 5, cbEdTimingSelect,
                                                               (char *)thing->GetName()));
        }
    }
}

i32 ThingManager::edTimingProc(f32 delta_time, FRAME_PAD_PC *pad) {
    NuFntSet(0);
    NuFntScale(12, 12);
    eduiMenuProcess(edTimingMenu, delta_time, pad);
    return edTimingMenu == NULL ? 2 : 0;
}

void ThingManager::edTimingRender() {
    eduiMenuRender(edTimingMenu);
}
