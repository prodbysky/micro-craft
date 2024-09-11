#include "game.h"

#include "../external/FastNoiseLite.h"
#include "config.h"
#include "world.h"

#include <limits.h>
#include <math.h>
#include <raylib.h>
#include <raymath.h>
#include <time.h>

GameState state;

void init_game() {
    InitWindow(WINDOW_W, WINDOW_H, "MicroCraft");
    SetTargetFPS(120);

    state.player          = (Player){0};
    state.player.rect     = (Rectangle){.x      = MAX_POS.x / 2.0,
                                        .y      = MAX_POS.y / 2.0,
                                        .width  = TILE_SIZE,
                                        .height = TILE_SIZE};
    state.camera.zoom     = 1;
    state.camera.rotation = 0;
    state.camera.target =
        (Vector2){state.player.rect.x + state.player.rect.width / 2,
                  state.player.rect.y + state.player.rect.height / 2};
    state.camera.offset = (Vector2){WINDOW_W / 2.0, WINDOW_H / 2.0};

    state.world = init_world();

    state.temperature_map            = fnlCreateState();
    state.temperature_map.noise_type = FNL_NOISE_OPENSIMPLEX2S;
    SetRandomSeed(time(0));
    state.temperature_map.seed = GetRandomValue(-1000000000, INT_MAX);
    TraceLog(LOG_INFO, "Seed: %d", state.temperature_map.seed);

    state.temperature_map.frequency         = 1;
    state.temperature_map.octaves           = 3;
    state.temperature_map.lacunarity        = 2;
    state.temperature_map.gain              = 0.79;
    state.temperature_map.weighted_strength = 0;

    state.humidity_map            = fnlCreateState();
    state.humidity_map.noise_type = FNL_NOISE_PERLIN;
    SetRandomSeed(time(0));
    state.humidity_map.seed = GetRandomValue(-1000000000, INT_MAX);

    state.humidity_map.frequency = 1;

    state.atlas = LoadTexture("atlas.png");
}

void quit_game() {
    quit_world(&state.world);
    UnloadTexture(state.atlas);
    CloseWindow();
}

void update_camera() {
    state.camera.target =
        (Vector2){Clamp(state.player.rect.x + state.player.rect.width / 2,
                        WINDOW_CENTER(state.player.rect).x + TILE_SIZE / 2.0,
                        MAX_POS.x - WINDOW_W / 2.0),
                  Clamp(state.player.rect.y + state.player.rect.height / 2,
                        WINDOW_CENTER(state.player.rect).y + TILE_SIZE / 2.0,
                        MAX_POS.y - WINDOW_H / 2.0)};
}

