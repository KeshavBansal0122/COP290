//
// Created by paradocx on 1/18/25.
//

#pragma once
#include <stddef.h>

typedef struct {
    void** data;
    size_t size;
    size_t capacity;
} Vec;

/**
 * This is a vector which holds pointers to data
 * Do note that this does not "own" the data like a c++ vector
 */
Vec newVec(int capacity);

void push(Vec* vec, void* data);

void* pop(Vec* vec);

void* get(const Vec* vec, size_t index);

size_t getSize(const Vec* vec);

void clear(Vec* vec);

/**
 * Free the vector's array AND the vector itself.
 * Don't call free(vec) after this. It would cause a double free
 * Again: it would not call free on the data stored inside the vector
 */
void freeVec(Vec* vec);
