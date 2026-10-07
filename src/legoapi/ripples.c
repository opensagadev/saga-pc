#include "saga.h"

typedef struct ripple_effect_cleanup_pc_s {
    u8 unknown_00[0x24];
    NUMTL *material;
} RIPPLE_EFFECT_CLEANUP_PC;
STATIC_ASSERT(offsetof(RIPPLE_EFFECT_CLEANUP_PC, material) == 0x24);
STATIC_ASSERT(sizeof(RIPPLE_EFFECT_CLEANUP_PC) == 0x28);

typedef struct world_ripple_cleanup_pc_s {
    u8 unknown_0000[0x46f4];
    RIPPLE_EFFECT_CLEANUP_PC *effects;
    i32 count;
} WORLD_RIPPLE_CLEANUP_PC;
STATIC_ASSERT(offsetof(WORLD_RIPPLE_CLEANUP_PC, effects) == 0x46f4);
STATIC_ASSERT(offsetof(WORLD_RIPPLE_CLEANUP_PC, count) == 0x46f8);

void __cdecl DestroyRippleMtls(WORLDINFO *world) {
    WORLD_RIPPLE_CLEANUP_PC *view = (WORLD_RIPPLE_CLEANUP_PC *)world;
    i32 i;
    for (i = 0; i < view->count; ++i) {
        if (view->effects[i].material != NULL) {
            NuTexDestroy(view->effects[i].material->texture_id);
            NuMtlDestroy(view->effects[i].material);
        }
    }
}

extern const char TerrainWaterJumpSoundNamePc[13];
void __cdecl NewBuzzFrames(void *, i32, i32);
void __cdecl AddWaterSplash(GAMEOBJECT *object, NUVEC *position) {
    AddGameDebrisXYZ(WORLD->game_debris_sys, 13, position->x, object->water_height, position->z);
    PlaySfx((char *)TerrainWaterJumpSoundNamePc, &object->collision_position);
    NewRumble(object->pad->bound_pad, 0.5f, 0);
    NewBuzzFrames(object->pad->bound_pad, 1, 0);
}

typedef struct ripple_node_pc_s RIPPLE_NODE_PC;
struct ripple_set_pc_s {
    u16 capacity, active_count;
    RIPPLE_NODE_PC *nodes, *free_head, *newest, *oldest;
};
struct ripple_node_pc_s {
    NUMTX matrix;
    NUVEC velocity;
    NUMTL *material;
    f32 initial_size, size, growth;
    u32 start_color, color, end_color;
    f32 age, lifetime, delay;
    u16 flags;
    u8 unknown_76[2];
    RIPPLE_NODE_PC *next, *previous;
};
STATIC_ASSERT(sizeof(RIPPLE_SET_PC) == 0x14);
STATIC_ASSERT(offsetof(RIPPLE_SET_PC, free_head) == 8);
STATIC_ASSERT(offsetof(RIPPLE_SET_PC, oldest) == 0x10);
STATIC_ASSERT(sizeof(RIPPLE_NODE_PC) == 0x80);
STATIC_ASSERT(offsetof(RIPPLE_NODE_PC, velocity) == 0x40);
STATIC_ASSERT(offsetof(RIPPLE_NODE_PC, material) == 0x4c);
STATIC_ASSERT(offsetof(RIPPLE_NODE_PC, size) == 0x54);
STATIC_ASSERT(offsetof(RIPPLE_NODE_PC, start_color) == 0x5c);
STATIC_ASSERT(offsetof(RIPPLE_NODE_PC, age) == 0x68);
STATIC_ASSERT(offsetof(RIPPLE_NODE_PC, flags) == 0x74);
STATIC_ASSERT(offsetof(RIPPLE_NODE_PC, next) == 0x78);
STATIC_ASSERT(offsetof(RIPPLE_SET_PC, capacity) == 0);
STATIC_ASSERT(offsetof(RIPPLE_SET_PC, active_count) == 2);
STATIC_ASSERT(offsetof(RIPPLE_SET_PC, newest) == 0xc);
STATIC_ASSERT(offsetof(RIPPLE_NODE_PC, matrix) == 0);
STATIC_ASSERT(offsetof(RIPPLE_NODE_PC, initial_size) == 0x50);
STATIC_ASSERT(offsetof(RIPPLE_NODE_PC, growth) == 0x58);
STATIC_ASSERT(offsetof(RIPPLE_NODE_PC, color) == 0x60);
STATIC_ASSERT(offsetof(RIPPLE_NODE_PC, end_color) == 0x64);
STATIC_ASSERT(offsetof(RIPPLE_NODE_PC, lifetime) == 0x6c);
STATIC_ASSERT(offsetof(RIPPLE_NODE_PC, delay) == 0x70);
STATIC_ASSERT(offsetof(RIPPLE_NODE_PC, previous) == 0x7c);

