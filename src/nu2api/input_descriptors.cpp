#include "saga.h"

extern "C" void SetPadDescriptorPc(i32 product_id, const u16 *descriptor) {
    if (descriptor == NULL)
        return;
    PC_INPUT_CONFIG_PC *config = NULL;
    /* Retail scans 24 table slots although the static table holds 15 configs. */
    for (u32 index = 0; index < 24; ++index) {
        if (PcStaticInputConfigs[index].product_id == (u32)product_id) {
            config = &PcStaticInputConfigs[index];
            break;
        }
    }
    if (config == NULL)
        return;
    for (i32 index = 0; index < 4; ++index) {
        config->axes[index][0] = 10;
        config->axes[index][1] = (u8)descriptor[0];
        config->axes[index][2] = (u8)descriptor[1];
        descriptor += 2;
    }
    for (i32 index = 0; index < 4; ++index) {
        config->unknown_axes[index][0] = 0;
        config->unknown_axes[index][1] = 0;
        config->unknown_axes[index][2] = 0;
    }
    for (i32 index = 0; index < 16; ++index) {
        if ((i16)*descriptor != -1) {
            config->buttons[index][0] = 7;
            config->buttons[index][1] = (u8)*descriptor;
        } else {
            config->buttons[index][0] = 0;
            config->buttons[index][1] = 0;
        }
        ++descriptor;
    }
    while ((i16)*descriptor != -1) {
        config->buttons[(i16)descriptor[0]][0] = (u8)descriptor[1];
        config->buttons[(i16)descriptor[0]][1] = (u8)descriptor[2];
        descriptor += 3;
    }
}

extern "C" void InitPadDescriptorsPc(const u16 *first, const u16 *second, const u16 *third, const u16 *fourth,
                                     const i8 *order) {
    SetPadDescriptorPc(3, first);
    SetPadDescriptorPc(1, second);
    SetPadDescriptorPc(4, third);
    SetPadDescriptorPc(2, fourth);
    PcInputActionOrder[0] = -1;
    if (order != NULL) {
        i32 index = 0;
        i32 value;
        do {
            value = *order++;
            PcInputActionOrder[index++] = value;
        } while (value >= 0);
    }
}
