#include "saga.h"

struct zipup_pc_s {
    char name[16];
    union {
        NUVEC endpoints[3];
        struct {
            NUVEC lower_position, hook_origin, upper_position;
        };
    };
    u16 hook_x_rotation, hook_y_rotation;
    NUVEC hook_position, rider_start_offset, rider_target_position;
    GAMEOBJECT *occupant;
    u16 pitch_adjustment, direction, facing_angle;
    u8 unknown_66[2];
    union {
        u16 flags_word;
        struct {
            u8 flags, runtime_flags;
        };
        struct {
            u16 bit_01 : 1;
            u16 bit_02 : 1;
            u16 bit_04 : 1;
            u16 bit_08 : 1;
            u16 bit_10 : 1;
            u16 bit_20 : 1;
        } config;
    };
    u8 unknown_6a[2];
    f32 lower_ground_height, upper_ground_height;
    u16 lower_ground_x_rotation, upper_ground_x_rotation;
    u16 lower_ground_z_rotation, upper_ground_z_rotation;
};
STATIC_ASSERT(sizeof(ZIPUP_PC) == 0x7c);
STATIC_ASSERT(offsetof(ZIPUP_PC, name) == 0x0);
STATIC_ASSERT(offsetof(ZIPUP_PC, endpoints) == 0x10);
STATIC_ASSERT(offsetof(ZIPUP_PC, lower_position) == 0x10);
STATIC_ASSERT(offsetof(ZIPUP_PC, hook_origin) == 0x1c);
STATIC_ASSERT(offsetof(ZIPUP_PC, upper_position) == 0x28);
STATIC_ASSERT(offsetof(ZIPUP_PC, hook_x_rotation) == 0x34);
STATIC_ASSERT(offsetof(ZIPUP_PC, hook_y_rotation) == 0x36);
STATIC_ASSERT(offsetof(ZIPUP_PC, hook_position) == 0x38);
STATIC_ASSERT(offsetof(ZIPUP_PC, rider_start_offset) == 0x44);
STATIC_ASSERT(offsetof(ZIPUP_PC, rider_target_position) == 0x50);
STATIC_ASSERT(offsetof(ZIPUP_PC, occupant) == 0x5c);
STATIC_ASSERT(offsetof(ZIPUP_PC, pitch_adjustment) == 0x60);
STATIC_ASSERT(offsetof(ZIPUP_PC, direction) == 0x62);
STATIC_ASSERT(offsetof(ZIPUP_PC, facing_angle) == 0x64);
STATIC_ASSERT(offsetof(ZIPUP_PC, flags_word) == 0x68);
STATIC_ASSERT(offsetof(ZIPUP_PC, flags) == 0x68);
STATIC_ASSERT(offsetof(ZIPUP_PC, runtime_flags) == 0x69);
STATIC_ASSERT(offsetof(ZIPUP_PC, lower_ground_height) == 0x6c);
STATIC_ASSERT(offsetof(ZIPUP_PC, upper_ground_height) == 0x70);
STATIC_ASSERT(offsetof(ZIPUP_PC, lower_ground_x_rotation) == 0x74);
STATIC_ASSERT(offsetof(ZIPUP_PC, upper_ground_x_rotation) == 0x76);
STATIC_ASSERT(offsetof(ZIPUP_PC, lower_ground_z_rotation) == 0x78);
STATIC_ASSERT(offsetof(ZIPUP_PC, upper_ground_z_rotation) == 0x7a);
ZIPUP_PC *__cdecl ZipUp_FindNearest(WORLDINFO *world, NUVEC *position, f32 radius, f32 *distance, i32 *endpoint,
                                    GAMEOBJECT *object) {
    ZIPUP_PC *nearest;
    i32 index, end;
    f32 square_distance, best_distance;

    ZIPUP_PC *zipup = world->zipups;
    nearest = NULL;

    if (zipup == NULL)
        return NULL;
    best_distance = radius + 0.25;
    best_distance *= best_distance;
    for (index = 0; index < world->zipup_count; ++index, ++zipup) {
        if (object != NULL) {
            if ((zipup->flags & 0xc0) != 0xc0)
                continue;
            if ((zipup->flags & 0x20) != 0) {
                if ((object->character_data->game_character->flags_094 & 0x20000000) == 0)
                    continue;
            } else {
                if ((object->character_data->game_character->flags_094 & 0x20000000) != 0)
                    continue;
                if ((zipup->flags & 2) == 0 && object->field_280 != 0xff)
                    continue;
            }
        }
        for (end = 0; end < 3; ++end) {
            if (object != NULL && (end == 1 || (end == 2 && (zipup->flags & 8) == 0)))
                continue;
            square_distance = NuVecDistSqr(position, &zipup->endpoints[end], NULL);
            if (best_distance > square_distance) {
                best_distance = square_distance;
                nearest = zipup;
                if (endpoint != NULL)
                    *endpoint = end;
            }
        }
    }
    if (distance != NULL)
        *distance = best_distance;
    return nearest;
}

extern i32 __cdecl ZipUps_Load(WORLDINFO *world) {
    i32 version, index;

    if (world->zipup_count == 0) {
        version = EdFileReadInt();
        world->zipup_count = EdFileReadInt();
        for (index = 0; index < world->zipup_count; ++index) {
            EdFileRead(world->zipups[index].name, sizeof(world->zipups[index].name));
            EdFileReadNuVec(&world->zipups[index].lower_position);
            EdFileReadNuVec(&world->zipups[index].hook_origin);
            EdFileReadNuVec(&world->zipups[index].upper_position);
            world->zipups[index].hook_x_rotation = EdFileReadUnsignedShort();
            world->zipups[index].hook_y_rotation = EdFileReadUnsignedShort();
            world->zipups[index].config.bit_01 = EdFileReadUnsignedChar() != 0;
            world->zipups[index].config.bit_02 = EdFileReadUnsignedChar() != 0;
            world->zipups[index].config.bit_08 = EdFileReadUnsignedChar() != 0;
            if (version >= 2)
                world->zipups[index].config.bit_10 = EdFileReadUnsignedChar() != 0;
            else
                world->zipups[index].config.bit_10 = 1;
            if (version >= 3)
                world->zipups[index].config.bit_20 = EdFileReadUnsignedChar() != 0;
            else
                world->zipups[index].config.bit_20 = 0;
            if (version >= 4)
                world->zipups[index].config.bit_04 = EdFileReadUnsignedChar() != 0;
            else
                world->zipups[index].config.bit_04 = 1;
        }
        return 1;
    }
    return 0;
}
extern void *__cdecl ZipUps_ReserveBufferSpace(WORLDINFO *world) {
    void *reserved;

    reserved = NULL;
    world->zipups = NULL;
    world->zipup_count = 0;
    if (world->current_level->max_zipups > 0) {
        world->giz_buffer.addr = (world->giz_buffer.addr + 3) & ~3u;
        world->zipups = (ZIPUP_PC *)world->giz_buffer.addr;
        world->giz_buffer.addr += world->current_level->max_zipups * sizeof(ZIPUP_PC);
        reserved = world->zipups;
    }
    return reserved;
}

typedef struct zipup_transport_view_pc_s {
    u8 unknown_00[0x748];
    NUVEC entry, start, swing, landing;
    u8 unknown_778[0x634];
    f32 detach_fall_delay;
} ZIPUP_TRANSPORT_VIEW_PC;
STATIC_ASSERT(offsetof(ZIPUP_TRANSPORT_VIEW_PC, detach_fall_delay) == 0xdac);
STATIC_ASSERT(offsetof(ZIPUP_TRANSPORT_VIEW_PC, entry) == 0x748);
STATIC_ASSERT(offsetof(ZIPUP_TRANSPORT_VIEW_PC, start) == 0x754);
STATIC_ASSERT(offsetof(ZIPUP_TRANSPORT_VIEW_PC, swing) == 0x760);
STATIC_ASSERT(offsetof(ZIPUP_TRANSPORT_VIEW_PC, landing) == 0x76c);

