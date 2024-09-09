#pragma once
#define SPEED 250
#define WINDOW_W 1280
#define WINDOW_H 720
#define WINDOW_CENTER(rect)                                                    \
    (Vector2) { WINDOW_W / 2 - rect.width / 2, WINDOW_H / 2 - rect.height / 2 }

#define CHUNK_W 64
#define CHUNK_H 64
#define NX_CHUNKS 256
#define NY_CHUNKS 256

#define TILE_SIZE 64
