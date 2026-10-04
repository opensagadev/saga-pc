#include "saga.h"
void Minicam_AddCommand(i32 type, f32 duration, i32 argument, void *target, NUVEC position) {
    if (type == 6 || type == 7) {
        i32 i = MiniCam.command_count;
        for (; i >= 0 && MiniCam.duplicate_search[i].type != type; --i) {
        }
        if (i != -1 && MiniCam.duplicate_search[i].argument == argument)
            return;
    }
    if (MiniCam.command_count == 32)
        return;
    switch (type) {
        case 1:
            MiniCam.commands[MiniCam.command_count].type = 1;
            MiniCam.commands[MiniCam.command_count].argument = 0;
            MiniCam.commands[MiniCam.command_count].duration = duration;
            MiniCam.commands[MiniCam.command_count].position = v000;
            MiniCam.commands[MiniCam.command_count].target = NULL;
            break;
        case 3:
            MiniCam.commands[MiniCam.command_count].type = 3;
            MiniCam.commands[MiniCam.command_count].argument = 0;
            MiniCam.commands[MiniCam.command_count].duration = duration;
            MiniCam.commands[MiniCam.command_count].position = v000;
            MiniCam.commands[MiniCam.command_count].target = NULL;
            break;
        case 4:
            MiniCam.commands[MiniCam.command_count].type = 4;
            MiniCam.commands[MiniCam.command_count].argument = 0;
            MiniCam.commands[MiniCam.command_count].duration = duration;
            MiniCam.commands[MiniCam.command_count].position = v000;
            MiniCam.commands[MiniCam.command_count].target = NULL;
            break;
        case 5:
            MiniCam.commands[MiniCam.command_count].type = 5;
            MiniCam.commands[MiniCam.command_count].argument = 0;
            MiniCam.commands[MiniCam.command_count].duration = duration;
            MiniCam.commands[MiniCam.command_count].position = v000;
            MiniCam.commands[MiniCam.command_count].target = NULL;
            break;
        case 6:
            MiniCam.commands[MiniCam.command_count].type = 6;
            MiniCam.commands[MiniCam.command_count].argument = argument;
            MiniCam.commands[MiniCam.command_count].duration = 0.0f;
            MiniCam.commands[MiniCam.command_count].position = v000;
            MiniCam.commands[MiniCam.command_count].target = NULL;
            break;
        case 7:
            MiniCam.commands[MiniCam.command_count].type = 7;
            MiniCam.commands[MiniCam.command_count].argument = argument;
            MiniCam.commands[MiniCam.command_count].duration = 0.0f;
            MiniCam.commands[MiniCam.command_count].position = v000;
            MiniCam.commands[MiniCam.command_count].target = NULL;
            break;
        case 8:
            MiniCam.commands[MiniCam.command_count].type = 8;
            MiniCam.commands[MiniCam.command_count].argument = 0;
            MiniCam.commands[MiniCam.command_count].duration = 0.0f;
            MiniCam.commands[MiniCam.command_count].position = v000;
            MiniCam.commands[MiniCam.command_count].target = (NUVEC *)target;
            break;
        case 9:
            MiniCam.commands[MiniCam.command_count].type = 9;
            MiniCam.commands[MiniCam.command_count].argument = 0;
            MiniCam.commands[MiniCam.command_count].duration = duration;
            MiniCam.commands[MiniCam.command_count].position = v000;
            MiniCam.commands[MiniCam.command_count].target = NULL;
            break;
        case 10:
            MiniCam.commands[MiniCam.command_count].type = 10;
            MiniCam.commands[MiniCam.command_count].argument = argument;
            MiniCam.commands[MiniCam.command_count].duration = 0.0f;
            MiniCam.commands[MiniCam.command_count].position = v000;
            MiniCam.commands[MiniCam.command_count].target = NULL;
            break;
        case 11:
            MiniCam.commands[MiniCam.command_count].type = 11;
            MiniCam.commands[MiniCam.command_count].argument = argument;
            MiniCam.commands[MiniCam.command_count].duration = 0.0f;
            MiniCam.commands[MiniCam.command_count].position = v000;
            MiniCam.commands[MiniCam.command_count].target = NULL;
            break;
        case 12:
            MiniCam.commands[MiniCam.command_count].type = 12;
            MiniCam.commands[MiniCam.command_count].argument = argument;
            MiniCam.commands[MiniCam.command_count].duration = 0.0f;
            MiniCam.commands[MiniCam.command_count].position = v000;
            MiniCam.commands[MiniCam.command_count].target = NULL;
            break;
        case 13:
            MiniCam.commands[MiniCam.command_count].type = 13;
            MiniCam.commands[MiniCam.command_count].argument = argument;
            MiniCam.commands[MiniCam.command_count].duration = 0.0f;
            MiniCam.commands[MiniCam.command_count].position = v000;
            MiniCam.commands[MiniCam.command_count].target = NULL;
            break;
        case 14:
            MiniCam.commands[MiniCam.command_count].type = 14;
            MiniCam.commands[MiniCam.command_count].argument = argument;
            MiniCam.commands[MiniCam.command_count].duration = 0.0f;
            MiniCam.commands[MiniCam.command_count].position = v000;
            MiniCam.commands[MiniCam.command_count].target = NULL;
            break;
        case 15:
            MiniCam.commands[MiniCam.command_count].type = 15;
            MiniCam.commands[MiniCam.command_count].argument = argument;
            MiniCam.commands[MiniCam.command_count].duration = 0.0f;
            MiniCam.commands[MiniCam.command_count].position = v000;
            MiniCam.commands[MiniCam.command_count].target = NULL;
            break;
        case 16:
            MiniCam.commands[MiniCam.command_count].type = 16;
            MiniCam.commands[MiniCam.command_count].argument = 0;
            MiniCam.commands[MiniCam.command_count].duration = 0.0f;
            MiniCam.commands[MiniCam.command_count].position = position;
            MiniCam.commands[MiniCam.command_count].target = NULL;
            break;
        case 17:
            MiniCam.commands[MiniCam.command_count].type = 17;
            MiniCam.commands[MiniCam.command_count].argument = 0;
            MiniCam.commands[MiniCam.command_count].duration = 0.0f;
            MiniCam.commands[MiniCam.command_count].position = v000;
            MiniCam.commands[MiniCam.command_count].target = (NUVEC *)target;
            break;
    }
    ++MiniCam.command_count;
}
