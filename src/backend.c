
#include "backend.h"

#include <stdio.h>

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
        XCL[i] = (CellData*)malloc(cols* sizeof(CellData));
        memset(XCL[i], 0, cols * sizeof(CellData));
    }
}

void reset() {
    const Function zero = {};
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            XCL[i][j].function = zero;
            XCL[i][j].error = NO_ERROR;
            freeVec(&XCL[i][j].dependents);
            XCL[i][j].value = 0;
        }
    }
}

int getCellValue(Cell cell, CellError *error) {
    *error = XCL[cell.row][cell.col].error;
    return XCL[cell.row][cell.col].value;
}


static void resetFound(Cell start) {
    //using dirty parents in place of found tag for space savings

    Vec stack = newVec(0);
    push(&stack, start);
    XCL[start.row][start.col].dirty_parents = false;

    while (getSize(&stack) > 0) {
        Cell top = pop(&stack);

        CellData *topCellData = &XCL[top.row][top.col];
        Vec* dependents = &topCellData->dependents;

        for (int i = 0; i < getSize(dependents); ++i) {
            Cell dep = get(dependents, i);
            if (XCL[dep.row][dep.col].dirty_parents) {
                push(&stack, dep);
                XCL[dep.row][dep.col].dirty_parents = false;
            }
        }
    }

    freeVec(&stack);
}

