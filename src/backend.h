//
// Created by paradocx on 1/18/25.
#pragma once

#include "utils.h"

void initBackend(int rows, int cols);

/**
 * Used for lazy evaluation. Outside cells will be evaluated only when needed
 */
void updateViewport(Cell topLeft, Cell bottomRight);

char* getCellValue(Cell cell);

char* getCellFormula(Cell cell);

/**
 * @param expression The expression entered by the user.
 * The backend will resolve it into a constant or a function
 * This basically the part before and after = broken up
 *
 * @return whether the provided expression cause a circular dependency
 * It would not be updated in the backend in that case
 */
bool setCellValue(Cell cell, char* expression);