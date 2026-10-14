#include "../saga.h"
/* PC594c301198bytes,cdecl7args. Native rows=count/columns+1 always. */
extern "C" f32 COLLECTION_DX, COLLECTION_DY, COLLECTION_ICONSIZE;
extern "C" i32 LEGOOBJ_ICON_FRAME_NEUTRAL, LEGOOBJ_ICON_FRAME_GREEN, LEGOOBJ_ICON_FRAME_BLUE;
extern "C" void Collection_DrawPc(COLLECTION_BUILD_PC *collection, f32 x, f32 y, f32 scale,
                                  APICHARACTERMODELLIST *models, f32 alpha, i32 hide_selected) {
    f32 dx = COLLECTION_DX * scale;
    const f32 dy = COLLECTION_DY * scale;
    f32 size = COLLECTION_ICONSIZE * scale;
    if (collection->list == NULL || ((FADEINFO_PC *)&FadeSys)->fade > 0.0f)
        return;
    const i32 columns = collection->columns, count = collection->count;
    if (columns == 0 || count == 0)
        return;
    const i32 rows = count / columns + 1;
    if (Game_OptionsSave != NULL && Game_OptionsSave->widescreen != 0) {
        dx = (f32)(dx * .75);
        size = (f32)(size * .875);
    }
    i32 selected_x[2] = {-1, -1}, selected_y[2] = {-1, -1};
    if (models != NULL && Collection_GetSelectingPlayerIDsFn != NULL) {
        i16 ids[2] = {-1, -1};
        Collection_GetSelectingPlayerIDsFn(ids);
        if (ids[0] != -1 || ids[1] != -1) {
            i32 found = 0;
            for (i32 row = 0; row < rows; ++row)
                for (i32 col = 0; col < columns; ++col) {
                    const i32 index = row * columns + col;
                    if (found != 2 && index < count &&
                        (collection->list[index].id == ids[0] || collection->list[index].id == ids[1])) {
                        selected_x[found] = col;
                        selected_y[found] = row;
                        ++found;
                    }
                }
        }
    }
    if (alpha > 1.0f)
        alpha = 1.0f;
    f32 py = (f32)(y - (rows - 1) * dy * .5);
    const f32 first_x = (f32)(x - (columns - 1) * dx * .5);
    for (i32 row = 0; row < rows; ++row) {
        f32 px = first_x;
        for (i32 col = 0; col < columns; ++col) {
            const i32 index = row * columns + col;
            if (index < count) {
                COLLECTID_PC *entry = &collection->list[index];
                i32 id = entry->id;
                entry->grid_x = px;
                entry->grid_y = py;
                if (!(alpha <= 0.0f)) {
                    i32 frame = LEGOOBJ_ICON_FRAME_NEUTRAL;
                    f32 opacity = 1.0f;
                    i32 model_index;
                    if (models != NULL && InModelList(models, id, &model_index) != 0) {
                        if (hide_selected != 0) {
                            px += dx;
                            continue;
                        }
                        if (model_index == 0 || model_index == 1) {
                            frame = model_index == 0 ? LEGOOBJ_ICON_FRAME_BLUE : LEGOOBJ_ICON_FRAME_GREEN;
                            opacity = (f32)(NuTrigTable[((i32)(GlobalTimer.time_elapsed_mod_seconds * 65536.0) >> 1) &
                                                        0x7fff] *
                                                .125 +
                                            .375);
                        } else
                            opacity = .25f;
                    }
                    if (Collection_Got(id) == 0) {
                        opacity = (f32)(opacity * .25);
                        id = -1;
                    }
                    opacity *= alpha;
                    if (opacity > 0.0f) {
                        u32 neighbours = 0;
                        for (i32 i = 0; i < 2; ++i) {
                            if (selected_x[i] == -1 || selected_y[i] == -1)
                                continue;
                            const i32 cx = selected_x[i], cy = selected_y[i];
                            if ((col == cx && (row == cy - 1 || row == cy + 1)) ||
                                (row == cy && (col == cx - 1 || col == cx + 1)))
                                neighbours |= 1;
                            else if ((col == cx - 1 || col == cx + 1) && (row == cy - 1 || row == cy + 1))
                                neighbours |= 2;
                            else if ((col == cx && (row == cy - 2 || row == cy + 2)) ||
                                     (row == cy && (col == cx - 2 || col == cx + 2)))
                                neighbours |= 4;
                        }
                        if ((neighbours & 1) != 0)
                            opacity = (f32)(opacity * .3330000042915344);
                        else if ((neighbours & 2) != 0)
                            opacity = (f32)(opacity * .5);
                        else if ((neighbours & 4) != 0)
                            opacity = (f32)(opacity * .6660000085830688);
                        DrawCharIcon(id, px, py, .002f, size, frame, opacity, opacity, 1, NULL);
                    }
                }
            }
            px += dx;
        }
        py += dy;
    }
}

/* PC434930227bytes,cdecl(model,object,buffer). Native name trim scans fixed14..0. */
extern "C" char *GameObj_GetNamePc(i32 model, GAMEOBJECT *object, char *buffer) {
    if (object != NULL) {
        if (object->rider != NULL && object->rider->character_data->name_id != -1)
            model = object->rider->id;
        else
            model = object->id;
    } else if (model == -1)
        return TTab[tUNKNOWN];
    if (buffer != NULL) {
        i32 custom = -1;
        if (model == PermanentWeirdo1IdPc) {
            if (Game.customiser_start[0].use_saved_name != 0)
                custom = 0;
        } else if (model == PermanentWeirdo2IdPc) {
            if (Game.customiser_start[1].use_saved_name != 0)
                custom = 1;
        }
        if (custom != -1) {
            NuStrCpy(buffer, Game.customiser_start[custom].name);
            i32 i = 14;
            while (buffer[i] == ' ') {
                --i;
                buffer[i + 1] = 0;
                if (i < 0)
                    break;
            }
            if (i < 0)
                NuStrCpy(buffer, "?");
            return buffer;
        }
    }
    return TTab[CDataList[model].name_id];
}
