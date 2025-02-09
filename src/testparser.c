//
// Created by paradocx on 1/18/25.
//

#include "parser.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h> // For free()

// Test for convert_to_int function
void test_convert_to_int() {
    printf("Starting test_convert_to_int...\n");
    assert(convert_to_int((char*)"123") == 123);
    printf("Passed test case: convert_to_int(\"123\")\n");
    assert(convert_to_int((char*)"0") == 0);
    printf("Passed test case: convert_to_int(\"0\")\n");
    assert(convert_to_int((char*)"00123") == 123);
    printf("Passed test case: convert_to_int(\"00123\")\n");
    printf("convert_to_int tests passed.\n");
}

// Test for parseCellReference function
void test_parseCellReference() {
    printf("Starting test_parseCellReference...\n");

    Cell cell = parseCellReference((char*)"A1");
    assert(cell.col == 1 && cell.row == 1);
    printf("Passed test case: parseCellReference(\"A1\")\n");

    cell = parseCellReference((char*)"Z10");
    assert(cell.col == 26 && cell.row == 10);
    printf("Passed test case: parseCellReference(\"Z10\")\n");

    cell = parseCellReference((char*)"AA1");
    assert(cell.col == 27 && cell.row == 1);
    printf("Passed test case: parseCellReference(\"AA1\")\n");

    printf("parseCellReference tests passed.\n");
}

// Test for parseBinaryOp function
void test_parseBinaryOp() {
    printf("Starting test_parseBinaryOp...\n");

    BinaryOp* op = parseBinaryOp((char*)"A1", (char*)"10");
    assert(op->first.type == OPERAND_CELL);
    assert(op->first.data.cell.col == 1 && op->first.data.cell.row == 1);
    assert(op->second.type == OPERAND_INT);
    assert(op->second.data.value == 10);
    printf("Passed test case: parseBinaryOp(\"A1\", \"10\")\n");
    free(op); // Free allocated memory

    op = parseBinaryOp((char*)"5", (char*)"B2");
    assert(op->first.type == OPERAND_INT);
    assert(op->first.data.value == 5);
    assert(op->second.type == OPERAND_CELL);
    assert(op->second.data.cell.col == 2 && op->second.data.cell.row == 2);
    printf("Passed test case: parseBinaryOp(\"5\", \"B2\")\n");
    free(op); // Free allocated memory

    printf("parseBinaryOp tests passed.\n");
}

// Test for parseExpression function
void test_parseExpression() {
    printf("Starting test_parseExpression...\n");
    bool success;

    Function func = parseExpression((char*)"MIN(A1:B2)", &success);
    assert(func.type == MIN_FUNCTION);
    //printf("Passed type check for MIN_FUNCTION\n");
    assert(func.data.rangeFunctions.topLeft.col == 1 && func.data.rangeFunctions.topLeft.row == 1);
    //printf("Passed topLeft check for MIN_FUNCTION\n");
    assert(func.data.rangeFunctions.bottomRight.col == 2 && func.data.rangeFunctions.bottomRight.row == 2);
    //printf("Passed bottomRight check for MIN_FUNCTION\n");
    //printf("Passed test case: parseExpression(\"MIN(A1:B2)\")\n");

    func = parseExpression((char*)"MAX(C3:D4)", &success);
    assert(func.type == MAX_FUNCTION);
    assert(func.data.rangeFunctions.topLeft.col == 3 && func.data.rangeFunctions.topLeft.row == 3);
    assert(func.data.rangeFunctions.bottomRight.col == 4 && func.data.rangeFunctions.bottomRight.row == 4);
    printf("Passed test case: parseExpression(\"MAX(C3:D4)\")\n");

    func = parseExpression((char*)"5+10", &success);
    assert(func.data.binaryOps.first.type == OPERAND_INT);
    assert(func.data.binaryOps.first.data.value == 5);
    assert(func.data.binaryOps.second.type == OPERAND_INT);
    assert(func.data.binaryOps.second.data.value == 10);
    printf("Passed test case: parseExpression(\"5+10\")\n");

    func = parseExpression((char*)"A1+B2", &success);
    assert(func.data.binaryOps.first.type == OPERAND_CELL);
    assert(func.data.binaryOps.first.data.cell.col == 1 && func.data.binaryOps.first.data.cell.row == 1);
    assert(func.data.binaryOps.second.type == OPERAND_CELL);
    assert(func.data.binaryOps.second.data.cell.col == 2 && func.data.binaryOps.second.data.cell.row == 2);
    printf("Passed test case: parseExpression(\"A1+B2\")\n");

    printf("parseExpression tests passed.\n");
}

int main() {
    printf("Starting all tests...\n");

    test_convert_to_int();
    test_parseCellReference();
    test_parseBinaryOp();
    test_parseExpression();

    printf("All tests passed.\n");
    return 0;

}
