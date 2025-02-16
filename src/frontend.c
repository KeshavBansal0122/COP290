//
// Created by paradocx on 1/18/25.
//

#include "frontend.h"

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "backend.h"
// #include "parser.h"

#define MAX_WIDTH 10

#define min(a,b) (((a) < (b)) ? (a) : (b))
#define max(a,b) (((a) > (b)) ? (a) : (b))



static int rows, cols;
int cellWidth = 12;
int rowWidth, colWidth;
static bool doPrint = false;

Cell topLeft;

char* getLine();

/*
 * Temporary, while parser is being worked on, to allow frontend to compile 
 */
static size_t next_token(const char *expression) {
    size_t index = 0;

    if (expression[index] == '\0') { // End of string, no tokens left
        return index;
    }

    if (isalnum(expression[index])) {
        while (expression[index] && isalnum(expression[index])) {
            index++;
        }
    } else {
        // It's a single special character
        index++;
    }

    return index;
}

static int convert_to_int(const char* expression, size_t len, bool* success) {
    int result = 0;
    int i = 0;
    int multiplier = 1;
    if(expression[i] == '+') {
        i++;
        if(i == len) {
            *success = false;
            return 0;
        }
    }
    if(expression[i] == '-') {
        multiplier = -1;
        i++;
        if(i == len) {
            *success = false;
            return 0;
        }
    }

    while (i < len && expression[i] >= '0' && expression[i] <= '9')
    {
        result = result * 10 + (expression[i] - '0');
        i++;
    }

    *success = i == len;

    return result * multiplier;
}

static Cell parseCellReference(const char *reference, size_t len, bool *success) {
    Cell cell = {0, 0};
    int i = 0;

    // Parse column (letters)
    while (i < len && reference[i] >= 'A' && reference[i] <= 'Z')
    {
        cell.col = cell.col * 26 + (reference[i] - 'A' + 1);  // Convert letters to column index
        i++;
    }


    if (i == len) {
        *success = false;
        return cell;
    }

    // Parse row (numbers)
    if (reference[i] >= '0' && reference[i] <= '9')
    {
        cell.row = convert_to_int(&reference[i], len - i, success);  // Convert digits to row number
    }

    cell.row--;cell.col--;
    if (cell.row < 0 || cell.row >= rows || cell.col < 0 || cell.col >= cols) {
        *success = false;
    }
    return cell;
}




/**
 * 0 based indexing. Returned string should be freed by the caller
 * */
static char* numberToColumnHeader(int number) {
    number++;
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
    rowWidth = min(MAX_WIDTH, rows - topLeft.row);
    colWidth = min(MAX_WIDTH, cols - topLeft.col);
    printf("%-*s", cellWidth, "");
    int maxCol = topLeft.col + colWidth - 1;
    int maxRow = topLeft.row + rowWidth - 1;

    for (int i = topLeft.col; i <= maxCol; i++) {
        char *header = numberToColumnHeader(i);
        printf("%-*s", cellWidth, header);
        free(header);
    }
    printf("\n");

    for (int i = topLeft.row; i <= maxRow; i++) {
        printf("%-*d", cellWidth, i+1);

        for (int j = topLeft.col; j <= maxCol; j++) {
            CellError error;
            Cell cell = {i , j};
            int value = getCellValue(cell, &error);
            if (error != NO_ERROR) {
                printf("%-*s", cellWidth, "ERR");
            } else {
                printf("%-*d", cellWidth, value);
            }
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

bool runFrontendCommand(const char* command) {
    //wasd,q
    if (command[1] == '\0' ) {
        switch (command[0]) {
            case 'w':
                if(topLeft.row - MAX_WIDTH >= 0) topLeft.row = topLeft.row - MAX_WIDTH; break;
            case 's':
                if(topLeft.row + 2*MAX_WIDTH<= rows) topLeft.row = topLeft.row + MAX_WIDTH; break;
            case 'd':
                if(topLeft.col + 2*MAX_WIDTH <= cols) topLeft.col = topLeft.col + MAX_WIDTH; break;
            case 'a':
                if(topLeft.col - MAX_FUNCTION >= 0) topLeft.col = topLeft.col - MAX_WIDTH; break;
            case 'q':
                exit(0);
            default:
                printf("Unknown Command\n"); return false;
        }
        return true;
    }
    if (strncmp(command, "disable_output", 14) == 0 && command[14] == '\0') {
        doPrint = false;
    } else if (strncmp(command, "enable_output", 13) == 0 && command[13] == '\0') {
        doPrint = true;
    } else if (strncmp(command, "scroll_to", 9) == 0 && command[9] == ' ') {

        const char* cellAddress = &command[10];
        int cellLen = next_token(cellAddress);
        if (cellAddress[cellLen] != '\0') {
            printf("Invalid Syntax\n");
            return false;
        }
        bool success;

        Cell cell = parseCellReference(cellAddress, cellLen, &success);
        if (!success) {
            printf("Could not parse\n");
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

    for(int i = 1; expression[i]; i++) {
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
    if (command[0] >= 'A' && command[0] <= 'Z') {
        //A cell expression
        bool success;
        int cellLen = next_token(command);
        Cell cell = parseCellReference(command, cellLen, &success);
        if (!success) {
            command[cellLen] = '\0';
            printf("Cell %s not in range\n", command);
            return false;
        }

        if (command[cellLen] != '=') {
            printf("Invalid Syntax\n");
            return false;
        }

        ExpressionError err = setCellValue(cell, &command[cellLen + 1]);
        switch (err) {
            case NONE:
                return true;
            case COULD_NOT_PARSE:
                printf("Could not parse expression\n");
                return false;
            case CIRCULAR_DEPENDENCY:
                printf("Circular Dependency\n");
                return false;
            default:
                printf("ExpressionError has an invalid value\n");
                return false;
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
        clock_t start_time, end_time;
        start_time = clock();

        removeSpaces(buffer);
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

        end_time = clock();
        timeTaken = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
        print_board();
    }
}

void initFrontend(int row, int col) {
    rows = row;
    cols = col;
    // parserSetSize(row, col);
    initBackend(row, col);
    topLeft.row = 0;
    topLeft.col = 0;
    rowWidth = min(MAX_WIDTH, rows);
    colWidth = min(MAX_WIDTH, cols);
    print_board();
    runConsole();
}

char* getLine() {
    int length = 16;
    char* buffer = malloc(length * sizeof(char));
    int i = 0;
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
