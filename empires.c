#include "SDL.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "drs.h"
#include "drs_table.h"
#include "drs_file.h"
#include "utils.h"

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

int main(int argc, char **argv) {
    if (argc == 1) {
        return EXIT_FAILURE;
    }

    char *game_dir = argv[1];

    char *interfac_path = string_concat(game_dir, "/DATA/interfac.drs");
    Drs *interfac_drs = drs_load(interfac_path);
    free(interfac_path);

    printf("Drs: %d\n", interfac_drs->table_count);
    for (uint32_t i = 0; i < interfac_drs->table_count; i++) {
        DrsTable *drs_table = interfac_drs->tables[i];
        printf("Table: %d - %d\n", drs_table->type, drs_table->file_count);
        for (uint32_t j = 0; j < drs_table->file_count; j++) {
            DrsFile *drs_file = drs_table->files[j];
            printf("File: %d - %d\n", drs_file->id, drs_file->size);
        }
        putchar('\n');
    }

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("Age of Empires II", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture *framebuffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, WINDOW_WIDTH, WINDOW_HEIGHT);
    uint32_t *pixels = malloc(WINDOW_HEIGHT * WINDOW_WIDTH * sizeof(uint32_t));

    // Set first pixel to red
    pixels[0] = 255;

    for (;;) {
        SDL_Event event;
        if (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                break;
            }
        }
        SDL_UpdateTexture(framebuffer, NULL, pixels, WINDOW_WIDTH * sizeof(uint32_t));
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, framebuffer, NULL, NULL);
        SDL_RenderPresent(renderer);
    }

    drs_free(interfac_drs);

    free(pixels);
    SDL_DestroyTexture(framebuffer);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}
