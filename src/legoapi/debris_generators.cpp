#include "saga.h"

#define DEBRIS_PARTICLE_AT(key, index, type)                                                                           \
    (&(key)->particle_chunks[(i16)(index) / ((type) == 7 ? 12 : 32)]->particles[(i16)(index) % ((type) == 7 ? 12 : 32)])

#define DEBRIS_PARTICLE_SLOT(key, index, type)                                                                         \
    ((type) == 7 ? &(key)->particle_chunks[(i16)(index) / 12]->particles[(i16)(index) % 12]                            \
                 : &(key)->particle_chunks[(i16)(index) / 32]->particles[(i16)(index) % 32])

#define DEBRIS_RANDOM_SPREAD(range) (NuRandFloatSeeded(&debrisseed) * 2.0f * (range) - (range))

extern "C" {
    DEBRISPARTICLE_PC *GenDebDummy(DEBRISKEY_PC *key, DEBRIS_INFO_PC *effect, float time) {
        return GenDebIndex(key, effect, time);
    }

    DEBRISPARTICLE_PC *GenDebIndex(DEBRISKEY_PC *key, DEBRIS_INFO_PC *effect, float time) {
        if (key->particle_cursor >= key->particle_count)
            key->particle_cursor = 0;
        DEBRISPARTICLE_PC *particle = DEBRIS_PARTICLE_SLOT(key, key->particle_cursor, effect->particle_type);
        key->particle_cursor++;
        particle->start_time = time;
        key->emission_epoch = time;
        particle->inverse_lifetime = 64.0 / effect->particle_lifetime;

        NUVEC random_position;
        NUVEC displacement;
        NuVecScale(&displacement, &effect->emitter_velocity, time - key->emission_time);

        random_position.x = DEBRIS_RANDOM_SPREAD(effect->emitter_random_extent.x);
        random_position.y = DEBRIS_RANDOM_SPREAD(effect->emitter_random_extent.y);
        random_position.z = DEBRIS_RANDOM_SPREAD(effect->emitter_random_extent.z);
        NuVecMtxTransformVU0(&random_position, &random_position, &key->emitter_orientation);
        particle->position.x = random_position.x + displacement.x;
        particle->position.y = random_position.y + displacement.y;
        particle->position.z = random_position.z + displacement.z;

        random_position.x = DEBRIS_RANDOM_SPREAD(effect->momentum_random_x);
        random_position.y = DEBRIS_RANDOM_SPREAD(effect->rotor_speed_x) + effect->momentum_base;
        random_position.z = DEBRIS_RANDOM_SPREAD(effect->rotor_speed_y);
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
            if (key->particle_cursor >= key->particle_count)
                key->particle_cursor = 0;
            DEBRISPARTICLE_PC *trail_particle = DEBRIS_PARTICLE_SLOT(key, key->particle_cursor, effect->particle_type);
            key->particle_cursor++;
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
        DEBRISPARTICLE_PC *particle = DEBRIS_PARTICLE_SLOT(key, key->particle_cursor, effect->particle_type);
        key->particle_cursor++;
        particle->start_time = time;
        key->emission_epoch = time;
        particle->inverse_lifetime = 64.0 / effect->particle_lifetime;
        i32 angle_y = static_cast<i32>(DEBRIS_RANDOM_SPREAD(effect->rotor_speed_x) + effect->emitter_random_extent.y);
        i32 angle_z = static_cast<i32>(DEBRIS_RANDOM_SPREAD(effect->rotor_speed_y) + effect->emitter_random_extent.z);
        NUVEC vector;
        vector.y = effect->scale_in_time != 0.0f && effect->scale_in_time > globaltime - key->last_update_time
                       ? (globaltime - key->last_update_time) / effect->scale_in_time * effect->emitter_random_extent.x
                       : effect->emitter_random_extent.x;
        vector.z = 0.0f;
        vector.x = 0.0f;
        NuVecRotateZ(&vector, &vector, angle_z);
        NuVecRotateY(&vector, &vector, angle_y);
        NuVecMtxTransformVU0(&vector, &vector, &key->emitter_orientation);
        particle->position.x = vector.x;
        particle->position.y = vector.y;
        particle->position.z = vector.z;
        vector.y = DEBRIS_RANDOM_SPREAD(effect->momentum_random_x) + effect->momentum_base;
        vector.z = 0.0f;
        vector.x = 0.0f;
        NuVecRotateZ(&vector, &vector, angle_z);
        NuVecRotateY(&vector, &vector, angle_y);
        NuVecMtxTransformVU0(&vector, &vector, &key->emitter_orientation);
        particle->momentum.x = vector.x;
        particle->momentum.y = vector.y;
        particle->momentum.z = vector.z;
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
            DEBRISPARTICLE_PC *copy = DEBRIS_PARTICLE_AT(key, key->particle_cursor, 0);
            key->particle_cursor++;
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
        DEBRISPARTICLE_PC *particle = DEBRIS_PARTICLE_SLOT(key, key->particle_cursor, effect->particle_type);
        key->particle_cursor++;
        particle->start_time = time;
        key->emission_epoch = time;
        particle->inverse_lifetime = 64.0 / effect->particle_lifetime;
        NUVEC displacement;
        NuVecScale(&displacement, &effect->emitter_velocity, time - key->emission_time);
        NuVecAdd(&displacement, &displacement, &effect->emitter_random_extent);
        i32 angle_y = static_cast<i32>(displacement.y + static_cast<f32>(key->emitter_rotation_x));
        i32 angle_z = static_cast<i32>(displacement.z + static_cast<f32>(key->emitter_rotation_y));
        NUVEC vector;
        vector.y = displacement.x;
        vector.z = 0.0f;
        vector.x = 0.0f;
        NuVecRotateZ(&vector, &vector, angle_z);
        NuVecRotateY(&vector, &vector, angle_y);
        NuVecMtxTransformVU0(&vector, &vector, &key->emitter_orientation);
        particle->position.x = vector.x;
        particle->position.y = vector.y;
        particle->position.z = vector.z;
        vector.y = DEBRIS_RANDOM_SPREAD(effect->momentum_random_x) + effect->momentum_base;
        vector.z = 0.0f;
        vector.x = 0.0f;
        NuVecRotateZ(&vector, &vector, angle_z);
        NuVecRotateY(&vector, &vector, angle_y);
        NuVecMtxTransformVU0(&vector, &vector, &key->emitter_orientation);
        particle->momentum.x = vector.x;
        particle->momentum.y = vector.y;
        particle->momentum.z = vector.z;
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
            DEBRISPARTICLE_PC *copy = DEBRIS_PARTICLE_AT(key, key->particle_cursor, 0);
            key->particle_cursor++;
            *copy = *particle;
            copy->start_time += static_cast<f32>(trail + 1) * effect->trail_time;
        }
        if (effect->native_data == NULL)
            GenericDebinfoDmaTypeUpdate(effect);
        if (effect->rotor_speed_x == 0.0f)
            key->emitter_rotation_x = 0;
        else
            key->emitter_rotation_x += static_cast<i16>(static_cast<i32>(effect->rotor_speed_x));
        if (effect->rotor_speed_y == 0.0f)
            key->emitter_rotation_y = 0;
        else
            key->emitter_rotation_y += static_cast<i16>(static_cast<i32>(effect->rotor_speed_y));
        return particle;
    }

    DEBRISPARTICLE_PC *GenDebIndexImprovedRadial(DEBRISKEY_PC *key, DEBRIS_INFO_PC *effect, float time) {
        if (key->particle_cursor >= key->particle_count)
            key->particle_cursor = 0;
        if (effect->particle_type == 7)
            return NULL;
        DEBRISPARTICLE_PC *particle = DEBRIS_PARTICLE_SLOT(key, key->particle_cursor, effect->particle_type);
        key->particle_cursor++;
        particle->start_time = time;
        key->emission_epoch = time;
        particle->inverse_lifetime = 64.0 / effect->particle_lifetime;
        NUVEC displacement;
        NuVecScale(&displacement, &effect->emitter_velocity, time - key->emission_time);
        NuVecAdd(&displacement, &displacement, &effect->emitter_random_extent);
        i32 random_angle = static_cast<i32>(NuRandFloatSeeded(&debrisseed) * 65536.0f - 32768.0f);
        f32 lower_sine = NuTrigTable[(static_cast<i32>(displacement.z - effect->rotor_speed_y) >> 1) & 0x7fff];
        f32 upper_sine = NuTrigTable[(static_cast<i32>(effect->rotor_speed_y + displacement.z) >> 1) & 0x7fff];
        f32 sine = NuRandFloatSeeded(&debrisseed) * (upper_sine - lower_sine) + lower_sine;
        i32 angle_z = static_cast<i16>(DebrisNuASin(sine));
        i32 angle_y = static_cast<i32>(displacement.y) +
                      static_cast<i32>(static_cast<f32>(random_angle) * effect->rotor_speed_x * 0.000030517578125f);
        NUVEC vector;
        vector.x = displacement.x;
        vector.z = 0.0f;
        vector.y = 0.0f;
        NuVecRotateZ(&vector, &vector, angle_z);
        NuVecRotateY(&vector, &vector, angle_y);
        NuVecMtxTransformVU0(&vector, &vector, &key->emitter_orientation);
        particle->position.x = vector.x;
        particle->position.y = vector.y;
        particle->position.z = vector.z;
        vector.x = DEBRIS_RANDOM_SPREAD(effect->momentum_random_x) + effect->momentum_base;
        vector.z = 0.0f;
        vector.y = 0.0f;
        NuVecRotateZ(&vector, &vector, angle_z);
        NuVecRotateY(&vector, &vector, angle_y);
        NuVecMtxTransformVU0(&vector, &vector, &key->emitter_orientation);
        particle->momentum.x = vector.x;
        particle->momentum.y = vector.y;
        particle->momentum.z = vector.z;
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
            DEBRISPARTICLE_PC *copy = DEBRIS_PARTICLE_AT(key, key->particle_cursor, 0);
            key->particle_cursor++;
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
        DEBRISPARTICLE_PC *particle = DEBRIS_PARTICLE_SLOT(key, key->particle_cursor, effect->particle_type);
        key->particle_cursor++;
        particle->start_time = time;
        key->emission_epoch = time;
        particle->inverse_lifetime = 64.0 / effect->particle_lifetime;
        NUVEC vector;
        i32 angle_y = static_cast<i32>(DEBRIS_RANDOM_SPREAD(effect->rotor_speed_x) + effect->emitter_random_extent.y);
        i32 angle_z = static_cast<i32>(DEBRIS_RANDOM_SPREAD(effect->rotor_speed_y) + effect->emitter_random_extent.z);
        if (effect->scale_in_time != 0.0f && effect->scale_in_time > globaltime - key->last_update_time)
            vector.y = (globaltime - key->last_update_time) / effect->scale_in_time * effect->emitter_random_extent.x;
        else
            vector.y = effect->emitter_random_extent.x;
        i32 segment_angle = 0x10000 / static_cast<i8>(effect->radial_segments);
        f32 fraction = static_cast<f32>((angle_y + 0x10000) % segment_angle) / segment_angle;
        i32 phase = static_cast<i32>(fraction * 32768.0);
        fraction = 1.0f - NuTrigTable[(phase >> 1) & 0x7fff];
        fraction = (1.0f - effect->radial_floor) * fraction + effect->radial_floor;
        vector.y = fraction * vector.y;
        vector.z = 0.0f;
        vector.x = 0.0f;
        NuVecRotateZ(&vector, &vector, angle_z);
        NuVecRotateY(&vector, &vector, angle_y);
        NuVecMtxTransformVU0(&vector, &vector, &key->emitter_orientation);
        particle->position.x = vector.x;
        particle->position.y = vector.y;
        particle->position.z = vector.z;
        vector.y = DEBRIS_RANDOM_SPREAD(effect->momentum_random_x) + effect->momentum_base;
        vector.z = 0.0f;
        vector.x = 0.0f;
        NuVecRotateZ(&vector, &vector, angle_z);
        NuVecRotateY(&vector, &vector, angle_y);
        NuVecMtxTransformVU0(&vector, &vector, &key->emitter_orientation);
        particle->momentum.x = vector.x;
        particle->momentum.y = vector.y;
        particle->momentum.z = vector.z;
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
            DEBRISPARTICLE_PC *copy = DEBRIS_PARTICLE_AT(key, key->particle_cursor, 0);
            key->particle_cursor++;
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
        particle->inverse_lifetime = 64.0 / effect->particle_lifetime;

        NUVEC vector;
        vector.x = DEBRIS_RANDOM_SPREAD(effect->emitter_random_extent.x);
        vector.y = DEBRIS_RANDOM_SPREAD(effect->emitter_random_extent.y);
        vector.z = DEBRIS_RANDOM_SPREAD(effect->emitter_random_extent.z);
        NuVecMtxTransformVU0(&vector, &vector, &key->emitter_orientation);
        particle->position.x = vector.x;
        particle->position.y = vector.y;
        particle->position.z = vector.z;
        vector.x = DEBRIS_RANDOM_SPREAD(effect->momentum_random_x);
        vector.y = DEBRIS_RANDOM_SPREAD(effect->rotor_speed_x) + effect->momentum_base;
        vector.z = DEBRIS_RANDOM_SPREAD(effect->rotor_speed_y);
        NuVecMtxTransformVU0(&vector, &vector, &key->emitter_orientation);
        particle->momentum.x = vector.x;
        particle->momentum.y = vector.y;
        particle->momentum.z = vector.z;

        f32 first, second;
        if (SolveQuadratic(effect->gravity, particle->momentum.y, particle->position.y - key->collision_plane, &first,
                           &second)) {
            if (MAX(first, second) > 0.0f && MAX(first, second) < effect->particle_lifetime) {
                DebrisGetControlStackLock();
                if (freechunkcontrolsptr < debrischunks + debrischunksglass) {
                    freechunkcontrols[freechunkcontrolsptr]->particle_chunk = chunk;
                    freechunkcontrols[freechunkcontrolsptr]->expiry_time = MAX(first, second) + time;
                    freechunkcontrols[freechunkcontrolsptr]->active = 5;
                    freechunkcontrols[freechunkcontrolsptr]->owner = NULL;
                    freechunkcontrols[freechunkcontrolsptr]->effect_index = key->effect_index;
                    freechunkcontrols[freechunkcontrolsptr]->rotation_y = 0;
                    freechunkcontrols[freechunkcontrolsptr]->field_01e = 0;
                    freechunkcontrols[freechunkcontrolsptr]->collision_plane = key->collision_plane;
                    freechunkcontrols[freechunkcontrolsptr]->restitution = key->reflection_scale;
                    freechunkcontrols[freechunkcontrolsptr]->particle_index = key->particle_cursor % 32;
                    freechunkcontrols[freechunkcontrolsptr]->collision_time = MAX(first, second);
                    AddChunkControlToStack(freechunkcontrols[freechunkcontrolsptr], &debris_chunk_control_stack[0]);
                    ++freechunkcontrolsptr;
                }
                /* Retail inlines DebrisReleaseControlStackLock here. */
                debris_control_stack_lock = 0;
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
        particle->inverse_lifetime = 64.0 / effect->particle_lifetime;

        NUVEC vector;
        vector.x = DEBRIS_RANDOM_SPREAD(effect->emitter_random_extent.x);
        vector.y = DEBRIS_RANDOM_SPREAD(effect->emitter_random_extent.y);
        vector.z = DEBRIS_RANDOM_SPREAD(effect->emitter_random_extent.z);
        NuVecMtxTransformVU0(&vector, &vector, &key->emitter_orientation);
        particle->position.x = vector.x;
        particle->position.y = vector.y;
        particle->position.z = vector.z;
        vector.x = DEBRIS_RANDOM_SPREAD(effect->momentum_random_x);
        vector.y = DEBRIS_RANDOM_SPREAD(effect->rotor_speed_x) + effect->momentum_base;
        vector.z = DEBRIS_RANDOM_SPREAD(effect->rotor_speed_y);
        NuVecMtxTransformVU0(&vector, &vector, &key->emitter_orientation);
        particle->momentum.x = vector.x;
        particle->momentum.y = vector.y;
        particle->momentum.z = vector.z;

        NuVecRotateY(&normal, &normal, key->reflection_y);
        NUVEC plane_position;
        NuVecScale(&plane_position, &normal, key->collision_plane);
        NUVEC end_position;
        end_position.x = particle->position.x + particle->momentum.x * effect->particle_lifetime;
        end_position.y = 0.0f;
        end_position.z = particle->position.z + particle->momentum.z * effect->particle_lifetime;
        NUVEC start_delta, end_delta;
        NuVecSub(&start_delta, &plane_position, &particle->position);
        NuVecSub(&end_delta, &plane_position, &end_position);
        f32 start_distance = start_delta.x * normal.x + start_delta.z * normal.z;
        f32 end_distance = normal.x * end_delta.x + normal.z * end_delta.z;
        if ((start_distance < 0.0f && end_distance > 0.0f) || (start_distance > 0.0f && end_distance < 0.0f)) {
            f32 collision_time = (0.0f - start_distance) / (end_distance - start_distance) * effect->particle_lifetime;
            /* Retail inlines DebrisGetControlStackLock here. */
            while (debris_control_stack_lock != 0) {
            }
            debris_control_stack_lock = 1;
            if (freechunkcontrolsptr < debrischunks + debrischunksglass) {
                freechunkcontrols[freechunkcontrolsptr]->particle_chunk = chunk;
                freechunkcontrols[freechunkcontrolsptr]->expiry_time = collision_time + time;
                freechunkcontrols[freechunkcontrolsptr]->active = 6;
                freechunkcontrols[freechunkcontrolsptr]->owner = NULL;
                freechunkcontrols[freechunkcontrolsptr]->effect_index = key->effect_index;
                freechunkcontrols[freechunkcontrolsptr]->rotation_y = key->reflection_y;
                freechunkcontrols[freechunkcontrolsptr]->field_01e = 0;
                freechunkcontrols[freechunkcontrolsptr]->collision_plane = key->collision_plane;
                freechunkcontrols[freechunkcontrolsptr]->restitution = key->reflection_scale;
                freechunkcontrols[freechunkcontrolsptr]->particle_index = key->particle_cursor % 32;
                freechunkcontrols[freechunkcontrolsptr]->collision_time = collision_time;
                AddChunkControlToStack(freechunkcontrols[freechunkcontrolsptr], &debris_chunk_control_stack[0]);
                ++freechunkcontrolsptr;
            }
            /* Retail inlines DebrisReleaseControlStackLock here. */
            debris_control_stack_lock = 0;
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
