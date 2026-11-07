#include "saga.h"
#include <new>

/* Network object system (retail [0x6118f0,0x617180)): replicates editor
 * objects between peers on listener channel 3. Names follow the Android
 * NetworkObjectManager. */

STATIC_ASSERT(sizeof(NetworkObject) == 0x18);
STATIC_ASSERT(offsetof(NetworkObject, owner) == 0x8);
STATIC_ASSERT(offsetof(NetworkObject, replicator_data) == 0x14);
STATIC_ASSERT(sizeof(NetReplicator) == 0x18);
STATIC_ASSERT(offsetof(NetReplicator, id) == 0xc);
STATIC_ASSERT(offsetof(NetReplicator, message_size) == 0x16);
STATIC_ASSERT(sizeof(NetworkObjectManager::NetPeerPush) == 0x14);
STATIC_ASSERT(offsetof(NetworkObjectManager, active) == 0xc);
STATIC_ASSERT(offsetof(NetworkObjectManager, guid_peers) == 0x20);
STATIC_ASSERT(offsetof(NetworkObjectManager, objects) == 0x30);
STATIC_ASSERT(offsetof(NetworkObjectManager, local_object_count) == 0xc030);
STATIC_ASSERT(offsetof(NetworkObjectManager, pending_objects) == 0xd034);
STATIC_ASSERT(offsetof(NetworkObjectManager, replicators) == 0xd1b4);
STATIC_ASSERT(offsetof(NetworkObjectManager, replicator_data_sizes) == 0xd4b4);
STATIC_ASSERT(offsetof(NetworkObjectManager, filters) == 0xd5b4);
STATIC_ASSERT(offsetof(NetworkObjectManager, registered_calls) == 0xd6b4);
STATIC_ASSERT(offsetof(NetworkObjectManager, registered_call_count) == 0xd834);
STATIC_ASSERT(offsetof(NetworkObjectManager, peer_push) == 0xd838);
STATIC_ASSERT(offsetof(NetworkObjectManager, default_push) == 0xd8d8);
STATIC_ASSERT(offsetof(NetworkObjectManager, class_stats) == 0xd8ec);
STATIC_ASSERT(offsetof(NetworkObjectManager, allow_duplicates) == 0xd96c);
STATIC_ASSERT(sizeof(NetworkObjectManager) == 0xd970);
STATIC_ASSERT(offsetof(TTNetwork, nos) == 0x3780);

void NetTrace(...);

NetworkObjectManager *theNos = NULL;

void NetworkObject::Initialise(i32 guid, void *new_object, EdClass *new_class, const V2Peer &new_owner, i32 new_flags) {
    id = guid;
    object = new_object;
    object_class = new_class;
    owner = &new_owner;
    flags |= new_flags;
    i32 size = theTTNetwork.nos.replicator_data_sizes[theRegistry.GetClassId(new_class)];
    if (size > 0)
        replicator_data = theMemoryManager.AllocPool(size, 1);
}

void NetworkObject::Destroy() {
    i32 size = theTTNetwork.nos.replicator_data_sizes[theRegistry.GetClassId(object_class)];
    if (size > 0) {
        theMemoryManager.FreePool(replicator_data, size);
        replicator_data = NULL;
    }
    id = 0;
    object = NULL;
    object_class = NULL;
    owner = NULL;
    flags = 0;
}

void NetworkObjectManager::Reset() {
}

void NetworkObjectManager::BindFilter(NOSFilter *filter, const EdClass *object_class) {
    filters[theRegistry.GetClassId((EdClass *)object_class)] = filter;
}

i32 NetworkObjectManager::RegisterRemoteCall(void (*callback)(NETMESSAGE_PC &), i32 flags) {
    i32 call_id = 0;
    if (registered_call_count < 32) {
        registered_calls[registered_call_count].type = 0;
        registered_calls[registered_call_count].flags = flags;
        registered_calls[registered_call_count].callback = (void *)callback;
        call_id = ++registered_call_count;
    }
    return call_id;
}

i32 NetworkObjectManager::RegisterObjectCall(void (*callback)(void *, NETMESSAGE_PC &), i32 flags) {
    i32 call_id = 0;
    if (registered_call_count < 32) {
        registered_calls[registered_call_count].type = 1;
        registered_calls[registered_call_count].flags = flags;
        registered_calls[registered_call_count].callback = (void *)callback;
        call_id = ++registered_call_count;
    }
    return call_id;
}

