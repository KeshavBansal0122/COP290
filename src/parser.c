//
// Created by paradocx on 1/18/25.

#include "structs.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>



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

    if(expression[i]!='\0' && expression[i]!=')')
    {
        //printf("expression[i]: %c\n", expression[i]);
        return -1;
    }

    return result;
}

// Helper function to parse a cell reference
Cell parseCellReference(char* reference, bool* success) 
{
    Cell cell = {0, 0};  // Ensure initialization
    int i = 0;
    int length_of_col = 0;

    *success = true;  // Explicitly initialize success to true

    //printf("Parsing reference: %s\n", reference);

    // Parse column (letters)
    while (reference[i] >= 'A' && reference[i] <= 'Z') 
    {
        cell.col = cell.col * 26 + (reference[i] - 'A' + 1);  // Convert letters to column index
        i++;
        length_of_col++;

       // printf("Current column value: %d\n", cell.col);

        // Check if the column name is too long (more than 3 letters, e.g., "AAAA")
        if (length_of_col > 3) 
        {   
          //  printf("Error: Column length exceeded (more than 3 letters)\n");
            *success = false;
            return (Cell){0, 0};  // Return a valid default Cell
        }
    }

    // Ensure that at least one letter was parsed
    if (length_of_col == 0) 
    {
        //printf("Error: No column letters found\n");
        *success = false;
        return (Cell){0, 0};
    }

    // Parse row (numbers)
    if (reference[i] >= '0' && reference[i] <= '9') 
    {
        cell.row = convert_to_int(&reference[i]);  // Convert digits to row number
        
        //printf("Parsed row value: %d\n", cell.row);

        // Check if row conversion failed
        if (cell.row == -1) 
        {
           // printf("Error: Invalid row number\n");
            *success = false;
            return (Cell){0, 0};
        }
    }
    else 
    {
       // printf("Error: No row digits found\n");
        *success = false;
        return (Cell){0, 0};
    }

    // Validate row range
    if (cell.row >= 1000 || cell.row < 0) 
    {
       // printf("Error: Row out of valid range (0-999)\n");
        *success = false;
        return (Cell){0, 0};
    }

    //printf("Final parsed cell - Column: %d, Row: %d\n", cell.col, cell.row);
    
    return cell;
}

// Helper function to parse binary operation
BinaryOp parseBinaryOp(char* operand1, char* operand2, bool* success) 
{
    // Allocate memory and check for success
    BinaryOp binaryOp;

    *success = true;  // Start assuming success

    // Operand 1 processing
    int i = 0;
    if ((operand1[i] >= '0' && operand1[i] <= '9') || operand1[i] == '-' || operand1[i] == '+') 
    {
        (&binaryOp)->first.type = OPERAND_INT;
        int sign = 1;

        // Handle sign
        if (operand1[i] == '-') 
        {
            sign = -1;
            i++;
        }
        else if (operand1[i] == '+') 
        {
            i++;
        }

        // Convert string to integer
        int value = 0;
        while (operand1[i] >= '0' && operand1[i] <= '9') 
        {
            value = value * 10 + (operand1[i] - '0');
            i++;
        }

        // Ensure valid number conversion
        if (operand1[i] != '\0') 
        {
            *success = false;
            return binaryOp;
        }

        (&binaryOp)->first.data.value = sign * value;
    } 
    else 
    {  
        // Assume it's a cell reference
        (&binaryOp)->first.type = OPERAND_CELL;
        (&binaryOp)->first.data.cell = parseCellReference(operand1, success);

        if (!(*success)) 
        {
            return binaryOp;
        }
    }

    // Operand 2 processing
    i = 0;
    if ((operand2[i] >= '0' && operand2[i] <= '9') || operand2[i] == '-' || operand2[i] == '+') 
    {
        (&binaryOp)->second.type = OPERAND_INT;
        int sign = 1;

        if (operand2[i] == '-') 
        {
            sign = -1;
            i++;
        }
        else if (operand2[i] == '+') 
        {
            i++;
        }

        int value = 0;
        while (operand2[i] >= '0' && operand2[i] <= '9') 
        {
            value = value * 10 + (operand2[i] - '0');
            i++;
        }

        if (operand2[i] != '\0') 
        {
            *success = false;
            return binaryOp;
        }

        (&binaryOp)->second.data.value = sign * value;
    } 
    else 
    {  
        // Assume it's a cell reference
        (&binaryOp)->second.type = OPERAND_CELL;
        (&binaryOp)->second.data.cell = parseCellReference(operand2, success);

        if (!(*success)) 
        {
            return binaryOp;
        }
    }

    return binaryOp;
}

