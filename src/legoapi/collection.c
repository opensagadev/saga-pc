#include "saga.h"

static i32 CollectCount;
static COLLECTID_PC *CollectList;
COLLECTID_PC *TempCollectID;

i32 __cdecl InCollectList_Index(i32 id, COLLECTID_PC *list, i32 count) {
    i32 index;
    TempCollectID = NULL;
    if (list == NULL) {
        list = CollectList;
        count = CollectCount;
        if (list == NULL)
            return id;
    }
    for (index = 0; index < count; ++index, ++list) {
        if (list->id == id) {
            TempCollectID = list;
            return index;
        }
    }
    return -1;
}
