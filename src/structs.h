//
// Created by paradocx on 1/18/25.
//
#pragma once
#include<stdbool.h>

#include "vec.h"
#include "cell.h"


typedef enum {
    NO_ERROR = 0,
    DIVIDE_BY_ZERO,
    DEPENDENCY_ERROR // depends on cell which has div by zero
} CellError;


// BinaryOperation is slightly complicated because it can be either a cell or a constant
typedef enum OperandType {
    OPERAND_INT,
    OPERAND_CELL
} OperandType;

typedef struct Operand {
    OperandType type;
    union {
        Cell cell;
        int value;
    } data;
} Operand;

typedef struct BinaryOp {
    Operand first;
    Operand second;
} BinaryOp;

typedef struct RangeFunction {
    Cell topLeft;
    Cell bottomRight;
} RangeFunction;


typedef enum FunctionType {
    // I explicitly mention this because zero initialization should be a
    // valid state
    CONSTANT = 0,
    MIN_FUNCTION,
    MAX_FUNCTION,
    AVG_FUNCTION,
    SUM_FUNCTION,
    STDEV_FUNCTION,
    SLEEP_FUNCTION,
    PLUS_OP, // Identity function can be written as A1+0
    MINUS_OP,
    MULTIPLY_OP,
    DIVIDE_OP
} FunctionType;

typedef struct Function {
    FunctionType type;
    union {
        RangeFunction rangeFunctions; // Used for MinFunction, MaxFunction, AvgFunction, SumFunction, StdevFunction
        BinaryOp binaryOps;          // Used for PlusOp, MinusOp, MultiplyOp, DivideOp
        Operand sleep_value;        // Used for SleepFunction
        int value;                 // Used for Constant
    } data;
} Function;

bool isRangeFunction(FunctionType type);
bool isBinaryOp(FunctionType type);


/*
 * Stores the cell and the graph data
 * The parents ie the dependencies are not stored directly
 * but can be inferred from the function object
 * */
typedef struct CellData {
    /**
     * The children
     */
    Vec dependents;
    /**
     * Cells that this cell depends on
     */
    Function function;
    CellError error;
    /**
     * The number of parents that need to be recalculated before this one can be
     */
    int dirty_parents;
    int value;
    /**
     * Useful for DFS
     * */
} CellData;


int minFunction(CellData** cells, RangeFunction *rangeFunction, CellError *error);
int maxFunction(CellData** cells, RangeFunction *rangeFunction, CellError *error);
int avgFunction(CellData** cells, RangeFunction *rangeFunction, CellError *error);
int sumFunction(CellData** cells, RangeFunction *rangeFunction, CellError *error);
int stdevFunction(CellData** cells, RangeFunction *rangeFunction, CellError *error);
int sleepFunction(CellData** cells, Operand *sleep_value, CellError *error);
int plusOp(CellData** cells, BinaryOp *binaryOp, CellError *error);
int minusOp(CellData** cells, BinaryOp *binaryOp, CellError *error);
int multiplyOp(CellData** cells, BinaryOp *binaryOp, CellError *error);
int divideOp(CellData** cells, BinaryOp *binaryOp, CellError *error);
