/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

/* saga.h includes this header once per part (SAGA_SHIM), so `#pragma once` holds for direct includes only; the
 * condition and the _PART selector go away with the shim. */
#ifndef SAGA_SHIM
#pragma once
#endif
#ifndef SAGA_LEGOAPI_ITEMS_COLLECT_BOLTS_H_PART
#define SAGA_LEGOAPI_ITEMS_COLLECT_BOLTS_H_PART 1
#endif

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "gameapi/edtools/edfile.h"
#include "gamelib/net/NetMessagePc.h"
#include "gamelib/util/gamelib_util_types.h"
#include "globals.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/world/world.h"
#include "nu2api/numath/nuvec.h"
#endif

/* 5 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_LEGOAPI_ITEMS_COLLECT_BOLTS_H_PART == 1 && !defined(SAGA_LEGOAPI_ITEMS_COLLECT_BOLTS_H_STOP)

struct PART_PC;
struct gameobject_s;

#ifdef __cplusplus
extern "C" {
#endif
    extern f32 BOLT_OVERRIDE_PLAYERBOLTSPEED;
    extern f32 BOLT_OVERRIDE_PLAYERBOLTDURATION;
    void PCByteCopy(void *destination, const void *source, u32 count);
    extern i32 (*Bolt_HitCustomFn)(void *bolt, void *points);
    extern i32 i_bolt;
    void Bolt_Reflect(NUVEC *, NUVEC *, NUVEC *);
    extern f32 BoltShootParameter;
    extern i32 BoltShootIdScratch;
    extern NUVEC BoltShootPositionScratch;
    extern i32 BoltNoSfxPc;
    extern i32 BoltNoObjectMomentumPc;
    extern i32 addbolt_nosfx;
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_ITEMS_COLLECT_BOLTS_H_PART
#define SAGA_LEGOAPI_ITEMS_COLLECT_BOLTS_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_ITEMS_COLLECT_BOLTS_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_ITEMS_COLLECT_BOLTS_H_PART == 2 && !defined(SAGA_LEGOAPI_ITEMS_COLLECT_BOLTS_H_STOP)

typedef BOLT BOLT_PC;
STATIC_ASSERT(sizeof(BOLT_PC) == 0x138);
STATIC_ASSERT(offsetof(BOLT_PC, time) == 0xc8);
STATIC_ASSERT(offsetof(BOLT_PC, speed) == 0xcc);
STATIC_ASSERT(offsetof(BOLT_PC, lifetime) == 0xd0);
STATIC_ASSERT(offsetof(BOLT_PC, ray_time) == 0xdc);
STATIC_ASSERT(offsetof(BOLT_PC, flags) == 0xf0);
STATIC_ASSERT(offsetof(BOLT_PC, active) == 0x100);
STATIC_ASSERT(offsetof(BOLT_PC, index) == 0x103);
STATIC_ASSERT(offsetof(BOLT_PC, bounds_min) == 0x108);
STATIC_ASSERT(offsetof(BOLT_PC, bounds_max) == 0x114);
STATIC_ASSERT(offsetof(BOLT_PC, hit_normal) == 0x120);
#ifdef __cplusplus
extern "C" {
#endif
    /* SpecialMove_KillVictim stays static with private ESI ABI in original caller TU; no public declaration. */
    extern BOLT *objhitobj_bolt;
    extern i32 (*Bolt_HitPlatFn)(BOLT *bolt);
    extern BOLTTYPE_PC GlobalBoltType_Default;
    void Bolt_Debris_Default(BOLT *, NUVEC *, i32, NUVEC *, i32);
    void BoltSys_Init(BOLTSYS_PC *);
    extern BOLT Bolt[32];
    void Bolts_Update(WORLDINFO *);
    void Bolts_Draw(WORLDINFO *);
    BOLT_PC *Bolt_Alloc(void);
    void Bolt_PlayHitSfx(BOLT_PC *);
    i32 Bolt_HitPartMode(BOLT_PC *);
    void Bolt_End(BOLT_PC *, i32);
    void Bolt_AddDeflectedBolt(BOLT_PC *, NUVEC *, NUVEC *, u8 *);
    i32 Bolt_HitGameObjects(BOLT_PC *, NUVEC *, NUVEC *, NUVEC *, f32, u8 *);
    extern BOLTTYPE_PC GlobalBoltType[44];
    extern BOLTSYS_PC BoltSys_LSW;
    extern BOLTSYS_PC BoltSys_Batman;
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_ITEMS_COLLECT_BOLTS_H_PART
#define SAGA_LEGOAPI_ITEMS_COLLECT_BOLTS_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_ITEMS_COLLECT_BOLTS_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_ITEMS_COLLECT_BOLTS_H_PART == 3 && !defined(SAGA_LEGOAPI_ITEMS_COLLECT_BOLTS_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    BOLT_PC *Bolt_Find(i32, NUVEC *, struct gameobject_s *);
    extern i32 (*InitBolt_AddMomentumType)(BOLT *bolt, struct gameobject_s *object, NUVEC *momentum);
    void Bolt_GetShootOrigin_Default(struct gameobject_s *, NUVEC *);
    u16 Bolt_GetShootDirection_Default(struct gameobject_s *, NUVEC *);
    BOLT_PC *FindIncomingBolt(struct gameobject_s *, i32, i32);
    void Bolt_Shoot(struct gameobject_s *, i32, i32);
    struct PART_PC *Bolt_HitParts(BOLT_PC *, NUVEC *, NUVEC *, NUVEC *, f32, i32);
    i32 Bolt_HitGameObject(BOLT_PC *, struct gameobject_s *, NUVEC *, NUVEC *, NUVEC *, f32, u8 *);
    extern i32 (*BoltInitSfxFn)(struct gameobject_s *);
    void GuidedMissile_Deflect(struct PART_PC *part);
    void GuidedMissile_Kill(struct PART_PC *part, i32 reason);
    void GuidedMissile_Move(struct PART_PC *part, f32 time);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_ITEMS_COLLECT_BOLTS_H_PART
