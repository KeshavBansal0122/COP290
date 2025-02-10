//
// Created by Keshav on 05-02-2025.
//

// This exists only to prevent circular linking between structs.h and vec.h

#pragma once

/**
 * A (row, column) tuple. Please just use directly as a value type.
 **/
typedef struct {
    int row;
    int col;
} Cell;