bool hasExtraCharsAfterClosingParen(const char *expression) {
    int i = 0;

    // Find the first ')' character
    while (expression[i] != '\0') {
        if (expression[i] == ')') {
            i++; // Move past ')'
            break;
        }
        i++;
    }

    // Check if there are any extra characters after the closing parenthesis
    if(expression[i+1] != '\0') {
        //printf("Extra characters found after closing parenthesis: %s\n", expression + i);
        return true;
    }

    return false; // No extra characters found
}

bool validateFormatAndExtractRange(const char *expression, Cell *topLeft, Cell *bottomRight, bool *success) {
   // printf("Validating expression: %s\n", expression);

    int length = strlen(expression);
    //printf("Expression length: %d\n", length);

    // Ensure the expression ends with ')' and has a minimum length
    if (expression[length - 1] != ')') {
        //printf("Error: Expression does not end with ')'\n");
        return false;
    }

    const char *start_pointer = expression + 4; // Skip "MIN(", "MAX(", etc.
    if (*start_pointer == '\0') { // Safety check
        //printf("Error: Empty reference after function name\n");
        return false;
    }

    //printf("Start parsing from: %s\n", start_pointer);

    int i = 0;
    const char *bottom_right_pointer = NULL;

    // Find the ':' separator
    while (start_pointer[i] != ')' && start_pointer[i] != '\0') {
        if (start_pointer[i] == ':') {
            bottom_right_pointer = start_pointer + i + 1;
            //printf("':' found at position %d, bottom-right starts at: %s\n", i, bottom_right_pointer);
            break;
        }
        i++;
    }

   *success = !(hasExtraCharsAfterClosingParen(expression));

    // If no ':' found or empty bottom-right reference, invalid format
    if (!bottom_right_pointer || *bottom_right_pointer == ')' || *bottom_right_pointer == '\0') {
        //printf("Error: Invalid range format, missing ':' or empty bottom-right reference\n");
        return false;
    }

    // Ensure top-left reference fits safely in the buffer
    if (i >= sizeof(char) * 31) { // 31 characters max, 1 for '\0'
       // printf("Error: Top-left reference exceeds max length\n");
        return false;
    }

    char top_left_reference[32]; 
    strncpy(top_left_reference, start_pointer, i);
    top_left_reference[i] = '\0'; // Ensure null termination
    //printf("Extracted top-left reference: %s\n", top_left_reference);

    size_t br_length = strlen(bottom_right_pointer);
    if (br_length >= 31) { // Avoid buffer overflow
        //printf("Error: Bottom-right reference exceeds max length\n");
        return false;
    }

    char bottom_right_reference[32];
    strncpy(bottom_right_reference, bottom_right_pointer, sizeof(bottom_right_reference) - 1);
    bottom_right_reference[sizeof(bottom_right_reference) - 1] = '\0'; // Ensure null termination
   // printf("Extracted bottom-right reference: %s\n", bottom_right_reference);

    // Parse the cell references
    *topLeft = parseCellReference(top_left_reference, success);
    //printf("Parsed topLeft: col=%d, row=%d (success=%d)\n", topLeft->col, topLeft->row, *success);

    if (!*success) {
       // printf("Error: Failed to parse top-left reference\n");
        return false; // Stop if top-left parsing fails
    }

    *bottomRight = parseCellReference(bottom_right_reference, success);
    //printf("Parsed bottomRight: col=%d, row=%d (success=%d)\n", bottomRight->col, bottomRight->row, *success);

    if (!*success) {
        //printf("Error: Failed to parse bottom-right reference\n");
        return false; // Stop if bottom-right parsing fails
    }

    //printf("Validation successful!\n");
    return true;
}



