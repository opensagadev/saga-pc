#include "MechInputTouch/MechInputTouch_types.h"
#include "editor/edlevelall.h"
#include "gameapi/edtools/edui.h"
#include "gamelib/net/NetMessagePc.h"
#include "gamelib/util/Refpack.h"
#include "gamelib/util/Utilities.h"
#include "gamelib/util/gamelib_util_types.h"
#include "globals.h"
#include "legoapi/items/collect/bolts.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nucore/NuNetEmu.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nufile/nufile.h"

NetSessionManager *theNetSessionManager;

/* TTNetwork object: network statistics and the TTNetwork singleton. */

void *NetTransportEntry::operator new(u32 size) {
    return theMemoryManager.AllocPool(size, 1);
}

void NetTransportEntry::operator delete(void *memory) {
    theMemoryManager.FreePool(memory, sizeof(NetTransportEntry));
}

NetTransportEntry &NetTransportEntry::operator=(const NetTransportEntry &other) {
    client = other.client;
    channel = other.channel;
    return *this;
}

i32 NetTransportEntry::operator==(const NetTransportEntry &other) {
    if (client == other.client && channel == other.channel) {
        return 1;
    }
    return 0;
}

NetListLink::NetListLink() {
    if (this != NULL) {
        next = NULL;
        prev = NULL;
    }
}

NetListLink *NetTransportList::GetHead() {
    return head;
}

NetListLink *NetListLink::GetNext() {
    return next;
}

void NetTransportList::Insert(NetTransportEntry *entry, NetTransportEntry *after) {
    if (after != NULL) {
        entry->prev = after;
        entry->next = after->next;
        if (after->next != NULL) {
            after->next->prev = entry;
        } else {
            tail = entry;
        }
        after->next = entry;
        count++;
    } else {
        entry->next = NULL;
        entry->prev = tail;
        if (tail != NULL) {
            tail->next = entry;
        }
        tail = entry;
        if (head == NULL) {
            head = entry;
        }
        count++;
    }
}

void NetTransportList::Remove(NetTransportEntry *entry) {
    if (entry->next != NULL) {
        entry->next->prev = entry->prev;
    } else {
        tail = entry->prev;
    }
    if (entry->prev != NULL) {
        entry->prev->next = entry->next;
    } else {
        head = entry->next;
    }
    entry->next = NULL;
    entry->prev = NULL;
    count--;
}

NetTransportEntry::NetTransportEntry(NetListenerInterface *entry_client, u8 entry_channel, const char *entry_name)
    : client(entry_client), channel(entry_channel), stats(name) {
    if (entry_name != NULL) {
        NuStrNCpy(name, entry_name, 0x20);
    } else {
        name[0] = 0;
    }
}

NetTransportEntry *NetTransportList::Find(const NetTransportEntry &key) {
    for (NetTransportEntry *entry = (NetTransportEntry *)head; entry != NULL; entry = (NetTransportEntry *)entry->next) {
        if (*entry == key) {
            return entry;
        }
    }
    return NULL;
}

void NetTransporter::AddListener(NetListenerInterface *client, u8 channel, const char *name) {
    NetTransportEntry *entry = new NetTransportEntry(client, channel, name);
    if (clients.Find(*entry) != NULL) {
        delete entry;
        return;
    }
    clients.Insert(entry, NULL);
}

void NetTransporter::RemoveListener(NetListenerInterface *client, u8 channel) {
    NetTransportEntry key(client, channel, NULL);
    NetTransportEntry *entry = clients.Find(key);
    if (entry != NULL) {
        clients.Remove(entry);
        delete entry;
    }
}

i32 TTNetwork::PeerRequest(const V2Peer &peer) {
    i32 result = 1;
    for (NetTransportEntry *entry = (NetTransportEntry *)clients.head; entry != NULL; entry = (NetTransportEntry *)entry->next) {
        if (!entry->client->PeerRequest(peer)) {
            result = 0;
        }
    }
    return result;
}

void TTNetwork::PeerJoined(const V2Peer &peer) {
    for (NetTransportEntry *entry = (NetTransportEntry *)clients.head; entry != NULL; entry = (NetTransportEntry *)entry->next) {
        entry->client->PeerJoined(peer);
    }
}

