#include "structs.h"
#include <math.h>
#include <unistd.h>
#include <limits.h>

int minFunction(CellData** cells, RangeFunction rangeFunction){
    int min = INT_MAX;
    for (int i = rangeFunction.topLeft.row; i <= rangeFunction.bottomRight.row; i++) {
        for (int j = rangeFunction.topLeft.col; j <= rangeFunction.bottomRight.col; j++) {
            if (cells[i][j].value < min) {
                min = cells[i][j].value;
            }
        }
    }
    return min;
}

int maxFunction(CellData** cells, RangeFunction rangeFunction){
    int max = INT_MIN;
    for (int i = rangeFunction.topLeft.row; i <= rangeFunction.bottomRight.row; i++) {
        for (int j = rangeFunction.topLeft.col; j <= rangeFunction.bottomRight.col; j++) {
            if (cells[i][j].value > max) {
                max = cells[i][j].value;
            }
        }
    }
    return max;
}

int avgFunction(CellData** cells, RangeFunction rangeFunction){
    int sum = 0;
    int count = 0;
    for (int i = rangeFunction.topLeft.row; i <= rangeFunction.bottomRight.row; i++) {
        for (int j = rangeFunction.topLeft.col; j <= rangeFunction.bottomRight.col; j++) {
            sum += cells[i][j].value;
            count++;
        }
    }
    return sum / count;
}

int sumFunction(CellData** cells, RangeFunction rangeFunction){
    int sum = 0;
    for (int i = rangeFunction.topLeft.row; i <= rangeFunction.bottomRight.row; i++) {
        for (int j = rangeFunction.topLeft.col; j <= rangeFunction.bottomRight.col; j++) {
            sum += cells[i][j].value;
        }
    }
    return sum;
}


int stdevFunction(CellData** cells, RangeFunction rangeFunction){
    int sum = 0;
    int count = 0;
    for (int i = rangeFunction.topLeft.row; i <= rangeFunction.bottomRight.row; i++) {
        for (int j = rangeFunction.topLeft.col; j <= rangeFunction.bottomRight.col; j++) {
            sum += cells[i][j].value;
            count++;
        }
    }
    int mean = sum / count;
    int sumOfSquares = 0;
    for (int i = rangeFunction.topLeft.row; i <= rangeFunction.bottomRight.row; i++) {
        for (int j = rangeFunction.topLeft.col; j <= rangeFunction.bottomRight.col; j++) {
            sumOfSquares += (cells[i][j].value - mean) * (cells[i][j].value - mean);
        }
    }
    return sqrt(sumOfSquares / count);
}

int sleepFunction(int sleepValue){
    sleep(sleepValue);
    return 0;
}

int plusOp(CellData** cells, BinaryOp binaryOp){
    int first = 0;
    int second = 0;
    if (binaryOp.first.type == OPERAND_CELL) {
        first = cells[binaryOp.first.data.cell.row][binaryOp.first.data.cell.col].value;
    } else {
        first = binaryOp.first.data.value;
    }
    if (binaryOp.second.type == OPERAND_CELL) {
        second = cells[binaryOp.second.data.cell.row][binaryOp.second.data.cell.col].value;
    } else {
        second = binaryOp.second.data.value;
    }
    return first + second;
}

int minusOp(CellData** cells, BinaryOp binaryOp){
    int first = 0;
    int second = 0;
    if (binaryOp.first.type == OPERAND_CELL) {
        first = cells[binaryOp.first.data.cell.row][binaryOp.first.data.cell.col].value;
    } else {
        first = binaryOp.first.data.value;
    }
    if (binaryOp.second.type == OPERAND_CELL) {
        second = cells[binaryOp.second.data.cell.row][binaryOp.second.data.cell.col].value;
    } else {
        second = binaryOp.second.data.value;
    }
    return first - second;
}

int multiplyOp(CellData** cells, BinaryOp binaryOp){
    int first = 0;
    int second = 0;
    if (binaryOp.first.type == OPERAND_CELL) {
        first = cells[binaryOp.first.data.cell.row][binaryOp.first.data.cell.col].value;
    } else {
        first = binaryOp.first.data.value;
    }
    if (binaryOp.second.type == OPERAND_CELL) {
        second = cells[binaryOp.second.data.cell.row][binaryOp.second.data.cell.col].value;
    } else {
        second = binaryOp.second.data.value;
    }
    return first * second;
}

int divideOp(CellData** cells, BinaryOp binaryOp){
    int first = 0;
    int second = 0;
    if (binaryOp.first.type == OPERAND_CELL) {
        first = cells[binaryOp.first.data.cell.row][binaryOp.first.data.cell.col].value;
    } else {
        first = binaryOp.first.data.value;
    }
    if (binaryOp.second.type == OPERAND_CELL) {
        second = cells[binaryOp.second.data.cell.row][binaryOp.second.data.cell.col].value;
    } else {
        second = binaryOp.second.data.value;
    }
    return first / second;
}
