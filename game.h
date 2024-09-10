#pragma once

#include "external/FastNoiseLite.h"
#include "world.h"

#include <raylib.h>

typedef struct {
    Rectangle player;
    Camera2D camera;
    World world;
    fnl_state height_map;
    // TODO: This does not scale, replace this with an atlas
    Texture2D water;
    Texture2D grass;
    Texture2D sand;
    Texture2D rock;
} GameState;

void init_game();
void quit_game();
void update_game();
void draw_game();
