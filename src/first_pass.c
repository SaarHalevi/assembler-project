#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "include/first_pass.h"
#include "include/front_end.h"
#include "include/general.h"
#include "include/pre_assembler.h"

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
 *   Returns NO_ERRORS (0) if the operation is successful. Returns MEMORY_ALLOCATION_ERROR (-2) if memory allocation fails.
 *
 * Algorithm:
 *   1. Calculate the hash index for the symbol name.
 *   2. Allocate memory for a new symbol structure.
 *   3. Copy the symbol name and other details into the new symbol structure.
 *   4. Insert the symbol into the symbol table at the calculated index.
 */

int insert_symbol(SYMBOL *symbol_table[HASH_TABLE_SIZE], char *symbol_name, int symbol_type, int address, int value)
{
    unsigned int index = hash(symbol_name);
    SYMBOL *new_symbol = (SYMBOL *)malloc(sizeof(SYMBOL));

    if (new_symbol == NULL)
    {
        return MEMORY_ALLOCATION_ERROR;
    }

    strcpy(new_symbol->name, symbol_name);
    new_symbol->type = symbol_type;
    new_symbol->address = address;
    new_symbol->value = value;
    new_symbol->next = NULL;

    if (symbol_table[index] == NULL)
    {
        symbol_table[index] = new_symbol;
        return NO_ERRORS;
    }

    else
    {
        new_symbol->next = symbol_table[index];
        symbol_table[index] = new_symbol;
        return NO_ERRORS;
    }
}


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
 *
 * Algorithm:
 *   1. Check the type of line (directive or instruction).
 *   2. Based on the line type, calculate the memory cells required.
 *   3. For directives, count the number of operands.
 *   4. For instructions, consider the number and types of operands.
 */

int memory_cell_calculator(LINE_AST line)
{
    int result = 1;
    int i = 0;
    if (line.type == dir)
    {
        if (line.operand.directive.type == string)
        {
            while (line.operand.directive.operands.str[i] != 0)
            {
                result += 1;
                i++;
            }
        }

        else
        { /*the directive is data*/
            result = line.operand.directive.operand_counter;
        }
    }

    else
    { /*line type is instruction*/

        /*The  operation type is suitable for receiving only one operand */
        if ((line.operand.instruction.type > sub) && (line.operand.instruction.type < rts) && (line.operand.instruction.type != lea))
        {
            if (line.operand.instruction.operands[0].type == label_with_index)
            {
                result += 2;
            }

            else
            {
                result += 1;
            }
        }

        /*The operation type is suitable for receiving 2 operands*/
        else if (line.operand.instruction.type <= sub || line.operand.instruction.type == lea)
        {
            if (line.operand.instruction.operands[0].type == reg && line.operand.instruction.operands[1].type == reg)
            {
                result += 1;
            }

            else
            {
                for (i = 0; i < MAX_INST_OPERANDS; i++)
                {
                    if (line.operand.instruction.operands[i].type == label_with_index)
                    {
                        result += 2;
                    }

                    else
                    {
                        result += 1;
                    }
                }
            }
        }
    }

    return result;
}


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
 *   Returns NO_ERRORS  if the operation is successful. Returns SYNTAX_OR_LOGIC_ERROR 
 *   if a syntax or logic error is encountered during processing and MEMORY_ALLOCATION_ERROR  If memory allocation fails.
 *
 * Algorithm:
 *   1. Read each line from the assembly file.
 *   2. Parse the line into an abstract syntax tree (AST).
 *   3. Process the AST to update memory counters and build the symbol table.
 *   4. Handle syntax errors:
 *      - Print an error message with file name and line number.
 *      - If memory allocation fails, return MEMORY_ALLOCATION_ERROR (-1).
 *   5. Return appropriate status codes based on success or failure.
 */

