#include "world.h"

#include "arena.h"
#include "config.h"
#include "game.h"

#include <raylib.h>
#include <raymath.h>

extern GameState state;

void fill_chunk(Chunk* c) {
    for (int i = 0; i < CHUNK_W * CHUNK_H; i++) {
        c->blocks[i] = (Block){.type = true};
    }
}

void random_chunk(Chunk* c) {
    for (int i = 0; i < CHUNK_W * CHUNK_H; i++) {
        c->blocks[i] = (Block){.type = GetRandomValue(0, 1)};
    }
}

void simple_perlin_chunk(Chunk* c) {

    for (int x = 0; x < CHUNK_H; x++) {
        for (int y = 0; y < CHUNK_W; y++) {
            Vector2 block_world_pos =
                Vector2Multiply(c->chunk_offset, (Vector2){CHUNK_W, CHUNK_H});
            block_world_pos.x += x;
            block_world_pos.y += y;
            block_world_pos.x /= 100.0;
            block_world_pos.y /= 100.0;
            const bool s = roundf((fnlGetNoise2D(&state.fnl, block_world_pos.x,
                                                 block_world_pos.y) +
                                   1.8) /
                                  2.0);

            c->blocks[y * CHUNK_W + x] = (Block){s};
        }
    }
}

void sinple_height_map_based_chunk(Chunk* c) {
    for (int x = 0; x < CHUNK_H; x++) {
        for (int y = 0; y < CHUNK_W; y++) {
            c->blocks[y * CHUNK_W + x].type = BT_ROCK;
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

            const Rectangle src  = (Rectangle){.x = pos_in_world.x,
                                               .y = pos_in_world.y,
                                               TILE_SIZE / 2.0,
                                               TILE_SIZE / 2.0};
            const Rectangle dest = (Rectangle){
                .x = pos_in_world.x, .y = pos_in_world.y, TILE_SIZE, TILE_SIZE};

            switch (b.type) {
            case BT_WATER: {
                DrawTexturePro(state.water, src, dest, Vector2Zero(), 0, WHITE);
                break;
            }
            case BT_GRASS: {
                DrawTexturePro(state.grass, src, dest, Vector2Zero(), 0, WHITE);
                break;
            }
            case BT_ROCK: {
                DrawTexturePro(state.rock, src, dest, Vector2Zero(), 0, WHITE);
                break;
            }
            case BT_SAND: {
                DrawTexturePro(state.sand, src, dest, Vector2Zero(), 0, WHITE);
                break;
            }
            }
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
                sinple_height_map_based_chunk(c);
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
