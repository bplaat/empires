#ifndef PALLET_H
#define PALLET_H

#include <stdint.h>
#include "drs_file.h"

typedef struct Pallet {
    uint32_t color_count;
    uint32_t *colors;
} Pallet;

Pallet *pallet_load(DrsFile *drs_file);

void pallet_free(Pallet *pallet);

#endif
