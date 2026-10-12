#include "saga.h"

i32 __cdecl Hub_Outside(void) {
    return WORLD->current_level == HUB_LDATA && HubAiMusicStatePc.area == 7;
}
static void Hub_MakeListCharactersAvailable(i16 *characters) {
    i32 character;
    struct gameobject_s *object;
    i32 object_index;
    i32 pack;
    for (character = 0; character < CHARCOUNT; ++character) {
        characters[character] = apicharsys->playermodelids[character];
    }

    object = Obj;
    for (object_index = 0; object_index < HIGHGAMEOBJECT; ++object_index, ++object) {
        if ((object->flags_1fc_half & APIOBJECT_FLAG_IN_USE) != 0) {
            characters[object->id] = -1;
        }
    }

    Hub_MarkCharacterUnavailable(characters, id_BARMAN);
    Hub_MarkCharacterUnavailable(characters, id_TRAININGREMOTE);
    Hub_MarkCharacterUnavailable(characters, id_JABBA);
    Hub_MarkCharacterUnavailable(characters, id_WOMPRAT);
    Hub_MarkCharacterUnavailable(characters, id_WEIRDO1);
    Hub_MarkCharacterUnavailable(characters, id_WEIRDO2);
    Hub_MarkCharacterUnavailable(characters, id_CANTINABAND);
    Hub_MarkCharacterUnavailable(characters, id_DROIDEKA);
    Hub_MarkCharacterUnavailable(characters, id_BOB);
    Hub_MarkCharacterUnavailable(characters, id_WHIP);

    for (pack = 0; pack < 11; ++pack) {
        i16 *character = StorePack[pack].id;
        if (character != NULL) {
            i16 character_id = *character;
            if (character_id != -1 && Store_IsPackUnlocked(pack) == 0) {
                characters[character_id] = -1;
            }
        }
    }
}
