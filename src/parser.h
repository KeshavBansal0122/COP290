//
// Created by paradocx on 1/18/25.

#pragma once
#include "structs.h"



/**
 *
 * @param cell The text format like A1
 * @return The cell object
 */
Cell getCell(char* cell);

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
Function parseExpression(char* expression, bool* success);