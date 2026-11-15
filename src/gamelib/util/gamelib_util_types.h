#pragma once

#include "nu2api/nucore/common.h"

#include "editor/edlevelall.h"
#include "gameapi/edtools/EdObjectNotifier.h"
#include "gameapi/edtools/edfile.h"
#include "gamelib/net/NetMessagePc.h"
#include "legoapi/items/collect/bolts.h"

struct EdClass;
struct NETMESSAGE;

#ifdef __cplusplus
struct VirtualStackAllocator { // size 0x10
    u8 owns_memory;            // 0x0
    u8 *cursor;                // 0x4
    u8 *end;                   // 0x8
    u8 *base;                  // 0xc
    VirtualStackAllocator();
    VirtualStackAllocator(i32);
    VirtualStackAllocator(void *, u32);
    VirtualStackAllocator(VirtualStackAllocator &, u32);
    ~VirtualStackAllocator();
    void setExternalMemoryPool(void *, u32);
    void fun_00740fa0(VirtualStackAllocator &parent, u32 size);
    i32 fun_00741100() const;
};

#endif
#ifdef __cplusplus
/* PC network session layer (0x60dee0-0x617d50). Provisional names except the
 * RTTI class names (NetStats, NetSmallStats, TTNetwork, V2SessionManager). */
struct NetStatCounters { // size 0x10
    u32 values[4];       // 0x0
    NetStatCounters();
    void Clear();
    NetStatCounters &KeepMax(const NetStatCounters &other);
    NetStatCounters &operator+=(const NetStatCounters &other);
    NetStatCounters &operator-=(const NetStatCounters &other);
};

class NetSmallStats {
  public:
    enum eInfo { INFO_BYTES, INFO_PACKETS };

    NetSmallStats(const char *name);
    virtual void Update();
    virtual void Draw(f32 x, f32 y, f32 width, f32 height, eInfo info);

    const char *name;
    NetStatCounters totals;
};

class NetStats : public NetSmallStats { // size 0x220
  public:
    NetStats(const char *name);
    virtual void Update();
    virtual void Draw(f32 x, f32 y, f32 width, f32 height, eInfo info);

    i32 history_index;           // 0x018
    u32 last_time;               // 0x01c
    NetStatCounters peak;        // 0x020
    NetStatCounters previous;    // 0x030
    NetStatCounters history[30]; // 0x040
};

struct FtpFile;
struct NetworkObject;

class NetListLink {
  public:
    NetListLink();
    NetListLink *GetNext();

    NetListLink *next;
    NetListLink *prev;
};

/* Session-level peer address (4 bytes on PC). */
struct NetAddress {
    NetAddress();

    u32 address;
};

struct ReplicatorData {
    void fun_00611b60(u8 *memory, u32 size);
    i32 fun_00611b80() const;
    void fun_00611b90(u32 alignment);
    template <class T> T *Allocate(u32 alignment);

    u8 *start;
    u8 *end;
    u8 *cursor;
    u32 unknown_0c;
};

class V2SessionManager;

class NetHost;

enum eHostVisibility { HOST_VISIBILITY_PUBLIC, HOST_VISIBILITY_PRIVATE };
#endif

#ifdef __cplusplus

template <typename T, u32 COUNT> struct TPool {
    T items[COUNT];

    ~TPool() {
        for (T *entry = items; entry != items + COUNT; ++entry) {
            if (entry->references > 0) {
                entry->references = 0;
            }
        }
    }

    void fun_0060f370();
};

struct NETMESSAGE { // size 0x10

    struct MessageData {
        u32 fun_00617420() const;

        u8 bytes[0x4b0];
        u32 references;

        MessageData() : references(0) {
        }

        void fun_0053d2e0();
        void fun_0053d5c0();
        void fun_0053d620();
        MessageData *fun_0053d420();
        i32 fun_0053d430() const;
        MessageData *fun_0053d560();
        MessageData *fun_0053d590();
    };

    static TPool<MessageData, 512> sm_poolMessageData;
    i32 swap_endianness; // 0x0
    MessageData *data;   // 0x4
    u32 read_offset;     // 0x8
    u32 write_offset;    // 0xc

