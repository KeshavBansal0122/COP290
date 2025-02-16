//
// Created by paradocx on 1/18/25.
//

#pragma once
#include <stddef.h>
#include <stdbool.h>
#include "cell.h"

typedef struct Vec {
    Cell* data;
    int size;
    int capacity;
} Vec;

/**
 * This is a vector which holds Cells
 */
Vec newVec(int capacity);

void push(Vec* vec, Cell data);

Cell pop(Vec* vec);

Cell get(const Vec* vec, int index);

int getSize(const Vec* vec);

void clear(Vec* vec);

void removeAt(Vec* vec, int index);

bool removeItem(Vec* vec, Cell item);
/**
 * Free the vector's array
 */
void freeVec(Vec* vec);
