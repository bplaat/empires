#ifndef SLP_H
#define SLP_H

#include <stdint.h>
#include "slp_frame.h"
#include "drs_file.h"

typedef struct Slp {
    uint32_t frame_count;
    SlpFrame **frames;
} Slp;

Slp *slp_load(DrsFile *drs_file);

void slp_free(Slp *slp);

#endif
