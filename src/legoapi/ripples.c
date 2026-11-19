#include "legoapi/items/fx/ripples.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/misc/gamemain.h"
#include "legoapi/render/fx/game_deb.h"
#include "legoapi/world/world.h"
#include "legoapi/world/world_shared.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nu3d/nurndr.h"
#include "nu2api/nu3d/nutex.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nufile/nufpar.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#include <string.h>

void DestroyRippleMtls(WORLDINFO *world) {
    // MATCH-EXCEPTION: the cast is repeated at each use; a cast local gives the other SIB order (U1)
    i32 i;
    for (i = 0; i < world->ripple_effect_count; ++i) {
        if (world->ripple_effects[i].material != NULL) {
            NuTexDestroy(world->ripple_effects[i].material->texture_id);
            NuMtlDestroy(world->ripple_effects[i].material);
        }
    }
}

typedef struct ripple_node_s RIPPLE_NODE;

struct ripple_set_s {       // size 0x14
    u16 capacity;           // 0x00
    u16 active_count;       // 0x02
    RIPPLE_NODE *nodes;     // 0x04
    RIPPLE_NODE *free_head; // 0x08
    RIPPLE_NODE *newest;    // 0x0c
    RIPPLE_NODE *oldest;    // 0x10
};

struct ripple_node_s {     // size 0x80
    NUMTX matrix;          // 0x00
    NUVEC velocity;        // 0x40
    NUMTL *material;       // 0x4c
    f32 initial_size;      // 0x50
    f32 size;              // 0x54
    f32 growth;            // 0x58
    u32 start_color;       // 0x5c
    u32 color;             // 0x60
    u32 end_color;         // 0x64
    f32 age;               // 0x68
    f32 lifetime;          // 0x6c
    f32 delay;             // 0x70
    u16 flags;             // 0x74
    u8 reserved_76[2];      // 0x76
    RIPPLE_NODE *next;     // 0x78
    RIPPLE_NODE *previous; // 0x7c
};

static RIPPLE_NODE *RippleAlloc(RIPPLE_SET *set) {
    RIPPLE_NODE *node = set->free_head;
    RIPPLE_NODE *newest = set->newest;
    RIPPLE_NODE *next_free;
    if (set->active_count < set->capacity) {
        if (node->next == node) {
            next_free = NULL;
        } else {
            node->next->previous = node->previous;
            node->previous->next = node->next;
            next_free = node->next;
        }
        if (newest != NULL) {
            node->next = newest->next;
            node->next->previous = node;
            node->previous = newest;
            newest->next = node;
        } else {
            node->next = node;
            node->previous = node;
        }
        ++set->active_count;
        set->free_head = next_free;
        set->newest = node;
        if (set->active_count == set->capacity) {
            set->free_head = NULL;
        }
        return node;
    }
    return NULL;
}

void AddRipple(RIPPLE_SET *set, NUMTX *matrix, f32 size, f32 growth, f32 lifetime, f32 delay, u32 start_color, u32 end_color, i32 flags,
               NUMTL *material, NUVEC *velocity) {
    RIPPLE_NODE *node;
    if (material == NULL || set == NULL) {
        return;
    }
    node = RippleAlloc(set);
    if (node == NULL) {
        node = set->oldest;
        set->newest = set->oldest;
        set->oldest = set->oldest->next;
    } else if (set->oldest == NULL) {
        set->oldest = node;
    }
    if (node != NULL) {
        node->matrix = *matrix;
        node->size = size;
        node->growth = growth;
        node->initial_size = size;
        node->age = 0;
        node->lifetime = lifetime;
        node->delay = delay;
        node->flags = (u16)flags;
        node->start_color = node->color = start_color;
        node->material = material;
        node->end_color = end_color;
        if (velocity != NULL) {
            node->velocity = *velocity;
        } else {
            node->velocity = v000;
        }
    }
}

static WORLDINFO *RippleEffectsWorldInfo = 0;
static RIPPLE_EFFECT *RippleEffectsCurrent = 0;
static const char *RippleEffectNames[2] = {"water", "forcefield"};

