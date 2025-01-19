//
// Created by paradocx on 1/18/25.
#pragma once

#include "utils.h"

typedef enum ExpressionError {
    NONE,
    CIRCULAR_DEPENDENCY,
    COULD_NOT_PARSE,
} ExpressionError;


void initBackend(int rows, int cols);

/**
 * Used for lazy evaluation. Outside cells will be evaluated only when needed
 */
void updateViewport(Cell topLeft, Cell bottomRight);

/**
 * @param error out parameter for error handling
 * @return The integer to displayed by the frontend. Not using an int
 * to allow changing data types in the future
 */
char* getCellValue(Cell cell, CellError* error);

char* getCellFormula(Cell cell);

/**
 * @param expression The expression entered by the user.
 * The backend will resolve it into a constant or a function
 * This basically the part before and after = broken up
 */
ExpressionError setCellValue(Cell cell, char* expression);