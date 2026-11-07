#include "saga.h"

/* Network message pool support object (0x60f2a0-0x60f3a0). */

void NetMessagePoolError(void) {
    theNetSessionManager->error = (i32)0xa0001000;
}

/* Writes count elements of size bytes, swapping each element's byte order when
 * the message is endian-swapped (standalone copy at 0x580e50). */
void NETMESSAGE_PC::Write(const void *values, i32 size, i32 count) {
    if (data != NULL) {
        i32 bytes = size * count;
        PCByteCopy_candidate(data->bytes + write_offset, values, bytes);
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

void NETMESSAGE_PC::Read(void *values, i32 size, i32 count) {
    if (data != NULL) {
        i32 bytes = size * count;
        PCByteCopy_candidate(values, data->bytes + read_offset, bytes);
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

/* Empty varargs trace used by NetworkObjectManager::ImportObjects (0x6118f0);
 * kept out of line so its callers keep the call. */
void NOSTrace(...) {
}
