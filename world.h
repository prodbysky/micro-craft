#pragma once
#include "arena.h"
#include "config.h"

#include <raylib.h>

typedef enum : char {
    BT_GRASS     = 0,
    BT_SAND      = 1,
    BT_WATER     = 2,
    BT_HOT_PLAIN = 3,
    BT_JUNGLE    = 4,
    BT_ROCK      = 5,
    BT_SNOW      = 6,
    BT_SAVANNA   = 7,
} BlockType;

typedef struct {
    BlockType type;
} Block;

typedef struct {
    Block blocks[CHUNK_W * CHUNK_H];
    Vector2 chunk_offset;
    Rectangle chunk_rect;
    bool visible;
    bool initialized;
} Chunk;

typedef struct {
    Chunk* chunks;
    Arena arena;
} World;

void fill_chunk(Chunk* c);

void random_chunk(Chunk* c);

void simple_perlin_chunk(Chunk* c);

void draw_chunk(const Chunk* c);

World init_world();

void quit_world(World* w);

void update_world(World* w);

void draw_world(World* w);
