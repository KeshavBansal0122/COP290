//
// Created by paradocx on 1/18/25.
//
#include "vec.h"
#include <stdlib.h>
#include <stdio.h>


Vec newVec(const int capacity) {
    Vec vec;
    vec.data = (void**)malloc(sizeof(void*) * capacity);
    if (!vec.data) {
        perror("Failed to allocate memory for vector");
        exit(EXIT_FAILURE);
    }
    vec.size = 0;
    vec.capacity = capacity;
    return vec;
}

void resize(Vec* vec) {

    size_t newCapacity = vec->capacity * 2;
    void** newData = realloc(vec->data, sizeof(void*) * newCapacity);
    if (!newData) {
        perror("Failed to resize vector");
        exit(EXIT_FAILURE);
    }
    vec->data = newData;
    vec->capacity = newCapacity;
}

void push(Vec* vec, void* data) {
    if (vec->size == vec->capacity) {
        resize(vec);
    }
    vec->data[vec->size++] = data;
}

void* get(const Vec* vec, size_t index) {
    if (index >= vec->size) {
        fprintf(stderr, "Index out of bounds: %lu\n", index);
        exit(EXIT_FAILURE);
    }
    return vec->data[index];
}

// Function to get the size of the vector
size_t getSize(const Vec* vec) {
    return vec->size;
}

void freeVec(Vec* vec) {
    free(vec->data);
    free(vec);
}
