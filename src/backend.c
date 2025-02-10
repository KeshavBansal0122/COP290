
#include "backend.h"
#include "parser.h"
#include <stdlib.h>
#include <string.h>

// Global variables to store the spreadsheet data and its dimensions
static CellData **XCL;
static int rows, cols;

static int evaluateExpression(Function *func, CellError *error);

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

int getCellValue(Cell cell, CellError *error) {
    *error = XCL[cell.row][cell.col].error;
    return XCL[cell.row][cell.col].value;
}

//todo: Remove recursion ~ done
static bool isInCycle(Cell start, Cell current, bool **visited) {
    Vec stack = newVec(10); // Initialize stack with initial capacity of 10
    push(&stack, current);

    while (stack.size > 0) {
        Cell top = pop(&stack);

        if (visited[top.row][top.col]) {
            if (top.row == start.row && top.col == start.col) {
                freeVec(&stack);
                return true;
            }
            continue;
        }

        visited[top.row][top.col] = true;
        CellData *currentCell = &XCL[top.row][top.col];

        for (int i = 0; i < currentCell->dependencies.size; i++) {
            Cell dep = get(&currentCell->dependencies, i);
            push(&stack, dep);
        }
    }

    freeVec(&stack);
    return false;
}

//todo: remove allocating an entire 2d array, move this to cell data ~ kuch toh kara h
//Galat hi kiya h
static bool checkCircularDependency(Cell cell) {
    Vec visitedCells = newVec(10);
    bool hasCycle = isInCycle(cell, cell, &visitedCells);
    freeVec(&visitedCells);
    return hasCycle;
}

/**
 * Removes all the old edges of the cell, and inserts new one according to its function.
 * This includes both the in and out edges
 * */
static void update_graph(Cell cell) {
    CellData *cellData = &XCL[cell.row][cell.col];

    // todo: clear this cell from the old dependencies' dependants ~ done
    // todo: update dependants ~ done
    // Clear old dependencies ~ done
    for (int i = 0; i < cellData->dependencies.size; i++) {
        Cell dep = get(&cellData->dependencies, i);
        CellData *depCell = &XCL[dep.row][dep.col];
        for (int j = 0; j < depCell->dependents.size; j++) {
            Cell dependent = get(&depCell->dependents, j);
            if (dependent.row == cell.row && dependent.col == cell.col) {
                removeAt(&depCell->dependents, j);
                break;
            }
        }
    }
    clear(&cellData->dependencies);

    // Add new dependencies based on function type
    switch (cellData->function.type) {
        case PLUS_OP:
        case MINUS_OP:
        case MULTIPLY_OP:
        case DIVIDE_OP: {
            BinaryOp *bop = &cellData->function.data.binaryOps;
            if (bop->first.type == OPERAND_CELL) {
                push(&cellData->dependencies, bop->first.data.cell);
                push(&XCL[bop->first.data.cell.row][bop->first.data.cell.col].dependents, cell);
            }
            if (bop->second.type == OPERAND_CELL) {
                push(&cellData->dependencies, bop->second.data.cell);
                push(&XCL[bop->second.data.cell.row][bop->second.data.cell.col].dependents, cell);
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
                    push(&cellData->dependencies, dep);
                    push(&XCL[i][j].dependents, cell);
                }
            }
            break;
        }
        default:
            break;
    }
}

/**
 * Recursively update the value of the cells that depend on the given cell
 * */
static void update_dependants(Cell cell) {
    CellData *cellData = &XCL[cell.row][cell.col];

    for (int i = 0; i < cellData->dependents.size; i++) {
        Cell dependent = get(&cellData->dependents, i);
        CellData *depCell = &XCL[dependent.row][dependent.col];

        // Recalculate the dependent cell's value
        CellError error = NO_ERROR;
        depCell->value = evaluateExpression(&depCell->function, &error);
        depCell->error = error;

        // Continue updating dependents
        update_dependants(dependent);
    }
}

/**
 * @return if the function can be safely replaced with a constant
 */
static bool isExpressionConstant(Function *func) {
    switch (func->type) {
        case PLUS_OP:
        case MINUS_OP:
        case MULTIPLY_OP:
        case DIVIDE_OP:
            return func->data.binaryOps.first.type == OPERAND_INT &&
                    func->data.binaryOps.second.type == OPERAND_INT;
        case CONSTANT:
            return true;
        default:
            return false;
    }
}

static Function constantFunction(int value) {
    Function func;
    func.type = CONSTANT;
    func.data.value = value;
    return func;
}

// Set the value of a cell based on an expression
ExpressionError setCellValue(Cell cell, char *expression) {
    // Parse the expression
    bool success;
    Function newFunction = parseExpression(expression, &success);
    if (!success) {
        return COULD_NOT_PARSE;
    }

    CellData *cellData = &XCL[cell.row][cell.col];
    if(isExpressionConstant(&newFunction)){
        cellData->value = evaluateExpression(&newFunction, &cellData->error);
        cellData->function = constantFunction(cellData->value);
        update_graph(cell);
        update_dependants(cell);
        return NONE;
    }

    // 1. Save old function
    Function oldFunction = cellData->function;

    //todo: update graph before checking for circular dependency
    // 2. Update graph
    cellData->function = newFunction;
    update_graph(cell);

    // Check for circular dependency
    if (checkCircularDependency(cell)) {
        cellData->function = oldFunction;
        return CIRCULAR_DEPENDENCY;
    }

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

    update_dependants(cell);

    return NONE;
}

// Evaluate an expression and return its value
static int evaluateExpression(Function *func, CellError *error) {

    switch (func->type) {
        case PLUS_OP:
            return plusOp(XCL, func->data.binaryOps, error);
        case MINUS_OP:
            return minusOp(XCL, func->data.binaryOps, error);
        case MULTIPLY_OP:
            return multiplyOp(XCL, func->data.binaryOps, error);
        case DIVIDE_OP:
            return divideOp(XCL, func->data.binaryOps, error);
        case MIN_FUNCTION:
            return minFunction(XCL, func->data.rangeFunctions, error);
        case MAX_FUNCTION:
            return maxFunction(XCL, func->data.rangeFunctions, error);
        case AVG_FUNCTION:
            return avgFunction(XCL, func->data.rangeFunctions, error);
        case SUM_FUNCTION:
            return sumFunction(XCL, func->data.rangeFunctions, error);
        case STDEV_FUNCTION:
            return stdevFunction(XCL, func->data.rangeFunctions, error);
        case SLEEP_FUNCTION:
            return sleepFunction(func->data.value);
        case CONSTANT:
            return func->data.value;
        default:
            *error = DEPENDENCY_ERROR;
            return 0;
    }
}
