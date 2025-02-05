#include "backend.h"
#include "parser.h"
#include <stdlib.h>
#include <string.h>

// Global variables to store the spreadsheet data and its dimensions
static CellData **XCL;
static int rows, cols;

// Initialize the backend with the given number of rows and columns
void initBackend(int r, int c) {
    rows = r;
    cols = c;

    // Allocate memory for the rows
    XCL = (CellData**)malloc(rows * sizeof(CellData*));
    for (int i = 0; i < rows; i++) {
        // Allocate memory for the columns in each row
        XCL[i] = (CellData*)malloc(cols * sizeof(CellData));
        for (int j = 0; j < cols; j++) {
            // Initialize each cell with default values
            XCL[i][j].cell.row = i;
            XCL[i][j].cell.col = j;
            XCL[i][j].value = 0;
            XCL[i][j].dependents = newVec(0);
            XCL[i][j].dependencies = newVec(0);
            XCL[i][j].function.type = CONSTANT;
            XCL[i][j].function.data.value = 0;
            XCL[i][j].error = NO_ERROR;
        }
    }
}

// Get the value of a cell and its error status
int getCellValue(Cell cell, CellError *error) {
    *error = XCL[cell.row][cell.col].error;
    return XCL[cell.row][cell.col].value;
}

// Check if there is a cycle starting from a given cell
static bool isInCycle(Cell start, Cell current, bool **visited) {
    if (visited[current.row][current.col]) {
        return current.row == start.row && current.col == start.col;
    }

    visited[current.row][current.col] = true;
    CellData *currentCell = &XCL[current.row][current.col];

    for (int i = 0; i < currentCell->dependencies.size; i++) {
        Cell *dep = get(&currentCell->dependencies, i);
        if (isInCycle(start, *dep, visited)) {
            return true;
        }
    }

    visited[current.row][current.col] = false;
    return false;
}

// Check for circular dependency in the spreadsheet
static bool checkCircularDependency(Cell cell) {
    bool **visited = malloc(rows * sizeof(bool*));
    for (int i = 0; i < rows; i++) {
        visited[i] = calloc(cols, sizeof(bool));
    }

    bool hasCycle = isInCycle(cell, cell, visited);

    for (int i = 0; i < rows; i++) {
        free(visited[i]);
    }
    free(visited);

    return hasCycle;
}

// Update the dependencies of a cell based on its function
static void update_on_which_dep(Cell cell) {
    CellData *cellData = &XCL[cell.row][cell.col];

    // Clear old dependencies
    clear(&cellData->dependencies);

    // Add new dependencies based on function type
    switch (cellData->function.type) {
        case PLUS_OP:
        case MINUS_OP:
        case MULTIPLY_OP:
        case DIVIDE_OP: {
            BinaryOp *bop = &cellData->function.data.binaryOps;
            if (bop->first.type == OPERAND_CELL) {
                push(&cellData->dependencies, &bop->first.data.cell);
            }
            if (bop->second.type == OPERAND_CELL) {
                push(&cellData->dependencies, &bop->second.data.cell);
            }
            break;
        }
        case MIN_FUNCTION:
        case MAX_FUNCTION:
        case AVG_FUNCTION:
        case SUM_FUNCTION:
        case STDEV_FUNCTION: {
            RangeFunction *rangeFunc = &cellData->function.data.rangeFunctions;
            for (int i = rangeFunc->topLeft.row; i <= rangeFunc->bottomRight.row; i++) {
                for (int j = rangeFunc->topLeft.col; j <= rangeFunc->bottomRight.col; j++) {
                    Cell dep = {i, j};
                    push(&cellData->dependencies, &dep);
                }
            }
            break;
        }
        default:
            break;
    }
}

// Recursively update all cells that depend on a given cell
static void update_that_depend(Cell cell) {
    CellData *cellData = &XCL[cell.row][cell.col];

    for (int i = 0; i < cellData->dependents.size; i++) {
        Cell *dependent = get(&cellData->dependents, i);
        CellData *depCell = &XCL[dependent->row][dependent->col];

        // Recalculate the dependent cell's value
        CellError error = NO_ERROR;
        depCell->value = evaluateExpression(&depCell->function, &error);
        depCell->error = error;

        // Continue updating dependents
        update_that_depend(*dependent);
    }
}

// Set the value of a cell based on an expression
ExpressionError setCellValue(Cell cell, char *expression) {
    // 1. Parse the expression
    bool success;
    Function newFunction = parseExpression(expression, &success);
    if (!success) {
        return COULD_NOT_PARSE;
    }

    CellData *cellData = &XCL[cell.row][cell.col];

    // Store the new function temporarily
    Function oldFunction = cellData->function;
    cellData->function = newFunction;

    // 2. Check for circular dependency with new function
    if (checkCircularDependency(cell)) {
        // Restore old function if circular dependency is found
        cellData->function = oldFunction;
        return CIRCULAR_DEPENDENCY;
    }

    // 3. Update dependencies
    update_on_which_dep(cell);

    // 4. Calculate new value
    CellError error = NO_ERROR;
    int newValue = evaluateExpression(&cellData->function, &error);

    if (error != NO_ERROR) {
        cellData->error = error;
        cellData->value = 0;  // Reset value on error
    } else {
        cellData->error = NO_ERROR;
        cellData->value = newValue;
    }

    // 5. Update all dependent cells
    update_that_depend(cell);

    return NONE;
}

// Evaluate an expression and return its value
static int evaluateExpression(Function *func, CellError *error) {
    *error = NO_ERROR;

    switch (func->type) {
        case PLUS_OP:
            return plusOp(XCL, func->data.binaryOps);
        case MINUS_OP:
            return minusOp(XCL, func->data.binaryOps);
        case MULTIPLY_OP:
            return multiplyOp(XCL, func->data.binaryOps);
        case DIVIDE_OP:
            return divideOp(XCL, func->data.binaryOps);
        case MIN_FUNCTION:
            return minFunction(XCL, func->data.rangeFunctions);
        case MAX_FUNCTION:
            return maxFunction(XCL, func->data.rangeFunctions);
        case AVG_FUNCTION:
            return avgFunction(XCL, func->data.rangeFunctions);
        case SUM_FUNCTION:
            return sumFunction(XCL, func->data.rangeFunctions);
        case STDEV_FUNCTION:
            return stdevFunction(XCL, func->data.rangeFunctions);
        case SLEEP_FUNCTION:
            return sleepFunction(func->data.value);
        case CONSTANT:
            return func->data.value;
        default:
            *error = DEPENDENCY_ERROR;
            return 0;
    }
}
