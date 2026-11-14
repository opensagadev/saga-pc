#include "saga.h"

/* Retail unit 0x597c20-0x5999e0: definitions in retail address order. */
typedef struct carry_plug_pc_s {
    char name[16];       // 0x00
    NUVEC position;      // 0x10
    u16 rotation_x;      // 0x1c
    u16 rotation_y;      // 0x1e
    u16 rotation_z;      // 0x20
    u8 enabled;          // 0x22
    u8 active : 1;       // 0x23:0
    u8 visible : 1;      // 0x23:1
    u8 plugged : 1;      // 0x23:2
    u8 unknown_23 : 5;   // 0x23:3
    u8 unknown_24[0x10]; // 0x24
} CARRY_PLUG_PC;         // size 0x34

typedef struct carry_plug_sys_pc_s {
    CARRY_PLUG_PC *plugs; // 0x0
    i32 count;            // 0x4
} CARRY_PLUG_SYS_PC;      // size 0x8

typedef struct carry_blowup_pc_s {
    NUMTX transform;            // 0x00
    u8 unknown_40[0x9c - 0x40]; // 0x40
    u8 secondary_flags;         // 0x9c
    u8 unknown_9d[0xa8 - 0x9d]; // 0x9d
    f32 target_scale;           // 0xa8
} CARRY_BLOWUP_PC;              // size 0xac

typedef struct carry_world_pc_s {
    u8 unknown_0000[0x516c];     // 0x0000
    CARRY_PLUG_SYS_PC *plug_sys; // 0x516c
} CARRY_WORLD_PC;                // size 0x5170

typedef struct carry_model_pc_s {
    u8 unknown_00[0xc];  // 0x0
    void **model_data_b; // 0xc
} CARRY_MODEL_PC;        // size 0x10

typedef struct carry_basis_pc_s {
    u8 unknown_000[0x4d8];          // 0x000
    PART_LIGHTING_DATA_PC lighting; // 0x4d8
    u8 unknown_5a4[0x748 - 0x5a4];  // 0x5a4
    NUVEC basis[3];                 // 0x748
} CARRY_BASIS_PC;                   // size 0x76c

typedef struct carry_part_pc_s {
    u8 unknown_000[0x218];  // 0x000
    GIZMOBLOWUP_PC *blowup; // 0x218
} CARRY_PART_PC;            // size 0x21c

STATIC_ASSERT(offsetof(PART_PC, force_flags) == 0x218);
STATIC_ASSERT(offsetof(PART_PC, rotation_y) == 0x13a);
STATIC_ASSERT(offsetof(GAMEOBJECT, takeover_start_angle) == 0x7a6);

typedef struct carry_drop_pc_s {
    u8 unknown_000[0x76c]; // 0x000
    NUVEC drop_position;   // 0x76c
} CARRY_DROP_PC;           // size 0x778

STATIC_ASSERT(offsetof(GAMEOBJECT, floor_height) == 0x21c);
STATIC_ASSERT(offsetof(GAMEOBJECT, external_force) == 0x748);
STATIC_ASSERT(offsetof(GAMEOBJECT, context_hover_target_y) == 0x778);
STATIC_ASSERT(offsetof(GAMEOBJECT, context_timer) == 0x77c);
STATIC_ASSERT(offsetof(GAMEOBJECT, context_duration) == 0x784);
STATIC_ASSERT(offsetof(GAMEOBJECT, grapple) == 0x798);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_7aa) == 0x7aa);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_7b3) == 0x7b3);
STATIC_ASSERT(offsetof(GAMEOBJECT, action_movement_state) == 0x7b8);
STATIC_ASSERT(offsetof(GAMEOBJECT, context_variant_flags) == 0x7bc);
STATIC_ASSERT(offsetof(GAMEOBJECT, context_variant_bits_pc) == 0x7bd);
STATIC_ASSERT(offsetof(GAMEOBJECT, layer_mask) == 0x1048);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_1050) == 0x1050);
STATIC_ASSERT(offsetof(GAMEOBJECT, terrain_platform_id) == 0x106c);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_107c) == 0x107c);
extern i32 SuperCarry_PutDownDrop;
extern f32 SUPERCARRY_JUMPSPEED;
extern u8 SuperCarryPlayerRgbPc[2][3];
#define SUPER_CARRY_IDLE_OR_WALK(object)                                                                               \
    do {                                                                                                               \
        if ((object)->pad->input_magnitude > 0.0f) {                                                                   \
            (object)->field_7b3 = 3;                                                                                   \
            (object)->field_7aa = LEGOACT_SUPERCARRY_WALK;                                                             \
        } else {                                                                                                       \
            (object)->field_7b3 = 2;                                                                                   \
            (object)->field_7aa = LEGOACT_SUPERCARRY_IDLE;                                                             \
        }                                                                                                              \
    } while (0)