void TTNetwork::PeerLeft(const V2Peer &peer, i32 reason) {
    for (NetTransportEntry *entry = (NetTransportEntry *)clients.head; entry != NULL; entry = (NetTransportEntry *)entry->next) {
        entry->client->PeerLeft(peer, reason);
    }
}

void TTNetwork::PeerDead(const V2Peer &peer) {
    for (NetTransportEntry *entry = (NetTransportEntry *)clients.head; entry != NULL; entry = (NetTransportEntry *)entry->next) {
        entry->client->PeerDead(peer);
    }
}

void TTNetwork::FtpUpload(FtpFile *file) {
    for (NetTransportEntry *entry = (NetTransportEntry *)clients.head; entry != NULL; entry = (NetTransportEntry *)entry->next) {
        entry->client->FtpUpload(file);
    }
}

void TTNetwork::FtpDownload(FtpFile *file) {
    for (NetTransportEntry *entry = (NetTransportEntry *)clients.head; entry != NULL; entry = (NetTransportEntry *)entry->next) {
        entry->client->FtpDownload(file);
    }
}

void TTNetwork::FtpComplete(FtpFile *file, i32 result) {
    for (NetTransportEntry *entry = (NetTransportEntry *)clients.head; entry != NULL; entry = (NetTransportEntry *)entry->next) {
        entry->client->FtpComplete(file, result);
    }
}

i32 TTNetwork::NosAcquire(NetworkObject *object, const V2Peer &peer) {
    i32 result = 1;
    for (NetTransportEntry *entry = (NetTransportEntry *)clients.head; entry != NULL; entry = (NetTransportEntry *)entry->next) {
        if (!entry->client->NosAcquire(object, peer)) {
            result = 0;
        }
    }
    return result;
}

void TTNetwork::NosAdopted(NetworkObject *object, const V2Peer &peer) {
    for (NetTransportEntry *entry = (NetTransportEntry *)clients.head; entry != NULL; entry = (NetTransportEntry *)entry->next) {
        entry->client->NosAdopted(object, peer);
    }
}

// noinline: the original calls UpdateClientStats out of line from TTNetwork::Update; VC8 would inline it there.
__declspec(noinline) void TTNetwork::UpdateClientStats() {
    for (NetTransportEntry *entry = (NetTransportEntry *)clients.head; entry != NULL; entry = (NetTransportEntry *)entry->next) {
        entry->stats.Update();
    }
}

void TTNetwork::DeliverMessage(NETMESSAGE &message, u8 channel, const V2Peer &peer) {
    for (NetTransportEntry *entry = (NetTransportEntry *)clients.head; entry != NULL; entry = (NetTransportEntry *)entry->next) {
        if (entry->channel == channel) {
            entry->stats.totals.values[1] += message.GetSize();
            entry->stats.totals.values[3] += 1;
            entry->client->Receive(message, channel, peer);
        }
    }
}

void TTNetwork::RecordSent(NETMESSAGE message, u8 channel) {
    for (NetTransportEntry *entry = (NetTransportEntry *)clients.head; entry != NULL; entry = (NetTransportEntry *)entry->next) {
        if (entry->channel == channel) {
            entry->stats.totals.values[0] += message.GetSize();
            entry->stats.totals.values[2] += 1;
            break;
        }
    }
    message.Release();
}

void TTNetwork::RecordReceived(NETMESSAGE message, u8 channel) {
    for (NetTransportEntry *entry = (NetTransportEntry *)clients.head; entry != NULL; entry = (NetTransportEntry *)entry->next) {
        if (entry->channel == channel) {
            entry->stats.totals.values[1] += message.GetSize();
            entry->stats.totals.values[3] += 1;
            break;
        }
    }
    message.Release();
}

extern "C" void fun_0060e650() {
}

extern "C" void fun_0060e680() {
}

NetLogFile::NetLogFile() {
    state = 0;
    name[0] = 0;
}

