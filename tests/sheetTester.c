//
// Created by paradocx on 2/28/25.
//

#include "sheetTester.h"
#include "../src/backend.h"
#include <stdio.h>
#include "../src/parser.h"

bool cycleDetectionTest() {
    bool success = true;
    ExpressionError result;

    // Simple self-referencing cycle
    result = setCellValue((Cell){0, 0}, "A1");
    if (result != CIRCULAR_DEPENDENCY) {
        success = false;
        printf("Failed on test A1 = A1\n");
        printf("Expected: CIRCULAR_DEPENDENCY, Got: %d\n", result);
    }
    reset();

    // More complex cycle: A1 = B1, B1 = C1, C1 = A1
    setCellValue((Cell){0, 0}, "B1"); // A1 = B1
    setCellValue((Cell){0, 1}, "C1"); // B1 = C1
    result = setCellValue((Cell){0, 2}, "A1");
    if (result != CIRCULAR_DEPENDENCY) { // C1 = A1 (creates cycle)
        success = false;
        printf("Failed on test A1 = B1, B1 = C1, C1 = A1\n");
        printf("Expected: CIRCULAR_DEPENDENCY, Got: %d\n", result);
    }
    reset();

    // Indirect cycle with intermediate cells
    setCellValue((Cell){1, 0}, "B2"); // A2 = B2
    setCellValue((Cell){1, 1}, "C2"); // B2 = C2
    setCellValue((Cell){1, 2}, "D2"); // C2 = D2
    result = setCellValue((Cell){1, 3}, "A2");
    if (result != CIRCULAR_DEPENDENCY) { // D2 = A2 (creates cycle)
        success = false;
        printf("Failed on test A2 = B2, B2 = C2, C2 = D2, D2 = A2\n");
        printf("Expected: CIRCULAR_DEPENDENCY, Got: %d\n", result);
    }
    reset();

    setCellValue((Cell){0, 0}, "SUM(B2:Z10)");
    result = setCellValue((Cell){5, 5}, "A1");
    if (result != CIRCULAR_DEPENDENCY) { // range cycle
        success = false;
        printf("Failed on test range cycle detection\n");
        printf("Expected: CIRCULAR_DEPENDENCY, Got: %d\n", result);
    }
    reset();

    return success;
}

bool functionTests() {
    bool success = true;

    CellError err;
    int x;

    // Test simple arithmetic expressions
    setCellValue((Cell){0, 0}, "B1+C1");
    setCellValue((Cell){0, 1}, "1");
    setCellValue((Cell){0, 2}, "2");
    x = getCellValue((Cell){0, 0}, &err);
    if (x != 3 || err != NO_ERROR) {
        printf("Failed on test B1 + C1 = 3\n");
        printf("Expected: 3 with NO_ERROR, Got: %d with error %d\n", x, err);
        success = false;
    }

    setCellValue((Cell){1, 0}, "10-5");
    x = getCellValue((Cell){1, 0}, &err);
    if (x != 5 || err != NO_ERROR) {
        printf("Failed on test 10 - 5 = 5\n");
        printf("Expected: 5 with NO_ERROR, Got: %d with error %d\n", x, err);
        success = false;
    }

    setCellValue((Cell){2, 0}, "4*2");
    x = getCellValue((Cell){2, 0}, &err);
    if (x != 8 || err != NO_ERROR) {
        printf("Failed on test 4 * 2 = 8\n");
        printf("Expected: 8 with NO_ERROR, Got: %d with error %d\n", x, err);
        success = false;
    }

    setCellValue((Cell){3, 0}, "8/2");
    x = getCellValue((Cell){3, 0}, &err);
    if (x != 4 || err != NO_ERROR) {
        printf("Failed on test 8 / 2 = 4\n");
        printf("Expected: 4 with NO_ERROR, Got: %d with error %d\n", x, err);
        success = false;
    }

    setCellValue((Cell){9, 1}, "6");
    setCellValue((Cell){9, 2}, "7");
    setCellValue((Cell){9, 0}, "B10*C10");
    x = getCellValue((Cell){9, 0}, &err);
    if (x != 42 || err != NO_ERROR) {
        printf("Failed on test B10 * C10 = 42\n");
        printf("Expected: 42 with NO_ERROR, Got: %d with error %d\n", x, err);
        success = false;
    }

    // Test functions
    setCellValue((Cell){4, 1}, "1");
    setCellValue((Cell){4, 2}, "3");
    setCellValue((Cell){4, 3}, "5");
    setCellValue((Cell){4, 0}, "SUM(B5:D5)");
    x = getCellValue((Cell){4, 0}, &err);
    if (x != 9 || err != NO_ERROR) {
        printf("Failed on test SUM(B5:D5) = 9\n");
        printf("Expected: 9 with NO_ERROR, Got: %d with error %d\n", x, err);
        success = false;
    }

    setCellValue((Cell){5, 0}, "AVG(B5:D5)");
    x = getCellValue((Cell){5, 0}, &err);
    if (x != 3 || err != NO_ERROR) {
        printf("Failed on test AVG(B5:D5) = 3\n");
        printf("Expected: 3 with NO_ERROR, Got: %d with error %d\n", x, err);
        success = false;
    }

    setCellValue((Cell){6, 0}, "MIN(B5:D5)");
    x = getCellValue((Cell){6, 0}, &err);
    if (x != 1 || err != NO_ERROR) {
        printf("Failed on test MIN(B5:D5) = 1\n");
        printf("Expected: 1 with NO_ERROR, Got: %d with error %d\n", x, err);
        success = false;
    }

    setCellValue((Cell){7, 0}, "MAX(B5:D5)");
    x = getCellValue((Cell){7, 0}, &err);
    if (x != 5 || err != NO_ERROR) {
        printf("Failed on test MAX(B5:D5) = 5\n");
        printf("Expected: 5 with NO_ERROR, Got: %d with error %d\n", x, err);
        success = false;
    }

    setCellValue((Cell){8, 0}, "STDEV(B5:D5)");
    x = getCellValue((Cell){8, 0}, &err);
    if (x != 2 || err != NO_ERROR) {
        printf("Failed on test STDEV(B5:D5) = 2\n");
        printf("Expected: 2 with NO_ERROR, Got: %d with error %d\n", x, err);
        success = false;
    }
    reset();

    return success;
}

bool div0Test() {
    bool success = true;

    CellError err;
    int value;

    setCellValue((Cell){0, 0}, "1/0");
    value = getCellValue((Cell){0, 0}, &err);
    if (err != DIVIDE_BY_ZERO) {
        printf("Failed on test 1/0\n");
        printf("Expected: DIVIDE_BY_ZERO error, Got: error %d with value %d\n", err, value);
        success = false;
    }

    setCellValue((Cell){1, 0}, "A1");
    value = getCellValue((Cell){1, 0}, &err);
    if (err == NO_ERROR) {
        printf("Failed on test B1 = A1, did not show a divide by 0\n");
        printf("Expected: DIVIDE_BY_ZERO error, Got: NO_ERROR with value %d\n", value);
        success = false;
    }
    reset();

    return success;
}

bool runSheetTests() {
    initBackend(100, 100);
//    parserSetSize(100, 100);
    bool success = true;
    printf("Starting cycle detection check\n");
    success &= cycleDetectionTest();
    reset();
    printf("Starting function tests\n");
    reset();
    success &= functionTests();
    printf("Starting divide by 0 test\n");
    reset();
    success &= div0Test();

    if (success) {
        printf("\nAll tests passed successfully!\n");
    } else {
        printf("\nSome tests failed. See above for details.\n");
    }

    return success;
}