i32 __cdecl SuperCarry_TurnBlowupBackOn(GIZMOBLOWUP_PC *blowup, NUVEC *position, u16 angle, i32 keep_matrix) {
    CARRY_BLOWUP_PC *view = (CARRY_BLOWUP_PC *)blowup;
    CARRY_PLUG_PC *plug;
    f32 distance;
    blowup->status_flags |= 0x804000;
    GizBlowup_Respawn(blowup);
    blowup->status_flags |= 0x10000;
    plug = Plug_FindNearest(((CARRY_WORLD_PC *)WORLD)->plug_sys, position, &distance, 1);
    if (plug != NULL && distance < 0.5625f) {
        Plug_MakeDrawMtx(plug, &view->transform);
        plug->plugged = 1;
        blowup->status_flags = (blowup->status_flags & ~0x800000) | 0x10000000;
        return 1;
    }
    if (!keep_matrix) {
        if (SuperCarry_AlignObject) {
            NuMtxSetRotationY(&view->transform, angle);
        } else {
            view->transform = NuMtxSetIdentityTemplate;
        }
        NuMtxTranslate(&view->transform, position);
    }
    return 0;
}

void __cdecl SuperCarry_PartImpact(PART_PC *part) {
    f32 amount = NuVecMag(&part->velocity);
    if (amount >= SuperCarryImpactSpeedPc) {
        amount = 1.0f;
    } else {
        amount /= SuperCarryImpactSpeedPc;
    }
    if (amount > 0.0f) {
        GameCam_Judder(NULL, (f32)(amount * 0.30000001192092896), qrand() < 0x8000 ? 0 : 2, &part->position);
    }
}

static void __cdecl SuperCarry_PartKill(PART_PC *part, i32 unused) {
    GIZMOBLOWUP_PC *blowup = ((CARRY_PART_PC *)part)->blowup;
    CARRY_BLOWUP_PC *view = (CARRY_BLOWUP_PC *)blowup;
    (void)unused;
    if (view->secondary_flags & 4) {
        SuperCarry_TurnBlowupBackOn(blowup, &part->position, part->rotation_y, 0);
    } else {
        GizmoBlowUp_AddEffects(&part->position, blowup, 0, 15, NULL);
    }
}

void __cdecl SuperCarry_DrawObject(struct gameobject_s *object) {
    NUMTX matrix;
    NUVEC offset;
    struct nuvec_s *translation;
    PLAYERCHARACTERCONFIG_PC *config;
    struct GIZMOBLOWUP_PC *blowup;
    if ((object->flags_e20 & 0x800000) != 0 ||
        (LEGOCONTEXT_SUPERCARRY != -1 && object->character_context == LEGOCONTEXT_SUPERCARRY &&
         ((object->field_7b3 == 0 && (object->context_variant_flags & 0x40) != 0) ||
          (object->field_7b3 == 1 && (object->context_variant_flags & 0x40) == 0) || object->field_7b3 == 2 ||
          object->field_7b3 == 3 || object->field_7b3 == 6 || object->field_7b3 == 7 ||
          (object->field_7b3 == 4 && (object->context_variant_flags & 0x40) == 0) ||
          (object->field_7b3 == 5 && (object->context_variant_flags & 0x40) == 0)))) {
        translation = NULL;
        if (SuperCarry_AlignObject != 0) {
            NuVecRotateY(&offset, &((CARRY_BASIS_PC *)object)->basis[0], object->field_0x276);
            translation = &offset;
            NuMtxSetRotationY(&matrix, object->takeover_start_angle);
        } else {
            matrix = NuMtxSetIdentityTemplate;
            *NUMTX_GET_ROW_VEC(&matrix, 0) = ((CARRY_BASIS_PC *)object)->basis[0];
            *NUMTX_GET_ROW_VEC(&matrix, 1) = ((CARRY_BASIS_PC *)object)->basis[1];
            *NUMTX_GET_ROW_VEC(&matrix, 2) = ((CARRY_BASIS_PC *)object)->basis[2];
        }
        if (SuperCarry_KeepObjectLevel != 0) {
            NuMtxRotateY(&matrix, object->field_0x276);
            dco_locatorposonly = 1;
        }
        config = object->character_data->game_character;
        blowup = object->grapple;
        DrawObjectOnCharacter(WORLD, object, -1, &blowup->type->animated_special, config->hand_locators[0],
                              config->hand_locators[1], object->locator_matrices, ((u8)object->field_107c),
                              object->layer_mask, &matrix, translation, 1.0f, 1.0f);
    }
}

void __cdecl SuperCarry_GetObjectPos(GAMEOBJECT *object, NUVEC *position, NUVEC *secondary_position) {
    i32 first;
    i32 second;
    NUVEC offset;
    *position = object->collision_position;
    first = object->character_data->game_character->hand_locators[0];
    if (first != -1 && object->character_model->points_of_interest[first] != NULL) {
        second = object->character_data->game_character->hand_locators[1];
        if (second != -1 && object->character_model->points_of_interest[second] != NULL) {
            *position = *(NUVEC *)&object->locator_matrices[first].m30;
            position->x += (object->locator_matrices[second].m30 - position->x) * 0.5f;
            position->y += (object->locator_matrices[second].m31 - position->y) * 0.5f;
            position->z += (object->locator_matrices[second].m32 - position->z) * 0.5f;
        }
    }
    if (SuperCarry_AlignObject != 0) {
        if (secondary_position != NULL) {
            NuVecRotateY(&offset, &object->launch_origin, object->field_0x276);
            NuVecAdd(secondary_position, position, &offset);
        }
        NuVecRotateY(&offset, &object->external_force, object->field_0x276);
        NuVecAdd(position, position, &offset);
    }
}

