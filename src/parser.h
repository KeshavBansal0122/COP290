//
// Created by paradocx on 1/18/25.

#pragma once
#include "structs.h"
#include <strings.h>



/**
 *
 * @param cell The text format like A1
 * @return The cell object
 */
Cell getCell(const char* cell);

/**
 * @param cell
 * @return Text format of the cell like A1
 */
char* getCellString(Cell cell);
/**
 *
 * @param cell
 * @return
 */
char* getCellString(Cell cell);
/**
 * Parses the expression, to which a Cell is set into a function object
 * @param expression The part after the = sign
 * @param success Whether the parsing was successful
 */

//helper functions for parseexpression 
int convert_to_int(char* expression) 
{
    int result = 0;
    int i = 0;

    while (expression[i] >= '0' && expression[i] <= '9') 
    {
        result = result * 10 + (expression[i] - '0');
        i++;
    }

    return result;
}

// Helper function to parse a cell reference
Cell parseCellReference(char* reference) 
{
    Cell cell = {0, 0};
    int i = 0;

    // Parse column (letters)
    while (reference[i] >= 'A' && reference[i] <= 'Z') 
    {
        cell.col = cell.col * 26 + (reference[i] - 'A' + 1);  // Convert letters to column index
        i++;
    }

    // Parse row (numbers)
    if (reference[i] >= '0' && reference[i] <= '9') 
    {
        cell.row = convert_to_int(&reference[i]);  // Convert digits to row number
    }

    return cell;
}

// Helper function to parse binary operation
BinaryOp* parseBinaryOp(char* operand1, char* operand2) 
{
    BinaryOp* binaryOp ; 

    // Operand 1 processing
    int i = 0;
    if (operand1[i] >= '0' && operand1[i] <= '9') 
    {  // Check if it's an integer
        binaryOp->first.type = OPERAND_INT;
        int value = 0;
        while (operand1[i] >= '0' && operand1[i] <= '9') 
        {
            value = value * 10 + (operand1[i] - '0');
            i++;
        }
        binaryOp->first.data.value = value;
    } 
    else 
    {  // Assume it's a cell reference
        binaryOp->first.type = OPERAND_CELL;
        binaryOp->first.data.cell = parseCellReference(operand1);  
    }

    // Operand 2 processing
    i = 0;
    if (operand2[i] >= '0' && operand2[i] <= '9') 
    {  // Check if it's an integer
        binaryOp->second.type = OPERAND_INT;
        int value = 0;
        while (operand2[i] >= '0' && operand2[i] <= '9') 
        {  
            value = value * 10 + (operand2[i] - '0');  
            i++;
        }
        binaryOp->second.data.value = value;
    } 
    else 
    {  // Assume it's a cell reference
        binaryOp->second.type = OPERAND_CELL;
        binaryOp->second.data.cell = parseCellReference(operand2);  
    }

    return binaryOp;
}





