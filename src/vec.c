//
// Created by paradocx on 1/18/25.
//
#include "vec.h"
#include <stdlib.h>
#include <stdio.h>


Vec newVec(const size_t capacity) {
    Vec vec;
    vec.data = malloc(sizeof(Cell) * capacity);
    if (!vec.data) {
        perror("Failed to allocate memory for vector");
        exit(EXIT_FAILURE);
    }
    vec.size = 0;
    vec.capacity = capacity;
    return vec;
}

void resize(Vec* vec) {
    vec->capacity = vec->capacity == 0 ? 1 : vec->capacity;
    size_t newCapacity = vec->capacity * 2;
    Cell* newData = realloc(vec->data, sizeof(Cell) * newCapacity);
    if (!newData) {
        perror("Failed to resize vector");
        exit(EXIT_FAILURE);
    }
    vec->data = newData;
    vec->capacity = newCapacity;
}

void push(Vec* vec, Cell data) {
    if (vec->size == vec->capacity) {
        resize(vec);
    }
    vec->data[vec->size++] = data;
}

Cell pop(Vec* vec) {
    if (vec->size == 0) {
        printf("Vector is empty\n");
        exit(EXIT_FAILURE);
    }
    return vec->data[vec->size--];
}

Cell get(const Vec* vec, size_t index) {
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

void clear(Vec* vec) {
    vec->size = 0;
}

void removeAt(Vec* vec, size_t index) {
    if (index >= vec->size) {
        fprintf(stderr, "Index out of bounds: %lu\n", index);
        exit(EXIT_FAILURE);
    }
    for (size_t i = index; i < vec->size - 1; i++) {
        vec->data[i] = vec->data[i + 1];
    }
    vec->size--;
}

bool removeItem(Vec* vec, Cell item) {
    for (size_t i = 0; i < vec->size; i++) {
        if (vec->data[i].row == item.row && vec->data[i].col == item.col) {
            removeAt(vec, i);
            return true;
        }
    }
    return false;
}

void freeVec(Vec* vec) {
    free(vec->data);
}