i32 LookupRippleEffectIndex(char *name) {
    i32 i;
    for (i = 0; i < 2; ++i) {
        if (NuStrICmp(RippleEffectNames[i], name) == 0) {
            return i;
        }
    }
    return -1;
}

static void RippleEndRadius(NUFPAR *parser) {
    RippleEffectsCurrent->end_size = NuFParGetFloat(parser);
}

static void RippleStartRadius(NUFPAR *parser) {
    RippleEffectsCurrent->initial_size = NuFParGetFloat(parser);
}

static void RippleLife(NUFPAR *parser) {
    RippleEffectsCurrent->lifetime = NuFParGetFloat(parser);
}

static void RippleStartColour(NUFPAR *parser) {
    RippleEffectsCurrent->start_rgba[0] = (u8)NuFParGetInt(parser);
    RippleEffectsCurrent->start_rgba[1] = (u8)NuFParGetInt(parser);
    RippleEffectsCurrent->start_rgba[2] = (u8)NuFParGetInt(parser);
    RippleEffectsCurrent->start_rgba[3] = (u8)NuFParGetInt(parser);
}

static void RippleEndColour(NUFPAR *parser) {
    RippleEffectsCurrent->end_rgba[0] = (u8)NuFParGetInt(parser);
    RippleEffectsCurrent->end_rgba[1] = (u8)NuFParGetInt(parser);
    RippleEffectsCurrent->end_rgba[2] = (u8)NuFParGetInt(parser);
    RippleEffectsCurrent->end_rgba[3] = (u8)NuFParGetInt(parser);
}

static void RippleTextureName(NUFPAR *parser) {
    if (NuFParGetWord(parser) && NuStrLen(parser->word_buffer_ptr) < 16) {
        NuStrCpy(RippleEffectsCurrent->texture_name, parser->word_buffer_ptr);
    }
}

static void RippleEffectType(NUFPAR *parser) {
    if (NuFParGetWord(parser)) {
        i32 index = LookupRippleEffectIndex(parser->word_buffer_ptr);
        if (index != -1) {
            WORLDINFO *world = RippleEffectsWorldInfo;
            world->ripple_effect_of_type[index] = world->ripple_effect_count;
        }
    }
}

static NUFPCOMJMP RippleEffectConfigKeywords[8] = {{"texture_name", RippleTextureName},
                                                   {"effect_type", RippleEffectType},
                                                   {"start_colour", RippleStartColour},
                                                   {"end_colour", RippleEndColour},
                                                   {"life", RippleLife},
                                                   {"start_radius", RippleStartRadius},
                                                   {"end_radius", RippleEndRadius},
                                                   {NULL, NULL}};

void RippleEffects_Configure(WORLDINFO *world, char *config) {
    NUFPAR *parser;
    RIPPLE_EFFECT *effect;
    i32 active;
    i32 i;
    world->ripple_effects = NULL;
    world->ripple_effect_count = 0;
    parser = NuFParCreateMem("rippleeffects", config, 0xffff);
    if (parser == NULL) {
        return;
    }
    world->giz_buffer.addr = (world->giz_buffer.addr + 3) & ~3u;
    effect = world->ripple_effects = world->giz_buffer.void_ptr;
    NuFParPushCom(parser, RippleEffectConfigKeywords);
    active = 0;
    while (NuFParGetLine(parser)) {
        NuFParGetWord(parser);
        if (parser->word_buffer_ptr[0] == 0) {
            continue;
        }
        if (active) {
            if (NuStrICmp(parser->word_buffer_ptr, "rippleeffects_end") == 0) {
                active = 0;
                if (effect->texture_name[0] != 0) {
                    ++effect;
                    ++world->ripple_effect_count;
                }
            } else {
                NuFParInterpretWord(parser);
            }
        } else if (world->ripple_effect_count < 2 && NuStrICmp(parser->word_buffer_ptr, "rippleeffects_start") == 0) {
            active = 1;
            RippleEffectsWorldInfo = world;
            RippleEffectsCurrent = effect;
            effect->lifetime = 2.0f;
            effect->initial_size = 0;
            effect->texture_name[0] = 0;
            effect->end_size = 1.0f;
            effect->start_rgba[0] = 0x40;
            effect->start_rgba[1] = 0x40;
            effect->start_rgba[2] = 0x40;
            effect->start_rgba[3] = 0xff;
            effect->end_rgba[0] = 0;
            effect->end_rgba[1] = 0;
            effect->end_rgba[2] = 0;
            effect->end_rgba[3] = 0;
            effect->material = NULL;
        }
    }
    NuFParDestroy(parser);
    if (world->ripple_effect_count > 0) {
        world->giz_buffer.addr = ((u32)(world->ripple_effects + world->ripple_effect_count) + 15) & ~15u;
        for (i = 0; i < world->ripple_effect_count; ++i) {
            InitRippleMtl(world->ripple_effects[i].texture_name, &world->ripple_effects[i].material, &world->giz_buffer,
                          &world->buffer_end);
        }
    } else {
        world->ripple_effects = NULL;
    }
}

