#!/bin/sh
clear
gcc -Wall -Wextra -Wpedantic -Werror -Wshadow -std=c99 \
    drs.c drs_table.c drs_file.c frame_buffer.c pallet.c slp_frame.c slp.c utils.c empires.c \
    -o empires -D_REENTRANT -I/usr/include/SDL2 -lSDL2 &&
./empires "/media/bplaat/BASSIEBAS/Program Files (x86)/Microsoft Games/Age of Empires II"
