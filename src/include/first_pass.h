#ifndef FIRST_PASS_H
#define FIRST_PASS_H

#include <stdio.h>


#include "front_end.h"
#include "general.h"
#include "pre_assembler.h"

/* 
 * Function: insert_symbol
 * -----------------------
 * Description:
 *   Inserts a symbol into the symbol table.
 *
 * Parameters:
 *   symbol_table: The symbol table to insert the symbol into.
 *   symbol_name: The name of the symbol.
 *   symbol_type: The type of the symbol (instruction, data, constant, etc.).
 *   address: The address associated with the symbol.
 *   value: The value associated with the symbol.
 *
 * Returns:
 *   An integer representing the success or failure of the operation.
 */

int insert_symbol(SYMBOL *symbol_table[HASH_TABLE_SIZE], char *symbol_name, int symbol_type, int address, int value);

/* 
 * Function: memory_cell_calculator
 * ---------------------------------
 * Description:
 *   Calculates the number of memory cells needed for a line.
 *
 * Parameters:
 *   line: The line for which memory needs to be allocated.
 *
 * Returns:
 *   The number of memory cells required for the line.
 */

int memory_cell_calculator(LINE_AST line);

/* 
 * Function: first_pass
 * ---------------------
 * Description:
 *   Performs the first pass of the assembly process. It reads each line from the assembly file,
 *   parses it into an abstract syntax tree (AST), processes the AST to update memory counters,
 *   and builds the symbol table. If a syntax error is encountered during processing, it prints
 *   an error message and handles it appropriately.
 *
 * Parameters:
 *   curr_program: Pointer to the current translation unit being processed.
 *   am_file: Pointer to the assembly file.
 *   am_file_name: Name of the assembly file.
 *   macro_table: An array of pointers to MACRO structures representing macro definitions.
 *
 * Returns:
 *   An integer representing the success or failure of the operation.
 */

int first_pass(TRANSLATION_UNIT *curr_program, FILE *am_file, char *am_file_name, MACRO *macro_table[HASH_TABLE_SIZE]);
#endif

