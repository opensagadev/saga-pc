#include "saga.h"

NETMESSAGE_PC::MessageData NETMESSAGE_PC::sm_poolMessageData[512] = {0};

/* AI script network replication (0x53d140-0x53db50). AINet is abstract: the
 * game derives from it to map AI packets to network ids. No class derives from
 * it in retail, so AINet::sm_instance stays NULL and replication is a no-op. */
extern "C" {
    extern AIACTIONDEF_PC api_aiactiondefs[];
    extern AIACTIONDEF_PC *game_aiactiondefs;
    void AINetTrace(const char *format, ...);
}

typedef i32 (*AINETFILTERFN)(void);
AINETFILTERFN AINetSendFilter = 0;
AINETFILTERFN AINetReceiveFilter = 0;

class AINet : public NetListenerInterface {
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
};
STATIC_ASSERT(sizeof(AINet) == 4);

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
                            PCByteCopy_candidate(param_text[i], message.data->bytes + message.read_offset, length);
                            message.read_offset += length;
                        }
                        params[i] = param_text[i];
                    }
                    definitions = is_game_action != 0 ? game_aiactiondefs : api_aiactiondefs;
                    if (system != NULL &&
                        definitions[(u16)index].eval_fn(system, NULL, packet, params, param_count, 1, 0.0f) != 1)
                        AINetTrace(definitions->name);
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
            PCByteCopy_candidate(destination, param, length);
            message.write_offset += length + 1;
        }
    }
    theTTNetwork.BroadcastEx(message, 10);
    message.Release();
}

extern "C" void AINetReplicateAction(AIACTION_PC *action, AIPACKET_PC *packet) {
    if (AINet::sm_instance != NULL)
        AINet::sm_instance->ReplicateAction(action, packet);
}