i32 NetworkObjectManager::IsPeerStarted(const V2Peer &peer) const {
    for (i32 i = 0; i < 8; i++) {
        if (peer_push[i].peer == &peer)
            return peer_push[i].stage > 0;
    }
    return 0;
}

i32 NetworkObjectManager::IsPeerReady(const V2Peer &peer) const {
    for (i32 i = 0; i < 8; i++) {
        if (peer_push[i].peer == &peer)
            return peer_push[i].stage == 3;
    }
    return 0;
}

i32 NetworkObjectManager::GetNextGuid() {
    i32 group = guid_group;
    if (group >= 0) {
        i32 attempts = 0;
        if (next_guid < 0) {
            next_guid = group << 10;
            if (next_guid == 0)
                next_guid = 1;
            next_guid--;
        }
        i32 group_start = group << 10;
        i32 group_end = group_start + 0x400;
        do {
            next_guid++;
            attempts++;
            if (next_guid >= group_end) {
                next_guid = group_start;
                if (group_start == 0)
                    next_guid = 1;
            }
        } while (objects[next_guid].object != NULL && attempts < 0x400);
        if (attempts < 0x400)
            return next_guid;
    }
    return 0;
}

void NetworkObjectManager::AddToLocalObjectList(NetworkObject *object) {
    NetworkObject **slot = local_objects;
    i32 i;
    for (i = 0; i < local_object_count; i++, slot++) {
        if (*slot == NULL)
            break;
    }
    if (i >= local_object_count)
        local_object_count = (i32)(slot - local_objects) + 1;
    *slot = object;
}

void NetworkObjectManager::RemoveFromLocalObjectList(NetworkObject *object) {
    NetworkObject **slot = local_objects;
    for (i32 i = 0; i < local_object_count; i++, slot++) {
        if (*slot == object) {
            *slot = NULL;
            return;
        }
    }
}

void NetworkObjectManager::UpdateLocalObjectList() {
}

NetworkObject *NetworkObjectManager::FindNetworkObject(void *object) {
    if (object != NULL) {
        for (i32 i = 0; i < 2048; i++) {
            if (objects[i].id != 0 && objects[i].object == object)
                return &objects[i];
        }
    }
    return NULL;
}

NetworkObject *NetworkObjectManager::FindNetworkObject(i32 id) {
    if (id != 0 && objects[id].object != NULL)
        return &objects[id];
    return NULL;
}

NetworkObjectManager::PendingObject *NetworkObjectManager::FindPendingObject(NetworkObject *object) {
    for (i32 i = 0; i < 32; i++) {
        if (pending_objects[i].object == object) {
            if (object == NULL) {
                pending_objects[i].requested = 0;
                pending_objects[i].request_time = 0;
            }
            return &pending_objects[i];
        }
    }
    return NULL;
}

NetworkObjectManager::PendingObject *NetworkObjectManager::StealPendingObject() {
    for (i32 i = 0; i < 32; i++) {
        if (pending_objects[i].request_time + 1000 < UtilGetFrameStartTime()) {
            pending_objects[i].request_time = 0;
            pending_objects[i].requested = 0;
            return &pending_objects[i];
        }
    }
    return NULL;
}

void NetworkObjectManager::RemovePendingObject(NetworkObject *object) {
    for (i32 i = 0; i < 32; i++) {
        if (pending_objects[i].object == object) {
            pending_objects[i].object = NULL;
            return;
        }
    }
}

i32 NetworkObjectManager::GetGuid(void *object) {
    if (object != NULL) {
        NetworkObject *network_object = FindNetworkObject(object);
        if (network_object != NULL)
            return network_object->id;
        if (allow_duplicates)
            NetTrace(object);
    }
    return 0;
}

#pragma push_macro("GetObject")
#undef GetObject
void *NetworkObjectManager::GetObject(i32 id) {
    if (id < 1 || id > 2048)
        return NULL;
    return objects[id].object;
}
#pragma pop_macro("GetObject")

i32 NetworkObjectManager::IsLocal(i32 id) {
    i32 local = 1;
    if (id != 0) {
        NetworkObject *network_object = FindNetworkObject(id);
        if (network_object != NULL && !network_object->owner->local)
            local = 0;
    }
    return local;
}