void __cdecl SuperCarry_Throw(GAMEOBJECT *object, i32 mode) {
    PLAYERCHARACTERCONFIG_PC *config = object->character_data->game_character;
    CARRY_BASIS_PC *basis = (CARRY_BASIS_PC *)object;
    CARRY_BLOWUP_PC *blowup;
    NUMTX matrix;
    NUVEC position;
    NUVEC offset;
    NUVEC velocity;
    ADDPART_PC params;
    PART_PC *part;
    i32 first = config->hand_locators[0];
    i32 second;
    f32 speed;
    if (first == -1 || object->character_model->points_of_interest[first] == NULL) {
        return;
    }
    if (SuperCarry_KeepObjectLevel) {
        if (SuperCarry_AlignObject) {
            NuMtxSetRotationY(&matrix, object->takeover_start_angle);
        } else {
            matrix = NuMtxSetIdentityTemplate;
            *(NUVEC *)&matrix.m00 = basis->basis[0];
            *(NUVEC *)&matrix.m10 = basis->basis[1];
            *(NUVEC *)&matrix.m20 = basis->basis[2];
        }
        NuMtxRotateY(&matrix, object->field_0x276);
        NuMtxTranslate(&matrix, (NUVEC *)&object->locator_matrices[first].m30);
    } else {
        matrix = object->locator_matrices[first];
    }
    second = object->character_data->game_character->hand_locators[1];
    if (second != -1 && object->character_model->points_of_interest[second] != NULL) {
        position = *(NUVEC *)&matrix.m30;
        if (!SuperCarry_KeepObjectLevel) {
            NUMTX left;
            NUMTX right;
            left = matrix;
            right = object->locator_matrices[second];
            left.m30 = left.m31 = left.m32 = 0.0f;
            right.m30 = right.m31 = right.m32 = 0.0f;
            QuatInterpolateRotationMatrix(&matrix, &left, &right, 0.5f);
        }
        matrix.m30 = (f32)((position.x + object->locator_matrices[second].m30) * 0.5);
        matrix.m31 = (f32)((position.y + object->locator_matrices[second].m31) * 0.5);
        matrix.m32 = (f32)((position.z + object->locator_matrices[second].m32) * 0.5);
    }
    if (SuperCarry_KeepObjectLevel) {
        if (SuperCarry_AlignObject) {
            NuVecRotateY(&offset, &basis->basis[0], object->field_0x276);
            NuMtxTranslate(&matrix, &offset);
        }
    } else {
        NUMTX basis_matrix;
        position = *(NUVEC *)&matrix.m30;
        *(NUVEC *)&basis_matrix.m00 = basis->basis[0];
        *(NUVEC *)&basis_matrix.m10 = basis->basis[1];
        *(NUVEC *)&basis_matrix.m20 = basis->basis[2];
        NuMtxMulR(&matrix, &basis_matrix, &matrix);
        *(NUVEC *)&matrix.m30 = position;
    }
    params = Default_ADDPART;
    params.matrix = &matrix;
    speed = mode == 0 ? SUPERCARRY_THROWSPEED_XZ : mode == 1 ? SUPERCARRY_RELEASESPEED_XZ : SUPERCARRY_DROPSPEED_XZ;
    velocity.x = NuTrigTable[object->facing_angle >> 1] * speed;
    velocity.z = NuTrigTable[((object->facing_angle + 0x4000) >> 1) & 0x7fff] * speed;
    velocity.y = mode == 2 ? SUPERCARRY_DROPSPEED_Y : SUPERCARRY_THROWSPEED_Y;
    params.velocity = &velocity;
    blowup = object->grapple;
    params.special = &((GIZMOBLOWUP_PC *)blowup)->type->animated_special;
    params.field_14 = params.field_18 = (f32)(0.5 * blowup->target_scale);
    params.flags = 0x8011;
    if (!(blowup->secondary_flags & 4)) {
        params.flags = 0x8111;
    }
    params.owner = object;
    params.time_step = FRAMETIME;
    params.gravity = SUPERCARRY_OBJGRAVITY;
    params.impact_fn = SuperCarry_PartImpact;
    params.field_a4 = 3.0f;
    params.kill_fn = SuperCarry_PartKill;
    params.lighting_data = &basis->lighting;
    part = AddPart(&params);
    if (part != NULL) {
        part->force_flags = ObjHitObj_Flags(object);
        ((CARRY_PART_PC *)part)->blowup = object->grapple;
        part->rotation_y = object->takeover_start_angle + object->movement_facing_angle;
    }
}

