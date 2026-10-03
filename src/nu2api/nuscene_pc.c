#include "saga.h"
#include <string.h>

i32 __cdecl PCFrameLoadingYieldEnabled(void) {
    return PCFrameLoadingYieldEnabledPC;
}

void __cdecl NuGScnCreatePS(NUGSCN *scene, VARIPTR *cursor, VARIPTR *end) {
    i32 i;
    (void)cursor;
    (void)end;
    for (i = 0; i != scene->texture_count; ++i) {
        scene->textures[i]->resource = SceneVideoResourcesPC.textures[i];
        scene->textures[i]->image_data = NULL;
        scene->textures[i]->image_size = 0;
    }
}
