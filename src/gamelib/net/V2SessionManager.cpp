#include "saga.h"
#include <stdarg.h>
#include <stdio.h>

/* PC-only network session manager (0x60ffd0-0x6104b0). Not reached from the
 * Android source; retail never constructs it outside this object. */

STATIC_ASSERT(sizeof(NetStatCounters) == 0x10);
STATIC_ASSERT(sizeof(NetStats) == 0x220);
STATIC_ASSERT(offsetof(NetStats, totals) == 8);
STATIC_ASSERT(offsetof(NetStats, history) == 0x40);
STATIC_ASSERT(offsetof(V2Peer, next) == 4);
STATIC_ASSERT(offsetof(V2Peer, prev) == 8);
STATIC_ASSERT(offsetof(V2Peer, counters) == 0x18);
STATIC_ASSERT(offsetof(V2SessionManager, state) == 4);
STATIC_ASSERT(offsetof(V2SessionManager, name) == 0x14);
STATIC_ASSERT(offsetof(V2SessionManager, field_34) == 0x34);
STATIC_ASSERT(offsetof(V2SessionManager, max_players) == 0x38);
STATIC_ASSERT(offsetof(V2SessionManager, private_slots) == 0x3c);
STATIC_ASSERT(offsetof(V2SessionManager, slot_mode) == 0x40);
STATIC_ASSERT(offsetof(V2SessionManager, local_slot) == 0x44);
STATIC_ASSERT(offsetof(V2SessionManager, player_slots) == 0x48);
STATIC_ASSERT(offsetof(V2SessionManager, timer_68) == 0x68);
STATIC_ASSERT(offsetof(V2SessionManager, timer_74) == 0x74);
STATIC_ASSERT(offsetof(V2SessionManager, field_78) == 0x78);
STATIC_ASSERT(offsetof(V2SessionManager, field_7c) == 0x7c);
STATIC_ASSERT(offsetof(V2SessionManager, local_peer) == 0x84);
STATIC_ASSERT(offsetof(V2SessionManager, host_list) == 0x88);
STATIC_ASSERT(offsetof(V2SessionManager, peer_list) == 0x94);
STATIC_ASSERT(offsetof(V2SessionManager, stats) == 0xa0);

extern "C" {
    void __cdecl PcDebugPrintLock(void);
    void __cdecl PcDebugPrintUnlock(void);
}

V2SessionManager *V2SessionManager::sm_instance = 0;

void V2SessionManager::SetPlayerSlots(const i32 *slots, i32 count) {
    i32 i;
    if (count >= 8)
        count = 8;
    for (i = 0; i < count; ++i)
        player_slots[i] = slots[i];
}

static void V2SessionPrintf(const char *format, ...) {
    char buffer[128];
    va_list args;
    va_start(args, format);
    PcDebugPrintLock();
    _vsnprintf(buffer, 126, format, args);
    PcDebugPrintUnlock();
    NuStrCat(buffer, "\r\n");
    printf(buffer);
}

i32 V2SessionManager::FilterNames(u8 **source, u8 **destination, i32 count, u8 *replacement) {
    i32 result = 0;
    i32 i;
    for (i = 0; i < count; ++i)
        result |= NuStringFilterBadWords(source[i], destination[i], replacement);
    return result;
}

V2HostList::V2HostList() {
    head = NULL;
    tail = NULL;
    count = 0;
}

i32 V2HostList::GetCount() {
    return count;
}

V2PeerList::V2PeerList() {
    head = NULL;
    tail = NULL;
    count = 0;
}

void V2PeerList::Remove(V2Peer *peer) {
    if (peer->next != NULL)
        peer->next->prev = peer->prev;
    else
        tail = peer->prev;
    if (peer->prev != NULL)
        peer->prev->next = peer->next;
    else
        head = peer->next;
    peer->next = NULL;
    peer->prev = NULL;
    --count;
}

i32 V2PeerList::GetCount() {
    return count;
}

V2PendingList::V2PendingList() {
    head = NULL;
    tail = NULL;
    count = 0;
}

V2ClosingList::V2ClosingList() {
    head = NULL;
    tail = NULL;
    count = 0;
}

void V2SessionManager::Reset() {
    i32 i;
    if (host_list.count != 0)
        printf("mHostList\n");
    if (peer_list.count != 0)
        printf("mPeerList\n");
    timer_6c = 0.0f;
    state = 0;
    timer_68 = 0.0f;
    field_34 = 0;
    timer_74 = 0.0f;
    local_slot = -1;
    for (i = 0; i < 8; ++i)
        player_slots[i] = -1;
}

void V2SessionManager::UpdateStats() {
    V2Peer *peer;
    stats.totals.Clear();
    for (peer = peer_list.head; peer != NULL; peer = peer->next)
        stats.totals += peer->counters;
    stats.Update();
}

void V2SessionManager::RemoveAllPeers(i32 reason) {
    V2Peer *peer;
    V2Peer *next;
    V2SessionPrintf("V2SessionManager::RemoveAllPeers() called");
    for (peer = peer_list.head; peer != NULL; peer = next) {
        next = peer->next;
        RemovePeer(peer, reason);
    }
    if (local_peer != NULL)
        RemovePeer(local_peer, reason);
}

void V2SessionManager::RemovePeer(V2Peer *peer, i32 reason) {
    theTTNetwork.NotifyPeerRemoved(peer, reason);
    peer->Disconnect();
    if (peer != local_peer) {
        peer_list.Remove(peer);
        delete peer;
    }
}

V2SessionManager::V2SessionManager(const char *session_name) : stats("SessionAll") {
    field_7c = 0;
    slot_mode = 2;
    private_slots = 0;
    sm_instance = this;
    NuStrCpy(name, session_name);
    field_78 = 0;
}
