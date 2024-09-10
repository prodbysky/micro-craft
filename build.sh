set -xe

gcc arena.c game.c world.c main.c -o micro -O3 -lraylib -lm
