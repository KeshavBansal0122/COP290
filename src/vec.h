//
// Created by paradocx on 1/18/25.
//

#pragma once
#include <stddef.h>
#include <stdbool.h>
#include "cell.h"

typedef struct {
    Cell* data;
    size_t size;
    size_t capacity;
} Vec;

/**
 * This is a vector which holds Cells
 */
Vec newVec(size_t capacity);

void push(Vec* vec, Cell data);

Cell pop(Vec* vec);

Cell get(const Vec* vec, size_t index);

size_t getSize(const Vec* vec);

void clear(Vec* vec);

void removeAt(Vec* vec, size_t index);

bool removeItem(Vec* vec, Cell item);
/**
 * Free the vector's array
 */
void freeVec(Vec* vec);
