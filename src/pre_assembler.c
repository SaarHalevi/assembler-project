#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "include/pre_assembler.h"

struct TEXT
{
    char *text;
    struct TEXT *next;
};

struct MACRO
{
    char *name;
    TEXT *content;
    struct MACRO *next;
};

enum
{
    error,
    macro_definition,
    end_macro_definition,
    macro_call,
    note_sentence,
    any_other_line
} line_type;

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
 *
 * Algorithm:
 *   1. Calculate the hash index for the given macro name.
 *   2. Traverse the linked list at the calculated index to find the macro.
 */

MACRO *macro_lookup(char *name, MACRO *macro_table[HASH_TABLE_SIZE])
{
    unsigned int index = hash(name);
    MACRO *current = macro_table[index];
    while (current != NULL)
    {
        if (strcmp(name, current->name) == 0)
        {
            return current;
        }

        current = current->next;
    }
    return current;
}

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
 *   A pointer to the macro if created successfully, otherwise returns NULL.
 *
 * Algorithm:
 *   1. Calculate the hash index for the given macro name.
 *   2. Allocate memory for the new macro.
 *   3. Insert the macro into the hash table at the calculated index.
 */

MACRO *insert_macro(char *macro_name, MACRO *macro_table[HASH_TABLE_SIZE], char *file_name)
{
    unsigned int index = hash(macro_name);
    MACRO *new_macro = (MACRO *)malloc(sizeof(MACRO));

    if (new_macro == NULL)
    {
        printf("Error in file : %s , memory allocation failed\n", file_name);
        return NULL;
    }

    new_macro->name = (char *)malloc((strlen(macro_name) + 1) * sizeof(char));
    if (new_macro->name == NULL)
    {
        printf("Error in file : %s , memory allocation failed\n", file_name);
        free(new_macro);
        return NULL;
    }
    strcpy(new_macro->name, macro_name);

    new_macro->content = NULL;
    new_macro->next = NULL;

    if (macro_table[index] == NULL)
    {
        macro_table[index] = new_macro;
        return new_macro;
    }

    else
    {
        new_macro->next = macro_table[index];
        macro_table[index] = new_macro;
        return new_macro;
    }
}

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
 *
 * Algorithm:
 *   1. Allocate memory for the new TEXT node.
 *   2. Copy the line of text into the TEXT node.
 *   3. Insert the TEXT node into end of the content linked list of the specified macro.
 */

int text_insert(MACRO *macro, char *line, char *file_name, int line_num)
{
    TEXT *content_ptr = NULL;
    TEXT *added_line = (TEXT *)malloc(sizeof(TEXT));
    if (added_line == NULL)
    {
        printf("Error in file : %s , memory allocation failed\n", file_name);
        return MEMORY_ALLOCATION_ERROR;
    }

    added_line->text = (char *)malloc((strlen(line) + 1) * sizeof(char));
    if (added_line->text == NULL)
    {
        printf("Error in file : %s , memory allocation failed \n", file_name);
        free(added_line);
        return MEMORY_ALLOCATION_ERROR;
    }

    else
    {
        strcpy(added_line->text, line);
        added_line->next = NULL;
    }

    if (macro->content == NULL)
    {
        macro->content = added_line;
    }

    else
    {
        content_ptr = macro->content;
        while (content_ptr->next != NULL)
        {
            content_ptr = content_ptr->next;
        }

        content_ptr->next = added_line;
    }

    return NO_ERRORS;
}

/*
 * Function: line_identifier
 * --------------------------
 * Identifies the type of line in the assembly file (error, macro definition, end of macro definition, macro call, any other line).
 *
 * Parameters:
 *   line: The line of text being processed.
 *   mcr_ptr: Pointer to the current macro (if any).
 *   macro_table: The hash table containing macro definitions.
 *   file_name: The name of the source file.
 *   line_num: The line number being processed.
 *
 * Returns:
 *   The type of line identified (e.g., macro_definition, macro_call, etc).
 *
 * Algorithm:

 * 1. Tokenization: The input line is tokenized to extract individual words.
 *
 * 2. Keyword Detection:
 *    - If the word "mcr" is encountered, it indicates the start of a macro definition.
 *    - If the word "endmcr" is encountered, it indicates the end of a macro definition.
 *    - If a word matches an existing macro name in the macro table, it indicates a macro call.
 *    - If a semicolon ';' is found at the beginning of the line, it indicates a comment or note.
 *
 * 3. Line Type Determination:
 *    - Based on the presence of keywords and specific conditions, the function determines the type of line.
 *
 * 4. Validation and Error Handling:
 *    - Various checks and validations are performed to ensure the correctness of the line structure and syntax.
 *    - Errors are reported if macro definitions are not properly formatted, macros have duplicate names,
 *      or reserved keywords are used.
 *
 */

