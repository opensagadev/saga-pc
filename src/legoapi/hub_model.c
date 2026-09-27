#include "saga.h"

void __cdecl Hub_MakeModelList(void) {
    i32 count;
    i16 *model_id;

    count = 0;
    Hub_ModelList[count].model_id = (i16)PlayerID[0];
    Hub_ModelList[count].count = 1;
    count++;
    Hub_ModelList[count].model_id = (i16)PlayerID[1];
    Hub_ModelList[count].count = 1;
    count++;
    if (HUB_ADATA != NULL) {
        model_id = HUB_ADATA->hub_player_ids;
        if (model_id != NULL) {
            while (count < 5 && *model_id != -1) {
                Hub_ModelList[count].model_id = *model_id++;
                Hub_ModelList[count].count = 1;
                count++;
            }
        }
    }
    Hub_ModelList[count].model_id = -1;

    PlayerList[0] = Hub_ModelList[0].model_id;
    PlayerList[1] = Hub_ModelList[1].model_id;
    PlayerList[2] = -1;
}
