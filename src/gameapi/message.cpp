#include "saga.h"

NETMESSAGE_PC::MessageData NETMESSAGE_PC::sm_poolMessageData[512] = {0};
NETMESSAGE_PC::NETMESSAGE_PC(u32 offset) {
    MessageData *entry = sm_poolMessageData;
    for (; entry != sm_poolMessageData + 512; ++entry)
        if (entry->references == 0) {
            entry->references = 1;
            break;
        }
    if (entry == sm_poolMessageData + 512)
        entry = NULL;
    data = entry;
    read_offset = offset;
    write_offset = offset;
    swap_endianness = 1;
}
void NETMESSAGE_PC::Write16(const i16 *value) {
    if (data) {
        PCByteCopy_candidate(data->bytes + write_offset, value, 2);
        if (swap_endianness)
            EdFileSwapEndianess16(data->bytes + write_offset);
        write_offset += 2;
    }
}
void NETMESSAGE_PC::Write32(const i32 *value) {
    if (data) {
        PCByteCopy_candidate(data->bytes + write_offset, value, 4);
        if (swap_endianness)
            EdFileSwapEndianess32(data->bytes + write_offset);
        write_offset += 4;
    }
}