int line_identifier(char *line, MACRO **mcr_ptr, MACRO *macro_table[HASH_TABLE_SIZE], char *file_name, int line_num)
{

    int word_cnt = 0;
    int mcr_definition = 0;
    int end_mcr = 0;
    char *word;
    char *macro_name = NULL;
    MACRO *temp_mcr = NULL;

    while ((word = get_word(&line)) != NULL)
    {
        word_cnt++;

        if (word_cnt == 1 && word[0] == ';')
        {
            free(word);
            return note_sentence;
        }

        if (strcmp(word, "mcr") == 0) /*Start of macro defenition*/
        {
            if (word_cnt > 1) /*Start of macro_defenition that is not in the begening of the line */
            {
                printf("Error in file : %s , line number: %d ,  macro definition must be at the beginning of the line \n", file_name, line_num);
                free(macro_name);
                free(word);
                return error;
            }

            else
            {
                mcr_definition = 1;
            }
        }

        else if (mcr_definition) /*Line of macro_defenition*/
        {
            if (word_cnt == 2)
            {
                macro_name = (char *)malloc((strlen(word) + 1) * sizeof(char));
                if (macro_name == NULL)
                {
                    printf("Error in file : %s ,memory allocation failed \n", file_name);
                    free(word);
                    return error;
                }

                strcpy(macro_name, word);

                if ((macro_lookup(macro_name, macro_table)) != NULL)
                {
                    printf("Error in file : %s , line number: %d , attempt to define a macro with the name of a macro that already exists \n", file_name, line_num);
                    free(macro_name);
                    free(word);
                    return error;
                }

                if (is_directive(macro_name) > -1 || is_operation(macro_name) > -1)
                {
                    printf("Error in file : %s , line number: %d , the macro was given the name of a directive or instruction \n", file_name, line_num);
                    free(macro_name);
                    free(word);
                    return error;
                }
            }

            if (word_cnt > 2)
            {
                printf("Error in file : %s , line number: %d , there are words in the line of the macro definition except the macro name and mcr \n", file_name, line_num);
                free(macro_name);
                free(word);
                return error;
            }
        }

        else if (strcmp(word, "endmcr") == 0)
        {

            end_mcr = 1;
        }

        else if ((temp_mcr = macro_lookup(word, macro_table)) != NULL)
        {
            *mcr_ptr = temp_mcr;
            free(macro_name);
            free(word);
            return macro_call;
        }

        else if (end_mcr && word_cnt > 1)
        {
            printf("Error in file : %s , line number: %d , text exists on the same line after endmcr \n", file_name, line_num);
            free(macro_name);
            free(word);
            return error;
        }

        free(word);
    }

    if (line[0] == '\0')
    {

        if (mcr_definition)
        {
            if (word_cnt == 1)
            {
                printf("Error in file : %s , line number: %d , defining a macro without giving a name\n", file_name, line_num);
                free(macro_name);
                return error;
            }

            else
            {
                if ((temp_mcr = macro_lookup(macro_name, macro_table)) == NULL)
                {

                    *mcr_ptr = insert_macro(macro_name, macro_table, file_name);
                    if (*mcr_ptr == NULL)
                    {
                        free(macro_name);
                        return error;
                    }

                    else
                    {
                        free(macro_name);
                        return macro_definition;
                    }
                }

                else
                {
                    free(macro_name);
                    return error;
                }
            }
        }

        else if (end_mcr)
        {

            *mcr_ptr = NULL;
            free(macro_name);
            return end_macro_definition;
        }

        else
        {
            free(macro_name);
            return any_other_line;
        }
    }

    else /*failed memory allocation in get_word function*/
    {
        printf("Error in file : %s , memory allocation failed\n", file_name);
        free(macro_name);
        return error;
    }
}

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
 *
 * Algorithm:
 *   1. Open the input assembly file for reading and create an output file for writing.
 *   2. Read each line from the input file and process it accordingly.
 *   3. Identify macro definitions, calls, and other lines using line_identifier function.
 *   4. Expand macros and write the processed lines to the output file.
 *   5. Close both input and output files.
 */

