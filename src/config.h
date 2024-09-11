#pragma once
#include <raylib.h>
#define SPEED 250
#define WINDOW_W 1280
#define WINDOW_H 720
#define WINDOW_CENTER(rect)                                                    \
    (Vector2) {                                                                \
        WINDOW_W / 2.0 - rect.width / 2.0, WINDOW_H / 2.0 - rect.height / 2.0  \
    }

#define CHUNK_W 8
#define CHUNK_H 8
#define NX_CHUNKS 256
#define NY_CHUNKS 256
#define TILE_SIZE 64
#define CHUNK_SIZE                                                             \
    (Vector2) { .x = CHUNK_W * TILE_SIZE, .y = CHUNK_H * TILE_SIZE }
#define MAX_POS                                                                \
    (Vector2) { .x = CHUNK_SIZE.x * NX_CHUNKS, .y = CHUNK_SIZE.y * NY_CHUNKS }
