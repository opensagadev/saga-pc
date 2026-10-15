#include "saga.h"

typedef struct TELEPORT_CONFIG_PC {
    char name[0x40];
    NUGSPLINE *path;
    f32 duration;
    f32 range_squared;
    u16 flags;
    u8 enabled;
    u8 active;
    NUSPECIALHANDLE blocking_special;
    NUSPECIALHANDLE flap1_special;
    NUSPECIALHANDLE flap2_special;
    u8 unknown_74[8];
    NUMTX flap1_matrix;
    NUMTX flap2_matrix;
} TELEPORT_CONFIG_PC;
STATIC_ASSERT(sizeof(TELEPORT_CONFIG_PC) == 0xfc);
STATIC_ASSERT(offsetof(TELEPORT_CONFIG_PC, name) == 0);
STATIC_ASSERT(offsetof(TELEPORT_CONFIG_PC, path) == 0x40);
STATIC_ASSERT(offsetof(TELEPORT_CONFIG_PC, duration) == 0x44);
STATIC_ASSERT(offsetof(TELEPORT_CONFIG_PC, range_squared) == 0x48);
STATIC_ASSERT(offsetof(TELEPORT_CONFIG_PC, flags) == 0x4c);
STATIC_ASSERT(offsetof(TELEPORT_CONFIG_PC, enabled) == 0x4e);
STATIC_ASSERT(offsetof(TELEPORT_CONFIG_PC, active) == 0x4f);
STATIC_ASSERT(offsetof(TELEPORT_CONFIG_PC, blocking_special) == 0x50);
STATIC_ASSERT(offsetof(TELEPORT_CONFIG_PC, flap1_special) == 0x5c);
STATIC_ASSERT(offsetof(TELEPORT_CONFIG_PC, flap2_special) == 0x68);
STATIC_ASSERT(offsetof(TELEPORT_CONFIG_PC, flap1_matrix) == 0x7c);
STATIC_ASSERT(offsetof(TELEPORT_CONFIG_PC, flap2_matrix) == 0xbc);

STATIC_ASSERT(offsetof(GIZMO_PC, object) == 0);

void __cdecl Teleport_Activate(GIZMO_PC *gizmo, i32 active) {
    TELEPORT_CONFIG_PC *teleport = (TELEPORT_CONFIG_PC *)gizmo->object;
    if (active != 0) {
        teleport->enabled = 1;
        teleport->active = 0;
    } else {
        teleport->enabled = 0;
    }
}

typedef struct WORLD_TELEPORT_CONFIG_PC {
    u8 unknown_00[0x104];
    VARIPTR giz_buffer;
    u8 unknown_108[0x34];
    NUGSCN *scene;
    u8 unknown_140[0x2988];
    GIZMOSYS_PREFIX_PC *gizmo_system;
    u8 unknown_2acc[0x1bb4];
    TELEPORT_CONFIG_PC *teleports;
    i32 teleport_count;
} WORLD_TELEPORT_CONFIG_PC;
STATIC_ASSERT(offsetof(WORLD_TELEPORT_CONFIG_PC, giz_buffer) == 0x104);
STATIC_ASSERT(offsetof(WORLD_TELEPORT_CONFIG_PC, scene) == 0x13c);
STATIC_ASSERT(offsetof(WORLD_TELEPORT_CONFIG_PC, gizmo_system) == 0x2ac8);
STATIC_ASSERT(offsetof(WORLD_TELEPORT_CONFIG_PC, teleports) == 0x4680);
STATIC_ASSERT(offsetof(WORLD_TELEPORT_CONFIG_PC, teleport_count) == 0x4684);
i32 __cdecl GizmoGetUniqueName(GIZMOSYS_PREFIX_PC *, char *, char *, char *, i32);
NUGSPLINE *__cdecl edSpline_SplineFind(NUGSCN *, char *);

