set -xe

gcc arena.c game.c world.c main.c -o micro -lraylib -lm
