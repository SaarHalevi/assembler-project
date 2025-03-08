#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "include/back_end.h"

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
 *
 * Algorithm:
 *   1. Concatenate ".ob" extension to the provided file_name to get the output file name.
 *   2. Open the output file for writing.
 *   3. Write the header containing the instruction counter (ic) and the data counter (dc) to the file.
 *   4. Iterate through the instruction array and data array, encoding the content in encrypted base 4 and writing the address and content to the file.
 *   5. Close the output file and free the dynamically allocated memory.
 */
int create_object_file(TRANSLATION_UNIT *curr_program, char *file_name)
{

    char Encrypted_base_four[] = {'*', '#', '%', '!'};
    char *ob_file_name;
    FILE *ob_file;
    int i;
    int address;
    short bits_0_1, bits_2_3, bits_4_5, bits_6_7, bits_8_9, bits_10_11, bits_12_13;

    ob_file_name = dynamic_strcat(file_name, ".ob");
    if (ob_file_name == NULL)
    {
        printf("Error the file : %s , memory allocation failed \n", file_name);
        return MEMORY_ALLOCATION_ERROR;
    }

    ob_file = fopen(ob_file_name, "w");

    if (ob_file == NULL)
    {
        printf("Error the file : %s , cannot be opened\n", ob_file_name);
        free(ob_file_name);
        return MEMORY_ALLOCATION_ERROR;
    }

    fprintf(ob_file, "  %d %d\n", curr_program->ic, curr_program->dc);

    for (i = 0; i < curr_program->ic; i++)
    {
        address = i + 100;
        bits_0_1 = curr_program->instruction_array[i] & 3;
        bits_2_3 = (curr_program->instruction_array[i] >> 2) & 3;
        bits_4_5 = (curr_program->instruction_array[i] >> 4) & 3;
        bits_6_7 = (curr_program->instruction_array[i] >> 6) & 3;
        bits_8_9 = (curr_program->instruction_array[i] >> 8) & 3;
        bits_10_11 = (curr_program->instruction_array[i] >> 10) & 3;
        bits_12_13 = (curr_program->instruction_array[i] >> 12) & 3;

        fprintf(ob_file, "0%d %c%c%c%c%c%c%c\n", address, Encrypted_base_four[bits_12_13], Encrypted_base_four[bits_10_11], Encrypted_base_four[bits_8_9],
                Encrypted_base_four[bits_6_7], Encrypted_base_four[bits_4_5], Encrypted_base_four[bits_2_3], Encrypted_base_four[bits_0_1]);
    }

    curr_program->ic += 100;

    for (i = 0; i < curr_program->dc; i++)
    {
        address = i + curr_program->ic;
        bits_0_1 = curr_program->data_array[i] & 3;
        bits_2_3 = (curr_program->data_array[i] >> 2) & 3;
        bits_4_5 = (curr_program->data_array[i] >> 4) & 3;
        bits_6_7 = (curr_program->data_array[i] >> 6) & 3;
        bits_8_9 = (curr_program->data_array[i] >> 8) & 3;
        bits_10_11 = (curr_program->data_array[i] >> 10) & 3;
        bits_12_13 = (curr_program->data_array[i] >> 12) & 3;

        fprintf(ob_file, "0%d %c%c%c%c%c%c%c\n", address, Encrypted_base_four[bits_12_13], Encrypted_base_four[bits_10_11], Encrypted_base_four[bits_8_9],
                Encrypted_base_four[bits_6_7], Encrypted_base_four[bits_4_5], Encrypted_base_four[bits_2_3], Encrypted_base_four[bits_0_1]);
    }

    fclose(ob_file);
    free(ob_file_name);
    return NO_ERRORS;
}

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
 *
 * Algorithm:
 *   1. Concatenate ".ent" extension to the provided file_name to get the output file name.
 *   2. Open the output file for writing.
 *   3. Iterate through the entries list, writing each symbol name and its corresponding address to the file.
 *   4. Close the output file and free the dynamically allocated memory.
 */

int create_entries_file(TRANSLATION_UNIT *curr_program, char *file_name)
{
    char *ent_file_name;
    FILE *ent_file;
    ENTRY_NODE *entry_ptr = curr_program->entries_list;

    ent_file_name = dynamic_strcat(file_name, ".ent");

    if (ent_file_name == NULL)
    {
        printf("Error the file : %s , memory allocation failed\n", file_name);
        return MEMORY_ALLOCATION_ERROR;
    }

    ent_file = fopen(ent_file_name, "w");

    if (ent_file == NULL)
    {
        printf("Error the file : %s , cannot be opened\n", ent_file_name);
        free(ent_file_name);
        return MEMORY_ALLOCATION_ERROR;
    }

    while (entry_ptr != NULL)
    {
        fprintf(ent_file, "%s\t0%d\n", entry_ptr->symbol->name, entry_ptr->symbol->address);
        entry_ptr = entry_ptr->next;
    }

    fclose(ent_file);
    free(ent_file_name);
    return NO_ERRORS;
}

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
 *
 * Algorithm:
 *   1. Concatenate ".ext" extension to the provided file name to obtain the output file name.
 *   2. Open the output file for writing.
 *   3. Iterate through the list of external symbols, and for each symbol:
 *      a. Iterate through its associated addresses in the machine code.
 *      b. Write each address-symbol pair to the file.
 *   4. Close the output file and free the dynamically allocated memory.
 */

int create_external_file(TRANSLATION_UNIT *curr_program, char *file_name)
{
    char *ext_file_name;
    FILE *ext_file;
    EXT_SYMBOL *ext_ptr = curr_program->ext_list;
    EXT_ADDRESS *address_ptr;
    int address;

    ext_file_name = dynamic_strcat(file_name, ".ext");
    if (ext_file_name == NULL)
    {

        printf("Error the file : %s , memory allocation failed \n", file_name);
        return MEMORY_ALLOCATION_ERROR;
    }

    ext_file = fopen(ext_file_name, "w");

    if (ext_file == NULL)
    {

        printf("Error the file : %s , cannot be opened\n", ext_file_name);
        free(ext_file_name);
        return MEMORY_ALLOCATION_ERROR;
    }

    while (ext_ptr != NULL)
    {
        address_ptr = ext_ptr->address;

        while (address_ptr != NULL)
        {
            address = address_ptr->num + 100;
            fprintf(ext_file, "%s\t0%d\n", ext_ptr->ext_name, address);
            address_ptr = address_ptr->next_address;
        }
        ext_ptr = ext_ptr->next_ext;
    }

    fclose(ext_file);
    free(ext_file_name);
    return NO_ERRORS;
}

