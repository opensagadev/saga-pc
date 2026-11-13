#include "saga.h"

extern "C" {

    void SetLevelExBlowupFlags(u32 flags) {
        EXBLOWUPFLAGS = flags;
    }

    void __cdecl SetLevelBlowupCallbackPc(LEVEL_BLOWUP_CALLBACK_PC callback) {
        if (callback != NULL)
            BlowupExFunc = callback;
    }
}