static TELEPORT_CONFIG_PC *Tel_teleport;
static WORLD_TELEPORT_CONFIG_PC *Tel_worldinfo;
typedef struct TELEPORT_KEYWORDS_PC {
    char *name_prefix;
    NUFPCOMJMP keywords[12];
} TELEPORT_KEYWORDS_PC;
STATIC_ASSERT(sizeof(TELEPORT_KEYWORDS_PC) == 100);
STATIC_ASSERT(offsetof(TELEPORT_KEYWORDS_PC, name_prefix) == 0);
STATIC_ASSERT(offsetof(TELEPORT_KEYWORDS_PC, keywords) == 4);
static TELEPORT_KEYWORDS_PC TeleportConfigDataPc;

static void __cdecl Tel_spline(NUFPAR_PC *parser) {
    i32 index;
    TELEPORT_CONFIG_PC *teleport;
    if (NuFParGetWord(parser) == 0)
        return;
    Tel_teleport->path = edSpline_SplineFind(Tel_worldinfo->scene, parser->word_buffer_ptr);
    teleport = Tel_teleport;
    if (teleport->path == NULL || teleport->path->length != 4) {
        teleport->path = NULL;
    } else {
        for (index = 0; index < Tel_worldinfo->teleport_count; ++index) {
            if (Tel_worldinfo->teleports[index].path == teleport->path)
                break;
        }
        if (index < Tel_worldinfo->teleport_count)
            teleport->path = NULL;
    }
    if (teleport->path != NULL) {
        NuStrCpy(teleport->name, TeleportConfigDataPc.name_prefix);
        NuStrCat(Tel_teleport->name, Tel_teleport->path->name);
        GizmoGetUniqueName(Tel_worldinfo->gizmo_system, TeleportConfigDataPc.name_prefix, Tel_teleport->name,
                           Tel_teleport->name, 0x40);
    }
}
static void __cdecl Tel_duration(NUFPAR_PC *parser) {
    Tel_teleport->duration = NuFParGetFloat(parser);
}
static void __cdecl Tel_range(NUFPAR_PC *parser) {
    f32 range = NuFParGetFloat(parser);
    Tel_teleport->flags |= 2;
    Tel_teleport->range_squared = range * range;
}
static void __cdecl Tel_1_way(NUFPAR_PC *parser) {
    Tel_teleport->flags |= 1;
    (void)parser;
}
static void __cdecl Tel_crawl(NUFPAR_PC *parser) {
    Tel_teleport->flags |= 4;
    (void)parser;
}
static void __cdecl Tel_flip_flap(NUFPAR_PC *parser) {
    Tel_teleport->flags |= 8;
    (void)parser;
}
static void __cdecl Tel_Hacky_Cam(NUFPAR_PC *parser) {
    Tel_teleport->flags |= 0x10;
    (void)parser;
}
static void __cdecl Tel_obj(NUFPAR_PC *parser) {
    if (NuFParGetWord(parser) != 0)
        NuSpecialFind(Tel_worldinfo->scene, &Tel_teleport->blocking_special, parser->word_buffer_ptr, 1);
}
static void __cdecl Tel_flap(NUFPAR_PC *parser) {
    if (NuFParGetWord(parser) == 0)
        return;
    if (NuStrIStr(parser->line_buffer_ptr, "flap1") != NULL) {
        if (NuSpecialFind(Tel_worldinfo->scene, &Tel_teleport->flap1_special, parser->word_buffer_ptr, 1) != 0)
            Tel_teleport->flap1_matrix = *NuSpecialGetDrawMtx(&Tel_teleport->flap1_special);
    } else {
        if (NuSpecialFind(Tel_worldinfo->scene, &Tel_teleport->flap2_special, parser->word_buffer_ptr, 1) != 0)
            Tel_teleport->flap2_matrix = *NuSpecialGetDrawMtx(&Tel_teleport->flap2_special);
    }
}
static void __cdecl Tel_name(NUFPAR_PC *parser) {
    if (NuFParGetWord(parser) != 0)
        GizmoGetUniqueName(((WORLD_TELEPORT_CONFIG_PC *)WORLD)->gizmo_system, TeleportConfigDataPc.name_prefix,
                           parser->word_buffer_ptr, Tel_teleport->name, 0x40);
}
static TELEPORT_KEYWORDS_PC TeleportConfigDataPc = {"TLT_",
                                                    {{"spline", Tel_spline},
                                                     {"duration", Tel_duration},
                                                     {"range", Tel_range},
                                                     {"1_way", Tel_1_way},
                                                     {"crawl", Tel_crawl},
                                                     {"flip_flap", Tel_flip_flap},
                                                     {"hackycam", Tel_Hacky_Cam},
                                                     {"obj", Tel_obj},
                                                     {"flap1", Tel_flap},
                                                     {"flap2", Tel_flap},
                                                     {"name", Tel_name},
                                                     {NULL, NULL}}};