extern f32 ZipUpSwingOffsetLengthPc, ZipUpSwingOffsetHeightPc;
extern f32 ZipUpRiderOffsetScalePc, ZipUpRiderHeightPc, ZipUpSwingDurationPc;
extern u16 ZipUpSwingStartAnimationPc, ZipUpSwingSwingAnimationPc, ZipUpSwingJumpAnimationPc;
extern i16 BigJumpLandFallbackPc;
extern char GrapDetach_text[11], GrapAttach_text[11], GrapSwing_text[10];
extern i32 __cdecl ObjLandReady(GAMEOBJECT *);

static __inline f32 ZipUpFabsPc(f32 value) {
    u32 bits = *(u32 *)&value & 0x7fffffff;
    return *(f32 *)&bits;
}

static i32 __cdecl ZipUpArcSinePc(f32 value) {
    f32 magnitude, root, low, b, c, d;
    f64 t, square, cube, fourth, polynomial;
    magnitude = ZipUpFabsPc(value);
    root = NuFsqrt(1.0f - value * value);
    low = root < magnitude ? root : magnitude;
    b = (f32)MAX(MIN((magnitude - (f64)0.70710677f) * (f64)3.40282e38f, 1.0), -1.0);
    c = (f32)(value * (f64)3.40282e38f > 1.0 ? 1.0 : value * (f64)3.40282e38f);
    c = MAX(c, -1.0f);
    d = b * c + c;
    t = (f64)b * c * low;
    square = t * t;
    cube = square * t;
    fourth = square * square;
    polynomial = d * (f64)0.785398f + t * -1.0 + (PCPathArcSineCoefficient3 * t) * square +
                 (PCPathArcSineCoefficient5 * square) * cube + (cube * PCPathArcSineCoefficient7) * fourth +
                 (square * cube) * (fourth * PCPathArcSineCoefficient9);
    return (i32)(polynomial * (f64)10430.4f);
}

#define NU_SIN_LUT(a) NuTrigTable[((i32)(a) >> 1) & 0x7fff]
#define NU_COS_LUT(a) NuTrigTable[(((i32)(a) + 0x4000) >> 1) & 0x7fff]

