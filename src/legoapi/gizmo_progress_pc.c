#include "saga.h"

typedef struct {
    u32 built[2], enabled[2], visible[2], completed[2];
    u8 object_count[64];
} BUILD_PROGRESS_PC;
typedef struct {
    u32 enabled[4], visible[4], reward[4], socket[4], released[4], pending[4], destroyed[4];
    u8 group_members[8][8];
} FORCE_PROGRESS_PC;
typedef struct {
    u32 triggered[16], enabled[16], visible[16], completed[16];
} BLOWUP_PROGRESS_PC;
typedef struct {
    u32 collected[16], enabled[16], visible[16], reserved[16];
} PICKUP_PROGRESS_PC;
typedef struct {
    u32 enabled[3], visible[3];
} BOMB_PROGRESS_PC;
typedef struct {
    u32 triggered, enabled, visible;
} PANEL_PROGRESS_PC;
typedef struct {
    u32 enabled, visible;
} GRAPPLE_PROGRESS_PC;
STATIC_ASSERT(sizeof(BUILD_PROGRESS_PC) == 0x60);
STATIC_ASSERT(offsetof(BUILD_PROGRESS_PC, object_count) == 0x20);
STATIC_ASSERT(sizeof(FORCE_PROGRESS_PC) == 0xb0);
STATIC_ASSERT(offsetof(FORCE_PROGRESS_PC, group_members) == 0x70);
STATIC_ASSERT(sizeof(BLOWUP_PROGRESS_PC) == 0x100);
STATIC_ASSERT(sizeof(PICKUP_PROGRESS_PC) == 0x100);
STATIC_ASSERT(sizeof(BOMB_PROGRESS_PC) == 0x18);
STATIC_ASSERT(sizeof(PANEL_PROGRESS_PC) == 0xc);
STATIC_ASSERT(sizeof(GRAPPLE_PROGRESS_PC) == 8);
typedef struct {
    u8 unused00[0x6d];
    u8 build_state, unused6e, built_object_count;
    u8 unused70[10];
    u16 flags;
} BUILD_PROGRESS_RECORD_PC;
typedef struct {
    BUILD_PROGRESS_RECORD_PC *records;
    u16 count;
} BUILD_PROGRESS_SYSTEM_PC;
typedef struct {
    u8 unused00[0xa0];
    u32 runtime_flags;
} FORCE_PROGRESS_RECORD_PC;
typedef struct {
    FORCE_PROGRESS_RECORD_PC *members[8];
    u8 unused20[6];
    u8 member_count, configured_count;
} FORCE_PROGRESS_GROUP_PC;
typedef struct {
    FORCE_PROGRESS_RECORD_PC *records;
    u8 unused04[10];
    u16 count;
    u8 unused10[8];
    FORCE_PROGRESS_GROUP_PC groups[8];
} FORCE_PROGRESS_SYSTEM_PC;
typedef struct {
    u8 unused00[0x2a];
    u8 flags, unused2b;
} BOMB_PROGRESS_RECORD_PC;
typedef struct {
    BOMB_PROGRESS_RECORD_PC *records;
    u16 count;
} BOMB_PROGRESS_SYSTEM_PC;
typedef struct {
    u8 unused00[0x62];
    u8 flags;
    u8 unused63[0x2d];
} PANEL_PROGRESS_RECORD_PC;
typedef struct {
    i32 count;
    PANEL_PROGRESS_RECORD_PC *records;
} PANEL_PROGRESS_SYSTEM_PC;
STATIC_ASSERT(sizeof(BUILD_PROGRESS_RECORD_PC) == 124);
STATIC_ASSERT(offsetof(BUILD_PROGRESS_RECORD_PC, build_state) == 0x6d);
STATIC_ASSERT(offsetof(BUILD_PROGRESS_RECORD_PC, built_object_count) == 0x6f);
STATIC_ASSERT(offsetof(BUILD_PROGRESS_RECORD_PC, flags) == 0x7a);
STATIC_ASSERT(offsetof(BUILD_PROGRESS_SYSTEM_PC, count) == 4);
STATIC_ASSERT(sizeof(FORCE_PROGRESS_RECORD_PC) == 164);
STATIC_ASSERT(offsetof(FORCE_PROGRESS_RECORD_PC, runtime_flags) == 0xa0);
STATIC_ASSERT(sizeof(FORCE_PROGRESS_GROUP_PC) == 40);
STATIC_ASSERT(offsetof(FORCE_PROGRESS_GROUP_PC, member_count) == 0x26);
STATIC_ASSERT(offsetof(FORCE_PROGRESS_SYSTEM_PC, count) == 0xe);
STATIC_ASSERT(offsetof(FORCE_PROGRESS_SYSTEM_PC, groups) == 0x18);
STATIC_ASSERT(sizeof(BOMB_PROGRESS_RECORD_PC) == 44);
STATIC_ASSERT(offsetof(BOMB_PROGRESS_RECORD_PC, flags) == 0x2a);
STATIC_ASSERT(offsetof(BOMB_PROGRESS_SYSTEM_PC, count) == 4);
STATIC_ASSERT(sizeof(PANEL_PROGRESS_RECORD_PC) == 144);
STATIC_ASSERT(offsetof(PANEL_PROGRESS_RECORD_PC, flags) == 0x62);
STATIC_ASSERT(sizeof(PANEL_PROGRESS_SYSTEM_PC) == 8);
STATIC_ASSERT(offsetof(PANEL_PROGRESS_SYSTEM_PC, records) == 4);
STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, status_flags) == 0x94);
STATIC_ASSERT(sizeof(GIZMOBLOWUP_PC) == 276);
STATIC_ASSERT(offsetof(GIZMOPICKUP_PC, flags) == 0x17);
STATIC_ASSERT(sizeof(GIZMOPICKUP_PC) == 44);
STATIC_ASSERT(offsetof(GIZMOPICKUPRUNTIMESYS_s, pickup_count) == 8);
STATIC_ASSERT(offsetof(GRAPPLE_DYNAMIC_PC, flags) == 0x1e);
STATIC_ASSERT(sizeof(GRAPPLE_DYNAMIC_PC) == 248);
STATIC_ASSERT(offsetof(WORLDINFO, giz_panel_sys) == 0x5070);
STATIC_ASSERT(offsetof(WORLDINFO, pickup_runtime_sys) == 0x50b8);
STATIC_ASSERT(offsetof(WORLDINFO, gizmo_blowups) == 0x50c8);
STATIC_ASSERT(offsetof(WORLDINFO, gizmo_blowup_count) == 0x50c0);
STATIC_ASSERT(offsetof(WORLDINFO, grapples) == 0x5080);
STATIC_ASSERT(offsetof(WORLDINFO, grapple_count) == 0x5084);
STATIC_ASSERT(offsetof(WORLDINFO, spinners) == 0x46e0);
STATIC_ASSERT(offsetof(WORLDINFO, current_level) == 0x128);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_spinners) == 0xf2);
STATIC_ASSERT(sizeof(GIZSPINNER_s) == 772);
STATIC_ASSERT(offsetof(GIZSPINNER_s, rotation) == 0x7c);
STATIC_ASSERT(offsetof(GIZSPINNER_s, target_rotation) == 0x7e);
STATIC_ASSERT(offsetof(GIZSPINNER_s, flags) == 0xac);
STATIC_ASSERT(offsetof(GIZSPINNER_s, anim_set) == 0x68);
STATIC_ASSERT(offsetof(GIZSPINNER_s, primary_anim_obj) == 0x6c);
STATIC_ASSERT(offsetof(GAMEANIMSET, state) == 0xc);

