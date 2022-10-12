// A cross-platform SDL2 application which you can use to explorer Age of Empires DRS container files
// gcc -Wall -Wextra -Wshadow -Wpedantic --std=c11 drsviewer.c $(pkg-config --cflags --libs sdl2) -o drsviewer && ./drsviewer
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>
#define EMPIRES_DEFINE
#include "empires.h"
#define FONT_DEFINE
#include "font.h"

int32_t framebuffer_width = 1280;
int32_t framebuffer_height = 720;

static inline void draw_pixel(uint32_t *framebuffer, int32_t x, int32_t y, uint32_t color) {
    if (x >= 0 && y >= 0 && x < framebuffer_width && y < framebuffer_height) {
        framebuffer[y * framebuffer_width + x] = color;
    }
}

void framebuffer_fill_rect(uint32_t *framebuffer, int32_t x, int32_t y, int32_t width, int32_t height, uint32_t color) {
    for (int32_t ry = y; ry < y + height; ry++) {
        for (int32_t rx = x; rx < x + width; rx++) {
            draw_pixel(framebuffer, rx, ry, color);
        }
    }
}

void framebuffer_draw_text(uint32_t *framebuffer, int32_t x, int32_t y, char *string, size_t size, uint32_t color) {
    for (size_t i = 0; i < size; i++) {
        char c = string[i];
        for (int32_t y2 = 0; y2 < 8; y2++) {
            uint8_t line = font[(c << 3) | y2];
            for (int32_t x2 = 0; x2 < 8; x2++) {
                if ((line >> (7 - x2)) & 1) {
                    draw_pixel(framebuffer, x + x2, y + y2, color);
                }
            }
        }
        x += 8;
    }
}

int main(int argc, char **argv) {
    char *root_path = "/Users/bplaat/Software/Age of Empires";

    // Interfac.drs
    char interface_path[255];
    strcpy(interface_path, root_path);
    strcat(interface_path, "/data/Interfac.drs");
    DRS *interface_drs = drs_new_from_file(interface_path);

    // Palette
    char *palette_text = drs_read_file(interface_drs, DRS_TABLE_BIN, 50500, NULL);
    uint32_t *palette = palette_parse(palette_text);
    free(palette_text);

    // Load DRS
    char drs_path[255];
    strcpy(drs_path, root_path);
    strcat(drs_path, "/data/");
    strcat(drs_path, argc == 1 ? "Interfac.drs" : argv[1]);
    DRS *drs = drs_new_from_file(drs_path);

    // Window
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window = SDL_CreateWindow("DRS Viewer",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, framebuffer_width, framebuffer_height, SDL_WINDOW_RESIZABLE);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture *texture = SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, framebuffer_width, framebuffer_height);

    bool running = false;
    int32_t scroll_y = 0;

    uint32_t selected_ext;
    int32_t selected_id;
    size_t selected_size;
    void *selected_data = NULL;

    while (!running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                int32_t y = 8 - scroll_y;
                for (int32_t i = 0; i < drs->header.table_count; i++) {
                    DRSTable *table = &drs->tables[i];
                    y += 12;
                    for (int32_t j = 0; j < table->header.file_count; j++) {
                        drs_file *file = &table->files[j];
                        if (event.button.x < 300 && event.button.y >= y && event.button.y < y + 12) {
                            selected_ext = table->header.extension;
                            selected_id = file->id;
                            selected_data = drs_read_file(drs, selected_ext, file->id, &selected_size);
                            break;
                        }
                        y += 12;
                    }
                    y += 12;
                }
                break;
            }

            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) {
                framebuffer_width = event.window.data1;
                framebuffer_height = event.window.data2;
                SDL_DestroyTexture(texture);
                texture = SDL_CreateTexture(renderer,
                    SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, framebuffer_width, framebuffer_height);
                break;
            }

            if (event.type == SDL_MOUSEWHEEL) {
                scroll_y -= event.wheel.y * 6;
                if (scroll_y < 0) scroll_y = 0;
                break;
            }

            if (event.type == SDL_QUIT) {
                running = true;
                break;
            }
        }

        uint32_t *framebuffer;
        int32_t pitch;
        SDL_LockTexture(texture, NULL, (void **)&framebuffer, &pitch);

        // Draw background
        for (int32_t y = 0; y < framebuffer_height; y++) {
            for (int32_t x = 0; x < framebuffer_width; x++) {
                framebuffer[y * framebuffer_width + x] = 0xffffff;
            }
        }

        // Draw sidebar
        framebuffer_fill_rect(framebuffer, 0, 0, 300, framebuffer_height, 0xeeeeee);
        for (int32_t ry = 0; ry < framebuffer_height; ry++) {
            framebuffer[ry * framebuffer_width + 300] = 0x000000;
        }

        // Draw items
        int32_t y = 8 - scroll_y;
        char line[256];
        for (int32_t i = 0; i < drs->header.table_count; i++) {
            DRSTable *table = &drs->tables[i];
            char *ext;
            if (table->header.extension == DRS_TABLE_BIN) ext = "bin";
            if (table->header.extension == DRS_TABLE_SHP) ext = "shp";
            if (table->header.extension == DRS_TABLE_SLP) ext = "slp";
            if (table->header.extension == DRS_TABLE_WAV) ext = "wav";

            sprintf(line, ".%s (%d files):", ext, table->header.file_count);
            framebuffer_draw_text(framebuffer, 8, y, line, strlen(line), 0x000000);
            y += 12;
            for (int32_t j = 0; j < table->header.file_count; j++) {
                drs_file *file = &table->files[j];
                if (table->header.extension == selected_ext && file->id == selected_id) {
                    framebuffer_fill_rect(framebuffer, 0, y, 300, 12, 0xcccccc);
                }
                sprintf(line, "%d.%s: %d bytes", file->id, ext, file->size);
                framebuffer_draw_text(framebuffer, 8, y + 2, line, strlen(line), 0x000000);
                y += 12;
            }
            y += 12;
        }

        // Draw selected item
        if (selected_data != 0) {
            if (selected_ext == DRS_TABLE_BIN) {
                y = 8 - scroll_y;
                char *c = selected_data;
                while (c < (char *)selected_data + selected_size) {
                    char *lineStart = c;
                    while (*c != '\r' && *c != '\n') c++;
                    framebuffer_draw_text(framebuffer, 308, y, lineStart, c - lineStart, 0x000000);
                    if (*c == '\r') c++;
                    c++;
                    y += 12;
                }
            }
            else if (selected_ext == DRS_TABLE_SLP) {
                slp_frame *frame = (slp_frame *)((uint8_t *)selected_data + sizeof(slp_header));
                uint8_t *bitmap = slp_frame_to_bitmap(selected_data, frame);
                for (int32_t ry = 0; ry < frame->height; ry++) {
                    for (int32_t rx = 0; rx < frame->width; rx++) {
                        uint8_t color = bitmap[ry * frame->width + rx];
                        if (color != 0) {
                            draw_pixel(framebuffer, 300 + ((framebuffer_width - 300) - frame->width) / 2 + rx, (framebuffer_height - frame->height) / 2 + ry, palette[color]);
                        }
                    }
                }
                free(bitmap);
            }
            else {
                char *text = "Can't view this file type!";
                framebuffer_draw_text(framebuffer, 308, 8, text, strlen(text),0x000000);
            }
        } else {
            char *text = "Open a file with the sidebar!";
            framebuffer_draw_text(framebuffer, 308, 8, text, strlen(text), 0x000000);
        }

        SDL_UnlockTexture(texture);

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
    }

    drs_free(drs);
    free(palette);
    drs_free(interface_drs);

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