i32 __cdecl SuperCarry_Possible(GAMEOBJECT *object, i32 require_grounded) {
    if (CanSuperCarryFn == NULL || CanSuperCarryFn(object) == 0) {
        return 0;
    }
    if ((object->flags_1fc & 0x80) == 0) {
        return 0;
    }
    if (require_grounded != 0 && (object->field_281 == 0 || ObjLandReady(object) == 0)) {
        return 0;
    }
    return 1;
}

void __cdecl SuperCarry_Start(struct gameobject_s *object, GIZMOBLOWUP_PC *blowup, i32 immediate) {
    object->character_context = ((i8)(LEGOCONTEXT_SUPERCARRY));
    object->context_variant_flags &= ~0x40u;
    object->action_movement_state = 0;
    object->grapple = blowup;
    if (immediate == 0 && LEGOACT_SUPERCARRY_PICKUP != -1 &&
        object->character_model->model_data_b[LEGOACT_SUPERCARRY_PICKUP] != NULL) {
        object->field_7b3 = 0;
        object->field_7aa = LEGOACT_SUPERCARRY_PICKUP;
        object->context_timer = AnimDuration((i16)object->id, object->field_7aa, 0.0f, 0.0f, 1);
        if (object->context_timer <= 0.0f) {
            object->context_timer = 0.5f;
        }
        object->movement_facing_angle = NuAtan2D(blowup->mid_position.x - object->collision_position.x,
                                                 blowup->mid_position.z - object->collision_position.z);
    } else {
        object->field_7b3 = 2;
        object->field_7aa = LEGOACT_SUPERCARRY_IDLE;
        object->context_timer = 0.0f;
        if (immediate == 0) {
            GizmoBlowupBlowup(blowup, 0, 7, -1, NULL, 1);
        }
    }
    if (SuperCarry_AlignObject != 0) {
        u16 angle;
        i32 difference;
        i32 magnitude;
        NUVEC minimum;
        NUVEC maximum;
        NuSpecialGetBounds(&((GIZMOBLOWUP_PC *)object->grapple)->type->animated_special, &minimum, &maximum);
        blowup = object->grapple;
        angle = NuAtan2D(blowup->transform.m20, blowup->transform.m22);
        difference = RotDiff(object->movement_facing_angle, angle);
        magnitude = abs(difference);
        if (magnitude < 0x2000) {
            object->takeover_start_angle = 0;
            ((CARRY_BASIS_PC *)object)->basis[0].x = 0.0f;
            ((CARRY_BASIS_PC *)object)->basis[0].y = -minimum.y;
            ((CARRY_BASIS_PC *)object)->basis[0].z = -minimum.z;
        } else if (magnitude > 0x6000) {
            object->takeover_start_angle = 0x8000;
            ((CARRY_BASIS_PC *)object)->basis[0].x = 0.0f;
            ((CARRY_BASIS_PC *)object)->basis[0].y = -minimum.y;
            ((CARRY_BASIS_PC *)object)->basis[0].z = maximum.z;
        } else if (difference < 0) {
            object->takeover_start_angle = 0xc000;
            ((CARRY_BASIS_PC *)object)->basis[0].x = 0.0f;
            ((CARRY_BASIS_PC *)object)->basis[0].y = -minimum.y;
            ((CARRY_BASIS_PC *)object)->basis[0].z = maximum.x;
        } else {
            object->takeover_start_angle = 0x4000;
            ((CARRY_BASIS_PC *)object)->basis[0].x = 0.0f;
            ((CARRY_BASIS_PC *)object)->basis[0].y = -minimum.y;
            ((CARRY_BASIS_PC *)object)->basis[0].z = -minimum.x;
        }
        ((CARRY_BASIS_PC *)object)->basis[1].x = 0.0f;
        ((CARRY_BASIS_PC *)object)->basis[1].y =
            ((CARRY_BASIS_PC *)object)->basis[0].y + (maximum.y - minimum.y) * 0.5f;
        ((CARRY_BASIS_PC *)object)->basis[1].z = ((CARRY_BASIS_PC *)object)->basis[0].z;
        object->movement_facing_angle = angle - object->takeover_start_angle;
    } else {
        NUMTX matrix = ((GIZMOBLOWUP_PC *)object->grapple)->transform;
        NuMtxRotateY(&matrix, 0x8000 - object->movement_facing_angle);
        ((CARRY_BASIS_PC *)object)->basis[0] = *NUMTX_GET_ROW_VEC(&matrix, 0);
        ((CARRY_BASIS_PC *)object)->basis[1] = *NUMTX_GET_ROW_VEC(&matrix, 1);
        ((CARRY_BASIS_PC *)object)->basis[2] = *NUMTX_GET_ROW_VEC(&matrix, 2);
    }
    object->context_hover_target_y = 0.5f;
}