Function parseExpression(char* expression, bool* success)
{
  //split into two parts either it is a parenthesis function or not 
  //read first 4 chars check if it matches given template of functions if yes then it is a function else handle it separately
  Function ans_funct ;

  char funct_checker_temp_arr[4];
  char MIN_array[5] = "MIN(";
  char MAX_array[5] = "MAX(";
  char AVG_array[5] = "AVG(";
  char SUM_array[5] = "SUM(";
  char STDEV_array[5] = "STDE" ;
  char SLEEP_array[5] = "SLEE";
  
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
        
        Cell topLeft ;
        Cell bottomRight ;
    
        if (!validateFormatAndExtractRange(expression, &topLeft, &bottomRight, success)) {
            return ans_funct;
        }
    
        ans_funct.data.rangeFunctions.topLeft = topLeft;
        ans_funct.data.rangeFunctions.bottomRight = bottomRight;
        *success = true;
        return ans_funct;

    } 

    else if (strncmp(funct_checker_temp_arr, MAX_array, 4) == 0) 
    {
        
        ans_funct.type = MAX_FUNCTION;
        *success = true;
        
        Cell topLeft, bottomRight;
        
        if (!validateFormatAndExtractRange(expression, &topLeft, &bottomRight, success)) {
            *success = false;
            return ans_funct;
        }

        //debug
        //printf("topLeft: %d %d\n", topLeft.col, topLeft.row);
        //printf("bottomRight: %d %d\n", bottomRight.col, bottomRight.row);

        ans_funct.data.rangeFunctions.topLeft = topLeft;
        ans_funct.data.rangeFunctions.bottomRight = bottomRight;
        *success = true;
        return ans_funct;
    } 

    else if (strncmp(funct_checker_temp_arr, AVG_array, 4) == 0) 
    {
        ans_funct.type = AVG_FUNCTION;
        
        Cell topLeft, bottomRight;
        
        if (!validateFormatAndExtractRange(expression, &topLeft, &bottomRight, success)) {
            *success = false;
            return ans_funct;
        }

        ans_funct.data.rangeFunctions.topLeft = topLeft;
        ans_funct.data.rangeFunctions.bottomRight = bottomRight;
        *success = true;
        return ans_funct;
    } 
    else if (strncmp(funct_checker_temp_arr, SUM_array, 4) == 0) 
    {
        ans_funct.type = SUM_FUNCTION;
        
        Cell topLeft, bottomRight;
        
        if (!validateFormatAndExtractRange(expression, &topLeft, &bottomRight, success)) {
            *success = false;
            return ans_funct;
        }

        ans_funct.data.rangeFunctions.topLeft = topLeft;
        ans_funct.data.rangeFunctions.bottomRight = bottomRight;
        *success = true;
        return ans_funct;
    } 
    else if (strncmp(funct_checker_temp_arr, STDEV_array, 4) == 0) 
    {
        int length = strlen(expression);
        if (expression[length - 1] != ')' || expression[4]!='V' || expression[5]!='(') 
        {   
            *success = false;
            return ans_funct;
        }
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

        if(start_pointer[i+1] != '\0')
        {
            *success = false;
            return ans_funct;
        }

        if (bottom_right_pointer) 
        {
            char top_left_reference[i + 1];
            strncpy(top_left_reference, start_pointer, i);
            top_left_reference[i] = '\0';

            char *bottom_right_reference = bottom_right_pointer;
            topLeft = parseCellReference(top_left_reference, success);
            bottomRight = parseCellReference(bottom_right_reference, success);

            ans_funct.data.rangeFunctions.topLeft = topLeft;
            ans_funct.data.rangeFunctions.bottomRight = bottomRight;
        }
    } 
    else if (strncmp(funct_checker_temp_arr, SLEEP_array, 4) == 0) 
    {
        if (expression[length - 1] != ')' || expression[4]!='V' || expression[5]!='(') 
        {   
            *success = false;
            return ans_funct;
        }

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
        
        for (int i = 1; i < length; i++) 
        {
            if (expression[i] == '+' || expression[i] == '-' || 
                expression[i] == '*' || expression[i] == '/') 
            {
                operand_address = expression + i + 1;  // Points to the part of the string after the operator
                isBinaryOp = true;
                switch (expression[i])
                {
                case '+':
                    ans_funct.type = PLUS_OP;
                    break;
                
                case '-':   
                    ans_funct.type = MINUS_OP;
                    break;

                case '*':
                    ans_funct.type = MULTIPLY_OP;
                    break;  

                case '/':       
                    ans_funct.type = DIVIDE_OP;
                    break;  

                default:
                    break;
                }

                pos = i; 
                break;
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
            
            //first check if succcess is true then only assign otherwise error 
            parseBinaryOp(operand1, operand2, success);
            if(!(*success))
            {
                return ans_funct;
            }
            ans_funct.data.binaryOps = parseBinaryOp(operand1, operand2, success);
        }

        else //not a binaryop means that it could either be a constant or a cell reference
        {
            if(expression[0]-'0'<=9 && expression[0]-'0'>=0) // first char is a number implies that it is a constant
            {
                ans_funct.type = CONSTANT;
                ans_funct.data.value = convert_to_int(expression); 
                if(ans_funct.data.value == -1)
                {
                    *success = false;
                    return ans_funct;
                }
            }

            else // parse cell reference
            {
                ans_funct.type = SUM_FUNCTION;
                char temp[2] = "0";
                parseBinaryOp(expression, temp, success);
                if(!(*success))
                {
                    return ans_funct;
                }
                ans_funct.data.binaryOps = parseBinaryOp(expression, temp, success);
                
            }
        }
    }  

  }


  else  // Expression is less than 4 characters long
{
    char* operand_address;
    bool isBinaryOp = false;
    int pos = 0;  // Position of operator in the expression

    for (int i = 1; i < length; i++) 
    {
        if (expression[i] == '+' || expression[i] == '-' || 
            expression[i] == '*' || expression[i] == '/') 
        {
            operand_address = expression + i + 1;  // Points to the part of the string after the operator
            isBinaryOp = true;
            switch (expression[i])
                {
                case '+':
                    ans_funct.type = PLUS_OP;
                    /* code */
                    break;
                
                case '-':   
                    ans_funct.type = MINUS_OP;
                    break;

                case '*':
                    ans_funct.type = MULTIPLY_OP;
                    break;  

                case '/':       
                    ans_funct.type = DIVIDE_OP;
                    break;  

                default:
                    break;
                }
            pos = i; 
            break;
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

        parseBinaryOp(operand1, operand2, success);
        if(!(*success))
        {
            return ans_funct;
        }
        ans_funct.data.binaryOps = parseBinaryOp(operand1, operand2, success);
    }
    else 
    {
        if (expression[0] >= '0' && expression[0] <= '9')  // If the first character is a number its a constant
        {
            ans_funct.type = CONSTANT;
            ans_funct.data.value = convert_to_int(expression);  // Convert string to integer
            if(ans_funct.data.value == -1)
            {
                *success = false;
                return ans_funct;
            }
        }
        else  // If it's not a number its a cell reference 
        {
            ans_funct.type = SUM_FUNCTION;  // Default to SUM_FUNCTION
            char temp[2] = "0";
            parseBinaryOp(expression, temp, success);
            if(!(*success))
            {
                return ans_funct;
            }
            ans_funct.data.binaryOps = parseBinaryOp(expression, temp, success); 
        }
    }
  }

  
  return ans_funct;
    
}