void __cdecl GizForces_ClearProgress(void *owner, void *progress_ptr) {
    FORCE_PROGRESS_PC *progress = (FORCE_PROGRESS_PC *)progress_ptr;
    if (progress == NULL)
        return;
    memset(progress->enabled, -1, sizeof(progress->enabled));
    memset(progress->visible, -1, sizeof(progress->visible));
    memset(progress->reward, 0, sizeof(progress->reward));
    memset(progress->socket, 0, sizeof(progress->socket));
    memset(progress->released, 0, sizeof(progress->released));
    memset(progress->pending, 0, sizeof(progress->pending));
    memset(progress->destroyed, 0, sizeof(progress->destroyed));
    memset(progress->group_members, -1, 64);
}

void __cdecl Blowup_ClearProgress(void *owner, void *progress_ptr) {
    BLOWUP_PROGRESS_PC *progress = (BLOWUP_PROGRESS_PC *)progress_ptr;
    if (progress == NULL)
        return;
    memset(progress->triggered, 0, 64);
    memset(progress->enabled, -1, 64);
    memset(progress->visible, -1, 64);
    memset(progress->completed, 0, 64);
}

void __cdecl GizmoPickups_ClearProgress(void *owner, void *progress_ptr) {
    PICKUP_PROGRESS_PC *progress = (PICKUP_PROGRESS_PC *)progress_ptr;
    if (progress == NULL)
        return;
    memset(progress->visible, -1, 64);
    memset(progress->enabled, -1, 64);
    memset(progress->collected, 0, 64);
    memset(progress->reserved, 0, 64);
}

void __cdecl GizBombGens_ClearProgress(void *owner, void *progress_ptr) {
    BOMB_PROGRESS_PC *progress = (BOMB_PROGRESS_PC *)progress_ptr;
    if (progress == NULL)
        return;
    memset(progress->enabled, -1, sizeof(progress->enabled));
    memset(progress->visible, -1, sizeof(progress->visible));
}