    NETMESSAGE(u32 offset) {
        MessageData *entry;
        for (entry = sm_poolMessageData.items; entry != sm_poolMessageData.items + 512; entry++) {
            if (entry->references <= 0) {
                break;
            }
        }
        if (entry != sm_poolMessageData.items + 512) {
            entry->references = 1;
        } else {
            entry = NULL;
        }
        data = entry;
        read_offset = offset;
        write_offset = offset;
        swap_endianness = 1;
    }

    /* Leaves the message unset; FtpFile embeds one and its constructor does
     * not touch it. */
    NETMESSAGE() {
    }

    NETMESSAGE(const NETMESSAGE &other) {
        data = other.data;
        read_offset = other.read_offset;
        write_offset = other.write_offset;
        swap_endianness = other.swap_endianness;
        if (data != NULL) {
            data->references++;
        } else {
            NetMessagePoolError();
        }
    }

    /* Drops this message's reference and shares other's buffer; the swap
     * flag is kept. */
    NETMESSAGE &operator=(const NETMESSAGE &other) {
        Release();
        data = other.data;
        read_offset = other.read_offset;
        write_offset = other.write_offset;
        if (data != NULL) {
            data->references++;
        }
        return *this;
    }

    /* Push messages come from theMemoryManager's pools. */
    static void *operator new(size_t size) {
        return theMemoryManager.AllocPool(size, 1);
    }

    static void operator delete(void *memory) {
        theMemoryManager.FreePool(memory, sizeof(NETMESSAGE));
    }

    /* Standalone copy at 0x53d640 (likely the destructor in retail). */
    void Release() {
        if (data != NULL) {
            u32 references = data->references;
            if (references > 1) {
                data->references = references - 1;
            } else {
                data->references = 0;
            }
        }
    }

    i32 GetSize() {
        if (data == NULL) {
            return 0;
        }
        return write_offset - read_offset;
    }

    void MakeUnique();
    void fun_0060ea60(const u8 &value);
    u32 fun_006174c0() const;
    void fun_00611a00(u32 size);
    void fun_00611a10(u32 size);
    void fun_00613090(const u8 &value);

    void Write8(const u8 &value) {
        if (data != NULL) {
            PCByteCopy(data->bytes + write_offset, &value, 1);
            write_offset++;
        }
    }

    void Write16(const i16 &value) {
        if (data != NULL) {
            PCByteCopy(data->bytes + write_offset, &value, 2);
            if (swap_endianness) {
                EdFileSwapEndianess16(data->bytes + write_offset);
            }
            write_offset += 2;
        }
    }

    void Write16(const u16 &value) {
        if (data != NULL) {
            PCByteCopy(data->bytes + write_offset, &value, 2);
            if (swap_endianness) {
                EdFileSwapEndianess16(data->bytes + write_offset);
            }
            write_offset += 2;
        }
    }

    void Write32(const i32 &value) {
        if (data != NULL) {
            PCByteCopy(data->bytes + write_offset, &value, 4);
            if (swap_endianness) {
                EdFileSwapEndianess32(data->bytes + write_offset);
            }
            write_offset += 4;
        }
    }

    void Write(const void *value, u32 size) {
        if (data != NULL) {
            PCByteCopy(data->bytes + write_offset, value, size);
            write_offset += size;
        }
    }

    /* Writes count elements of size bytes, swapping each element's byte order. */
    void Write(const void *values, i32 size, i32 count) {
        if (data != NULL) {
            i32 bytes = size * count;
            PCByteCopy(data->bytes + write_offset, values, bytes);
            if (swap_endianness && size > 1) {
                u8 *element = data->bytes + write_offset;
                for (i32 i = 0; i < count; i++) {
                    switch (size) {
                        case 2:
                            EdFileSwapEndianess16(element);
                            break;
                        case 4:
                            EdFileSwapEndianess32(element);
                            break;
                    }
                    element += size;
                }
            }
            write_offset += bytes;
        }
    }

    void Read(void *values, i32 size, i32 count) {
        if (data != NULL) {
            i32 bytes = size * count;
            PCByteCopy(values, data->bytes + read_offset, bytes);
            if (swap_endianness && size > 1) {
                u8 *element = (u8 *)values;
                for (i32 i = 0; i < count; i++) {
                    switch (size) {
                        case 2:
                            EdFileSwapEndianess16(element);
                            break;
                        case 4:
                            EdFileSwapEndianess32(element);
                            break;
                    }
                    element += size;
                }
            }
            read_offset += bytes;
        }
    }

