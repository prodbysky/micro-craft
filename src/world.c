#include "world.h"

#include "../external/FastNoiseLite.h"
#include "arena.h"
#include "config.h"
#include "game.h"

#include <raylib.h>
#include <raymath.h>

extern GameState state;

void generate_chunk(Chunk* c) {
    for (int x = 0; x < CHUNK_H; x++) {
        for (int y = 0; y < CHUNK_W; y++) {
            Vector2 block_world_pos =
                Vector2Multiply(c->chunk_offset, (Vector2){CHUNK_W, CHUNK_H});
            block_world_pos.x += x;
            block_world_pos.y += y;
            block_world_pos.x /= 100.0;
            block_world_pos.y /= 100.0;

            const float temperature =
                ((fnlGetNoise2D(&state.temperature_map, block_world_pos.x,
                                block_world_pos.y) +
                  0.95) /
                 2.0) +
                GetRandomValue(0, 1) / 300.0;
            const float humidity =
                ((fnlGetNoise2D(&state.humidity_map, block_world_pos.x,
                                block_world_pos.y) +
                  0.95) /
                 2.0) +
                GetRandomValue(0, 1) / 300.0;

            const float r = GetRandomValue(0, 100) / 100.0;
            Block* block  = &c->blocks[y * CHUNK_W + x];

            if (temperature > .33 && temperature < .66 && humidity < .5) {
                if (r < 0.01) {
                    block->type = BT_WOOD;
                } else if (r < 0.03) {
                    block->type = BT_BUSH;
                } else if (r < 0.04) {
                    block->type = BT_LOOSE_ROCKS;
                } else if (r < 0.05) {
                    block->type = BT_ROCK;
                } else {
                    block->type = BT_GRASS;
                }
            } else if (temperature < .33 && humidity < .5) {
                block->type = BT_SNOW;
            } else if (humidity < .6 && temperature < .66) {
                block->type = BT_HOT_PLAIN;
            } else if (temperature > .66 & humidity < .33) {
                if (r < 0.01) {
                    block->type = BT_CACTUS;
                } else {
                    block->type = BT_SAND;
                }
            } else if (temperature > .33 && humidity > .6) {
                block->type = BT_JUNGLE;
            } else if (temperature > .66 && humidity > .3 && humidity < .6) {
                block->type = BT_SAVANNA;
            } else {
                if (r < 0.01) {
                    block->type = BT_LILLY_PAD;
                } else {
                    block->type = BT_WATER;
                }
            }
        }
    }
}

void draw_chunk(const Chunk* c) {
    for (int y = 0; y < CHUNK_H; y++) {
        for (int x = 0; x < CHUNK_W; x++) {
            Block b                    = c->blocks[y * CHUNK_W + x];
            const Vector2 pos_in_chunk = {.x = x * TILE_SIZE,
                                          .y = y * TILE_SIZE};
            const Vector2 pos_in_world = {
                .x = pos_in_chunk.x + c->chunk_offset.x * CHUNK_W * TILE_SIZE,
                .y = pos_in_chunk.y + c->chunk_offset.y * CHUNK_H * TILE_SIZE,
            };

            const Rectangle dest = (Rectangle){
                .x = pos_in_world.x, .y = pos_in_world.y, TILE_SIZE, TILE_SIZE};

            const Rectangle src = (Rectangle){.x = (b.type & 0x00ff) << 5,
                                              .y = (b.type >> 8) << 5,
                                              TILE_SIZE >> 1,
                                              TILE_SIZE >> 1};

            DrawTexturePro(state.atlas, src, dest, Vector2Zero(), 0, WHITE);
        }
    }
}

World init_world() {
    World w;
    w.arena = make_arena(1024 * 1024 * 1024);
    w.chunks =
        (Chunk*) alloc_arena(&w.arena, sizeof(Chunk) * NX_CHUNKS * NY_CHUNKS);
    for (int y = 0; y < NY_CHUNKS; y++) {
        for (int x = 0; x < NX_CHUNKS; x++) {
            Chunk* c        = &w.chunks[y * NX_CHUNKS + x];
            c->chunk_offset = (Vector2){.x = x, .y = y};
            c->chunk_rect   = (Rectangle){
                  .x      = c->chunk_offset.x * CHUNK_W * TILE_SIZE,
                  .y      = c->chunk_offset.y * CHUNK_H * TILE_SIZE,
                  .width  = CHUNK_W * TILE_SIZE,
                  .height = CHUNK_H * TILE_SIZE,
            };
            c->initialized = false;
            c->visible     = false;
        }
    }

    return w;
}

void quit_world(World* w) { delete_arena(&w->arena); }

void update_world(World* w) {
    const Vector2 window_pos = Vector2Subtract(
        state.camera.target, (Vector2){WINDOW_W / 2.0, WINDOW_H / 2.0});
    const Rectangle window_rect = {.x      = window_pos.x,
                                   .y      = window_pos.y,
                                   .width  = WINDOW_W,
                                   .height = WINDOW_H};
    for (int y = 0; y < NY_CHUNKS; y++) {
        for (int x = 0; x < NX_CHUNKS; x++) {
            Chunk* c   = &w->chunks[y * NX_CHUNKS + x];
            c->visible = (CheckCollisionRecs(window_rect, c->chunk_rect));

            if (c->visible && !c->initialized) {
                generate_chunk(c);
                c->initialized = true;
            }
        }
    }
}

void draw_world(World* w) {
    for (int y = 0; y < NY_CHUNKS; y++) {
        for (int x = 0; x < NX_CHUNKS; x++) {
            const Chunk* c = &w->chunks[y * NX_CHUNKS + x];
            if (c->visible) {
                draw_chunk(c);
            }
        }
    }
}
