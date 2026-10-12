#include "saga.h"

void edGraEnableTerrainSwap(void) {
}

void edGraDisableTerrainSwap(void) {
}

void edGraInitTerrainSwapProtection(void) {
}

void edgracbSetGlobalFadeIn(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    edui_slider_s *slider = globalfadeoutitem;
    edgra_global_fadein = ((edui_slider_s *)(item))->value;
    if (edgra_global_fadein > edgra_global_fadeout) {
        edgra_global_fadeout = edgra_global_fadein;
        slider->value = edgra_global_fadeout;
        slider->normalized_value = (slider->value - slider->minimum) / slider->range;
    }
}

void edgracbSetGlobalFadeOut(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    edui_slider_s *slider = globalfadeinitem;
    edgra_global_fadeout = ((edui_slider_s *)(item))->value;
    if (edgra_global_fadein > edgra_global_fadeout) {
        edgra_global_fadein = edgra_global_fadeout;
        slider->value = edgra_global_fadein;
        slider->normalized_value = (slider->value - slider->minimum) / slider->range;
    }
}

void edgracbCancelGlobalsMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(edgra_globals_menu);
    edgra_globals_menu = NULL;
}

static void edgraRender() {
    edcamSet();
    edgraDrawCursor();
    if (edgra_nearest != -1) {
        PC_EDGRACLUMP *clump = &(GrassClumps[edgra_nearest]);
        if ((*clump).kind == 3) {
            if (edgra_editormode == 1) {
                edbitsDrawDiagonalCross((*clump).position.x, (*clump).position.y, (*clump).position.z, 0.125f,
                                        0xffff0000, edgra_mtl);
                if (edgra_nearest_instance != -1) {
                    PC_EDGRAINDIVIDUAL *individual =
                        GetIndGrassClump((*clump).individual_index, edgra_nearest_instance);
                    edbitsDrawCross((*clump).position.x + individual->position.x,
                                    (*clump).position.y + individual->position.y,
                                    (*clump).position.z + individual->position.z, 0.125f, 0xff80ffff, edgra_mtl);
                }
            } else {
                edbitsDrawDiagonalCross((*clump).position.x, (*clump).position.y, (*clump).position.z, 0.125f,
                                        0xff80ffff, edgra_mtl);
            }
        } else {
            edbitsDrawDiagonalCross((*clump).position.x, (*clump).position.y, (*clump).position.z, 0.125f, 0xff80ffff,
                                    edgra_mtl);
            if (((u8)((*clump).unknown_25 - 3)) < 2) {
                edbitsDrawCube((*clump).position.x, (*clump).position.y, (*clump).position.z, (*clump).size, 0.0f,
                               (*clump).size, 0, 0, 0, (*clump).rotation_z, (*clump).rotation_y, 0xff80ffff, edgra_mtl);
            } else {
                edbitsDrawCircleTilted(&(*clump).position, (*clump).size, 0xff80ffff, edgra_mtl, (*clump).rotation_z,
                                       (*clump).rotation_y);
            }
        }
    }
    if (edgra_active_menu)
        eduiMenuRender(edgra_active_menu);
}
