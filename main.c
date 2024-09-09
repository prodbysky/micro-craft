#define FNL_IMPL
#include "game.h"

int main() {
    init_game();
    while (!WindowShouldClose()) {
        update_game();
        draw_game();
    }
    quit_game();

    return 0;
}
