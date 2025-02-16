
#include "backend.h"
#include "parser.h"
#include <stdlib.h>

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
            XCL[i][j].value = 0;
            XCL[i][j].dependents = newVec(0);
            XCL[i][j].dependencies = newVec(0);
            XCL[i][j].function.type = CONSTANT;
            XCL[i][j].function.data.value = 0;
            XCL[i][j].error = NO_ERROR;
            XCL[i][j].dirty_parents = 0;
            XCL[i][j].found = false;
        }
    }
}

int getCellValue(Cell cell, CellError *error) {
    *error = XCL[cell.row][cell.col].error;
    return XCL[cell.row][cell.col].value;
}

static void resetFound(Cell start) {
    Vec stack = newVec(10);// Initialize stack with initial capacity of 10
    push(&stack, start);
    XCL[start.row][start.col].found = false;

    while (stack.size > 0) {
        Cell top = pop(&stack);

        CellData *topCellData = &XCL[top.row][top.col];
        Vec* dependents = &topCellData->dependents;

        for (int i = 0; i < dependents->size; ++i) {
            Cell dep = get(dependents, i);
            if (XCL[dep.row][dep.col].found) {
                push(&stack, dep);
                XCL[dep.row][dep.col].found = false;
            }
        }
    }

    freeVec(&stack);
}

static bool isInCycle(Cell start) {

    //do it via hash set

    Vec stack = newVec(10); // Initialize stack with initial capacity of 10
    push(&stack, start);

    while (stack.size > 0) {
        Cell top = pop(&stack);

        CellData *topCellData = &XCL[top.row][top.col];
        Vec* dependents = &topCellData->dependents;

        for (int i = 0; i < dependents->size; ++i) {
            Cell dep = get(dependents, i);
            if (dep.row == start.row && dep.col == start.col) {
                freeVec(&stack);
                resetFound(start);
                return true;
            }

            if (!XCL[dep.row][dep.col].found) {
                push(&stack, dep);
                XCL[dep.row][dep.col].found = true;
            }
        }
    }

    freeVec(&stack);
    resetFound(start);
    return false;
}

static bool checkCircularDependency(Cell cell) {
    return isInCycle(cell);
}

/**
 * Removes all the old edges of the cell, and inserts new one according to its function.
 * This includes both the in and out edges
 * */
static void update_graph(Cell cell) {
    CellData *cellData = &XCL[cell.row][cell.col];

    //remove in edges => remove itself from the dependants of the old dependencies
    for (int i = 0; i < cellData->dependencies.size; i++) {
        Cell dep = get(&cellData->dependencies, i);
        CellData *depCell = &XCL[dep.row][dep.col];
        removeItem(&depCell->dependents, cell);
    }

    //remove out edges
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

        case SLEEP_FUNCTION: {
            Operand *sleepValue = &cellData->function.data.sleep_value;
            if (sleepValue->type == OPERAND_CELL) {
                push(&cellData->dependencies, sleepValue->data.cell);
                push(&XCL[sleepValue->data.cell.row][sleepValue->data.cell.col].dependents, cell);
            }
            break;
        }

        default:
            break;
    }
}

/**
 * Sets the number of *direct* parents which are dirty, starting from the given cell.
 * Considers the root to be dirty, therefore the direct children of the root
 * will have dirty_parents >= 1.
 * The dirty_parents of root is set to 0
 * */
static void setDirtyParents(Cell cell, Vec* stack) {
    push(stack,cell);
    XCL[cell.row][cell.col].dirty_parents = 0;

    while(stack->size) {
        Cell top = pop(stack);
        Vec* children = &XCL[top.row][top.col].dependents;
        for (int i = 0; i < children->size; ++i) {
            Cell child = get(children, i);
            CellData *childData = &XCL[child.row][child.col];

            if (childData->dirty_parents == 0) { //found for the first time
                push(stack, child);
            }
            childData->dirty_parents++;
        }
    }
}
/**
 * Recursively update the value of the cells that depend on the given cell.
 * This performs sort of a toposort, and then updates the value of the cells
 * */
static void update_dependants(Cell cell) {

    CellData *cellData = &XCL[cell.row][cell.col];
    Vec stack = newVec(cellData->dependents.size);
    setDirtyParents(cell, &stack);

    //doing this for root out of the main loop to
    //avoid re calculating the root twice
    for (int i = 0; i < cellData->dependents.size; ++i) {
        Cell child = get(&cellData->dependents, i);
        CellData* childData = &XCL[child.row][child.col];
        childData->dirty_parents--;
        if (childData->dirty_parents == 0) {
            push(&stack,child);
        }
    }

    while (stack.size) {
        Cell top = pop(&stack);
        CellData *topCell = &XCL[top.row][top.col];
        topCell->value = evaluateExpression(&topCell->function, &topCell->error);

        for (int i = 0; i < topCell->dependents.size; ++i) {
            Cell child = get(&topCell->dependents, i);
            CellData* childData = &XCL[child.row][child.col];
            childData->dirty_parents--;
            if (childData->dirty_parents == 0) {
                push(&stack,child);
            }
        }
    }

    freeVec(&stack);
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

    // Shortcut if the entered function was a constant function
    // or can be evaluated to a constant
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

    // 2. Update graph
    cellData->function = newFunction;
    update_graph(cell);

    // Check for circular dependency
    if (checkCircularDependency(cell)) {
        cellData->function = oldFunction;
        update_graph(cell);
        return CIRCULAR_DEPENDENCY;
    }

    // 4. Calculate new value
    CellError error = NO_ERROR;
    int newValue = evaluateExpression(&cellData->function, &error);

    if ((cellData->error = error) != NO_ERROR) {
        cellData->value = 0;  // Reset value on error
    } else {
        cellData->value = newValue;
    }

    update_dependants(cell);

    return NONE;
}

// Evaluate an expression and return its value
static int evaluateExpression(Function *func, CellError *error) {

    switch (func->type) {
        case PLUS_OP:
            return plusOp(XCL, &func->data.binaryOps, error);
        case MINUS_OP:
            return minusOp(XCL, &func->data.binaryOps, error);
        case MULTIPLY_OP:
            return multiplyOp(XCL, &func->data.binaryOps, error);
        case DIVIDE_OP:
            return divideOp(XCL, &func->data.binaryOps, error);
        case MIN_FUNCTION:
            return minFunction(XCL, &func->data.rangeFunctions, error);
        case MAX_FUNCTION:
            return maxFunction(XCL, &func->data.rangeFunctions, error);
        case AVG_FUNCTION:
            return avgFunction(XCL, &func->data.rangeFunctions, error);
        case SUM_FUNCTION:
            return sumFunction(XCL, &func->data.rangeFunctions, error);
        case STDEV_FUNCTION:
            return stdevFunction(XCL, &func->data.rangeFunctions, error);
        case SLEEP_FUNCTION:
            return sleepFunction(XCL, &func->data.sleep_value, error);
        case CONSTANT:
            *error = NO_ERROR;
            return func->data.value;

    }
}
