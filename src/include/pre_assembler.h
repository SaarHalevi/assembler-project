
#ifndef PRE_ASSEMBLER_H
#define PRE_ASSEMBLER_H

#include "general.h"

typedef struct TEXT TEXT;
typedef struct MACRO MACRO;

/*
 * Function: macro_lookup
 * -----------------------
 * Looks up a macro in the hash table by its name.
 *
 * Parameters:
 *   name: The name of the macro to lookup.
 *   macro_table: The hash table containing macro definitions.
 *
 * Returns:
 *   A pointer to the found macro if it exists, otherwise NULL.
 */

MACRO *macro_lookup(char *name, MACRO *macro_table[HASH_TABLE_SIZE]);

/*
 * Function: insert_macro
 * -----------------------
 * Inserts a new macro into the hash table.
 *
 * Parameters:
 *   macro_name: The name of the macro to insert.
 *   macro_table: The hash table containing macro definitions.
 *   file_name: The name of the source file.
 *
 * Returns:
 *   A pointer to the inserted macro.
 */

MACRO *insert_macro(char *macro_name, MACRO *macro_table[HASH_TABLE_SIZE], char *file_name);

/*
 * Function: text_insert
 * ----------------------
 * Inserts a line of text into a macro's content.
 *
 * Parameters:
 *   macro: The macro to insert the text into.
 *   line: The line of text to insert.
 *   file_name: The name of the source file.
 *   line_num: The line number being processed.
 *
 * Returns:
 *   NO_ERRORS if successful, MEMORY_ALLOCATION_ERROR if memory allocation fails.
 */

int text_insert(MACRO *macro, char *line, char *file_name, int line_num);

/*
 * Function: line_identifier
 * --------------------------
 * Identifies the type of line in the assembly file (e.g., macro definition, macro call, etc.).
 *
 * Parameters:
 *   line: The line of text being processed.
 *   mcr_ptr: Pointer to the current macro (if any).
 *   macro_table: The hash table containing macro definitions.
 *   file_name: The name of the source file.
 *   line_num: The line number being processed.
 *
 * Returns:
 *   The type of line identified (e.g., macro_definition, macro_call, etc.).
 */

int line_identifier(char *line, MACRO **mcr_ptr, MACRO *macro_table[HASH_TABLE_SIZE], char *file_name, int line_num);

/*
 * Function: pre_assembly
 * -----------------------
 * Performs preprocessing on the assembly file, expanding macros.
 *
 * Parameters:
 *   file_name: The name of the assembly file.
 *   macro_table: The hash table containing macro definitions.
 *
 * Returns:
 *   A pointer to the name of the processed assembly file.
 */

char *pre_assembly(char *file_name, MACRO *macro_table[HASH_TABLE_SIZE]);

/*
 * Function: free_macro_content
 * -----------------------------
 * Frees the memory allocated for a macro's content (lines of text).
 *
 * Parameters:
 *   content: The content of the macro to free.
 */

void free_macro_content(TEXT *content);

/*
 * Function: free_macro_table
 * ---------------------------
 * Frees the memory allocated for the macro hash table and its contents.
 *
 * Parameters:
 *   macro_table: The hash table containing macro definitions.
 */

void free_macro_table(MACRO *macro_table[HASH_TABLE_SIZE]);

#endif
