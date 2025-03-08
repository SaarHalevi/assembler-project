#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "include/second_pass.h"

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
 *
 * Algorithm:
 *   1. Initialize a pointer (sym_ptr) to the head of the external symbol list.
 *   2. Iterate through the external symbol list using a while loop until sym_ptr reaches the end (NULL).
 *   3. Within each iteration, check if the name of the current symbol (sym_ptr->ext_name) matches the provided symbol_name using strcmp.
 *   4. If a match is found, return the pointer to the current symbol (sym_ptr).
 *   5. If no match is found after iterating through the entire list, return NULL to indicate that the symbol was not found.
 */

EXT_SYMBOL *ext_search(EXT_SYMBOL *ext_list, char *symbol_name)
{

    EXT_SYMBOL *sym_ptr = ext_list;

    while (sym_ptr != NULL)
    {
        if (strcmp(sym_ptr->ext_name, symbol_name) == 0)
        {
            return sym_ptr;
        }

        sym_ptr = sym_ptr->next_ext;
    }

    return sym_ptr;
}

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
 *
 * Algorithm:
 *   1. Allocate memory for a new external address structure.
 *   2. Check if memory allocation was successful. If not, return MEMORY_ALLOCATION_ERROR.
 *   3. Initialize the new external address with the provided symbol address.
 *   4. Search for the symbol in the external symbol list.
 *   5. If the symbol exists in the list:
 *      - Update the new address's next pointer to point to the existing symbol's address list.
 *      - Update the existing symbol's address list to point to the new address.
 *      - Return NO_ERRORS to indicate success.
 *   6. If the symbol does not exist in the list:
 *      - Allocate memory for a new external symbol structure.
 *      - Check if memory allocation was successful. If not, free the allocated address memory and return MEMORY_ALLOCATION_ERROR.
 *      - Initialize the new external symbol with the provided symbol name and the newly allocated address.
 *      - Update the new symbol's next pointer to point to the current head of the external symbol list.
 *      - Update the external symbol list pointer to point to the new symbol.
 *      - Return NO_ERRORS to indicate success.
 */

int add_ext_address(EXT_SYMBOL **ext_list, char *symbol_name, int symbol_address)
{
    EXT_SYMBOL *ext_find;
    EXT_SYMBOL *new_ext;
    EXT_ADDRESS *new_address;

    new_address = (EXT_ADDRESS *)malloc(sizeof(EXT_ADDRESS));

    if (new_address == NULL)
    {
        return MEMORY_ALLOCATION_ERROR;
    }

    new_address->num = symbol_address;
    new_address->next_address = NULL;

    ext_find = ext_search(*ext_list, symbol_name);
    if (ext_find)
    {
        new_address->next_address = ext_find->address;
        ext_find->address = new_address;
        return NO_ERRORS;
    }

    else /*The ext symbol is not exist in the external symbol list*/
    {
        new_ext = (EXT_SYMBOL *)malloc(sizeof(EXT_SYMBOL));
        if (new_ext == NULL)
        {
            free(new_address);
            return MEMORY_ALLOCATION_ERROR;
        }

        new_ext->address = new_address;
        strcpy(new_ext->ext_name, symbol_name);
        new_ext->next_ext = *ext_list;
        *ext_list = new_ext;
        return NO_ERRORS;
    }
}

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
 *
 * Algorithm:
 *   1. Initialize variables including error flags, line number, and addressing method.
 *   2. Read each line from the assembly file.
 *   3. Create an Abstract Syntax Tree (AST) from the text of the line.
 *   5. Process each line based on its type (instruction or directive).
 *   6. Generate machine code for instructions or directives line and store it in the translation unit.
 *   7. Handle errors related to undefined symbols or constants.
 *   8. Continue processing until the end of the file.
 */

