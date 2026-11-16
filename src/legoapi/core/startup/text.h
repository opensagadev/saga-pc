#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/items/base/apiobject.h"

#ifdef __cplusplus
extern "C" {
#endif
    void Text_InitDefaultStrings(void);
    void Text_LoadStrings(VARIPTR *buffer, VARIPTR *buffer_end);
    extern const char Text_CrossCodeword[6];
    void Text_DecodeButtons(char *source, char *destination);
    extern i32 (*Tag_NoHiddenIconFn)(GAMEOBJECT *);
    extern const char *LEGOASCII_UP;
    void TextInputGlyphName(i32, char *);
    i32 TriggerAutoSave(void);
    void Text_FillInExtendedSaveInfo(void);
#ifdef __cplusplus
}
#endif
