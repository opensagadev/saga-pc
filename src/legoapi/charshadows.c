#include "saga.h"

/* Retail unit 0x589f60-0x58a504: definitions in retail address order. */

extern i32 CharacterBlobShadowsOnPc;
f32 __cdecl DropInOutScale(GAMEOBJECT *object);

void __cdecl CharShadows_Reset(CHARACTER_SHADOW_PACKET_PC *packet) {
    packet->shadows[0].position.y = 2000000.0f;
    packet->shadows[1].position.y = 2000000.0f;
    packet->shadows[2].position.y = 2000000.0f;
    packet->shadows[3].position.y = 2000000.0f;
    packet->shadows[4].position.y = 2000000.0f;
}

void __cdecl CharShadows_Update(void) {
    GAMEOBJECT *object = Obj;
    CHARACTER_SHADOW_PACKET_PC *packet;
    CHARACTER_SHADOW_RESET_SLOT_PC *slot;
    NUVEC position;
    NUVEC *locator_position;
    f32 height;
    f32 extended_height;
    i32 index;
    i32 locator;
    i32 count;
    i32 layer;
    for (index = 0; index < HIGHGAMEOBJECT; ++index, ++object) {
        if ((object->flags_1fc_word & 1) == 0 || (object->flags_1fc_word & 0x1000) == 0 || object->dead != 0 ||
            (object->character_data->model_flags & 0x10000) != 0 || object->use_model_origin == 0 ||
            object->character_data->game_character->shadow_locators == 0 || object->model_draw_result == 0 ||
            (U64_PAIR(&CInfo[(i8)object->character_context].flags) & 0x20000) != 0 || object->shadow_opacity <= 0.0f) {
            continue;
        }
        object->shadow_joint_mask = object->character_data->game_character->shadow_locators;
        if (object->character_data->game_character->field_08c < 99.0f) {
            object->shadow_radius = object->character_data->game_character->field_08c;
        } else {
            object->shadow_radius = object->character_data->collision_radius;
        }
        packet = (CHARACTER_SHADOW_PACKET_PC *)object->ai_player_packet_pc;
        count = 0;
        for (locator = 0; locator < 16; ++locator) {
            if (count >= 5) {
                break;
            }
            if ((object->shadow_joint_mask & (1u << locator)) == 0 ||
                object->character_model->points_of_interest[locator] == NULL) {
                continue;
            }
            slot = &packet->shadows[count];
            locator_position = (NUVEC *)&object->locator_matrices[locator].m30;
            slot->position.x = locator_position->x;
            slot->position.z = locator_position->z;
            if ((object->flags_1fc_word & 0x10) != 0) {
                slot->position.y = object->floor_height;
                slot->x_rotation = 0;
                slot->z_rotation = 0;
                slot->opacity = 1.0f;
            } else {
                position = *locator_position;
                height = GameShadow(object, &position, 5.0f, -1);
                slot->position.y = height;
                if ((f64)height != 2000000.0) {
                    layer = EShadowInfo();
                    extended_height = EShadY;
                    if ((f64)extended_height != 2000000.0 && (u32)layer <= 16 && (TerLayer[layer].flags & 1) != 0) {
                        if (height < extended_height || (f64)locator_position->y + (f64)0.075f < (f64)height) {
                            slot->position.y = 2000000.0f;
                            height = 2000000.0f;
                        }
                    }
                    if ((f64)height != 2000000.0) {
                        slot->opacity = ShadNorm.y;
                        FindAnglesZX(&ShadNorm, NULL, NULL);
                        slot->x_rotation = temp_zrot;
                        slot->z_rotation = temp_xrot;
                    }
                }
            }
            ++count;
        }
        for (; count < 5; ++count) {
            packet->shadows[count].position.y = 2000000.0f;
        }
    }
}

void __cdecl CharShadows_Draw(void) {
    GAMEOBJECT *object;
    CHARACTER_SHADOW_PACKET_PC *packet;
    CHARACTER_SHADOW_RESET_SLOT_PC *slot;
    NUVEC position;
    f32 alpha;
    f32 radius;
    i32 index;
    i32 locator;
    i32 count;
    i32 level_alpha;
    if (CharacterBlobShadowsOnPc == 0 || PcWindowOptions.shadowmethod != 1 || PcWindowOptions.shadowsenabled == 0) {
        return;
    }
    object = Obj;
    for (index = 0; index < HIGHGAMEOBJECT; ++index, ++object) {
        if ((object->flags_1fc_word & 1) == 0 || (object->flags_1fc_word & 0x1000) == 0 || object->dead != 0) {
            continue;
        }
        if ((object->flags_e20 & 0x20) != 0 && (U64_PAIR(&CInfo[(i8)object->character_context].flags) & 0x40000) == 0) {
            continue;
        }
        if ((object->character_data->model_flags & 0x10000) != 0 || !(object->shadow_opacity > 0.0f) ||
            object->model_draw_result == 0 || object->shadow_joint_mask == 0 || (object->flags_ef0 & 0x4000000) != 0 ||
            (U64_PAIR(&CInfo[(i8)object->character_context].flags) & 0x20000) != 0) {
            continue;
        }
        if (object->character_data->game_character->blob_shadow_alpha < 255) {
            level_alpha = object->character_data->game_character->blob_shadow_alpha;
        } else {
            level_alpha = WORLD->current_level->blob_shadow_alpha;
        }
        if (level_alpha <= 0) {
            continue;
        }
        radius = object->shadow_radius;
        if (!(radius > 0.0f)) {
            continue;
        }
        alpha = (f32)level_alpha * object->shadow_opacity;
        radius *= object->origin_scale;
        radius *= DropInOutScale(object);
        packet = (CHARACTER_SHADOW_PACKET_PC *)object->ai_player_packet_pc;
        slot = packet->shadows;
        count = 0;
        for (locator = 0; locator < 16; ++locator) {
            if (count >= 5) {
                break;
            }
            if ((object->shadow_joint_mask & (1u << locator)) == 0 ||
                object->character_model->points_of_interest[locator] == NULL) {
                continue;
            }
            if ((f64)slot->position.y != 2000000.0 && slot->opacity > 0.0f &&
                (f64)slot->position.y <= (f64)object->locator_matrices[locator].m31 + (f64)0.025f) {
                position.x = slot->position.x;
                position.y = (f32)((f64)slot->position.y + (f64)0.005f);
                position.z = slot->position.z;
                NuRndrAddShadow(&position, radius, (i32)(slot->opacity * alpha), slot->x_rotation, 0, slot->z_rotation);
            }
            ++count;
            ++slot;
        }
    }
}