static bool isInCycle(Cell start) {

    //using dirty parents in place of found tag for space savings

    Vec stack = newVec(0);
    push(&stack, start);

    while (getSize(&stack) > 0) {
        Cell top = pop(&stack);

        CellData *topCellData = &XCL[top.row][top.col];
        Vec* dependents = &topCellData->dependents;

        for (int i = 0; i < getSize(dependents); ++i) {
            Cell dep = get(dependents, i);
            if (dep.row == start.row && dep.col == start.col) {
                freeVec(&stack);
                resetFound(start);
                return true;
            }

            if (!XCL[dep.row][dep.col].dirty_parents) {
                push(&stack, dep);
                XCL[dep.row][dep.col].dirty_parents = true;
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
 * @param cell The cell whose edges are to be updated
 * @param oldFunction The old function of the cell. Used for removing the old edges
 * */
static void update_graph(Cell cell, Function *oldFunction) {
    CellData *cellData = &XCL[cell.row][cell.col];

    //remove in edges => remove itself from the dependants of the old dependencies
    FunctionType type = oldFunction->type;
    if (isRangeFunction(type)) {
        RangeFunction *range = &oldFunction->data.rangeFunctions;
        for(short i = range->topLeft.row; i <= range->bottomRight.row; i++) {
            for(short j = range->topLeft.col; j <= range->bottomRight.col; j++) {
                CellData *parent = &XCL[i][j];
                removeItem(&parent->dependents, cell);
            }
        }

    } else if (isBinaryOp(type)) {
        if (oldFunction->data.binaryOps.first.type == OPERAND_CELL) {
            Cell dep = cellData->function.data.binaryOps.first.data.cell;
            CellData *parent = &XCL[dep.row][dep.col];
            removeItem(&parent->dependents, cell);
        }
        if (oldFunction->data.binaryOps.second.type == OPERAND_CELL) {
            Cell dep = cellData->function.data.binaryOps.second.data.cell;
            CellData *parent = &XCL[dep.row][dep.col];
            removeItem(&parent->dependents, cell);
        }

    } else if (type == SLEEP_FUNCTION) {
        Operand *sleepValue = &oldFunction->data.sleep_value;
        if (sleepValue->type == OPERAND_CELL) {
            Cell dep = sleepValue->data.cell;
            CellData *parent = &XCL[dep.row][dep.col];
            removeItem(&parent->dependents, cell);
        }
    }

    //add new edges => add itself to the dependants of the new dependencies
    type = cellData->function.type;
    if (isBinaryOp(type)) {
        BinaryOp *bop = &cellData->function.data.binaryOps;
        if (bop->first.type == OPERAND_CELL) {
            push(&XCL[bop->first.data.cell.row][bop->first.data.cell.col].dependents, cell);
        }
        if (bop->second.type == OPERAND_CELL) {
            push(&XCL[bop->second.data.cell.row][bop->second.data.cell.col].dependents, cell);
        }

    } else if (isRangeFunction(type)) {
        RangeFunction *rangeFunc = &cellData->function.data.rangeFunctions;
        for (int i = rangeFunc->topLeft.row; i <= rangeFunc->bottomRight.row; i++) {
            for (int j = rangeFunc->topLeft.col; j <= rangeFunc->bottomRight.col; j++) {
                push(&XCL[i][j].dependents, cell);
            }
        }

    } else if (type == SLEEP_FUNCTION) {
        Operand *sleepValue = &cellData->function.data.sleep_value;
        if (sleepValue->type == OPERAND_CELL) {
            push(&XCL[sleepValue->data.cell.row][sleepValue->data.cell.col].dependents, cell);
        }
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

    while(getSize(stack)) {
        Cell top = pop(stack);
        Vec* children = &XCL[top.row][top.col].dependents;
        for (int i = 0; i < getSize(children); ++i) {
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
static void update_dependants(Cell cell, int oldValue) {

    CellData *cellData = &XCL[cell.row][cell.col];
    Vec stack = newVec(getSize(&cellData->dependents));
    setDirtyParents(cell, &stack);

    //doing this for root out of the main loop to
    //avoid re calculating the root twice
    for (int i = 0; i < getSize(&cellData->dependents); ++i) {
        Cell child = get(&cellData->dependents, i);
        CellData* childData = &XCL[child.row][child.col];
        childData->dirty_parents--;
        if (childData->dirty_parents == 0) {
            push(&stack,child);
        }
    }

    while (getSize(&stack)) {
        Cell top = pop(&stack);
        CellData *topCell = &XCL[top.row][top.col];

        if (topCell->function.type == SUM_FUNCTION) {
            topCell-> value += cellData->value - oldValue;
        }
        else if(topCell->function.type == MIN_FUNCTION){
            if(cellData->value < topCell->value){
                topCell->value = cellData->value;
            }
        }
        else if(topCell->function.type == MAX_FUNCTION){
            if(cellData->value > topCell->value){
                topCell->value = cellData->value;
            }
        }
        else {
            topCell->value = evaluateExpression(&topCell->function, &topCell->error);
        }

        for (int i = 0; i < getSize(&topCell->dependents); ++i) {
            Cell child = get(&topCell->dependents, i);
            CellData* childData = &XCL[child.row][child.col];
            childData->dirty_parents--;
            if (childData->dirty_parents == 0) {
                push(&stack, child);
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


    CellData *cellData = &XCL[cell.row][cell.col];

    // 1. Save copy of old function
    Function oldFunction = cellData->function;

    // Shortcut if the entered function was a constant function
    // or can be evaluated to a constant
    if(isExpressionConstant(&newFunction)){
        int oldValue = cellData->value;
        cellData->value = evaluateExpression(&newFunction, &cellData->error);
        cellData->function = constantFunction(cellData->value);
        update_graph(cell, &oldFunction);
        update_dependants(cell, oldValue);
        return NONE;
    }


    // 2. Update graph
    cellData->function = newFunction;
    update_graph(cell, &oldFunction);

    // Check for circular dependency
    if (checkCircularDependency(cell)) {
        cellData->function = oldFunction;
        update_graph(cell, &newFunction);
        return CIRCULAR_DEPENDENCY;
    }

    // 4. Calculate new value
    CellError error = NO_ERROR;
    int oldValue = cellData->value;
    int newValue = evaluateExpression(&cellData->function, &error);

    if ((cellData->error = error) != NO_ERROR) {
        cellData->value = 0;  // Reset value on error
    } else {
        cellData->value = newValue;
    }

    update_dependants(cell, oldValue);

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
    printf("ExpressionError has an invalid value\n");
    exit(-1);
}
