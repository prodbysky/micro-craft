#pragma once

#include "../external/FastNoiseLite.h"
#include "world.h"

#include <raylib.h>

typedef enum { D_NO = 0, D_LEFT, D_TOP, D_RIGHT, D_BOTTOM, D_COUNT } Direction;

typedef struct {
    Rectangle rect;
    Chunk* chunks[D_COUNT];
    Block* blocks[D_COUNT];
} Player;

typedef struct {
    Player player;
    Camera2D camera;
    World world;
    fnl_state temperature_map;
    fnl_state humidity_map;
    Texture2D atlas;
} GameState;

void init_game();
void quit_game();
void update_game();
void draw_game();