void __cdecl SuperCarry_MoveCode(WORLDINFO *world, struct gameobject_s *object) {
    if (LEGOCONTEXT_SUPERCARRY == -1) {
        return;
    }
    if (object->character_context == LEGOCONTEXT_SUPERCARRY) {
        f32 *frame = NULL;
        if (object->character_model->model_data_b[object->field_7aa] != NULL) {
            frame = AnimPlaying(&object->anim_packet, object->field_7aa, 1, 0);
            if (frame == NULL) {
                return;
            }
        }
        switch (object->field_7b3) {
            case 0: {
                i32 event = 0;
                if ((object->context_variant_flags & 0x40) == 0) {
                    GIZMOBLOWUP_PC *blowup = object->grapple;
                    object->movement_facing_angle = NuAtan2D(blowup->mid_position.x - object->collision_position.x,
                                                             blowup->mid_position.z - object->collision_position.z);
                    if (frame != NULL) {
                        f32 marker = AnimListFrame(object->character_model, object->field_7aa, 0);
                        if (marker >= 1.0f && *frame >= marker) {
                            event = 1;
                        }
                    }
                }
                object->context_timer -= FRAMETIME;
                if (object->context_timer <= 0.0f) {
                    object->field_7b3 = 2;
                    object->field_7aa = LEGOACT_SUPERCARRY_IDLE;
                    if ((object->context_variant_flags & 0x40) == 0) {
                        event = 1;
                    }
                }
                if (event) {
                    object->context_variant_flags |= 0x40;
                    GizmoBlowupBlowup(object->grapple, 0, 7, -1, NULL, 1);
                    NewBuzzFrames(object->pad->bound_pad, 1, 0);
                }
                return;
            }
            case 1: {
                i32 event = 0;
                if ((object->context_variant_flags & 0x40) == 0) {
                    if (frame != NULL) {
                        f32 marker = AnimListFrame(object->character_model, object->field_7aa, 0);
                        if (marker >= 1.0f && *frame >= marker) {
                            event = 1;
                        }
                    } else if (SuperCarry_PutDownDrop != 0) {
                        event = 1;
                    }
                }
                object->context_timer -= FRAMETIME;
                if (object->context_timer <= 0.0f) {
                    object->character_context = -1;
                    if ((object->context_variant_flags & 0x40) == 0) {
                        event = 1;
                    }
                }
                if (event) {
                    struct GIZMOBLOWUP_PC *blowup;
                    NUVEC *drop;
                    object->context_variant_flags |= 0x40;
                    NewBuzzFrames(object->pad->bound_pad, 1, 0);
                    blowup = object->grapple;
                    drop = &((CARRY_DROP_PC *)object)->drop_position;
                    if (SuperCarry_PutDownDrop != 0) {
                        if (!SuperCarry_TurnBlowupBackOn(
                                blowup, drop, object->takeover_start_angle + object->movement_facing_angle, 1)) {
                            SuperCarry_Throw(object, 2);
                        }
                    } else {
                        SuperCarry_TurnBlowupBackOn(blowup, drop,
                                                    object->takeover_start_angle + object->movement_facing_angle, 0);
                    }
                }
                return;
            }
            case 2:
            case 3: {
                i32 throw_object = 0;
                i32 put_down = 0;
                object->context_timer = object->context_timer + FRAMETIME;
                if (object->field_281 != 0) {
                    if (SuperCarry_UseActionButton != 0) {
                        if ((object->pad->buttons_pressed & GAMEPAD_ACTION) != 0) {
                            throw_object = 1;
                        } else if ((object->pad->buttons_pressed & GAMEPAD_SPECIAL) != 0) {
                            put_down = 1;
                        }
                    } else if ((object->pad->buttons_pressed & GAMEPAD_SPECIAL) != 0) {
                        if (object->pad->input_magnitude > 0.0f) {
                            throw_object = 1;
                        } else {
                            put_down = 1;
                        }
                    }
                    if (throw_object || put_down) {
                        struct GIZMOBLOWUP_PC *blowup;
                        if ((throw_object || SuperCarry_UseActionButton == 0) && SuperCarryEnablePc != 0 &&
                            LEGOACT_SUPERCARRY_BASH != -1 &&
                            ((CARRY_MODEL_PC *)object->character_model)->model_data_b[LEGOACT_SUPERCARRY_BASH] !=
                                NULL) {
                            objopponent_ignoreaiopponent = 1;
                            object->force_target = ObjOpponent(object, 1.0f, PUNCHGAP, 1, 0, 1);
                            if (object->force_target == NULL) {
                                objopponent_ignoreaiopponent = 1;
                                object->force_target = ObjOpponent(object, 1.0f, PUNCHGAP, 1, 0, 2);
                            }
                            if (object->force_target != NULL) {
                                object->field_7b3 = 5;
                                object->field_7aa = LEGOACT_SUPERCARRY_BASH;
                                object->context_variant_flags &= ~0x40u;
                                object->context_timer = AnimDuration((i16)object->id, object->field_7aa, 0.0f, 0.0f, 1);
                                if (object->context_timer <= 0.0f) {
                                    object->context_timer = 0.5f;
                                }
                                return;
                            }
                        }
                        if (throw_object) {
                            if (LEGOACT_SUPERCARRY_THROW != -1 &&
                                ((CARRY_MODEL_PC *)object->character_model)->model_data_b[LEGOACT_SUPERCARRY_THROW] !=
                                    NULL) {
                                object->field_7b3 = 4;
                                object->field_7aa = LEGOACT_SUPERCARRY_THROW;
                                object->context_variant_flags &= ~0x40u;
                                object->context_timer = AnimDuration((i16)object->id, object->field_7aa, 0.0f, 0.0f, 1);
                                if (object->context_timer <= 0.0f) {
                                    object->context_timer = 0.5f;
                                }
                            } else {
                                object->character_context = -1;
                                object->flags_e20 |= 0x800000;
                                NewRumble(object->pad->bound_pad, 0.6f, 0);
                            }
                            return;
                        }
                        blowup = object->grapple;
                        if (put_down && (blowup->secondary_flags & 4) != 0 && LEGOACT_SUPERCARRY_PUTDOWN != -1 &&
                            ((CARRY_MODEL_PC *)object->character_model)->model_data_b[LEGOACT_SUPERCARRY_PUTDOWN] !=
                                NULL &&
                            object->floor_height != 2000000.0f &&
                            object->lower_position.y - object->floor_height < 0.1f) {
                            NUVEC offset;
                            object->field_7b3 = 1;
                            object->field_7aa = LEGOACT_SUPERCARRY_PUTDOWN;
                            object->context_variant_flags &= ~0x40u;
                            object->context_timer = AnimDuration((i16)object->id, object->field_7aa, 0.0f, 0.0f, 1);
                            if (object->context_timer <= 0.0f) {
                                object->context_timer = 0.5f;
                            }
                            ((CARRY_DROP_PC *)object)->drop_position.x = object->lower_position.x;
                            ((CARRY_DROP_PC *)object)->drop_position.y = object->floor_height;
                            ((CARRY_DROP_PC *)object)->drop_position.z = object->lower_position.z;
                            NuVecRotateY(&offset, &((CARRY_BASIS_PC *)object)->basis[0], object->field_0x276);
                            ((CARRY_DROP_PC *)object)->drop_position.x =
                                ((CARRY_DROP_PC *)object)->drop_position.x + offset.x;
                            ((CARRY_DROP_PC *)object)->drop_position.z =
                                ((CARRY_DROP_PC *)object)->drop_position.z + offset.z;
                            return;
                        }
                    }
                }
                if (CombatRollEnablePc != 0 && LEGOACT_SUPERCARRY_JUMP != -1 &&
                    (object->pad->buttons_pressed & GAMEPAD_JUMP) != 0 && object->field_281 != 0) {
                    object->field_7b3 = 6;
                    object->field_7aa = LEGOACT_SUPERCARRY_JUMP;
                    object->context_timer = 0.0f;
                    object->velocity.y = SUPERCARRY_JUMPSPEED;
                    if (object->character_model->model_data_b[object->field_7aa] != NULL) {
                        object->context_duration = AnimDuration((i16)object->id, object->field_7aa, 0.0f, 0.0f, 0);
                    } else {
                        object->context_duration =
                            (-object->velocity.y / object->character_data->game_character->gravity) * 2.0f;
                    }
                    object->context_variant_flags &= ~0x8000u;
                    object->context_duration += 0.1f;
                    object->field_281 = 0;
                    object->field_1050 = 0;
                    object->jump_start_height = object->collision_min.y;
                    ResetAnimPacket(&object->anim_packet, -1);
                }
                SUPER_CARRY_IDLE_OR_WALK(object);
                return;
            }
            case 4: {
                i32 event;
                if (object->context_timer <= 0.0f && (object->context_variant_flags & 0x40) != 0) {
                    object->character_context = -1;
                    return;
                }
                event = 0;
                if (frame != NULL && (object->context_variant_flags & 0x40) == 0) {
                    f32 marker = AnimListFrame(object->character_model, object->field_7aa, 0);
                    if (marker >= 1.0f && *frame >= marker) {
                        event = 1;
                    }
                }
                object->context_timer -= FRAMETIME;
                if (object->context_timer <= 0.0f) {
                    if ((object->context_variant_flags & 0x40) == 0) {
                        event = 1;
                    } else {
                        object->character_context = -1;
                    }
                }
                if (!event) {
                    return;
                }
                object->context_variant_flags |= 0x40;
                object->flags_e20 |= 0x800000;
                NewRumble(object->pad->bound_pad, 0.6f, 0);
                return;
            }
            case 5: {
                i32 event;
                if (object->context_timer <= 0.0f && (object->context_variant_flags & 0x40) != 0) {
                    object->character_context = -1;
                    return;
                }
                event = 0;
                if (frame != NULL && (object->context_variant_flags & 0x40) == 0) {
                    f32 marker = AnimListFrame(object->character_model, object->field_7aa, 0);
                    if (marker >= 1.0f && *frame >= marker) {
                        event = 1;
                    }
                }
                object->context_timer -= FRAMETIME;
                if (object->context_timer <= 0.0f) {
                    if ((object->context_variant_flags & 0x40) == 0) {
                        event = 1;
                    } else {
                        object->character_context = -1;
                    }
                }
                if (!event) {
                    return;
                }
                object->context_variant_flags |= 0x40;
                if (ObjOpponentStillThere(object, object->force_target, PUNCHGAP)) {
                    ObjHitObj(object, object->force_target, 1, 0x2000, 0, 1);
                } else if (object->character_context != -1 && object->context_timer > 0.0f) {
                    object->context_variant_flags &= ~0x40u;
                }
                if ((object->context_variant_flags & 0x40) != 0) {
                    struct GIZMOBLOWUP_PC *blowup;
                    NUVEC position;
                    i32 effect;
                    SuperCarry_GetObjectPos(object, &position, NULL);
                    blowup = object->grapple;
                    effect = 3;
                    if (blowup->effect_delay > 0.0f) {
                        effect = 11;
                    }
                    GizmoBlowUp_AddEffects(&position, blowup, 0, effect, NULL);
                    NewRumble(object->pad->bound_pad, 0.6f, 0);
                    NewBuzzFrames(object->pad->bound_pad, 2, 0);
                }
                return;
            }
            case 6: {
                object->context_timer += FRAMETIME;
                if (object->context_timer >= object->context_duration) {
                    object->context_variant_flags |= 0x8000;
                }
                if (object->context_timer >= 0.1f && object->field_281 != 0) {
                    i32 fell = (object->context_variant_flags >> 15) & 1;
                    if (fell || object->pad->input_magnitude == 0.0f) {
                        object->field_7b3 = 7;
                        if (fell && LEGOACT_SUPERCARRY_FALLLAND != -1 &&
                            ((CARRY_MODEL_PC *)object->character_model)->model_data_b[LEGOACT_SUPERCARRY_FALLLAND] !=
                                NULL) {
                            object->field_7aa = LEGOACT_SUPERCARRY_FALLLAND;
                        } else {
                            object->field_7aa = LEGOACT_SUPERCARRY_LAND;
                        }
                        if (object->field_7aa != -1) {
                            object->context_timer = AnimDuration((i16)object->id, object->field_7aa, 0.0f, 0.0f, 1);
                            if (object->context_timer <= 0.0f) {
                                object->context_timer = 0.5f;
                            }
                            ResetAnimPacket(&object->anim_packet, -1);
                            object->velocity.x = object->velocity.z = 0.0f;
                            NewBuzzFrames(object->pad->bound_pad,
                                          object->field_7aa == LEGOACT_SUPERCARRY_FALLLAND ? 2 : 1, 0);
                            return;
                        }
                    }
                    SUPER_CARRY_IDLE_OR_WALK(object);
                }
                return;
            }
            case 7:
                object->context_timer -= FRAMETIME;
                if (object->context_timer <= 0.0f ||
                    ((object->context_variant_flags & 0x8000) == 0 && object->pad->input_magnitude > 0.0f)) {
                    SUPER_CARRY_IDLE_OR_WALK(object);
                }
                return;
            default:
                return;
        }
    } else {
        NUVEC forward;
        struct GIZMOBLOWUP_PC *nearest;
        f32 best;
        struct GIZMOBLOWUP_PC *blowup;
        i32 i;
        i8 player;
        if (CanSuperCarryFn == NULL || CanSuperCarryFn(object) == 0) {
            return;
        }
        if ((object->flags_1fc & 0x80) == 0) {
            return;
        }
        if (object->field_281 == 0 || ObjLandReady(object) == 0) {
            return;
        }
        best = 100000.0f;
        NuVecRotateY(&forward, &v001, object->movement_facing_angle);
        blowup = world->gizmo_blowups;
        nearest = NULL;
        if (blowup == NULL) {
            return;
        }
        for (i = 0; i < world->gizmo_blowup_count; ++i, ++blowup) {
            if ((blowup->status_flags & 1) == 0 && (blowup->status_flags & 0x800000) != 0 &&
                (blowup->status_flags & 0x4000) != 0 && (blowup->status_flags & 0x8000) != 0 &&
                (blowup->secondary_flags & 1) != 0) {
                NUVEC delta;
                f32 distance;
                f32 radius;
                if ((blowup->draw_flags & 0x20) != 0 && ShadowMode == 0) {
                    continue;
                }
                if (blowup->platform_id != -1 && object->terrain_platform_id == blowup->platform_id) {
                    continue;
                }
                distance = NuVecDistSqr(&blowup->mid_position, &object->collision_position, &delta);
                if (!(distance < best)) {
                    continue;
                }
                radius = object->bounds_radius + blowup->target_scale + 0.2f;
                if (!(distance < radius * radius)) {
                    continue;
                }
                if (!(delta.x * forward.x + delta.z * forward.z > 0.0f)) {
                    continue;
                }
                nearest = blowup;
                best = distance;
            }
        }
        if (nearest == NULL) {
            return;
        }
        player = (i8)object->field_280;
        if (player == 0 || player == 1) {
            ADD_GAME_MESSAGE_PC message = AddGameMsg_Default;
            message.text = LEGOASCII_DOWN != NULL ? LEGOASCII_DOWN : txt_UNKNOWN;
            message.position = &nearest->mid_position;
            message.flags = 0x87;
            message.unknown_4f = 4;
            message.scale = 2.0f;
            if (Player[0] != NULL && (((GAMEOBJECT *)Player[0])->flags_1fc & 0x80) && Player[1] != NULL &&
                (((GAMEOBJECT *)Player[1])->flags_1fc & 0x80)) {
                message.unknown_4f = (player == 0 ? 8 : 2) | 4;
            }
            message.red = SuperCarryPlayerRgbPc[player][0];
            message.green = SuperCarryPlayerRgbPc[player][1];
            message.blue = SuperCarryPlayerRgbPc[player][2];
            message.alpha =
                ((u8)(80.0 + 48.0 * (f64)NuTrigTable[((u16)((f64)NuFmod(GameTimer.time_elapsed_mod_seconds, 0.5f) *
                                                            2.0 * 65536.0)) >>
                                                     1]));
            AddGameMsg(&message);
        }
        if ((object->pad->buttons_held & GAMEPAD_SPECIAL) != 0) {
            SuperCarry_Start(object, nearest, 0);
        }
    }
}

