#ifndef DRS_FILE_H
#define DRS_FILE_H

#include <stdio.h>
#include <stdint.h>

typedef struct DrsFile {
    uint32_t id;
    uint32_t offset;
    uint32_t size;
} DrsFile;

DrsFile *drs_file_load(FILE *file);

void drs_file_free(DrsFile *drs_file);

#endif
