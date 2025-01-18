//
// Created by paradocx on 1/19/25.
//

#pragma once
#include "utils.h"

typedef struct RangeFunction {
    Cell topLeft;
    Cell bottomRight;
} RangeFunction;

typedef struct BinaryOp {
    Cell first;
    Cell second;
} BinaryOp;

typedef enum FunctionType {
    CONSTANT,
    MIN_FUNCTION,
    MAX_FUNCTION,
    AVG_FUNCTION,
    SUM_FUNCTION,
    STDEV_FUNCTION,
    SLEEP_FUNCTION,
    PLUS_OP,
    MINUS_OP,
    MULTIPLY_OP,
    DIVIDE_OP
} FunctionType;

typedef struct Function {
    FunctionType type;
    union {
        RangeFunction rangeFunctions; // Used for MinFunction, MaxFunction, AvgFunction, SumFunction, StdevFunction
        BinaryOp binaryOps;          // Used for PlusOp, MinusOp, MultiplyOp, DivideOp
        int value;                  // Used for SleepFunction, Constant
    } data;
} Function;


int minFunction(CellData** cells, RangeFunction rangeFunction);
int maxFunction(CellData** cells, RangeFunction rangeFunction);
int avgFunction(CellData** cells, RangeFunction rangeFunction);
int sumFunction(CellData** cells, RangeFunction rangeFunction);
int stdevFunction(CellData** cells, RangeFunction rangeFunction);
int sleepFunction(int sleepValue);
int plusOp(CellData** cells, BinaryOp binaryOp);
int minusOp(CellData** cells, BinaryOp binaryOp);
int multiplyOp(CellData** cells, BinaryOp binaryOp);
int divideOp(CellData** cells, BinaryOp binaryOp);