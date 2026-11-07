#include "saga.h"

/* RefPack decompressor (retail 0x617a60), the counterpart of refpack. Returns
 * the number of bytes written to destination. */
i32 unref(u8 *source, u8 *destination) {
    u8 *output = destination;
    u8 *match;
    u8 first;
    u8 second;
    u8 third;
    u8 fourth;
    u32 run;

    for (;;) {
        first = *source++;
        if ((first & 0x80) == 0) {
            second = *source++;
            run = first & 3;
            while (run--)
                *output++ = *source++;
            match = output - 1 - (((first & 0x60) << 3) + second);
            run = ((first & 0x1c) >> 2) + 3 - 1;
            do {
                *output++ = *match++;
            } while (run--);
        } else if ((first & 0x40) == 0) {
            second = *source++;
            third = *source++;
            run = second >> 6;
            while (run--)
                *output++ = *source++;
            match = output - 1 - (((second & 0x3f) << 8) + third);
            run = (first & 0x3f) + 4 - 1;
            do {
                *output++ = *match++;
            } while (run--);
        } else if ((first & 0x20) == 0) {
            second = *source++;
            third = *source++;
            fourth = *source++;
            run = first & 3;
            while (run--)
                *output++ = *source++;
            match = output - 1 - (((first & 0x10) << 12) + (second << 8) + third);
            run = ((first & 0x0c) << 6) + fourth + 5 - 1;
            do {
                *output++ = *match++;
            } while (run--);
        } else {
            run = ((first & 0x1f) << 2) + 4;
            if (run <= 0x70) {
                while (run--)
                    *output++ = *source++;
                continue;
            }
            break;
        }
    }
    run = first & 3;
    while (run--)
        *output++ = *source++;
    return output - destination;
}
