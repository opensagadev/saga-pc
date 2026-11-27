#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nucore/nuthread.h"
#include "nu2api/nucore/nutime.h"
#include "nu2api/nudir_gen.h"
#include "nu2api/numainfilter_gen.h"
#include "nu2api/numath/numaths.h"

void PCDisplayListCriticalSectionEnter(void) {
    NuThreadCriticalSectionBegin(nudlist_field_fc4);
}

void PCDisplayListCriticalSectionLeave(void) {
    NuThreadCriticalSectionEnd(nudlist_field_fc4);
}

void NuDisplayQueueMaterialAdd(NUMTL *material) {
    NuThreadCriticalSectionBegin(nudlist_field_fc4);
    if (PCDisplayMaterialAddCount == 600) {
        NuThreadCriticalSectionEnd(nudlist_field_fc4);
        AnimationDiagnosticNoOp();
        return;
    }
    PCDisplayMaterialAddQueue[PCDisplayMaterialAddCount++] = material;
    NuThreadCriticalSectionEnd(nudlist_field_fc4);
}