i32 __cdecl SuperCarry_SetTargetMom(GAMEOBJECT *object, f32 input_speed) {
    f32 speed = 0.0f;
    object->target_velocity.x = 0.0f;
    object->target_velocity.z = 0.0f;
    if (input_speed > 0.0f) {
        if ((object->field_7b3 == 3 || object->field_7b3 == 1) && object->field_7aa != -1 &&
            object->character_model->model_data_b[object->field_7aa] != NULL) {
            speed = AnimSpeed(object->character_model, object->field_7aa);
        } else if (object->field_7b3 == 3 || object->field_7b3 == 6) {
            if (object->field_7b3 == 6 && LEGOACT_SUPERCARRY_WALK != 0 &&
                object->character_model->model_data_b[LEGOACT_SUPERCARRY_WALK] != NULL) {
                speed = AnimSpeed(object->character_model, LEGOACT_SUPERCARRY_WALK);
            } else {
                speed = NuQFntHalfScale * object->character_data->game_character->run_speed;
            }
        }
        if (speed > 0.0f) {
            object->target_velocity.x = NuTrigTable[object->facing_angle >> 1] * speed;
            object->target_velocity.z = NuTrigTable[((object->facing_angle + 0x4000) >> 1) & 0x7fff] * speed;
        }
    }
    return 0;
}

