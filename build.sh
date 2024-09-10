set -xe

gcc arena.c game.c world.c main.c -o micro -O0 -lraylib -lm
