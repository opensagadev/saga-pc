#include "saga.h"

STATIC_ASSERT(sizeof(VARIPTR) == 4);
STATIC_ASSERT(offsetof(NUFPAR_PC, file_name) == 8);
STATIC_ASSERT(offsetof(NUFPAR_PC, word_buffer_ptr) == 0x510);

i32 MovieCountPc = 0;
char (*MovieListPc)[32] = NULL;

#pragma const_seg(".rdata$MoviesListKeywordPc")
const char MoviesListKeywordPc[6] = "movie";
#pragma const_seg()
#pragma const_seg(".rdata$MoviesListPathPc")
const char MoviesListPathPc[18] = "movies\\movies.txt";
#pragma const_seg()

void __cdecl MoviesListNoOpPc(char *path);

void __cdecl Movies_ConfigureList(char *path, VARIPTR *buffer, VARIPTR *buffer_end) {
    NUFPAR_PC *parser;
    char *destination;
    (void)buffer_end;
    parser = NuFParCreate(path);
    if (parser == NULL) {
        MoviesListNoOpPc(path);
        return;
    }
    buffer->addr = (buffer->addr + 3U) & ~3U;
    destination = buffer->char_ptr;
    MovieListPc = (char(*)[32])destination;
    MovieCountPc = 0;
    while (NuFParGetLine(parser) != 0) {
        NuFParGetWord(parser);
        if (NuStrICmp(parser->word_buffer_ptr, (char *)MoviesListKeywordPc) != 0)
            continue;
        if (NuFParGetWord(parser) == 0)
            continue;
        if (NuStrLen(parser->word_buffer_ptr) >= 32)
            continue;
        NuStrCpy(destination, parser->word_buffer_ptr);
        destination += 32;
        MovieCountPc = (i32)((u32)MovieCountPc + 1U);
    }
    NuFParDestroy(parser);
    if (MovieCountPc > 0) {
        buffer->char_ptr = destination;
    } else {
        MovieListPc = NULL;
        MoviesListNoOpPc(path);
    }
}
