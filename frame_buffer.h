#ifndef FRAME_BUFFER_H
#define FRAME_BUFFER_H

#include "SDL.h"
#include <stdint.h>

typedef struct FrameBuffer {
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    uint32_t width;
    uint32_t height;
    uint32_t *data;
} FrameBuffer;

FrameBuffer *frame_buffer_new(SDL_Renderer *renderer, uint32_t width, uint32_t height);

void frame_buffer_set_pixel(FrameBuffer *frame_buffer, uint32_t x, uint32_t y, uint32_t color);

void frame_buffer_fill_rect(FrameBuffer *frame_buffer, uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color);

void frame_buffer_render(FrameBuffer *frame_buffer);

void frame_buffer_free(FrameBuffer *frame_buffer);

#endif