    void Read(void *value, u32 size) {
        if (data != NULL) {
            PCByteCopy(value, data->bytes + read_offset, size);
            read_offset += size;
        }
    }

    /* Prepend writes go in front of the unread data (used to add headers). */
    void Prepend8(u8 value) {
        if (data != NULL) {
            read_offset--;
            PCByteCopy(data->bytes + read_offset, &value, 1);
        }
    }

    void Prepend16(const i16 &value) {
        if (data != NULL) {
            read_offset -= 2;
            PCByteCopy(data->bytes + read_offset, &value, 2);
            if (swap_endianness) {
                EdFileSwapEndianess16(data->bytes + read_offset);
            }
        }
    }

    void Read8(u8 &value) {
        if (data != NULL) {
            PCByteCopy(&value, data->bytes + read_offset, 1);
            read_offset++;
        }
    }

    void Read16(i16 &value) {
        if (data != NULL) {
            PCByteCopy(&value, data->bytes + read_offset, 2);
            if (swap_endianness) {
                EdFileSwapEndianess16(&value);
            }
            read_offset += 2;
        }
    }

    void Read16(u16 &value) {
        if (data != NULL) {
            PCByteCopy(&value, data->bytes + read_offset, 2);
            if (swap_endianness) {
                EdFileSwapEndianess16(&value);
            }
            read_offset += 2;
        }
    }

    void Read32(i32 &value) {
        if (data != NULL) {
            PCByteCopy(&value, data->bytes + read_offset, 4);
            if (swap_endianness) {
                EdFileSwapEndianess32(&value);
            }
            read_offset += 4;
        }
    }

    void fun_0053d330(i8 &value);
    void fun_0053d360(i16 &value);
    void fun_0053d4a0(char *text);
    void fun_0053d4e0(char *text, i32 size);
    void fun_00613140(i16 &value);
    void fun_00613180(i8 &value);
    void fun_006131b0(u32 &value);
    i32 fun_00613230();
};

#ifdef __cplusplus
extern "C" {
#endif
    void Bolt_Init(void *, NETMESSAGE &);
    void Bolt_HitGameObjectRC(NETMESSAGE &);
#ifdef __cplusplus
}
#endif
#endif
#ifdef __cplusplus

class V2Peer { // size 0x2a0
  public:
    virtual ~V2Peer();
    virtual void Disconnect();
    virtual void Send(NETMESSAGE message);
    virtual void SendEx(NETMESSAGE message, i32 arg0, i32 arg1);
    virtual void unknown_slot_10();
    virtual i32 GetAvailableMessages();

    V2Peer *next;        // 0x004
    V2Peer *prev;        // 0x008
    u8 local;            // 0x00c
    u8 unknown_0d[3];    // 0x00d
    NetStats stats;      // 0x010
    u32 unknown_230[27]; // 0x230
    i32 time_offset;     // 0x29c

    u8 fun_00611a20() const;
    i32 fun_00611a60(i32 time) const;
};
#endif

#ifdef __cplusplus

class V2HostList {
  public:
    V2HostList();
    i32 GetCount();

    V2Peer *head;
    V2Peer *tail;
    i32 count;
};

class V2PeerList {
  public:
    V2PeerList();
    void Remove(V2Peer *peer);
    i32 GetCount();

    V2Peer *head;
    V2Peer *tail;
    i32 count;
};

/* Listener interface registered with a NetTransporter. The
 * default bodies are emitted at 0x53d1b0-0x53d240 and 0x53d670. */
class NetListenerInterface {
  public:
    virtual ~NetListenerInterface() {
    }

    virtual void Receive(NETMESSAGE message, u8 channel, const V2Peer &peer) {
        message.Release();
    }

    virtual i32 PeerRequest(const V2Peer &peer) {
        return 1;
    }

    virtual void PeerJoined(const V2Peer &peer) {
    }

    virtual void PeerLeft(const V2Peer &peer, i32 reason) {
    }

