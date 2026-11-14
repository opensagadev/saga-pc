#include "saga.h"

void NuPadSetMotorsPc(NUPAD_INIT_STATE *pad, i32 motor0, i32 motor1) {
    (void)pad;
    (void)motor0;
    (void)motor1;
}

void fun_006de6e0(void) {
}

void fun_006de6f0(void) {
}

void fun_006de700(void) {
}

i32 fun_006de710(void) {
    return 0;
}

void fun_006de720(void) {
}

i32 NuPs2ApplyDeadZone(i32 v, i32 deadrng) {
    i32 dx;

    v -= 128;
    if (v > deadrng) {
        dx = (v - deadrng) * 127 / (127 - deadrng);
    } else if (v < -deadrng) {
        dx = (v + deadrng) * 127 / (127 - deadrng);
    } else {
        dx = 0;
    }
    return dx;
}

i32 fun_006dec50(void) {
    return 1;
}

i32 fun_006dec60(void) {
    return 1;
}

void fun_006dec70(void) {
}

void fun_006dec80(void) {
}

i32 fun_006dec90(void) {
    return 0;
}

i32 fun_006deca0(void) {
    return 0;
}

i32 fun_006decb0(void) {
    return 0;
}

i32 fun_006decc0(void) {
    return 0;
}

i32 fun_006decd0(void) {
    return 0;
}

void fun_006dece0(void) {
}

i32 fun_006decf0(i32 *value) {
    *value = 0;
    return 0;
}