static RIPPLE_NODE_PC *RippleAllocPc(RIPPLE_SET_PC *set) {
    RIPPLE_NODE_PC *node = set->free_head, *newest = set->newest, *next_free;
    if (set->active_count >= set->capacity)
        return NULL;
    if (node->next == node)
        next_free = NULL;
    else {
        node->next->previous = node->previous;
        node->previous->next = node->next;
        next_free = node->next;
    }
    if (newest == NULL) {
        node->next = node;
        node->previous = node;
    } else {
        node->next = newest->next;
        node->next->previous = node;
        node->previous = newest;
        newest->next = node;
    }
    ++set->active_count;
    set->free_head = next_free;
    set->newest = node;
    if (set->active_count == set->capacity)
        set->free_head = NULL;
    return node;
}
void __cdecl AddRipple(RIPPLE_SET_PC *set, NUMTX *matrix, f32 size, f32 growth, f32 lifetime, f32 delay,
                       u32 start_color, u32 end_color, i32 flags, NUMTL *material, NUVEC *velocity) {
    RIPPLE_NODE_PC *node;
    if (material == NULL || set == NULL)
        return;
    node = RippleAllocPc(set);
    if (node == NULL) {
        node = set->oldest;
        set->newest = node;
        set->oldest = node->next;
    } else if (set->oldest == NULL)
        set->oldest = node;
    if (node == NULL)
        return;
    node->matrix = *matrix;
    node->initial_size = node->size = size;
    node->growth = growth;
    node->age = 0;
    node->lifetime = lifetime;
    node->delay = delay;
    node->flags = (u16)flags;
    node->color = node->start_color = start_color;
    node->material = material;
    node->end_color = end_color;
    node->velocity = velocity != NULL ? *velocity : v000;
}
void __cdecl BuildRippleMtx(NUMTX *matrix, NUVEC *normal, NUVEC *position, u16 unused_x, u16 unused_y) {
    (void)unused_x;
    (void)unused_y;
    FindAnglesXY(normal, NULL, NULL);
    NuMtxSetRotationX(matrix, temp_zrot);
    NuMtxRotateY(matrix, ConfigBuffer.fields.find_angles_y_rotation);
    NuMtxTranslate(matrix, position);
}

typedef struct ripple_effect_pc_s {
    u32 start_color, end_color;
    f32 lifetime, initial_size, end_size;
    char texture_name[16];
    NUMTL *material;
} RIPPLE_EFFECT_PC;
STATIC_ASSERT(sizeof(RIPPLE_EFFECT_PC) == 0x28);
STATIC_ASSERT(offsetof(RIPPLE_EFFECT_PC, lifetime) == 8);
STATIC_ASSERT(offsetof(RIPPLE_EFFECT_PC, initial_size) == 0xc);
STATIC_ASSERT(offsetof(RIPPLE_EFFECT_PC, texture_name) == 0x14);
STATIC_ASSERT(offsetof(RIPPLE_EFFECT_PC, material) == 0x24);
STATIC_ASSERT(offsetof(RIPPLE_EFFECT_PC, start_color) == 0);
STATIC_ASSERT(offsetof(RIPPLE_EFFECT_PC, end_color) == 4);
STATIC_ASSERT(offsetof(RIPPLE_EFFECT_PC, end_size) == 0x10);