/* Original ripples.cpp private erase worklist. Native allocation derives its
 * pointer-array working view from the serialized caller count, clamped to64. */
static RIPPLE_NODE **eraselist = 0;
static i32 RippleEraseCount = 0;

static void SetupMainPoolPtrs(RIPPLE_NODE *nodes, i32 count) {
    i32 i;
    if (nodes != NULL) {
        for (i = 1; i < count - 1; ++i) {
            nodes[i].next = &nodes[i + 1];
            nodes[i].previous = &nodes[i - 1];
        }
        nodes[0].next = &nodes[1];
        nodes[0].previous = &nodes[count - 1];
        nodes[count - 1].next = &nodes[0];
        nodes[count - 1].previous = &nodes[count - 2];
    }
}

void InitRipples(RIPPLE_SET **result, VARIPTR *buffer, VARIPTR *unused_end, i32 count) {
    RIPPLE_SET *set;
    RIPPLE_NODE *nodes;
    RIPPLE_NODE **erase_list;
    i32 node_count;
    set = (RIPPLE_SET *)((buffer->addr + 3) & ~3u);
    buffer->addr = (u32)(set + 1);
    *result = set;
    node_count = count <= 0 ? 0 : count;
    if (node_count >= 64) {
        node_count = 64;
    }
    (*result)->capacity = (u16)node_count;
    (*result)->active_count = 0;
    node_count = (*result)->capacity;
    nodes = (RIPPLE_NODE *)((buffer->addr + 127) & ~127u);
    buffer->addr = (u32)nodes + node_count * sizeof(*nodes);
    SetupMainPoolPtrs(nodes, node_count);
    erase_list = (RIPPLE_NODE **)((buffer->addr + 3) & ~3u);
    buffer->addr = (u32)(erase_list + node_count);
    eraselist = erase_list;
    (*result)->nodes = nodes;
    (*result)->free_head = (*result)->nodes;
    (*result)->newest = NULL;
    (*result)->oldest = NULL;
}

void ResetRippleSet(RIPPLE_SET *set) {
    RIPPLE_NODE *nodes = set->nodes;
    i32 count = set->capacity;
    memset(nodes, 0, count * sizeof(*nodes));
    memset(set, 0, sizeof(*set));
    set->nodes = nodes;
    set->capacity = count;
    SetupMainPoolPtrs(nodes, count);
    set->free_head = set->nodes;
}

static void RippleFreeNode(RIPPLE_SET *set, RIPPLE_NODE *node) {
    RIPPLE_NODE *free_head = set->free_head;
    RIPPLE_NODE *newest = set->newest;
    if (set->active_count > 0) {
        if (node->previous != NULL) {
            node->previous->next = node->next;
        } else {
            newest = node->next;
        }
        if (node->next != NULL) {
            node->next->previous = node->previous;
        }
        if (free_head == NULL) {
            node->previous = node;
            node->next = node;
        } else {
            node->next = free_head->next;
            free_head->next = node;
            node->previous = free_head;
            node->next->previous = node;
        }
        --set->active_count;
        set->free_head = node;
        set->newest = newest;
        if (set->active_count == 0) {
            set->newest = NULL;
            set->oldest = NULL;
        }
    }
}

