#include "arena.h"

Arena make_arena(size_t cap) {
    return (Arena){.memory = (char*) malloc(cap), .used = 0, .capacity = cap};
}

char* alloc_arena(Arena* arena, size_t n) {
    size_t alignedSize = (n + 7) & ~7;
    assert(arena->used + alignedSize < arena->capacity);

    char* obj    = arena->memory + arena->used;
    arena->used += alignedSize;

    return obj;
}

void clear_arena(Arena* arena) { arena->used = 0; }
void delete_arena(Arena* arena) {
    arena->used = 0;
    free(arena->memory);
}
