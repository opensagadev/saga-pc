#include "saga.h"

V2SessionManager *theNetSessionManager;

/* TTNetwork object (0x60dee0-0x60ffcb): network statistics and the TTNetwork
 * singleton. Only the statistics helpers are ported so far. */

STATIC_ASSERT(offsetof(NetStats, history_index) == 0x18);
STATIC_ASSERT(offsetof(NetStats, last_time) == 0x1c);
STATIC_ASSERT(offsetof(NetStats, peak) == 0x20);
STATIC_ASSERT(offsetof(NetStats, previous) == 0x30);
STATIC_ASSERT(offsetof(NetTransportEntry, client) == 0x8);
STATIC_ASSERT(offsetof(NetTransportEntry, channel) == 0xc);
STATIC_ASSERT(offsetof(NetTransportEntry, stats) == 0x30);
STATIC_ASSERT(sizeof(NetTransportEntry) == 0x250);
STATIC_ASSERT(offsetof(TTNetwork, clients) == 0x4);
STATIC_ASSERT(sizeof(NetTransporter) == 0x10);

NetStatCounters::NetStatCounters() {
    values[0] = 0;
    values[1] = 0;
    values[2] = 0;
    values[3] = 0;
}

NetSmallStats::NetSmallStats(const char *stats_name) : name(stats_name) {
}

void NetSmallStats::Update() {
}

NetStats::NetStats(const char *stats_name) : NetSmallStats(stats_name), history_index(0), last_time(0) {
}

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

NetTransportEntry::NetTransportEntry(NetTransportClient *entry_client, u8 entry_channel, const char *entry_name)
    : client(entry_client), channel(entry_channel), stats(name) {
    if (entry_name != NULL) {
        NuStrNCpy(name, entry_name, 0x20);
    } else {
        name[0] = 0;
    }
}

NetTransportEntry *NetTransportList::Find(const NetTransportEntry &key) {
    for (NetTransportEntry *entry = (NetTransportEntry *)head; entry != NULL;
         entry = (NetTransportEntry *)entry->next) {
        if (*entry == key) {
            return entry;
        }
    }
    return NULL;
}

void NetTransporter::AddClient(NetTransportClient *client, u8 channel, const char *name) {
    NetTransportEntry *entry = new NetTransportEntry(client, channel, name);
    if (clients.Find(*entry) != NULL) {
        delete entry;
        return;
    }
    clients.Insert(entry, NULL);
}

void NetTransporter::RemoveClient(NetTransportClient *client, u8 channel) {
    NetTransportEntry key(client, channel, NULL);
    NetTransportEntry *entry = clients.Find(key);
    if (entry != NULL) {
        clients.Remove(entry);
        delete entry;
    }
}

#define TT_FOR_EACH_CLIENT(entry)                                                                                      \
    for (NetTransportEntry *entry = (NetTransportEntry *)clients.head; entry != NULL;                                  \
         entry = (NetTransportEntry *)entry->next)

i32 TTNetwork::QueryAll08(i32 value) {
    i32 result = 1;
    TT_FOR_EACH_CLIENT(entry) {
        if (!entry->client->Query08(value)) {
            result = 0;
        }
    }
    return result;
}

void TTNetwork::NotifyAll0c(i32 value) {
    TT_FOR_EACH_CLIENT(entry) {
        entry->client->Notify0c(value);
    }
}

void TTNetwork::NotifyPeerRemoved(V2Peer *peer, i32 reason) {
    TT_FOR_EACH_CLIENT(entry) {
        entry->client->OnPeerRemoved(peer, reason);
    }
}

void TTNetwork::NotifyAll14(i32 value) {
    TT_FOR_EACH_CLIENT(entry) {
        entry->client->Notify14(value);
    }
}

void TTNetwork::NotifyAll18(i32 value) {
    TT_FOR_EACH_CLIENT(entry) {
        entry->client->Notify18(value);
    }
}

void TTNetwork::NotifyAll1c(i32 value) {
    TT_FOR_EACH_CLIENT(entry) {
        entry->client->Notify1c(value);
    }
}

void TTNetwork::NotifyAll20(i32 value, i32 extra) {
    TT_FOR_EACH_CLIENT(entry) {
        entry->client->Notify20(value, extra);
    }
}

i32 TTNetwork::QueryAll24(i32 value, i32 extra) {
    i32 result = 1;
    TT_FOR_EACH_CLIENT(entry) {
        if (!entry->client->Query24(value, extra)) {
            result = 0;
        }
    }
    return result;
}

void TTNetwork::NotifyAll28(i32 value, i32 extra) {
    TT_FOR_EACH_CLIENT(entry) {
        entry->client->Notify28(value, extra);
    }
}