    virtual void PeerDead(const V2Peer &peer) {
    }

    virtual void FtpUpload(FtpFile *file) {
    }

    virtual void FtpDownload(FtpFile *file) {
    }

    virtual void FtpComplete(FtpFile *file, i32 result) {
    }

    virtual i32 NosAcquire(NetworkObject *object, const V2Peer &peer) {
        return 1;
    }

    virtual void NosAdopted(NetworkObject *object, const V2Peer &peer) {
    }
};

/* 0x250-byte pool-allocated entry registered with a NetTransporter. */
class NetTransportEntry : public NetListLink { // size 0x250
  public:
    NetTransportEntry(NetListenerInterface *client, u8 channel, const char *name);
    static void *operator new(u32 size);
    static void operator delete(void *memory);
    NetTransportEntry &operator=(const NetTransportEntry &other);
    i32 operator==(const NetTransportEntry &other);

    NetListenerInterface *client; // 0x008
    u8 channel;                   // 0x00c
    char name[0x21];              // 0x00d
    NetStats stats;               // 0x030
};

class NetTransportList {
  public:
    NetTransportList() : head(NULL), tail(NULL), count(0) {
    }

    NetListLink *GetHead();
    void Insert(NetTransportEntry *entry, NetTransportEntry *after);
    void Remove(NetTransportEntry *entry);
    NetTransportEntry *Find(const NetTransportEntry &key);

    NetListLink *head;
    NetListLink *tail;
    i32 count;
};

class NetTransporterInterface {
  public:
    virtual void Send(NETMESSAGE message, u8 channel, V2Peer *peer) = 0;
    virtual void SendEx(NETMESSAGE message, u8 channel, V2Peer *peer, i32 arg0, i32 arg1) = 0;
    virtual void Broadcast(NETMESSAGE message, u8 channel) = 0;
    virtual void BroadcastEx(NETMESSAGE message, u8 channel) = 0;
    virtual void AddListener(NetListenerInterface *client, u8 channel, const char *name) = 0;
    virtual void RemoveListener(NetListenerInterface *client, u8 channel) = 0;

    virtual ~NetTransporterInterface() {
    }
};

class NetTransporter : public NetTransporterInterface { // size 0x10
  public:
    virtual void AddListener(NetListenerInterface *client, u8 channel, const char *name);
    virtual void RemoveListener(NetListenerInterface *client, u8 channel);

    NetTransportList clients; // 0x4
};

/* One file transfer slot of the NetFtpManager (0xb0 bytes). */
struct FtpFile { // size 0xb0

    FtpFile() {
        transfer = 0;
    }

    bool InUse() const {
        return transfer > 0;
    }

    void Close() {
        if (InUse() && message.data != NULL) {
            NETMESSAGE::MessageData *data = message.data;
            if (data->references > 1) {
                data->references--;
            } else {
                data->references = 0;
            }
        }
        transfer = 0;
    }

    i32 CompareAddress(const NetAddress &other) const;
    i32 fun_00617210(const NetAddress &other) const;
    const u32 *fun_00617230() const;
    i32 fun_00617240() const;
    char *fun_00617250();
    i32 fun_00617260() const;
    void RecvData(struct NETMESSAGE &message);
    void fun_006176d0();
    void Init(i32 new_direction, const char *new_name, i32 new_size, const NetAddress &new_address, void *new_buffer, i32 new_capacity);
    i32 Accept(i32 new_capacity, void *new_buffer);
    i32 Accept(i32 new_size);
    i32 Accept();
    void Term();
    void Update();

    u32 address;        // 0x00
    i32 direction;      // 0x04
    i32 accepted;       // 0x08
    char name[0x80];    // 0x0c
    i32 size;           // 0x8c
    void *buffer;       // 0x90
    i32 capacity;       // 0x94
    i32 transferred;    // 0x98
    NETMESSAGE message; // 0x9c
    u32 transfer;       // 0xac
};

/* The manager's 32 transfer slots. Retail destroys them through a separate
 * destructor (0x617790) that closes every slot still in use. */
struct FtpFileArray {
    ~FtpFileArray() {
        for (FtpFile *file = items; file != items + 32; ++file) {
            if (file->InUse()) {
                file->Close();
            }
        }
    }

