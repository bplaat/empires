#include <stdlib.h>
#include <string.h>
#include "utils.h"

char *string_concat(char *string1, char *string2) {
    char *new_string = malloc(strlen(string1) + strlen(string2) + 1);
    strcpy(new_string, string1);
    strcat(new_string, string2);
    return new_string;
}
