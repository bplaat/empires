#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "slp.h"
#include "drs_file.h"

Slp *slp_load(DrsFile *drs_file) {
    Slp *slp = malloc(sizeof(Slp));

    fseek(drs_file->file, drs_file->offset + 4, SEEK_SET);
    fread(&slp->frame_count, sizeof(uint32_t), 1, drs_file->file);
    fseek(drs_file->file, 24, SEEK_CUR);

    slp->frames = malloc(slp->frame_count * sizeof(SlpFrame *));
    for (uint32_t i = 0; i < slp->frame_count; i++) {
        slp->frames[i] = slp_frame_load(drs_file);
    }

    return slp;
}

void slp_free(Slp *slp) {
    for (uint32_t i = 0; i < slp->frame_count; i++) {
        slp_frame_free(slp->frames[i]);
    }
    free(slp->frames);
    free(slp);
}
