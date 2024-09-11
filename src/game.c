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

    state.player = (Rectangle){
        .x = 800, .y = 800, .width = TILE_SIZE, .height = TILE_SIZE};
    state.camera.zoom     = 1;
    state.camera.rotation = 0;
    state.camera.target   = (Vector2){state.player.x + state.player.width / 2,
                                      state.player.y + state.player.height / 2};
    state.camera.offset   = (Vector2){WINDOW_W / 2.0, WINDOW_H / 2.0};

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
        (Vector2){Clamp(state.player.x + state.player.width / 2,
                        WINDOW_CENTER(state.player).x + TILE_SIZE / 2.0,
                        MAX_POS.x - WINDOW_W / 2.0),
                  Clamp(state.player.y + state.player.height / 2,
                        WINDOW_CENTER(state.player).y + TILE_SIZE / 2.0,
                        MAX_POS.y - WINDOW_H / 2.0)};
}

void update_player() {
    if (IsKeyDown(KEY_A))
        state.player.x -= SPEED * GetFrameTime();
    if (IsKeyDown(KEY_D))
        state.player.x += SPEED * GetFrameTime();
    if (IsKeyDown(KEY_W))
        state.player.y -= SPEED * GetFrameTime();
    if (IsKeyDown(KEY_S))
        state.player.y += SPEED * GetFrameTime();

    state.player.x = roundf(Clamp(state.player.x, 0, MAX_POS.x - TILE_SIZE));
    state.player.y = roundf(Clamp(state.player.y, 0, MAX_POS.y - TILE_SIZE));
}

void update_game() {
    update_camera();
    update_player();

    update_world(&state.world);
}

void draw_game() {
    BeginDrawing();
    ClearBackground(GetColor(0x181818ff));
    BeginMode2D(state.camera);
    draw_world(&state.world);
    DrawRectangleRec(state.player, WHITE);
    EndMode2D();
    DrawFPS(10, 10);
    EndDrawing();
}
