#!/bin/sh
clear
rm -f -r data
AOE2_PATH="/media/bplaat/BASSIEBAS/Program Files (x86)/Microsoft Games/Age of Empires II"
gcc -Wall -Wextra -Wpedantic -Werror -Wshadow -std=c99 \
    drs.c drs_table.c drs_file.c drs_unpacker.c \
    -o drs_unpacker &&
mkdir data data/gamedata data/graphics data/interfac data/sounds data/terrain &&
./drs_unpacker "$AOE2_PATH/DATA/gamedata.drs" "data/gamedata" &&
./drs_unpacker "$AOE2_PATH/DATA/graphics.drs" "data/graphics" &&
./drs_unpacker "$AOE2_PATH/DATA/interfac.drs" "data/interfac" &&
./drs_unpacker "$AOE2_PATH/DATA/sounds.drs" "data/sounds" &&
./drs_unpacker "$AOE2_PATH/DATA/terrain.drs" "data/terrain"