i32 NetLogFile::Flush() {
    if (state != 2) {
        return (i32)0xa1000003;
    }
    if (count > 0) {
        NUFILE file = NuFileOpen(name, mode);
        if (file == 0) {
            return (i32)0xa1000001;
        }
        NuFileSeek(file, 0, 2);
        NuFileWrite(file, buffer, count);
        NuFileClose(file);
        count = 0;
        mode = 2;
    }
    return 0;
}

void NETMESSAGE::fun_0060ea60(const u8 &value) {
    if (data != NULL) {
        read_offset--;
        PCByteCopy(data->bytes + read_offset, &value, 1);
    }
}

NetLogFile::~NetLogFile() {
    Flush();
}

char const *TTNetwork::GetName() {
    return "Network";
}

void NETMESSAGE::MakeUnique() {
    MessageData *entry;
    for (MessageData *candidate = sm_poolMessageData.items; candidate != sm_poolMessageData.items + 512; ++candidate) {
        if (candidate->references <= 0) {
            candidate->references = 1;
            entry = candidate;
            goto found;
        }
    }
    entry = NULL;
found:
    if (data != NULL && entry != NULL) {
        PCByteCopy(entry->bytes + read_offset, data->bytes + read_offset, write_offset - read_offset);
    } else {
        NetMessagePoolError();
    }
    Release();
    data = entry;
}

void TTNetwork::Send(NETMESSAGE message, u8 channel, V2Peer *peer) {
    if (message.GetSize() > 0) {
        PCByteCopy(message.data->bytes + --message.read_offset, &channel, 1);
        RecordSent(message, channel);
        peer->Send(message);
    }
    message.Release();
}

void TTNetwork::SendEx(NETMESSAGE message, u8 channel, V2Peer *peer, i32 arg0, i32 arg1) {
    if (message.GetSize() > 0) {
        PCByteCopy(message.data->bytes + --message.read_offset, &channel, 1);
        RecordSent(message, channel);
        peer->SendEx(message, arg0, arg1);
    }
    message.Release();
}

void TTNetwork::Broadcast(NETMESSAGE message, u8 channel) {
    if (message.GetSize() > 0) {
        PCByteCopy(message.data->bytes + --message.read_offset, &channel, 1);
        RecordSent(message, channel);
        for (V2Peer *peer = V2SessionManager::mpSessionManager->peer_list.head; peer != NULL; peer = peer->next) {
            message.MakeUnique();
            peer->Send(message);
        }
    }
    message.Release();
}

void TTNetwork::BroadcastEx(NETMESSAGE message, u8 channel) {
    if (message.GetSize() > 0) {
        PCByteCopy(message.data->bytes + --message.read_offset, &channel, 1);
        RecordSent(message, channel);
        for (V2Peer *peer = V2SessionManager::mpSessionManager->peer_list.head; peer != NULL; peer = peer->next) {
            message.MakeUnique();
            peer->SendEx(message, 0, 0);
        }
    }
    message.Release();
}

NetAddress::NetAddress() {
    address = 0;
}

void NetFtpManager::fun_0060e6f0(i32 value) {
    unknown_1604 = value;
}

NetStats *NetworkObjectManager::fun_0060e740(i32 class_id) {
    return class_stats[class_id];
}

void NuNetEmu::fun_0060e750(u8 new_enabled) {
    enabled = new_enabled;
}

NetStats *NuNetEmu::fun_0060e760() {
    return &raw_stats;
}

NuNetEmu::PackStats *NuNetEmu::fun_0060e770() {
    return &packet_stats;
}

void TTNetwork::Resume() {
}

i32 TTNetwork::Suspend() {
    return 1;
}

NetAddress *TTNetwork::GetMyAddress() {
    return &my_address;
}

NetAddress *TTNetwork::GetMyHostAddress() {
    if (has_my_host_address) {
        return &my_host_address;
    }
    return NULL;
}

void TTNetwork::SetMyHostAddress(NetAddress *address) {
    my_host_address = *address;
    has_my_host_address = 1;
}

void TTNetwork::ClearMyHostAddress() {
    has_my_host_address = 0;
}

