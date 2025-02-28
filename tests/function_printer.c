#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../src/structs.h"

// Helper function to convert a Cell to string
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

char* cell_to_string(Cell cell) {
    int row2 = cell.row;
    int col2 = cell.col;
//    printf("converting (%d, %d)", row2, col2);
    char* result = malloc(10);  // Enough space for "ZZ999\0"
    char* col = numberToColumnHeader(cell.col);
    sprintf(result, "%s%d", col, cell.row+1);
    free(col);
    return result;
}

// Helper function to convert an Operand to string
char* operand_to_string(Operand operand) {
    char* result = malloc(20);  // Sufficient space for either case
    if (operand.type == OPERAND_CELL) {
        char* cell_str = cell_to_string(operand.data.cell);
        strcpy(result, cell_str);
        free(cell_str);
    } else {  // OPERAND_INT
        sprintf(result, "%d", operand.data.value);
    }
    return result;
}

// Helper function to convert a RangeFunction to string
char* range_to_string(RangeFunction range) {
    char* topLeft = cell_to_string(range.topLeft);
    char* bottomRight = cell_to_string(range.bottomRight);
    char* result = malloc(strlen(topLeft) + strlen(bottomRight) + 2);
    sprintf(result, "%s:%s", topLeft, bottomRight);
    free(topLeft);
    free(bottomRight);
    return result;
}

// Main function to convert a Function to string
char *function_to_string(Function *func) {
    char* result = malloc(100);  // Initial allocation, may need to realloc for large ranges
    char* temp;

    switch (func->type) {
        case CONSTANT:
            sprintf(result, "%d", func->data.value);
            break;

        case MIN_FUNCTION:
            temp = range_to_string(func->data.rangeFunctions);
            sprintf(result, "MIN(%s)", temp);
            free(temp);
            break;

        case MAX_FUNCTION:
            temp = range_to_string(func->data.rangeFunctions);
            sprintf(result, "MAX(%s)", temp);
            free(temp);
            break;

        case AVG_FUNCTION:
            temp = range_to_string(func->data.rangeFunctions);
            sprintf(result, "AVG(%s)", temp);
            free(temp);
            break;

        case SUM_FUNCTION:
            temp = range_to_string(func->data.rangeFunctions);
            sprintf(result, "SUM(%s)", temp);
            free(temp);
            break;

        case STDEV_FUNCTION:
            temp = range_to_string(func->data.rangeFunctions);
            sprintf(result, "STDEV(%s)", temp);
            free(temp);
            break;

        case SLEEP_FUNCTION:
            temp = operand_to_string(func->data.sleep_value);
            sprintf(result, "SLEEP(%s)", temp);
            free(temp);
            break;

        case PLUS_OP: {
            char* first = operand_to_string(func->data.binaryOps.first);
            char* second = operand_to_string(func->data.binaryOps.second);
            sprintf(result, "%s + %s", first, second);
            free(first);
            free(second);
            break;
        }

        case MINUS_OP: {
            char* first = operand_to_string(func->data.binaryOps.first);
            char* second = operand_to_string(func->data.binaryOps.second);
            sprintf(result, "%s - %s", first, second);
            free(first);
            free(second);
            break;
        }

        case MULTIPLY_OP: {
            char* first = operand_to_string(func->data.binaryOps.first);
            char* second = operand_to_string(func->data.binaryOps.second);
            sprintf(result, "%s * %s", first, second);
            free(first);
            free(second);
            break;
        }

        case DIVIDE_OP: {
            char* first = operand_to_string(func->data.binaryOps.first);
            char* second = operand_to_string(func->data.binaryOps.second);
            sprintf(result, "%s / %s", first, second);
            free(first);
            free(second);
            break;
        }

        default:
            sprintf(result, "Unknown Function Type, type = %d", func->type);
    }

    return result;
}

// Function comparison for testing
bool function_equals(Function* f1, Function* f2) {
    if (f1->type != f2->type) return false;

    switch (f1->type) {
        case CONSTANT:
            return f1->data.value == f2->data.value;

        case MIN_FUNCTION:
        case MAX_FUNCTION:
        case AVG_FUNCTION:
        case SUM_FUNCTION:
        case STDEV_FUNCTION: {
            RangeFunction* r1 = &f1->data.rangeFunctions;
            RangeFunction* r2 = &f2->data.rangeFunctions;
            return r1->topLeft.col == r2->topLeft.col &&
                   r1->topLeft.row == r2->topLeft.row &&
                   r1->bottomRight.col == r2->bottomRight.col &&
                   r1->bottomRight.row == r2->bottomRight.row;
        }

        case SLEEP_FUNCTION:
            return f1->data.sleep_value.type == f2->data.sleep_value.type &&
                   (f1->data.sleep_value.type == OPERAND_INT ?
                    f1->data.sleep_value.data.value == f2->data.sleep_value.data.value :
                    (f1->data.sleep_value.data.cell.col == f2->data.sleep_value.data.cell.col &&
                     f1->data.sleep_value.data.cell.row == f2->data.sleep_value.data.cell.row));

        case PLUS_OP:
        case MINUS_OP:
        case MULTIPLY_OP:
        case DIVIDE_OP: {
            BinaryOp* b1 = &f1->data.binaryOps;
            BinaryOp* b2 = &f2->data.binaryOps;

            bool first_equal = b1->first.type == b2->first.type &&
                               (b1->first.type == OPERAND_INT ?
                                b1->first.data.value == b2->first.data.value :
                                (b1->first.data.cell.col == b2->first.data.cell.col &&
                                 b1->first.data.cell.row == b2->first.data.cell.row));

            bool second_equal = b1->second.type == b2->second.type &&
                                (b1->second.type == OPERAND_INT ?
                                 b1->second.data.value == b2->second.data.value :
                                 (b1->second.data.cell.col == b2->second.data.cell.col &&
                                  b1->second.data.cell.row == b2->second.data.cell.row));

            return first_equal && second_equal;
        }

        default:
            return false;
    }
}