char *pre_assembly(char *file_name, MACRO *macro_table[HASH_TABLE_SIZE])
{

    FILE *as_file;
    FILE *am_file;
    char *as_file_name;
    char *am_file_name;
    int line_num = 1;
    char line[MAX_LINE_LEN] = {0};
    TEXT *content_ptr = NULL;
    MACRO *mcr_ptr = NULL;
    int mcr_def_flag = 0;

    /*creating the file names*/

    as_file_name = dynamic_strcat(file_name, ".as");
    am_file_name = dynamic_strcat(file_name, ".am");

    if (as_file_name == NULL || am_file_name == NULL)
    {
        printf("Error in file : %s ,memory allocation failed\n", file_name);
        free(as_file_name);
        free(am_file_name);
        return NULL;
    }

    /*opening the files*/

    as_file = fopen(as_file_name, "r");

    if (as_file == NULL)
    {
        printf("Error in file : %s , cannot be opened\n", as_file_name);
        free(as_file_name);
        free(am_file_name);
        return NULL;
    }

    am_file = fopen(am_file_name, "w");

    if (am_file == NULL)
    {
        printf("Error in file : %s , cannot be opened\n", am_file_name);
        free(as_file_name);
        free(am_file_name);
        fclose(as_file);
        return NULL;
    }

    while (fgets(line, MAX_LINE_LEN, as_file) != NULL) /*keep reading line until EOF*/
    {
        /*Checking if the line contains more than 80 characters*/
        if ((strchr(line, '\n') == NULL) && (strchr(line, EOF)))
        {
            printf("Error in file : %s , line number: %d,  The line contains over 80 characters\n", as_file_name, line_num);
            fclose(am_file);
            fclose(as_file);
            remove(am_file_name);
            free(am_file_name);
            free(as_file_name);
            return NULL;
        }

        switch (line_identifier(line, &mcr_ptr, macro_table, as_file_name, line_num))
        {
        case error: /*The line_identifier function print the error type*/

            fclose(am_file);
            fclose(as_file);
            remove(am_file_name);
            free(am_file_name);
            free(as_file_name);
            return NULL;

            break;

        case macro_definition: /*The line_identifier function add the macro into the macro table and change mcr_ptr to point to this macro*/
            mcr_def_flag = 1;
            break;

        case end_macro_definition: /*The line_identifier function change mcr_ptr so it wiil point to NULL */
            mcr_def_flag = 0;
            break;

        case macro_call: /*The line_identifier function change mcr_ptr to point to the relevant macro*/

            content_ptr = mcr_ptr->content;
            mcr_ptr = NULL;
            while (content_ptr)
            {
                fputs(content_ptr->text, am_file);
                content_ptr = content_ptr->next;
            }

            break;

        case note_sentence:
            fputs(line, am_file);

            break;

        case any_other_line:

            if (mcr_ptr != NULL)
            {
                if (text_insert(mcr_ptr, line, as_file_name, line_num) == MEMORY_ALLOCATION_ERROR)
                {
                    fclose(am_file);
                    fclose(as_file);
                    remove(am_file_name);
                    free(am_file_name);
                    free(as_file_name);
                    return NULL;
                }
            }

            else
            {
                fputs(line, am_file);
            }

            break;
        }
        line_num++;
    }

    if (mcr_def_flag == 1)
    {
        printf("Error in file : %s , a macro is defined without closing, i.e. without end_mcr\n", as_file_name);
        fclose(am_file);
        fclose(as_file);
        remove(am_file_name);
        free(am_file_name);
        free(as_file_name);
        return NULL;
    }

    fclose(am_file);
    fclose(as_file);
    free(as_file_name);

    return am_file_name;
}

/*
 * Function: free_macro_content
 * -----------------------------
 * Frees the memory allocated for a macro's content (lines of text).
 *
 * Parameters:
 *   content: The content of the macro to free.
 *
 * Algorithm:
 *   1. Traverse the linked list of TEXT nodes.
 *   2. Free memory allocated for each TEXT node.
 */

void free_macro_content(TEXT *content)
{
    TEXT *current = content;
    TEXT *temp;
    while (current != NULL)
    {
        temp = current;
        current = current->next;
        free(temp->text);
        free(temp);
    }
}

/*
 * Function: free_macro_table
 * ---------------------------
 * Frees the memory allocated for the macro hash table and its contents.
 *
 * Parameters:
 *   macro_table: The hash table containing macro definitions.
 *
 * Algorithm:
 *   1. Traverse each slot of the hash table.
 *   2. Free memory allocated for each MACRO node and its content.
 */

void free_macro_table(MACRO *macro_table[HASH_TABLE_SIZE])
{
    MACRO *current;
    MACRO *temp;
    int i;

    for (i = 0; i < HASH_TABLE_SIZE; i++)
    {
        current = macro_table[i];
        while (current != NULL)
        {
            temp = current;
            current = current->next;
            free_macro_content(temp->content);
            free(temp->name);
            free(temp);
        }
        macro_table[i] = NULL;
    }
}
