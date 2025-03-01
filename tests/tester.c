#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <limits.h>
#include "../src/parser.h"
#include "function_printer.h"
#include "sheetTester.h"

#define MAX_LINE_LENGTH 100

/**
 * Removes as many spaces as possible, only alphanumerics will be separated by a single space
 * @param str
 */
static void removeMaxSpaces(char* str) {
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


bool processInvalidLine(char* line) {
	fprintf(stderr, "Testing invalid line: %s\n", line);
    bool success;
    removeMaxSpaces(line);
    parseExpression(line, &success);
    if (success) {
        fprintf(stderr, "Failed to detect invalid expression: %s\n", line);
    }
    return !success;
}

bool processValidLine(char* line, char* ans_line) {
 	fprintf(stderr, "Testing valid line: %s\n", line);
    bool success;
    char line2[MAX_LINE_LENGTH];
    strcpy(line2, line);
    removeMaxSpaces(line2);
    Function func = parseExpression(line2, &success);
    if (!success) {
        fprintf(stderr, "Failed to parse valid expression: %s\n", line);
        return false;
    }
    char* result = function_to_string(&func);
    if (strcmp(result, ans_line) != 0) {
        fprintf(stderr, "Mismatch in parsing: %s\n", line);
        fprintf(stderr, "Parsed: %s\n", result);
        fprintf(stderr, "Expected: %s\n", ans_line);
        return false;
    }
    return true;
}



int main(int argc, char* argv[]) {
    fprintf(stderr, "Starting parser tester\n");
    FILE *file;
    char line[MAX_LINE_LENGTH];
    char ans_line[MAX_LINE_LENGTH];


    parserSetSize(INT_MAX, INT_MAX);
    bool success = true;

    char* invalidCommands = argv[1];
    char* validCommands = argv[2];

    /*
     * Process invalid commands
     *
     * */
    // Open the file
    fprintf(stderr, "Processing invalid commands, Checking if all these give error\n");
    file = fopen(invalidCommands, "r");

    // Check if file exists
    if (file == NULL) {
        fprintf(stderr, "Error opening Invalid commands file!\n");
        fprintf(stderr, "provided path was: %s\n", invalidCommands);
        return 1;
    }

    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        size_t len = strlen(line);
        if (len > 0 && line[len-1] == '\n') {
            line[len-1] = '\0';
        }
        if (line[0] == '#') {
            continue;
        }

        success &= processInvalidLine(line);

    }

    fclose(file);


    /*
     * Process valid commands
     *
     * */
    fprintf(stderr, "Processing valid commands, and comparing output\n");
    file = fopen(validCommands, "r");

    if (file == NULL) {
        fprintf(stderr, "Error opening valid commands file!\n");
        fprintf(stderr, "provided path was: %s\n", validCommands);
        return 1;
    }

    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        // Remove trailing newline if present
        size_t len = strlen(line);
        if (len > 0 && line[len-1] == '\n') {
            line[len-1] = '\0';
        }
        if (line[0] == '#') {
            continue;
        }
        fgets(ans_line, MAX_LINE_LENGTH, file);
        len = strlen(ans_line);
        if (len > 0 && ans_line[len-1] == '\n') {
            ans_line[len-1] = '\0';
        }

        success &= processValidLine(line, ans_line);

    }
    fclose(file);

    fprintf(stderr, "Parser test %s\n", success ? "succeeded" : "failed");

    if(success) {
      fprintf(stderr, "starting sheet tests\n");
      success &= runSheetTests();
    } else {
      fprintf(stderr, "skipping sheet tests due to parser failing");
    }
    return success? 0 : 1;
}