const V2Peer *NetworkObjectManager::Owner(i32 id) {
    if (id != 0) {
        NetworkObject *network_object = FindNetworkObject(id);
        if (network_object != NULL)
            return network_object->owner;
    }
    return NULL;
}

void NetworkObjectManager::NetPeerPush::Sync() {
    next_object = 0;
    stage = 1;
}

void NetworkObjectManager::NetPeerPush::Stop() {
    next_object = 0;
    stage = 0;
}

void NetworkObjectManager::NetPeerPush::NextStage() {
    if (stage < 3) {
        stage++;
        next_object = 0;
    }
}

void NetworkObjectManager::ChangeContext(NOSContext &new_context) {
    PCByteCopy_candidate(&context, &new_context, sizeof(context));
}

void NetworkObjectManager::ReceiveStopMessage(NETMESSAGE_PC &, const V2Peer &peer) {
    for (i32 i = 0; i < 8; i++) {
        if (peer_push[i].peer != NULL && peer_push[i].peer == &peer) {
            peer_push[i].Stop();
            return;
        }
    }
}

i32 NetworkObjectManager::GetPeerStatus() {
    i32 status = 0;
    for (i32 i = 0; i < 8; i++) {
        if (peer_push[i].peer != NULL && peer_push[i].stage != 3) {
            if (status == 0 && peer_push[i].stage > 0 && peer_push[i].stage < 3)
                status = 1;
            else
                status = 2;
        }
    }
    return status;
}

i32 NOSGetGuid() {
    return theNos->GetNextGuid();
}

void NetworkObjectManager::Term() {
    theTTNetwork.RemoveListener(this, 3);
}

i32 NetworkObjectManager::SendPushMessage(NETMESSAGE_PC *message, const NetPeerPush *push, i32 flags) {
    V2Peer *peer = (V2Peer *)push->peer;
    i32 available = 1;
    if (message == NULL || message->data == NULL || (i32)(message->write_offset - message->read_offset) <= 0)
        return 1;
    if (peer != NULL) {
        if ((flags & 1) == 0 || (push->stage > 0 && push->stage < 3))
            theTTNetwork.SendEx(*message, 3, peer, 0, 0);
        else
            theTTNetwork.Send(*message, 3, peer);
        if (peer->GetAvailableMessages() < 16)
            return 0;
        return 1;
    }
    for (NetPeerPush *entry = peer_push; entry != peer_push + 8; entry++) {
        peer = (V2Peer *)entry->peer;
        if (peer == NULL || entry->stage != 3)
            continue;
        if (flags & 1)
            theTTNetwork.Send(*message, 3, peer);
        else
            theTTNetwork.SendEx(*message, 3, peer, 0, 0);
        if (peer->GetAvailableMessages() < 16)
            available = 0;
    }
    return available;
}

void NetworkObjectManager::SendAcquireMessage(NetworkObject *object) {
    NETMESSAGE_PC message(0x20);
    message.Write8(3);
    message.Write16(object->id);
    theTTNetwork.Send(message, 3, (V2Peer *)object->owner);
    message.Release();
}

void NetworkObjectManager::SendAcquiredMessage(i16 id, const V2Peer &peer) {
    NETMESSAGE_PC message(0x20);
    message.Write8(4);
    message.Write16(id);
    theTTNetwork.SendEx(message, 3, (V2Peer *)&peer, 0, 0);
    message.Release();
}

void NetworkObjectManager::SendAdoptedMessage(i16 id) {
    NETMESSAGE_PC message(0x20);
    message.Write8(5);
    message.Write16(id);
    theTTNetwork.BroadcastEx(message, 3);
    message.Release();
}

void NetworkObjectManager::Recover(NetworkObject *object) {
    if (object != NULL) {
        object->owner = theNetSessionManager->local_peer;
        AddToLocalObjectList(object);
        SendAdoptedMessage(object->id);
        theTTNetwork.NosAdopted(object, *object->owner);
    }
}

