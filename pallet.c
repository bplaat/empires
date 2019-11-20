#include <stdio.h>
#include <stdlib.h>
#include "pallet.h"
#include "drs_file.h"

Pallet *pallet_load(DrsFile *drs_file) {
    Pallet *pallet = malloc(sizeof(Pallet));

    fseek(drs_file->file, drs_file->offset, SEEK_SET);

    char label[16], version[8];
    fscanf(drs_file->file, "%15s\n", label);
    fscanf(drs_file->file, "%7s\n", version);
    fscanf(drs_file->file, "%u\n", &pallet->color_count);

    pallet->colors = malloc(pallet->color_count * sizeof(uint32_t));
    for(uint32_t i = 0; i < pallet->color_count; i++) {
        uint32_t r, g, b;
        fscanf(drs_file->file, "%u %u %u\n", &r, &g, &b);
        pallet->colors[i] = (b << 16) | (g << 8) | r;
    }

    return pallet;
}

void pallet_free(Pallet *pallet) {
    free(pallet->colors);
    free(pallet);
}
