#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "drs.h"
#include "drs_table.h"

Drs *drs_load(char *path) {
    Drs *drs = malloc(sizeof(Drs));

    drs->file = fopen(path, "rb");

    fseek(drs->file, 56, SEEK_CUR);
    fread(&drs->table_count, sizeof(uint32_t), 1, drs->file);
    fseek(drs->file, 4, SEEK_CUR);

    drs->tables = malloc(drs->table_count * sizeof(DrsTable *));
    for (uint32_t i = 0; i < drs->table_count; i++) {
        drs->tables[i] = drs_table_load(drs->file);
    }

    return drs;
}

DrsTable *drs_get_table(Drs *drs, DrsTableType table_type) {
    for (uint32_t i = 0; i < drs->table_count; i++) {
       if (drs->tables[i]->type == table_type) {
           return drs->tables[i];
       }
    }
    return NULL;
}

void drs_free(Drs *drs) {
    for (uint32_t i = 0; i < drs->table_count; i++) {
        drs_table_free(drs->tables[i]);
    }
    free(drs->tables);
    fclose(drs->file);
    free(drs);
}
