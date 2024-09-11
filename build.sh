set -xe


gcc src/arena.c src/game.c src/world.c src/main.c -o build/micro -O3 -lraylib -lm
