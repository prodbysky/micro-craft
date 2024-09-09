#define FNL_IMPL
#include "external/FastNoiseLite.h"

#include <raylib.h>
#include <raymath.h>

#define SPEED 250
#define WINDOW_W 1280
#define WINDOW_H 720
#define WINDOW_CENTER(rect)                                                    \
    (Vector2) { WINDOW_W / 2 - rect.width / 2, WINDOW_H / 2 - rect.height / 2 }

#define CHUNK_W 8
#define CHUNK_H 8
#define NX_CHUNKS 200
#define NY_CHUNKS 200

#define TILE_SIZE 64

typedef struct {
    bool solid;
} Block;

typedef struct {
    Block blocks[CHUNK_W * CHUNK_H];
    Vector2 chunk_offset;
    Rectangle chunk_rect;
    bool visible;
    bool initialized;
} Chunk;

typedef struct {
    Chunk chunks[NX_CHUNKS * NY_CHUNKS];
} World;

typedef struct {
    Rectangle player;
    Camera2D camera;
    World world;
    fnl_state fnl;
    Texture2D water;
    Texture2D grass;
} GameState;

static GameState state = {0};

void fill_chunk(Chunk* c) {
    for (int i = 0; i < CHUNK_W * CHUNK_H; i++) {
        c->blocks[i] = (Block){.solid = true};
    }
}

void random_chunk(Chunk* c) {
    for (int i = 0; i < CHUNK_W * CHUNK_H; i++) {
        c->blocks[i] = (Block){.solid = GetRandomValue(0, 1)};
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

            if (b.solid) {
                DrawTexturePro(state.grass,
                               (Rectangle){.x = pos_in_world.x,
                                           .y = pos_in_world.y,
                                           TILE_SIZE / 2.0,
                                           TILE_SIZE / 2.0},

                               (Rectangle){.x = pos_in_world.x,
                                           .y = pos_in_world.y,
                                           TILE_SIZE,
                                           TILE_SIZE},
                               Vector2Zero(), 0, WHITE);
                continue;
            }
            DrawTexturePro(state.water,
                           (Rectangle){.x = pos_in_world.x,
                                       .y = pos_in_world.y,
                                       TILE_SIZE / 2.0,
                                       TILE_SIZE / 2.0},

                           (Rectangle){.x = pos_in_world.x,
                                       .y = pos_in_world.y,
                                       TILE_SIZE,
                                       TILE_SIZE},
                           Vector2Zero(), 0, WHITE);
        }
    }
}

World init_world() {
    World w;
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
                simple_perlin_chunk(c);
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
    state.world           = init_world();
    state.fnl             = fnlCreateState();
    state.fnl.noise_type  = FNL_NOISE_OPENSIMPLEX2;
    state.fnl.frequency   = 5;
    state.water           = LoadTexture("water.png");
    state.grass           = LoadTexture("grass.png");
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

int main() {
    init_game();

    while (!WindowShouldClose()) {
        update_game();
        draw_game();
    }

    CloseWindow();
    return 0;
}
