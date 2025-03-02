//
// Created by paradocx on 1/18/25.

#pragma once

#include "structs.h"

/**
 * Returns the interval for the next token in the string. A token is defined as the next set of
 * alphanumeric characters, or a single special non-space character
 * @param expression The expression to tokenize
 * @param token_end out parameter for end of the next token, exclusive.
 * If this is the same as the token_start, then the does not contain more tokens
 *
 * @param next_token out parameter for the start of the next token,
 * or the end of the string if there are no more tokens
 * @return The offset to the end of the current token(exclusive). 0 if the string is empty
 * */
size_t next_token(const char *expression);


/**
 * Converts the string to cell object with a 0 based indexing.
 * A1 -> 0,0
 * AA35 -> 26, 34
 * */
Cell parseCellReference(const char* reference, size_t len, bool* success);


/**
 * Will return a parsing error for a cell if the cell is out of bounds
 * */
void parserSetSize(int row, int col);

//helper functions for parseexpression 
int convert_to_int(const char* expression, size_t len, bool* success);


/**
 * Parses the expression, to which a Cell is set into a function object
 * @param expression The part after the = sign
 * @param success Whether the parsing was successful
 */
Function parseExpression(char* expression, bool* success);
