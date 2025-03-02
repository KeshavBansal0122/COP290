#pragma once

#include "structs.h"

typedef enum ExpressionError {
    NONE,
    CIRCULAR_DEPENDENCY,
    COULD_NOT_PARSE,
} ExpressionError;

/**
 * Resets the backend to its initial state, after the init was calledf
 * The same grid is still retained, only the data is reset
 */
void reset();

void initBackend(int rows, int cols);

/**
 * @param error out parameter for error handling
 * @return The integer to be displayed by the frontend. Not using an int
 * to allow changing data types in the future
 */
int getCellValue(Cell cell, CellError* error);

/**
 * @param expression The expression entered by the user.
 * The backend will resolve it into a constant or a function
 * This basically the part before and after = broken up
 */
ExpressionError setCellValue(Cell cell, char* expression);