void TTNetwork::UpdateClientStats() {
    TT_FOR_EACH_CLIENT(entry) {
        entry->stats.Update();
    }
}

void TTNetwork::DeliverMessage(NETMESSAGE_PC &message, u8 channel, i32 arg) {
    TT_FOR_EACH_CLIENT(entry) {
        if (entry->channel == channel) {
            entry->stats.totals.values[1] += message.GetSize();
            entry->stats.totals.values[3] += 1;
            entry->client->OnMessage(message, channel, arg);
        }
    }
}

void TTNetwork::RecordSent(NETMESSAGE_PC message, u8 channel) {
    TT_FOR_EACH_CLIENT(entry) {
        if (entry->channel == channel) {
            entry->stats.totals.values[0] += message.GetSize();
            entry->stats.totals.values[2] += 1;
            break;
        }
    }
    message.Release();
}

void TTNetwork::RecordReceived(NETMESSAGE_PC message, u8 channel) {
    TT_FOR_EACH_CLIENT(entry) {
        if (entry->channel == channel) {
            entry->stats.totals.values[1] += message.GetSize();
            entry->stats.totals.values[3] += 1;
            break;
        }
    }
    message.Release();
}

void NETMESSAGE_PC::MakeUnique() {
    MessageData *entry;
    for (entry = sm_poolMessageData; entry != sm_poolMessageData + 512; ++entry) {
        if (entry->references <= 0) {
            entry->references = 1;
            goto found;
        }
    }
    entry = NULL;
found:
    if (data != NULL && entry != NULL) {
        PCByteCopy_candidate(entry->bytes + read_offset, data->bytes + read_offset, write_offset - read_offset);
    } else {
        NetMessagePoolError();
    }
    Release();
    data = entry;
}

void TTNetwork::Send(NETMESSAGE_PC message, u8 channel, V2Peer *peer) {
    if (message.GetSize() > 0) {
        PCByteCopy_candidate(message.data->bytes + --message.read_offset, &channel, 1);
        RecordSent(message, channel);
        peer->Send(message);
    }
    message.Release();
}

void TTNetwork::SendEx(NETMESSAGE_PC message, u8 channel, V2Peer *peer, i32 arg0, i32 arg1) {
    if (message.GetSize() > 0) {
        PCByteCopy_candidate(message.data->bytes + --message.read_offset, &channel, 1);
        RecordSent(message, channel);
        peer->SendEx(message, arg0, arg1);
    }
    message.Release();
}

void TTNetwork::Broadcast(NETMESSAGE_PC message, u8 channel) {
    if (message.GetSize() > 0) {
        PCByteCopy_candidate(message.data->bytes + --message.read_offset, &channel, 1);
        RecordSent(message, channel);
        for (V2Peer *peer = V2SessionManager::sm_instance->peer_list.head; peer != NULL; peer = peer->next) {
            message.MakeUnique();
            peer->Send(message);
        }
    }
    message.Release();
}

void TTNetwork::BroadcastEx(NETMESSAGE_PC message, u8 channel) {
    if (message.GetSize() > 0) {
        PCByteCopy_candidate(message.data->bytes + --message.read_offset, &channel, 1);
        RecordSent(message, channel);
        for (V2Peer *peer = V2SessionManager::sm_instance->peer_list.head; peer != NULL; peer = peer->next) {
            message.MakeUnique();
            peer->SendEx(message, 0, 0);
        }
    }
    message.Release();
}

void NetStatCounters::Clear() {
    values[0] = 0;
    values[1] = 0;
    values[2] = 0;
    values[3] = 0;
}

NetStatCounters &NetStatCounters::operator+=(const NetStatCounters &other) {
    values[0] += other.values[0];
    values[1] += other.values[1];
    values[2] += other.values[2];
    values[3] += other.values[3];
    return *this;
}

NetStatCounters &NetStatCounters::operator-=(const NetStatCounters &other) {
    values[0] -= other.values[0];
    values[1] -= other.values[1];
    values[2] -= other.values[2];
    values[3] -= other.values[3];
    return *this;
}

NetStatCounters &NetStatCounters::KeepMax(const NetStatCounters &other) {
    values[0] = values[0] > other.values[0] ? values[0] : other.values[0];
    values[1] = values[1] > other.values[1] ? values[1] : other.values[1];
    values[2] = values[2] > other.values[2] ? values[2] : other.values[2];
    values[3] = values[3] > other.values[3] ? values[3] : other.values[3];
    return *this;
}

void NetStats::Update() {
    i32 index;
    if (NetTimeGetMs() - last_time > 1000) {
        index = history_index + 1;
        if (index >= 30)
            index = 0;
        history[index] = totals;
        history[index] -= previous;
        peak.KeepMax(history[index]);
        previous = totals;
        history_index = index;
        last_time = NetTimeGetMs();
    }
}
