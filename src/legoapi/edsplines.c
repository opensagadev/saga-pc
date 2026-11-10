#include "saga.h"

#define SPLINE_POINT(spline, index) ((NUVEC *)((u8 *)(spline)->pts + (index) * (i16)(spline)->pt_size))

#undef SPLINE_POINT

extern i32 RenderHubFromCutscenePlayerPc;
extern const char RenderSplineBountyStartPc[13];
extern const char RenderSplineShopStartPc[11];
STATIC_ASSERT(offsetof(WORLDINFO, camera_splines) == 0x2ac4);
STATIC_ASSERT(offsetof(LEVELSPLINE_PC, scene) == 0);
STATIC_ASSERT(offsetof(LEVELSPLINE_PC, max_points) == 0xa);
STATIC_ASSERT(offsetof(LEVELSPLINE_PC, level) == 0xc);
void __cdecl LevelSplines_InitForLevel(WORLDINFO *world) {
    i32 i;
    world->giz_buffer.addr = (world->giz_buffer.addr + 3u) & ~3u;
    world->camera_splines = (NUGSPLINE **)world->giz_buffer.void_ptr;
    world->giz_buffer.addr += LEVELSPLINECOUNT * sizeof(*world->camera_splines);
    memset(world->camera_splines, 0, LEVELSPLINECOUNT * sizeof(*world->camera_splines));
    if (LevSplList == NULL)
        return;
    for (i = 0; i < LEVELSPLINECOUNT; ++i) {
        LEVELSPLINE_PC *entry = &LevSplList[i];
        NUGSCN *scene;
        NUGSPLINE *spline;
        char name[64];
        if ((entry->area != -1 && world->area_index != entry->area) ||
            (entry->level != -1 && world->level_idx != entry->level))
            continue;
        scene = *(entry->scene != NULL ? entry->scene : &world->current_gscn);
        if (scene != NULL) {
            spline = NuSplineFind(scene, (char *)entry->name);
            world->camera_splines[i] = spline;
            if (spline != NULL && ((entry->min_points > 0 && spline->length < entry->min_points) ||
                                   (entry->max_points > 0 && entry->max_points >= entry->min_points &&
                                    spline->length > entry->max_points)))
                world->camera_splines[i] = NULL;
        }
        if (levspl_i_start != -1) {
            name[0] = '\0';
            if (Mission_Active(NULL) != NULL)
                NuStrCpy(name, RenderSplineBountyStartPc);
            else if (world->area_index != -1 && (ADataList[world->area_index].flags & 0x40) &&
                     RenderHubFromCutscenePlayerPc != 0)
                NuStrCpy(name, RenderSplineShopStartPc);
            if (name[0] != '\0') {
                spline = NuSplineFind(scene, name);
                if (spline != NULL && spline->length >= 2) {
                    world->camera_splines[levspl_i_start] = spline;
                    if (levspl_i_startcam != -1)
                        world->camera_splines[levspl_i_startcam] = NULL;
                }
            }
        }
    }
}
