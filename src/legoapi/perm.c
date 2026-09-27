#include "saga.h"

void __cdecl SetBackgroundMusic(i32 track) {
    (void)track;
}

void __cdecl LoadPerm(void) {
    WORLDINFO *saved_world;

    SetBackgroundMusic(1);
    saved_world = WORLD;
    PermDataLoaded = 0;
    WORLD = NULL;

    if (BGLOAD != 0 && LOADEROFF == 0) {
        FRAMETIME = DEFAULTFRAMETIME;

        // PC copies the camera identity matrix and prepares panel/font coordinates.
        STUBBED_NAMED("LoadPerm: camera, panel and save-icon setup");
        if (PAL == 0) {
            LoadPerm_LanguageSelect = 3;
        }

        STUBBED_NAMED("LoadPerm: bgPostRequest(LoadPermData)");
        // The real loop waits for assets while advancing save-icon, language,
        // legal-screen and intro-text stages. Those dependencies are unresolved.
        STUBBED_NAMED("LoadPerm: input, menu and loading-screen frame loop");
    } else {
        LoadPerm_LanguageSelect = 3;
        STUBBED_NAMED("LoadPerm: synchronous LoadPermData");
    }

    if (legal_mtl != NULL) {
        STUBBED_NAMED("LoadPerm: NuMtlDestroy(legal_mtl)");
        legal_mtl = NULL;
    }
    WORLD = saved_world;
}
