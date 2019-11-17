#ifndef DRS_TABLE_H
#define DRS_TABLE_H

#include <stdio.h>
#include <stdint.h>
#include "drs_file.h"

typedef enum DrsTableType {
    DRS_TABLE_TYPE_BIN,
    DRS_TABLE_TYPE_SLP,
    DRS_TABLE_TYPE_WAV
} DrsTableType;

typedef struct DrsTable {
    DrsTableType type;
    uint32_t file_count;
    DrsFile **files;
} DrsTable;

DrsTable *drs_table_load(FILE *file);

DrsFile *drs_table_find_file(DrsTable *drs_table, uint32_t file_id);

void drs_table_free(DrsTable *drs_table);

#endif
