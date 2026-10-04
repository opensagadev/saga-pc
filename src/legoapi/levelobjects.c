#include "saga.h"

LEVEL_OBJECT_CONFIG_PC *ObjTabList;
i32 LEVELOBJECTCOUNT;
i32 EXTRALEVELOBJECTCOUNT;

static i32 LEVELOBJECTMAX;
static char *ExtraLevelObject_NameTable;
static i32 ExtraLevelObject_NameTableSize;
static i32 ExtraLevelObject_NameTableIndex;

i32 LEVOBJREF_FIRSTOBJ = -1;
i32 LEVOBJREF_LASTOBJ = -1;
i32 LEVOBJREF_FIRSTREFOBJ = -1;
i32 LEVOBJREF_LASTREFOBJ = -1;

void __cdecl LevelObjects_InitForGame(LEVEL_OBJECT_CONFIG_PC *table, VARIPTR *buffer, VARIPTR *buffer_end, i32 maximum,
                                      i32 name_table_size) {
    i32 reflection_range;
    i32 object_range;
    LEVOBJREF_FIRSTOBJ = -1;
    LEVOBJREF_LASTOBJ = -1;
    LEVOBJREF_FIRSTREFOBJ = -1;
    LEVOBJREF_LASTREFOBJ = -1;
    LEVELOBJECTMAX = maximum;
    ObjTabList = table;
    LEVELSPLINECOUNT = 0;
    for (; table->kind != 0xff; ++table, ++LEVELOBJECTCOUNT) {
        if (table->reflection == 1) {
            if (LEVOBJREF_FIRSTOBJ == -1)
                LEVOBJREF_FIRSTOBJ = LEVELOBJECTCOUNT;
            LEVOBJREF_LASTOBJ = LEVELOBJECTCOUNT;
        }
        if (table->reflection == 2) {
            if (LEVOBJREF_FIRSTREFOBJ == -1)
                LEVOBJREF_FIRSTREFOBJ = LEVELOBJECTCOUNT;
            LEVOBJREF_LASTREFOBJ = LEVELOBJECTCOUNT;
        }
    }
    reflection_range = LEVOBJREF_LASTREFOBJ - LEVOBJREF_FIRSTREFOBJ;
    object_range = LEVOBJREF_LASTOBJ - LEVOBJREF_FIRSTOBJ;
    if (object_range > reflection_range)
        LEVOBJREF_LASTOBJ = LEVOBJREF_FIRSTOBJ + reflection_range;
    else if (object_range < reflection_range)
        LEVOBJREF_LASTREFOBJ = LEVOBJREF_FIRSTREFOBJ + object_range;
    if (name_table_size > 0) {
        ExtraLevelObject_NameTable = (char *)buffer->void_ptr;
        ExtraLevelObject_NameTableSize = name_table_size;
        buffer->addr += name_table_size;
        if (buffer->addr >= buffer_end->addr) {
            STUBBED_NAMED("LevelObjects_InitForGame: unresolved name arena diagnostic at 0x00578890");
        }
    }
}

i32 __cdecl LevelObject_AddExtra(char *name, i32 kind) {
    if (LEVELOBJECTCOUNT < LEVELOBJECTMAX && ExtraLevelObject_NameTable != NULL) {
        i32 length = NuStrLen(name);
        if (length + ExtraLevelObject_NameTableIndex + 1 < ExtraLevelObject_NameTableSize) {
            char *destination = ExtraLevelObject_NameTable + ExtraLevelObject_NameTableIndex;
            LEVEL_OBJECT_CONFIG_PC *object = &ObjTabList[LEVELOBJECTCOUNT];
            object->kind = (u8)kind;
            object->name = destination;
            ++LEVELOBJECTCOUNT;
            ++EXTRALEVELOBJECTCOUNT;
            NuStrCpy(destination, name);
            ExtraLevelObject_NameTableIndex += length + 1;
            return 1;
        }
    }
    return 0;
}

i32 __cdecl LevelObject_FindIndexFromName_RefOnly(char *name) {
    i32 end, i;
    if (ObjTabList == NULL || LEVOBJREF_FIRSTOBJ == -1)
        return -1;
    end = LEVOBJREF_LASTOBJ - LEVOBJREF_FIRSTOBJ + 1;
    for (i = LEVOBJREF_FIRSTOBJ; i < end; ++i)
        if (NuStrICmp(ObjTabList[i].name, name) == 0)
            return i;
    return -1;
}
i32 __cdecl LevelObject_GetReflection(i32 index) {
    if (index != -1 && index >= LEVOBJREF_FIRSTOBJ && index <= LEVOBJREF_LASTOBJ)
        return LEVOBJREF_FIRSTREFOBJ - LEVOBJREF_FIRSTOBJ + index;
    return index;
}

