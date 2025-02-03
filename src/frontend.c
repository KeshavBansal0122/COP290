//
// Created by paradocx on 1/18/25.
//

#include "frontend.h"

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <minmax.h>

#include "backend.h"
#include "parser.h"

#define MAX_WIDTH 10

int rows;
int cols;
int cellWidth = 10;
int rowWidth;
int colWidth;
static bool doPrint = true;

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
    if (!doPrint) return;
    rowWidth = min(MAX_WIDTH, rows - topLeft.row + 1);
    colWidth = min(MAX_WIDTH, cols - topLeft.col + 1);
    printf("%-*s", cellWidth, "");
    int maxCol = topLeft.col + colWidth - 1;
    int maxRow = topLeft.row + rowWidth - 1;

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
 * Removes as many spaces as possible, only alphanumerics will be separated by a single space
 * @param str
 */
static void removeSpaces(char* str) {
    int i = 0, j = 0;

    while (str[i]) {
        if (isspace(str[i])) {
            // Check if we need to keep a space (alphanumeric on both sides)
            if (j > 0 && isalnum(str[j - 1]) && isalnum(str[i + 1])) {
                str[j++] = ' ';
            }
            // Skip the space otherwise
            while (isspace(str[i])) i++;
        } else {
            str[j++] = str[i++];
        }
    }
    str[j] = '\0';
}

static bool isCellInRange(Cell cell) {
    return cell.row >= 1 && cell.row <= rows && cell.col >= 1 && cell.col <= cols;
}

bool runFrontendCommand(const char* command) {
    //wasd,q
    if (command[1] == '\0' ) {
        switch (command[0]) {
            case 'w':
                topLeft.row = topLeft.row - MAX_WIDTH; break;
            case 's':
                topLeft.row = topLeft.row + MAX_WIDTH; break;
            case 'd':
                topLeft.col = topLeft.col + MAX_WIDTH; break;
            case 'a':
                topLeft.col = topLeft.col - MAX_WIDTH; break;
            case 'q':
                exit(0);
            default:
                printf("Unknown Command\n"); return false;
        }
        if (topLeft.row < 1) {
            topLeft.row = 1;
        }
        if (topLeft.col < 1) {topLeft.col = 1;}
        if (topLeft.row + MAX_WIDTH > rows) {
            topLeft.row = rows - MAX_WIDTH + 1;
        }
        if (topLeft.col + MAX_WIDTH > cols) {
            topLeft.col = cols - MAX_WIDTH + 1;
        }
        return true;
    }
    if (strncmp(command, "disable_output", 14) == 0) {
        doPrint = false;
    } else if (strncmp(command, "enable_output", 13) == 0) {
        doPrint = true;
    } else if (strncmp(command, "scroll_to", 9) == 0) {
        const char* cellAddress = &command[10];
        Cell cell = getCell(cellAddress);
        if (!isCellInRange(cell)) {
            printf("Cell %s not in range\n", cellAddress);
            return false;
        }

        topLeft = cell;
    } else {
        printf("Unknown Command\n");
        return false;
    }
    return true;
}

/**
 * @return if the expression is empty or contains alphanumeric characters separated by spaces
 * Both are sufficient but not necessary conditions for an error
 * */
bool doesExpressionContainError(const char* expression) {
    if (expression[0] == '\0') {
        return true;
    }

    for(size_t i = 1; expression[i]; i++) {
        if (isspace(expression[i]) && isalnum(expression[i-1]) && isalnum(expression[i+1])) {
            return true;
        }
    }

    return false;
}


/**
 * @param command
 * @return whether the command was successful
 */
bool runCommand(char* command) {
    removeSpaces(command);
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
            return false;
        }

        if (command[i] == '=') {
            //set expression
            i++;
            size_t l = strlen(command);
            char expression[l-i+1];
            strncpy(expression, &command[i], l-i);
            expression[l-i] = '\0';

            if (doesExpressionContainError(expression)) {
                printf("Invalid Expression\n");
                return false;
            }
            setCellValue(cell, expression);
            //todo: handle error
        } else if (command[i] == '\0') {
            //Query
            CellError error;
            char* a = getCellValue(cell, &error);

            printf("Computed Cell Value: %s\n", a);


        } else {
            printf("Could not understand the query");
        }
    } else {
        //movement or quit
        return runFrontendCommand(command);
    }

}

_Noreturn void runConsole() {
    char status[4] = "ok";
    double timeTaken = 0;
    while (true) {
        printf("[%.1f] (%s) > ", timeTaken, status);

        char* buffer = getLine();
        time_t start_time, end_time;
        time(&start_time);

        if (buffer[0] == '\0') {
            free(buffer);
            continue; // only enter
        }

        if (runCommand(buffer)) {
            strcpy(status, "ok");
        } else {
            strcpy(status, "err");
        }
        free(buffer);

        time(&end_time);
        timeTaken = (end_time - start_time) / 10.0;
        print_board();
    }
}

void initFrontend(int row, int col) {
    rows = row;
    cols = col;
    topLeft.row = 1;
    topLeft.col = 1;
    rowWidth = min(MAX_WIDTH, rows);
    colWidth = min(MAX_WIDTH, cols);
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