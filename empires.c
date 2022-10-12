// An Age of Empires I demo thingy
// gcc -Wall -Wextra -Wshadow -Wpedantic --std=c11 empires.c $(pkg-config --cflags --libs sdl2) -o empires && ./empires
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>
#define EMPIRES_DEFINE
#include "empires.h"

int main(void) {
    char *root_path = "/Users/bplaat/Software/Age of Empires";

    // Interfac.drs
    char interface_path[255];
    strcpy(interface_path, root_path);
    strcat(interface_path, "/data/Interfac.drs");
    DRS *interface_drs = drs_new_from_file(interface_path);

    // Graphics.drs
    char graphics_path[255];
    strcpy(graphics_path, root_path);
    strcat(graphics_path, "/data/graphics.drs");
    DRS *graphics_drs = drs_new_from_file(graphics_path);

    // Terrain.drs
    char terrain_path[255];
    strcpy(terrain_path, root_path);
    strcat(terrain_path, "/data/terrain.drs");
    DRS *terrain_drs = drs_new_from_file(terrain_path);

    // Pallet
    char *palette_text = drs_read_file(interface_drs, DRS_TABLE_BIN, 50500, NULL);
    uint32_t *palette = palette_parse(palette_text);
    free(palette_text);

    // Terrain SLP
    // 0 sand 1 grass 1 water 2 deep water
    void *grass = drs_read_file(terrain_drs, DRS_TABLE_SLP, 15000, NULL);
    slp_frame *grass_frame = (slp_frame *)((uint8_t *)grass + sizeof(slp_header));
    uint8_t *grass_bitmap = slp_frame_to_bitmap(grass, grass_frame);

    // SLP
    void *barracks = drs_read_file(graphics_drs, DRS_TABLE_SLP, 17, NULL);
    slp_frame *barracks_frame = (slp_frame *)((uint8_t *)barracks + sizeof(slp_header));
    uint8_t *barracks_bitmap = slp_frame_to_bitmap(barracks, barracks_frame);

    void *town_center = drs_read_file(graphics_drs, DRS_TABLE_SLP, 18, NULL);
    slp_frame *town_center_frame = (slp_frame *)((uint8_t *)town_center + sizeof(slp_header));
    uint8_t *town_center_bitmap = slp_frame_to_bitmap(town_center, town_center_frame);

    void *tree = drs_read_file(graphics_drs, DRS_TABLE_SLP, 503, NULL);
    slp_frame *tree_frame = (slp_frame *)((uint8_t *)tree + sizeof(slp_header));
    uint8_t *tree_bitmap = slp_frame_to_bitmap(tree, tree_frame);

    // Window
    SDL_Init(SDL_INIT_VIDEO);

    int32_t window_width = 1280;
    int32_t window_height = 720;

    SDL_Window *window = SDL_CreateWindow("Age of Empires",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_width, window_height, 0);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture *texture = SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, window_width, window_height);

    bool running = false;
    while (!running) {
        SDL_Event event;
        SDL_PollEvent(&event);

        if (event.type == SDL_QUIT) {
            running = true;
            break;
        }

        uint32_t *framebuffer;
        int32_t pitch;
        SDL_LockTexture(texture, NULL, (void **)&framebuffer, &pitch);

        for (int32_t y = 0; y < window_height; y++) {
            for (int32_t x = 0; x < window_width; x++) {
                framebuffer[y * window_width + x] = 0xffffff;
            }
        }

        for (int32_t y = 0; y < barracks_frame->height; y++) {
            for (int32_t x = 0; x < barracks_frame->width; x++) {
                uint32_t color = palette[barracks_bitmap[y * barracks_frame->width + x]];
                if (color != 0) {
                    framebuffer[(y + 100) * window_width + (x + 100)] = color;
                }
            }
        }

        for (int32_t y = 0; y < town_center_frame->height; y++) {
            for (int32_t x = 0; x < town_center_frame->width; x++) {
                uint32_t color = palette[town_center_bitmap[y * town_center_frame->width + x]];
                if (color != 0) {
                    framebuffer[(y + 100) * window_width + (x + 300)] = color;
                }
            }
        }

        int32_t oy = 0;
        for (int32_t ry = 0; ry < 10; ry++) {
            int32_t ox = 0;
            for (int32_t rx = 0; rx < 30; rx++) {
                ox += grass_frame->width / 2 - 1;

                for (int32_t y = 0; y < grass_frame->height; y++) {
                    for (int32_t x = 0; x < grass_frame->width; x++) {

                        uint32_t color = grass_bitmap[y * grass_frame->width + x];
                        if (color != 0) {
                            framebuffer[(y + 300 + oy + (rx % 2 ? grass_frame->height / 2 : 0)) * window_width + (x + 100 + ox)] = palette[color];
                        }
                    }
                }
            }
            oy += grass_frame->height;
        }

        for (int32_t y = 0; y < tree_frame->height; y++) {
            for (int32_t x = 0; x < tree_frame->width; x++) {
                uint32_t color = tree_bitmap[y * tree_frame->width + x];
                if (color != 0) {
                    framebuffer[(y + 400) * window_width + (x + 400)] = palette[color];
                }
            }
        }

        SDL_UnlockTexture(texture);

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
    }

    drs_free(graphics_drs);
    drs_free(interface_drs);

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
