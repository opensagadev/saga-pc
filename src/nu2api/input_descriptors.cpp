#include "saga.h"

extern "C" void SetPadDescriptorPc(i32 product_id, const u16 *descriptor) {
    if (descriptor == NULL)
        return;
    PC_INPUT_CONFIG_PC *config = NULL;
    for (i32 index = 0; index < 15; ++index) {
        if (PcStaticInputConfigs[index].product_id == (u32)product_id) {
            config = &PcStaticInputConfigs[index];
            break;
        }
    }
    if (config == NULL)
        return;
    for (i32 index = 0; index < 4; ++index) {
        config->axes[index][0] = 10;
        config->axes[index][1] = (u8)*descriptor++;
        config->axes[index][2] = (u8)*descriptor++;
    }
    for (i32 index = 0; index < 4; ++index) {
        config->unknown_axes[index][0] = 0;
        config->unknown_axes[index][1] = 0;
        config->unknown_axes[index][2] = 0;
    }
    for (i32 index = 0; index < 16; ++index) {
        u16 key = *descriptor++;
        config->buttons[index][0] = key == 0xffff ? 0 : 7;
        config->buttons[index][1] = key == 0xffff ? 0 : (u8)key;
    }
    while (*descriptor != 0xffff) {
        i16 index = (i16)descriptor[0];
        config->buttons[index][0] = (u8)descriptor[1];
        config->buttons[index][1] = (u8)descriptor[2];
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
        i8 value;
        do {
            value = *order++;
            PcInputActionOrder[index++] = value;
        } while (value >= 0);
    }
}
