#include "SDL.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "empires.h"
#include "drs.h"
#include "drs_table.h"
#include "drs_file.h"
#include "utils.h"
#include "frame_buffer.h"
#include "pallet.h"
#include "slp.h"

bool running = true;
Pallet *pallet;
Slp *slp1;
Slp *slp2;

void render(FrameBuffer *frame_buffer) {
    for (uint32_t y = 0; y < WINDOW_HEIGHT; y++) {
        for (uint32_t x = 0; x < WINDOW_WIDTH; x++) {
            frame_buffer_set_pixel(frame_buffer, x, y, 0x0023ef45);
        }
    }

    uint32_t x = 0, y = 0;
    for (uint32_t i = 0; i < 256; i++) {
        frame_buffer_fill_rect(frame_buffer, x * 10, y * 10, 10, 10, pallet->colors[i]);
        if (x == 16 - 1) {
            x = 0;
            y++;
        } else {
            x++;
        }
    }

    slp_frame_draw(slp1->frames[0], pallet, frame_buffer, 200, 100);
    slp_frame_draw(slp2->frames[0], pallet, frame_buffer, 700, 300);

    frame_buffer_render(frame_buffer);
}

int main(int argc, char **argv) {
    if (argc == 1) {
        return EXIT_FAILURE;
    }

    char *game_dir = argv[1];

    char *interfac_path = string_concat(game_dir, "/DATA/interfac.drs");
    Drs *interfac_drs = drs_load(interfac_path);
    free(interfac_path);

    DrsTable *bin_table = drs_get_table(interfac_drs, DRS_TABLE_TYPE_BIN);
    pallet = pallet_load(drs_table_get_file(bin_table, 50500));

    char *graphics_path = string_concat(game_dir, "/DATA/graphics.drs");
    Drs *graphics_drs = drs_load(graphics_path);
    free(graphics_path);

    DrsTable *slp_table = drs_get_table(graphics_drs, DRS_TABLE_TYPE_SLP);
    slp1 = slp_load(drs_table_get_file(slp_table, 24));
    slp2 = slp_load(drs_table_get_file(slp_table, 133));

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    FrameBuffer *frame_buffer = frame_buffer_new(renderer, WINDOW_WIDTH, WINDOW_HEIGHT);

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        render(frame_buffer);
    }

    slp_free(slp1);
    slp_free(slp2);
    pallet_free(pallet);
    drs_free(graphics_drs);
    drs_free(interfac_drs);

    frame_buffer_free(frame_buffer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}