typedef struct startup_level_object_pc_s {
    NUSPECIALHANDLE special;
    i16 platform_id;
    u8 active;
    u8 unknown_f;
} STARTUP_LEVEL_OBJECT_PC;
STATIC_ASSERT(sizeof(STARTUP_LEVEL_OBJECT_PC) == 0x10);
STATIC_ASSERT(offsetof(STARTUP_LEVEL_OBJECT_PC, special) == 0);
STATIC_ASSERT(offsetof(STARTUP_LEVEL_OBJECT_PC, platform_id) == 0xc);
i32 __cdecl FindPlatInst(i32 instance);
void __cdecl LevObj_FixUpPlatIDs(WORLDINFO *world) {
    i32 index;
    if (ObjTabList == NULL)
        return;
    for (index = 0; index < LEVELOBJECTCOUNT; ++index) {
        STARTUP_LEVEL_OBJECT_PC *objects = (STARTUP_LEVEL_OBJECT_PC *)world->level_objects;
        objects[index].platform_id = -1;
        if (world->terrain != NULL && NuSpecialExistsFn(&objects[index].special) && ObjTabList[index].kind == 1)
            objects[index].platform_id = (i16)FindPlatInst(NuSpecialGetInstanceix(&objects[index].special));
    }
}

typedef struct WORLD_OBJECT_SETUP_READER_PC {
    u8 unknown_0000[0x2964];
    NUGSCN *icons_gscn;
} WORLD_OBJECT_SETUP_READER_PC;
STATIC_ASSERT(offsetof(WORLD_OBJECT_SETUP_READER_PC, icons_gscn) == 0x2964);
STATIC_ASSERT(offsetof(WORLDINFO, giz_buffer) == 0x104);
STATIC_ASSERT(offsetof(WORLDINFO, current_gscn) == 0x13c);
STATIC_ASSERT(offsetof(WORLDINFO, area) == 0x12c);
STATIC_ASSERT(offsetof(WORLDINFO, level_objects) == 0x2ac0);
STATIC_ASSERT(offsetof(LEVEL_OBJECT_RUNTIME_PC, active) == 0xe);
extern NUGSCN *area_scene, *vehicle_scene, *big_icon_scene;
extern i32 KNOBS;
void __cdecl LevelObjects_InitForLevel(WORLDINFO *world) {
    i32 index;
    world->giz_buffer.addr = (world->giz_buffer.addr + 3) & ~3u;
    world->level_objects = (LEVEL_OBJECT_RUNTIME_PC *)world->giz_buffer.void_ptr;
    world->giz_buffer.addr += LEVELOBJECTCOUNT * sizeof(LEVEL_OBJECT_RUNTIME_PC);
    memset(world->level_objects, 0, LEVELOBJECTCOUNT * sizeof(LEVEL_OBJECT_RUNTIME_PC));
    if (ObjTabList == NULL)
        return;
    for (index = 0; index < LEVELOBJECTCOUNT; ++index) {
        NUGSCN *scene = NULL;
        LEVEL_OBJECT_RUNTIME_PC *object = &world->level_objects[index];
        LEVEL_OBJECT_CONFIG_PC *type = &ObjTabList[index];
        i32 area_override = 0;
        if (NuStrICmp(type->name, "power_up") == 0)
            KNOBS = index;
        switch ((i8)type->kind) {
            case 1:
                scene = world->current_gscn;
                break;
            case 2:
                scene = area_scene != NULL ? area_scene : world->current_gscn;
                break;
            case 3: {
                i32 character;
                i32 found = 0;
                for (character = 0; character < CHARCOUNT && !found; ++character) {
                    scene = IconScene_FindById(character);
                    if (scene != NULL)
                        found = NuSpecialFind(scene, &object->special, type->name, 1);
                }
                if (found)
                    goto resolved;
                scene = big_icon_scene != NULL ? big_icon_scene : ((WORLD_OBJECT_SETUP_READER_PC *)world)->icons_gscn;
                break;
            }
            case 4:
                scene = save_icon_scene;
                break;
            case 5:
                scene = vehicle_scene;
                break;
            default:
                scene = area_scene;
                if (scene != NULL && world->area != NULL && (world->area->flags & 0x400) != 0)
                    area_override = 1;
                else
                    scene = things_scene;
                break;
        }
        if (scene == NULL)
            continue;
        if (!NuSpecialFind(scene, &object->special, type->name, 1) && area_override)
            NuSpecialFind(things_scene, &object->special, type->name, 1);
    resolved:
        object->active = (u8)NuSpecialExistsFn(&object->special);
        if (object->active != 0)
            NuSpecialSetVisibility(&object->special, 0);
    }
}