typedef struct world_ripple_view_pc_s {
    u8 unknown_0000[0x46f4];
    RIPPLE_EFFECT_PC *effects;
    i32 count, water_effect, sabre_effect;
} WORLD_RIPPLE_VIEW_PC;
STATIC_ASSERT(offsetof(WORLD_RIPPLE_VIEW_PC, effects) == 0x46f4);
STATIC_ASSERT(offsetof(WORLD_RIPPLE_VIEW_PC, water_effect) == 0x46fc);
STATIC_ASSERT(offsetof(WORLD_RIPPLE_VIEW_PC, sabre_effect) == 0x4700);
typedef struct ripple_contact_timer_view_pc_s {
    u8 unknown_0000[0xdc0];
    f32 timer;
} RIPPLE_CONTACT_TIMER_VIEW_PC;
STATIC_ASSERT(offsetof(RIPPLE_CONTACT_TIMER_VIEW_PC, timer) == 0xdc0);
extern RIPPLE_SET_PC *RippleSetPc;
extern f32 RipplePerturbationPc, RippleSfxCooldownPc, ForceFieldRippleRatePc;
extern const char RippleForceFieldSoundNamePc[10];
static f32 RippleRandomRadiusPc = 0.15f;
void __cdecl VecRotateAxis(NUVEC *, u16, NUVEC *);
void __cdecl AddSurfaceRipples(GAMEOBJECT *object) {
    WORLD_RIPPLE_VIEW_PC *world = (WORLD_RIPPLE_VIEW_PC *)WORLD;
    RIPPLE_CONTACT_TIMER_VIEW_PC *contact_timer = (RIPPLE_CONTACT_TIMER_VIEW_PC *)object;
    PLAYERCHARACTERCONFIG_PC *data;
    RIPPLE_EFFECT_PC *effect;
    NUVEC position, normal, seed, velocity;
    NUMTX matrix;
    f32 rate, size;
    i32 count, i;
    if (world->effects == NULL || object->dead != 0)
        return;
    if (object->contact_count != 1) {
        if (object->water_intersects == 0 || object->model_draw_result == 0)
            return;
        data = object->character_data->game_character;
        rate = (f32)(((f64)object->pad->input_magnitude / (f64)data->run_speed) * 20.0);
        if (rate < 1.0f)
            rate = 1.0f;
        count = ParticlesPerSecond(rate, FRAMETIME);
        if (count <= 0)
            return;
        position.x = position.y = position.z = 0;
        BuildRippleMtx(&matrix, &v010, &position, 0, 0);
        size = (f32)((f64)object->bounds_radius * .5);
        for (i = 0; i < count; ++i) {
            matrix.m30 = (f32)((((f64)qrand() * (f64)(1.0f / 65535.0f)) - .5) * (f64)RipplePerturbationPc +
                               (f64)object->collision_position.x);
            matrix.m31 = (f32)((f64)object->water_height + (f64).001f);
            matrix.m32 = (f32)((((f64)qrand() * (f64)(1.0f / 65535.0f)) - .5) * (f64)RipplePerturbationPc +
                               (f64)object->collision_position.z);
            world = (WORLD_RIPPLE_VIEW_PC *)WORLD;
            effect = &world->effects[world->water_effect];
            RippleRandomRadiusPc =
                (f32)(((f64)object->bounds_radius + (f64)object->bounds_radius) + (f64)object->bounds_radius);
            AddRipple(RippleSetPc, &matrix, size, RippleRandomRadiusPc, effect->lifetime, 0, effect->start_color,
                      effect->end_color, 7, effect->material, NULL);
        }
        return;
    }
    if (object->contact_material[0] == 12 || object->contact_material[0] == 13) {
        data = object->character_data->game_character;
        rate = (f32)(((f64)object->pad->input_magnitude / (f64)data->run_speed) * (f64)ForceFieldRippleRatePc);
        if (rate < .5f)
            rate = .5f;
        count = ParticlesPerSecond(rate, FRAMETIME);
        if (count > 0) {
            position = object->contact_position;
            normal = object->contact_normal;
            BuildRippleMtx(&matrix, &normal, &position, 0, 0);
            seed.x = (f32)((f64)normal.y * FRAMETIME);
            seed.y = -(f32)((f64)normal.z * FRAMETIME);
            seed.z = -(f32)((f64)normal.x * FRAMETIME);
            for (i = 0; i < count; ++i) {
                u16 angle = (u16)qrand();
                velocity = seed;
                VecRotateAxis(&velocity, angle, &normal);
                world = (WORLD_RIPPLE_VIEW_PC *)WORLD;
                effect = &world->effects[world->sabre_effect];
                AddRipple(RippleSetPc, &matrix, effect->initial_size, effect->end_size, effect->lifetime, 0,
                          effect->start_color, effect->end_color, 15, effect->material, &velocity);
            }
        }
        /* The native sound position remains unwritten when no particles are emitted. */
        if (contact_timer->timer <= 0) {
            PlaySfx((char *)RippleForceFieldSoundNamePc, &position);
            contact_timer->timer = RippleSfxCooldownPc;
        }
    }
    if (contact_timer->timer > 0)
        contact_timer->timer -= FRAMETIME;
}

