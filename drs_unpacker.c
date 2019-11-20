#include <stdio.h>
#include <stdlib.h>
#include "drs.h"
#include "drs_table.h"
#include "drs_file.h"

int main(int argc, char **argv) {
    if (argc <= 2) {
        return EXIT_FAILURE;
    }

    Drs *drs = drs_load(argv[1]);

    for (uint32_t i = 0; i < drs->table_count; i++) {
        DrsTable *table = drs->tables[i];

        char *file_extension;
        if (table->type == DRS_TABLE_TYPE_BIN) {
            file_extension = "bin";
        }
        if (table->type == DRS_TABLE_TYPE_SLP) {
            file_extension = "slp";
        }
        if (table->type == DRS_TABLE_TYPE_WAV) {
            file_extension = "wav";
        }

        for (uint32_t j = 0; j < table->file_count; j++) {
            DrsFile *drs_file = table->files[j];

            char file_path_buffer[255];
            sprintf(file_path_buffer, "%s/%u.%s", argv[2], drs_file->id, file_extension);

            FILE *file = fopen(file_path_buffer, "wb");

            uint8_t *data_buffer = malloc(drs_file->size);
            fseek(drs_file->file, drs_file->offset, SEEK_SET);
            fread(data_buffer, drs_file->size, 1, drs_file->file);
            fwrite(data_buffer, drs_file->size, 1, file);
            free(data_buffer);

            fclose(file);
        }
    }

    drs_free(drs);

    return EXIT_SUCCESS;
}
