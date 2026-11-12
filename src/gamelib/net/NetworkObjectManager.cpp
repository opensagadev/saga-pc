#include "saga.h"

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
void NOSTrace(...);

/* Serialises replicated members into a NETMESSAGE_PC (Android NetOutputStream). */
class NetOutputStream : public EdOutputStream {
  public:
    NetOutputStream() {
    }
    virtual ~NetOutputStream() {
    }
    virtual i32 Eat(i32, i32) {
        return 0;
    }
    virtual i32 SerialiseBuffer(void *data, i32 size, i32 count) {
        message->Write(data, size, count);
        return size * count;
    }
    virtual char const *BeginBlock(char const *name) {
        return name;
    }
    virtual void EndBlock() {
    }
    void SetMessage(NETMESSAGE_PC *new_message) {
        message = new_message;
    }

    NETMESSAGE_PC *message;
};
STATIC_ASSERT(sizeof(NetOutputStream) == 0x24);

/* Reads replicated members back out of a NETMESSAGE_PC (Android NetInputStream). */
class NetInputStream : public EdInputStream {
  public:
    NetInputStream() : message(NULL) {
    }
    virtual ~NetInputStream() {
    }
    virtual i32 Eat(i32 size, i32 count) {
        i32 bytes = size * count;
        if (message->data != NULL)
            message->read_offset += bytes;
        return bytes;
    }
    virtual i32 SerialiseBuffer(void *data, i32 size, i32 count) {
        message->Read(data, size, count);
        return size * count;
    }
    virtual char const *BeginBlock(char const *name) {
        return name;
    }
    virtual void EndBlock() {
    }
    void SetMessage(NETMESSAGE_PC *new_message) {
        message = new_message;
    }

    NETMESSAGE_PC *message;
};
STATIC_ASSERT(sizeof(NetInputStream) == 0x24);

/* Replicator data is a packed run of word-aligned per-object records. */
#define REPLICATOR_ALIGN(cursor) (((u32)(cursor) + 3) & ~3)

/* Replicators the game binds to its classes (Android Network.cpp). Retail
 * never constructs them, so no vtables or RTTI are emitted. */
class NetSimpleReplicator : public NetReplicator {
  public:
    virtual i32 AllowPush(const EdClass *object_class, const void *object, ReplicatorData &data, i32 force,
                          i32 check_only);
};

class NetConstReplicator : public NetReplicator {
  public:
    virtual i32 AllowPush(const EdClass *object_class, const void *object, ReplicatorData &data, i32 force,
                          i32 check_only);
};

class NetChangedReplicator : public NetReplicator {
  public:
    virtual i32 AllowPush(const EdClass *object_class, const void *object, ReplicatorData &data, i32 force,
                          i32 skip_checksum);
    void CheckSum(u8 byte, u32 &checksum) const;
    void CheckSum(const u8 *bytes, u32 size, u32 &checksum) const;
    void CheckSumObject(const EdClass *object_class, const void *object, u32 &checksum) const;
    void InitTable();

    static i16 mTableInited;
    static i16 mCrc32Table[256];
};

class NetPredictor : public NetReplicator {
  public:
    struct PredictorData {
        f32 values[3];
    };
    struct PredictorTime {
        i32 sample_count;
        f32 values[3];
        f32 factors[3];
    };

    virtual i32 AllowPush(const EdClass *object_class, const void *object, ReplicatorData &data, i32 force,
                          i32 check_only);
    virtual i32 SerialiseObject(EdStream &stream, V2Peer *peer, const EdClass *object_class, void *object,
                                ReplicatorData &data, i16 *class_mapping);
    virtual i32 DoPrediction(const EdClass *object_class, void *object, ReplicatorData &data, i32 check_only);
    virtual void StoreSampleData(const EdClass *object_class, void *object, PredictorTime *time,
                                 PredictorData **samples, f32 *values, i32 count);
    virtual i32 CheckPredictionError(const EdClass *object_class, void *object, f32 *actual, f32 *predicted, i32 count);
    virtual void PredictValue(const EdClass *object_class, void *object, PredictorTime *time, PredictorData **samples,
                              f32 *values, i32 count) = 0;
    i32 SerialiseObject(EdStream &stream, V2Peer *peer, const EdClass *object_class, void *object, ReplicatorData &data,
                        PredictorTime *time, i16 *class_mapping);
    i32 DoPrediction(const EdClass *object_class, void *object, ReplicatorData &data, PredictorTime *time,
                     i32 check_only);

    f32 maximum_prediction_error;
    f32 maximum_sample_delta;
    f32 minimum_value;
    f32 maximum_value;
};
STATIC_ASSERT(sizeof(NetPredictor) == 0x28);
STATIC_ASSERT(sizeof(NetPredictor::PredictorTime) == 0x1c);
STATIC_ASSERT(offsetof(V2Peer, time_offset) == 0x29c);

class NetPredictor2 : public NetPredictor {
  public:
    virtual void PredictValue(const EdClass *object_class, void *object, PredictorTime *time, PredictorData **samples,
                              f32 *values, i32 count);
};

class NetPredictor3 : public NetPredictor {
  public:
    virtual void PredictValue(const EdClass *object_class, void *object, PredictorTime *time, PredictorData **samples,
                              f32 *values, i32 count);
};

class NetRotator2 : public NetPredictor {
  public:
    virtual void PredictValue(const EdClass *object_class, void *object, PredictorTime *time, PredictorData **samples,
                              f32 *values, i32 count);
};

i32 ForceDummySerialise = 0;
NetworkObjectManager *theNos = NULL;

void NetChangedReplicator::CheckSum(u8 byte, u32 &checksum) const {
    checksum = (checksum >> 8) ^ mCrc32Table[(checksum & 0xff) ^ byte];
}

i32 NetworkObjectManager::NetPeerPush::IsReady() const {
    return stage == 3;
}

i32 NetworkObjectManager::NetPeerPush::IsSyncing() const {
    return stage > 0 && stage < 3;
}

