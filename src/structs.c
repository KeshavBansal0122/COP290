#include "structs.h"
#include <math.h>
#include <limits.h>
#ifdef _WIN32
#include <Windows.h>
void sleep(int sleepValue){
    Sleep(sleepValue * 1000);
}
#else
#include <unistd.h>
#endif

int minFunction(CellData** cells, RangeFunction rangeFunction, CellError *error){
    int min = INT_MAX;
    *error = NO_ERROR;
    for (int i = rangeFunction.topLeft.row; i <= rangeFunction.bottomRight.row; i++) {
        for (int j = rangeFunction.topLeft.col; j <= rangeFunction.bottomRight.col; j++) {
            if(cells[i][j].error != NO_ERROR){
                *error = DEPENDENCY_ERROR;
                return 0;
            }
            if (cells[i][j].value < min) {
                min = cells[i][j].value;
            }
        }
    }
    return min;
}

int maxFunction(CellData** cells, RangeFunction rangeFunction, CellError *error){
    *error = NO_ERROR;
    int max = INT_MIN;
    for (int i = rangeFunction.topLeft.row; i <= rangeFunction.bottomRight.row; i++) {
        for (int j = rangeFunction.topLeft.col; j <= rangeFunction.bottomRight.col; j++) {
            if(cells[i][j].error != NO_ERROR){
                *error = DEPENDENCY_ERROR;
                return 0;
            }
            if (cells[i][j].value > max) {
                max = cells[i][j].value;
            }
        }
    }
    return max;
}

int avgFunction(CellData** cells, RangeFunction rangeFunction, CellError *error){
    *error = NO_ERROR;
    int sum = 0;
    int count = 0;
    for (int i = rangeFunction.topLeft.row; i <= rangeFunction.bottomRight.row; i++) {
        for (int j = rangeFunction.topLeft.col; j <= rangeFunction.bottomRight.col; j++) {
            if(cells[i][j].error != NO_ERROR){
                *error = DEPENDENCY_ERROR;
                return 0;
            }
            sum += cells[i][j].value;
            count++;
        }
    }
    return sum / count;
}

int sumFunction(CellData** cells, RangeFunction rangeFunction, CellError *error){
    *error = NO_ERROR;
    int sum = 0;
    for (int i = rangeFunction.topLeft.row; i <= rangeFunction.bottomRight.row; i++) {
        for (int j = rangeFunction.topLeft.col; j <= rangeFunction.bottomRight.col; j++) {
            if(cells[i][j].error != NO_ERROR){
                *error = DEPENDENCY_ERROR;
                return 0;
            }
            sum += cells[i][j].value;
        }
    }
    return sum;
}


int stdevFunction(CellData** cells, RangeFunction rangeFunction, CellError *error){
    *error = NO_ERROR;
    int sum = 0;
    int count = 0;
    for (int i = rangeFunction.topLeft.row; i <= rangeFunction.bottomRight.row; i++) {
        for (int j = rangeFunction.topLeft.col; j <= rangeFunction.bottomRight.col; j++) {
            if(cells[i][j].error != NO_ERROR){
                *error = DEPENDENCY_ERROR;
                return 0;
            }
            sum += cells[i][j].value;
            count++;
        }
    }
    // do all rounding at the end
    double mean = (double)sum / count;
    double sumOfSquares = 0;
    for (int i = rangeFunction.topLeft.row; i <= rangeFunction.bottomRight.row; i++) {
        for (int j = rangeFunction.topLeft.col; j <= rangeFunction.bottomRight.col; j++) {
            sumOfSquares += (cells[i][j].value - mean) * (cells[i][j].value - mean);
        }
    }
    return (int) round(sqrt(sumOfSquares / count));
}

int sleepFunction(int sleepValue){
    sleep(sleepValue);
    return sleepValue;
}

