// An Age of Empires I demo thingy
// gcc -Wall -Wextra -Wshadow -Wpedantic --std=c11 empires.c $(pkg-config --cflags --libs sdl2) -o empires && ./empires
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>
#define EMPIRES_DEFINE
#include "empires.h"
#define FRAMEBUFFER_DEFINE
#include "framebuffer.h"

typedef struct Object {
    int32_t id;
    int32_t x;
    int32_t y;
} Object;

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
    Palette *palette = palette_new_from_text(palette_text);
    free(palette_text);

    // Load terrain SLP's
    // 0 sand 1 grass 1 water 2 deep water
    slp_header *terrain_slps[4];
    for (int32_t i = 0; i < 4; i++) {
        terrain_slps[i] = drs_read_file(terrain_drs, DRS_TABLE_SLP, 15000 + i, NULL);
    }

    // Load unit SLP's
    slp_header *unit_slps[716];
    for (int32_t i = 0; i < 716; i++) {
        unit_slps[i] = drs_read_file(graphics_drs, DRS_TABLE_SLP, i, NULL);
    }

    // Window
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("Age of Empires", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_RESIZABLE);
    Framebuffer *framebuffer = framebuffer_new(window);

    // State
    int32_t map_width = 16;
    int32_t map_height = 16;

    uint8_t map[map_height * map_width];
    memset(map, 0, map_height * map_width);
    for (int32_t i = 0; i < 10; i++) {
        map[2 + i] = 1;
    }
    for (int32_t i = 0; i < map_width; i++) {
        map[i * map_width + 0] = 3;
    }
    for (int32_t i = 0; i < map_width; i++) {
        map[i * map_width + 1] = 2;
    }

    size_t objects_size = 2;
    Object objects[100];
    objects[0] = (Object){
        .id = 503,
        .x = 5,
        .y = 5
    };
    objects[1] = (Object){
        .id = 19,
        .x = 5,
        .y = 1
    };

    // Event loop
    bool running = false;
    while (!running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) {
                framebuffer_resize(framebuffer);
                break;
            }

            if (event.type == SDL_QUIT) {
                running = true;
                break;
            }
        }

        framebuffer_begin(framebuffer);
        framebuffer_clear(framebuffer, 0x000000);

        int32_t start_x = framebuffer->width / 2;
        int32_t start_y = 50;

        int32_t terrain_width = ((slp_frame *)((uint8_t *)terrain_slps[0] + sizeof(slp_header)))->width;
        int32_t terrain_hwidth = terrain_width / 2;
        int32_t terrain_height = ((slp_frame *)((uint8_t *)terrain_slps[0] + sizeof(slp_header)))->height;
        int32_t terrain_hheight = terrain_height / 2;

        // Draw map
        for (int32_t ry = 0; ry < map_height; ry++) {
            for (int32_t rx = 0; rx < map_width; rx++) {
                slp_header *slp = terrain_slps[map[ry * map_width + rx]];
                slp_frame *frame = (slp_frame *)((uint8_t *)slp + sizeof(slp_header));

                framebuffer_draw_slp(framebuffer, slp, frame,
                    start_x - terrain_hwidth + rx * terrain_hwidth - ry * terrain_hwidth,
                    start_y + rx * terrain_hheight + ry * terrain_hheight,
                    palette);
            }
        }

        // Draw objects
        for (size_t i = 0; i < objects_size; i++) {
            Object *object = &objects[i];
            slp_header *slp = unit_slps[object->id];
            slp_frame *frame = (slp_frame *)((uint8_t *)slp + sizeof(slp_header));
            framebuffer_draw_slp(framebuffer, slp, frame,
                start_x - frame->center_x + object->x  * terrain_hwidth - object->y * terrain_hwidth,
                start_y - frame->center_y + terrain_hheight + object->x * terrain_hheight + object->y * terrain_hheight,
                palette);
        }

        framebuffer_end(framebuffer);
        framebuffer_present(framebuffer);
    }

    palette_free(palette);
    drs_free(graphics_drs);
    drs_free(interface_drs);

    framebuffer_free(framebuffer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
