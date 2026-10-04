#include "saga.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
u32 ImplodeDecodeC(void);
u32 ImplodeDecodeOffset(void);
void ImplodeMakeTable(i32, u8 *, i32, u16 *);
void ImplodeHufDecodeStart(void);
void ImplodeInitGetBits(void);
u32 ImplodeGetBits(i32 count);
void ImplodeFillBuf(i32 count);
u8 ImplodeGetByteFromMem(void);
void ImplodeError(char *msg, ...);
extern u32 implode_bitbuf;
extern u16 implode_left[0x3fb];
extern u16 implode_right[0x3fb];
extern i32 gExplodeInitialised;

extern char *implode_inbuffer;
extern char *implode_outbuffer;
extern u32 implode_origsize;
extern u32 implode_compsize;

static i32 ImplodePendingCopyPc;

static void ImplodeDecodeRingPrivatePc(u32 size, unsigned char *buf) {
    static u32 ImplodeRingCursorPc;

    u32 c;
    u32 n;

    n = 0;

    while (--ImplodePendingCopyPc >= 0) {
        buf[n] = buf[ImplodeRingCursorPc];
        ImplodeRingCursorPc = (ImplodeRingCursorPc + 1) & 0x1fff;

        if (++n == size) {
            return;
        }
    }

    while (1) {
        c = ImplodeDecodeC();

        if (c < 0x100) {
            buf[n] = c;

            if (++n == size) {
                return;
            }
        } else {
            ImplodePendingCopyPc = c - 0xfd;
            ImplodeRingCursorPc = (n - ImplodeDecodeOffset() - 1) & 0x1fff;

            while (--ImplodePendingCopyPc >= 0) {
                buf[n] = buf[ImplodeRingCursorPc];
                ImplodeRingCursorPc = (ImplodeRingCursorPc + 1) & 0x1fff;

                if (++n == size) {
                    return;
                }
            }
        }
    }
}

static unsigned char ImplodeRingBufferPc[0x2000];

i32 ExplodeBufferNoHeader(char *in_buf, char *out_buf, i32 compressed_size, i32 orig_size) {
    i32 total_read;
    u32 read_size;

    if (gExplodeInitialised == 0) {
        return 0;
    }

    implode_inbuffer = in_buf;
    implode_outbuffer = out_buf;
    implode_origsize = orig_size;
    implode_compsize = compressed_size;

    ImplodeHufDecodeStart();
    ImplodePendingCopyPc = 0;

    total_read = 0;
    while (implode_origsize != 0) {
        read_size = implode_origsize < 0x2000 ? implode_origsize : 0x2000;

        ImplodeDecodeRingPrivatePc(read_size, ImplodeRingBufferPc);

        memcpy(implode_outbuffer, ImplodeRingBufferPc, read_size);

        implode_outbuffer += read_size;
        implode_origsize -= read_size;
        total_read += read_size;
    }

    return total_read;
}

#define SELECT(dst, table, shift, count, initial_mask, len)                                                            \
    do {                                                                                                               \
        (dst) = (table)[implode_bitbuf >> shift];                                                                      \
                                                                                                                       \
        if ((dst) > (count)) {                                                                                         \
            u32 mask = (initial_mask);                                                                                 \
                                                                                                                       \
            do {                                                                                                       \
                if ((implode_bitbuf & mask) != 0) {                                                                    \
                    (dst) = implode_right[(dst)];                                                                      \
                } else {                                                                                               \
                    (dst) = implode_left[(dst)];                                                                       \
                }                                                                                                      \
                                                                                                                       \
                mask >>= 1;                                                                                            \
            } while ((dst) > (count));                                                                                 \
        }                                                                                                              \
                                                                                                                       \
        ImplodeFillBuf((len)[(dst)]);                                                                                  \
    } while (0)

static u32 ImplodeBlockRemainingPc;

static u16 ImplodeOffsetTablePc[0x100];
static u8 ImplodeOffsetLengthsPc[0x13];