    FtpFile *fun_00617480();
    const FtpFile *fun_00617490() const;

    FtpFile items[32];
};

/* File transfer listener on channel 2 (TTNetwork+0x2178). */
class NetFtpManager : public NetListenerInterface { // size 0x1608
  public:
    NetFtpManager();
    ~NetFtpManager();
    virtual void Receive(NETMESSAGE message, u8 channel, const NetAddress &address);
    virtual void PeerLeft(const NetAddress &address, i32 reason);
    void Init();
    i32 Send(const char *name, const void *data, i32 size, const NetAddress &address);
    i32 Get(const char *name, void *buffer, i32 capacity, const NetAddress &address);
    i32 Abort(const char *name, const NetAddress &address, i32 direction, i32 reason);
    void Reset();
    void Term();
    void Update();
    FtpFile *FindTransfer(const char *name, const NetAddress &address, i32 direction);
    const FtpFile *FindTransfer(const char *name, const NetAddress &address, i32 direction) const;
    const FtpFile *GetTransfers() const;
    void fun_0060e6f0(i32 value);

    FtpFileArray files; // 0x0004
    i32 unknown_1604;   // 0x1604
};

struct NOSContext {
    NOSContext() {
        memset(bytes, 0, sizeof(bytes));
    }

    NOSContext &operator=(const NOSContext &other);
    bool operator==(const NOSContext &other) const;

    u8 bytes[0x10];
};

class NOSFilter {
  public:
    virtual i32 AllowPush(const struct EdClass *object_class, void *object) = 0;
};

/* Replicates one group of an object's members. */
class NetReplicator : public CSListLink<NetReplicator> { // size 0x18
  public:
    NetReplicator(i32 group, f32 minimum_seconds, f32 maximum_seconds);
    virtual i32 AllowPush(const struct EdClass *object_class, const void *object, ReplicatorData &data, i32 force, i32 check_only) = 0;
    virtual i32 SerialiseObject(EdStream &stream, V2Peer *peer, const struct EdClass *object_class, void *object, ReplicatorData &data,
                                i16 *flags);
    virtual i32 DoPrediction(const struct EdClass *object_class, void *object, ReplicatorData &data, i32 check_only);
    i32 fun_00611bb0() const;
    i32 fun_00611bc0(i32 groups) const;
    i32 fun_00611be0(u16 groups);
    i32 fun_00611c00(u16 groups);
    i32 fun_00611c20() const;
    void fun_00611c30(u16 size);
    i32 fun_00611c40() const;
    void fun_00611c50(u16 size);

    u16 id;                // 0x0c
    u16 replication_group; // 0x0e
    u16 minimum_interval;  // 0x10
    u16 maximum_interval;  // 0x12
    u16 data_size;         // 0x14
    u16 message_size;      // 0x16
    static i32 smNextId;
};

/* One replicated object slot, indexed by guid. */
struct NetworkObject { // size 0x18
    void Initialise(i32 guid, void *new_object, struct EdClass *new_class, const V2Peer &new_owner, i32 new_flags);
    void Destroy();

    i16 flags;                    // 0x00
    i16 id;                       // 0x02
    u32 unknown_04;               // 0x04
    const V2Peer *owner;          // 0x08
    void *object;                 // 0x0c
    struct EdClass *object_class; // 0x10
    void *replicator_data;        // 0x14
};

/* Network object system (listener channel 3, TTNetwork+0x3780). */
class NetworkObjectManager : public NetListenerInterface, public EdObjectNotifier { // size 0xd970
  public:
    struct NetPeerPush { // size 0x14
        void FlushMessages();
#pragma push_macro("GetMessage")
#undef GetMessage
        struct NETMESSAGE *GetMessage(i32 size);
#pragma pop_macro("GetMessage")
        struct NETMESSAGE *GetReliableMessage(i32 size);
        void NextStage();
        void Stop();
        void Sync();
        i32 IsReady() const;
        i32 IsSyncing() const;
        i32 GetStage() const;
        i32 GetNextObject() const;
        void SetNextObject(i32 index);

