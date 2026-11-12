#include "saga.h"
#include <stdio.h>
ShaderCompilerBufferAllocatorPc::ShaderCompilerBufferAllocatorPc(i32 size, bool owns) {
    data = new u8[size];
    length = 0;
    capacity = size;
    owns_memory = owns;
}
ShaderCompilerBufferAllocatorPc::ShaderCompilerBufferAllocatorPc(VirtualStackAllocator &parent, i32 size) {
    data = parent.cursor;
    parent.cursor += size;
    length = 0;
    capacity = size;
    owns_memory = 0;
}
ShaderCompilerBufferAllocatorPc::~ShaderCompilerBufferAllocatorPc() {
    if (data != NULL && owns_memory != 0) {
        delete[] data;
        data = NULL;
    }
}
void ShaderCompilerBufferAllocatorPc::Resize(i32 size) {
    u8 *memory = new u8[size];
    i32 copied = length < size ? length : size;
    memcpy(memory, data, copied);
    delete[] data;
    data = memory;
    capacity = size;
    if (length > size)
        length = size;
}

void ShaderCompilerBufferAllocatorPc::AppendBytes(const char *source, i32 count) {
    i32 required = length + count;
    if (required > capacity)
        Resize((3 * required) / 2);
    strncpy(reinterpret_cast<char *>(data + length), source, count);
    length += count;
}
void ShaderCompilerBufferAllocatorPc::AppendText(const char *source) {
    AppendBytes(source, (i32)strlen(source));
}

void ShaderCompilerBufferAllocatorPc::AppendInt(i32 value) {
    char text[64];
    sprintf(text, "%d", value);
    AppendBytes(text, (i32)strlen(text));
}

/* A heap copy of the buffer as a terminated string. */
char *ShaderCompilerBufferAllocatorPc::CopyText() {
    char *result = reinterpret_cast<char *>(::operator new(length + 1));
    memcpy(result, data, length);
    result[length] = 0;
    return result;
}

/* Position of the first occurrence of text at or after start, or -1. */
i32 ShaderCompilerBufferAllocatorPc::Find(const char *text, i32 start) {
    const char *match = text;
    char *cursor = (char *)data + start;
    for (; start < length; ++start, ++cursor) {
        if (*match == 0)
            return start - (i32)(match - text);
        if (*cursor != *match) {
            if (match != text)
                match = text;
        } else {
            ++match;
        }
    }
    return -1;
}

/* True when the buffer holds text at start (as far as the buffer goes). */
bool ShaderCompilerBufferAllocatorPc::MatchesAt(i32 start, const char *text) {
    char *cursor = (char *)data + start;
    for (; start < length && *text != 0; ++start, ++cursor, ++text)
        if (*cursor != *text)
            return false;
    return true;
}

/* Overwrites the buffer from start with text, without growing it. */
void ShaderCompilerBufferAllocatorPc::OverwriteText(i32 start, const char *text) {
    char *cursor = (char *)data + start;
    for (; start < length && *text != 0; ++start, ++cursor, ++text)
        *cursor = *text;
}

void ShaderCompilerBufferAllocatorPc::WriteBytes(i32 offset, const void *source, i32 count) {
    memcpy(data + offset, source, count);
}
