#include "gameframework/saveload.h"
#include "legoapi/items/collect/bolts.h"
#include "nu2api/nufile_pc.h"
#include "nu2api/nuwin32/d3dApiCalls.h"
#include "nu2api/platform_files.h"
#include "nu2api/save_folder.h"
#include <stdio.h>
#include <string.h>

extern "C" {
    u8 PcSaveHeaderMetadata[16] = {0};

    void PcRegisterSaveMetadata(const void *metadata) {
        if (metadata != NULL) {
            PCByteCopy(PcSaveHeaderMetadata, metadata, 16);
        }
    }
}