void ZipUp_MoveCode(GAMEOBJECT *object, i32 special_pressed) {
    ZIPUP_TRANSPORT_VIEW_PC *transport;
    ZIPUP_PC *zipup;
    NUVEC *hook;
    NUVEC *destination;
    i32 endpoint;
    i32 player_active;

    transport = (ZIPUP_TRANSPORT_VIEW_PC *)object;
    if (object->character_context == 0x47) {
        if (object->action_movement_state == 1) {
            NUVEC delta;
            f32 *playing;
            f32 fraction;
            f32 duration;

            playing = NULL;
            if (object->character_model->model_data_b[object->field_7aa] == NULL ||
                (playing = AnimPlaying(&object->anim_packet, object->field_7aa, 1, 0)) != NULL) {
                object->context_timer += FRAMETIME;
                if (object->context_timer >= object->context_duration)
                    object->context_timer = object->context_duration;
            }
            object->velocity = v000;
            switch (object->field_7b3) {
                case 0:
                    if (playing != NULL && 0.0f < *playing) {
                        f32 frame;

                        frame = AnimListFrame(object->character_model, object->field_7aa, 0);
                        if (frame > 1.0f && AnimEndFrame(object->character_model, object->field_7aa) > frame) {
                            f32 progress;

                            progress = (*playing - 1.0f) / (frame - 1.0f);
                            fraction = 1.0f < progress ? 1.0f : progress;
                            goto have_fraction;
                        }
                    }
                    fraction = object->context_timer / object->context_duration;
                have_fraction:
                    NuVecSub(&delta, &transport->start, &transport->entry);
                    NuVecScale(&delta, &delta, fraction);
                    NuVecAdd(&object->position, &transport->entry, &delta);
                    if (object->context_timer >= object->context_duration) {
                        object->field_7b3 = 1;
                        object->field_7aa = ZipUpSwingSwingAnimationPc;
                        object->context_timer = 0.0f;
                        duration = AnimDuration((i16)object->id, object->field_7aa, 0.0f, 0.0f, 0);
                        object->context_duration = duration;
                        if (duration <= 0.0f)
                            object->context_duration = 1.0f;
                        GameAudio_PlaySfx(0x4d, &object->collision_position, GameAudio_GetPlrSfxBits(object), 0);
                    }
                    break;
                case 1:
                    fraction = object->context_timer / object->context_duration;
                    NuVecSub(&delta, &transport->swing, &transport->start);
                    NuVecScale(&delta, &delta, fraction);
                    NuVecAdd(&object->position, &transport->start, &delta);
                    if (object->context_timer >= object->context_duration) {
                        object->field_7b3 = 2;
                        object->field_7aa = ZipUpSwingJumpAnimationPc;
                        object->context_timer = 0.0f;
                        duration = AnimDuration((i16)object->id, object->field_7aa, 0.0f, 0.0f, 0);
                        object->context_duration = duration;
                        if (duration <= 0.0f)
                            object->context_duration = 1.0f;
                        PlayJumpSfx(object, 0);
                    }
                    break;
                case 2:
                    fraction = object->context_timer / object->context_duration;
                    NuVecSub(&delta, &transport->landing, &transport->swing);
                    NuVecScale(&delta, &delta, fraction);
                    NuVecAdd(&object->position, &transport->swing, &delta);
                    if (object->context_timer >= object->context_duration) {
                        object->velocity.x = 0.0f;
                        object->velocity.y = -2.0f;
                        object->velocity.z = 0.0f;
                        object->character_context = -1;
                        StartFallLand(object, BigJumpLandFallbackPc);
                    }
                    break;
            }
            return;
        }
        zipup = (ZIPUP_PC *)object->grapple;
        hook = &zipup->hook_origin;
        destination = object->field_7b3 == 0 ? &zipup->upper_position : &zipup->lower_position;
        if ((zipup->flags_word & 1) == 0) {
            if (NuVecDistSqr(&object->position, hook, NULL) < 0.01f || object->context_timer >= 5.0f) {
                StartJump(object, 6);
                PlaySfx(GrapDetach_text, &object->collision_position);
                if ((object->flags_1fc & 0x80) != 0)
                    Hint_SetComplete(0x262);
                object->field_0x276 = object->movement_facing_angle = object->facing_angle =
                    NuAtan2D(destination->x - object->position.x, destination->z - object->position.z);
                object->airborne_action_timer = NuVecXZDist(&object->position, destination, NULL) /
                                                object->character_data->game_character->jump_duration;
                object->velocity.x = NU_SIN_LUT(object->movement_facing_angle) * object->airborne_action_timer;
                object->velocity.z = NU_COS_LUT(object->movement_facing_angle) * object->airborne_action_timer;
                object->context_variant_flags &= ~0x20u;
                ((ZIPUP_PC *)object->grapple)->flags_word &= ~0x100u;
                object->grapple = NULL;
                return;
            }
        }
        zipup = (ZIPUP_PC *)object->grapple;
        if ((zipup->flags_word & 0x40) == 0 || (zipup->flags_word & 0x80) == 0 || (zipup->flags_word & 0x100) == 0 ||
            (object->context_variant_flags & 0x20) == 0) {
            object->context_variant_flags &= ~0x20u;
            object->character_context = -1;
            zipup->flags_word &= ~0x100u;
            object->grapple = NULL;
            return;
        }
        if ((zipup->flags_word & 1) != 0) {
            if (object->context_timer >= ZipUpSwingDurationPc) {
                object->context_timer = 0.0f;
                object->character_context = -1;
                ((ZIPUP_PC *)object->grapple)->occupant = NULL;
                object->context_variant_flags &= ~0x20u;
                ((ZIPUP_PC *)object->grapple)->flags_word &= ~0x100u;
                transport->detach_fall_delay = 0.2f;
                object->grapple = NULL;
                object->field_7a4 = 0;
                object->target_velocity = v000;
                object->target_velocity.y = -1.0f;
                object->velocity = v000;
                object->velocity.y = -1.0f;
                PlaySfx(GrapDetach_text, &object->collision_position);
                if ((object->flags_1fc & 0x80) != 0)
                    Hint_SetComplete(0x262);
                return;
            }
            zipup->flags_word |= 0x200;
            if ((zipup->flags_word & 0x200) != 0) {
                NUVEC end_offset;
                NUVEC offset;
                u16 start_angle;
                u16 end_angle;
                i32 difference;
                u16 half;
                u16 axis;
                f32 normalized_time;
                f64 phase;
                i32 swing_pitch;

                start_angle = NuAtan2D(zipup->rider_start_offset.x, zipup->rider_start_offset.z);
                NuVecSub(&end_offset, destination, hook);
                end_offset.y += ZipUpRiderHeightPc;
                end_angle = NuAtan2D(end_offset.x, end_offset.z);
                difference = abs(0x8000 - (u16)RotDiff(start_angle, end_angle));
                half = (u16)(difference * 0.5);
                axis = start_angle > 0x8000 ? start_angle + half : start_angle - half;
                offset = zipup->rider_start_offset;
                NuVecRotateY(&offset, &offset, (u16)-axis);
                normalized_time = object->context_timer / ZipUpSwingDurationPc;
                phase = normalized_time * 32768.0;
                swing_pitch =
                    (i32)((1.0f - (NU_SIN_LUT((i32)(phase + 16384.0)) + 1.0f) * 0.5) * zipup->pitch_adjustment);
                NuVecRotateX(&zipup->rider_target_position, &offset, swing_pitch);
                NuVecRotateY(&zipup->rider_target_position, &zipup->rider_target_position, axis);
                zipup->rider_target_position.y *= 1.0f - (1.0f - NuTrigTable[0x1000]) * NU_SIN_LUT((i32)phase);
                NuVecAdd(&zipup->rider_target_position, hook, &zipup->rider_target_position);
            }
            {
                f32 old_time;
                f32 threshold;

                old_time = object->context_timer;
                object->context_timer += FRAMETIME;
                threshold = (f32)(ZipUpSwingDurationPc * 0.5 - (f64)0.2f);
                if (old_time < threshold && object->context_timer >= threshold)
                    PlaySfx(GrapSwing_text, &object->collision_position);
            }
        } else {
            object->context_timer += FRAMETIME;
            PlaySfx((char *)PlayerGrappleWindSoundPc, &object->collision_position);
        }
        return;
    }
    if (!ObjLandReady(object) && !objInNetWaitContext(object, 0x47))
        return;
    player_active = (object->flags_1fc_word & 0x80) != 0;
    if (!player_active && object->field_f00 != 4)
        return;
    if ((object->character_data->model_flags & 0x100000) == 0) {
        if (!player_active)
            return;
        if (!SuperWeirdo(object)) {
            if ((object->character_data->model_flags & 8) == 0 ||
                (object->character_data->game_character->flags_094 & 0x8000) != 0 || !Cheat_IsOn(13))
                return;
        }
    }
    zipup = ZipUp_FindNearest(WORLD, &object->lower_position, object->collision_radius, NULL, &endpoint, object);
    if (objInNetWaitContext(object, 0x47)) {
        object->context_timer -= FRAMETIME;
        if (object->context_timer <= 0.0f)
            object->character_context = -1;
    }
    if (zipup == NULL)
        return;
    if ((special_pressed != 0 || objInNetWaitContext(object, 0x47)) &&
        ((zipup->flags_word & 1) == 0 || (zipup->flags_word & 0x100) == 0)) {
        if ((object->character_data->model_flags & 0x100000) == 0) {
            destination = endpoint == 0 ? &zipup->upper_position : &zipup->lower_position;
            if (StartBigJump(object, destination, 0, 0.5f, 1.0f, 0))
                object->context_variant_flags |= 0x100;
            return;
        }
        Hint_SetComplete(0x262);
        object->grapple = zipup;
        object->character_context = 0x47;
        zipup->occupant = object;
        object->context_variant_flags |= 0x20;
        ((ZIPUP_PC *)object->grapple)->flags_word |= 0x100;
        if ((object->character_data->game_character->flags_094 & 0x20000000) != 0) {
            NUVEC offset;
            f32 duration;
            ZIPUP_PC *current;

            object->action_movement_state = 1;
            SetWeaponIn(object);
            GameAudio_PlaySfx(0x4c, &object->collision_position, GameAudio_GetPlrSfxBits(object), 0);
            object->field_7b3 = 0;
            object->field_7aa = ZipUpSwingStartAnimationPc;
            object->context_timer = 0.0f;
            duration = AnimDuration((i16)object->id, object->field_7aa, 0.0f, 0.0f, 0);
            object->context_duration = duration;
            if (duration <= 0.0f)
                object->context_duration = 0.5f;
            transport->entry = object->position;
            current = (ZIPUP_PC *)object->grapple;
            transport->start = current->endpoints[endpoint != 0 ? 2 : 0];
            transport->landing = current->endpoints[endpoint != 0 ? 0 : 2];
            transport->landing.y = endpoint == 0 ? zipup->upper_ground_height : zipup->lower_ground_height;
            NuVecSub(&offset, &transport->landing, &transport->start);
            offset.y = 0.0f;
            NuVecNorm(&offset, &offset);
            NuVecScale(&offset, &offset, ZipUpSwingOffsetLengthPc);
            NuVecAdd(&transport->swing, &transport->start, &offset);
            transport->swing.y = transport->swing.y + ZipUpSwingOffsetHeightPc;
            object->movement_facing_angle =
                NuAtan2D(transport->landing.x - transport->start.x, transport->landing.z - transport->start.z);
            object->field_281 = 0;
        } else {
            NUVEC end_offset;
            NUVEC start_offset;
            NUVEC *start;
            NUVEC *heading;
            f32 dx, dz;
            i32 angle;
            i32 pitch;
            i32 yaw;
            f32 rider_height;
            f32 scale;

            zipup = (ZIPUP_PC *)object->grapple;
            object->action_movement_state = 0;
            object->field_7aa = 0x2a;
            hook = &zipup->hook_origin;
            if (endpoint == 0) {
                object->field_7b3 = 0;
                start = &zipup->lower_position;
                destination = &zipup->upper_position;
            } else {
                object->field_7b3 = 1;
                start = &zipup->upper_position;
                destination = &zipup->lower_position;
            }
            heading = hook->x == start->x && hook->z == start->z ? destination : hook;
            object->movement_facing_angle = NuAtan2D(heading->x - start->x, heading->z - start->z);
            object->velocity.y = 0.0f;
            if (object->character_data->game_character->hover_fall_mode == 2)
                FastWeaponOut(object, 1);
            object->context_timer = 0.0f;
            PlaySfx(GrapAttach_text, &object->upper_position);
            FastWeaponOut(object, 0);
            object->jump_variant_state = 0;
            dx = hook->x - start->x;
            dz = hook->z - start->z;
            angle = NuAtan2D(hook->y - start->y, NuFsqrt(dx * dx + dz * dz));
            pitch = 0x4000 - abs(angle);
            object->field_7a4 = angle < 0 ? -pitch : pitch;
            zipup = (ZIPUP_PC *)object->grapple;
            if ((zipup->flags_word & 1) == 0)
                return;
            NuVecSub(&zipup->rider_start_offset, start, hook);
            NuVecSub(&end_offset, destination, hook);
            rider_height = ZipUpRiderHeightPc;
            scale = ZipUpRiderOffsetScalePc;
            end_offset.y += rider_height;
            ((ZIPUP_PC *)object->grapple)->rider_start_offset.x *= scale;
            ((ZIPUP_PC *)object->grapple)->rider_start_offset.y *= scale;
            ((ZIPUP_PC *)object->grapple)->rider_start_offset.y =
                rider_height * object->scaled_height + ((ZIPUP_PC *)object->grapple)->rider_start_offset.y;
            ((ZIPUP_PC *)object->grapple)->rider_start_offset.z *= scale;
            yaw = -(u16)NuAtan2D(((ZIPUP_PC *)object->grapple)->rider_start_offset.x,
                                 ((ZIPUP_PC *)object->grapple)->rider_start_offset.z);
            start_offset = ((ZIPUP_PC *)object->grapple)->rider_start_offset;
            NuVecRotateY(&start_offset, &start_offset, yaw);
            NuVecRotateY(&end_offset, &end_offset, yaw);
            NuVecNorm(&start_offset, &start_offset);
            NuVecNorm(&end_offset, &end_offset);
            ((ZIPUP_PC *)object->grapple)->pitch_adjustment =
                (u16)(0x4000 - ZipUpArcSinePc(start_offset.z * end_offset.z + start_offset.y * end_offset.y));
            ((ZIPUP_PC *)object->grapple)->rider_target_position = ((ZIPUP_PC *)object->grapple)->rider_start_offset;
            NuVecAdd(&((ZIPUP_PC *)object->grapple)->rider_target_position, hook,
                     &((ZIPUP_PC *)object->grapple)->rider_target_position);
        }
    } else {
        SetHeadTarget(object, &zipup->hook_origin, 4, 2.0f, 1.0f, 2.0f);
    }
}

