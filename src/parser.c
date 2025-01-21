//
// Created by paradocx on 1/18/25.
//

#include "parser.h"

#include <ctype.h>

#include "structs.h"

void parseRange(char* range);

/**
 * MAKE SURE THE STRING ONLY IS A VALID ADDRESS AND DOES NOT CONTAIN WEIRD SHIT. THIS CODE WILL BLOW UP
 * @param cell
 * @return
 */
Cell getCell(const char *cell) {
    Cell result = {0, 0}; // Initialize row and column to 0

    if (cell == NULL) {
        return result; // Handle null input gracefully
    }

    // Parse column part (letters)
    while (*cell && isalpha(*cell)) {
        result.col = result.col * 26 + (toupper(*cell) - 'A' + 1);
        cell++;
    }

    // Parse row part (numbers)
    while (*cell && isdigit(*cell)) {
        result.row = result.row * 10 + (*cell - '0');
        cell++;
    }

    return result;
}
