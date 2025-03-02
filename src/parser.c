//
// Created by paradocx on 1/18/25.

#include "structs.h"
#include <string.h>
#include <ctype.h>

static int rows, cols;

size_t next_token(const char *expression) {
    size_t index = 0;

    if (expression[index] == '\0') { // End of string, no tokens left
        return index;
    }

    if (isalnum(expression[index])) {
        while (expression[index] && isalnum(expression[index])) {
            index++;
        }
    } else {
        // It's a single special character
        index++;
    }

    return index;
}

/**
 * supports only a single leading + or - sign
 * */
int convert_to_int(const char* expression, size_t len, bool* success) {
    int result = 0;
    size_t i = 0;
    int multiplier = 1;
    if(expression[i] == '+') {
        i++;
        if(i == len) {
            *success = false;
            return 0;
        }
    }
    if(expression[i] == '-') {
        multiplier = -1;
        i++;
        if(i == len) {
            *success = false;
            return 0;
        }
    }

    while (i < len && expression[i] >= '0' && expression[i] <= '9')
    {
        result = result * 10 + (expression[i] - '0');
        i++;
    }

    *success = i == len;

    return result * multiplier;
}

void parserSetSize(int row, int col) {
    rows = row;
    cols = col;
}


Cell parseCellReference(const char *reference, size_t len, bool *success) {
    Cell cell = {0, 0};
    size_t i = 0;

    // Parse column (letters)
    while (i < len && reference[i] >= 'A' && reference[i] <= 'Z')
    {
        cell.col = cell.col * 26 + (reference[i] - 'A' + 1);  // Convert letters to column index
        i++;
    }


    if (i == len) {
        *success = false;
        return cell;
    }

    //to satisfy the inane requirement that A01 should be an invalid cell
    if (reference[i] == '0') {
        *success = false;
        return cell;
    }

    // Parse row (numbers)
    if (reference[i] >= '0' && reference[i] <= '9')
    {
        cell.row = convert_to_int(&reference[i], len - i, success);  // Convert digits to row number
    }

    cell.row--;cell.col--;
    if (cell.row < 0 || cell.row >= rows || cell.col < 0 || cell.col >= cols) {
        *success = false;
    }
    return cell;
}

Operand parseOperand(const char* expression, size_t len, bool* success) {
    Operand operand;
    if (expression[0] >= 'A' && expression[0] <= 'Z') {
        operand.type = OPERAND_CELL;
        operand.data.cell = parseCellReference(expression, len, success);
    } else {
        operand.type = OPERAND_INT;
        operand.data.value = convert_to_int(expression, len, success);
    }
    return operand;
}

/**
 * A default function returned only in the case of error
 * */
Function defaultFn() {
    Function fn;
    fn.type=CONSTANT;
    fn.data.value=0;
    return fn;
}
/**
 * A default range returned only in the case of error
 * */
RangeFunction defaultRange() {
    RangeFunction range;
    range.topLeft.row = 0;
    range.topLeft.col = 0;
    range.bottomRight.row = 0;
    range.bottomRight.col = 0;
    return range;
}

// Helper function to parse binary operation
Function parseBinaryOp(char* operand1, char* remaining, bool* success)
{
    size_t op1len = remaining - operand1;
    char operation = remaining[0];
    char* operand2 = remaining + 1;
    if (*operand2 == '\0') {
        *success = false;
        return defaultFn();
    }
    Operand op1 = parseOperand(operand1, op1len, success);
    if (!*success) {
        return defaultFn();
    }
    size_t op2len = next_token(operand2);

    //integer can be preceded by a sign
    if (operand2[0] == '-' || operand2[0] == '+') {
        op2len += next_token(operand2);
    }

    //operand2 should be the end of the string
    if (operand2[op2len] != '\0') {
        *success = false;
        return defaultFn();
    }

    Operand op2 = parseOperand(operand2, op2len, success);
    if (!*success) {
        return defaultFn();
    }
    Function fn;
    fn.data.binaryOps.first = op1;
    fn.data.binaryOps.second = op2;

    switch (operation) {
        case '+': fn.type = PLUS_OP; break;
        case '-': fn.type = MINUS_OP; break;
        case '*': fn.type = MULTIPLY_OP; break;
        case '/': fn.type = DIVIDE_OP; break;
        default: *success = false;
    }
    return fn;
}

/**
 * Parses a range and returns the corresponding range.
 * @param range_expression The beginning of the range, does not include the opening bracket
 * @param len The len of the range
 * */