void TTNetwork::Start() {
    if (state != 0) {
        return;
    }
    unknown_2148 = -1;
    ftp.unknown_1604 = -1;
    nos.unknown_08 = -1;
    theNetSessionManager = (NetSessionManager *)V2SessionManager::mpSessionManager;
    session_manager = theNetSessionManager;
    frame_count = 0;
    unknown_2138 = 0;
    ftp.Init();
    nos.Init();
    session_manager->Init(NULL);
    theNetSessionManager->GetLocalAddress(&my_address);
    state = 1;
    unknown_2144 = 0;
    theNuNetEmu.enabled = 1;
    theNuNetEmu.SetConditions(NuNetEmu::CONDITIONS_NORMAL);
    refpack_init();
}

void TTNetwork::Update() {
    if (state != 0) {
        frame_count++;
        UtilFrameStart();
        session_manager->Update();
        ftp.Update();
        nos.Update();
        theNuNetEmu.Update();
        UpdateClientStats();
    }
}

void TTNetwork::ProcessEvenWhenPaused(ThingProcessData *data) {
    if (state != 0) {
        Update();
        if (display_page != 0) {
            if (data->pads[0] != NULL && (reinterpret_cast<FRAME_PAD *>(data->pads[0])->digital_buttons_pressed & PAD_BUTTON_L1)) {
                display_page++;
            }
            if (data->pads[1] != NULL && (reinterpret_cast<FRAME_PAD *>(data->pads[1])->digital_buttons_pressed & PAD_BUTTON_L1)) {
                display_page++;
            }
            if (data->pads[0] != NULL && (reinterpret_cast<FRAME_PAD *>(data->pads[0])->digital_buttons_pressed & PAD_BUTTON_R1)) {
                display_page--;
            }
            if (data->pads[1] != NULL && (reinterpret_cast<FRAME_PAD *>(data->pads[1])->digital_buttons_pressed & PAD_BUTTON_R1)) {
                display_page--;
            }
            if (display_page < 1) {
                display_page = 1;
            }
        }
    }
}

void TTNetwork::Shutdown() {
    if (state != 0) {
        nos.Term();
        ftp.Term();
        session_manager->Close();
        state = 0;
    }
}

void TTNetwork::Display(ThingRenderData *data) {
    if (display_page == 0) {
        return;
    }
    NetSmallStats *stats = NULL;
    if (display_page == 1) {
        stats = &theNuNetEmu.packet_stats;
    }
    if (display_page == 2) {
        stats = &theNuNetEmu.raw_stats;
    }
    i32 page = 4;
    if (display_page == 3) {
        stats = &session_manager->stats;
    }
    if (stats == NULL) {
        for (V2Peer *peer = session_manager->peer_list.head; peer != NULL; peer = peer->next) {
            if (display_page == page++) {
                stats = &peer->stats;
                break;
            }
        }
    }
    if (stats == NULL) {
        for (i32 i = 0; i < theRegistry.class_count; i++) {
            EdClass *cls = theRegistry.GetClass(i);
            if (cls->object_interface != NULL && display_page == page++) {
                stats = nos.class_stats[i];
                break;
            }
        }
    }
    if (stats == NULL) {
        for (NetTransportEntry *entry = (NetTransportEntry *)clients.head; entry != NULL; entry = (NetTransportEntry *)entry->next) {
            if (display_page == page++) {
                stats = &entry->stats;
                break;
            }
        }
    }
    if (stats != NULL) {
        stats->Draw(display_x, display_y, display_width, display_height, NetSmallStats::INFO_BYTES);
    } else {
        display_page = 1;
    }
}

TTNetwork::TTNetwork() : state(0) {
    display_x = 0.3f;
    display_y = -0.3f;
    has_my_host_address = 0;
    unknown_213c = 0;
    display_page = 0;
    display_width = 0.5f;
    display_height = 0.3f;
}

TTNetwork::~TTNetwork() {
}

TTNetwork theTTNetwork;
TPool<NETMESSAGE::MessageData, 512> NETMESSAGE::sm_poolMessageData;

/* Drops every outstanding reference of the message pool. */
template <> void TPool<NETMESSAGE::MessageData, 512>::fun_0060f370() {
    for (NETMESSAGE::MessageData *entry = items; entry != items + 512; ++entry) {
        if (entry->references > 0) {
            entry->references = 0;
        }
    }
}