void NetworkObjectManager::PeerJoined(const V2Peer &peer) {
    if (theNetSessionManager->state == 4 || theNetSessionManager->state == 3) {
        if (!peer.local) {
            i32 i;
            for (i = 0; i < 2; i++) {
                if (guid_peers[i] == NULL)
                    break;
            }
            if (i < 2) {
                guid_peers[i] = &peer;
                NETMESSAGE_PC message(0x20);
                message.Write8(0);
                message.Write32(i);
                theTTNetwork.SendEx(message, 3, (V2Peer *)&peer, 0, 0);
                message.Release();
            } else {
                NetTrace();
            }
        }
    }
    if (peer.local)
        return;
    if (active) {
        NETMESSAGE_PC message(0x20);
        message.Write8(9);
        message.Write(&context, sizeof(context));
        theTTNetwork.SendEx(message, 3, (V2Peer *)&peer, 0, 0);
        message.Release();
    }
    for (i32 i = 0; i < 8; i++) {
        if (peer_push[i].peer == NULL) {
            peer_push[i].peer = &peer;
            peer_push[i].Stop();
            return;
        }
    }
}

void NetworkObjectManager::PeerLeft(const V2Peer &peer, i32 reason) {
    if (!peer.local && (theNetSessionManager->state == 4 || theNetSessionManager->state == 3)) {
        for (i32 i = 0; i < 2; i++) {
            if (guid_peers[i] == &peer)
                guid_peers[i] = NULL;
        }
        for (i32 i = 0; i < 2048; i++) {
            if (objects[i].id != 0 && objects[i].owner == &peer)
                Recover(&objects[i]);
        }
    }
    for (i32 i = 0; i < 8; i++) {
        if (peer_push[i].peer != NULL && peer_push[i].peer == &peer) {
            peer_push[i].peer = NULL;
            peer_push[i].Stop();
        }
    }
}

i32 NetworkObjectManager::Acquire(i32 id) {
    i32 result = 0;
    NetworkObject *object;
    if (id != 0 && (object = FindNetworkObject(id)) != NULL) {
        if (object->owner->local) {
            result = 1;
        } else if (object->flags & 8) {
            if (!IsPeerReady(*object->owner)) {
                result = -1;
            } else if ((object->flags & 0x20) == 0) {
                PendingObject *pending = FindPendingObject(object);
                if (pending == NULL) {
                    pending = FindPendingObject(NULL);
                    if (pending == NULL)
                        pending = StealPendingObject();
                }
                if (pending != NULL) {
                    u32 now = UtilGetFrameStartTime();
                    if (pending->request_time < now) {
                        pending->requested = 1;
                        pending->request_time = now + 250;
                        pending->object = object;
                        SendAcquireMessage(object);
                    }
                }
            }
        }
    } else {
        result = -1;
    }
    return result;
}

i32 NetworkObjectManager::ReleaseObject(void *object, EdClass *, i32 guid) {
    if (object == NULL)
        return 1;
    NetworkObject *network_object;
    if (guid != 0)
        network_object = FindNetworkObject(guid);
    else
        network_object = FindNetworkObject(object);
    if (network_object == NULL)
        return 1;
    RemoveFromLocalObjectList(network_object);
    if (active) {
        i16 id = network_object->id;
        i16 class_id = theRegistry.GetClassId(network_object->object_class);
        NETMESSAGE_PC message(0x20);
        message.Write8(6);
        message.Write16(&id);
        message.Write16(&class_id);
        theTTNetwork.BroadcastEx(message, 3);
        message.Release();
    }
    network_object->Destroy();
    return 1;
}

void NetworkObjectManager::ContinuityBreak(i32 id, f32) {
    if (id == 0)
        return;
    NetworkObject *object = FindNetworkObject(id);
    if (object == NULL)
        return;
    object->flags |= 2;
    if (object->owner->local) {
        NETMESSAGE_PC message(0x20);
        i16 object_id = object->id;
        i16 class_id = theRegistry.GetClassId(object->object_class);
        message.Write8(12);
        message.Write16(&object_id);
        message.Write16(&class_id);
        theTTNetwork.BroadcastEx(message, 3);
        message.Release();
    }
}

void NetworkObjectManager::NotifyDestroyObject(void *object, EdClass *object_class, i32 guid, i32 flags) {
    if ((flags & 1) == 0)
        ReleaseObject(object, object_class, guid);
}

