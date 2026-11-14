#include "saga.h"

/* AI script network replication. AINet is abstract: the game derives from it
 * to map AI packets to network ids. No class derives from it, so
 * AINet::sm_instance stays NULL and replication is a no-op. */
extern "C" {
    extern AIACTIONDEF_PC api_aiactiondefs[];
    extern AIACTIONDEF_PC *game_aiactiondefs;
    void AINetTrace(const char *format, ...);
}

typedef i32 (*AINETFILTERFN)(void);
AINETFILTERFN AINetSendFilter = 0;
AINETFILTERFN AINetReceiveFilter = 0;

class AINet : NetListenerInterface {
  public:
    AINet();
    virtual ~AINet();
    static void operator delete(void *memory);
    virtual void Receive(NETMESSAGE_PC message, u8 channel, const V2Peer &peer);
    virtual AISYS_PC *GetSystem() = 0;
    virtual u16 GetPacketId(AIPACKET_PC *packet) = 0;
    virtual AIPACKET_PC *GetPacket(i32 id) = 0;
    void Register();
    void ReplicateAction(AIACTION_PC *action, AIPACKET_PC *packet);

    static AINet *sm_instance;
}; // size 0x4

AINet *AINet::sm_instance = NULL;

AINet::AINet() {
    sm_instance = this;
}

void AINet::operator delete(void *memory) {
    theMemoryManager.FreePool(memory, sizeof(AINet));
}

AINet::~AINet() {
    sm_instance = NULL;
}

void AINet::Register() {
    theTTNetwork.AddListener(this, 10, "AINet");
}

void NETMESSAGE_PC::MessageData::fun_0053d2e0() {
    references++;
}

void NETMESSAGE_PC::fun_0053d330(i8 &value) {
    if (data != NULL) {
        PCByteCopy(&value, data->bytes + read_offset, 1);
        read_offset++;
    }
}

void NETMESSAGE_PC::fun_0053d360(i16 &value) {
    if (data != NULL) {
        PCByteCopy(&value, data->bytes + read_offset, 2);
        if (swap_endianness) {
            EdFileSwapEndianess16(&value);
        }
        read_offset += 2;
    }
}

NETMESSAGE_PC::MessageData *NETMESSAGE_PC::MessageData::fun_0053d420() {
    return this + 512;
}

i32 NETMESSAGE_PC::MessageData::fun_0053d430() const {
    return references > 0;
}

extern "C" void fun_0053d450(void) {
}

/* Writes the string with a leading length byte. */
void NETMESSAGE_PC::fun_0053d4a0(char *text) {
    if (data != NULL) {
        i32 length = NuStrLen(text);
        u8 *destination = data->bytes + write_offset;
        *destination++ = (u8)length;
        PCByteCopy(destination, text, length);
        write_offset += length + 1;
    }
}

/* Reads a string written by fun_0053d4a0 into a cleared buffer. */
void NETMESSAGE_PC::fun_0053d4e0(char *text, i32 size) {
    if (data != NULL) {
        i32 length = (i8)data->bytes[read_offset];
        read_offset++;
        memset(text, 0, size);
        PCByteCopy(text, data->bytes + read_offset, length);
        read_offset += length;
    }
}

NETMESSAGE_PC::MessageData *NETMESSAGE_PC::MessageData::fun_0053d560() {
    references = 1;
    return this;
}

/* Claims the first free entry of the pool starting at this one. */
NETMESSAGE_PC::MessageData *NETMESSAGE_PC::MessageData::fun_0053d590() {
    for (MessageData *entry = this; entry != this + 512; entry++) {
        if (entry->references <= 0) {
            entry->references = 1;
            return entry;
        }
    }
    return NULL;
}

void NETMESSAGE_PC::MessageData::fun_0053d5c0() {
    references = 0;
}

void NETMESSAGE_PC::MessageData::fun_0053d620() {
    u32 count = references;
    if (count > 1) {
        references = count - 1;
    } else {
        references = 0;
    }
}

