#include "game.h"

#include "external/FastNoiseLite.h"
#include "world.h"

#include <limits.h>
#include <raylib.h>
#include <raymath.h>
#include <time.h>

GameState state;

void init_game() {
    InitWindow(WINDOW_W, WINDOW_H, "MicroCraft");

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
    TraceLog(LOG_INFO, "Seed: %d", state.humidity_map.seed);

    state.humidity_map.frequency = 1;

    state.water     = LoadTexture("water.png");
    state.grass     = LoadTexture("grass.png");
    state.rock      = LoadTexture("rock.png");
    state.sand      = LoadTexture("sand.png");
    state.savanna   = LoadTexture("savanna.png");
    state.jungle    = LoadTexture("jungle.png");
    state.snow      = LoadTexture("snow.png");
    state.hot_plain = LoadTexture("hot_plain.png");
}

void quit_game() {
    quit_world(&state.world);
    UnloadTexture(state.grass);
    UnloadTexture(state.water);
    UnloadTexture(state.rock);
    UnloadTexture(state.sand);
    UnloadTexture(state.savanna);
    UnloadTexture(state.jungle);
    UnloadTexture(state.snow);
    UnloadTexture(state.hot_plain);
    CloseWindow();
}

void update_game() {
    state.camera.target = (Vector2){state.player.x + state.player.width / 2,
                                    state.player.y + state.player.height / 2};

    if (IsKeyDown(KEY_A)) {
        state.player.x -= SPEED * GetFrameTime();
    }
    if (IsKeyDown(KEY_D)) {
        state.player.x += SPEED * GetFrameTime();
    }
    if (IsKeyDown(KEY_W)) {
        state.player.y -= SPEED * GetFrameTime();
    }
    if (IsKeyDown(KEY_S)) {
        state.player.y += SPEED * GetFrameTime();
    }
    state.player.x =
        Clamp(state.player.x, WINDOW_CENTER(state.player).x, 10000);
    state.player.y =
        Clamp(state.player.y, WINDOW_CENTER(state.player).y, 10000);
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