static void ImplodeReadOffsetLengthsPrivatePc(i32 table_size, i32 bit_count, i32 zero_value) {
    i32 n;
    i32 i;
    i32 value;
    u32 k;

    n = ImplodeGetBits(bit_count);
    if (n == 0) {
        value = ImplodeGetBits(bit_count);

        for (i = 0; i < table_size; i++) {
            ImplodeOffsetLengthsPc[i] = 0;
        }

        for (i = 0; i < 0x100; i++) {
            ImplodeOffsetTablePc[i] = value;
        }

        return;
    }

    i = 0;
    while (i < n) {
        value = implode_bitbuf >> 0x1d;

        if (value == 7) {
            for (k = 0x10000000; (implode_bitbuf & k) != 0; k >>= 1, value++) {
            }
        }

        ImplodeFillBuf(value >= 7 ? value - 3 : 3);

        ImplodeOffsetLengthsPc[i] = value;
        i++;

        if (i == zero_value) {
            value = ImplodeGetBits(2);

            while (--value >= 0) {
                ImplodeOffsetLengthsPc[i] = 0;
                i++;
            }
        }
    }

    for (; i < table_size; i++) {
        ImplodeOffsetLengthsPc[i] = 0;
    }

    ImplodeMakeTable(table_size, ImplodeOffsetLengthsPc, 8, ImplodeOffsetTablePc);
}

static u16 ImplodeLiteralTablePc[0x1000];
static u8 ImplodeLiteralLengthsPc[0x1fe];

static void ImplodeReadLiteralLengthsPrivatePc() {
    i32 n;
    i32 i;
    i32 value;

    n = ImplodeGetBits(9);
    if (n == 0) {
        value = ImplodeGetBits(9);

        for (i = 0; i < sizeof(ImplodeLiteralLengthsPc); i++) {
            ImplodeLiteralLengthsPc[i] = 0;
        }

        for (i = 0; i < 0x1000; i++) {
            ImplodeLiteralTablePc[i] = value;
        }

        return;
    }

    i = 0;
    while (i < n) {
        SELECT(value, ImplodeOffsetTablePc, 0x18, 0x12, 0x800000, ImplodeOffsetLengthsPc);

        if (value < 3) {
            if (value == 0) {
                value = 1;
            } else if (value == 1) {
                value = ImplodeGetBits(4) + 0x3;
            } else {
                value = ImplodeGetBits(9) + 0x14;
            }

            while (--value >= 0) {
                ImplodeLiteralLengthsPc[i] = 0;
                i++;
            }
        } else {
            ImplodeLiteralLengthsPc[i] = value - 2;
            i++;
        }
    }

    for (; i < sizeof(ImplodeLiteralLengthsPc); i++) {
        ImplodeLiteralLengthsPc[i] = 0;
    }

    ImplodeMakeTable(sizeof(ImplodeLiteralLengthsPc), ImplodeLiteralLengthsPc, 0xc, ImplodeLiteralTablePc);
}

extern u16 implode_left[0x3fb];
extern u16 implode_right[0x3fb];

u32 ImplodeDecodeC() {
    u32 value;

    if (ImplodeBlockRemainingPc == 0) {
        ImplodeBlockRemainingPc = ImplodeGetBits(0x10);

        ImplodeReadOffsetLengthsPrivatePc(0x13, 5, 3);
        ImplodeReadLiteralLengthsPrivatePc();
        ImplodeReadOffsetLengthsPrivatePc(0xe, 4, -1);
    }

    ImplodeBlockRemainingPc--;

    SELECT(value, ImplodeLiteralTablePc, 0x14, sizeof(ImplodeLiteralLengthsPc) - 1, 0x80000, ImplodeLiteralLengthsPc);

    return value;
}

u32 ImplodeDecodeOffset() {
    u32 offset;

    SELECT(offset, ImplodeOffsetTablePc, 0x18, 0xd, 0x800000, ImplodeOffsetLengthsPc);

    if (offset != 0) {
        offset = (1u << (offset - 1)) + ImplodeGetBits(offset - 1);
    }

    return offset;
}