#define SAGA_LEGOAPI_ITEMS_COLLECT_BOLTS_H_PART 4
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_ITEMS_COLLECT_BOLTS_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_ITEMS_COLLECT_BOLTS_H_PART == 4 && !defined(SAGA_LEGOAPI_ITEMS_COLLECT_BOLTS_H_STOP)
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

struct NETMESSAGE_PC {
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
    i32 swap_endianness;
    MessageData *data;
    u32 read_offset;
    u32 write_offset;

    NETMESSAGE_PC(u32 offset) {
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
    NETMESSAGE_PC() {
    }

    NETMESSAGE_PC(const NETMESSAGE_PC &other) {
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
    NETMESSAGE_PC &operator=(const NETMESSAGE_PC &other) {
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
        theMemoryManager.FreePool(memory, sizeof(NETMESSAGE_PC));
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

STATIC_ASSERT(sizeof(NETMESSAGE_PC) == 0x10);
STATIC_ASSERT(offsetof(NETMESSAGE_PC, swap_endianness) == 0);
STATIC_ASSERT(offsetof(NETMESSAGE_PC, data) == 4);
STATIC_ASSERT(offsetof(NETMESSAGE_PC, read_offset) == 8);
STATIC_ASSERT(offsetof(NETMESSAGE_PC, write_offset) == 0xc);
STATIC_ASSERT(sizeof(NETMESSAGE_PC::MessageData) == 0x4b4);
STATIC_ASSERT(offsetof(NETMESSAGE_PC::MessageData, bytes) == 0);
STATIC_ASSERT(offsetof(NETMESSAGE_PC::MessageData, references) == 0x4b0);
#ifdef __cplusplus
extern "C" {
#endif
    void Bolt_Init(void *, NETMESSAGE_PC &);
    void Bolt_HitGameObjectRC(NETMESSAGE_PC &);
#ifdef __cplusplus
}
#endif
#endif
#ifdef __cplusplus

class V2Peer {
  public:
    virtual ~V2Peer();
    virtual void Disconnect();
    virtual void Send(NETMESSAGE_PC message);
    virtual void SendEx(NETMESSAGE_PC message, i32 arg0, i32 arg1);
    virtual void unknown_slot_10();
    virtual i32 GetAvailableMessages();

    V2Peer *next;
    V2Peer *prev;
    u8 local;
    u8 unknown_0d[3];
    NetStats stats;
    u32 unknown_230[27];
    i32 time_offset;

    u8 fun_00611a20() const;
    i32 fun_00611a60(i32 time) const;
};
#endif
#undef SAGA_LEGOAPI_ITEMS_COLLECT_BOLTS_H_PART
#define SAGA_LEGOAPI_ITEMS_COLLECT_BOLTS_H_PART 5
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_ITEMS_COLLECT_BOLTS_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_ITEMS_COLLECT_BOLTS_H_PART == 5 && !defined(SAGA_LEGOAPI_ITEMS_COLLECT_BOLTS_H_STOP)
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
#undef SAGA_LEGOAPI_ITEMS_COLLECT_BOLTS_H_PART
#define SAGA_LEGOAPI_ITEMS_COLLECT_BOLTS_H_PART 6
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_ITEMS_COLLECT_BOLTS_H_STOP
#endif
#endif

#undef SAGA_LEGOAPI_ITEMS_COLLECT_BOLTS_H_STOP
