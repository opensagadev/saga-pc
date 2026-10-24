#include "saga.h"

i32 EduiUsingMenuFocusPc = 0;

static eduimenu_s *EduiDeepestChildPc(eduimenu_s *menu) {
    if (menu != NULL) {
        if (menu->child != NULL)
            return EduiDeepestChildPc(menu->child);
        return menu;
    }
    return NULL;
}

void eduiSetActiveMenu(eduimenu_s *menu) {
    if (EduiUsingMenuFocusPc != 0)
        active_menu = EduiDeepestChildPc(menu != NULL ? menu : default_active_menu);
}

i32 __cdecl eduiMenuDetach(eduimenu_s *menu) {
    if (menu->parent != NULL) {
        if (menu->child != NULL)
            eduiMenuDetach(menu->child);
        if (active_menu == menu)
            active_menu = menu->parent;
        menu->parent->child = NULL;
        menu->parent = NULL;
        return 1;
    }
    eduiSetActiveMenu(NULL);
    return 0;
}

i32 eduiCheckForPadMenuCancel(eduimenu_s *menu, FRAME_PAD_PC *pad) {
    struct eduimenu_s *parent;
    if (!pad || !(pad->digital_buttons_pressed & 0x10))
        return 0;

    parent = NULL;
    if (menu->parent != NULL)
        parent = menu->parent;
    if (EduiUsingMenuFocusPc == 0 && parent)
        eduiMenuDetach(menu);
    if (menu->callback)
        menu->callback(menu, parent);
    return 1;
}
