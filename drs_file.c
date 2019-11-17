#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "drs_file.h"

DrsFile *drs_file_load(FILE *file) {
    DrsFile *drs_file = malloc(sizeof(DrsFile));
    fread(&drs_file->id, sizeof(uint32_t), 1, file);
    fread(&drs_file->offset, sizeof(uint32_t), 1, file);
    fread(&drs_file->size, sizeof(uint32_t), 1, file);
    return drs_file;
}

void drs_file_free(DrsFile *drs_file) {
    free(drs_file);
}
