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