extern const NUVEC ZipUpHookOffsetPc;
extern i32 CanDrawZipUpSwirls;

typedef struct zipup_progress_pc_s {
    u32 active_mask, visible_mask;
} ZIPUP_PROGRESS_PC;
STATIC_ASSERT(sizeof(ZIPUP_PROGRESS_PC) == 8);
STATIC_ASSERT(offsetof(ZIPUP_PROGRESS_PC, active_mask) == 0);
STATIC_ASSERT(offsetof(ZIPUP_PROGRESS_PC, visible_mask) == 4);

static void ZipUpResetSinglePc(ZIPUP_PC *zipup) {
    NuVecRotateX(&zipup->hook_position, (NUVEC *)&ZipUpHookOffsetPc, zipup->hook_x_rotation);
    NuVecRotateY(&zipup->hook_position, &zipup->hook_position, zipup->hook_y_rotation);
    NuVecAdd(&zipup->hook_position, &zipup->hook_origin, &zipup->hook_position);
    NewTerrPlatformsOff();
    zipup->lower_ground_height = GameShadow(NULL, &zipup->lower_position, 5.0f, -1);
    if (zipup->lower_ground_height != 2000000.0) {
        zipup->lower_ground_height += (f64)0.005f;
        FindAnglesZX(&ShadNorm, &zipup->lower_ground_x_rotation, &zipup->lower_ground_z_rotation);
    }
    NewTerrPlatformsOff();
    zipup->upper_ground_height = GameShadow(NULL, &zipup->upper_position, 5.0f, -1);
    if (zipup->upper_ground_height != 2000000.0) {
        zipup->upper_ground_height += (f64)0.005f;
        FindAnglesZX(&ShadNorm, &zipup->upper_ground_x_rotation, &zipup->upper_ground_z_rotation);
    }
    zipup->facing_angle =
        NuAtan2D(zipup->upper_position.x - zipup->lower_position.x, zipup->upper_position.z - zipup->lower_position.z);
    zipup->occupant = NULL;
    zipup->flags_word = (zipup->flags_word & ~0x100) | 0xc0;
}
extern void ZipUps_Reset(WORLDINFO *world, void *unused, ZIPUP_PROGRESS_PC *progress) {
    ZIPUP_PC *zipup;

    (void)unused;
    if (world == NULL)
        return;
    zipup = world->zipups;
    if (zipup != NULL) {
        i32 index;

        for (index = 0; index < WORLD->zipup_count; ++index, ++zipup) {
            ZipUpResetSinglePc(zipup);
            if (progress != NULL && index < 32) {
                u32 bit;
                u16 flag;

                bit = 1u << index;
                flag = (progress->visible_mask & bit) != 0;
                zipup->flags_word ^= (zipup->flags_word ^ (flag << 7)) & 0x80;
                flag = (progress->active_mask & bit) != 0;
                zipup->flags_word ^= (zipup->flags_word ^ (flag << 6)) & 0x40;
            }
        }
    }
}
extern void ZipUps_Update(WORLDINFO *world, void *unused, f32 time) {
    ZIPUP_PC *zipup;
    i32 index;

    (void)unused;
    (void)time;
    if (world == NULL)
        return;
    CanDrawZipUpSwirls = 0;
    zipup = world->zipups;
    if (zipup == NULL)
        return;
    CanDrawZipUpSwirls = AvailableToPlayer(0x100000, -1, 0, 0);

    for (index = 0; index < world->zipup_count; ++index, ++zipup) {
        GAMEOBJECT *occupant;

        occupant = zipup->occupant;
        if (occupant != NULL && occupant->character_context == -1) {
            occupant->context_variant_flags &= ~0x20;
            occupant->grapple = NULL;
            zipup->flags_word &= ~0x100;
            zipup->occupant = NULL;
        }
    }
}

enum ZIPUP_FLAGS {
    ZIPUP_FLAG_CONFIG_0 = 1,
    ZIPUP_FLAG_CONFIG_1 = 2,
    ZIPUP_FLAG_CONFIG_2 = 4,
    ZIPUP_FLAG_CONFIG_3 = 8,
    ZIPUP_FLAG_CONFIG_4 = 0x10,
    ZIPUP_FLAG_CONFIG_5 = 0x20,
    ZIPUP_FLAG_ACTIVE = 0x40,
    ZIPUP_FLAG_VISIBLE = 0x80
};

extern f32 ZipUpMarkerOffsetYPc, ZipUpMarkerOffsetZPc;
extern NUMTL *ropemtl;
extern f32 ZipUpRopeHalfWidthPc;
extern u32 ZipUpRopeCheatColourPc;
extern i32 ZipUpRopeModulationPc;
extern i16 ZipUpLineCharacterIdPc;
extern NUMTL *ZipUpLineMaterialPc;