int first_pass(TRANSLATION_UNIT *curr_program, FILE *am_file, char *am_file_name, MACRO *macro_table[HASH_TABLE_SIZE])
{

    char line[MAX_LINE_LEN];
    int ic = 100;
    int dc = 0;
    int err_flag = NO_ERRORS;
    int line_num = 1;
    int i;
    int added_memory;
    LINE_AST line_ast = {0};
    SYMBOL *sym_find;
    SYMBOL *sym_ptr;

    while (fgets(line, MAX_LINE_LEN, am_file))
    {
        added_memory = 0;
        line_ast = create_ast_from_text(line);

        /*Chceck if there is syntax error*/
        if (line_ast.type == error)
        {

            printf("Error in: %s , in line number: %d , %s\n", am_file_name, line_num, line_ast.error_detail);
            
            /*If memory allocation failed, the program must be stopped for the current file*/
            if (strcmp(line_ast.error_detail, "memory allocation failed") == 0)
            {
                return MEMORY_ALLOCATION_ERROR;
            }

            line_num++;
            err_flag = SYNTAX_OR_LOGIC_ERROR;
            continue;
        }

        if (line_ast.type == dir || line_ast.type == inst)
        {

            /*Checking a scenario in which a label is defined (for a directive or an instruction)*/
            if (line_ast.label[0] != '\0')
            {

                if (macro_lookup(line_ast.label, macro_table) != NULL)
                {
                    printf("Error in: %s, in line number: %d , redefining a name for a macro and symbol\n", am_file_name, line_num);
                    line_num++;
                    err_flag = SYNTAX_OR_LOGIC_ERROR;
                    continue;
                }

                else
                {

                    sym_find = symbol_lookup(line_ast.label, curr_program->symbol_table);

                    if (sym_find)
                    {
                        /*if the label was previously defined as an entry, we will associate the current address with it*/
                        if (sym_find->type == entry_sym_without_definition)
                        {
                            if (line_ast.type == dir)
                            {
                                sym_find->type = data_entry_sym;
                                sym_find->address = dc;
                            }

                            else
                            {
                                sym_find->type = inst_entry_sym;
                                sym_find->address = ic;
                            }
                        }

                        /*It was previously defined without an entry, this is a repeated definition and therefore an error */
                        else
                        {
                            printf("Error in: %s , in line number: %d , redefenition of symbol\n", am_file_name, line_num);
                            line_num++;
                            err_flag = SYNTAX_OR_LOGIC_ERROR;
                            continue;
                        }
                    }

                    /*It is a symbol defeniton and the symbol is not exist in the symbol table*/
                    else
                    {
                        if (line_ast.type == inst)
                        {
                            if (insert_symbol(curr_program->symbol_table, line_ast.label, inst_sym, ic, 0) == MEMORY_ALLOCATION_ERROR)
                            {
                                return MEMORY_ALLOCATION_ERROR;
                            }
                        }

                        else
                        {
                            if (insert_symbol(curr_program->symbol_table, line_ast.label, data_sym, dc, 0) == MEMORY_ALLOCATION_ERROR)
                            {
                                return MEMORY_ALLOCATION_ERROR;
                            }
                        }
                    }
                }
            }

            /*Advancing the memory counters*/

            if (line_ast.type == inst)
            {
                added_memory = memory_cell_calculator(line_ast);
                ic += added_memory;
            }

            else
            {
                if (line_ast.operand.directive.type == data || line_ast.operand.directive.type == string)
                {
                    added_memory = memory_cell_calculator(line_ast);
                    dc += added_memory;
                }

                /*Checking a scenario of declaring a symbol as entry or extern*/
                else if ((line_ast.operand.directive.type == entry || line_ast.operand.directive.type == ext))
                {
                    if (macro_lookup(line_ast.operand.directive.operands.label_operand, macro_table) != NULL)
                    {
                        printf("Error in: %s, in line number: %d , redefining a name for a macro and symbol\n", am_file_name, line_num);
                        line_num++;
                        err_flag = SYNTAX_OR_LOGIC_ERROR;
                        continue;
                    }

                    else
                    {

                        sym_find = symbol_lookup(line_ast.operand.directive.operands.label_operand, curr_program->symbol_table);

                        if (sym_find) /*If the symbol is already exist in the symbil table*/
                        {
                            if (line_ast.operand.directive.type == entry)
                            {
                                if (sym_find->type == data_sym)
                                {
                                    sym_find->type = data_entry_sym;
                                }

                                else if (sym_find->type == inst_sym)
                                {
                                    sym_find->type = inst_entry_sym;
                                }

                                else
                                {
                                    printf("Error in: %s , in line number: %d , redefenition of symbol\n", am_file_name, line_num);
                                    line_num++;
                                    err_flag = SYNTAX_OR_LOGIC_ERROR;
                                    continue;
                                }
                            }

                            else /*It is external and cannot be defined twice in the same file */
                            {
                                printf("Error in: %s , in line number: %d , redefenition of symbol\n", am_file_name, line_num);
                                line_num++;
                                err_flag = SYNTAX_OR_LOGIC_ERROR;
                                continue;
                            }
                        }

                        else /*The symbol is not exist in the symbol table*/
                        {
                            if (line_ast.operand.directive.type == entry)
                            {
                                insert_symbol(curr_program->symbol_table, line_ast.operand.directive.operands.label_operand, entry_sym_without_definition, 0, 0);
                            }

                            else
                            {
                                insert_symbol(curr_program->symbol_table, line_ast.operand.directive.operands.label_operand, extern_sym, 0, 0);
                            }
                        }
                    }
                }
            }
        }

        /*If it is a constant definition statement we will insert it into the symbol table if it has not already been declared*/
        else if (line_ast.type == constant_def)
        {
            if (macro_lookup(line_ast.operand.constant.constant_name, macro_table) != NULL)
            {
                printf("Error in: %s , in line number: %d , redefining a name for a macro and constant\n", am_file_name, line_num);
                line_num++;
                err_flag = SYNTAX_OR_LOGIC_ERROR;
                continue;
            }

            else
            {
                sym_find = symbol_lookup(line_ast.label, curr_program->symbol_table);

                if (sym_find)
                {
                    printf("Error in: %s , in line number: %d , redefenition of symbol\n", am_file_name, line_num);
                    line_num++;
                    err_flag = SYNTAX_OR_LOGIC_ERROR;
                    continue;
                }

                else
                {
                    if (insert_symbol(curr_program->symbol_table, line_ast.operand.constant.constant_name, const_sym, line_num, line_ast.operand.constant.constant_num) == MEMORY_ALLOCATION_ERROR)
                    {
                        return MEMORY_ALLOCATION_ERROR;
                    }
                }
            }
        }

        line_num++;
    }

    for (i = 0; i < HASH_TABLE_SIZE; i++)
    {
        if (curr_program->symbol_table[i] != NULL)
        {
            sym_ptr = curr_program->symbol_table[i];
            while (sym_ptr != NULL)
            {
                if (sym_ptr->type == entry_sym_without_definition)
                {
                    printf("Error in: %s , the symbol: %s was defined as an entry but did not receive a value\n", am_file_name, sym_ptr->name);
                    err_flag = SYNTAX_OR_LOGIC_ERROR;
                    continue;
                }

                if (sym_ptr->type == data_entry_sym || sym_ptr->type == data_sym)
                {
                    sym_ptr->address += ic;
                }

                /*Creating a linked list of pointers for all entry symbols*/
                if (sym_ptr->type == data_entry_sym || sym_ptr->type == inst_entry_sym)
                {
                    ENTRY_NODE *new_entry = malloc(sizeof(ENTRY_NODE));
                    if (new_entry == NULL)
                    {
                        printf("Error in: %s , memory allocation failed\n", am_file_name);
                        return MEMORY_ALLOCATION_ERROR;
                    }

                    new_entry->symbol = sym_ptr;
                    new_entry->next = curr_program->entries_list;
                    curr_program->entries_list = new_entry;
                    curr_program->entries_counter++;
                }

                sym_ptr = sym_ptr->next;
            }
        }
    }

    if (err_flag == NO_ERRORS)
    {
        return NO_ERRORS;
    }

    else {
        return SYNTAX_OR_LOGIC_ERROR;
    }
}

