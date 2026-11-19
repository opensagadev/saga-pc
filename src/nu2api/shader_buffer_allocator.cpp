#include "gamelib/util/CRC16.h"
#include "gamelib/util/gamelib_util_types.h"
#include <stdio.h>

ShaderCompilerBufferAllocator::ShaderCompilerBufferAllocator(i32 size, bool owns) {
    data = new u8[size];
    length = 0;
    capacity = size;
    owns_memory = owns;
}

ShaderCompilerBufferAllocator::ShaderCompilerBufferAllocator(VirtualStackAllocator &parent, i32 size) {
    u8 *memory = parent.cursor;
    parent.cursor = memory + size;
    data = memory;
    capacity = size;
    length = 0;
    owns_memory = 0;
}

ShaderCompilerBufferAllocator::~ShaderCompilerBufferAllocator() {
    if (data != NULL && owns_memory != 0) {
        delete[] data;
        data = NULL;
    }
}

void ShaderCompilerBufferAllocator::Resize(i32 size) {
    u8 *memory = new u8[size];
    i32 copied = length < size ? length : size;
    memcpy(memory, data, copied);
    delete[] data;
    data = memory;
    capacity = size;
    if (length > size) {
        length = size;
    }
}

void ShaderCompilerBufferAllocator::AppendBytes(const char *source, i32 count) {
    i32 required = length + count;
    if (required > capacity) {
        Resize((3 * required) / 2);
    }
    strncpy(reinterpret_cast<char *>(data + length), source, count);
    length += count;
}

void ShaderCompilerBufferAllocator::AppendText(const char *source) {
    AppendBytes(source, (i32)strlen(source));
}

void ShaderCompilerBufferAllocator::AppendInt(i32 value) {
    char text[64];
    sprintf(text, "%d", value);
    AppendBytes(text, (i32)strlen(text));
}

/* A heap copy of the buffer as a terminated string. */
char *ShaderCompilerBufferAllocator::CopyText() {
    char *result = reinterpret_cast<char *>(::operator new(length + 1));
    memcpy(result, data, length);
    result[length] = 0;
    return result;
}

/* Position of the first occurrence of text at or after start, or -1. */
i32 ShaderCompilerBufferAllocator::Find(const char *text, i32 start) {
    const char *match = text;
    char *cursor = (char *)data + start;
    for (; start < length; ++start, ++cursor) {
        if (*match == 0) {
            return start - (match - text);
        }
        if (*cursor != *match) {
            if (match != text) {
                match = text;
            }
        } else {
            ++match;
        }
    }
    return -1;
}

/* True when the buffer holds text at start (as far as the buffer goes). */
bool ShaderCompilerBufferAllocator::MatchesAt(i32 start, const char *text) {
    char *cursor = (char *)data + start;
    for (; start < length && *text != 0; ++start, ++cursor, ++text) {
        if (*cursor != *text) {
            return false;
        }
    }
    return true;
}

/* Overwrites the buffer from start with text, without growing it. */
void ShaderCompilerBufferAllocator::OverwriteText(i32 start, const char *text) {
    char *cursor = (char *)data + start;
    for (; start < length && *text != 0; ++start, ++cursor, ++text) {
        *cursor = *text;
    }
}

void ShaderCompilerBufferAllocator::WriteBytes(i32 offset, const void *source, i32 count) {
    memcpy(data + offset, source, count);
}

void ShaderCompilerDictionary::SetOutputStage(i32 stage) {
    output_stage = stage;
}

void ShaderCompilerDictionary::fun_00748820() {
    scratch.cursor = scratch.base;
    program_count = 0;
}

void ShaderCompilerDictionary::fun_00748840() {
    for (i32 i = 0; i < definition_count; ++i) {
        definitions[i]->expansion_state = 0;
    }
}
