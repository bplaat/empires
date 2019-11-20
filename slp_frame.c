#include <stdio.h>
#include <stdlib.h>
#include "slp_frame.h"
#include "drs_file.h"
#include "pallet.h"
#include "frame_buffer.h"

SlpFrame *slp_frame_load(DrsFile *drs_file) {
    SlpFrame *slp_frame = malloc(sizeof(SlpFrame));

    uint32_t command_offset;
    fread(&command_offset, sizeof(uint32_t), 1, drs_file->file);
    uint32_t outline_offset;
    fread(&outline_offset, sizeof(uint32_t), 1, drs_file->file);
    fseek(drs_file->file, 8, SEEK_CUR);
    fread(&slp_frame->width, sizeof(uint32_t), 1, drs_file->file);
    fread(&slp_frame->height, sizeof(uint32_t), 1, drs_file->file);
    fseek(drs_file->file, 8, SEEK_CUR);

    slp_frame->data = malloc(slp_frame->height * slp_frame->width * sizeof(uint8_t));

    for (uint32_t i = 0; i < slp_frame->height * slp_frame->width; i++) {
        slp_frame->data[i] = 0;
    }

    uint32_t drs_file_current_position = ftell(drs_file->file);

    for (uint32_t y = 0; y < slp_frame->height; y++) {
        fseek(drs_file->file, drs_file->offset + outline_offset + y * sizeof(uint32_t), SEEK_SET);
        uint16_t left, right;
        fread(&left, sizeof(uint16_t), 1, drs_file->file);
        fread(&right, sizeof(uint16_t), 1, drs_file->file);

        // uint32_t x = left;

        slp_frame->data[y * slp_frame->width + left] = 10;
        slp_frame->data[y * slp_frame->width + (slp_frame->width - right)] = 10;

        /*fseek(drs_file->file, drs_file->offset + command_offset + y * sizeof(uint32_t), SEEK_SET);
        uint32_t data_offset;
        fread(&data_offset, sizeof(uint32_t), 1, drs_file->file);

        fseek(drs_file->file, command_offset + slp_frame->height * sizeof(uint32_t) + data_offset, SEEK_SET);

        uint8_t command;
        fread(&command, sizeof(uint8_t), 1, drs_file->file);

        for (uint32_t i = 0; i < 100; i++) {
            // Lesser draw
            if ((command & 3) == 0) {
                uint32_t count = command >> 2;
                for (uint32_t j = 0; j < count; j++) {
                    uint8_t pixel;
                    fread(&pixel, sizeof(uint8_t), 1, drs_file->file);
                    slp_frame->data[y * slp_frame->width + x++] = pixel;
                }
            }

            // Lesser skip
            else if ((command & 3) == 1) {
                uint32_t count = command >> 2;
                x += count;
            }

            // Greater draw
            else if ((command & 15) == 2) {
                uint32_t count = ((command & 15) << 4);
                uint8_t next;
                fread(&next, sizeof(uint8_t), 1, drs_file->file);
                count += next;
                for (uint32_t j = 0; j < count; j++) {
                    uint8_t pixel;
                    fread(&pixel, sizeof(uint8_t), 1, drs_file->file);
                    slp_frame->data[y * slp_frame->width + x++] = pixel;
                }
            }

            // Greater skip
            else if ((command & 15) == 3) {

            }

            // Player color draw
            else if ((command & 15) == 6) {

            }

            // Fill
            else if ((command & 15) == 7) {

            }

            // Fill player color
            else if ((command & 15) == 10) {

            }

            // Shadow draw
            else if ((command & 15) == 11) {

            }

            // Shadow draw
            else if ((command & 15) == 11) {

            }

            // Extended command
            else if ((command & 15) == 14) {

            }

            // End of row
            else if (command == 0x0f) {
                break;
            }

            // Unknown command
            else {
                printf("Unknown command: 0x%x\n", command);
                // exit(EXIT_FAILURE);
            }

            fread(&command, sizeof(uint8_t), 1, drs_file->file);
        }*/
    }

    fseek(drs_file->file, drs_file_current_position, SEEK_SET);

    return slp_frame;
}

void slp_frame_draw(SlpFrame *slp_frame, Pallet *pallet, FrameBuffer *frame_buffer, uint32_t x, uint32_t y) {
    for (uint32_t ry = 0; ry < slp_frame->height; ry++) {
        for (uint32_t rx = 0; rx < slp_frame->width; rx++) {
            uint32_t index = slp_frame->data[ry * slp_frame->width + rx];
            if (index != 0) {
                frame_buffer_set_pixel(frame_buffer, x + rx, y + ry, pallet->colors[index]);
            }
        }
    }
}

void slp_frame_free(SlpFrame *slp_frame) {
    free(slp_frame->data);
    free(slp_frame);
}