        const V2Peer *peer;                  // 0xd838
        struct NETMESSAGE *message;          // 0xd83c
        struct NETMESSAGE *reliable_message; // 0xd840
        i32 stage;                           // 0xd844
        i32 next_object;                     // 0xd848
    };

    struct PendingObject {     // size 0xc
        i32 requested;         // 0xd034
        u32 request_time;      // 0xd038
        NetworkObject *object; // 0xd03c
    };

    struct ReplicatorList { // size 0xc

        ReplicatorList() : head(NULL), tail(NULL), count(0) {
        }

        void fun_00612f70(NetReplicator *replicator, NetReplicator *after);

        NetReplicator *head; // 0xd1b4
        NetReplicator *tail; // 0xd1b8
        i32 count;           // 0xd1bc
    };

    struct RegisteredCall { // size 0xc
        i32 type;           // 0xd6b4
        i32 flags;          // 0xd6b8
        void *callback;     // 0xd6bc
    };

    NetworkObjectManager();
    ~NetworkObjectManager();
    virtual void Receive(NETMESSAGE message, u8 channel, const V2Peer &peer);
    virtual void PeerJoined(const V2Peer &peer);
    virtual void PeerLeft(const V2Peer &peer, i32 reason);
    virtual void NotifyCreateObject(void *object, struct EdClass *object_class, void *data, i32 size, i32 guid, i32 flags);
    virtual void NotifyDestroyObject(void *object, struct EdClass *object_class, i32 guid, i32 flags);

    i32 Acquire(i32 id);
    void AddToLocalObjectList(NetworkObject *object);
    void BindFilter(NOSFilter *filter, const struct EdClass *object_class);
    void BindReplicator(NetReplicator *replicator, const struct EdClass *object_class);
    void CalcReplicatorDataSize(NetReplicator *replicator, const struct EdClass *object_class, i32 &data_size, i32 &message_size);
    void ChangeContext(NOSContext &new_context);
    void ConstructObject(NetworkObject *object, NetPeerPush *push);
    void ContinuityBreak(i32 id, f32 time);
    NetworkObject *FindNetworkObject(i32 id);
    NetworkObject *FindNetworkObject(void *object);
    PendingObject *FindPendingObject(NetworkObject *object);
    i32 GetGuid(void *object);
    i32 GetNextGuid();
#pragma push_macro("GetObject")
#undef GetObject
    void *GetObject(i32 id);
#pragma pop_macro("GetObject")
    i32 GetPeerStatus();
    void FlushObjects(i32 flush_level_editor);
    void ImportObjects();
    void Init();
    void InitClassStats();
    i32 IsLocal(i32 id);
    i32 IsPeerReady(const V2Peer &peer) const;
    i32 IsPeerStarted(const V2Peer &peer) const;
    const V2Peer *Owner(i32 id);
    void ReceiveAcquireMessage(struct NETMESSAGE &message, const V2Peer &peer);
    void ReceiveAcquiredMessage(struct NETMESSAGE &message, const V2Peer &peer);
    void ReceiveAdoptedMessage(struct NETMESSAGE &message, const V2Peer &peer);
    void ReceiveConstructorMessage(struct NETMESSAGE &message, const V2Peer &peer);
    void ReceiveContinuityBreak(struct NETMESSAGE &message, const V2Peer &peer);
    void ReceiveObjectCallMessage(struct NETMESSAGE &message, const V2Peer &peer);
    void ReceiveReleaseMessage(struct NETMESSAGE &message, const V2Peer &peer);
    void ReceiveReplicaMessage(struct NETMESSAGE &message, const V2Peer &peer);
    void ReceiveRemoteCallMessage(struct NETMESSAGE &message, const V2Peer &peer);
    void ReceiveStartMessage(struct NETMESSAGE &message, const V2Peer &peer);
    void ReceiveStatusMessage(struct NETMESSAGE &message, const V2Peer &peer);
    void ReceiveStopMessage(struct NETMESSAGE &message, const V2Peer &peer);
    void Recover(NetworkObject *object);
    i32 RegisterObject(void *object, struct EdClass *object_class, i32 guid);
    i32 RegisterObjectCall(void (*callback)(void *, struct NETMESSAGE &), i32 flags);
    i32 RegisterRemoteCall(void (*callback)(struct NETMESSAGE &), i32 flags);
    i32 RemoteCall(i32 call_id, NETMESSAGE message, const V2Peer *peer);
    i32 ReleaseObject(void *object, struct EdClass *object_class, i32 guid);
    void RemoveFromLocalObjectList(NetworkObject *object);
    void RemovePendingObject(NetworkObject *object);
    void Reset();
    void SendAcquireMessage(NetworkObject *object);
    void SendAcquiredMessage(i16 id, const V2Peer &peer);
    void SendAdoptedMessage(i16 id);
    i32 ObjectCall(void *instance, i32 call_id, NETMESSAGE message, const V2Peer *peer);
    i32 ObjectOtherCall(void *instance, i32 call_id, NETMESSAGE message);
    i32 ObjectOwnerCall(void *instance, i32 call_id, NETMESSAGE message);
    i32 Push(const NetworkObject *object, NetReplicator *replicator, ReplicatorData &data, NetPeerPush *push);
    i32 PushObject(NetworkObject *object, NetPeerPush *push, i32 force);
    i32 SendPushMessage(struct NETMESSAGE *message, const NetPeerPush *push, i32 flags);
    void Start(const NOSContext &new_context);
    PendingObject *StealPendingObject();
    void Stop();
    void Term();
    void Update();
    void UpdateLocalObjectList();
    NetStats *fun_0060e740(i32 class_id);

