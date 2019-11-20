#include "SDL.h"
#include <stdint.h>
#include "frame_buffer.h"

FrameBuffer *frame_buffer_new(SDL_Renderer *renderer, uint32_t width, uint32_t height) {
    FrameBuffer *frame_buffer = malloc(sizeof(FrameBuffer));
    frame_buffer->renderer = renderer;
    frame_buffer->texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, width, height);
    frame_buffer->width = width;
    frame_buffer->height = height;
    frame_buffer->data = malloc(height * width * sizeof(uint32_t));
    return frame_buffer;
}

void frame_buffer_set_pixel(FrameBuffer *frame_buffer, uint32_t x, uint32_t y, uint32_t color) {
    if (x < frame_buffer->width && y < frame_buffer->height) {
        frame_buffer->data[y * frame_buffer->width + x] = color;
    }
}

void frame_buffer_fill_rect(FrameBuffer *frame_buffer, uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color) {
    for (uint32_t ry = 0; ry < height; ry++) {
        for (uint32_t rx = 0; rx < width; rx++) {
            frame_buffer_set_pixel(frame_buffer, x + rx, y + ry, color);
        }
    }
}

void frame_buffer_render(FrameBuffer *frame_buffer) {
    SDL_UpdateTexture(frame_buffer->texture, NULL, frame_buffer->data, frame_buffer->width * sizeof(uint32_t));
    SDL_RenderClear(frame_buffer->renderer);
    SDL_RenderCopy(frame_buffer->renderer, frame_buffer->texture, NULL, NULL);
    SDL_RenderPresent(frame_buffer->renderer);
}

void frame_buffer_free(FrameBuffer *frame_buffer) {
    free(frame_buffer->data);
    SDL_DestroyTexture(frame_buffer->texture);
    SDL_DestroyRenderer(frame_buffer->renderer);
    free(frame_buffer);
}
