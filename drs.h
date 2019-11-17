#ifndef DRS_H
#define DRS_H

#include <stdio.h>
#include <stdint.h>
#include "drs_table.h"

typedef struct Drs {
    FILE *file;
    uint32_t table_count;
    DrsTable **tables;
} Drs;

Drs *drs_load(char *path);

DrsTable *drs_find_table(Drs *drs, DrsTableType table_type);

void drs_free(Drs *drs);

#endif