typedef struct zipup_rope_vertex_pc_s {
    NUVEC position, normal;
    u32 colour;
    f32 u, v;
} ZIPUP_ROPE_VERTEX_PC;
STATIC_ASSERT(sizeof(ZIPUP_ROPE_VERTEX_PC) == 36);
STATIC_ASSERT(offsetof(ZIPUP_ROPE_VERTEX_PC, normal) == 12);
STATIC_ASSERT(offsetof(ZIPUP_ROPE_VERTEX_PC, colour) == 24);
STATIC_ASSERT(offsetof(ZIPUP_ROPE_VERTEX_PC, u) == 28);
STATIC_ASSERT(offsetof(ZIPUP_ROPE_VERTEX_PC, v) == 32);
typedef struct zipup_rope_material_pc_s {
    u8 unknown_0[0x70];
    f32 opacity;
} ZIPUP_ROPE_MATERIAL_PC;
STATIC_ASSERT(offsetof(ZIPUP_ROPE_MATERIAL_PC, opacity) == 0x70);

static void ZipUp_GetStartPoint(GAMEOBJECT *object, NUVEC *position) {
    i32 joint;

    PLAYERCHARACTERCONFIG_PC *character = object->character_data->game_character;
    joint = character->grapple_locators[0];
    if (joint != -1 && object->character_model->points_of_interest[joint] != NULL && object->origin_joint_ready != 0) {
        *position = object->locator_matrices[joint].translation;
        return;
    }
    joint = character->weapon_shoot_joints[0];
    if (joint != -1 && object->character_model->points_of_interest[joint] != NULL && object->origin_joint_ready != 0) {
        *position = object->locator_matrices[joint].translation;
        return;
    }
    joint = character->weapon_joints[0];
    if (joint != -1 && object->character_model->points_of_interest[joint] != NULL && object->origin_joint_ready != 0) {
        *position = object->locator_matrices[joint].translation;
        return;
    }
    position->x = object->collision_position.x;
    position->y = object->collision_position.y + object->field_b0;
    position->z = object->collision_position.z;
}

static void ZipUpDrawMarkerPc(ZIPUP_PC *zipup, i32 endpoint, WORLDINFO *world, i32 special, u16 angle, f32 alpha) {
    NUMTX matrix;
    NUVEC position;
    i32 upper;

    if (world->level_objects[special].active == 0)
        return;

    upper = endpoint != 0;
    NuMtxSetRotationY(&matrix, angle);
    NuMtxRotateZ(&matrix, upper ? zipup->upper_ground_z_rotation : zipup->lower_ground_z_rotation);
    NuMtxRotateX(&matrix, upper ? zipup->upper_ground_x_rotation : zipup->lower_ground_x_rotation);
    position.x = zipup->endpoints[endpoint].x;
    position.y = upper ? zipup->upper_ground_height : zipup->lower_ground_height;
    position.z = zipup->endpoints[endpoint].z;
    NuMtxTranslate(&matrix, &position);
    NuSpecialDrawAtAlpha(&world->level_objects[special].special, &matrix, alpha);
}

static void ZipUps_Draw(WORLDINFO *world, void *unused, f32 time) {
    enum ZIPUP_SPECIALS {
        ZIPUP_SPECIAL_HOOK = 83,
        ZIPUP_SPECIAL_GROUND_SWIRL = 85,
        ZIPUP_SPECIAL_GROUND_MARKER = 86,
        ZIPUP_SPECIAL_ENDPOINT_SWIRL = 229,
    };
    enum ZIPUP_DRAW_MATRICES {
        ZIPUP_MATRIX_HOOK,
        ZIPUP_MATRIX_LOWER_MARKER,
        ZIPUP_MATRIX_UPPER_MARKER,
        ZIPUP_MATRIX_LOWER_SWIRL,
        ZIPUP_MATRIX_UPPER_SWIRL,
        ZIPUP_MATRIX_COUNT,
    };

    if (world != NULL && world->zipups != NULL) {
        u16 spin_angle;
        f32 pulse_phase;
        f32 pulse_alpha;
        NUVEC endpoint_position;
        NUVEC offset;
        NUVEC ground_position;
        NUMTX matrices[ZIPUP_MATRIX_COUNT];
        ZIPUP_PC *zipup;
        i32 index;

        spin_angle = ((u16)(NuFmod(GameTimer.time_elapsed, 5.0f) / 5.0f * 65536.0f));
        pulse_phase = NuFmod(GameTimer.time_elapsed_mod_seconds, 0.5f) * 2.0f * 65536.0f;
        pulse_alpha = NuTrigTable[(((i32)(pulse_phase)) >> 1) & 0x7fff] * 0.2f + 0.8f;

        zipup = world->zipups;

        for (index = 0; index < world->zipup_count; ++index, ++zipup) {
            if ((zipup->flags & (ZIPUP_FLAG_ACTIVE | ZIPUP_FLAG_VISIBLE)) != (ZIPUP_FLAG_ACTIVE | ZIPUP_FLAG_VISIBLE)) {
                continue;
            }

            if ((zipup->flags & ZIPUP_FLAG_CONFIG_5) != 0) {
                if (world->level_objects[ZIPUP_SPECIAL_HOOK].active != 0) {
                    offset.x = 0.0f;
                    offset.y = ZipUpMarkerOffsetYPc;
                    offset.z = ZipUpMarkerOffsetZPc;

                    NuVecRotateY(&offset, &offset, zipup->facing_angle);
                    NuVecAdd(&endpoint_position, &zipup->lower_position, &offset);
                    NuMtxSetTranslation(&matrices[ZIPUP_MATRIX_HOOK], &endpoint_position);
                    NuSpecialDrawAt(&world->level_objects[ZIPUP_SPECIAL_HOOK].special, &matrices[ZIPUP_MATRIX_HOOK]);

                    if ((zipup->flags & ZIPUP_FLAG_CONFIG_3) != 0) {
                        NuVecRotateY(&offset, &offset, 0x8000);
                        NuVecAdd(&endpoint_position, &zipup->upper_position, &offset);
                        NuMtxSetTranslation(&matrices[ZIPUP_MATRIX_HOOK], &endpoint_position);
                        NuSpecialDrawAt(&world->level_objects[ZIPUP_SPECIAL_HOOK].special,
                                        &matrices[ZIPUP_MATRIX_HOOK]);
                    }
                }

                ZipUpDrawMarkerPc(zipup, 0, world, ZIPUP_SPECIAL_GROUND_MARKER, spin_angle, pulse_alpha);
                if ((zipup->flags & 8) != 0)
                    ZipUpDrawMarkerPc(zipup, 2, world, ZIPUP_SPECIAL_GROUND_MARKER, spin_angle, pulse_alpha);
                continue;
            }

            if ((zipup->flags & ZIPUP_FLAG_CONFIG_4) != 0 && world->level_objects[ZIPUP_SPECIAL_HOOK].active != 0) {
                EnableShadowMapRendering(0);
                NuMtxSetRotationX(&matrices[ZIPUP_MATRIX_HOOK], zipup->hook_x_rotation);
                NuMtxRotateY(&matrices[ZIPUP_MATRIX_HOOK], zipup->hook_y_rotation);
                NuMtxTranslate(&matrices[ZIPUP_MATRIX_HOOK], &zipup->hook_origin);
                NuSpecialDrawAt(&world->level_objects[ZIPUP_SPECIAL_HOOK].special, &matrices[ZIPUP_MATRIX_HOOK]);
                ResetShadowMapRendering();
            }

            if (CanDrawZipUpSwirls == 0 || (zipup->flags & ZIPUP_FLAG_CONFIG_1) == 0 ||
                world->level_objects[ZIPUP_SPECIAL_ENDPOINT_SWIRL].active == 0) {
                continue;
            }

            NuMtxSetTranslation(&matrices[ZIPUP_MATRIX_HOOK], &zipup->lower_position);
            NuSpecialDrawAtAlpha(&world->level_objects[ZIPUP_SPECIAL_ENDPOINT_SWIRL].special,
                                 &matrices[ZIPUP_MATRIX_HOOK], 1.0f);

            if ((zipup->flags & ZIPUP_FLAG_CONFIG_3) != 0) {
                NuMtxSetTranslation(&matrices[ZIPUP_MATRIX_HOOK], &zipup->upper_position);
                NuSpecialDrawAtAlpha(&world->level_objects[ZIPUP_SPECIAL_ENDPOINT_SWIRL].special,
                                     &matrices[ZIPUP_MATRIX_HOOK], 1.0f);
            }

            if ((zipup->flags & ZIPUP_FLAG_CONFIG_2) == 0) {
                continue;
            }

            ZipUpDrawMarkerPc(zipup, 0, world, ZIPUP_SPECIAL_GROUND_SWIRL, spin_angle, pulse_alpha);
            if ((zipup->flags & 8) != 0)
                ZipUpDrawMarkerPc(zipup, 2, world, ZIPUP_SPECIAL_GROUND_SWIRL, spin_angle, pulse_alpha);
        }
    }
}

