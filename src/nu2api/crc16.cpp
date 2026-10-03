#include "saga.h"

CRC16::CRC16() {
    for (u32 i = 0; i < 256; ++i) {
        u32 value = 0, input = i << 8;
        for (i32 bit = 0; bit < 8; ++bit) {
            bool carry = ((value ^ input) & 0x8000) != 0;
            value = carry ? ((value << 1) ^ 0x1021) & 0xffff : (value << 1) & 0xffff;
            input <<= 1;
        }
        CRC16TablePC[i] = value;
    }
}
u32 CRC16::hash(const unsigned char *data, i32 length) {
    u32 crc = 0xffff;
    for (; length > 0; --length, ++data)
        crc = ((crc << 8) ^ CRC16TablePC[((crc >> 8) ^ *data) & 0xff]) & 0xffff;
    return crc;
}
u32 CRC16::hashInverse(const unsigned char *data, i32 length) {
    const unsigned char *cursor = data + length;
    u32 crc = 0xffff;
    for (; length > 0; --length) {
        --cursor;
        crc = ((crc << 8) ^ CRC16TablePC[((crc >> 8) ^ *cursor) & 0xff]) & 0xffff;
    }
    return crc;
}