int second_pass(TRANSLATION_UNIT *curr_program, char *am_file_name, FILE *am_file)
{
    int err_flag = NO_ERRORS;
    int line_num = 1;
    int i;
    short addressing_method;
    char line[MAX_LINE_LEN];
    LINE_AST line_ast = {0};
    SYMBOL *sym_find;
    int ext_add_result;

    while (fgets(line, MAX_LINE_LEN, am_file))
    {

        line_ast = create_ast_from_text(line);

        if (strcmp(line_ast.error_detail, "memory allocation failed") == 0)
        {
            return MEMORY_ALLOCATION_ERROR;
        }

        if (line_ast.type == inst)
        {
            /*create the binary code of the first memory word - according to th logic of the project*/

            for (i = 0; i < MAX_INST_OPERANDS; i++)
            {
                if (line_ast.operand.instruction.operands[i].type == number || line_ast.operand.instruction.operands[i].type == constant || line_ast.operand.instruction.operands[i].type == none)
                {
                    addressing_method = 0;
                }

                else if (line_ast.operand.instruction.operands[i].type == label)
                {
                    addressing_method = 1;
                }

                else if (line_ast.operand.instruction.operands[i].type == label_with_index)
                {
                    addressing_method = 2;
                }

                else /*The operand type is register*/
                {
                    addressing_method = 3;
                }

                if (i == DESTINATION_OPERAND)
                {
                    curr_program->instruction_array[curr_program->ic] |= addressing_method << 2;
                }

                else
                {
                    curr_program->instruction_array[curr_program->ic] |= addressing_method << 4;
                }
            }

            curr_program->instruction_array[curr_program->ic] |= line_ast.operand.instruction.type << 6;
            curr_program->ic++;

            /*Identifying whether additional memory words need to be created and if so creating them*/

            /*If the operand are register, add one memory word*/
            if ((line_ast.operand.instruction.operands[SOURCE_OPERAND].type == reg) && (line_ast.operand.instruction.operands[DESTINATION_OPERAND].type == reg))
            {
                curr_program->instruction_array[curr_program->ic] |= line_ast.operand.instruction.operands[DESTINATION_OPERAND].num << 2;
                curr_program->instruction_array[curr_program->ic] |= line_ast.operand.instruction.operands[SOURCE_OPERAND].num << 5;
                curr_program->ic++;
            }

            else /*at least one of the operands is not register*/
            {
                for (i = 0; i < MAX_INST_OPERANDS; i++)
                {
                    if (line_ast.operand.instruction.operands[i].type == number)
                    {

                        curr_program->instruction_array[curr_program->ic] |= line_ast.operand.instruction.operands[i].num << 2;
                        curr_program->ic++;
                    }

                    else if (line_ast.operand.instruction.operands[i].type == constant)
                    {
                        /*Checking whether the constant was defined in the file, and if so checking whether it was defined in the line before this line*/
                        sym_find = symbol_lookup(line_ast.operand.instruction.operands[i].constant_name, curr_program->symbol_table);

                        if (sym_find)
                        {
                            /*The symbol was define on a line before the current line*/
                            if (sym_find->address < line_num)
                            {
                                curr_program->instruction_array[curr_program->ic] |= sym_find->value << 2;
                                curr_program->ic++;
                            }

                            else /*The symbol was define on a line after the current line*/
                            {
                                printf("Error in: %s , in line number: %d , using a constant whose definition is done at a later stage in the file\n", am_file_name, line_num);
                                err_flag = SYNTAX_OR_LOGIC_ERROR;
                                continue;
                            }
                        }

                        else /*The constant was not defined in the file*/
                        {
                            printf("Error in: %s , in line number: %d , using a constant that was not defined in the file\n", am_file_name, line_num);
                            err_flag = SYNTAX_OR_LOGIC_ERROR;
                            continue;
                        }
                    }

                    else if (line_ast.operand.instruction.operands[i].type == reg)
                    {
                        if (i == SOURCE_OPERAND)
                        {
                            curr_program->instruction_array[curr_program->ic] |= line_ast.operand.instruction.operands[SOURCE_OPERAND].num << 5;
                        }

                        else /*Its is a destnation operand */
                        {
                            curr_program->instruction_array[curr_program->ic] |= line_ast.operand.instruction.operands[DESTINATION_OPERAND].num << 2;
                        }

                        curr_program->ic++;
                    }

                    else if (line_ast.operand.instruction.operands[i].type == label)
                    {
                        /*Checking whether the label was defined in the file*/
                        sym_find = symbol_lookup(line_ast.operand.instruction.operands[i].label, curr_program->symbol_table);

                        if (sym_find)
                        {

                            if (sym_find->type == extern_sym)
                            {
                                /*The 0-1 bits are 01, the other 12 bits are 0*/
                                curr_program->instruction_array[curr_program->ic] |= 1;
                                /*add_ext_adress */
                                curr_program->external_counter++;
                                ext_add_result = add_ext_address(&curr_program->ext_list, sym_find->name, curr_program->ic);
                                if (ext_add_result == MEMORY_ALLOCATION_ERROR)
                                {
                                    printf("Error in: %s , memory allocation failed \n", am_file_name);
                                    return MEMORY_ALLOCATION_ERROR;
                                }
                            }

                            else
                            {
                                /*The 0-1 bits are 10, the other 12 bits are the binary code of the adrress of the symbol as unsinged number*/
                                curr_program->instruction_array[curr_program->ic] |= 2;
                                curr_program->instruction_array[curr_program->ic] |= sym_find->address << 2;
                            }

                            curr_program->ic++;
                        }

                        else /*The label was not defined in the file*/
                        {
                            printf("Error in: %s , in line number: %d , using a label that was not defined in the file\n", am_file_name, line_num);
                            err_flag = SYNTAX_OR_LOGIC_ERROR;
                            continue;
                        }
                    }

                    else if (line_ast.operand.instruction.operands[i].type == label_with_index)
                    {
                        /*Checking whether the label was defined in the file*/
                        sym_find = symbol_lookup(line_ast.operand.instruction.operands[i].label, curr_program->symbol_table);

                        if (sym_find)
                        {

                            if (sym_find->type == extern_sym)
                            {
                                /*The 0-1 bits are 01, the other 12 bits are 0*/
                                curr_program->instruction_array[curr_program->ic] |= 1;
                                /*add_ext_adress */
                                curr_program->external_counter++;
                                ext_add_result = add_ext_address(&curr_program->ext_list, sym_find->name, curr_program->ic);
                                if (ext_add_result == MEMORY_ALLOCATION_ERROR)
                                {
                                    printf("Error in: %s , memory allocation failed\n", am_file_name);
                                    return MEMORY_ALLOCATION_ERROR;
                                }
                            }

                            else
                            {
                                /*The 0-1 bits are 10, the other 12 bits are the binary code of the adrress of the symbol as unsinged number*/
                                curr_program->instruction_array[curr_program->ic] |= 2;
                                curr_program->instruction_array[curr_program->ic] |= sym_find->address << 2;
                            }

                            curr_program->ic++;

                            /*Check whether the index is constant*/
                            if (line_ast.operand.instruction.operands[i].constant_name[0] != '\0')
                            {
                                sym_find = symbol_lookup(line_ast.operand.instruction.operands[i].constant_name, curr_program->symbol_table);

                                if (sym_find)
                                {
                                    /*The constant was define on a line before the current line*/
                                    if (sym_find->address < line_num)
                                    {
                                        /*The 0-1 bits are 0, the other 12 bits are the binary code of the number That the constant represent*/
                                        curr_program->instruction_array[curr_program->ic] |= sym_find->value << 2;
                                    }

                                    else /*The constant was define on a line after the current line*/
                                    {
                                        printf("Error in: %s , in line number: %d , using a constant whose definition is done at a later stage in the file\n", am_file_name, line_num);
                                        err_flag = SYNTAX_OR_LOGIC_ERROR;
                                        continue;
                                    }
                                }

                                else /*The constant was not defined in the file*/
                                {
                                    printf("Error in: %s , in line number: %d , using a constant that was not defined in the file\n", am_file_name, line_num);
                                    err_flag = SYNTAX_OR_LOGIC_ERROR;
                                    continue;
                                }
                            }

                            else /*the index is a number*/
                            {
                                /*the 0-1 bits are 0 and the others are the binary code of the num*/
                                curr_program->instruction_array[curr_program->ic] |= line_ast.operand.instruction.operands[i].num << 2;
                            }

                            curr_program->ic++;
                        }

                        else /*The label was not defined in the file*/
                        {
                            printf("Error in: %s , in line number: %d , using a label that was not defined in the file\n", am_file_name, line_num);
                            err_flag = SYNTAX_OR_LOGIC_ERROR;
                            continue;
                        }
                    }
                }
            }
        }

        else if (line_ast.type == dir)
        {
            if (line_ast.operand.directive.type == string)
            {
                i = 0;
                while (line_ast.operand.directive.operands.str[i] != 0)
                {
                    /* add to the data image the asci code of the number using 14 bits */
                    curr_program->data_array[curr_program->dc] |= line_ast.operand.directive.operands.str[i];
                    curr_program->dc++;
                    i++;
                }
                /*advancing the dc counter for the \0 char */
                curr_program->dc++;
            }

            else if (line_ast.operand.directive.type == data)
            {
                for (i = 0; i < line_ast.operand.directive.operand_counter; i++)
                {
                    if (line_ast.operand.directive.operands.data[i].data_type == data_int)
                    {
                        curr_program->data_array[curr_program->dc] |= line_ast.operand.directive.operands.data[i].data_value.num;
                    }

                    else /*the current operand in data is constant*/
                    {
                        sym_find = symbol_lookup(line_ast.operand.directive.operands.data[i].data_value.constant_name, curr_program->symbol_table);

                        if (sym_find)
                        {
                            /*The symbol was define on a line before the current line*/
                            if (sym_find->address < line_num)
                            {
                                /* add the asci code of the number that constant is represent */
                                curr_program->data_array[curr_program->dc] |= sym_find->value;
                            }

                            else /*/*The symbol was define on a line after the current line*/
                            {
                                printf("Error in: %s , in line number: %d , using a constant whose definition is done at a later stage in the file\n", am_file_name, line_num);
                                err_flag = SYNTAX_OR_LOGIC_ERROR;
                                continue;
                            }
                        }

                        else /*The constant was not defined in the file*/
                        {
                            printf("Error in: %s , in line number: %d , using a constant that was not defined in the file\n", am_file_name, line_num);
                            err_flag = SYNTAX_OR_LOGIC_ERROR;
                            continue;
                        }
                    }
                    curr_program->dc++;
                }
            }
        }

        line_num++;
    }

    if (err_flag == NO_ERRORS)
    {
        return NO_ERRORS;
    }

    else
    {
        return SYNTAX_OR_LOGIC_ERROR;
    }
}