void InitRopeMtl(char *name, VARIPTR *buffer, VARIPTR *buffer_end) {
    char path[64] = "stuff\\";
    if (name == NULL) {
        ropemtl = NULL;
        return;
    }
    ropemtl = NuMtlCreate3D(1);
    ((ZIPUP_ROPE_MATERIAL_PC *)ropemtl)->opacity = 1.0f;
    ropemtl->attribs.unknown_2_1_2 = 0;
    ropemtl->attribs.unknown_1_1_2 = 0;
    ropemtl->attribs.unknown_1_4_8 = 0;
    ropemtl->attribs.z_mode = 0;
    ropemtl->attribs.alpha_mode = 0;
    ropemtl->attribs.filter_mode = 1;
    NuStrCat(path, name);
    buffer->addr = ((buffer->addr + 15) & ~15u);
    ropemtl->texture_id = NuTexReadPc(path, buffer, *buffer_end);
    if (ropemtl->texture_id == 0) {
        NuMtlDestroy(ropemtl);
        ropemtl = NULL;
    } else {
        NuMtlUpdate(ropemtl);
    }
}

// The retail rope draw inlines these numtx rotations (other units call the
// exported versions), so this unit carries local copies of their bodies.
static void RopeMtxSetRotationZ(NUMTX *m, NUANG a) {
    m->m00 = m->m11 = NU_COS_LUT(a);
    m->m01 = NU_SIN_LUT(a);
    m->m10 = -m->m01;
    m->m02 = m->m12 = m->m03 = m->m13 = 0.0f;
    m->m20 = ANI3_EVAL_IDENTITY_MATRIX.m20;
    m->m21 = ANI3_EVAL_IDENTITY_MATRIX.m21;
    m->m22 = ANI3_EVAL_IDENTITY_MATRIX.m22;
    m->m23 = ANI3_EVAL_IDENTITY_MATRIX.m23;
    m->m30 = ANI3_EVAL_IDENTITY_MATRIX.m30;
    m->m31 = ANI3_EVAL_IDENTITY_MATRIX.m31;
    m->m32 = ANI3_EVAL_IDENTITY_MATRIX.m32;
    m->m33 = ANI3_EVAL_IDENTITY_MATRIX.m33;
}

static void RopeMtxRotateX(NUMTX *m, NUANG a) {
    f32 cosx;
    f32 sinx;
    f32 m01;
    f32 m11;
    f32 m21;
    f32 m31;

    cosx = NU_COS_LUT(a);
    sinx = NU_SIN_LUT(a);
    m01 = m->m01;
    m11 = m->m11;
    m21 = m->m21;
    m31 = m->m31;

    m->m01 = m01 * cosx - m->m02 * sinx;
    m->m02 = m01 * sinx + m->m02 * cosx;
    m->m11 = m11 * cosx - m->m12 * sinx;
    m->m12 = m11 * sinx + m->m12 * cosx;
    m->m21 = m21 * cosx - m->m22 * sinx;
    m->m22 = m21 * sinx + m->m22 * cosx;
    m->m31 = m31 * cosx - m->m32 * sinx;
    m->m32 = m31 * sinx + m->m32 * cosx;
}

static void RopeMtxRotateY(NUMTX *m, NUANG a) {
    f32 cosx;
    f32 sinx;
    f32 m00;
    f32 m10;
    f32 m20;
    f32 m30;

    cosx = NU_COS_LUT(a);
    sinx = NU_SIN_LUT(a);
    m00 = m->m00;
    m10 = m->m10;
    m20 = m->m20;
    m30 = m->m30;

    m->m00 = m00 * cosx + m->m02 * sinx;
    m->m02 = m->m02 * cosx - m00 * sinx;
    m->m10 = m10 * cosx + m->m12 * sinx;
    m->m12 = m->m12 * cosx - m10 * sinx;
    m->m20 = m20 * cosx + m->m22 * sinx;
    m->m22 = m->m22 * cosx - m20 * sinx;
    m->m30 = m30 * cosx + m->m32 * sinx;
    m->m32 = m->m32 * cosx - m30 * sinx;
}

static void RopeMtxRotateZ(NUMTX *m, NUANG a) {
    f32 cosx;
    f32 sinx;
    f32 m00;
    f32 m10;
    f32 m20;
    f32 m30;

    cosx = NU_COS_LUT(a);
    sinx = NU_SIN_LUT(a);
    m00 = m->m00;
    m10 = m->m10;
    m20 = m->m20;
    m30 = m->m30;

    m->m00 = m00 * cosx - m->m01 * sinx;
    m->m01 = m00 * sinx + m->m01 * cosx;
    m->m10 = m10 * cosx - m->m11 * sinx;
    m->m11 = m10 * sinx + m->m11 * cosx;
    m->m20 = m20 * cosx - m->m21 * sinx;
    m->m21 = m20 * sinx + m->m21 * cosx;
    m->m30 = m30 * cosx - m->m31 * sinx;
    m->m31 = m30 * sinx + m->m31 * cosx;
}

