#include "gamelib/util/V2SessionManager.h"
#include "gamelib/util/gamelib_util_types.h"
#include "nu2api/nucore/bgproc.h"
#include "nu2api/nucore/nustring.h"
#include <stdarg.h>
#include <stdio.h>

/* PC-only network session manager, never constructed outside this
 * object. */

V2SessionManager *V2SessionManager::mpSessionManager = 0;

void V2SessionManager::SetHostGameData(const i32 *data, i32 count) {
    i32 i;
    if (count >= 8) {
        count = 8;
    }
    for (i = 0; i < count; ++i) {
        player_slots[i] = data[i];
    }
}

void V2SessionManager::Log(const char *format, ...) {
    char buffer[128];
    va_list args;
    va_start(args, format);
    PcDebugPrintLock();
    _vsnprintf(buffer, 126, format, args);
    PcDebugPrintUnlock();
    NuStrCat(buffer, "\r\n");
    printf(buffer);
}

i32 V2SessionManager::VerifyStrings(u8 **source, u8 **destination, i32 count, u8 *replacement) {
    i32 result = 0;
    i32 i;
    for (i = 0; i < count; ++i) {
        result |= NuStringFilterBadWords(source[i], destination[i], replacement);
    }
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
    if (peer->next != NULL) {
        peer->next->prev = peer->prev;
    } else {
        tail = peer->prev;
    }
    if (peer->prev != NULL) {
        peer->prev->next = peer->next;
    } else {
        head = peer->next;
    }
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
    if (host_list.count != 0) {
        printf("mHostList\n");
    }
    if (peer_list.count != 0) {
        printf("mPeerList\n");
    }
    timer_6c = 0.0f;
    state = 0;
    timer_68 = 0.0f;
    error = 0;
    timer_74 = 0.0f;
    local_slot = -1;
    for (i = 0; i < 8; ++i) {
        player_slots[i] = -1;
    }
}

void V2SessionManager::Update() {
    V2Peer *peer;
    stats.totals.Clear();
    for (peer = peer_list.head; peer != NULL; peer = peer->next) {
        stats.totals += peer->stats.totals;
    }
    stats.Update();
}

void V2SessionManager::RemoveAllPeers(i32 reason) {
    V2Peer *peer;
    V2Peer *next;
    Log("V2SessionManager::RemoveAllPeers() called");
    for (peer = peer_list.head; peer != NULL; peer = next) {
        next = peer->next;
        RemovePeer(peer, reason);
    }
    if (local_peer != NULL) {
        RemovePeer(local_peer, reason);
    }
}

void V2SessionManager::RemovePeer(V2Peer *peer, i32 reason) {
    theTTNetwork.PeerLeft(*peer, reason);
    peer->Disconnect();
    if (peer != local_peer) {
        peer_list.Remove(peer);
        delete peer;
    }
}

extern "C" void V2SessionManagerNuError() {
}

V2SessionManager::V2SessionManager(const char *session_name) : stats("SessionAll") {
    host_lost = 0;
    slot_mode = 2;
    num_local_players = 0;
    mpSessionManager = this;
    NuStrCpy(name, session_name);
    joining = 0;
}