int plusOp(CellData** cells, BinaryOp binaryOp, CellError *error){
    *error = NO_ERROR;
    int first, second;
    if (binaryOp.first.type == OPERAND_CELL) {
        if(cells[binaryOp.first.data.cell.row][binaryOp.first.data.cell.col].error != NO_ERROR){
            *error = DEPENDENCY_ERROR;
            return 0;
        }
        first = cells[binaryOp.first.data.cell.row][binaryOp.first.data.cell.col].value;
    } else {
        first = binaryOp.first.data.value;
    }
    if (binaryOp.second.type == OPERAND_CELL) {
        if(cells[binaryOp.second.data.cell.row][binaryOp.second.data.cell.col].error != NO_ERROR){
            *error = DEPENDENCY_ERROR;
            return 0;
        }
        second = cells[binaryOp.second.data.cell.row][binaryOp.second.data.cell.col].value;
    } else {
        second = binaryOp.second.data.value;
    }
    return first + second;
}

int minusOp(CellData** cells, BinaryOp binaryOp, CellError *error){
    *error = NO_ERROR;
    int first, second;
    if (binaryOp.first.type == OPERAND_CELL) {
        if(cells[binaryOp.first.data.cell.row][binaryOp.first.data.cell.col].error != NO_ERROR){
            *error = DEPENDENCY_ERROR;
            return 0;
        }
        first = cells[binaryOp.first.data.cell.row][binaryOp.first.data.cell.col].value;
    } else {
        first = binaryOp.first.data.value;
    }
    if (binaryOp.second.type == OPERAND_CELL) {
        if(cells[binaryOp.second.data.cell.row][binaryOp.second.data.cell.col].error != NO_ERROR){
            *error = DEPENDENCY_ERROR;
            return 0;
        }
        second = cells[binaryOp.second.data.cell.row][binaryOp.second.data.cell.col].value;
    } else {
        second = binaryOp.second.data.value;
    }
    return first - second;
}

int multiplyOp(CellData** cells, BinaryOp binaryOp, CellError *error){
    *error = NO_ERROR;
    int first, second;
    if (binaryOp.first.type == OPERAND_CELL) {
        if(cells[binaryOp.first.data.cell.row][binaryOp.first.data.cell.col].error != NO_ERROR){
            *error = DEPENDENCY_ERROR;
            return 0;
        }
        first = cells[binaryOp.first.data.cell.row][binaryOp.first.data.cell.col].value;
    } else {
        first = binaryOp.first.data.value;
    }
    if (binaryOp.second.type == OPERAND_CELL) {
        if(cells[binaryOp.second.data.cell.row][binaryOp.second.data.cell.col].error != NO_ERROR){
            *error = DEPENDENCY_ERROR;
            return 0;
        }
        second = cells[binaryOp.second.data.cell.row][binaryOp.second.data.cell.col].value;
    } else {
        second = binaryOp.second.data.value;
    }
    return first - second;
}

int divideOp(CellData** cells, BinaryOp binaryOp, CellError *error){
    *error = NO_ERROR;
    int first, second;
    if (binaryOp.first.type == OPERAND_CELL) {
        if(cells[binaryOp.first.data.cell.row][binaryOp.first.data.cell.col].error != NO_ERROR){
            *error = DEPENDENCY_ERROR;
            return 0;
        }
        first = cells[binaryOp.first.data.cell.row][binaryOp.first.data.cell.col].value;
    } else {
        first = binaryOp.first.data.value;
    }
    if (binaryOp.second.type == OPERAND_CELL) {
        if(cells[binaryOp.second.data.cell.row][binaryOp.second.data.cell.col].error != NO_ERROR){
            *error = DEPENDENCY_ERROR;
            return 0;
        }
        second = cells[binaryOp.second.data.cell.row][binaryOp.second.data.cell.col].value;
    } else {
        second = binaryOp.second.data.value;
        if(!second){
            *error = DIVIDE_BY_ZERO;
            return 0;
        }
    }
    return first/second;
}