void __cdecl Teleports_Configure(WORLDINFO *world_ptr, char *config) {
    WORLD_TELEPORT_CONFIG_PC *world = (WORLD_TELEPORT_CONFIG_PC *)world_ptr;
    TELEPORT_CONFIG_PC *teleport;
    NUFPAR_PC *parser;
    i32 active;
    world->teleports = NULL;
    if (world->scene == NULL)
        return;
    parser = NuFParCreateMem("teleports", config, 0xffff);
    if (parser == NULL)
        return;
    world->giz_buffer.addr = (world->giz_buffer.addr + 3) & ~3u;
    teleport = (TELEPORT_CONFIG_PC *)world->giz_buffer.addr;
    world->teleports = teleport;
    NuFParPushCom(parser, TeleportConfigDataPc.keywords);
    active = 0;
    while (NuFParGetLine(parser) != 0) {
        if (NuFParGetWord(parser) == 0)
            continue;
        if (active != 0) {
            if (NuStrICmp(parser->word_buffer_ptr, "teleport_end") != 0) {
                NuFParInterpretWord(parser);
                continue;
            }
            active = 0;
            if (NuStrLen(Tel_teleport->name) == 0) {
                NuStrCpy(Tel_teleport->name, TeleportConfigDataPc.name_prefix);
                NuStrCat(Tel_teleport->name, "TeleportNoSpline!");
                GizmoGetUniqueName(((WORLD_TELEPORT_CONFIG_PC *)WORLD)->gizmo_system, TeleportConfigDataPc.name_prefix,
                                   Tel_teleport->name, Tel_teleport->name, 0x40);
            }
            if (teleport->path != NULL) {
                ++teleport;
                ++world->teleport_count;
            }
        } else if (NuStrICmp(parser->word_buffer_ptr, "teleport_start") == 0) {
            Tel_worldinfo = world;
            Tel_teleport = teleport;
            NuStrCpy(teleport->name, "");
            teleport->duration = 5.0f;
            teleport->enabled = 1;
            teleport->path = NULL;
            teleport->range_squared = 0;
            teleport->flags = 0;
            teleport->active = 0;
            teleport->blocking_special.special = NULL;
            teleport->blocking_special.scene = NULL;
            teleport->blocking_special.display_special = NULL;
            teleport->flap1_special.special = NULL;
            teleport->flap1_special.scene = NULL;
            teleport->flap1_special.display_special = NULL;
            teleport->flap2_special.special = NULL;
            teleport->flap2_special.scene = NULL;
            teleport->flap2_special.display_special = NULL;
            active = 1;
        }
    }
    NuFParDestroy(parser);
    if (world->teleport_count > 0)
        world->giz_buffer.addr = ((u32)teleport + 15) & ~15u;
    else
        world->teleports = NULL;
}

