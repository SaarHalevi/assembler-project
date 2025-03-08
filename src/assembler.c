#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/front_end.h"
#include "include/general.h"
#include "include/pre_assembler.h"
#include "include/first_pass.h"
#include "include/second_pass.h"
#include "include/back_end.h"

/*
 * Main Function - Assembler Program
 *
 * Description:
 *   The main function orchestrates the entire process of assembling assembly files.
 *   It iterates over each file passed through the command line and performs these steps:
 *   pre-assembling, first pass, second pass, and back-end.
 *   Memory allocation errors are handled by freeing allocated memory and continuing to the next file.
 *
 * The assumption is that the files do not require more than 3996 memory cells in order to encode them.
 *
 *
 * Parameters:
 *   argc: An integer representing the number of command-line arguments.
 *   argv: An array of strings containing the command-line arguments.
 *
 * Return Value: 1
 *
 *
 * Operation:
 *   - The function initializes variables and data structures needed for processing.
 *   - It iterates over each input file received through the command line and make these steps:
 *   - Pre-assembling:
 *                     - Check errors in the macros definition and that no line in the file containing more than 80 characters.
 *                     - If there are no errors, a file with the extension am is created that containing the contents of the file after placing the macro.
 *                     - If there are errors the program will not proceed to the next steps for the current file.
 *                     - Rather, it will release the allocated memory and continue to the next file received on the command line.
 *   - first pass:
 *                     - Go over the am file and detect syntax errors in the text.
 *                     - Additionally creates the symbol table, and a list containing the entry symbols in the program.
 *   - Second pass:
 *                     - If there are no errors in the first pass, the program continues to the second pass.
 *                     - At this step, the program go over the am file again and detect logical errors as well as encode  the assembly code into binary code.
 *   - Back end :
 *                     - If there were no errors in the previous steps, the program continues to this step.
 *                     - it creates object files and additional files (entries, externs) if needed.
 *
 *   - At the end of each iteration, the program releases the allocated dynamic memory and closes the relevant files.
 */

int main(int argc, char **argv)
{
    int i;
    char *am_file_name;
    FILE *am_file;
    TRANSLATION_UNIT curr_program;
    MACRO *macro_table[HASH_TABLE_SIZE] = {0};
    int first_pass_error_flag, second_pass_error_flag;

    for (i = 1; i < argc; i++)
    {
        memset(&curr_program, 0, sizeof(curr_program));

        /*pre_assembley process  - create a file after opening the macros (am file) ,if an error occured it remove the am file */
        am_file_name = pre_assembly(argv[i], macro_table);

        /*If there is an error in the pre-abmsley process (meaning that The am_file_name == NULL),  the program moves to the next file received by the command line*/
        if (am_file_name)
        {

            am_file = fopen(am_file_name, "r");
            if (am_file)
            {

                first_pass_error_flag = first_pass(&curr_program, am_file, am_file_name, macro_table);

                if (first_pass_error_flag == MEMORY_ALLOCATION_ERROR)
                {
                    /* free all the memory in the program and continue to next file */
                    free_symbol_table(curr_program.symbol_table);
                    free_entries_list(curr_program.entries_list);
                    free_macro_table(macro_table);
                    free(am_file_name);
                    fclose(am_file);
                    continue;
                }

                else if (first_pass_error_flag == NO_ERRORS)
                {
                    rewind(am_file);
                    second_pass_error_flag = second_pass(&curr_program, am_file_name, am_file);

                    if (second_pass_error_flag == MEMORY_ALLOCATION_ERROR)
                    {
                        /* free all the memory in the program and continue to next file */
                        free_symbol_table(curr_program.symbol_table);
                        free_entries_list(curr_program.entries_list);
                        free_ext_list(&curr_program.ext_list);
                        free_macro_table(macro_table);
                        free(am_file_name);
                        fclose(am_file);
                        continue;
                    }

                    else if (second_pass_error_flag == NO_ERRORS)
                    {
                        /*create the object file, using the translation unit */

                        if (create_object_file(&curr_program, argv[i]) == MEMORY_ALLOCATION_ERROR)
                        {
                            /* free all the memory in the program and continue to next file */
                            free_symbol_table(curr_program.symbol_table);
                            free_entries_list(curr_program.entries_list);
                            free_ext_list(&curr_program.ext_list);
                            free_macro_table(macro_table);
                            free(am_file_name);
                            fclose(am_file);
                            continue;
                        }

                        else /*There is no memory or file opening error in the creation of the ob file */
                        {
                            if (curr_program.entries_counter > 0)
                            {
                                if (create_entries_file(&curr_program, argv[i]) == MEMORY_ALLOCATION_ERROR)
                                {
                                    /* free all the memory in the program and continue to next file */
                                    free_symbol_table(curr_program.symbol_table);
                                    free_entries_list(curr_program.entries_list);
                                    free_ext_list(&curr_program.ext_list);
                                    free_macro_table(macro_table);
                                    free(am_file_name);
                                    fclose(am_file);
                                    continue;
                                }
                            }

                            if (curr_program.external_counter > 0)
                            {
                                if (create_external_file(&curr_program, argv[i]) == MEMORY_ALLOCATION_ERROR)
                                {
                                    /* free all the memory in the program and continue to next file */
                                    free_symbol_table(curr_program.symbol_table);
                                    free_entries_list(curr_program.entries_list);
                                    free_ext_list(&curr_program.ext_list);
                                    free_macro_table(macro_table);
                                    free(am_file_name);
                                    fclose(am_file);
                                    continue;
                                }
                            }
                        }
                    }
                }

                free_ext_list(&curr_program.ext_list);
                free_symbol_table(curr_program.symbol_table);
                free_entries_list(curr_program.entries_list);
            }

            free(am_file_name);
            fclose(am_file);
        }

        free_macro_table(macro_table);
    }

    return 1;
}