void update_player() {
    if (IsKeyDown(KEY_A))
        state.player.rect.x -= SPEED * GetFrameTime();
    if (IsKeyDown(KEY_D))
        state.player.rect.x += SPEED * GetFrameTime();
    if (IsKeyDown(KEY_W))
        state.player.rect.y -= SPEED * GetFrameTime();
    if (IsKeyDown(KEY_S))
        state.player.rect.y += SPEED * GetFrameTime();

    state.player.rect.x =
        roundf(Clamp(state.player.rect.x, 0, MAX_POS.x - TILE_SIZE));
    state.player.rect.y =
        roundf(Clamp(state.player.rect.y, 0, MAX_POS.y - TILE_SIZE));

    state.player.chunks[D_NO] =
        &state.world
             .chunks[(int) floorf(state.player.rect.y / TILE_SIZE / CHUNK_H) *
                         NX_CHUNKS +
                     (int) floorf(state.player.rect.x / TILE_SIZE / CHUNK_W)];

    if (state.player.chunks[D_NO]->chunk_offset.x != 0) {
        state.player.chunks[D_LEFT] =
            &state.world
                 .chunks[(int) state.player.chunks[D_NO]->chunk_offset.y *
                             NX_CHUNKS +
                         (int) state.player.chunks[D_NO]->chunk_offset.x - 1];
    }
    if (state.player.chunks[D_NO]->chunk_offset.y != 0) {
        state.player.chunks[D_TOP] =
            &state.world
                 .chunks[(int) (state.player.chunks[D_NO]->chunk_offset.y - 1) *
                             NX_CHUNKS +
                         (int) state.player.chunks[D_NO]->chunk_offset.x];
    }
    if (state.player.chunks[D_NO]->chunk_offset.x + 1 < NX_CHUNKS) {
        state.player.chunks[D_RIGHT] =
            &state.world
                 .chunks[(int) state.player.chunks[D_NO]->chunk_offset.y *
                             NX_CHUNKS +
                         (int) state.player.chunks[D_NO]->chunk_offset.x + 1];
    }
    if (state.player.chunks[D_NO]->chunk_offset.y < NY_CHUNKS) {
        state.player.chunks[D_BOTTOM] =
            &state.world
                 .chunks[(int) (state.player.chunks[D_NO]->chunk_offset.y + 1) *
                             NX_CHUNKS +
                         (int) state.player.chunks[D_NO]->chunk_offset.x];
    }

    const Vector2 pos_in_chunk = {
        .x = Clamp(roundf((state.player.rect.x -
                           state.player.chunks[D_NO]->chunk_offset.x *
                               TILE_SIZE * CHUNK_W) /
                          TILE_SIZE),
                   0, CHUNK_W - 1),
        .y = Clamp(roundf((state.player.rect.y -
                           state.player.chunks[D_NO]->chunk_offset.y *
                               TILE_SIZE * CHUNK_H) /
                          TILE_SIZE),
                   0, CHUNK_H - 1),
    };
    state.player.blocks[D_NO] =
        &state.player.chunks[D_NO]
             ->blocks[(int) (pos_in_chunk.y * CHUNK_W + pos_in_chunk.x)];

    if (pos_in_chunk.x != 0) {
        state.player.blocks[D_LEFT] = &state.player.chunks[D_NO]->blocks[(
            int) (pos_in_chunk.y * CHUNK_W + pos_in_chunk.x - 1)];
    } else if (state.player.chunks[D_LEFT] != NULL) {
        state.player.blocks[D_LEFT] =
            &state.player.chunks[D_LEFT]
                 ->blocks[(int) (pos_in_chunk.y * CHUNK_W + CHUNK_W - 1)];
    }
    if (pos_in_chunk.x + 1 != CHUNK_W) {
        state.player.blocks[D_RIGHT] = &state.player.chunks[D_NO]->blocks[(
            int) (pos_in_chunk.y * CHUNK_W + pos_in_chunk.x + 1)];
    } else if (state.player.chunks[D_RIGHT] != NULL) {
        state.player.blocks[D_RIGHT] =
            &state.player.chunks[D_RIGHT]
                 ->blocks[(int) (pos_in_chunk.y * CHUNK_W)];
    }
    if (pos_in_chunk.y != 0) {
        state.player.blocks[D_TOP] = &state.player.chunks[D_NO]->blocks[(
            int) ((pos_in_chunk.y - 1) * CHUNK_W + pos_in_chunk.x)];
    } else if (state.player.chunks[D_TOP] != NULL) {
        state.player.blocks[D_TOP] =
            &state.player.chunks[D_TOP]
                 ->blocks[(int) ((CHUNK_H - 1) * CHUNK_W + pos_in_chunk.x)];
    }
    if (pos_in_chunk.y + 1 != CHUNK_H) {
        state.player.blocks[D_BOTTOM] = &state.player.chunks[D_NO]->blocks[(
            int) ((pos_in_chunk.y + 1) * CHUNK_W + pos_in_chunk.x)];
    } else if (state.player.chunks[D_BOTTOM] != NULL) {
        state.player.blocks[D_BOTTOM] =
            &state.player.chunks[D_BOTTOM]->blocks[(int) (pos_in_chunk.x)];
    }
}
void update_game() {
    update_camera();
    update_player();
    update_world(&state.world);

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        state.player.blocks[D_LEFT]->type   = BT_LILLY_PAD;
        state.player.blocks[D_RIGHT]->type  = BT_SAND;
        state.player.blocks[D_TOP]->type    = BT_WOOD;
        state.player.blocks[D_BOTTOM]->type = BT_CACTUS;
    }
}

void draw_game() {
    BeginDrawing();
    ClearBackground(GetColor(0x181818ff));
    BeginMode2D(state.camera);
    draw_world(&state.world);
    DrawRectangleRec(state.player.rect, WHITE);
    EndMode2D();
    DrawFPS(10, 10);
    EndDrawing();
}
