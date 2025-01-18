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


typedef struct {
    Cell cell;
    int value;
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
} CellData;
