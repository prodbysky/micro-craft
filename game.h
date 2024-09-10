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
    // TODO: This does not scale, replace this with an atlas
    Texture2D water;
    Texture2D grass;
    Texture2D sand;
    Texture2D rock;
    Texture2D savanna;
    Texture2D jungle;
    Texture2D hot_plain;
    Texture2D snow;
} GameState;

void init_game();
void quit_game();
void update_game();
void draw_game();