void NETMESSAGE_PC::fun_00613140(i16 &value) {
    if (data != NULL) {
        PCByteCopy(&value, data->bytes + read_offset, 2);
        if (swap_endianness) {
            EdFileSwapEndianess16(&value);
        }
        read_offset += 2;
    }
}

void NETMESSAGE_PC::fun_00613180(i8 &value) {
    if (data != NULL) {
        PCByteCopy(&value, data->bytes + read_offset, 1);
        read_offset++;
    }
}

void NETMESSAGE_PC::fun_006131b0(u32 &value) {
    if (data != NULL) {
        PCByteCopy(&value, data->bytes + read_offset, 4);
        if (swap_endianness) {
            EdFileSwapEndianess32(&value);
        }
        read_offset += 4;
    }
}

/* Bytes left in the message buffer after the written data. */
i32 NETMESSAGE_PC::fun_00613230() {
    if (data == NULL) {
        return 0;
    }
    return 0x4af - write_offset;
}

void AINet::Receive(NETMESSAGE_PC message, u8 channel, const V2Peer &peer) {
    char param_text[16][256];
    char *params[16];
    u8 type;
    u8 is_game_action;
    u8 param_count;
    i16 index;
    u16 id;
    AISYS_PC *system;
    AIPACKET_PC *packet;
    AIACTIONDEF_PC *definitions;

    if (AINetReceiveFilter != NULL && AINetReceiveFilter() == 0) {
        message.Release();
        return;
    }
    do {
        message.Read8(type);
        switch (type) {
            case 1:
                memset(param_text, 0, sizeof(param_text));
                memset(params, 0, sizeof(params));
                message.Read8(is_game_action);
                message.Read16(index);
                message.Read16(id);
                message.Read8(param_count);
                system = GetSystem();
                packet = GetPacket(id);
                if (param_count < 16) {
                    for (i32 i = 0; i < param_count; i++) {
                        if (message.data != NULL) {
                            i32 length = (i8)message.data->bytes[message.read_offset];
                            message.read_offset++;
                            memset(param_text[i], 0, sizeof(param_text[i]));
                            PCByteCopy(param_text[i], message.data->bytes + message.read_offset, length);
                            message.read_offset += length;
                        }
                        params[i] = param_text[i];
                    }
                    definitions = is_game_action != 0 ? game_aiactiondefs : api_aiactiondefs;
                    if (system != NULL &&
                        definitions[(u16)index].eval_fn(system, NULL, packet, params, param_count, 1, 0.0f) != 1) {
                        AINetTrace(definitions->name);
                    }
                }
                break;
        }
    } while (message.GetSize() > 0);
    message.Release();
}

void AINet::ReplicateAction(AIACTION_PC *action, AIPACKET_PC *packet) {
    NETMESSAGE_PC message(0x20);
    i16 id;
    i16 index;
    u8 is_game_action;
    u8 param_count;

    if (AINetSendFilter != NULL && AINetSendFilter() == 0) {
        message.Release();
        return;
    }
    id = GetPacketId(packet);
    index = action->definition->index;
    is_game_action = action->definition->is_game_action;
    param_count = (u8)action->param_count;
    message.Write8(1);
    message.Write8(is_game_action);
    message.Write16(index);
    message.Write16(id);
    message.Write8(param_count);
    for (i32 i = 0; i < action->param_count; i++) {
        char *param = action->params[i];
        if (message.data != NULL) {
            i32 length = NuStrLen(param);
            u8 *destination = message.data->bytes + message.write_offset;
            *destination++ = (u8)length;
            PCByteCopy(destination, param, length);
            message.write_offset += length + 1;
        }
    }
    theTTNetwork.BroadcastEx(message, 10);
    message.Release();
}

extern "C" void AINetReplicateAction(AIACTION_PC *action, AIPACKET_PC *packet) {
    if (AINet::sm_instance != NULL) {
        AINet::sm_instance->ReplicateAction(action, packet);
    }
}