void DrawRopeSingle(NUVEC *start, NUVEC *end, float amount, NUMTL *material, float time, float grow_time, float spacing,
                    float scale) {
    NUVEC direction;
    f32 length;
    f32 repeats;
    ZIPUP_ROPE_VERTEX_PC vertices[10];
    u16 x_rotation, z_rotation;
    NUMTX matrix;
    NUVEC position;
    NUVEC size;
    f32 step;
    u16 rotation;
    i32 segment_index;
    i32 segment_count;

    static f32 ROPELEN = 0;
    static u32 ropedif = 0xff505050;
    ROPELEN = ZipUpRopeHalfWidthPc * 4.0;
    ropedif = Cheat_IsOn(3) ? ZipUpRopeCheatColourPc : 0xff505050;
    if (material == NULL)
        material = ropemtl;
    if (start == NULL || end == NULL)
        return;
    amount = 0.0 > MIN(1.0f, amount) ? 0.0f : MIN(1.0f, amount);
    direction.x = end->x - start->x;
    direction.y = end->y - start->y;
    direction.z = end->z - start->z;
    length = NuVecMag(&direction) * amount;
    repeats = length / ROPELEN;

    vertices[0].position.x = -ZipUpRopeHalfWidthPc;
    vertices[0].position.y = 0.0f;
    vertices[0].position.z = ZipUpRopeHalfWidthPc;
    vertices[0].normal.x = -0.7071068286895752f;
    vertices[0].normal.y = 0.0f;
    vertices[0].normal.z = 0.7071068286895752f;
    vertices[0].colour = ropedif;
    vertices[0].u = 0.0f;
    vertices[0].v = 0.0f;
    vertices[1].position.x = -ZipUpRopeHalfWidthPc;
    vertices[1].position.y = length;
    vertices[1].position.z = ZipUpRopeHalfWidthPc;
    vertices[1].normal.x = -0.7071068286895752f;
    vertices[1].normal.y = 0.0f;
    vertices[1].normal.z = 0.7071068286895752f;
    vertices[1].colour = ropedif;
    vertices[1].u = repeats;
    vertices[1].v = 0.0f;
    vertices[2].position.x = ZipUpRopeHalfWidthPc;
    vertices[2].position.y = 0.0f;
    vertices[2].position.z = ZipUpRopeHalfWidthPc;
    vertices[2].normal.x = 0.7071068286895752f;
    vertices[2].normal.y = 0.0f;
    vertices[2].normal.z = 0.7071068286895752f;
    vertices[2].colour = ropedif;
    vertices[2].u = 0.0f;
    vertices[2].v = 1.0f;
    vertices[3].position.x = ZipUpRopeHalfWidthPc;
    vertices[3].position.y = length;
    vertices[3].position.z = ZipUpRopeHalfWidthPc;
    vertices[3].normal.x = 0.7071068286895752f;
    vertices[3].normal.y = 0.0f;
    vertices[3].normal.z = 0.7071068286895752f;
    vertices[3].colour = ropedif;
    vertices[3].u = repeats;
    vertices[3].v = 1.0f;
    vertices[4].position.x = ZipUpRopeHalfWidthPc;
    vertices[4].position.y = 0.0f;
    vertices[4].position.z = -ZipUpRopeHalfWidthPc;
    vertices[4].normal.x = 0.7071068286895752f;
    vertices[4].normal.y = 0.0f;
    vertices[4].normal.z = -0.7071068286895752f;
    vertices[4].colour = ropedif;
    vertices[4].u = 0.0f;
    vertices[4].v = 2.0f;
    vertices[5].position.x = ZipUpRopeHalfWidthPc;
    vertices[5].position.y = length;
    vertices[5].position.z = -ZipUpRopeHalfWidthPc;
    vertices[5].normal.x = 0.7071068286895752f;
    vertices[5].normal.y = 0.0f;
    vertices[5].normal.z = -0.7071068286895752f;
    vertices[5].colour = ropedif;
    vertices[5].u = repeats;
    vertices[5].v = 2.0f;
    vertices[6].position.x = -ZipUpRopeHalfWidthPc;
    vertices[6].position.y = 0.0f;
    vertices[6].position.z = -ZipUpRopeHalfWidthPc;
    vertices[6].normal.x = -0.7071068286895752f;
    vertices[6].normal.y = 0.0f;
    vertices[6].normal.z = -0.7071068286895752f;
    vertices[6].colour = ropedif;
    vertices[6].u = 0.0f;
    vertices[6].v = 3.0f;
    vertices[7].position.x = -ZipUpRopeHalfWidthPc;
    vertices[7].position.y = length;
    vertices[7].position.z = -ZipUpRopeHalfWidthPc;
    vertices[7].normal.x = -0.7071068286895752f;
    vertices[7].normal.y = 0.0f;
    vertices[7].normal.z = -0.7071068286895752f;
    vertices[7].colour = ropedif;
    vertices[7].u = repeats;
    vertices[7].v = 3.0f;
    vertices[8].position.x = -ZipUpRopeHalfWidthPc;
    vertices[8].position.y = 0.0f;
    vertices[8].position.z = ZipUpRopeHalfWidthPc;
    vertices[8].normal.x = -0.7071068286895752f;
    vertices[8].normal.y = 0.0f;
    vertices[8].normal.z = 0.7071068286895752f;
    vertices[8].colour = ropedif;
    vertices[8].u = 0.0f;
    vertices[8].v = 4.0f;
    vertices[9].position.x = -ZipUpRopeHalfWidthPc;
    vertices[9].position.y = length;
    vertices[9].position.z = ZipUpRopeHalfWidthPc;
    vertices[9].normal.x = -0.7071068286895752f;
    vertices[9].normal.y = 0.0f;
    vertices[9].normal.z = 0.7071068286895752f;
    vertices[9].colour = ropedif;
    vertices[9].u = repeats;
    vertices[9].v = 4.0f;

    FindAnglesZX(&direction, &x_rotation, &z_rotation);

    RopeMtxSetRotationZ(&matrix, z_rotation);
    RopeMtxRotateX(&matrix, x_rotation);
    NuMtxTranslate(&matrix, start);
    NuRndrTriStrip3dClip((NURND_VERTEX3D_PC *)vertices, 10, &matrix, material);
    if (Cheat_IsOn(3) == 0 || VehicleArea != 0)
        return;
    step = ROPELEN * spacing;
    position = v000;
    size = v000;
    rotation = 0;
    segment_count = (i32)(length / step);
    position.y = 0.0f;
    segment_index = 0;
    while (position.y < length) {
        f32 magnitude;

        if (ZipUpRopeModulationPc == 1 && segment_count > 0) {
            f32 fraction;
            i32 angle;

            fraction = (f32)(1.0 - (f64)segment_index / segment_count);
            angle = (i32)(fraction * 16384.0 + 49152.0 + 16384.0);
            magnitude = scale * NuTrigTable[(angle >> 1) & 0x7fff];
        } else {
            magnitude = scale;
        }
        if (time <= grow_time) {
            i32 angle;

            angle = (i32)((1.0f / grow_time * time) * 16384.0f + 32768.0f + 16384.0f);
            magnitude = (1.0f + NuTrigTable[(angle >> 1) & 0x7fff]) * magnitude;
        }
        size.x = size.y = size.z = magnitude;
        rotation = (u16)(rotation + 0x5555);
        NuMtxSetScale(&matrix, &size);
        NuMtxTranslate(&matrix, &position);
        RopeMtxRotateY(&matrix, rotation);
        RopeMtxRotateZ(&matrix, z_rotation);
        RopeMtxRotateX(&matrix, x_rotation);
        NuMtxTranslate(&matrix, start);
        NuSpecialDrawAt(&WORLD->level_objects[0x124].special, &matrix);
        NuSpecialDrawAt(&WORLD->level_objects[0x125].special, &matrix);
        NuSpecialDrawAt(&WORLD->level_objects[0x126].special, &matrix);
        position.y += step;
        ++segment_index;
    }
}

void ZipUps_DrawLines() {
    i32 i;

    GAMEOBJECT *object = Obj;

    for (i = 0; i < HIGHGAMEOBJECT; ++i, ++object) {
        ZIPUP_ROPE_VERTEX_PC vertices[3];
        ZIPUP_ROPE_VERTEX_PC *start, *end;

        start = &vertices[0];
        end = &vertices[1];
        if (object->character_context == 0x47) {
            if (object->action_movement_state == 0) {
                ZipUp_GetStartPoint(object, &start->position);
                start->colour = 0xffffffff;
                end->position = ((ZIPUP_PC *)(object->grapple))->hook_position;
                if (object->context_timer < (f64)0.2f) {
                    f32 fraction;

                    fraction = (f32)(object->context_timer / (f64)0.2f);
                    end->position.x = (end->position.x - start->position.x) * fraction + start->position.x;
                    end->position.y = (end->position.y - start->position.y) * fraction + start->position.y;
                    end->position.z = (end->position.z - start->position.z) * fraction + start->position.z;
                }
                end->colour = 0xff808080;
                DrawRopeSingle(&start->position, &end->position, 1.0f, ropemtl, object->context_timer, 0.2f, 3.5f,
                               1.0f);
            }
        } else if (object->character_context == 0x35) {
            start->colour = 0xffffffff;
            end->colour = 0xff808080;
            end->position = ((ZIPUP_TRANSPORT_VIEW_PC *)object)->entry;
            if ((u16)object->id == (u16)ZipUpLineCharacterIdPc) {
                start->position = object->position;
                NuRndrLine3d((NURND_VERTEX3D_PC *)start, ZipUpLineMaterialPc, NULL);
                continue;
            }
            ZipUp_GetStartPoint(object, &start->position);
            DrawRopeSingle(&start->position, &end->position, 1.0f, ropemtl, object->context_timer, 0.2f, 3.5f, 1.0f);
        }
    }
}

