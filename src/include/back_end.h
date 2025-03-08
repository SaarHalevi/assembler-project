#ifndef BACK_END_H
#define BACK_END_H

#include <stdio.h>

#include "front_end.h"
#include "general.h"
#include "pre_assembler.h"
#include "first_pass.h"
#include "second_pass.h"

/*
 * Function: create_object_file
 * ----------------------------
 * Description:
 *   -This function creates an object file containing the entire memory image of a certain program.
 *   -Each line of the file contains the address of the word in memory and the content of the word, where the content is written in encrypted base 4.
 *   -The object file structure starts with the header, indicating the instruction counter (ic) and the data counter (dc), followed by the encoded instructions and data.
 *
 * Parameters:
 *   - curr_program: Pointer to the central processing unit that contains all the relevant information about the current program.
 *   - file_name: The file name without the .ob extension.
 *
 * Returns:
 *   - MEMORY_ALLOCATION_ERROR if memory allocation or file opening failed, otherwise returns NO_ERRORS.
 */

int create_object_file(TRANSLATION_UNIT *curr_program, char *file_name);

/*
 * Function: create_entries_file
 * ------------------------------
 * Description:
 *   This function creates a file containing all the symbols defined as entries in the program and the addresses where they were defined.
 *
 * Parameters:
 *   - curr_program: Pointer to the central processing unit that contains all the relevant information about the current program.
 *   - file_name: The file name without the .ent extension.
 *
 * Returns:
 *   - MEMORY_ALLOCATION_ERROR if memory allocation or file opening failed, otherwise returns NO_ERRORS.
 */

int create_entries_file(TRANSLATION_UNIT *curr_program, char *file_name);

/*
 * Function: create_external_file
 * ------------------------------
 * Description:
 *   -The function generates a file containing information about external symbols used in the program.
 *   -Each line in the file corresponds to an external symbol and its address in the machine code where it is referenced as an operand.
 *   -If a symbol has multiple references, each reference is listed on a separate line.
 *
 * Parameters:
 *   - curr_program: A pointer to the translation unit representing the current program, containing relevant program information.
 *   - file_name: The name of the output file without the .ext extension.
 *
 * Returns:
 *   - MEMORY_ALLOCATION_ERROR if memory allocation for file name concatenation fails or if file opening fails.
 *   - NO_ERRORS if the external file is successfully created.
 */

int create_external_file(TRANSLATION_UNIT *curr_program, char *file_name);

#endif