void NetworkObjectManager::ConstructObject(NetworkObject *object, NetPeerPush *push) {
    u8 constructor_data[256];
    EdClassInterface *object_interface = object->object_class->object_interface;
    i16 size = object_interface->GetConstructorData(object->object, constructor_data, sizeof(constructor_data));
    i16 class_id = theRegistry.GetClassId(object->object_class);
    NETMESSAGE_PC *message = push->GetReliableMessage(size + 10);
    message->Write8(1);
    message->Write16(object->id);
    message->Write16(class_id);
    message->Write16(size);
    if (size > 0)
        message->Write(constructor_data, size);
}

#pragma push_macro("GetMessage")
#undef GetMessage
NETMESSAGE_PC *NetworkObjectManager::NetPeerPush::GetMessage(i32 size) {
    if (message != NULL) {
        i32 available = message->data != NULL ? 0x4af - (i32)message->write_offset : 0;
        if (available < size) {
            theNos->SendPushMessage(message, this, 1);
            if (message != NULL) {
                message->Release();
                theMemoryManager.FreePool(message, sizeof(NETMESSAGE_PC));
            }
            message = NULL;
        }
    }
    if (message == NULL)
        message = new (theMemoryManager.AllocPool(sizeof(NETMESSAGE_PC), 1)) NETMESSAGE_PC(0x20);
    return message;
}
#pragma pop_macro("GetMessage")

NETMESSAGE_PC *NetworkObjectManager::NetPeerPush::GetReliableMessage(i32 size) {
    if (reliable_message != NULL) {
        i32 available = reliable_message->data != NULL ? 0x4af - (i32)reliable_message->write_offset : 0;
        if (available < size) {
            theNos->SendPushMessage(reliable_message, this, 0);
            if (reliable_message != NULL) {
                reliable_message->Release();
                theMemoryManager.FreePool(reliable_message, sizeof(NETMESSAGE_PC));
            }
            reliable_message = NULL;
        }
    }
    if (reliable_message == NULL)
        reliable_message = new (theMemoryManager.AllocPool(sizeof(NETMESSAGE_PC), 1)) NETMESSAGE_PC(0x20);
    return reliable_message;
}

void NetworkObjectManager::NetPeerPush::FlushMessages() {
    if (message != NULL) {
        theNos->SendPushMessage(message, this, 1);
        if (message != NULL) {
            message->Release();
            theMemoryManager.FreePool(message, sizeof(NETMESSAGE_PC));
        }
        message = NULL;
    }
    if (reliable_message != NULL) {
        theNos->SendPushMessage(reliable_message, this, 0);
        if (reliable_message != NULL) {
            reliable_message->Release();
            theMemoryManager.FreePool(reliable_message, sizeof(NETMESSAGE_PC));
        }
        reliable_message = NULL;
    }
}

i32 NetworkObjectManager::RegisterObject(void *object, EdClass *object_class, i32 guid) {
    if (guid == 0)
        guid = GetNextGuid();
    if (guid <= 0)
        return guid;
    for (i32 i = 0; i < 2048; i++) {
        if (objects[i].object == object) {
            if (!allow_duplicates)
                return 0;
            NetTrace();
        }
    }
    NetworkObject *network_object = &objects[guid];
    network_object->Initialise(guid, object, object_class, *theNetSessionManager->local_peer, 1);
    AddToLocalObjectList(network_object);
    if (active) {
        ConstructObject(network_object, &default_push);
        default_push.FlushMessages();
    }
    return guid;
}

void NetworkObjectManager::NotifyCreateObject(void *object, EdClass *object_class, void *, i32, i32 guid, i32 flags) {
    if ((flags & 1) == 0)
        RegisterObject(object, object_class, guid);
}

void NetworkObjectManager::Init() {
    guid_peers[0] = NULL;
    guid_peers[1] = NULL;
    memset(objects, 0, sizeof(objects));
    memset(local_objects, 0, sizeof(local_objects));
    theTTNetwork.AddListener(this, 3, "NOS");
    theRegistry.AddObjectNotifier(this);
    guid_group = 0;
    guid_peers[0] = (const V2Peer *)-1;
    allow_duplicates = 1;
}

void NetworkObjectManager::InitClassStats() {
    for (i32 i = 0; i < theRegistry.class_count; i++) {
        EdClass *object_class = theRegistry.GetClass(i);
        if (object_class->object_interface != NULL)
            class_stats[i] = new NetStats(object_class->name);
    }
}