i32 __cdecl Teleport_GetMaxGizmos(void *);
void __cdecl Teleport_AddGizmos(GIZMOSYS_PC *, i32, void *, void *);
char *__cdecl Teleport_GetGizmoName(GIZMO_PC *);
i32 __cdecl Teleport_GetOutput(GIZMO_PC *, i32, i32);
char *__cdecl Teleport_GetOutputName(GIZMO_PC *, i32);
i32 __cdecl Teleport_GetNumOutputs(GIZMO_PC *);
void __cdecl Teleport_Activate(GIZMO_PC *, i32);
i32 __cdecl Teleport_ActivateRev(GIZMO_PC *, i32, i32);

static ADDGIZMOTYPE_PC GizmoRegistryTeleportAddTypePc;
extern const char PCContextName_Teleport[9];
i32 teleport_gizmotype_id = -1;

ADDGIZMOTYPE_PC *__cdecl Teleport_RegisterGizmo(i32 type_id) {
    ADDGIZMOTYPE_PC *addtype = &GizmoRegistryTeleportAddTypePc;
    *addtype = Default_ADDGIZMOTYPE;
    addtype->prefix = TeleportConfigDataPc.name_prefix;
    addtype->fns.options = 0x0;
    addtype->fns.early_update_fn = NULL;
    addtype->fns.late_update_fn = NULL;
    addtype->fns.draw_fn = NULL;
    addtype->fns.panel_draw_fn = NULL;
    addtype->fns.set_visibility_fn = NULL;
    addtype->fns.get_visibility_fn = NULL;
    addtype->fns.get_pos_fn = NULL;
    addtype->fns.using_special_fn = NULL;
    addtype->fns.allocate_progress_data_fn = NULL;
    addtype->fns.clear_progress_fn = NULL;
    addtype->fns.store_progress_fn = NULL;
    addtype->fns.reset_fn = NULL;
    addtype->fns.reserve_buffer_space_fn = NULL;
    addtype->fns.load_fn = NULL;
    addtype->fns.post_load_fn = NULL;
    addtype->fns.add_level_sfx_fn = NULL;
    addtype->name = (char *)PCContextName_Teleport;
    addtype->fns.get_max_gizmos_fn = (i32(__cdecl *)(void *))Teleport_GetMaxGizmos;
    addtype->fns.add_gizmos_fn = (void *)Teleport_AddGizmos;
    addtype->fns.get_gizmo_name_fn = (char *(__cdecl *)(GIZMO_PC *))Teleport_GetGizmoName;
    addtype->fns.get_output_fn = (i32(__cdecl *)(GIZMO_PC *, i32, i32))Teleport_GetOutput;
    addtype->fns.get_output_name_fn = (char *(__cdecl *)(GIZMO_PC *, i32))Teleport_GetOutputName;
    addtype->fns.get_num_outputs_fn = (void *)Teleport_GetNumOutputs;
    addtype->fns.activate_fn = (void(__cdecl *)(GIZMO_PC *, i32))Teleport_Activate;
    addtype->fns.activate_rev_fn = (i32(__cdecl *)(GIZMO_PC *, i32, i32))Teleport_ActivateRev;
    teleport_gizmotype_id = type_id;
    return addtype;
}

i32 __cdecl Teleport_GetOutput(GIZMO_PC *gizmo, i32 output, i32 mode) {
    (void)output;
    (void)mode;
    if (gizmo != NULL && gizmo->object != NULL)
        return ((TELEPORT_CONFIG_PC *)gizmo->object)->active;
    return 0;
}

i32 __cdecl Teleport_ActivateRev(GIZMO_PC *gizmo, i32 reverse, i32 mode) {
    TELEPORT_CONFIG_PC *teleport;
    if (gizmo == NULL || gizmo->object == NULL)
        return 0;
    teleport = (TELEPORT_CONFIG_PC *)gizmo->object;
    if ((mode & 1) != 0 && teleport->enabled != reverse)
        return 0;
    if (reverse != 0)
        teleport->enabled = 0;
    else if (teleport->enabled == 0)
        teleport->enabled = 1;
    return 1;
}
