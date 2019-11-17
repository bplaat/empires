#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "drs_table.h"
#include "drs_file.h"

DrsTable *drs_table_load(FILE *file) {
    DrsTable *drs_table = malloc(sizeof(DrsTable));

    uint32_t drs_table_type;
    fread(&drs_table_type, sizeof(uint32_t), 1, file);
    if (drs_table_type == 0x62696E61) {
        drs_table->type = DRS_TABLE_TYPE_BIN;
    }
    if (drs_table_type == 0x736c7020) {
        drs_table->type = DRS_TABLE_TYPE_SLP;
    }
    if (drs_table_type == 0x77617620) {
        drs_table->type = DRS_TABLE_TYPE_WAV;
    }

    uint32_t drs_table_files_offset;
    fread(&drs_table_files_offset, sizeof(uint32_t), 1, file);

    fread(&drs_table->file_count, sizeof(uint32_t), 1, file);

    uint32_t drs_file_current_position = ftell(file);
    fseek(file, drs_table_files_offset, SEEK_SET);

    drs_table->files = malloc(drs_table->file_count * sizeof(DrsFile *));
    for (uint32_t i = 0; i < drs_table->file_count; i++) {
        drs_table->files[i] = drs_file_load(file);
    }

    fseek(file, drs_file_current_position, SEEK_SET);

    return drs_table;
}

DrsFile *drs_table_find_file(DrsTable *drs_table, uint32_t file_id) {
    for (uint32_t i = 0; i < drs_table->file_count; i++) {
       if (drs_table->files[i]->id == file_id) {
           return drs_table->files[i];
       }
    }
    return NULL;
}

void drs_table_free(DrsTable *drs_table) {
    for (uint32_t i = 0; i < drs_table->file_count; i++) {
        drs_file_free(drs_table->files[i]);
    }
    free(drs_table->files);
    free(drs_table);
}
