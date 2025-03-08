#ifndef SECOND_PASS_H
#define SECOND_PASS_H

#include <stdio.h>


#include "front_end.h"
#include "general.h"
#include "pre_assembler.h"
#include "first_pass.h"

/*
 * Function: ext_search
 * ---------------------
 * Description:
 *   This function searches for a given symbol name in the external symbol list.
 *
 * Parameters:
 *   - ext_list: Pointer to the head of the external symbol list.
 *   - symbol_name: Name of the symbol to search for.
 *
 * Returns:
 *   - Pointer to the symbol if found, otherwise NULL.
 */

EXT_SYMBOL * ext_search(EXT_SYMBOL * ext_list, char * symbol_name);


/*
 * Function: add_ext_address
 * --------------------------
 * Description:
 *   This function adds an external address to the external symbol list or updates an existing symbol with a new address.
 *
 * Parameters:
 *   - ext_list: Pointer to the pointer to the head of the external symbol list.
 *   - symbol_name: Name of the symbol.
 *   - symbol_address: Address of the symbol.
 *
 * Returns:
 *   - NO_ERROS if successful, MEMORY_ALLOCATION_ERROR if memory allocation fails.
 */

int add_ext_address(EXT_SYMBOL ** ext_list, char *symbol_name, int symbol_address);

/*
* Function: second_pass
 * ----------------------
 * Description:
 * Creates the binary coding for each line of instructions or directive (if the directive is data or string) and stores it in the array of instructions/data.
 * Creates a list of all external symbols.
 *
 * Parameters:
 *   - curr_program: Pointer to the translation unit hat contains all the information about the file (symbol table, instruction array and ect...).
 *   - am_file_name: Name of the assembly file being processed.
 *   - am_file: Pointer to the file containing the assembly code.
 *
 * Returns:
 *   - NO_ERRORS if successful, MEMORY_ALLOCATION_ERROR if memory allocation fails, SYNTAX_OR_LOGIC_ERROR if there are logical errors.
 */

int second_pass(TRANSLATION_UNIT *curr_program, char *am_file_name, FILE *am_file);




#endif