static i32 ZipUp_ActivateRev(GIZMO_PC *gizmo, i32 active, i32 query) {
    ZIPUP_PC *zipup;

    if (gizmo == NULL || gizmo->object == NULL)
        return 0;
    zipup = ((ZIPUP_PC *)(gizmo->object));
    if (query & 1)
        return zipup->direction != active;
    if (active != 0) {
        zipup->direction = 1;
        zipup->flags_word &= ~0x40;
    } else {
        zipup->direction = 0;
        zipup->flags_word |= 0x40;
    }
    return 1;
}
static void ZipUp_Activate(GIZMO_PC *gizmo, i32 active) {
    ZIPUP_PC *zipup;

    zipup = ((ZIPUP_PC *)(gizmo->object));
    if (active) {
        zipup->direction = 0;
        zipup->flags_word |= 0x40;
    } else
        zipup->flags_word &= ~0x40;
}
static void ZipUp_SetVisibility(GIZMO_PC *gizmo, i32 visible) {
    if (gizmo) {
        ZIPUP_PC *zipup;

        zipup = ((ZIPUP_PC *)(gizmo->object));
        zipup->flags_word = (zipup->flags_word & ~0x80) | ((visible != 0) << 7);
    }
}
static i32 ZipUps_GetMaxGizmos(void *world_ptr) {
    WORLDINFO *world;

    world = ((WORLDINFO *)(world_ptr));
    return world ? world->current_level->max_zipups : 0;
}
extern GIZMO_PC *__cdecl AddGizmo(void *, i32, char *, void *);
static void ZipUps_AddGizmos(void *system, i32 type_id, WORLDINFO *world) {
    i32 index;

    for (index = 0; index < world->zipup_count; ++index)
        if (NuStrLen(world->zipups[index].name) != 0)
            AddGizmo(system, type_id, NULL, &world->zipups[index]);
}
static char *ZipUp_GetGizmoName(GIZMO_PC *gizmo) {
    return gizmo ? ((ZIPUP_PC *)(gizmo->object))->name : NULL;
}
extern void *__cdecl GizmoBufferAlloc(VARIPTR *, VARIPTR *, i32);
static void *ZipUps_AllocateProgressData(VARIPTR *buffer, VARIPTR *end) {
    return GizmoBufferAlloc(buffer, end, sizeof(ZIPUP_PROGRESS_PC));
}
static void ZipUps_ClearProgress(void *unused, ZIPUP_PROGRESS_PC *progress) {
    if (progress) {
        progress->active_mask = ~0u;
        progress->visible_mask = ~0u;
    }
}
static void ZipUps_StoreProgress(WORLDINFO *world, void *unused, ZIPUP_PROGRESS_PC *progress) {
    if (!progress)
        return;
    progress->active_mask = ~0u;
    progress->visible_mask = ~0u;
    if (world && world->zipups) {
        i32 i;

        for (i = 0; i < world->zipup_count && i < 32; ++i) {
            u32 mask;

            mask = 1u << i;
            if (!(world->zipups[i].flags & 0x80))
                progress->visible_mask &= ~mask;
            if (!(world->zipups[i].flags & 0x40))
                progress->active_mask &= ~mask;
        }
    }
}
static i32 ZipUp_GetOutput(GIZMO_PC *gizmo, i32 output, i32 mode) {
    return gizmo != NULL && (((ZIPUP_PC *)(gizmo->object))->flags & 0x40) != 0;
}
static char ZipUpOutputNamePc[] = "Active";
static char *ZipUp_GetOutputName(GIZMO_PC *gizmo, i32 output) {
    return ZipUpOutputNamePc;
}
static i32 ZipUp_GetNumOutputs(GIZMO_PC *gizmo) {
    return 1;
}
extern char *ZipUpTypeNamePtrPc;
extern i32 zipup_gizmotype_id;
static ADDGIZMOTYPE_PC ZipUpAddTypePc = {0};
extern ADDGIZMOTYPE_PC *ZipUps_RegisterGizmo(i32 type_id) {
    ZipUpAddTypePc = Default_ADDGIZMOTYPE;
    ZipUpAddTypePc.name = ZipUpTypeNamePtrPc;
    ZipUpAddTypePc.prefix = "";
    ZipUpAddTypePc.fns.options = 8;
    ZipUpAddTypePc.fns.get_max_gizmos_fn = ZipUps_GetMaxGizmos;
    ZipUpAddTypePc.fns.add_gizmos_fn = (void *)ZipUps_AddGizmos;
    ZipUpAddTypePc.fns.early_update_fn = NULL;
    ZipUpAddTypePc.fns.late_update_fn = (void(__cdecl *)(void *, void *, f32))ZipUps_Update;
    ZipUpAddTypePc.fns.draw_fn = (void(__cdecl *)(void *, void *, f32))ZipUps_Draw;
    ZipUpAddTypePc.fns.panel_draw_fn = NULL;
    ZipUpAddTypePc.fns.get_gizmo_name_fn = (void *)ZipUp_GetGizmoName;
    ZipUpAddTypePc.fns.get_output_fn = (void *)ZipUp_GetOutput;
    ZipUpAddTypePc.fns.get_output_name_fn = (void *)ZipUp_GetOutputName;
    ZipUpAddTypePc.fns.get_num_outputs_fn = (void *)ZipUp_GetNumOutputs;
    ZipUpAddTypePc.fns.activate_fn = (void *)ZipUp_Activate;
    ZipUpAddTypePc.fns.activate_rev_fn = (void *)ZipUp_ActivateRev;
    ZipUpAddTypePc.fns.set_visibility_fn = (void *)ZipUp_SetVisibility;
    ZipUpAddTypePc.fns.get_visibility_fn = NULL;
    ZipUpAddTypePc.fns.get_pos_fn = NULL;
    ZipUpAddTypePc.fns.using_special_fn = NULL;
    ZipUpAddTypePc.fns.bolt_hit_plat_fn = NULL;
    ZipUpAddTypePc.fns.get_best_bolt_target_fn = NULL;
    ZipUpAddTypePc.fns.bolt_hit_fn = NULL;
    ZipUpAddTypePc.fns.allocate_progress_data_fn = ZipUps_AllocateProgressData;
    ZipUpAddTypePc.fns.clear_progress_fn = (void(__cdecl *)(void *, void *))ZipUps_ClearProgress;
    ZipUpAddTypePc.fns.store_progress_fn = (void(__cdecl *)(void *, void *, void *))ZipUps_StoreProgress;
    ZipUpAddTypePc.fns.reset_fn = (void(__cdecl *)(void *, void *, void *))ZipUps_Reset;
    ZipUpAddTypePc.fns.reserve_buffer_space_fn = (void *(__cdecl *)(void *))ZipUps_ReserveBufferSpace;
    ZipUpAddTypePc.fns.load_fn = (i32(__cdecl *)(void *, void *))ZipUps_Load;
    ZipUpAddTypePc.fns.post_load_fn = NULL;
    ZipUpAddTypePc.fns.add_level_sfx_fn = NULL;
    zipup_gizmotype_id = type_id;
    return &ZipUpAddTypePc;
}