    i32 unknown_08;                      // 0x0008
    i32 active;                          // 0x000c
    NOSContext context;                  // 0x0010
    const V2Peer *guid_peers[2];         // 0x0020
    i32 guid_group;                      // 0x0028
    i32 next_guid;                       // 0x002c
    NetworkObject objects[2048];         // 0x0030
    i32 local_object_count;              // 0xc030
    NetworkObject *local_objects[1024];  // 0xc034
    PendingObject pending_objects[32];   // 0xd034
    ReplicatorList replicators[64];      // 0xd1b4
    i32 replicator_data_sizes[64];       // 0xd4b4
    NOSFilter *filters[64];              // 0xd5b4
    RegisteredCall registered_calls[32]; // 0xd6b4
    i32 registered_call_count;           // 0xd834
    NetPeerPush peer_push[8];            // 0xd838
    NetPeerPush default_push;            // 0xd8d8
    NetStats *class_stats[32];           // 0xd8ec
    u8 allow_duplicates;                 // 0xd96c
};

extern NetworkObjectManager *theNos;
#endif

#ifdef __cplusplus

class TTNetwork : public NetTransporter, public BaseThing { // size 0x110f4
  public:
    TTNetwork();
    ~TTNetwork();
    virtual void ProcessEvenWhenPaused(struct ThingProcessData *data);
    virtual void Display(ThingRenderData *data);
    void Start();
    void Update();
    void Shutdown();
    virtual void Send(NETMESSAGE message, u8 channel, V2Peer *peer);
    virtual void SendEx(NETMESSAGE message, u8 channel, V2Peer *peer, i32 arg0, i32 arg1);
    virtual void Broadcast(NETMESSAGE message, u8 channel);
    virtual void BroadcastEx(NETMESSAGE message, u8 channel);
    virtual char const *GetName();
    i32 PeerRequest(const V2Peer &peer);
    void PeerJoined(const V2Peer &peer);
    void PeerLeft(const V2Peer &peer, i32 reason);
    void PeerDead(const V2Peer &peer);
    void FtpUpload(struct FtpFile *file);
    void FtpDownload(struct FtpFile *file);
    void FtpComplete(struct FtpFile *file, i32 result);
    i32 NosAcquire(struct NetworkObject *object, const V2Peer &peer);
    void NosAdopted(struct NetworkObject *object, const V2Peer &peer);
    void UpdateClientStats();
    void DeliverMessage(struct NETMESSAGE &message, u8 channel, const V2Peer &peer);
    void RecordSent(NETMESSAGE message, u8 channel);
    void RecordReceived(NETMESSAGE message, u8 channel);
    void Resume();
    i32 Suspend();
    struct NetAddress *GetMyAddress();
    struct NetAddress *GetMyHostAddress();
    void SetMyHostAddress(struct NetAddress *address);
    void ClearMyHostAddress();
    i32 fun_00611dc0() const;

