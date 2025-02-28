//
// Created by paradocx on 1/18/25.
//

#pragma once
#define STACK_ARRAY_LEN 3
#include <stddef.h>
#include <stdbool.h>
#include "cell.h"


/*
 * This vector stack allocates the data for capacity <= 3
 * This does not affect the stack size of the vector
 * */
typedef struct Vec {
    union {
        struct {
            int capacity;
            int _size;
            Cell* data;
        };
        struct {
            //needed for alignment with the actual useful capacity variable
            int capacity2;
            Cell stack_data[STACK_ARRAY_LEN];
        };
    };
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