i32 __cdecl SuperCarry_YRotation(GAMEOBJECT *object, u16 input_angle) {
    if (object->field_7b3 == 0) {
        object->facing_angle = SeekRot(object->facing_angle, object->movement_facing_angle, 12.0f);
        object->field_0x276 = object->facing_angle;
    } else if (object->field_7b3 == 5) {
        if (object->force_target != NULL) {
            object->movement_facing_angle =
                NuAtan2D(object->force_target->collision_position.x - object->collision_position.x,
                         object->force_target->collision_position.z - object->collision_position.z);
        }
        object->facing_angle = SeekRot(object->facing_angle, object->movement_facing_angle, 8.0f);
        object->field_0x276 = object->facing_angle;
    } else if (object->field_7b3 == 2 || object->field_7b3 == 3 || object->field_7b3 == 6) {
        if (object->pad->input_magnitude > 0.0f) {
            object->facing_angle = TurnRot(
                object->facing_angle, input_angle,
                (i32)(VehicleTurnPhaseOffset * object->context_hover_target_y * TerrainSkinSampleCountPc), NULL);
        }
        object->field_0x276 = SeekRot(object->field_0x276, object->facing_angle, 10.0f);
        object->movement_facing_angle = object->facing_angle;
    }
    return 1;
}

i32 __cdecl SuperCarry_Carrying(GAMEOBJECT *object) {
    if (LEGOCONTEXT_SUPERCARRY != -1 && object->character_context == LEGOCONTEXT_SUPERCARRY) {
        if (object->field_7b3 == 2 || object->field_7b3 == 3 || object->field_7b3 == 6) {
            return 1;
        }
    }
    return 0;
}

void __cdecl SuperCarry_Release(GAMEOBJECT *object) {
    if (LEGOCONTEXT_SUPERCARRY != -1 && object->character_context == LEGOCONTEXT_SUPERCARRY &&
        (object->field_7b3 == 2 || object->field_7b3 == 3 || object->field_7b3 == 6)) {
        SuperCarry_Throw(object, 1);
        object->character_context = -1;
    }
}

i32 SuperCarry_PutDownDrop = 1;
f32 SUPERCARRY_JUMPSPEED = 1.4f;
u8 SuperCarryPlayerRgbPc[2][3] = {{0, 127, 255}, {0, 255, 0}};