typedef struct ripple_material_reader_pc_s {
    u8 unknown_0000[0x70];
    f32 opacity;
} RIPPLE_MATERIAL_READER_PC;
STATIC_ASSERT(offsetof(RIPPLE_MATERIAL_READER_PC, opacity) == 0x70);
void __cdecl InitRippleMtl(char *name, NUMTL **result, VARIPTR *buffer, VARIPTR *end) {
    char filename[64] = "stuff\\";
    i32 texture;
    if (name != NULL) {
        NuStrCat(filename, name);
        buffer->addr = (buffer->addr + 15) & ~15u;
        NuStrCat(filename, ".tex");
        texture = NuTexReadPc(filename, buffer, *end);
        if (texture != 0) {
            *result = NuMtlCreateEx3D(1, 1);
            (*result)->attribs.unknown_1_1_2 = 0;
            (*result)->attribs.unknown_1_4_8 = 0;
            ((RIPPLE_MATERIAL_READER_PC *)*result)->opacity = 1.0f;
            (*result)->attribs.z_mode = 1;
            (*result)->attribs.alpha_mode = 2;
            (*result)->attribs.filter_mode = 1;
            (*result)->attribs.unknown_2_1_2 = 2;
            (*result)->texture_id = (i16)texture;
            NuMtlUpdate(*result);
            return;
        }
    }
    *result = NULL;
}
static WORLDINFO *RippleEffectsWorldInfoPc;
static RIPPLE_EFFECT_PC *RippleEffectsCurrentPc;
static const char *RippleEffectNamesPc[2] = {"water", "forcefield"};
i32 __cdecl LookupRippleEffectIndex(char *name) {
    i32 i;
    for (i = 0; i < 2; ++i)
        if (NuStrICmp(RippleEffectNamesPc[i], name) == 0)
            return i;
    return -1;
}
static void RippleEndRadiusPc(NUFPAR_PC *parser) {
    RippleEffectsCurrentPc->end_size = NuFParGetFloat(parser);
}
static void RippleStartRadiusPc(NUFPAR_PC *parser) {
    RippleEffectsCurrentPc->initial_size = NuFParGetFloat(parser);
}
static void RippleLifePc(NUFPAR_PC *parser) {
    RippleEffectsCurrentPc->lifetime = NuFParGetFloat(parser);
}
static void RippleStartColourPc(NUFPAR_PC *parser) {
    u8 *colour = (u8 *)&RippleEffectsCurrentPc->start_color;
    colour[0] = (u8)NuFParGetInt(parser);
    colour[1] = (u8)NuFParGetInt(parser);
    colour[2] = (u8)NuFParGetInt(parser);
    colour[3] = (u8)NuFParGetInt(parser);
}
static void RippleEndColourPc(NUFPAR_PC *parser) {
    u8 *colour = (u8 *)&RippleEffectsCurrentPc->end_color;
    colour[0] = (u8)NuFParGetInt(parser);
    colour[1] = (u8)NuFParGetInt(parser);
    colour[2] = (u8)NuFParGetInt(parser);
    colour[3] = (u8)NuFParGetInt(parser);
}
static void RippleTextureNamePc(NUFPAR_PC *parser) {
    if (NuFParGetWord(parser) && NuStrLen(parser->word_buffer_ptr) < 16)
        NuStrCpy(RippleEffectsCurrentPc->texture_name, parser->word_buffer_ptr);
}
static void RippleEffectTypePc(NUFPAR_PC *parser) {
    if (NuFParGetWord(parser)) {
        i32 index = LookupRippleEffectIndex(parser->word_buffer_ptr);
        if (index != -1) {
            WORLD_RIPPLE_VIEW_PC *world = (WORLD_RIPPLE_VIEW_PC *)RippleEffectsWorldInfoPc;
            if (index == 0)
                world->water_effect = world->count;
            else
                world->sabre_effect = world->count;
        }
    }
}
static NUFPCOMJMP RippleEffectConfigKeywordsPc[8] = {{"texture_name", RippleTextureNamePc},
                                                     {"effect_type", RippleEffectTypePc},
                                                     {"start_colour", RippleStartColourPc},
                                                     {"end_colour", RippleEndColourPc},
                                                     {"life", RippleLifePc},
                                                     {"start_radius", RippleStartRadiusPc},
                                                     {"end_radius", RippleEndRadiusPc},
                                                     {NULL, NULL}};
