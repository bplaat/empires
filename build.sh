gcc -Wall -Wextra -Wpedantic -Werror -Wshadow -std=c99 $(find -name "*.c") -o empires -D_REENTRANT -I/usr/include/SDL2 -lSDL2 &&
    ./empires "/media/bplaat/BASSIEBAS/Program Files (x86)/Microsoft Games/Age of Empires II"