void ImplodeMakeTable(i32 size, u8 *len, i32 unknown, u16 *table) {
    u16 symbol_count[17];
    u16 masks[17];
    u16 offsets[18];
    u32 symbol;
    i32 shift;
    u32 length;
    u32 i;
    u32 j;
    u32 k;
    u32 unknown2;
    u32 unknown3;
    u32 unknown4;
    u16 *cursor;

    for (i = 1; i < 0x11; i++) {
        symbol_count[i] = 0;
    }

    for (i = 0; i < size; i++) {
        symbol_count[len[i]]++;
    }

    offsets[1] = 0;

    for (i = 1; i < 0x11; i++) {
        offsets[i + 1] = offsets[i] + (symbol_count[i] << (0x10 - i));
    }

    if (offsets[0x11] != 0) {
        ImplodeError("Bad table");
    }

    shift = 0x10 - unknown;

    for (i = 1; i <= unknown; i++) {
        offsets[i] >>= shift;
        masks[i] = 1 << (unknown - i);
    }

    for (; i < 0x11; i++) {
        masks[i] = 1 << (0x10 - i);
    }

    i = (i32)offsets[unknown + 1] >> shift;
    if (i != 0) {
        for (j = 1 << unknown; i != j; i++) {
            table[i] = 0;
        }
    }

    unknown2 = size;
    unknown3 = 1 << (0xf - unknown);

    for (symbol = 0; symbol < size; symbol++) {
        if ((length = len[symbol]) == 0) {
            continue;
        }

        unknown4 = offsets[length] + masks[length];
        if (unknown >= length) {
            for (i = offsets[length]; i < unknown4; i++) {
                table[i] = symbol;
            }
        } else {
            j = offsets[length];
            cursor = table + (j >> shift);

            for (k = length - unknown; k != 0; k--) {
                if (*cursor == 0) {
                    implode_left[unknown2] = 0;
                    implode_right[unknown2] = implode_left[unknown2];
                    *cursor = unknown2;

                    unknown2++;
                }

                if ((j & unknown3) != 0) {
                    cursor = implode_right + *cursor;
                } else {
                    cursor = implode_left + *cursor;
                }

                j <<= 1;
            }

            *cursor = symbol;
        }

        offsets[length] = unknown4;
    }
}

void ImplodeHufDecodeStart() {
    ImplodeInitGetBits();

    ImplodeBlockRemainingPc = 0;
}

extern u32 implode_bitbuf;
static u32 ImplodeSubBitsPc;
static u32 ImplodeAvailableBitsPc;

void ImplodeInitGetBits() {
    implode_bitbuf = 0;
    ImplodeSubBitsPc = 0;
    ImplodeAvailableBitsPc = 0;

    ImplodeFillBuf(0x20);
}

u32 ImplodeGetBits(i32 count) {
    u32 value;

    if (count == 0) {
        return 0;
    }

    value = implode_bitbuf >> (0x20u - count);

    ImplodeFillBuf(count);

    return value;
}

void ImplodeFillBuf(i32 count) {
    implode_bitbuf <<= count;

    while (ImplodeAvailableBitsPc < count) {
        implode_bitbuf |= ImplodeSubBitsPc << (count -= ImplodeAvailableBitsPc);

        if (implode_compsize != 0) {
            implode_compsize--;

            ImplodeSubBitsPc = ImplodeGetByteFromMem() & 0xff;
        } else {
            ImplodeSubBitsPc = 0;
        }

        ImplodeAvailableBitsPc = 8;
    }

    implode_bitbuf |= ImplodeSubBitsPc >> (ImplodeAvailableBitsPc -= count);
}

unsigned char ImplodeGetByteFromMem() {
    return *implode_inbuffer++;
}

void ImplodeError(char *msg, ...) {
    va_list args;
    va_start(args, msg);

    putc('\n', stderr);
    vfprintf(stderr, msg, args);
    putc('\n', stderr);

    NuFileError();
}
