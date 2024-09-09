#include "arena.h"
#define FNL_IMPL
#include "config.h"
#include "external/FastNoiseLite.h"
#include "game.h"

#include <raylib.h>
#include <raymath.h>

int main() {
    init_game();

    while (!WindowShouldClose()) {
        update_game();
        draw_game();
    }

    CloseWindow();
    return 0;
}
