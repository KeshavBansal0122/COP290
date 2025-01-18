//
// Created by paradocx on 1/18/25.

#pragma once
#include "utils.h"
#include "functions.h"

/**
 *
 * @param cell The text format like A1
 * @return The cell object
 */
Cell getCell(char* cell);

/**
 * Parses the expression, to which a Cell is set into a function object
 * @param expression The part after the = sign
 */
Function parseExpression(char* expression);