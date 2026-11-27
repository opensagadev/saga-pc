#include "gamelib/util/CRC16.h"
#include "nu2api/ShaderManagerHLSL.h"

extern "C" {
    u32 CRC16TablePC[256] = {0};
}

CRC16 CRC16InstancePC;

u32 CRC16::hash(const unsigned char *data, i32 length) {
    i32 crc = 0xffff;
    for (i32 i = 0; i < length; ++i) {
        crc = ((crc << 8) ^ CRC16TablePC[((crc >> 8) ^ *data++) & 0xff]) & 0xffff;
    }
    return crc;
}

u32 CRC16::hashInverse(const unsigned char *data, i32 length) {
    data += length;
    i32 crc = 0xffff;
    for (i32 i = 0; i < length; ++i) {
        crc = ((crc << 8) ^ CRC16TablePC[((crc >> 8) ^ *--data) & 0xff]) & 0xffff;
    }
    return crc;
}

extern "C" i32 ShaderBufferReserve(i32 count, ShaderCompilerBufferAllocator *buffer) {
    i32 offset = buffer->length;
    buffer->length = offset + count;
    return offset;
}

void ShaderCompilerBufferAllocator::AppendChar(char value) {
    if (length + 1 > capacity) {
        Resize((3 * (length + 1)) / 2);
    }
    data[length] = value;
    ++length;
}

char *ShaderCompilerBufferAllocator::TerminatedText() {
    if (length > 0 && data[length - 1] != 0) {
        AppendChar(0);
    }
    return data;
}