    NetLogFile log;                    // 0x00020
    u32 frame_count;                   // 0x02134
    i32 unknown_2138;                  // 0x02138
    i32 unknown_213c;                  // 0x0213c
    i32 state;                         // 0x02140
    i32 unknown_2144;                  // 0x02144
    i32 unknown_2148;                  // 0x02148
    i32 unknown_214c;                  // 0x0214c
    i32 display_page;                  // 0x02150
    f32 display_x;                     // 0x02154
    f32 display_y;                     // 0x02158
    f32 display_width;                 // 0x0215c
    f32 display_height;                // 0x02160
    u32 unknown_2164[2];               // 0x02164
    NetAddress my_address;             // 0x0216c
    NetAddress my_host_address;        // 0x02170
    i32 has_my_host_address;           // 0x02174
    NetFtpManager ftp;                 // 0x02178
    NetworkObjectManager nos;          // 0x03780
    V2SessionManager *session_manager; // 0x110f0
};
#endif

#ifdef __cplusplus

/* Session base class. Retail never constructs
 * it: the vtable is referenced only by the constructor. */
class V2SessionManager { // size 0x2c0
  public:
    V2SessionManager(const char *name);

    virtual void Init(char *address) {
    }

    virtual void Start() {
    }

    virtual void Update();

    virtual void Leave() {
    }

    virtual void Close() {
    }

    virtual i32 SignIn() {
        return 0;
    }

    virtual void RemovePeer(V2Peer *peer, i32 reason);

    virtual void Render() {
    }

    virtual bool IsChatRestricted() {
        return false;
    }

    virtual bool IsAgeRestricted() {
        return false;
    }

    virtual void GetLocalAddress(NetAddress *address) {
    }

    virtual bool IsSearchingForHosts() {
        return false;
    }

    virtual void Join(NetHost *host) {
    }

    virtual void Host(eHostVisibility visibility) {
    }

    virtual void SearchForHosts(i32 count, i32 *data) {
    }

    virtual void SetHostVisibility(eHostVisibility visibility) {
        host_visibility = visibility;
    }

    virtual void SetNumLocalPlayers(i32 count) {
        num_local_players = count;
    }

    virtual void RemoveRemotePlayer(V2Peer &peer) {
    }

    virtual void AcceptInvitation() {
    }

    virtual void SendInvitation(char *name) {
    }

    virtual void ViewInvitations() {
    }

    virtual i32 VerifyStrings(u8 **source, u8 **destination, i32 count, u8 *replacement);

    static void Log(const char *format, ...);
    void SetHostGameData(const i32 *data, i32 count);
    void Reset();
    void RemoveAllPeers(i32 reason);

    i32 state;             // 0x004
    u32 unknown_08[3];     // 0x008
    char name[0x20];       // 0x014
    i32 error;             // 0x034
    i32 host_visibility;   // 0x038
    i32 num_local_players; // 0x03c
    i32 slot_mode;         // 0x040
    i32 local_slot;        // 0x044
    i32 player_slots[8];   // 0x048
    f32 timer_68;          // 0x068
    f32 timer_6c;          // 0x06c
    u32 unknown_70;        // 0x070
    f32 timer_74;          // 0x074
    u8 unknown_78;         // 0x078
    u8 unknown_79[3];      // 0x079
    i32 unknown_7c;        // 0x07c
    u32 unknown_80;        // 0x080
    V2Peer *local_peer;    // 0x084
    V2HostList host_list;  // 0x088
    V2PeerList peer_list;  // 0x094
    NetStats stats;        // 0x0a0

    static V2SessionManager *mpSessionManager;
};
#endif

#ifdef __cplusplus

extern TTNetwork theTTNetwork;

/* Platform session layer behind theNetSessionManager. It adds the raw datagram
 * slots 22/23 that nunetemu calls; retail links no implementation, so neither
 * its vtable nor its RTTI is emitted. */
class NetSessionManager : public V2SessionManager {
  public:
    virtual i32 SendTo(void *data, i32 size, u32 *address) = 0;
    virtual i32 RecvFrom(void *data, i32 size, u32 *address) = 0;
};

extern NetSessionManager *theNetSessionManager;
#endif