void __cdecl GizForces_StoreProgress(void *world_ptr, void *data, void *progress_ptr) {
    FORCE_PROGRESS_PC *progress = (FORCE_PROGRESS_PC *)progress_ptr;
    FORCE_PROGRESS_SYSTEM_PC *system = (FORCE_PROGRESS_SYSTEM_PC *)data;
    FORCE_PROGRESS_RECORD_PC *record;
    i32 index, group, slot, word;
    u32 bit;
    GizForces_ClearProgress(world_ptr, progress);
    if (progress == NULL)
        return;
    record = system->records;
    for (index = 0; index < system->count; ++index, ++record) {
        if (index >= 128)
            break;
        word = index / 32;
        bit = 1u << (index & 31);
        if (!(record->runtime_flags & 2))
            progress->visible[word] &= ~bit;
        if (!(record->runtime_flags & 1))
            progress->enabled[word] &= ~bit;
        if (record->runtime_flags & 0x200)
            progress->reward[word] |= bit;
        if (record->runtime_flags & 0x800)
            progress->socket[word] |= bit;
        if (record->runtime_flags & 0x4000)
            progress->released[word] |= bit;
        if (record->runtime_flags & 0x8000)
            progress->pending[word] |= bit;
        if (record->runtime_flags & 0x10000)
            progress->destroyed[word] |= bit;
    }
    for (group = 0; group < 8; ++group)
        for (slot = 0; slot < 8; ++slot) {
            if (slot < system->groups[group].member_count)
                progress->group_members[group][slot] = (u8)(system->groups[group].members[slot] - system->records);
            else
                progress->group_members[group][slot] = 0xff;
        }
}

void __cdecl Blowup_StoreProgress(void *world_ptr, void *data, void *progress_ptr) {
    BLOWUP_PROGRESS_PC *progress = (BLOWUP_PROGRESS_PC *)progress_ptr;
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    GIZMOBLOWUP_PC *record;
    i32 index;
    i32 word;
    u32 bit;
    if (progress == NULL)
        return;
    memset(progress->triggered, 0, 64);
    memset(progress->enabled, -1, 64);
    memset(progress->visible, -1, 64);
    memset(progress->completed, 0, 64);

    if (world == NULL || world->gizmo_blowups == NULL)
        return;
    record = world->gizmo_blowups;
    for (index = 0; index < world->gizmo_blowup_count; ++index, ++record) {
        if (index >= 512)
            break;
        word = index / 32;
        bit = 1u << (index & 31);
        if (record->status_flags & 1)
            progress->triggered[word] |= bit;
        if (!(record->status_flags & 0x4000))
            progress->visible[word] &= ~bit;
        if (!(record->status_flags & 0x800000))
            progress->enabled[word] &= ~bit;
        if (record->status_flags & 0x10000000)
            progress->completed[word] |= bit;
    }
}

void __cdecl GizmoPickups_StoreProgress(void *world_ptr, void *data, void *progress_ptr) {
    PICKUP_PROGRESS_PC *progress = (PICKUP_PROGRESS_PC *)progress_ptr;
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    GIZMOPICKUP_PC *record;
    i32 index;
    i32 word;
    u32 bit;
    if (progress == NULL)
        return;
    memset(progress->visible, -1, 64);
    memset(progress->enabled, -1, 64);
    memset(progress->collected, 0, 64);
    memset(progress->reserved, 0, 64);

    if (world == NULL || world->pickup_runtime_sys->pickups == NULL)
        return;
    record = world->pickup_runtime_sys->pickups;
    for (index = 0; index < world->pickup_runtime_sys->pickup_count; ++index, ++record) {
        if (index >= 512)
            break;
        word = index / 32;
        bit = 1u << (index & 31);
        if (!(record->flags & 4))
            progress->visible[word] &= ~bit;
        if (!(record->flags & 2))
            progress->enabled[word] &= ~bit;
        if (record->flags & 8)
            progress->collected[word] |= bit;
    }
}

void __cdecl GizBombGens_StoreProgress(void *world_ptr, void *data, void *progress_ptr) {
    BOMB_PROGRESS_PC *progress = (BOMB_PROGRESS_PC *)progress_ptr;
    BOMB_PROGRESS_SYSTEM_PC *system = (BOMB_PROGRESS_SYSTEM_PC *)data;
    BOMB_PROGRESS_RECORD_PC *record;
    i32 index;
    i32 word;
    u32 bit;
    if (progress == NULL)
        return;
    memset(progress->enabled, -1, sizeof(progress->enabled));
    memset(progress->visible, -1, sizeof(progress->visible));

    record = system->records;
    for (index = 0; index < system->count; ++index, ++record) {
        if (index >= 96)
            break;
        word = index / 32;
        bit = 1u << (index & 31);
        if (!(record->flags & 2))
            progress->visible[word] &= ~bit;
        if (!(record->flags & 1))
            progress->enabled[word] &= ~bit;
    }
}
