#include "saga.h"
void __cdecl LetGoOfBalloon(GAMEOBJECT *);
void __cdecl PopBalloon(GAMEOBJECT *object) {
    LetGoOfBalloon(object);
}
