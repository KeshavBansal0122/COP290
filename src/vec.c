//
// Created by paradocx on 1/18/25.
//
#include "vec.h"
#include <stdlib.h>
#include <stdio.h>


Vec newVec(const int capacity) {
    Vec vec;
    if (capacity > STACK_ARRAY_LEN) {
        vec.data = malloc(sizeof(Cell) * capacity);
        if (!vec.data) {
            perror("Failed to allocate memory for vector");
            exit(EXIT_FAILURE);
        }
        vec._size = 0;
        vec.capacity = capacity;
    } else {
        vec.capacity = 0;
    }
    return vec;
}

void move_to_heap(Vec* vec) {
    Cell* data = malloc(sizeof(Cell) * 8);
    if (!data) {
        perror("Failed to allocate memory for vector");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < vec->capacity; i++) {
        data[i] = vec->stack_data[i];
    }

    // need to be careful, can only set these values after the data has been copied
    vec->capacity = 8;
    vec->_size = STACK_ARRAY_LEN;
    vec->data = data;

}

void resize(Vec* vec) {
    int newCapacity = vec->capacity * 2;
    Cell* newData = realloc(vec->data, sizeof(Cell) * newCapacity);
    if (!newData) {
        perror("Failed to resize vector");
        exit(EXIT_FAILURE);
    }
    vec->data = newData;
    vec->capacity = newCapacity;
}

void push(Vec* vec, Cell data) {
    //for stack-based vector
    if (vec->capacity < STACK_ARRAY_LEN) {
        vec->stack_data[vec->capacity++] = data;
        return;
    }
    if (vec->capacity == STACK_ARRAY_LEN) {
        move_to_heap(vec);
    }

    //for heap-based vector
    if (vec->_size == vec->capacity) {
        resize(vec);
    }
    vec->data[vec->_size++] = data;
}

Cell pop(Vec* vec) {

    if (vec->capacity == 0 || (vec->capacity > STACK_ARRAY_LEN && vec->_size == 0)) {
        printf("Vector is empty\n");
        exit(EXIT_FAILURE);
    }

    if (vec->capacity <= STACK_ARRAY_LEN) {
        vec->capacity--;
        return vec->stack_data[vec->capacity];

    } else {
        vec->_size--;
        return vec->data[vec->_size];
    }
}

Cell get(const Vec* vec, int index) {
    if (vec->capacity <= STACK_ARRAY_LEN) {
        if (index >= vec->capacity) {
            fprintf(stderr, "Index out of bounds: %d\n", index);
            exit(EXIT_FAILURE);
        }
        return vec->stack_data[index];
    } else {
        if (index >= vec->_size) {
            fprintf(stderr, "Index out of bounds: %d\n", index);
            exit(EXIT_FAILURE);
        }
        return vec->data[index];
    }
}

// Function to get the size of the vector
int getSize(const Vec* vec) {
    if (vec->capacity <= STACK_ARRAY_LEN) {
        return vec->capacity;
    }
    return vec->_size;
}

void clear(Vec* vec) {
    if (vec->capacity <= STACK_ARRAY_LEN) {
        vec->capacity = 0;
    } else {
        vec->_size = 0;
    }
}

void removeAt(Vec* vec, int index) {
    if (vec->capacity <= STACK_ARRAY_LEN) {
        if (index >= vec->capacity) {
            fprintf(stderr, "Index out of bounds: %d\n", index);
            exit(EXIT_FAILURE);
        }
        for (int i = index; i < vec->capacity - 1; i++) {
            vec->stack_data[i] = vec->stack_data[i + 1];
        }
        vec->capacity--;
    } else {
        if (index >= vec->_size) {
            fprintf(stderr, "Index out of bounds: %d\n", index);
            exit(EXIT_FAILURE);
        }
        for (int i = index; i < vec->_size - 1; i++) {
            vec->data[i] = vec->data[i + 1];
        }
        vec->_size--;
    }
}

bool removeItem(Vec* vec, Cell item) {
    for (int i = 0; i < getSize(vec); i++) {
        Cell cell = get(vec, i);
        if (cell.row == item.row && cell.col == item.col) {
            removeAt(vec, i);
            return true;
        }
    }
    return false;
}

void freeVec(Vec* vec) {
    if (vec->capacity > STACK_ARRAY_LEN) {
        free(vec->data);
    }
    vec->capacity = vec->_size=0;
}
