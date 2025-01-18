//
// Created by paradocx on 1/18/25.
//
#pragma once
#include<stdbool.h>

#include "vec.h"
#include "functions.h"

typedef struct {
    int row;
    int col;
} Cell;

typedef enum {
    NO_ERROR,
    DIVIDE_BY_ZERO,
    DEPENDENCY_ERROR // depends on cell which has div by zero
} CellError;

typedef struct {
    Cell cell;
    int value;
    /**
     * Needs recalculation
     */
    bool isDirty;
    /**
     * Cells that depend on this cell
     */
    Vec dependents;
    /**
     * Cells that this cell depends on
     */
    Vec dependencies;
    Function function;
    CellError error;
} CellData;
