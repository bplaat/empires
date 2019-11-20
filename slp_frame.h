#ifndef SLP_FRAME_H
#define SLP_FRAME_H

#include <stdint.h>
#include "drs_file.h"
#include "pallet.h"
#include "frame_buffer.h"

typedef struct SlpFrame {
    uint32_t width;
    uint32_t height;
    uint8_t *data;
} SlpFrame;

SlpFrame *slp_frame_load(DrsFile *drs_file);

void slp_frame_draw(SlpFrame *slp_frame, Pallet *pallet, FrameBuffer *frame_buffer, uint32_t x, uint32_t y);

void slp_frame_free(SlpFrame *slp_frame);

#endif
