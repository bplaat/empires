// Simple library when functions to decode and work with Age of Empires GENIE Engine files
#ifndef EMPIRES_H
#define EMPIRES_H

// ############################################# Header #############################################
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

// DRS
typedef struct drs_header {
    char copyright[40];
    char version[4];
    char type[12];
    int32_t table_count;
    int32_t offset_first_file;
} drs_header;

#define DRS_TABLE_BIN 0x62696E61
#define DRS_TABLE_SHP 0x73687020
#define DRS_TABLE_SLP 0x736C7020
#define DRS_TABLE_WAV 0x77617620

typedef struct drs_table {
    uint32_t extension;
    int32_t offset;
    int32_t file_count;
} drs_table;

typedef struct drs_file {
    int32_t id;
    int32_t offset;
    int32_t size;
} drs_file;

typedef struct DRSTable {
    drs_table header;
    drs_file *files;
} DRSTable;

typedef struct DRS {
    FILE *file;
    drs_header header;
    DRSTable *tables;
} DRS;

DRS *drs_new_from_file(char *path);

void *drs_read_file(DRS *drs, uint32_t type, int32_t id, size_t *size);

void drs_free(DRS *drs);

// Palette
uint32_t *palette_parser(char *palette_text);

// SLP
typedef struct slp_header {
    char version[4];
    int32_t frame_count;
    char comment[24];
} slp_header;

typedef struct slp_frame {
    uint32_t command_table_offset;
    uint32_t outline_table_offset;
    uint32_t palette_offset;
    uint32_t properties;
    int32_t width;
    int32_t height;
    int32_t center_x;
    int32_t center_y;
} slp_frame;

typedef struct slp_outline_row {
    uint16_t left;
    uint16_t right;
} slp_outline_row;

uint8_t *slp_frame_to_bitmap(void *slp, slp_frame *frame);

// ############################################# Implementation #############################################
#ifdef EMPIRES_DEFINE

#include <stdlib.h>
#include <string.h>

DRS *drs_new_from_file(char *path) {
    DRS *drs = malloc(sizeof(DRS));
    drs->file = fopen(path, "rb");
    if (drs->file == NULL) {
        fprintf(stderr, "Can't open file: %s\n", path);
        exit(EXIT_FAILURE);
    }

    fread(&drs->header, sizeof(drs_header), 1, drs->file);
    drs->tables = malloc(drs->header.table_count * sizeof(DRSTable));
    for (int32_t i = 0; i < drs->header.table_count; i++) {
        DRSTable *table = &drs->tables[i];
        fread(&table->header, sizeof(drs_table), 1, drs->file);
    }

    for (int32_t i = 0; i < drs->header.table_count; i++) {
        DRSTable *table = &drs->tables[i];
        table->files = malloc(table->header.file_count * sizeof(drs_file));
        fseek(drs->file, table->header.offset, SEEK_SET);
        fread(table->files, sizeof(drs_file), table->header.file_count, drs->file);
    }

    return drs;
}

void *drs_read_file(DRS *drs, uint32_t type, int32_t id, size_t *size) {
    for (int32_t i = 0; i < drs->header.table_count; i++) {
        DRSTable *table = &drs->tables[i];
        if (table->header.extension == type) {
            for (int32_t j = 0; j < table->header.file_count; j++) {
                drs_file *file = &table->files[j];
                if (file->id == id) {
                    void *ptr = malloc(file->size);
                    if (size != NULL) *size = file->size;
                    fseek(drs->file, file->offset, SEEK_SET);
                    fread(ptr, file->size, 1, drs->file);
                    return ptr;
                }
            }
        }
    }
    return NULL;
}

void drs_free(DRS *drs) {
    for (int32_t i = 0; i < drs->header.table_count; i++) {
        DRSTable *table = &drs->tables[i];
        free(table->files);
    }
    free(drs->tables);
    fclose(drs->file);
    free(drs);
}

// Palette
uint32_t *palette_parse(char *palette_text) {
    char *c = palette_text;
    for (int32_t i = 0; i < 2; i++) {
        while (*c != '\n') c++;
        c++;
    }
    uint32_t length = strtod(c, &c);
    c++;
    uint32_t *palette = malloc(length * sizeof(uint32_t));
    for (int32_t i = 0; i < 256; i++) {
        uint8_t red = strtod(c, &c);
        c++;
        uint8_t green = strtod(c, &c);
        c++;
        uint8_t blue = strtod(c, &c);
        c++;
        palette[i] = (blue << 16) | (green << 8) | red;
        c++;
    }
    return palette;
}

// SLP
uint8_t *slp_frame_to_bitmap(void *slp, slp_frame *frame) {
    uint8_t *bitmap = malloc(frame->height * frame->width);
    memset(bitmap, 0, frame->height * frame->width);

    uint32_t *command_table = (uint32_t *)((uint8_t *)slp + frame->command_table_offset);
    slp_outline_row *outline = (slp_outline_row *)((uint8_t *)slp + frame->outline_table_offset);
    for (int32_t y = 0; y < frame->height; y++) {
        if (outline->left == 0x8000 && outline->right == 0x8000) {
            outline++;
            continue;
        }

        uint8_t *c = (uint8_t *)slp + command_table[y];

        int32_t x = outline->left;
        for (;;) {
            uint8_t opcode = *c++;

            // Lesser draw
            if ((opcode & 3) == 0) {
                for (int32_t i = 0; i < opcode >> 2; i++) {
                    bitmap[y * frame->width + x++] = *c++;
                }
                continue;
            }

            // Lesser skip
            if ((opcode & 3) == 1) {
                x += opcode >> 2;
                continue;
            }

            // Greater draw
            if ((opcode & 15) == 2) {
                int32_t length = ((opcode & 0xf0) << 4) | *c++;
                for (int32_t i = 0; i < length; i++) {
                    bitmap[y * frame->width + x++] = *c++;
                }
                continue;
            }

            // Greater skip
            if ((opcode & 15) == 3) {
                x += ((opcode & 0xf0) << 4) | *c++;
                continue;
            }

            // Player color draw
            if ((opcode & 15) == 6) {
                int32_t length = opcode >> 4 != 0 ? opcode >> 4 : *c++;
                x += length;
                continue;
            }

            // Fill
            if ((opcode & 15) == 7) {
                int32_t length = opcode >> 4 != 0 ? opcode >> 4 : *c++;
                uint8_t color = *c++;
                for (int32_t i = 0; i < length; i++) {
                    bitmap[y * frame->width + x++] = color;
                }
                continue;
            }

            // Fill player color
            if ((opcode & 15) == 10) {
                int32_t length = opcode >> 4 != 0 ? opcode >> 4 : *c++;
                x += length;
                continue;
            }

            // Shadow draw
            if ((opcode & 15) == 11) {
                int32_t length = opcode >> 4 != 0 ? opcode >> 4 : *c++;
                x += length;
                continue;
            }

            // Extended command
            if ((opcode & 15) == 14) {
                fprintf(stderr, "Unkown extended command: %02x\n", opcode >> 4);
                continue;
            }

            // End of row
            if (opcode == 0x0f) break;

            fprintf(stderr, "Unkown opcode: %02x\n", opcode);
        }
        outline++;
    }
    return bitmap;
}

#endif

#endif
