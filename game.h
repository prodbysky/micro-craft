#pragma once

#include "external/FastNoiseLite.h"
#include "world.h"

#include <raylib.h>

typedef struct {
    Rectangle player;
    Camera2D camera;
    World world;
    fnl_state fnl;
    Texture2D water;
    Texture2D grass;
} GameState;

void init_game();
void quit_game();
void update_game();
void draw_game();
