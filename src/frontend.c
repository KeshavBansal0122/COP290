//
// Created by paradocx on 1/18/25.
//

#include "frontend.h"

#include <stdlib.h>
#include <stdio.h>
#include "backend.h"

#define TABLE_WIDTH 10

int rows;
int cols;
int width = 10;


Cell topLeft;

char* numberToColumnHeader(int number) {
    char buffer[4]; // Temporary buffer for the result in reverse order
    int index = 0;

    while (number > 0) {
        int rem = (number - 1) % 26;
        buffer[index++] = 'A' + rem;
        number = (number - 1) / 26;
    }

    char* result = malloc((index + 1) * sizeof(char));

    for (int i = 0; i < index; i++) {
        result[i] = buffer[index - i - 1];
    }
    result[index] = '\0';

    return result;
}


void print_board() {
    //print headers
    printf("%-*s", width, "");
    int maxCol = topLeft.col + TABLE_WIDTH;
    int maxRow = topLeft.row + TABLE_WIDTH;

    for (int i = topLeft.col; i <= maxCol; i++) {
        printf(" %-*s", width, numberToColumnHeader(i));
    }
    printf("\n");

    for (int i = topLeft.row; i <= maxRow; i++) {
        printf("%-*d", width, i);

        for (int j = topLeft.col; j <= maxCol; j++) {
            CellError error;
            Cell cell = {i , j};
            char* value = getCellValue(cell, &error);
            printf("%-*s", width, value);
        }
        printf("\n");
    }
}

void initFrontend(int row, int col) {
    rows = row;
    cols = col;
    topLeft.row = 1;
    topLeft.col = 1;
    print_board();
}