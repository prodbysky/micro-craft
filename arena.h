#pragma once

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>

typedef struct {
    char* memory;
    size_t used;
    size_t capacity;
} Arena;

Arena make_arena(size_t cap);

char* alloc_arena(Arena* arena, size_t n);