RangeFunction parseRange(char* range_expression, size_t len, bool* success) {
    size_t topLeftPart = next_token(range_expression);
    if (range_expression[topLeftPart] != ':') {
        *success = false;
        return defaultRange();
    }
    Cell topLeft = parseCellReference(range_expression, topLeftPart, success);
    if (!*success) {
        return defaultRange();
    }

    size_t bottomRightPart = next_token(&range_expression[topLeftPart + 1]);
    //this should be the len of the expression
    if (topLeftPart + bottomRightPart + 1 != len) {
        *success = false;
        return defaultRange();
    }
    Cell bottomRight = parseCellReference(&range_expression[topLeftPart + 1], bottomRightPart, success);
    if (!*success) {
        return defaultRange();
    }

    if(topLeft.row > bottomRight.row || topLeft.col > bottomRight.col) {
        *success = false;
        return defaultRange();
    }

    RangeFunction range = {topLeft, bottomRight};
    return range;
}

Function parseFunction(char* expression, size_t bracketStart, bool* success) {
    *success = true;
    size_t closeBracket = bracketStart + 1;
    while(expression[closeBracket] != '\0' && expression[closeBracket] != ')') {
        closeBracket++;
    }

    //bracket end should be the end of the expression
    if(expression[closeBracket] != ')' || expression[closeBracket + 1] != '\0') {
        *success = false;
        return defaultFn();
    }

    //start parsing the function -> name is min 3 or 5 long
    if(bracketStart != 3 && bracketStart != 5) {
        *success = false;
        return defaultFn();
    }

    Function ans_funct;
    static char MIN_array[] = "MIN(";
    static char MAX_array[] = "MAX(";
    static char AVG_array[] = "AVG(";
    static char SUM_array[] = "SUM(";
    static char STDEV_array[] = "STDEV(";
    static char SLEEP_array[] = "SLEEP(";

    size_t itemStart = bracketStart + 1;
    size_t range_len = closeBracket - itemStart;
    if (bracketStart == 3) {
        //the length of the range string inside the brackets to be parsed

        //set the function type
        if(strncmp(expression, MIN_array, 4) == 0) {
            ans_funct.type = MIN_FUNCTION;
        } else if(strncmp(expression, MAX_array, 4) == 0) {
            ans_funct.type = MAX_FUNCTION;
        } else if(strncmp(expression, AVG_array, 4) == 0) {
            ans_funct.type = AVG_FUNCTION;
        } else if(strncmp(expression, SUM_array, 4) == 0) {
            ans_funct.type = SUM_FUNCTION;
        } else {
            *success = false;
            return defaultFn();
        }

        //all are range functions
        ans_funct.data.rangeFunctions = parseRange(&expression[itemStart], range_len, success);
    } else {
        if(strncmp(expression, STDEV_array, 6) == 0) {
            ans_funct.type = STDEV_FUNCTION;
            ans_funct.data.rangeFunctions = parseRange(&expression[itemStart], range_len, success);
        } else if(strncmp(expression, SLEEP_array, 6) == 0) {
            ans_funct.type = SLEEP_FUNCTION;
            ans_funct.data.sleep_value = parseOperand(&expression[itemStart], range_len, success);
        } else {
            *success = false;
            return defaultFn();
        }
    }

    return ans_funct;

}

Function parseExpression(char* expression, bool* success)
{
    if (expression == NULL || expression[0] == '\0')
    {
        *success = false;
        return defaultFn();
    }

  size_t nextStart = next_token(expression);
  if (expression[nextStart] == '(') { // A Function
      return parseFunction(expression, nextStart, success);
  }
  //binary operation or constant or identity function are left

  //This sign is a part of the integer
  if (expression[0] == '-' || expression[0] == '+') {
      nextStart += next_token(&expression[1]);
  }

  if (expression[nextStart] == '\0') {
      //an integer or a constant function
      Operand op = parseOperand(expression, nextStart, success);
      if (!*success) {
          return defaultFn();
      }
      Function fn;
      if (op.type == OPERAND_INT) {
          fn.type = CONSTANT;
          fn.data.value = op.data.value;
      } else {
          fn.type = PLUS_OP;
          fn.data.binaryOps.first.type = OPERAND_CELL;
          fn.data.binaryOps.first.data.cell = op.data.cell;
          fn.data.binaryOps.second.type = OPERAND_INT;
          fn.data.binaryOps.second.data.value = 0;
      }
      return fn;
  }

  //binary operation
    return parseBinaryOp(expression, &expression[nextStart], success);
}
