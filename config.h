#pragma once
#define SPEED 500
#define WINDOW_W 1280
#define WINDOW_H 720
#define WINDOW_CENTER(rect)                                                    \
    (Vector2) { WINDOW_W / 2 - rect.width / 2, WINDOW_H / 2 - rect.height / 2 }

#define CHUNK_W 16
#define CHUNK_H 16
#define NX_CHUNKS 256
#define NY_CHUNKS 256

#define TILE_SIZE 64
