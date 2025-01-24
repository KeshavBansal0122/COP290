//
// Created by paradocx on 1/18/25.
//

#include "frontend.h"

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "backend.h"
#include "parser.h"

#define TABLE_WIDTH 10

int rows;
int cols;
int cellWidth = 10;

Cell topLeft;

char* getLine();

char* numberToColumnHeader(int number) {
    char buffer[4]; // Temporary buffer for the result in reverse order
    int index = 0;

    while (number > 0) {
        int rem = (number - 1) % 26;
        buffer[index++] = 'A' + rem; // NOLINT(*-narrowing-conversions)
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
    printf("%-*s", cellWidth, "");
    int maxCol = topLeft.col + TABLE_WIDTH;
    int maxRow = topLeft.row + TABLE_WIDTH;

    for (int i = topLeft.col; i <= maxCol; i++) {
        char *header = numberToColumnHeader(i);
        printf(" %-*s", cellWidth, header);
        free(header);
    }
    printf("\n");

    for (int i = topLeft.row; i <= maxRow; i++) {
        printf("%-*d", cellWidth, i);

        for (int j = topLeft.col; j <= maxCol; j++) {
            CellError error;
            Cell cell = {i , j};
            char* value = getCellValue(cell, &error);
            printf("%-*s", cellWidth, value);
        }
        printf("\n");
    }
}

/**
 * Removes spaces from a string
 * @param string
 * @return The string without the spaces, may allocate a new string. The old pointer is invalidated
 */
char* removeSpaces(char* string) {
    if (string == NULL) {
        return NULL;
    }

    int i = 0, j = 0;
    while (string[i] != '\0') {
        if (string[i] != ' ') {
            string[j++] = string[i];
        }
        i++;
    }
    string[j] = '\0';

    if(j < i/2) { // realloc if we have a lot of spaces
        string = realloc(string, (j+1) * sizeof(char));
    }
    return string;
}

/**
 *
 * @param command a string with spaces at the end
 */
void runCommand(const char* command) {
    if (command[0] >= 'A' && command[0] <= 'Z') {
        //A cell expression
        size_t i =0;
        while (isalnum(command[i])) {i++;}
        
        char cellAddress[i+1];
        strncpy(cellAddress, command, i);
        cellAddress[i] = '\0';

        Cell cell = getCell(cellAddress);

        if (cell.row > rows || cell.col > cols) {
            printf("Cell %s not in range\n", cellAddress);
            return;
        }

        if (command[i] == '=') {
            //set expression
            i++;
            size_t l = strlen(command);
            char expression[l-i+1];
            strncpy(expression, &command[i], l-i);
            expression[l-i] = '\0';

            setCellValue(cell, expression);
        } else if (command[i] == '\0') {
            //Query
            CellError error;
            char* a = getCellValue(cell, &error);
            char* b = getCellFormula(cell);

            printf("Computed Cell Value: %s\n", a);
            printf("Entered Cell expression: %s\n", b);


        } else {
            printf("Could not understand the query");
        }
    } else {
        //movement or quit
        if (command[1] != '\0') {
            printf("Unknown Command\n");
            return;
        }
        switch (command[0]) {
            case 'w':
                topLeft.row = topLeft.row - TABLE_WIDTH; break;
            case 's':
                topLeft.row = topLeft.row + TABLE_WIDTH; break;
            case 'd':
                topLeft.col = topLeft.col + TABLE_WIDTH; break;
            case 'a':
                topLeft.col = topLeft.col - TABLE_WIDTH; break;
            default:
                printf("Unknown Command\n");
        }
        if (topLeft.row < 1) {
            topLeft.row = 1;
        }
        if (topLeft.col < 1) {
            topLeft.col = 1;
        }
        if (topLeft.row + TABLE_WIDTH > rows) {
            topLeft.row = rows - TABLE_WIDTH;
        }
        if (topLeft.col + TABLE_WIDTH > cols) {
            topLeft.col = cols - TABLE_WIDTH;
        }
    }

}

_Noreturn void runConsole() {
    while (true) {
        printf("> ");

        char* buffer = getLine();
        if (buffer[0] == '\0') {
            continue; // only enter
        }
        buffer = removeSpaces(buffer);
        runCommand(buffer);
        print_board();
    }
}

void initFrontend(int row, int col) {
    rows = row;
    cols = col;
    topLeft.row = 1;
    topLeft.col = 1;
    print_board();
    runConsole();
}

char* getLine() {
    size_t length = 16;
    char* buffer = malloc(length * sizeof(char));
    size_t i = 0;
    int c;
    while( ((c = getchar()) != EOF) && c != '\n') {
        if (i >= length-1) {
            length *= 2;
            buffer = realloc(buffer, length * sizeof(char));
        }
        buffer[i++] = c;
    }
    buffer[i] = '\0';
    return buffer;
}