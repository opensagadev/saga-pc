#include "saga.h"

CRC16::CRC16() {
    for (i32 i = 0; i < 256; ++i) {
        i32 value = 0;
        u32 input = i << 8;
        for (i32 bit = 0; bit < 8; ++bit) {
            if ((value ^ input) & 0x8000) {
                value = (value << 1) ^ 0x1021;
            } else {
                value <<= 1;
            }
            input <<= 1;
            value &= 0xffff;
        }
        CRC16TablePC[i] = value;
    }
}

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

extern "C" i32 fun_00746400(i32 count, ShaderCompilerBufferAllocatorPc *buffer) {
    i32 offset = buffer->length;
    buffer->length = offset + count;
    return offset;
}

void ShaderCompilerBufferAllocatorPc::fun_00746450(char value) {
    if (length + 1 > capacity) {
        Resize((3 * (length + 1)) / 2);
    }
    data[length] = value;
    ++length;
}

u8 *ShaderCompilerBufferAllocatorPc::fun_00746490() {
    if (length > 0 && data[length - 1] != 0) {
        fun_00746450(0);
    }
    return data;
}
