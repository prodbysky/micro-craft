#include <raylib.h>

#define SPEED 250
#define WINDOW_W 1280
#define WINDOW_H 720
#define WINDOW_CENTER(rect)                                                    \
    (Vector2) { WINDOW_W / 2 - rect.width / 2, WINDOW_H / 2 - rect.height / 2 }

#define CHUNK_W 8
#define CHUNK_H 8
#define NX_CHUNKS 8
#define NY_CHUNKS 8

#define TILE_SIZE 64

typedef struct {
    bool solid;
} Block;

typedef struct {
    Block blocks[CHUNK_W * CHUNK_H];
    Vector2 chunk_offset;
} Chunk;

void fill_chunk(Chunk* c) {
    for (int i = 0; i < CHUNK_W * CHUNK_W; i++) {
        c->blocks[i] = (Block){.solid = true};
    }
}

void random_chunk(Chunk* c) {
    for (int i = 0; i < CHUNK_W * CHUNK_W; i++) {
        c->blocks[i] = (Block){.solid = GetRandomValue(0, 1)};
    }
}

void draw_chunk(Chunk* c) {
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
                DrawRectangle(pos_in_world.x, pos_in_world.y, TILE_SIZE,
                              TILE_SIZE, GRAY);
            }
        }
    }
}

typedef struct {
    Chunk chunks[NX_CHUNKS * NY_CHUNKS];
} World;

World init_world() {
    World w;
    for (int y = 0; y < NY_CHUNKS; y++) {
        for (int x = 0; x < NX_CHUNKS; x++) {
            Chunk* c        = &w.chunks[y * NX_CHUNKS + x];
            c->chunk_offset = (Vector2){.x = x, .y = y};
            random_chunk(c);
        }
    }

    return w;
}

void draw_world(World* w) {
    for (int y = 0; y < NY_CHUNKS; y++) {
        for (int x = 0; x < NX_CHUNKS; x++) {
            draw_chunk(&w->chunks[y * NX_CHUNKS + x]);
        }
    }
}

typedef struct {
    Rectangle player;
    Camera2D camera;
    World world;
} GameState;

static GameState state = {0};

void init_game() {
    InitWindow(WINDOW_W, WINDOW_H, "MicroCraft");
    SetTargetFPS(120);
    state.player =
        (Rectangle){.x = 0, .y = 0, .width = TILE_SIZE, .height = TILE_SIZE};
    state.camera.zoom     = 1;
    state.camera.rotation = 0;
    state.camera.target   = (Vector2){state.player.x + state.player.width / 2,
                                      state.player.y + state.player.height / 2};
    state.camera.offset   = (Vector2){WINDOW_W / 2.0f, WINDOW_H / 2.0f};
    state.world           = init_world();
}

int main() {
    init_game();

    while (!WindowShouldClose()) {
        const Vector2 fps_pos =
            GetScreenToWorld2D((Vector2){10, 10}, state.camera);
        state.camera.target =
            (Vector2){state.player.x + state.player.width / 2,
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
        BeginDrawing();
        ClearBackground(GetColor(0x181818ff));
        DrawFPS(10, 10);
        BeginMode2D(state.camera);
        draw_world(&state.world);
        DrawRectangleRec(state.player, WHITE);
        EndMode2D();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
