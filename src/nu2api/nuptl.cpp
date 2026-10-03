#include "saga.h"

#define NU_SIN_LUT(angle) (NuTrigTable[((i32)(angle) >> 1) & 0x7fff])

extern "C" void __cdecl GenericDebinfoDmaTypeUpdate(DEBRIS_INFO_PC *effect) {
    if (effect->native_data == NULL) {
        if (freeDmaDebType >= EDPP_MAX_DMADEBTYPES) {
            NuPtlDmaSlotsExhaustedPc();
            return;
        }
        effect->native_data = DmaDebTypes[freeDmaDebType++];
    }

    DEBRIS_PART_HEADER_PC *header = effect->native_data;
    header->gravity = effect->gravity;
    header->texture_u0 =
        static_cast<f32>(static_cast<u32>(effect->texture_u0) & 0x1ffU) * static_cast<f64>(1.0f / 255.0f);
    header->texture_v0 =
        static_cast<f32>(static_cast<u32>(effect->texture_v0) & 0x1ffU) * static_cast<f64>(1.0f / 255.0f);
    header->texture_u1 =
        static_cast<f32>(static_cast<u32>(effect->texture_u1) & 0x1ffU) * static_cast<f64>(1.0f / 255.0f);
    header->texture_v1 =
        static_cast<f32>(static_cast<u32>(effect->texture_v1) & 0x1ffU) * static_cast<f64>(1.0f / 255.0f);

    f32 width, height, rotation, alpha, red, green, blue;
    f32 time, duration, elapsed, wave_x, wave_y, texture_x_numerator, texture_y_numerator;
    i32 i;
    for (i32 frame_index = 0; frame_index < 64; ++frame_index) {
        width = height = rotation = 0.0f;
        red = green = blue = alpha = 0.0f;
        time = static_cast<f32>(frame_index) / 64.0f;
        for (i = 0; i < 7; ++i) {
            if (effect->width_keys[i].time <= time && time <= effect->width_keys[i + 1].time) {
                duration = effect->width_keys[i + 1].time - effect->width_keys[i].time;
                elapsed = time - effect->width_keys[i].time;
                width = elapsed == 0.0f ? effect->width_keys[i].value
                                        : effect->width_keys[i].value +
                                              (effect->width_keys[i + 1].value - effect->width_keys[i].value) *
                                                  (elapsed / duration);
                break;
            }
        }
        for (i = 0; i < 7; ++i) {
            if (effect->height_keys[i].time <= time && time <= effect->height_keys[i + 1].time) {
                duration = effect->height_keys[i + 1].time - effect->height_keys[i].time;
                elapsed = time - effect->height_keys[i].time;
                height = elapsed == 0.0f ? effect->height_keys[i].value
                                         : effect->height_keys[i].value +
                                               (effect->height_keys[i + 1].value - effect->height_keys[i].value) *
                                                   (elapsed / duration);
                break;
            }
        }
        for (i = 0; i < 7; ++i) {
            if (effect->rotation_keys[i].time <= time && time <= effect->rotation_keys[i + 1].time) {
                duration = effect->rotation_keys[i + 1].time - effect->rotation_keys[i].time;
                elapsed = time - effect->rotation_keys[i].time;
                rotation = elapsed == 0.0f ? effect->rotation_keys[i].value
                                           : effect->rotation_keys[i].value +
                                                 (effect->rotation_keys[i + 1].value - effect->rotation_keys[i].value) *
                                                     (elapsed / duration);
                break;
            }
        }
        wave_x = effect->jib_x_amplitude * NU_SIN_LUT(65536.0 * effect->jib_x_frequency * time);
        wave_y = effect->jib_y_amplitude * NU_SIN_LUT(65536.0 * effect->jib_y_frequency * time);
        texture_x_numerator =
            NU_SIN_LUT(rotation + 16384.0) * (width / 4.0f) - (height / 4.0f) * NU_SIN_LUT(rotation) + wave_x;
        texture_y_numerator =
            NU_SIN_LUT(rotation) * (-width / 4.0f) - (height / 4.0f) * NU_SIN_LUT(rotation + 16384.0) + wave_y;

        DEBRIS_PARTICLE_FRAME_PC &frame = header->frames[frame_index];
        frame.position.x =
            (NU_SIN_LUT(rotation + 16384.0) * (-width / 4.0f) - (height / 4.0f) * NU_SIN_LUT(rotation) + wave_x) /
            2048.0f;
        frame.position.y =
            (NU_SIN_LUT(rotation) * (width / 4.0f) - (height / 4.0f) * NU_SIN_LUT(rotation + 16384.0) + wave_y) /
            2048.0f;
        frame.position.z = 0.0f;
        frame.texture_offset.x = texture_x_numerator / 2048.0f;
        frame.texture_offset.y = texture_y_numerator / 2048.0f;
        frame.texture_offset.z = 0.0f;
        frame.extent.x = (texture_x_numerator + NU_SIN_LUT(rotation) * (height / 2.0f)) / 2048.0f;
        frame.extent.y = (texture_y_numerator + NU_SIN_LUT(rotation + 16384.0) * (height / 2.0f)) / 2048.0f;
        frame.extent.z = 0.0f;
        for (i = 0; i < 7; ++i) {
            if (effect->colour_keys[i].time <= time && time <= effect->colour_keys[i + 1].time) {
                duration = effect->colour_keys[i + 1].time - effect->colour_keys[i].time;
                elapsed = time - effect->colour_keys[i].time;
                if (elapsed == 0.0f) {
                    red = effect->colour_keys[i].red;
                    green = effect->colour_keys[i].green;
                    blue = effect->colour_keys[i].blue;
                } else {
                    red = effect->colour_keys[i].red +
                          static_cast<i32>(effect->colour_keys[i + 1].red - effect->colour_keys[i].red) *
                              (elapsed / duration);
                    green = effect->colour_keys[i].green +
                            static_cast<i32>(effect->colour_keys[i + 1].green - effect->colour_keys[i].green) *
                                (elapsed / duration);
                    blue = effect->colour_keys[i].blue +
                           static_cast<i32>(effect->colour_keys[i + 1].blue - effect->colour_keys[i].blue) *
                               (elapsed / duration);
                }
                break;
            }
        }
        for (i = 0; i < 7; ++i) {
            if (effect->alpha_keys[i].time <= time && time <= effect->alpha_keys[i + 1].time) {
                duration = effect->alpha_keys[i + 1].time - effect->alpha_keys[i].time;
                elapsed = time - effect->alpha_keys[i].time;
                alpha = elapsed == 0.0f ? effect->alpha_keys[i].value
                                        : effect->alpha_keys[i].value +
                                              (effect->alpha_keys[i + 1].value - effect->alpha_keys[i].value) *
                                                  (elapsed / duration);
                break;
            }
        }
        alpha += alpha;
        if (alpha > 255.0f)
            alpha = 255.0f;
        frame.colour = (((static_cast<u32>(alpha) << 8) | static_cast<u32>(red)) << 8 | static_cast<u32>(green)) << 8 |
                       static_cast<u32>(blue);
    }
}
