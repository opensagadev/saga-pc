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
