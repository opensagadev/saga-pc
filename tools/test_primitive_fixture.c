#include <stdio.h>
#include <string.h>

VARIPTR *NuDisplayListStreamCursor;

static int untouched(const u8 *stream, i32 begin, i32 end) {
    i32 i;
    for (i = begin; i < end; ++i) {
        if (stream[i] != 0xa5)
            return 0;
    }
    return 1;
}

static int check_packet(NUDISPLAYLISTITEM *commands, NUMTL *material) {
    return commands[0].type == 0x80 && commands[0].id == 3 && commands[0].next == material &&
           commands[1].type == 0x87 && commands[1].id == 0 && commands[1].next == NULL && commands[2].type == 0x87 &&
           commands[2].id == 0 && commands[2].next == NULL && commands[3].type == 0x8d && commands[3].id == 1 &&
           commands[3].next == NULL;
}

int main(void) {
    __declspec(align(16)) u8 stream[512];
    u8 snapshot[512];
    VARIPTR cursor;
    NUDISPLAYLIST list;
    NURNDRLIBSTATE state;
    NUDISPLAYLISTITEM head;
    NUMTL untextured, textured, reused;
    NUDISPLAYLISTITEM *first, *second, *third;
    memset(stream, 0xa5, sizeof(stream));
    memset(&list, 0, sizeof(list));
    memset(&state, 0, sizeof(state));
    memset(&head, 0, sizeof(head));
    memset(&untextured, 0, sizeof(untextured));
    memset(&textured, 0, sizeof(textured));
    memset(&reused, 0, sizeof(reused));
    textured.texture_id = 7;
    reused.texture_id = 7;
    cursor.void_ptr = stream + 3;
    NuDisplayListStreamCursor = &cursor;
    list.state = &state;
    list.mtl_last = &head;

    NuDisplayListLinkMtl(&list, &untextured);
    first = (NUDISPLAYLISTITEM *)(stream + 16);
    if (cursor.void_ptr != stream + 80 || head.next != first || list.mtl_last != &first[3] ||
        state.mtl != &untextured || state.tex_id != 0 || !check_packet(first, &untextured) ||
        !untouched(stream, 0, 16) || !untouched(stream, 80, 512))
        return 1;

    memcpy(snapshot, stream, sizeof(stream));
    NuDisplayListLinkMtl(&list, &textured);
    ((NUDISPLAYLISTITEM *)(snapshot + 16))[3].next = stream + 80;
    second = (NUDISPLAYLISTITEM *)(stream + 80);
    if (cursor.void_ptr != stream + 176 || first[3].next != second || list.mtl_last != &second[5] ||
        state.mtl != &textured || state.tex_id != 7 || second[0].type != 0x87 || second[0].id != 0 ||
        second[0].next != NULL || second[1].type != 0x87 || second[1].id != 0 || second[1].next != NULL ||
        !check_packet(second + 2, &textured) || memcmp(snapshot, stream, 80) != 0 || !untouched(stream, 176, 512))
        return 2;

    memcpy(snapshot, stream, sizeof(stream));
    NuDisplayListLinkMtl(&list, &textured);
    if (cursor.void_ptr != stream + 176 || list.mtl_last != &second[5] ||
        memcmp(snapshot, stream, sizeof(stream)) != 0)
        return 3;

    NuDisplayListLinkMtl(&list, &reused);
    ((NUDISPLAYLISTITEM *)(snapshot + 80))[5].next = stream + 176;
    third = (NUDISPLAYLISTITEM *)(stream + 176);
    if (cursor.void_ptr != stream + 240 || second[5].next != third || list.mtl_last != &third[3] ||
        state.mtl != &reused || state.tex_id != 7 || !check_packet(third, &reused) ||
        !untouched(stream, 0, 16) || !untouched(stream, 240, 512))
        return 4;
    puts("PASS primitive material chain: untextured, new texture, repeated material, reused texture");
    return 0;
}
