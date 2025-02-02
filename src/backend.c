#include "backend.h"
#include <stdlib.h>
#include <string.h>

//
// NEEDS CHANGES, NOT DONE
//

// Global variables to store the spreadsheet data and its dimensions
static CellData **XCL;
static int rows, cols;

// Initialize the backend with the given number of rows and columns
void initBackend(int r, int c) {
    // Store the dimensions globally
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
            vecInit(&XCL[i][j].dependents);
            vecInit(&XCL[i][j].dependencies);
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
    Vec stack = newVec(10); // Initialize stack with initial capacity of 10
    push(&stack, &current);

    while (stack.size > 0) {
        Cell *top = (Cell *)pop(&stack);

        if (visited[top->row][top->col]) {
            if (top->row == start.row && top->col == start.col) {
                freeVec(&stack);
                return true;
            }
            continue;
        }

        visited[top->row][top->col] = true;
        CellData *currentCell = &XCL[top->row][top->col];

        for (int i = 0; i < currentCell->dependencies.size; i++) {
            Cell *dep = vecGet(&currentCell->dependencies, i);
            push(&stack, dep);
        }
    }

    freeVec(&stack);
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
static void updateDependencies(Cell cell) {
    CellData *cellData = &XCL[cell.row][cell.col];
    
    // Clear old dependencies
    vecClear(&cellData->dependencies);
    
    // Add new dependencies based on function type
    switch(cellData->function.type) {
        case PLUS_OP:
        case MINUS_OP:
        case MULTIPLY_OP:
        case DIVIDE_OP: {
            BinaryOp *bop = &cellData->function.data.binaryOps;
            if (bop->first.type == OPERAND_CELL) {
                vecPush(&cellData->dependencies, &bop->first.data.cell);
            }
            if (bop->second.type == OPERAND_CELL) {
                vecPush(&cellData->dependencies, &bop->second.data.cell);
            }
            break;
        }
    }
}

// Recursively update all cells that depend on a given cell
static void updateDependents(Cell cell) {
    CellData *cellData = &XCL[cell.row][cell.col];
    
    for (int i = 0; i < cellData->dependents.size; i++) {
        Cell *dependent = vecGet(&cellData->dependents, i);
        CellData *depCell = &XCL[dependent->row][dependent->col];
        
        // Recalculate the dependent cell's value
        CellError error = NO_ERROR;
        depCell->value = evaluateExpression(&depCell->function, &error);
        depCell->error = error;
        
        // Continue updating dependents
        updateDependents(*dependent);
    }
}

// Set the value of a cell based on an expression
ExpressionError setCellValue(Cell cell, char *expression) {
    // 1. Parse the expression
    Function newFunction;
    if (parseExpression(expression, &newFunction) != 0) {
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
    updateDependencies(cell);
    
    // 4. Calculate new value
    CellError error = NO_ERROR;
    int newValue = evaluateExpression(&cellData->function, &error);
    
    if (error != NO_ERROR) {
        cellData->error = error;
        cellData->value = 0;  // Reset value on error
        // Note: We don't restore the old function here as the error is in evaluation,
        // not in the formula structure
    } else {
        cellData->error = NO_ERROR;
        cellData->value = newValue;
    }
    
    // 5. Update all dependent cells
    updateDependents(cell);
    
    return NONE;
}

// Evaluate an expression and return its value
static int evaluateExpression(Function *func, CellError *error) {
    *error = NO_ERROR;
    
    switch(func->type) {
            
        case PLUS_OP: {
            return plusOp(XCL, func->data.binaryOps);
        }
        
        case MINUS_OP: {
            return minusOp(XCL, func->data.binaryOps);
        }
        
        case MULTIPLY_OP: {
            return multiplyOp(XCL, func->data.binaryOps);
        }
        
        case DIVIDE_OP: {
            return divideOp(XCL, func->data.binaryOps);
        }

        case MIN_FUNCTION: {
            return minFunction(XCL, func->data.rangeFunctions);
        }

        case MAX_FUNCTION: {
            return maxFunction(XCL, func->data.rangeFunctions);
        }

        case AVG_FUNCTION: {
            return avgFunction(XCL, func->data.rangeFunctions);
        }

        case SUM_FUNCTION: {
            return sumFunction(XCL, func->data.rangeFunctions);
        }

        case STDEV_FUNCTION: {
            return stdevFunction(XCL, func->data.rangeFunctions);
        }

        case SLEEP_FUNCTION: {
            return sleepFunction(func->data.value);
        }

        case CONSTANT: {
            return func->data.value;
        }
        
        default:
            *error = DEPENDENCY_ERROR;
            return 0;
        }
}

// Get the formula of a cell as a string
// char* getCellFormula(Cell cell) {
//     CellData *cellData = &XCL[cell.row][cell.col];
//     char *formula = malloc(100); // Allocate space for formula string
    
//     switch(cellData->function.type) {
//         case CONSTANT:
//             sprintf(formula, "%d", cellData->function.data.value);
//             break;
            
//         case PLUS_OP:
//         case MINUS_OP:
//         case MULTIPLY_OP:
//         case DIVIDE_OP: {
//             char op = cellData->function.type == PLUS_OP ? '+' : 
//                      cellData->function.type == MINUS_OP ? '-' :
//                      cellData->function.type == MULTIPLY_OP ? '*' : '/';
            
//             BinaryOp *bop = &cellData->function.data.binaryOps;
//             if (bop->first.type == OPERAND_CELL && bop->second.type == OPERAND_CELL) {
//                 sprintf(formula, "%c%d%c%c%d", 
//                     'A' + bop->first.data.cell.col,
//                     bop->first.data.cell.row,
//                     op,
//                     'A' + bop->second.data.cell.col,
//                     bop->second.data.cell.row);
//             } else if (bop->first.type == OPERAND_CELL) {
//                 sprintf(formula, "%c%d%c%d",
//                     'A' + bop->first.data.cell.col,
//                     bop->first.data.cell.row,
//                     op,
//                     bop->second.data.value);
//             } else if (bop->second.type == OPERAND_CELL) {
//                 sprintf(formula, "%d%c%c%d",
//                     bop->first.data.value,
//                     op,
//                     'A' + bop->second.data.cell.col,
//                     bop->second.data.cell.row);
//             } else {
//                 sprintf(formula, "%d%c%d",
//                     bop->first.data.value,
//                     op,
//                     bop->second.data.value);
//             }
//             break;
//         }
        
//         // Add other function types here
//         default:
//             strcpy(formula, "");
//     }
    
//     return formula;
// }