Function parseExpression(char* expression, bool* success)
{
  //split into two parts either it is a parenthesis function or not 
  //read first 4 chars check if it matches given template of functions if yes then it is a function else handle it separately
  Function* ans_funct_add ;
  Function ans_funct = *ans_funct_add;

  char funct_checker_temp_arr[4];
  char MIN_array[4] = "MIN(";
  char MAX_array[4] = "MAX(";
  char AVG_array[4] = "AVG(";
  char SUM_array[4] = "SUM(";
  char STDEV_array[4] = "STDE" ;
  char SLEEP_array[4] = "SLEE";
  
  //check whether it is possible to be a parenthesis function (>=4 is the size)
  int length = 0;
  while (expression[length] != '\0') 
  {
      length++;
      if (length >= 4) 
      {
          break;
      }
  }
  
  //length >=4 implies that it can be a parenthesis function
  if (length >= 4) 
  {
    for(int i=0;i<4;i++)
    {
        funct_checker_temp_arr[i]= expression[i];

    }
  
    if (strncmp(funct_checker_temp_arr, MIN_array, 4) == 0) 
    {
        ans_funct.type = MIN_FUNCTION;
        char *start_pointer = expression + 4;
        int i = 0;
        char *bottom_right_pointer = NULL;
        Cell topLeft, bottomRight;

        while (start_pointer[i] != ')') 
        {
            if (start_pointer[i] == ':') 
            {
                bottom_right_pointer = start_pointer + i + 1;
            }
            i++;
        }

        if (bottom_right_pointer) 
        {
            char top_left_reference[i + 1];
            strncpy(top_left_reference, start_pointer, i);
            top_left_reference[i] = '\0';

            char *bottom_right_reference = bottom_right_pointer;
            topLeft = parseCellReference(top_left_reference);
            bottomRight = parseCellReference(bottom_right_reference);

            ans_funct.data.rangeFunctions.topLeft = topLeft;
            ans_funct.data.rangeFunctions.bottomRight = bottomRight;
        }


    } 
    else if (strncmp(funct_checker_temp_arr, MAX_array, 4) == 0) 
    {
        ans_funct.type = MAX_FUNCTION;
        char *start_pointer = expression + 4;
        int i = 0;
        char *bottom_right_pointer = NULL;
        Cell topLeft, bottomRight;

        while (start_pointer[i] != ')') 
        {
            if (start_pointer[i] == ':') 
            {
                bottom_right_pointer = start_pointer + i + 1;
            }
            i++;
        }

        if (bottom_right_pointer) 
        {
            char top_left_reference[i + 1];
            strncpy(top_left_reference, start_pointer, i);
            top_left_reference[i] = '\0';

            char *bottom_right_reference = bottom_right_pointer;
            topLeft = parseCellReference(top_left_reference);
            bottomRight = parseCellReference(bottom_right_reference);

            ans_funct.data.rangeFunctions.topLeft = topLeft;
            ans_funct.data.rangeFunctions.bottomRight = bottomRight;
        }
    } 
    else if (strncmp(funct_checker_temp_arr, AVG_array, 4) == 0) 
    {
        ans_funct.type = AVG_FUNCTION;
        char *start_pointer = expression + 4;
        int i = 0;
        char *bottom_right_pointer = NULL;
        Cell topLeft, bottomRight;

        while (start_pointer[i] != ')') 
        {
            if (start_pointer[i] == ':') 
            {
                bottom_right_pointer = start_pointer + i + 1;
            }
            i++;
        }

        if (bottom_right_pointer) 
        {
            char top_left_reference[i + 1];
            strncpy(top_left_reference, start_pointer, i);
            top_left_reference[i] = '\0';

            char *bottom_right_reference = bottom_right_pointer;
            topLeft = parseCellReference(top_left_reference);
            bottomRight = parseCellReference(bottom_right_reference);

            ans_funct.data.rangeFunctions.topLeft = topLeft;
            ans_funct.data.rangeFunctions.bottomRight = bottomRight;
        }
    } 
    else if (strncmp(funct_checker_temp_arr, SUM_array, 4) == 0) 
    {
        ans_funct.type = SUM_FUNCTION;
        char *start_pointer = expression + 4;
        int i = 0;
        char *bottom_right_pointer = NULL;
        Cell topLeft, bottomRight;

        while (start_pointer[i] != ')') 
        {
            if (start_pointer[i] == ':') 
            {
                bottom_right_pointer = start_pointer + i + 1;
            }
            i++;
        }

        if (bottom_right_pointer) 
        {
            char top_left_reference[i + 1];
            strncpy(top_left_reference, start_pointer, i);
            top_left_reference[i] = '\0';

            char *bottom_right_reference = bottom_right_pointer;
            topLeft = parseCellReference(top_left_reference);
            bottomRight = parseCellReference(bottom_right_reference);

            ans_funct.data.rangeFunctions.topLeft = topLeft;
            ans_funct.data.rangeFunctions.bottomRight = bottomRight;
        }
    } 
    else if (strncmp(funct_checker_temp_arr, STDEV_array, 4) == 0) 
    {
        ans_funct.type = STDEV_FUNCTION;
        char *start_pointer = expression + 6;
        int i = 0;
        char *bottom_right_pointer = NULL;
        Cell topLeft, bottomRight;

        while (start_pointer[i] != ')') 
        {
            if (start_pointer[i] == ':') 
            {
                bottom_right_pointer = start_pointer + i + 1;
            }
            i++;
        }

        if (bottom_right_pointer) 
        {
            char top_left_reference[i + 1];
            strncpy(top_left_reference, start_pointer, i);
            top_left_reference[i] = '\0';

            char *bottom_right_reference = bottom_right_pointer;
            topLeft = parseCellReference(top_left_reference);
            bottomRight = parseCellReference(bottom_right_reference);

            ans_funct.data.rangeFunctions.topLeft = topLeft;
            ans_funct.data.rangeFunctions.bottomRight = bottomRight;
        }
    } 
    else if (strncmp(funct_checker_temp_arr, SLEEP_array, 4) == 0) 
    {
        ans_funct.type = SLEEP_FUNCTION;
        int val = 0; 
        int i = 6;

        while(expression[i]<= '9'&& expression[i]>='0')
        {
            val = 10*val + expression[i]-'0';
        }
        ans_funct.data.value = val;
    }

    //above this line is cases for parenthesis functions, below this for other types

    else 
    {   
        char* operand_address;
        bool isBinaryOp = false;
        int pos = 0;
        for (int i = 0; i < length; i++) 
        {
            if (expression[i] == '+' || expression[i] == '-' || expression[i] == '*' || expression[i] == '/') 
            {
                operand_address = expression + i + 1;  // Points to the part of the string after the operator
                isBinaryOp = true;  
                pos = i;
            }
        }

        if (isBinaryOp) //this is for binary operations 
        {
            // Create operand1 (before the operator)
            char operand1[pos + 1]; 
            strncpy(operand1, expression, pos);  // Copy the part before the operator
            operand1[pos] = '\0';  // Null-terminate operand1

            // Create operand2 (after the operator)
            char operand2[length - (operand_address - expression) + 1];  // Allocate space for operand2 (+1 for null terminator)
            strcpy(operand2, operand_address);  // Copy the part after the operator

            ans_funct.data.binaryOps = *parseBinaryOp(operand1, operand2);  
        }



        else //not a binaryop means that it could either be a constant or a cell reference
        {
            if(expression[0]-'0'<=9 && expression[0]-'0'>=0) // first char is a number implies that it is a constant
            {
                ans_funct.type = CONSTANT;
                ans_funct.data.value = convert_to_int(expression); 
            }

            else // parse cell reference
            {
                ans_funct.type = SUM_FUNCTION;
                char* temp = "0";
                ans_funct.data.binaryOps = *parseBinaryOp(expression, temp);
                
            }
        }
    }  

  }


  else  // Expression is less than 4 characters long
{
    char* operand_address;
    bool isBinaryOp = false;
    int pos = 0;  // Position of operator in the expression

    for (int i = 0; i < length; i++) 
    {
        if (expression[i] == '+' || expression[i] == '-' || 
            expression[i] == '*' || expression[i] == '/') 
        {
            operand_address = expression + i + 1;  // Points to the part of the string after the operator
            isBinaryOp = true;
            pos = i; 
        }
    }

    if (isBinaryOp)  // If it is a binary operation
    {
        // Create operand1 
        char operand1[pos + 1]; 
        strncpy(operand1, expression, pos); 
        operand1[pos] = '\0';  // Null-terminate operand1

        // Create operand2 (after the operator)
        char operand2[length - (operand_address - expression) + 1];  // Allocate space for operand2
        strcpy(operand2, operand_address);  // Copy the part after the operator

        ans_funct.data.binaryOps = *parseBinaryOp(operand1, operand2);
    }
    else 
    {
        if (expression[0] >= '0' && expression[0] <= '9')  // If the first character is a number its a constant
        {
            ans_funct.type = CONSTANT;
            ans_funct.data.value = convert_to_int(expression);  // Convert string to integer
        }
        else  // If it's not a number its a cell reference 
        {
            ans_funct.type = SUM_FUNCTION;  // Default to SUM_FUNCTION
            char* temp = "0";
            ans_funct.data.binaryOps = *parseBinaryOp(expression, temp); 
        }
    }
}

  
  return ans_funct;
    
}