i32 NetworkObjectManager::NetPeerPush::GetStage() const {
    return stage;
}

i32 NetworkObjectManager::NetPeerPush::GetNextObject() const {
    return next_object;
}

void NetworkObjectManager::NetPeerPush::SetNextObject(i32 index) {
    next_object = index;
}

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

i16 NetChangedReplicator::mTableInited = 0;
i16 NetChangedReplicator::mCrc32Table[256] = {0};

void NetChangedReplicator::InitTable() {
    if (mTableInited != 0)
        return;
    mTableInited = 1;
    for (i32 i = 0; i < 256; i++) {
        u32 value = i;
        for (i32 bit = 0; bit < 8; bit++) {
            if (value & 1)
                value = (value >> 1) ^ 0xedb88320;
            else
                value >>= 1;
        }
        mCrc32Table[i] = (i16)value;
    }
}

void NetChangedReplicator::CheckSum(const u8 *bytes, u32 size, u32 &checksum) const {
    for (u32 i = 0; i < size; i++)
        CheckSum(*bytes++, checksum);
    checksum = ~checksum;
}

void NetPredictor::StoreSampleData(const EdClass *, void *, PredictorTime *time, PredictorData **samples, f32 *values,
                                   i32 count) {
    for (i32 i = 0; i < count; i++) {
        samples[i]->values[0] = samples[i]->values[1];
        samples[i]->values[1] = samples[i]->values[2];
        samples[i]->values[2] = values[i];
        if (maximum_sample_delta > 0.0f && samples[i]->values[2] - samples[i]->values[1] > maximum_sample_delta)
            time->sample_count = 0;
    }
}

i32 NetPredictor::CheckPredictionError(const EdClass *, void *, f32 *actual, f32 *predicted, i32 count) {
    for (i32 i = 0; i < count; i++) {
        f32 error = actual[i] - predicted[i];
        if (error > maximum_prediction_error || error < -maximum_prediction_error)
            return 1;
    }
    return 0;
}

void NetPredictor2::PredictValue(const EdClass *, void *, PredictorTime *time, PredictorData **samples, f32 *values,
                                 i32 count) {
    f32 now = (f32)UtilGetFrameStartTime();
    f32 interval_scale = 1.0f / (time->values[2] - time->values[1]);
    f32 elapsed = now - time->values[2];
    for (i32 i = 0; i < count; i++) {
        f32 delta = samples[i]->values[2] - samples[i]->values[1];
        f32 velocity = delta * interval_scale;
        f32 predicted = velocity * elapsed + samples[i]->values[2];
        if (replication_group & 0x10)
            predicted = predicted > minimum_value ? predicted : minimum_value;
        if (replication_group & 0x20)
            predicted = predicted < maximum_value ? predicted : maximum_value;
        values[i] = values[i] * 0.8f + predicted * 0.2f;
    }
}

void NetPredictor3::PredictValue(const EdClass *, void *, PredictorTime *time, PredictorData **samples, f32 *values,
                                 i32 count) {
    f32 now = (f32)UtilGetFrameStartTime();
    for (i32 i = 0; i < count; i++) {
        f32 weight0 = (now - time->values[1]) * (now - time->values[2]) * time->factors[0];
        f32 weight1 = (now - time->values[0]) * (now - time->values[2]) * time->factors[1];
        f32 weight2 = (now - time->values[0]) * (now - time->values[1]) * time->factors[2];
        f32 predicted =
            weight0 * samples[i]->values[0] + weight1 * samples[i]->values[1] + weight2 * samples[i]->values[2];
        if (replication_group & 0x10)
            predicted = predicted > minimum_value ? predicted : minimum_value;
        if (replication_group & 0x20)
            predicted = predicted < maximum_value ? predicted : maximum_value;
        values[i] = values[i] * 0.8f + predicted * 0.2f;
    }
}

void NetRotator2::PredictValue(const EdClass *, void *, PredictorTime *time, PredictorData **samples, f32 *values,
                               i32 count) {
    f32 now = (f32)UtilGetFrameStartTime();
    f32 interval_scale = 1.0f / (time->values[2] - time->values[1]);
    f32 elapsed = now - time->values[2];
    for (i32 i = 0; i < count; i++) {
        i32 delta = ((i32)samples[i]->values[2] - (i32)samples[i]->values[1]) & 0xffff;
        if (delta >= 0x8000)
            delta -= 0x10000;
        f32 velocity = delta * interval_scale;
        f32 predicted = velocity * elapsed + samples[i]->values[2];
        if (replication_group & 0x10)
            predicted = predicted > minimum_value ? predicted : minimum_value;
        if (replication_group & 0x20)
            predicted = predicted < maximum_value ? predicted : maximum_value;
        values[i] = predicted;
    }
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
            return peer_push[i].IsReady();
    }
    return 0;
}

