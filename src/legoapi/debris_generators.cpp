#include "saga.h"

#define DEBRIS_PARTICLE_AT(key, index, type)                                                                           \
    (&(key)->particle_chunks[(i16)(index) / ((type) == 7 ? 12 : 32)]->particles[(i16)(index) % ((type) == 7 ? 12 : 32)])

extern "C" {
    DEBRISPARTICLE_PC *GenDebDummy(DEBRISKEY_PC *key, DEBRIS_INFO_PC *effect, float time) {
        return GenDebIndex(key, effect, time);
    }

    DEBRISPARTICLE_PC *GenDebIndex(DEBRISKEY_PC *key, DEBRIS_INFO_PC *effect, float time) {
        u16 particle_index = key->particle_cursor;
        u16 next_particle_index;
        if (static_cast<i16>(particle_index) < key->particle_count) {
            next_particle_index = particle_index + 1;
        } else {
            particle_index = 0;
            next_particle_index = 1;
        }

        DEBRISPARTICLE_PC *particle = DEBRIS_PARTICLE_AT(key, particle_index, effect->particle_type);
        key->particle_cursor = next_particle_index;
        particle->start_time = time;
        key->emission_epoch = time;
        particle->inverse_lifetime = 64.0f / effect->particle_lifetime;

        NUVEC random_position;
        NUVEC displacement;
        NuVecScale(&displacement, &effect->emitter_velocity, time - key->emission_time);

        f32 random = NuRandFloatSeeded(&debrisseed);
        random_position.x = (random + random) * effect->emitter_random_extent.x - effect->emitter_random_extent.x;
        random = NuRandFloatSeeded(&debrisseed);
        random_position.y = (random + random) * effect->emitter_random_extent.y - effect->emitter_random_extent.y;
        random = NuRandFloatSeeded(&debrisseed);
        random_position.z = (random + random) * effect->emitter_random_extent.z - effect->emitter_random_extent.z;
        NuVecMtxTransformVU0(&random_position, &random_position, &key->emitter_orientation);
        particle->position.x = random_position.x + displacement.x;
        particle->position.y = random_position.y + displacement.y;
        particle->position.z = random_position.z + displacement.z;

        random = NuRandFloatSeeded(&debrisseed);
        random_position.x = (random + random) * effect->momentum_random_x - effect->momentum_random_x;
        random = NuRandFloatSeeded(&debrisseed);
        random_position.y = (random + random) * effect->rotor_speed_x - effect->rotor_speed_x + effect->momentum_base;
        random = NuRandFloatSeeded(&debrisseed);
        random_position.z = (random + random) * effect->rotor_speed_y - effect->rotor_speed_y;
        NuVecMtxTransformVU0(&random_position, &random_position, &key->emitter_orientation);
        particle->momentum.x = random_position.x;
        particle->momentum.y = random_position.y;
        particle->momentum.z = random_position.z;
        if (key->momentum_adjuster != NULL) {
            key->momentum_adjuster(key, effect, particle);
        }
        particle->position.x += key->emission_position.x;
        particle->position.y += key->emission_position.y;
        particle->position.z += key->emission_position.z;
        particle->momentum.x += key->momentum.x;
        particle->momentum.y += key->momentum.y;
        particle->momentum.z += key->momentum.z;

        for (i32 trail = 0; trail < static_cast<i8>(effect->trail_count); ++trail) {
            u16 trail_index = key->particle_cursor;
            u16 next_trail_index;
            if (static_cast<i16>(trail_index) < key->particle_count) {
                next_trail_index = trail_index + 1;
            } else {
                trail_index = 0;
                next_trail_index = 1;
            }
            DEBRISPARTICLE_PC *trail_particle = DEBRIS_PARTICLE_AT(key, trail_index, effect->particle_type);
            key->particle_cursor = next_trail_index;
            *trail_particle = *particle;
            trail_particle->start_time += static_cast<f32>(trail + 1) * effect->trail_time;
        }

        if (effect->native_data == NULL) {
            GenericDebinfoDmaTypeUpdate(effect);
        }
        return particle;
    }

    DEBRISPARTICLE_PC *GenDebIndexRadial(DEBRISKEY_PC *key, DEBRIS_INFO_PC *effect, float time) {
        if (key->particle_cursor >= key->particle_count)
            key->particle_cursor = 0;
        if (effect->particle_type == 7)
            return NULL;
        DEBRISPARTICLE_PC *particle = DEBRIS_PARTICLE_AT(key, key->particle_cursor++, effect->particle_type);
        particle->start_time = time;
        key->emission_epoch = time;
        particle->inverse_lifetime = 64.0f / effect->particle_lifetime;
        f32 random = NuRandFloatSeeded(&debrisseed);
        i32 angle_y = static_cast<i32>((random + random) * effect->rotor_speed_x - effect->rotor_speed_x +
                                       effect->emitter_random_extent.y);
        random = NuRandFloatSeeded(&debrisseed);
        i32 angle_z = static_cast<i32>((random + random) * effect->rotor_speed_y - effect->rotor_speed_y +
                                       effect->emitter_random_extent.z);
        f32 radius;
        if (effect->scale_in_time != 0.0f && effect->scale_in_time > globaltime - key->last_update_time)
            radius = (globaltime - key->last_update_time) / effect->scale_in_time * effect->emitter_random_extent.x;
        else
            radius = effect->emitter_random_extent.x;
        NUVEC vector = {0.0f, radius, 0.0f};
        NuVecRotateZ(&vector, &vector, angle_z);
        NuVecRotateY(&vector, &vector, angle_y);
        NuVecMtxTransformVU0(&vector, &vector, &key->emitter_orientation);
        particle->position = vector;
        random = NuRandFloatSeeded(&debrisseed);
        vector.x = 0.0f;
        vector.y = (random + random) * effect->momentum_random_x - effect->momentum_random_x + effect->momentum_base;
        vector.z = 0.0f;
        NuVecRotateZ(&vector, &vector, angle_z);
        NuVecRotateY(&vector, &vector, angle_y);
        NuVecMtxTransformVU0(&vector, &vector, &key->emitter_orientation);
        particle->momentum = vector;
        if (key->momentum_adjuster != NULL)
            key->momentum_adjuster(key, effect, particle);
        particle->position.x += key->emission_position.x;
        particle->position.y += key->emission_position.y;
        particle->position.z += key->emission_position.z;
        particle->momentum.x += key->momentum.x;
        particle->momentum.y += key->momentum.y;
        particle->momentum.z += key->momentum.z;
        for (i32 trail = 0; trail < static_cast<i8>(effect->trail_count); ++trail) {
            if (key->particle_cursor >= key->particle_count)
                key->particle_cursor = 0;
            DEBRISPARTICLE_PC *copy = DEBRIS_PARTICLE_AT(key, key->particle_cursor++, 0);
            *copy = *particle;
            copy->start_time += static_cast<f32>(trail + 1) * effect->trail_time;
        }
        if (effect->native_data == NULL)
            GenericDebinfoDmaTypeUpdate(effect);
        return particle;
    }

    DEBRISPARTICLE_PC *GenDebIndexRadialRotor(DEBRISKEY_PC *key, DEBRIS_INFO_PC *effect, float time) {
        if (key->particle_cursor >= key->particle_count)
            key->particle_cursor = 0;
        if (effect->particle_type == 7)
            return NULL;
        DEBRISPARTICLE_PC *particle = DEBRIS_PARTICLE_AT(key, key->particle_cursor++, effect->particle_type);
        particle->start_time = time;
        key->emission_epoch = time;
        particle->inverse_lifetime = 64.0f / effect->particle_lifetime;
        NUVEC displacement;
        NuVecScale(&displacement, &effect->emitter_velocity, time - key->emission_time);
        NuVecAdd(&displacement, &displacement, &effect->emitter_random_extent);
        i32 angle_y = static_cast<i32>(static_cast<f32>(key->emitter_rotation_x) + displacement.y);
        i32 angle_z = static_cast<i32>(static_cast<f32>(key->emitter_rotation_y) + displacement.z);
        NUVEC vector = {0.0f, displacement.x, 0.0f};
        NuVecRotateZ(&vector, &vector, angle_z);
        NuVecRotateY(&vector, &vector, angle_y);
        NuVecMtxTransformVU0(&vector, &vector, &key->emitter_orientation);
        particle->position = vector;
        f32 random = NuRandFloatSeeded(&debrisseed);
        vector.x = 0.0f;
        vector.y = (random + random) * effect->momentum_random_x - effect->momentum_random_x + effect->momentum_base;
        vector.z = 0.0f;
        NuVecRotateZ(&vector, &vector, angle_z);
        NuVecRotateY(&vector, &vector, angle_y);
        NuVecMtxTransformVU0(&vector, &vector, &key->emitter_orientation);
        particle->momentum = vector;
        if (key->momentum_adjuster != NULL)
            key->momentum_adjuster(key, effect, particle);
        particle->position.x += key->emission_position.x;
        particle->position.y += key->emission_position.y;
        particle->position.z += key->emission_position.z;
        particle->momentum.x += key->momentum.x;
        particle->momentum.y += key->momentum.y;
        particle->momentum.z += key->momentum.z;
        for (i32 trail = 0; trail < static_cast<i8>(effect->trail_count); ++trail) {
            if (key->particle_cursor >= key->particle_count)
                key->particle_cursor = 0;
            DEBRISPARTICLE_PC *copy = DEBRIS_PARTICLE_AT(key, key->particle_cursor++, 0);
            *copy = *particle;
            copy->start_time += static_cast<f32>(trail + 1) * effect->trail_time;
        }
        if (effect->native_data == NULL)
            GenericDebinfoDmaTypeUpdate(effect);
        if (effect->rotor_speed_x != 0.0f)
            key->emitter_rotation_x += static_cast<i16>(static_cast<i32>(effect->rotor_speed_x));
        else
            key->emitter_rotation_x = 0;
        if (effect->rotor_speed_y != 0.0f)
            key->emitter_rotation_y += static_cast<i16>(static_cast<i32>(effect->rotor_speed_y));
        else
            key->emitter_rotation_y = 0;
        return particle;
    }

    DEBRISPARTICLE_PC *GenDebIndexImprovedRadial(DEBRISKEY_PC *key, DEBRIS_INFO_PC *effect, float time) {
        if (key->particle_cursor >= key->particle_count)
            key->particle_cursor = 0;
        if (effect->particle_type == 7)
            return NULL;
        DEBRISPARTICLE_PC *particle = DEBRIS_PARTICLE_AT(key, key->particle_cursor++, effect->particle_type);
        particle->start_time = time;
        key->emission_epoch = time;
        particle->inverse_lifetime = 64.0f / effect->particle_lifetime;
        NUVEC displacement;
        NuVecScale(&displacement, &effect->emitter_velocity, time - key->emission_time);
        NuVecAdd(&displacement, &displacement, &effect->emitter_random_extent);
        i32 random_angle = static_cast<i32>(NuRandFloatSeeded(&debrisseed) * 65536.0f - 32768.0f);
        f32 lower_sine = NuTrigTable[(static_cast<i32>(displacement.z - effect->rotor_speed_y) >> 1) & 0x7fff];
        f32 upper_sine = NuTrigTable[(static_cast<i32>(effect->rotor_speed_y + displacement.z) >> 1) & 0x7fff];
        f32 random = NuRandFloatSeeded(&debrisseed);
        f32 sine = random * (upper_sine - lower_sine) + lower_sine;
        i16 angle_z = static_cast<i16>(DebrisNuASin(sine));
        i32 angle_y = static_cast<i32>(displacement.y) +
                      static_cast<i32>(static_cast<f32>(random_angle) * effect->rotor_speed_x * 0.000030517578125f);
        NUVEC vector = {displacement.x, 0.0f, 0.0f};
        NuVecRotateZ(&vector, &vector, angle_z);
        NuVecRotateY(&vector, &vector, angle_y);
        NuVecMtxTransformVU0(&vector, &vector, &key->emitter_orientation);
        particle->position = vector;
        random = NuRandFloatSeeded(&debrisseed);
        vector.x = (random + random) * effect->momentum_random_x - effect->momentum_random_x + effect->momentum_base;
        vector.y = 0.0f;
        vector.z = 0.0f;
        NuVecRotateZ(&vector, &vector, angle_z);
        NuVecRotateY(&vector, &vector, angle_y);
        NuVecMtxTransformVU0(&vector, &vector, &key->emitter_orientation);
        particle->momentum = vector;
        if (key->momentum_adjuster != NULL)
            key->momentum_adjuster(key, effect, particle);
        particle->position.x += key->emission_position.x;
        particle->position.y += key->emission_position.y;
        particle->position.z += key->emission_position.z;
        particle->momentum.x += key->momentum.x;
        particle->momentum.y += key->momentum.y;
        particle->momentum.z += key->momentum.z;
        for (i32 trail = 0; trail < static_cast<i8>(effect->trail_count); ++trail) {
            if (key->particle_cursor >= key->particle_count)
                key->particle_cursor = 0;
            DEBRISPARTICLE_PC *copy = DEBRIS_PARTICLE_AT(key, key->particle_cursor++, 0);
            *copy = *particle;
            copy->start_time += static_cast<f32>(trail + 1) * effect->trail_time;
        }
        if (effect->native_data == NULL)
            GenericDebinfoDmaTypeUpdate(effect);
        return particle;
    }

    DEBRISPARTICLE_PC *GenDebIndexRadialStar(DEBRISKEY_PC *key, DEBRIS_INFO_PC *effect, float time) {
        if (key->particle_cursor >= key->particle_count)
            key->particle_cursor = 0;
        if (effect->particle_type == 7)
            return NULL;
        DEBRISPARTICLE_PC *particle = DEBRIS_PARTICLE_AT(key, key->particle_cursor++, effect->particle_type);
        particle->start_time = time;
        key->emission_epoch = time;
        particle->inverse_lifetime = 64.0f / effect->particle_lifetime;
        f32 random = NuRandFloatSeeded(&debrisseed);
        i32 angle_y = static_cast<i32>((random + random) * effect->rotor_speed_x - effect->rotor_speed_x +
                                       effect->emitter_random_extent.y);
        random = NuRandFloatSeeded(&debrisseed);
        i32 angle_z = static_cast<i32>((random + random) * effect->rotor_speed_y - effect->rotor_speed_y +
                                       effect->emitter_random_extent.z);
        f32 radius;
        if (effect->scale_in_time != 0.0f && effect->scale_in_time > globaltime - key->last_update_time)
            radius = (globaltime - key->last_update_time) / effect->scale_in_time * effect->emitter_random_extent.x;
        else
            radius = effect->emitter_random_extent.x;
        i32 segment_angle = 0x10000 / static_cast<i8>(effect->radial_segments);
        i32 phase = static_cast<i32>(static_cast<f32>((angle_y + 0x10000) % segment_angle) /
                                     static_cast<f32>(segment_angle) * 32768.0f);
        NUVEC vector = {
            0.0f,
            ((1.0f - effect->radial_floor) * (1.0f - NuTrigTable[(phase >> 1) & 0x7fff]) + effect->radial_floor) *
                radius,
            0.0f};
        NuVecRotateZ(&vector, &vector, angle_z);
        NuVecRotateY(&vector, &vector, angle_y);
        NuVecMtxTransformVU0(&vector, &vector, &key->emitter_orientation);
        particle->position = vector;
        random = NuRandFloatSeeded(&debrisseed);
        vector.x = 0.0f;
        vector.y = (random + random) * effect->momentum_random_x - effect->momentum_random_x + effect->momentum_base;
        vector.z = 0.0f;
        NuVecRotateZ(&vector, &vector, angle_z);
        NuVecRotateY(&vector, &vector, angle_y);
        NuVecMtxTransformVU0(&vector, &vector, &key->emitter_orientation);
        particle->momentum = vector;
        if (key->momentum_adjuster != NULL)
            key->momentum_adjuster(key, effect, particle);
        particle->position.x += key->emission_position.x;
        particle->position.y += key->emission_position.y;
        particle->position.z += key->emission_position.z;
        particle->momentum.x += key->momentum.x;
        particle->momentum.y += key->momentum.y;
        particle->momentum.z += key->momentum.z;
        for (i32 trail = 0; trail < static_cast<i8>(effect->trail_count); ++trail) {
            if (key->particle_cursor >= key->particle_count)
                key->particle_cursor = 0;
            DEBRISPARTICLE_PC *copy = DEBRIS_PARTICLE_AT(key, key->particle_cursor++, 0);
            *copy = *particle;
            copy->start_time += static_cast<f32>(trail + 1) * effect->trail_time;
        }
        if (effect->native_data == NULL)
            GenericDebinfoDmaTypeUpdate(effect);
        return particle;
    }

    DEBRISPARTICLE_PC *GenDebIndexBounceY(DEBRISKEY_PC *key, DEBRIS_INFO_PC *effect, float time) {
        if (key->particle_cursor >= key->particle_count)
            key->particle_cursor = 0;
        if (effect->particle_type == 7)
            return NULL;
        DEBRIS_PARTICLE_CHUNK_PC *chunk = key->particle_chunks[key->particle_cursor / 32];
        DEBRISPARTICLE_PC *particle = &chunk->particles[key->particle_cursor % 32];
        particle->start_time = time;
        key->emission_epoch = time;
        particle->inverse_lifetime = 64.0f / effect->particle_lifetime;

        NUVEC vector;
        f32 random = NuRandFloatSeeded(&debrisseed);
        vector.x = (random + random) * effect->emitter_random_extent.x - effect->emitter_random_extent.x;
        random = NuRandFloatSeeded(&debrisseed);
        vector.y = (random + random) * effect->emitter_random_extent.y - effect->emitter_random_extent.y;
        random = NuRandFloatSeeded(&debrisseed);
        vector.z = (random + random) * effect->emitter_random_extent.z - effect->emitter_random_extent.z;
        NuVecMtxTransformVU0(&vector, &vector, &key->emitter_orientation);
        particle->position = vector;
        random = NuRandFloatSeeded(&debrisseed);
        vector.x = (random + random) * effect->momentum_random_x - effect->momentum_random_x;
        random = NuRandFloatSeeded(&debrisseed);
        vector.y = (random + random) * effect->rotor_speed_x - effect->rotor_speed_x + effect->momentum_base;
        random = NuRandFloatSeeded(&debrisseed);
        vector.z = (random + random) * effect->rotor_speed_y - effect->rotor_speed_y;
        NuVecMtxTransformVU0(&vector, &vector, &key->emitter_orientation);
        particle->momentum = vector;

        f32 first, second;
        if (SolveQuadratic(effect->gravity, particle->momentum.y, particle->position.y - key->collision_plane, &first,
                           &second)) {
            f32 collision_time = first > second ? first : second;
            if (!(collision_time <= 0.0f) && effect->particle_lifetime > collision_time) {
                DebrisGetControlStackLock();
                if (freechunkcontrolsptr < debrischunks + debrischunksglass) {
                    DEBRIS_CHUNK_CONTROL_PC *control = freechunkcontrols[freechunkcontrolsptr];
                    control->particle_chunk = chunk;
                    control->owner = NULL;
                    control->active = 5;
                    control->expiry_time = time + collision_time;
                    control->effect_index = key->effect_index;
                    control->rotation_y = 0;
                    control->field_01e = 0;
                    control->collision_plane = key->collision_plane;
                    control->restitution = key->reflection_scale;
                    control->collision_time = collision_time;
                    control->particle_index = key->particle_cursor % 32;
                    AddChunkControlToStack(control, &debris_chunk_control_stack[0]);
                    ++freechunkcontrolsptr;
                }
                DebrisReleaseControlStackLock();
            }
        }
        if (key->momentum_adjuster != NULL)
            key->momentum_adjuster(key, effect, particle);
        ++key->particle_cursor;
        particle->position.x += key->emission_position.x;
        particle->position.y += key->emission_position.y;
        particle->position.z += key->emission_position.z;
        particle->momentum.x += key->momentum.x;
        particle->momentum.y += key->momentum.y;
        particle->momentum.z += key->momentum.z;
        if (effect->native_data == NULL)
            GenericDebinfoDmaTypeUpdate(effect);
        return particle;
    }

    DEBRISPARTICLE_PC *GenDebIndexBounceXZ(DEBRISKEY_PC *key, DEBRIS_INFO_PC *effect, float time) {
        NUVEC normal = {1.0f, 0.0f, 0.0f};
        if (key->particle_cursor >= key->particle_count)
            key->particle_cursor = 0;
        if (effect->particle_type == 7)
            return NULL;
        DEBRIS_PARTICLE_CHUNK_PC *chunk = key->particle_chunks[key->particle_cursor / 32];
        DEBRISPARTICLE_PC *particle = &chunk->particles[key->particle_cursor % 32];
        particle->start_time = time;
        key->emission_epoch = time;
        particle->inverse_lifetime = 64.0f / effect->particle_lifetime;

        NUVEC vector;
        f32 random = NuRandFloatSeeded(&debrisseed);
        vector.x = (random + random) * effect->emitter_random_extent.x - effect->emitter_random_extent.x;
        random = NuRandFloatSeeded(&debrisseed);
        vector.y = (random + random) * effect->emitter_random_extent.y - effect->emitter_random_extent.y;
        random = NuRandFloatSeeded(&debrisseed);
        vector.z = (random + random) * effect->emitter_random_extent.z - effect->emitter_random_extent.z;
        NuVecMtxTransformVU0(&vector, &vector, &key->emitter_orientation);
        particle->position = vector;
        random = NuRandFloatSeeded(&debrisseed);
        vector.x = (random + random) * effect->momentum_random_x - effect->momentum_random_x;
        random = NuRandFloatSeeded(&debrisseed);
        vector.y = (random + random) * effect->rotor_speed_x - effect->rotor_speed_x + effect->momentum_base;
        random = NuRandFloatSeeded(&debrisseed);
        vector.z = (random + random) * effect->rotor_speed_y - effect->rotor_speed_y;
        NuVecMtxTransformVU0(&vector, &vector, &key->emitter_orientation);
        particle->momentum = vector;

        NuVecRotateY(&normal, &normal, key->reflection_y);
        NUVEC plane_position;
        NuVecScale(&plane_position, &normal, key->collision_plane);
        NUVEC end_position = {particle->momentum.x * effect->particle_lifetime + particle->position.x, 0.0f,
                              effect->particle_lifetime * particle->momentum.z + particle->position.z};
        NUVEC start_delta, end_delta;
        NuVecSub(&start_delta, &plane_position, &particle->position);
        NuVecSub(&end_delta, &plane_position, &end_position);
        f32 start_distance = start_delta.x * normal.x + start_delta.z * normal.z;
        f32 end_distance = normal.x * end_delta.x + normal.z * end_delta.z;
        if ((start_distance < 0.0f && end_distance > 0.0f) || (start_distance > 0.0f && end_distance < 0.0f)) {
            f32 lifetime = effect->particle_lifetime;
            DebrisGetControlStackLock();
            if (freechunkcontrolsptr < debrischunks + debrischunksglass) {
                f32 collision_time = (0.0f - start_distance) / (end_distance - start_distance) * lifetime;
                DEBRIS_CHUNK_CONTROL_PC *control = freechunkcontrols[freechunkcontrolsptr];
                control->particle_chunk = chunk;
                control->expiry_time = time + collision_time;
                control->owner = NULL;
                control->active = 6;
                control->effect_index = key->effect_index;
                control->rotation_y = key->reflection_y;
                control->field_01e = 0;
                control->collision_plane = key->collision_plane;
                control->restitution = key->reflection_scale;
                control->collision_time = collision_time;
                control->particle_index = key->particle_cursor % 32;
                AddChunkControlToStack(control, &debris_chunk_control_stack[0]);
                ++freechunkcontrolsptr;
            }
            DebrisReleaseControlStackLock();
        }
        if (key->momentum_adjuster != NULL)
            key->momentum_adjuster(key, effect, particle);
        ++key->particle_cursor;
        particle->position.x += key->emission_position.x;
        particle->position.y += key->emission_position.y;
        particle->position.z += key->emission_position.z;
        particle->momentum.x += key->momentum.x;
        particle->momentum.y += key->momentum.y;
        particle->momentum.z += key->momentum.z;
        if (effect->native_data == NULL)
            GenericDebinfoDmaTypeUpdate(effect);
        return particle;
    }
}
