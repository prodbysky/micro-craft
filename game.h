#pragma once

#include "external/FastNoiseLite.h"
#include "world.h"

#include <raylib.h>

typedef struct {
    Rectangle player;
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