void NetworkObjectManager::FlushObjects(i32 flush_level_editor) {
    NetworkObject *object = objects;
    for (i32 i = 0; i < 2048; i++, object++) {
        if (object->id != 0) {
            object->object_class->object_interface->SetObjectGuid(object->object, 0);
            object->Destroy();
        }
    }
    memset(local_objects, 0, sizeof(local_objects));
    local_object_count = 0;
    memset(objects, 0, sizeof(objects));
    if (flush_level_editor)
        theLevelEditor.Flush();
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
            pending_objects[i].requested = 0;
            pending_objects[i].request_time = 0;
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
        if (peer_push[i].peer != NULL && !peer_push[i].IsReady()) {
            if (status == 0 && peer_push[i].IsSyncing())
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

NetworkObjectManager::NetworkObjectManager() {
    active = 0;
    guid_group = -1;
    next_guid = -1;
    theNos = this;
    theRegistry.create_object_guid = NOSGetGuid;
}

NetworkObjectManager::~NetworkObjectManager() {
    theNos = NULL;
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
    } else {
        for (i32 i = 0; i < 8; i++) {
            peer = (V2Peer *)peer_push[i].peer;
            if (peer == NULL || !peer_push[i].IsReady())
                continue;
            if (flags & 1)
                theTTNetwork.Send(*message, 3, peer);
            else
                theTTNetwork.SendEx(*message, 3, peer, 0, 0);
            if (peer->GetAvailableMessages() < 16)
                available = 0;
        }
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
            if (&peer == guid_peers[i])
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
        message.Write16(id);
        message.Write16(class_id);
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
        message.Write16(object_id);
        message.Write16(class_id);
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
        i32 available = message->data == NULL ? 0 : 0x4af - (i32)message->write_offset;
        if (available < size) {
            theNos->SendPushMessage(message, this, 1);
            NETMESSAGE_PC *sent = message;
            if (sent != NULL) {
                sent->Release();
                delete sent;
            }
            message = NULL;
        }
    }
    if (message == NULL)
        message = new NETMESSAGE_PC(0x20);
    return message;
}
#pragma pop_macro("GetMessage")

NETMESSAGE_PC *NetworkObjectManager::NetPeerPush::GetReliableMessage(i32 size) {
    if (reliable_message != NULL) {
        i32 available = reliable_message->data == NULL ? 0 : 0x4af - (i32)reliable_message->write_offset;
        if (available < size) {
            theNos->SendPushMessage(reliable_message, this, 0);
            NETMESSAGE_PC *sent = reliable_message;
            if (sent != NULL) {
                sent->Release();
                delete sent;
            }
            reliable_message = NULL;
        }
    }
    if (reliable_message == NULL)
        reliable_message = new NETMESSAGE_PC(0x20);
    return reliable_message;
}

void NetworkObjectManager::NetPeerPush::FlushMessages() {
    if (message != NULL) {
        theNos->SendPushMessage(message, this, 1);
        NETMESSAGE_PC *sent = message;
        if (sent != NULL) {
            sent->Release();
            delete sent;
        }
        message = NULL;
    }
    if (reliable_message != NULL) {
        theNos->SendPushMessage(reliable_message, this, 0);
        NETMESSAGE_PC *sent = reliable_message;
        if (sent != NULL) {
            sent->Release();
            delete sent;
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

#pragma push_macro("GetMessage")
#undef GetMessage
i32 NetworkObjectManager::Push(const NetworkObject *object, NetReplicator *replicator, ReplicatorData &data,
                               NetPeerPush *push) {
    if (push == NULL)
        push = &default_push;
    i32 size = replicator->message_size + 10;
    NETMESSAGE_PC *message;
    if (replicator->replication_group & 1)
        message = push->GetMessage(size);
    else
        message = push->GetReliableMessage(size);
    NetOutputStream stream;
    i16 object_id = object->id;
    i16 class_id = (i16)theRegistry.GetClassId(object->object_class);
    i16 flags = object->flags;
    i16 replicator_id = replicator->id;
    message->Write8(2);
    message->Write16(object_id);
    message->Write16(class_id);
    message->Write16(replicator_id);
    message->Write16(flags);
    i32 initial_size = message->GetSize();
    stream.SetMessage(message);
    if (!replicator->SerialiseObject(stream, NULL, object->object_class, object->object, data, &flags) &&
        message->data != NULL)
        message->write_offset -= 9;
    if (class_stats[class_id] != NULL)
        class_stats[class_id]->totals.values[0] += message->GetSize() - initial_size;
    return 1;
}

#pragma pop_macro("GetMessage")

i32 NetworkObjectManager::PushObject(NetworkObject *object, NetPeerPush *push, i32 force) {
    i32 result = 1;
    if (push != NULL && push->IsSyncing())
        object->flags |= 4;
    else
        object->flags &= ~4u;
    if (push == NULL)
        push = &default_push;
    i32 class_id = theRegistry.GetClassId(object->object_class);
    i32 allowed = 1;
    if (filters[class_id] != NULL && !force)
        allowed = filters[class_id]->AllowPush(object->object_class, object->object);
    i32 limit;
    if (theNuNetEmu.packet_stats.pack_ratio > 0.0f)
        limit = (i32)(theNuNetEmu.packet_stats.pack_ratio * theNuNetEmu.queued_bytes);
    else
        limit = theNuNetEmu.queued_bytes;
    i32 permit = 1;
    if (allowed || force) {
        NetReplicator *replicator = replicators[class_id].head;
        i32 offset = 0;
        while (replicator != NULL) {
            if (object->flags & 2)
                replicator->replication_group |= 8;
            i32 size = replicator->data_size;
            ReplicatorData data;
            data.start = data.cursor = (u8 *)object->replicator_data + offset;
            data.end = data.start + size;
            offset += size;
            if (limit > theNuNetEmu.field_00 && (replicator->replication_group & 0x40) == 0)
                permit = 0;
            if ((permit || force) && replicator->AllowPush(object->object_class, object->object, data, force, 0) &&
                !Push(object, replicator, data, push))
                result = 0;
            replicator = replicator->next;
        }
    }
    if (object->flags & 2)
        object->flags &= ~2u;
    return result;
}

void NetworkObjectManager::Update() {
    for (i32 i = 0; i < 2048; i++) {
        NetworkObject *object = &objects[i];
        if (object->id == 0)
            continue;
        if (object->owner->local) {
            PushObject(object, NULL, 0);
        } else if ((object->flags & 2) == 0) {
            NetReplicator *replicator = replicators[theRegistry.GetClassId(object->object_class)].head;
            i32 offset = 0;
            while (replicator != NULL) {
                i32 size = replicator->data_size;
                if (replicator->replication_group & 4) {
                    ReplicatorData data;
                    data.start = data.cursor = (u8 *)object->replicator_data + offset;
                    data.end = data.start + size;
                    replicator->AllowPush(object->object_class, object->object, data, 0, 0);
                    replicator->DoPrediction(object->object_class, object->object, data, 0);
                }
                replicator = replicator->next;
                offset += size;
            }
        }
    }
    for (i32 i = 0; i < 8; i++) {
        NetPeerPush *push = &peer_push[i];
        if (push->peer == NULL)
            continue;
        if (push->IsSyncing()) {
            i32 index = push->GetNextObject();
            i32 limit;
            if (theNuNetEmu.packet_stats.pack_ratio > 0.0f)
                limit = (i32)(theNuNetEmu.packet_stats.pack_ratio * theNuNetEmu.queued_bytes);
            else
                limit = theNuNetEmu.queued_bytes;
            while (index < local_object_count && limit < theNuNetEmu.field_00) {
                NetworkObject *object = local_objects[index++];
                if (object == NULL || object->object == NULL)
                    continue;
                if (push->GetStage() == 1)
                    ConstructObject(object, push);
                if (push->GetStage() == 2)
                    PushObject(object, push, 1);
                if (((V2Peer *)push->peer)->GetAvailableMessages() < 16)
                    break;
                if (theNuNetEmu.packet_stats.pack_ratio > 0.0f)
                    limit = (i32)(theNuNetEmu.packet_stats.pack_ratio * theNuNetEmu.queued_bytes);
                else
                    limit = theNuNetEmu.queued_bytes;
            }
            if (index < local_object_count)
                push->SetNextObject(index);
            else
                push->NextStage();
        }
        push->FlushMessages();
    }
    default_push.FlushMessages();
    for (i32 i = 0; i < 32; i++) {
        if (class_stats[i] != NULL)
            class_stats[i]->Update();
    }
}

NOSContext &NOSContext::operator=(const NOSContext &other) {
    PCByteCopy_candidate(this, &other, sizeof(*this));
    return *this;
}

bool NOSContext::operator==(const NOSContext &other) const {
    return memcmp(bytes, other.bytes, sizeof(bytes)) == 0;
}

void NetworkObjectManager::ReceiveAcquireMessage(NETMESSAGE_PC &message, const V2Peer &peer) {
    i16 id;
    message.Read16(id);
    NetworkObject *object = &objects[id];
    if (object->id == id && object->owner->local && theTTNetwork.NosAcquire(object, peer) == 1) {
        RemoveFromLocalObjectList(object);
        objects[id].owner = &peer;
        objects[id].flags |= 0x20;
        theTTNetwork.NosAdopted(&objects[id], peer);
        SendAcquiredMessage(id, peer);
    }
}

void NetworkObjectManager::ReceiveAcquiredMessage(NETMESSAGE_PC &message, const V2Peer &) {
    i16 id;
    message.Read16(id);
    objects[id].owner = theNetSessionManager->local_peer;
    AddToLocalObjectList(&objects[id]);
    SendAdoptedMessage(id);
    theTTNetwork.NosAdopted(&objects[id], *objects[id].owner);
    RemovePendingObject(&objects[id]);
}

void NetworkObjectManager::ReceiveConstructorMessage(NETMESSAGE_PC &message, const V2Peer &peer) {
    i16 guid;
    i16 class_id;
    i16 size;
    u8 constructor_data[256];
    message.Read16(guid);
    message.Read16(class_id);
    message.Read16(size);
    if (size > 0)
        message.Read(constructor_data, size);
    EdClass *object_class = theRegistry.GetClass(class_id);
    NetworkObject *network_object = &objects[guid];
    if (network_object->object == NULL) {
        void *object = theRegistry.CreateObject(object_class->object_interface, constructor_data, size, guid, 1);
        network_object->Initialise(guid, object, object_class, peer, 0);
    }
}

void NetworkObjectManager::ReceiveReplicaMessage(NETMESSAGE_PC &message, const V2Peer &peer) {
    NetInputStream stream;
    i16 guid;
    i16 class_id;
    i16 replicator_id;
    i16 flags;
    i32 offset = 0;
    message.Read16(guid);
    message.Read16(class_id);
    message.Read16(replicator_id);
    message.Read16(flags);
    EdClass *object_class = theRegistry.GetClass(class_id);
    NetworkObject *object = &objects[guid];
    object->flags = flags | 8;
    NetReplicator *replicator;
    for (replicator = replicators[class_id].head; replicator != NULL; replicator = replicator->next) {
        if (replicator_id == replicator->id)
            break;
        offset += replicator->data_size;
    }
    if (object->object != NULL && object->object_class == object_class) {
        if (object->flags & 2)
            replicator->replication_group |= 8;
        ReplicatorData data;
        data.start = data.cursor = (u8 *)object->replicator_data + offset;
        data.end = data.start + replicator->data_size;
        replicator->AllowPush(object->object_class, object->object, data, 1, 0);
        stream.SetMessage(&message);
        replicator->SerialiseObject(stream, (V2Peer *)&peer, object->object_class, object->object, data,
                                    (i16 *)&object->flags);
    } else if (message.data != NULL) {
        message.read_offset += replicator->message_size;
    }
}

void NetworkObjectManager::ReceiveAdoptedMessage(NETMESSAGE_PC &message, const V2Peer &peer) {
    i16 id;
    message.Read16(id);
    objects[id].flags &= ~0x20u;
    if (objects[id].owner != &peer) {
        objects[id].owner = &peer;
        theTTNetwork.NosAdopted(&objects[id], peer);
    }
}

void NetworkObjectManager::ReceiveReleaseMessage(NETMESSAGE_PC &message, const V2Peer &) {
    i16 id;
    i16 class_id;
    message.Read16(id);
    message.Read16(class_id);
    NetworkObject *object = &objects[id];
    if (id == object->id && class_id == theRegistry.GetClassId(object->object_class) && object->object != NULL) {
        theRegistry.DestroyObject(object->object_class->object_interface, object->object, id, 1);
        RemoveFromLocalObjectList(object);
        object->Destroy();
    }
}

void NetworkObjectManager::ReceiveRemoteCallMessage(NETMESSAGE_PC &message, const V2Peer &) {
    i8 call_id;
    message.Read8((u8 &)call_id);
    if (call_id > 0) {
        call_id--;
        if (registered_calls[call_id].type == 0)
            ((void (*)(NETMESSAGE_PC &))registered_calls[call_id].callback)(message);
    }
}

void NetworkObjectManager::ReceiveObjectCallMessage(NETMESSAGE_PC &message, const V2Peer &peer) {
    i8 call_id;
    message.Read8((u8 &)call_id);
    if (call_id > 0) {
        call_id--;
        if (registered_calls[call_id].type == 1) {
            i16 id;
            i16 class_id;
            message.Read16(id);
            message.Read16(class_id);
            EdClass *object_class = theRegistry.GetClass(class_id);
            NetworkObject *object = &objects[id];
            if (object->object == NULL && (registered_calls[call_id].flags & 1))
                object->Initialise(id, theRegistry.CreateObject(object_class->object_interface, NULL, 0, id, 1),
                                   object_class, peer, 0);
            if (object->object != NULL) {
                if (object->object_class != object_class)
                    NetTrace(object->object_class->name, object_class->name, id);
                ((void (*)(void *, NETMESSAGE_PC &))registered_calls[call_id].callback)(object->object, message);
            }
        }
    }
}

void NetworkObjectManager::ReceiveContinuityBreak(NETMESSAGE_PC &message, const V2Peer &) {
    i16 id;
    i16 class_id;
    message.Read16(id);
    message.Read16(class_id);
    EdClass *object_class = theRegistry.GetClass(class_id);
    NetworkObject *object = &objects[id];
    if (object->object_class == object_class)
        object->flags &= 2;
}

void NetworkObjectManager::ReceiveStartMessage(NETMESSAGE_PC &message, const V2Peer &peer) {
    NOSContext received_context;
    message.Read(&received_context, sizeof(received_context));
    if (!active)
        return;
    for (i32 i = 0; i < 8; i++) {
        if (peer_push[i].peer != NULL && peer_push[i].peer == &peer) {
            i32 accepted;
            if (received_context == context) {
                accepted = 1;
                peer_push[i].Sync();
            } else {
                accepted = 0;
                peer_push[i].Stop();
            }
            NETMESSAGE_PC reply(0x20);
            reply.Write8(11);
            reply.Write32(accepted);
            reply.Write(&context, sizeof(context));
            theTTNetwork.SendEx(reply, 3, (V2Peer *)&peer, 0, 0);
            reply.Release();
            return;
        }
    }
}

void NetworkObjectManager::ReceiveStatusMessage(NETMESSAGE_PC &message, const V2Peer &peer) {
    NOSContext received_context;
    i32 accepted;
    message.Read32(accepted);
    message.Read(&received_context, sizeof(received_context));
    i32 matching = accepted && received_context == context;
    for (i32 i = 0; i < 8; i++) {
        if (peer_push[i].peer != NULL && peer_push[i].peer == &peer) {
            if (matching) {
                if (!peer_push[i].IsReady() && !peer_push[i].IsSyncing())
                    peer_push[i].Sync();
            } else {
                peer_push[i].Stop();
            }
            return;
        }
    }
}

void NetworkObjectManager::CalcReplicatorDataSize(NetReplicator *replicator, const EdClass *object_class,
                                                  i32 &data_size, i32 &message_size) {
    u8 data_memory[0x1400];
    u8 object_memory[0x1400];
    memset(data_memory, 0, sizeof(data_memory));
    memset(object_memory, 0, sizeof(object_memory));
    ReplicatorData data;
    data.start = data_memory;
    data.end = data_memory + 0x100;
    data.cursor = data_memory;
    replicator->AllowPush(object_class, object_memory, data, 1, 1);
    NetOutputStream stream;
    NETMESSAGE_PC message(0x20);
    i16 flags = 0x10;
    ForceDummySerialise = 1;
    stream.SetMessage(&message);
    replicator->SerialiseObject(stream, NULL, object_class, object_memory, data, &flags);
    data_size = data.cursor - data.start;
    ForceDummySerialise = 0;
    message_size = message.GetSize();
    message.Release();
}

void NetworkObjectManager::BindReplicator(NetReplicator *replicator, const EdClass *object_class) {
    i32 class_id = theRegistry.GetClassId((EdClass *)object_class);
    ReplicatorList &list = replicators[class_id];
    replicator->next = NULL;
    replicator->previous = list.tail;
    if (list.tail != NULL)
        list.tail->next = replicator;
    list.tail = replicator;
    if (list.head == NULL)
        list.head = replicator;
    list.count++;
    i32 data_size;
    i32 message_size;
    CalcReplicatorDataSize(replicator, object_class, data_size, message_size);
    replicator_data_sizes[class_id] += data_size;
    replicator->data_size = data_size;
    replicator->message_size = message_size;
}

void NetworkObjectManager::Start(const NOSContext &new_context) {
    if (!active) {
        active = 1;
        PCByteCopy_candidate(&context, &new_context, sizeof(context));
        NETMESSAGE_PC message(0x20);
        message.Write8(9);
        message.Write(&context, sizeof(context));
        theTTNetwork.BroadcastEx(message, 3);
        message.Release();
    }
}

void NetworkObjectManager::Stop() {
    if (active) {
        active = 0;
        NETMESSAGE_PC message(0x20);
        message.Write8(10);
        theTTNetwork.BroadcastEx(message, 3);
        for (NetPeerPush *push = peer_push; push != peer_push + 8; push++) {
            if (push->peer != NULL)
                push->Stop();
        }
        message.Release();
    }
}

i32 NetworkObjectManager::ObjectCall(void *instance, i32 call_id, NETMESSAGE_PC message, const V2Peer *peer) {
    i32 result = 0;
    NetworkObject *object = FindNetworkObject(instance);
    if (object != NULL) {
        i16 class_id = theRegistry.GetClassId(object->object_class);
        i16 object_id = object->id;
        NETMESSAGE_PC outgoing(message);
        outgoing.Prepend16(class_id);
        outgoing.Prepend16(object_id);
        outgoing.Prepend8(call_id);
        outgoing.Prepend8(8);
        if (peer != NULL) {
            if (peer->local)
                ((void (*)(void *, NETMESSAGE_PC &))registered_calls[call_id - 1].callback)(instance, message);
            else
                theTTNetwork.SendEx(outgoing, 3, (V2Peer *)peer, 0, 0);
        } else {
            theTTNetwork.BroadcastEx(outgoing, 3);
            ((void (*)(void *, NETMESSAGE_PC &))registered_calls[call_id - 1].callback)(instance, message);
        }
        result = 1;
        outgoing.Release();
    }
    message.Release();
    return result;
}

i32 NetworkObjectManager::ObjectOwnerCall(void *instance, i32 call_id, NETMESSAGE_PC message) {
    i32 result = 0;
    NetworkObject *object = FindNetworkObject(instance);
    if (object != NULL) {
        if (object->owner->local) {
            ((void (*)(void *, NETMESSAGE_PC &))registered_calls[call_id - 1].callback)(instance, message);
        } else {
            i16 class_id = theRegistry.GetClassId(object->object_class);
            i16 object_id = object->id;
            NETMESSAGE_PC outgoing(message);
            outgoing.Prepend16(class_id);
            outgoing.Prepend16(object_id);
            outgoing.Prepend8(call_id);
            outgoing.Prepend8(8);
            /* Retail passes the address of the owner field, not the owner. */
            theTTNetwork.SendEx(outgoing, 3, (V2Peer *)&object->owner, 0, 0);
            outgoing.Release();
        }
        result = 1;
    }
    message.Release();
    return result;
}

i32 NetworkObjectManager::ObjectOtherCall(void *instance, i32 call_id, NETMESSAGE_PC message) {
    i32 result = 0;
    NetworkObject *object = FindNetworkObject(instance);
    if (object != NULL) {
        if (object->owner->local) {
            i16 class_id = theRegistry.GetClassId(object->object_class);
            i16 object_id = object->id;
            NETMESSAGE_PC outgoing(message);
            outgoing.Prepend16(class_id);
            outgoing.Prepend16(object_id);
            outgoing.Prepend8(call_id);
            outgoing.Prepend8(8);
            theTTNetwork.BroadcastEx(outgoing, 3);
            outgoing.Release();
        }
        result = 1;
    }
    message.Release();
    return result;
}

i32 NetReplicator::smNextId = 1;

NetReplicator::NetReplicator(i32 group, f32 minimum_seconds, f32 maximum_seconds) {
    if (minimum_seconds < 0.1f)
        minimum_seconds = 0.1f;
    if (maximum_seconds > 0.0f && maximum_seconds < 0.1f)
        maximum_seconds = 0.1f;
    id = smNextId++;
    replication_group = group;
    data_size = 0;
    message_size = 0;
    minimum_interval = (u16)(minimum_seconds * 1000.0);
    maximum_interval = (u16)(maximum_seconds * 1000.0);
}

i32 NetReplicator::SerialiseObject(EdStream &stream, V2Peer *peer, const EdClass *object_class, void *object,
                                   ReplicatorData &data, i16 *class_mapping) {
    u8 member_data[256];
    memset(member_data, 0, sizeof(member_data));
    for (EdRef *member = object_class->members; member != NULL; member = member->next) {
        if (member->attributes & 0x80000000) {
            EdClass *member_class = theRegistry.GetClass(member->type_id);
            void *member_object = member->GetMemberObject(object);
            if (ForceDummySerialise && member_object == NULL)
                member_object = object;
            if (member_class->SerialiseObjectHeader(stream, member_object))
                SerialiseObject(stream, peer, member_class, member_object, data, NULL);
        } else if (member->replication_group == id) {
            EdType *type = theRegistry.GetType(member->type_id);
            i32 size = member->size;
            if (size <= 0)
                size = type->size;
            if (object != NULL) {
                if (stream.mode == 2)
                    member->GetMemberData(object, member->type_id, member_data, sizeof(member_data));
                type->serialise(stream, member_data, size);
                if (stream.mode == 1)
                    member->SetMemberData(object, member->type_id, member_data, sizeof(member_data), class_mapping);
            } else {
                stream.Eat(size, 1);
            }
        }
    }
    return 1;
}

i32 NetReplicator::DoPrediction(const EdClass *, void *, ReplicatorData &, i32) {
    return 0;
}

i32 NetConstReplicator::AllowPush(const EdClass *, const void *, ReplicatorData &data, i32 force, i32) {
    u32 *last_push = (u32 *)REPLICATOR_ALIGN(data.cursor);
    data.cursor = (u8 *)(last_push + 1);
    u32 now = UtilGetFrameStartTime();
    i32 allowed = 0;
    if (force != 0 || *last_push == 0) {
        allowed = 1;
        *last_push = now;
    }
    return allowed;
}

i32 NetSimpleReplicator::AllowPush(const EdClass *, const void *, ReplicatorData &data, i32 force, i32) {
    u32 *last_push = (u32 *)REPLICATOR_ALIGN(data.cursor);
    data.cursor = (u8 *)(last_push + 1);
    u32 now = UtilGetFrameStartTime();
    i32 allowed = 0;
    if (force != 0 || now - *last_push > minimum_interval) {
        allowed = 1;
        *last_push = now;
    }
    return allowed;
}

void NetChangedReplicator::CheckSumObject(const EdClass *object_class, const void *object, u32 &checksum) const {
    for (EdRef *member = object_class->members; member != NULL; member = member->next) {
        if (member->attributes & 0x80000000) {
            EdClass *member_class = theRegistry.GetClass(member->type_id);
            void *member_object = member->GetMemberObject((void *)object);
            if (member_object != NULL)
                CheckSumObject(member_class, member_object, checksum);
        } else if (member->replication_group == id) {
            EdType *type = theRegistry.GetType(member->type_id);
            i32 size = member->size;
            if (size <= 0)
                size = type->size;
            u8 member_data[256];
            member->GetMemberData((void *)object, member->type_id, member_data, sizeof(member_data));
            CheckSum(member_data, size, checksum);
        }
    }
}

i32 NetChangedReplicator::AllowPush(const EdClass *object_class, const void *object, ReplicatorData &data, i32 force,
                                    i32 skip_checksum) {
    u32 *last_push = (u32 *)REPLICATOR_ALIGN(data.cursor);
    data.cursor = (u8 *)(last_push + 1);
    u32 *last_checksum = (u32 *)REPLICATOR_ALIGN(data.cursor);
    data.cursor = (u8 *)(last_checksum + 1);
    u32 checksum = 0xffffffff;
    u32 now = UtilGetFrameStartTime();
    u32 elapsed = now - *last_push;
    i32 push = 0;
    if (force != 0 || (maximum_interval > 0 && elapsed > maximum_interval)) {
        push = 1;
    } else if (elapsed > minimum_interval && skip_checksum == 0) {
        CheckSumObject(object_class, object, checksum);
        if (checksum != *last_checksum) {
            *last_checksum = checksum;
            push = 1;
        }
    }
    if (push)
        *last_push = now;
    return push;
}

i32 NetPredictor::AllowPush(const EdClass *object_class, const void *object, ReplicatorData &data, i32 force, i32) {
    u32 *last_push = (u32 *)REPLICATOR_ALIGN(data.cursor);
    data.cursor = (u8 *)(last_push + 1);
    u32 *failed_predictions = (u32 *)REPLICATOR_ALIGN(data.cursor);
    data.cursor = (u8 *)(failed_predictions + 1);
    u32 now = UtilGetFrameStartTime();
    i32 allowed = 0;
    if (force != 0) {
        allowed = 1;
    } else if (now - *last_push > minimum_interval) {
        ReplicatorData prediction_data = data;
        if (DoPrediction(object_class, (void *)object, prediction_data, 1) != 0) {
            *failed_predictions = 0;
            replication_group |= 1;
            allowed = 1;
        } else if (*failed_predictions < 3) {
            ++*failed_predictions;
            replication_group &= ~1;
            allowed = 1;
        }
    }
    if (allowed)
        *last_push = now;
    return allowed;
}

i32 NetPredictor::SerialiseObject(EdStream &stream, V2Peer *peer, const EdClass *object_class, void *object,
                                  ReplicatorData &data, PredictorTime *time, i16 *class_mapping) {
    u8 member_data[256];
    u8 continuity_break = (replication_group >> 3) & 1;
    for (EdRef *member = object_class->members; member != NULL; member = member->next) {
        if (member->attributes & 0x80000000) {
            EdClass *member_class = theRegistry.GetClass(member->type_id);
            void *member_object = member->GetMemberObject(object);
            if (ForceDummySerialise && member_object == NULL)
                member_object = object;
            if (member_class->SerialiseObjectHeader(stream, member_object))
                SerialiseObject(stream, peer, member_class, member_object, data, time, class_mapping);
        } else if (member->replication_group == id) {
            EdType *type = theRegistry.GetType(member->type_id);
            i32 size = member->size;
            if (size <= 0)
                size = type->size;
            if (object != NULL) {
                if (stream.mode == 2)
                    member->GetMemberData(object, member->type_id, member_data, sizeof(member_data));
                type->serialise(stream, member_data, size);
                if (stream.mode == 1 && time->sample_count < 3 && continuity_break == 0)
                    member->SetMemberData(object, member->type_id, member_data, sizeof(member_data), class_mapping);
            } else {
                stream.Eat(size, 1);
            }
            f32 converted[1];
            f32 *values;
            i32 count = 1;
            if (member->type_id == EdType_Float) {
                values = (f32 *)member_data;
            } else if (member->type_id == EdType_VuVec || member->type_id == EdType_NuVec) {
                count = 3;
                values = (f32 *)member_data;
            } else if (member->type_id == EdType_Char) {
                converted[0] = *(i8 *)member_data;
                values = converted;
            } else if (member->type_id == EdType_Short) {
                converted[0] = *(i16 *)member_data;
                values = converted;
            } else if (member->type_id == EdType_Int) {
                converted[0] = *(i32 *)member_data;
                values = converted;
            } else {
                NetTrace();
            }
            PredictorData *samples[3];
            for (i32 i = 0; i < count; i++) {
                samples[i] = (PredictorData *)REPLICATOR_ALIGN(data.cursor);
                data.cursor = (u8 *)(samples[i] + 1);
            }
            StoreSampleData(object_class, object, time, samples, values, count);
        }
    }
    return 1;
}

i32 NetPredictor::DoPrediction(const EdClass *object_class, void *object, ReplicatorData &data, PredictorTime *time,
                               i32 check_only) {
    i32 result = 0;
    u8 member_data[16];
    for (EdRef *member = object_class->members; member != NULL; member = member->next) {
        if (member->attributes & 0x80000000) {
            EdClass *member_class = theRegistry.GetClass(member->type_id);
            void *member_object = member->GetMemberObject(object);
            result |= DoPrediction(member_class, member_object, data, time, check_only);
        } else if (member->replication_group == id) {
            member->GetMemberData(object, member->type_id, member_data, 256);
            f32 converted[1];
            f32 *values;
            i32 count = 1;
            if (member->type_id == EdType_Float) {
                values = (f32 *)member_data;
            } else if (member->type_id == EdType_VuVec || member->type_id == EdType_NuVec) {
                count = 3;
                values = (f32 *)member_data;
            } else if (member->type_id == EdType_Char) {
                converted[0] = *(i8 *)member_data;
                values = converted;
            } else if (member->type_id == EdType_Short) {
                converted[0] = *(i16 *)member_data;
                values = converted;
            } else if (member->type_id == EdType_Int) {
                converted[0] = *(i32 *)member_data;
                values = converted;
            } else {
                NetTrace();
            }
            f32 actual[3];
            PCByteCopy_candidate(actual, values, count * sizeof(f32));
            PredictorData *samples[3];
            for (i32 i = 0; i < count; i++) {
                samples[i] = (PredictorData *)REPLICATOR_ALIGN(data.cursor);
                data.cursor = (u8 *)(samples[i] + 1);
            }
            PredictValue(object_class, object, time, samples, values, count);
            if (check_only != 0) {
                result |= CheckPredictionError(object_class, object, actual, values, count);
            } else {
                if (member->type_id == EdType_Char)
                    *(i8 *)member_data = (i8)converted[0];
                else if (member->type_id == EdType_Short)
                    *(i16 *)member_data = (i16)converted[0];
                else if (member->type_id == EdType_Int)
                    *(i32 *)member_data = (i32)converted[0];
                member->SetMemberData(object, member->type_id, member_data, 256, NULL);
            }
        }
    }
    return result;
}

void NetworkObjectManager::ImportObjects() {
    i32 class_count = theRegistry.class_count;
    for (i32 i = 0; i < class_count; i++) {
        EdClass *object_class = theRegistry.GetClass(i);
        if (object_class == NULL || object_class->object_interface == NULL)
            continue;
        NOSTrace(object_class->name);
        i32 count = 0;
        void *object = object_class->object_interface->GetNextObject(NULL);
        while (object != NULL) {
            i32 guid = object_class->object_interface->GetObjectGuid(object);
            if (guid == 0) {
                guid = GetNextGuid();
                object_class->object_interface->SetObjectGuid(object, guid);
            }
            RegisterObject(object, object_class, guid);
            object = object_class->object_interface->GetNextObject(object);
            count++;
        }
        NOSTrace(count, object_class->name);
    }
}

i32 NetPredictor::SerialiseObject(EdStream &stream, V2Peer *peer, const EdClass *object_class, void *object,
                                  ReplicatorData &data, i16 *class_mapping) {
    PredictorTime *time = (PredictorTime *)REPLICATOR_ALIGN(data.cursor);
    data.cursor = (u8 *)(time + 1);
    u8 continuity_break = (replication_group >> 3) & 1;
    UtilGetFrameStartTime();
    stream.SerialiseBuffer(&continuity_break, 1, 1);
    if (continuity_break != 0)
        time->sample_count = 0;
    time->values[0] = time->values[1];
    time->values[1] = time->values[2];
    if (stream.mode == 2)
        time->values[2] = (f32)UtilGetFrameStartTime();
    stream.SerialiseBuffer(&time->values[2], 4, 1);
    if (stream.mode == 1) {
        UtilGetFrameStartTime();
        time->values[2] = (i32)((u32)time->values[2] + peer->time_offset);
    }
    if (time->values[1] >= time->values[2])
        time->sample_count = 0;
    if (time->sample_count < 3)
        time->sample_count++;
    if (time->sample_count >= 3) {
        time->factors[0] = 1.0f / ((time->values[0] - time->values[2]) * (time->values[0] - time->values[1]));
        time->factors[1] = 1.0f / ((time->values[1] - time->values[0]) * (time->values[1] - time->values[2]));
        time->factors[2] = 1.0f / ((time->values[2] - time->values[0]) * (time->values[2] - time->values[1]));
    }
    SerialiseObject(stream, peer, object_class, object, data, time, class_mapping);
    if (continuity_break != 0)
        replication_group &= ~8;
    return 1;
}

i32 NetPredictor::DoPrediction(const EdClass *object_class, void *object, ReplicatorData &data, i32 check_only) {
    PredictorTime *time = (PredictorTime *)REPLICATOR_ALIGN(data.cursor);
    data.cursor = (u8 *)(time + 1);
    if (time->sample_count < 3)
        return 1;
    if (check_only == 0 && (time->values[0] >= time->values[1] || time->values[1] >= time->values[2]))
        return 1;
    return DoPrediction(object_class, object, data, time, check_only);
}

void NetworkObjectManager::Receive(NETMESSAGE_PC message, u8, const V2Peer &peer) {
    i32 error = 0;
    do {
        u8 type;
        message.Read8(type);
        switch (type) {
            case 9:
                ReceiveStartMessage(message, peer);
                break;
            case 10:
                ReceiveStopMessage(message, peer);
                break;
            case 11:
                ReceiveStatusMessage(message, peer);
                break;
            case 0:
                if (active && IsPeerReady(peer))
                    message.Read32(guid_group);
                else
                    error = 1;
                break;
            case 1:
                if (active && IsPeerStarted(peer))
                    ReceiveConstructorMessage(message, peer);
                else
                    error = 1;
                break;
            case 2:
                if (active && IsPeerStarted(peer))
                    ReceiveReplicaMessage(message, peer);
                else
                    error = 1;
                break;
            case 3:
                if (active && IsPeerReady(peer))
                    ReceiveAcquireMessage(message, peer);
                else
                    error = 1;
                break;
            case 4:
                if (active && IsPeerReady(peer))
                    ReceiveAcquiredMessage(message, peer);
                else
                    error = 1;
                break;
            case 5:
                if (active && IsPeerReady(peer))
                    ReceiveAdoptedMessage(message, peer);
                else
                    error = 1;
                break;
            case 6:
                if (active && IsPeerReady(peer))
                    ReceiveReleaseMessage(message, peer);
                else
                    error = 1;
                break;
            case 7:
                if (active && IsPeerReady(peer))
                    ReceiveRemoteCallMessage(message, peer);
                else
                    error = 1;
                break;
            case 8:
                if (active && IsPeerReady(peer))
                    ReceiveObjectCallMessage(message, peer);
                else
                    error = 1;
                break;
            case 12:
                if (active && IsPeerReady(peer))
                    ReceiveContinuityBreak(message, peer);
                else
                    error = 1;
                break;
            default:
                error = 1;
                break;
        }
    } while (message.GetSize() > 0 && !error);
    message.Release();
}
