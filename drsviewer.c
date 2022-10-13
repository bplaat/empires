// A cross-platform SDL2 application which you can use to explorer Age of Empires DRS container files
// gcc -Wall -Wextra -Wshadow -Wpedantic --std=c11 drsviewer.c $(pkg-config --cflags --libs sdl2) -o drsviewer && ./drsviewer
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>
#define EMPIRES_DEFINE
#include "empires.h"
#define FRAMEBUFFER_DEFINE
#include "framebuffer.h"

int main(int argc, char **argv) {
    char *root_path = "/Users/bplaat/Software/Age of Empires";

    // Interfac.drs
    char interface_path[255];
    strcpy(interface_path, root_path);
    strcat(interface_path, "/data/Interfac.drs");
    DRS *interface_drs = drs_new_from_file(interface_path);

    // Palette
    char *palette_text = drs_read_file(interface_drs, DRS_TABLE_BIN, 50500, NULL);
    Palette *default_palette = palette_new_from_text(palette_text);
    free(palette_text);

    // Load DRS
    char drs_path[255];
    strcpy(drs_path, root_path);
    strcat(drs_path, "/data/");
    strcat(drs_path, argc == 1 ? "Interfac.drs" : argv[1]);
    DRS *drs = drs_new_from_file(drs_path);

    // Window
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    SDL_Window *window = SDL_CreateWindow("DRS Viewer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_RESIZABLE);
    Framebuffer *framebuffer = framebuffer_new(window);

    // State
    int32_t scroll_y = 0;
    struct {
        uint32_t extension;
        int32_t id;
        size_t size;
        void *ptr;
        int32_t frame;
    } selected = {0};

    // Event loop
    bool running = false;
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
                            if (selected.ptr != NULL) free(selected.ptr);

                            selected.extension = table->header.extension;
                            selected.id = file->id;
                            selected.ptr = drs_read_file(drs, selected.extension, file->id, &selected.size);
                            selected.frame = 0;

                            if (selected.extension == DRS_TABLE_WAV) {
                                FILE *f = fopen("tmp.wav", "wb");
                                fwrite(selected.ptr, selected.size, 1, f);
                                fclose(f);

                                SDL_AudioSpec wavSpec;
                                uint8_t *wavBuffer;
                                uint32_t wavLength;
                                SDL_LoadWAV("tmp.wav", &wavSpec, &wavBuffer, &wavLength);
                                SDL_AudioDeviceID deviceId = SDL_OpenAudioDevice(NULL, 0, &wavSpec, NULL, 0);
                                SDL_QueueAudio(deviceId, wavBuffer, wavLength);
                                SDL_PauseAudioDevice(deviceId, 0);

                                // TODO releasing SDL audio stuff
                                // SDL_CloseAudioDevice(deviceId);
                                // SDL_FreeWAV(wavBuffer);
                            }
                            break;
                        }
                        y += 12;
                    }
                    y += 12;
                }

                if (event.button.x >= 300 && selected.extension == DRS_TABLE_SLP) {
                    if (selected.extension == DRS_TABLE_SLP) {
                        slp_header *slp = (slp_header *)selected.ptr;
                        selected.frame++;
                        if (selected.frame == slp->frame_count) {
                            selected.frame = 0;
                        }
                    }
                }
                break;
            }

            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) {
                framebuffer_resize(framebuffer);
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

        framebuffer_begin(framebuffer);

        // Draw background
        for (int32_t y = 0; y < framebuffer->height; y++) {
            for (int32_t x = 0; x < framebuffer->width; x++) {
                framebuffer_draw_pixel(framebuffer, x, y, 0xffffff);
            }
        }

        // Draw sidebar
        framebuffer_fill_rect(framebuffer, 0, 0, 299, framebuffer->height, 0xeeeeee);
        framebuffer_fill_rect(framebuffer, 299, 0, 1, framebuffer->height, 0x000000);

        int32_t sidebar_y = 8 - scroll_y;
        char line[256];
        for (int32_t i = 0; i < drs->header.table_count; i++) {
            DRSTable *table = &drs->tables[i];
            char *ext;
            if (table->header.extension == DRS_TABLE_BIN) ext = "bin";
            if (table->header.extension == DRS_TABLE_SHP) ext = "shp";
            if (table->header.extension == DRS_TABLE_SLP) ext = "slp";
            if (table->header.extension == DRS_TABLE_WAV) ext = "wav";

            sprintf(line, ".%s (%d files):", ext, table->header.file_count);
            framebuffer_draw_text(framebuffer, 8, sidebar_y, line, strlen(line), 0x000000);
            sidebar_y += 12;
            for (int32_t j = 0; j < table->header.file_count; j++) {
                drs_file *file = &table->files[j];
                if (table->header.extension == selected.extension && file->id == selected.id) {
                    framebuffer_fill_rect(framebuffer, 0, sidebar_y, 300, 12, 0xcccccc);
                }
                sprintf(line, "%d.%s: %d bytes", file->id, ext, file->size);
                framebuffer_draw_text(framebuffer, 8, sidebar_y + 2, line, strlen(line), 0x000000);
                sidebar_y += 12;
            }
            sidebar_y += 12;
        }

        // Draw selected item
        if (selected.ptr != NULL) {
            if (selected.extension == DRS_TABLE_BIN) {
                int32_t text_y = 8 - scroll_y;
                char *c = selected.ptr;
                while (c < (char *)selected.ptr + selected.size) {
                    char *lineStart = c;
                    while (*c != '\r' && *c != '\n') c++;
                    framebuffer_draw_text(framebuffer, 308, text_y, lineStart, c - lineStart, 0x000000);
                    if (*c == '\r') c++;
                    c++;
                    text_y += 12;
                }
            }
            else if (selected.extension == DRS_TABLE_SLP) {
                slp_header *slp = (slp_header *)selected.ptr;
                sprintf(line, "SLP Frame %d / %d", selected.frame + 1, slp->frame_count);
                framebuffer_draw_text(framebuffer, 308, 8, line, strlen(line),0x000000);

                slp_frame *frame = (slp_frame *)((uint8_t *)selected.ptr + sizeof(slp_header) + selected.frame * (sizeof(slp_frame)));
                framebuffer_draw_slp(framebuffer, (slp_header *)selected.ptr, frame, 300 + ((framebuffer->width - 300) - frame->width) / 2, (framebuffer->height - frame->height) / 2, default_palette);
            }
            else if (selected.extension == DRS_TABLE_WAV) {
                char *text = "You can hear the WAV sound playing!";
                framebuffer_draw_text(framebuffer, 308, 8, text, strlen(text),0x000000);
            }
            else {
                char *text = "Can't view this file type!";
                framebuffer_draw_text(framebuffer, 308, 8, text, strlen(text),0x000000);
            }
        } else {
            char *text = "Open a file with the sidebar!";
            framebuffer_draw_text(framebuffer, 308, 8, text, strlen(text), 0x000000);
        }

        framebuffer_end(framebuffer);
        framebuffer_present(framebuffer);
    }

    drs_free(drs);
    palette_free(default_palette);
    drs_free(interface_drs);

    framebuffer_free(framebuffer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
