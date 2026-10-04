#include "saga.h"

static i32 edfile_media;
static void *edfile_pakfile;
i32 edfile_handle = -1;
i32 edfile_write_flag;
char edfile_buffer[0x1000];
i32 edfile_buffer_pointer;
i32 edfile_lock[2];
i32 edfile_readwrongendianess;

extern i32 __cdecl NuFileWrite(NUFILE file, const void *data, i32 size);

void __cdecl EdFileSetMedia(i32 media) {
    edfile_media = media;
}

void __cdecl EdFileSetPakFile(void *pak) {
    edfile_pakfile = pak;
}

void __cdecl EdFileSetReadWrongEndianess(i32 value) {
    (void)value;
}

void __cdecl EdFileResetBuffers(void) {
    memset(edfile_buffer, 0, sizeof(edfile_buffer));
    edfile_buffer_pointer = 0;
    edfile_lock[1] = 0;
    edfile_lock[0] = 0;
}

__declspec(noinline) void __cdecl EdFileFillBuffer(void) {
    if (edfile_handle != -1) {
        memset(edfile_buffer, 0, sizeof(edfile_buffer));
        switch (edfile_media) {
            case 1:
                NuFileRead(edfile_handle, edfile_buffer, sizeof(edfile_buffer));
                edfile_buffer_pointer = 0;
                break;
            default:
                break;
        }
    }
}

i32 __cdecl EdFileOpen(char *filepath, i32 mode) {
    i32 item_handle;
    void *item;
    i32 item_size;

    if (edfile_handle != -1) {
        return 0;
    }
    EdFileResetBuffers();
    edfile_handle = -1;
    edfile_write_flag = 0;
    if (edfile_media != 1) {
        return 0;
    }
    if (mode != 0) {
        if (mode != 1) {
            return 0;
        }
        edfile_write_flag = 1;
        edfile_handle = NuFileOpen(filepath, 1);
        if (edfile_handle > 0) {
            return 1;
        }
        edfile_handle = -1;
        return 0;
    }
    if (edfile_pakfile != NULL) {
        item_handle = NuFilePakGetItem(edfile_pakfile, filepath);
        if (item_handle != 0) {
            NuFilePakGetItemInfo(edfile_pakfile, item_handle, &item, &item_size);
            edfile_handle = NuMemFileOpen(item, item_size, 0);
            if (edfile_handle > 0) {
                EdFileFillBuffer();
                return 1;
            }
            edfile_handle = -1;
        }
    }
    edfile_handle = NuFileOpen(filepath, 0);
    if (edfile_handle > 0) {
        EdFileFillBuffer();
        return 1;
    }
    edfile_handle = -1;
    return 0;
}

__declspec(noinline) void __cdecl EdFileFlushBuffer(void) {
    if (edfile_handle != -1 && edfile_buffer_pointer != 0 && edfile_media == 1) {
        NuFileWrite(edfile_handle, edfile_buffer, edfile_buffer_pointer);
        edfile_buffer_pointer = 0;
    }
}

i32 __cdecl EdFileClose(void) {
    edfile_pakfile = NULL;
    if (edfile_handle == -1) {
        return 0;
    }
    if (edfile_write_flag != 0) {
        EdFileFlushBuffer();
    }
    if (edfile_media != 1) {
        return 0;
    }
    NuFileClose(edfile_handle);
    edfile_handle = -1;
    edfile_media = 0;
    return 1;
}

__declspec(noinline) void __cdecl EdFileRead(void *buffer, i32 length) {
    u8 *destination = (u8 *)buffer;
    i32 count;
    while (length > 0) {
        count = sizeof(edfile_buffer) - edfile_buffer_pointer;
        if (length < count) {
            count = length;
        }
        memcpy(destination, edfile_buffer + edfile_buffer_pointer, count);
        edfile_buffer_pointer += count;
        destination += count;
        length -= count;
        if (edfile_buffer_pointer == sizeof(edfile_buffer)) {
            if (edfile_handle != -1) {
                memset(edfile_buffer, 0, sizeof(edfile_buffer));
                if (edfile_media == 1) {
                    NuFileRead(edfile_handle, edfile_buffer, sizeof(edfile_buffer));
                    edfile_buffer_pointer = 0;
                }
            }
        }
    }
}

i32 __cdecl EdFileReadInt(void) {
    union {
        i32 value;
        struct {
            u8 first;
            u8 second;
            u8 third;
            u8 fourth;
        } bytes;
    } data;
    u8 temporary;
    EdFileRead(&data.value, sizeof(data.value));
    if (edfile_readwrongendianess != 0) {
        temporary = data.bytes.first;
        data.bytes.first = data.bytes.fourth;
        data.bytes.fourth = temporary;
        temporary = data.bytes.second;
        data.bytes.second = data.bytes.third;
        data.bytes.third = temporary;
    }
    return data.value;
}

f32 __cdecl EdFileReadFloat(void) {
    union {
        f32 value;
        struct {
            u8 first;
            u8 second;
            u8 third;
            u8 fourth;
        } bytes;
    } data;
    u8 temporary;
    EdFileRead(&data.value, sizeof(data.value));
    if (edfile_readwrongendianess != 0) {
        temporary = data.bytes.first;
        data.bytes.first = data.bytes.fourth;
        data.bytes.fourth = temporary;
        temporary = data.bytes.second;
        data.bytes.second = data.bytes.third;
        data.bytes.third = temporary;
    }
    return data.value;
}

i16 __cdecl EdFileReadShort(void) {
    union {
        i16 value;
        struct {
            u8 first;
            u8 second;
        } bytes;
    } data;
    u8 temporary;
    EdFileRead(&data.value, sizeof(data.value));
    if (edfile_readwrongendianess != 0) {
        temporary = data.bytes.first;
        data.bytes.first = data.bytes.second;
        data.bytes.second = temporary;
    }
    return data.value;
}

void __cdecl EdFileReadNuVec(NUVEC *out) {
    out->x = EdFileReadFloat();
    out->y = EdFileReadFloat();
    out->z = EdFileReadFloat();
}

char __cdecl EdFileReadChar(void) {
    char character;
    EdFileRead(&character, sizeof(character));
    return character;
}

u16 __cdecl EdFileReadUnsignedShort(void) {
    union {
        u16 value;
        struct {
            u8 first;
            u8 second;
        } bytes;
    } data;
    u8 temporary;
    EdFileRead(&data.value, sizeof(data.value));
    if (edfile_readwrongendianess != 0) {
        temporary = data.bytes.first;
        data.bytes.first = data.bytes.second;
        data.bytes.second = temporary;
    }
    return data.value;
}

u8 __cdecl EdFileReadUnsignedChar(void) {
    u8 character;
    EdFileRead(&character, sizeof(character));
    return character;
}
