#pragma once
#include "arena.h"
#include "config.h"

#include <raylib.h>

// Atlas is 64x64 tiles
// Each enum value has to be a number in format of y coord (1 byte) and x coord
// (1 byte) Each coordinate has to be less than 64
typedef enum {
    BT_GRASS       = 0x0000, // x: 0, y: 0
    BT_SAND        = 0x0001, // x: 1, y: 0
    BT_WATER       = 0x0002,
    BT_HOT_PLAIN   = 0x0003,
    BT_JUNGLE      = 0x0004,
    BT_ROCK        = 0x0005,
    BT_SNOW        = 0x0006,
    BT_SAVANNA     = 0x0007,
    BT_CACTUS      = 0x0008,
    BT_LILLY_PAD   = 0x0009,
    BT_WOOD        = 0x000a,
    BT_BUSH        = 0x000b,
    BT_LOOSE_ROCKS = 0x000c,
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

void simple_height_map_based_chunk(Chunk* c);

void draw_chunk(const Chunk* c);

World init_world();

void quit_world(World* w);

void update_world(World* w);

void draw_world(World* w);
