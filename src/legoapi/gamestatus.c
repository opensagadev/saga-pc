#include "saga.h"

void __cdecl SetBonusWinner(i32 player_index) {
    BonusWinner = player_index;
    GameCam_Blend(GameCam, 1.5f, 0.0f, 1);
    LookAtBoth = 1;
}
