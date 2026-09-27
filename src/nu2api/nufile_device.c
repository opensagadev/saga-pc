#include "saga.h"

#include <string.h>

extern i32 numdevices;
extern NUFILE_DEVICE devices[16];
extern i32 DontDoFileUpperCaseHack;

extern void __cdecl NuFileCorrectSlashes(NUFILE_DEVICE *device, char *path);
extern void __cdecl NuFileReldirFix(NUFILE_DEVICE *device, char *path);

NUFILE_DEVICE *__cdecl NuFileGetDeviceFromPath(const char *path) {
    i32 prefix_end;
    i32 device_index;

    for (prefix_end = 0; prefix_end < 8; prefix_end++) {
        if (path[prefix_end] == ':') {
            break;
        }
    }

    if (prefix_end == 8) {
        return NULL;
    }

    for (device_index = 0; device_index < numdevices; device_index++) {
        if (NuStrNICmp(path, devices[device_index].name, devices[device_index].match_len) == 0) {
            return &devices[device_index];
        }
    }

    return NULL;
}

i32 __cdecl DEV_FormatName(NUFILE_DEVICE *device, char *formatted_name, char *path, i32 buf_size) {
    char buffer[512];
    i32 device_name_len;
    char *subpath;
    i32 path_len;
    i32 path_pos;

    device_name_len = NuStrLen(device->name);

    /* PC accepts an already-qualified path unchanged. */
    subpath = path;
    while (*subpath != '\0' && *subpath != ':') {
        subpath++;
    }

    if (*subpath == ':') {
        NuStrCpy(buffer, path);
    } else if (NuStrNICmp(path, device->name, device_name_len) == 0) {
        subpath = path + device_name_len;
        if (*subpath == '\\' || *subpath == '/') {
            NuStrCpy(buffer, path);
        } else {
            NuStrCpy(buffer, device->root);
            NuStrCat(buffer, device->cur_dir);
            NuStrCat(buffer, subpath);
        }
    } else {
        NuStrCpy(buffer, device->root);
        NuStrCat(buffer, device->cur_dir);
        NuStrCat(buffer, path);
    }

    path_pos = NuStrLen(device->root);
    NuFileCorrectSlashes(device, buffer + path_pos);
    NuFileReldirFix(device, buffer);

    path_len = NuStrLen(buffer);
    if (buf_size > path_len) {
        NuStrCpy(formatted_name, buffer);
        return 1;
    }

    return 0;
}

i32 __cdecl DEVHOST_Interrogate(NUFILE_DEVICE *device) {
    device->status = 1;
    return 1;
}

void __cdecl NuFileCorrectSlashes(NUFILE_DEVICE *device, char *path) {
    char separator;
    u8 *cursor;

    separator = '\\';
    if (device != NULL) {
        separator = device->dir_separator;
    }

    cursor = (u8 *)path;
    while (*cursor != 0) {
        if (DontDoFileUpperCaseHack != 0) {
            if (*cursor == '/' || *cursor == '\\') {
                *cursor = (u8)separator;
            }
        } else if (*cursor == '/' || *cursor == '\\') {
            *cursor = (u8)separator;
        } else if (*cursor >= 'a' && *cursor <= 'z') {
            *cursor = (u8)(*cursor - ('a' - 'A'));
        }
        cursor++;
    }
}

void __cdecl NuFileReldirFix(NUFILE_DEVICE *device, char *path) {
    char separator;
    char *source;
    char *destination;

    separator = device->dir_separator;
    source = path;
    destination = path;

    while (*source != '\0') {
        if (source > path && source[0] == separator && source[1] == '.' && source[2] == '.' && source[3] == separator) {
            char *component = source;

            while (component > path && component[-1] != separator && component[-1] != ':') {
                component--;
            }

            if (component > path && component[-1] == separator) {
                destination = component - 1;
                source += 3;
                continue;
            }
            if (component > path && component[-1] == ':') {
                destination = component;
                source += 4;
                continue;
            }
        }

        *destination++ = *source++;
    }
    *destination = '\0';
}

NUFILE_DEVICE *__cdecl NuFileAddDevice(NUFILE_DEVICE *device) {
    i32 device_index;
    NUFILE_DEVICE *registered_device;
    char *destination;
    const char *source;

    device_index = numdevices;
    if (device_index >= 16) {
        NuFileError();
    }

    registered_device = &devices[device_index];
    *registered_device = *device;

    destination = registered_device->cur_dir;
    source = default_device->cur_dir;
    do {
        *destination++ = *source;
    } while (*source++ != '\0');

    destination = registered_device->sys_dir;
    source = default_device->sys_dir;
    do {
        *destination++ = *source;
    } while (*source++ != '\0');

    destination = registered_device->dll_dir;
    source = default_device->dll_dir;
    do {
        *destination++ = *source;
    } while (*source++ != '\0');

    numdevices = device_index + 1;
    return registered_device;
}

i32 __cdecl NuFileGetCurrentDirectory(char *destination) {
    const char *source;

    source = default_device->cur_dir;
    if (source != NULL) {
        while (*source != '\0') {
            *destination++ = *source++;
        }
    }
    *destination = '\0';
    return 0;
}

void __cdecl NuFileSetCurrentDirectory(const char *path) {
    char separator[2] = {'\\', '\0'};
    char *destination;
    char *destination_start;
    const char *source;

    destination_start = default_device->cur_dir;
    if (*path == '\0') {
        *destination_start = '\0';
        return;
    }

    destination = destination_start;
    source = path;
    do {
        *destination++ = *source;
    } while (*source++ != '\0');

    if (destination[-2] != '\\' && destination[-2] != '/') {
        separator[0] = (char)default_device->dir_separator;
        separator[1] = '\0';
        NuStrCat(default_device->cur_dir, separator);
    }
}

void __cdecl NuFileGetCurrentDllPath(char *destination) {
    i32 root_length;

    NuStrCpy(destination, default_device->root);
    NuStrCat(destination, default_device->dll_dir);
    root_length = NuStrLen(default_device->root);
    NuFileCorrectSlashes(default_device, destination + root_length);
}

void __cdecl NuFileSetCurrentDllDirectory(const char *path) {
    if (*path != '\0') {
        NuStrCpy(default_device->dll_dir, path);
        NuStrCat(default_device->dll_dir, "\\");
    } else {
        default_device->dll_dir[0] = '\0';
    }
}