void UpdateRippleSet(RIPPLE_SET *set) {
    RIPPLE_NODE *node;
    RIPPLE_NODE *next;
    i32 i;
    i32 erase_count;
    RIPPLE_NODE **erase_list;
    if (set == NULL) {
        return;
    }
    node = set->newest;
    erase_list = eraselist;
    erase_count = RippleEraseCount;
    for (i = 0; i < set->active_count; ++i) {
        if (node == NULL) {
            continue;
        }
        next = node->next;
        if (node->age <= node->lifetime) {
            if (node->delay > 0.0f) {
                node->delay -= FRAMETIME;
            } else {
                f32 ratio = node->age / node->lifetime;
                /* Existing original reader labels: +50 is animated size,
                 * +54 base size, +5c current colour, +60 initial colour. */
                if ((node->flags & 1) != 0) {
                    node->initial_size = (f32)((f64)node->size + ((f64)node->growth - node->size) * ratio);
                }
                if ((node->flags & 2) != 0) {
                    u8 *current = (u8 *)&node->start_color;
                    const u8 *initial = (const u8 *)&node->color;
                    const u8 *end = (const u8 *)&node->end_color;
                    current[3] = (u8)(i32)((f64)initial[3] + (f64)(end[3] - initial[3]) * ratio);
                }
                if ((node->flags & 4) != 0) {
                    u8 *current = (u8 *)&node->start_color;
                    const u8 *initial = (const u8 *)&node->color;
                    const u8 *end = (const u8 *)&node->end_color;
                    current[0] = (u8)(i32)((f64)initial[0] + (f64)(end[0] - initial[0]) * ratio);
                    current[1] = (u8)(i32)((f64)initial[1] + (f64)(end[1] - initial[1]) * ratio);
                    current[2] = (u8)(i32)((f64)initial[2] + (f64)(end[2] - initial[2]) * ratio);
                }
                if ((node->flags & 8) != 0) {
                    node->matrix.m30 += node->velocity.x;
                    node->matrix.m31 += node->velocity.y;
                    node->matrix.m32 += node->velocity.z;
                }
            }
            node->age += FRAMETIME;
        } else {
            erase_list[erase_count++] = node;
        }
        node = next;
    }
    for (i = 0; i < erase_count; ++i) {
        node = erase_list[i];
        if (node != NULL) {
            if (node == set->oldest) {
                set->oldest = node->next;
            }
            RippleFreeNode(set, node);
        }
        erase_list[i] = NULL;
    }
    RippleEraseCount = 0;
}

void DrawRipple(RIPPLE_NODE *node) {
    NURND_VERTEX3D vertices[4];
    NUMTX matrix;
    f32 size = node->initial_size;
    NUVEC top;
    NUVEC left;
    NUVEC right;
    NUVEC bottom;
    top.x = 0;
    top.y = size;
    top.z = 0;
    left.x = -size;
    left.y = 0;
    left.z = 0;
    right.x = size;
    right.y = -0.0f;
    right.z = 0;
    bottom.x = -0.0f;
    bottom.y = -size;
    bottom.z = 0;
    vertices[0].position = top;
    vertices[1].position = left;
    vertices[2].position = right;
    vertices[3].position = bottom;
    vertices[0].u = vertices[2].u = vertices[3].u = 1;
    vertices[1].u = 0;
    vertices[0].v = vertices[1].v = vertices[3].v = 0;
    vertices[2].v = 1;
    vertices[0].colour = vertices[1].colour = vertices[2].colour = vertices[3].colour = node->start_color;
    matrix = node->matrix;
    NuRndrTriStrip3dClip(vertices, 4, &matrix, node->material);
}

void DrawRippleSet(RIPPLE_SET *set) {
    RIPPLE_NODE *node;
    i32 i;
    if (set == NULL) {
        return;
    }
    node = set->newest;
    for (i = 0; i < set->active_count; ++i) {
        if (node != NULL) {
            DrawRipple(node);
            node = node->next;
        }
    }
}