void __cdecl RippleEffects_Configure(WORLDINFO *world, char *config) {
    WORLD_RIPPLE_VIEW_PC *view = (WORLD_RIPPLE_VIEW_PC *)world;
    NUFPAR_PC *parser;
    RIPPLE_EFFECT_PC *effect;
    i32 active = 0, i;
    view->effects = NULL;
    view->count = 0;
    parser = NuFParCreateMem("rippleeffects", config, 0xffff);
    if (parser == NULL)
        return;
    world->giz_buffer.addr = (world->giz_buffer.addr + 3) & ~3u;
    effect = view->effects = (RIPPLE_EFFECT_PC *)world->giz_buffer.void_ptr;
    NuFParPushCom(parser, RippleEffectConfigKeywordsPc);
    while (NuFParGetLine(parser)) {
        NuFParGetWord(parser);
        if (parser->word_buffer_ptr[0] == 0)
            continue;
        if (active) {
            if (NuStrICmp(parser->word_buffer_ptr, "rippleeffects_end") == 0) {
                active = 0;
                if (effect->texture_name[0] != 0) {
                    ++effect;
                    ++view->count;
                }
            } else
                NuFParInterpretWord(parser);
        } else if (view->count < 2 && NuStrICmp(parser->word_buffer_ptr, "rippleeffects_start") == 0) {
            RippleEffectsWorldInfoPc = world;
            effect->lifetime = 2.0f;
            effect->initial_size = 0;
            effect->end_size = 1.0f;
            effect->texture_name[0] = 0;
            effect->start_color = 0xff404040u;
            effect->end_color = 0;
            effect->material = NULL;
            RippleEffectsCurrentPc = effect;
            active = 1;
        }
    }
    NuFParDestroy(parser);
    if (view->count <= 0) {
        view->effects = NULL;
        return;
    }
    world->giz_buffer.addr = ((u32)(view->effects + view->count) + 15) & ~15u;
    for (i = 0; i < view->count; ++i)
        InitRippleMtl(view->effects[i].texture_name, &view->effects[i].material, &world->giz_buffer,
                      &world->buffer_end);
}
