/*
 * File: front_end.c
 * -----------------
 * Description:
 *   This file implements functions related to the front-end phase of the assembly
 *   language processing. These functions parse assembly code, check syntax, and
 *   create abstract syntax trees (ASTs) for further processing.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "include/front_end.h"

#define MIN_12_BIT_VALUE -2048
#define MAX_12_BIT_VALUE 2047

enum
{
    pre_line_label,
    operand_label
} type_of_label;

/**
 * Function: is_label
 * -----------------------------
 * Description:
 * checks whether a word qualifies as a label based on certain criteria:
 *
 * Parameters:
 *   - word: The word to be checked for label validity.
 *   - label_position: The position of the label in the line (pre_line_label or post_line_label).
 *
 * Returns:
 *   - NO_ERRORS: If the word is a valid label.
 *   - SYNTAX_OR_LOGIC_ERROR: If the word does not meet the label criteria.
 *   - MEMORY_ALLOCATION_ERROR: If memory allocation fails.
 *
 * Algorithm:
 *   1. Determine the length of the word without the colon suffix (if present).
 *   2. Check if the word meets label criteria:
 *      - The first character must be an alphabetic character.
 *      - All characters except the last one (if it's a colon) must be alphanumeric.
 *      - The length of the word without the colon suffix (if present) must not exceed MAX_LABEL_LEN.
 *   3. Validate the alphanumeric characters in the word.
 *   4. Allocate memory for the word without the colon suffix.
 *   5. Copy the word without the colon suffix.
 *   6. Check if the word matches any reserved keywords (registers, directives, or operations).
 */

int is_label(char *word, int label_position)
{

    int i;
    int word_length_without_suffix;
    char *word_without_suffix;

    if (label_position == pre_line_label)
    {
        word_length_without_suffix = strlen(word) - 1;
    }

    else
    {
        word_length_without_suffix = strlen(word);
    }

    if ((word_length_without_suffix > MAX_LABEL_LEN) || (isalpha(word[0]) == 0) || ((word[word_length_without_suffix] != ':') && (label_position == pre_line_label)))
    {
        return SYNTAX_OR_LOGIC_ERROR;
    }

    for (i = 0; i < word_length_without_suffix; i++)
    {
        if ((isalpha(word[i]) == 0) && (isdigit(word[i]) == 0))
        {
            return SYNTAX_OR_LOGIC_ERROR;
        }
    }

    word_without_suffix = (char *)malloc((word_length_without_suffix + 1) * sizeof(char));
    if (word_without_suffix == NULL)
    {
        return MEMORY_ALLOCATION_ERROR;
    }

    if (label_position == pre_line_label)
    {
        strncpy(word_without_suffix, word, word_length_without_suffix);
    }

    else
    {
        strcpy(word_without_suffix, word);
    }

    if ((is_register(word_without_suffix) != -1) || (is_directive(word_without_suffix) != -1) || (is_operation(word_without_suffix) != -1))
    {
        free(word_without_suffix);
        return SYNTAX_OR_LOGIC_ERROR;
    }

    free(word_without_suffix);
    return NO_ERRORS;
}

/**
 * Check if a given word is a valid label with an index.
 *
 * This function checks whether a word qualifies as a label with an index based on certain criteria:
 *  - The word must contain '[' and ']' characters to delineate the index.
 *  - The label part before '[' must meet label criteria.
 *  - The index part within '[' and ']' must be either a valid number or a valid label.
 *  - The operand structure is updated with the label and index information.
 *
 * Parameters:
 *   - word: The word to be checked for label with index validity.
 *   - operand: Pointer to the operand structure to be updated.
 *
 * Returns:
 *   - NO_ERRORS: If the word is a valid label with an index.
 *   - SYNTAX_OR_LOGIC_ERROR: If the word does not meet the criteria for a label with an index.
 *   - MEMORY_ALLOCATION_ERROR: If memory allocation fails.
 *
 * Algorithm:
 *   1. Find the '[' character in the word.
 *   2. Extract the label part before '[' and validate it as a label.
 *   3. If label validation fails, return SYNTAX_OR_LOGIC_ERROR.
 *   4. Extract the index part between '[' and ']' and validate it as either a number or a label.
 *   5. If index validation fails, return SYNTAX_OR_LOGIC_ERROR.
 *   6. Update the operand structure with the label and index information.
 */

int is_label_with_index(char *word, INST_OPERAND *operand)
{
    char *label_part;
    char *index_part;
    int label_result, index_result;
    char *index_char;
    char *closing_bracket_char;

    index_char = strchr(word, '[');
    if (index_char == NULL || index_char == word)
    {
        return SYNTAX_OR_LOGIC_ERROR;
    }

    label_part = (char *)malloc((index_char - word + 1) * sizeof(char));
    if (label_part == NULL)
    {

        return MEMORY_ALLOCATION_ERROR;
    }

    strncpy(label_part, word, index_char - word);
    label_part[index_char - word] = '\0';

    label_result = is_label(label_part, operand_label);

    if (label_result == SYNTAX_OR_LOGIC_ERROR)
    {
        free(label_part);
        return SYNTAX_OR_LOGIC_ERROR;
    }

    else if (label_result == MEMORY_ALLOCATION_ERROR)
    {
        free(label_part);
        return MEMORY_ALLOCATION_ERROR;
    }

    else
    {
        strcpy(operand->label, label_part);
        free(label_part);
    }

    index_char++;

    closing_bracket_char = strchr(index_char, ']');

    if (closing_bracket_char == NULL || closing_bracket_char[1] != '\0')
    {
        return SYNTAX_OR_LOGIC_ERROR;
    }

    index_part = (char *)malloc((closing_bracket_char - index_char + 1) * sizeof(char));
    if (index_part == NULL)
    {
        return MEMORY_ALLOCATION_ERROR;
    }

    strncpy(index_part, index_char, closing_bracket_char - index_char);
    index_part[closing_bracket_char - index_char] = '\0';

    if ((index_result = is_valid_num(index_part)) != NON_VALID_NUM)
    {
        operand->num = index_result;
        free(index_part);
        return NO_ERRORS;
    }

    else
    {
        index_result = is_label(index_part, operand_label);

        if (index_result == NO_ERRORS)
        {
            strcpy(operand->constant_name, index_part);
            free(index_part);
            return NO_ERRORS;
        }

        else if (index_result == SYNTAX_OR_LOGIC_ERROR)
        {
            free(index_part);
            return SYNTAX_OR_LOGIC_ERROR;
        }

        else
        {
            free(index_part);
            return MEMORY_ALLOCATION_ERROR;
        }
    }
}

/*
 * Function: is_register
 * ----------------------
 * Description:
 *   Check if a given word represents a register and if so return its index.
 *
 * Parameters:
 *   word: The word to be checked.
 *
 * Returns:
 *   i -  the index of the register
 *  -1 -  if the word does not represent a register.
 *
 * Algorithm:
 *   1. Compare the word against a list of valid register names.
 */

int is_register(char *word)
{
    int i;
    char *reg[10] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "PSW", "PC"};

    for (i = 0; i < 10; i++)
    {
        if (strcmp(word, reg[i]) == 0)
        {
            return i;
        }
    }

    return -1;
}

/*
 * Function: is_valid_num
 * ----------------------
 * Description:
 *   Checks if the given string represents a valid number within the range
 *   that can be represented by 12 bits in two's complement notation.
 *
 * Parameters:
 *   word: A pointer to the string containing the number to be validated.
 *
 * Returns:
 *   If the number is valid and within the range of 12 bits in two's complement notation,
 *   it returns the integer representation of the number. If the number is invalid
 *   or outside the valid range, it returns NON_VALID_NUM.
 *
 * Algorithm:
 *   1. Check if the length of the string exceeds 5 characters. If so, return NON_VALID_NUM.
 *   2. Use the strtol function to convert the string to a long integer.
 *   3. Check if any invalid characters were encountered during conversion. If so, return NON_VALID_NUM.
 *   4. Check if the converted number falls within the valid range for a 12-bit two's complement number.
 *   5. If the number is within the valid range, return the integer representation of the number.
 *      Otherwise, return NON_VALID_NUM.
 */

int is_valid_num(char *word)
{
    long converted_num;
    char *end_ptr;

    if (strlen(word) > 5)
    {
        return NON_VALID_NUM;
    }

    converted_num = strtol(word, &end_ptr, 10);
    if (*end_ptr != '\0')
    {
        return NON_VALID_NUM;
    }

    else
    {
        if (converted_num >= MIN_12_BIT_VALUE && converted_num <= MAX_12_BIT_VALUE)
        {
            return (int)converted_num;
        }

        else
        {
            return NON_VALID_NUM;
        }
    }
}

/*
 * Function: check_line_type
 * -------------------------
 * Description:
 *   Determines the type of a given line in the assembly code and updates the Abstract Syntax Tree (AST) accordingly.
 *   It identifies labels, directives, instructions, or constant definitions, and handles various error conditions.
 *
 * Parameters:
 *   line: A pointer to a pointer to the string representing the line of assembly code.
 *   ast: A pointer to the Abstract Syntax Tree (AST) structure to update with the line type and details.
 *
 * Returns:
 *   An integer representing the type of the line:
 *     - note: If the line is a comment.
 *     - dir: If the line is a directive.
 *     - inst: If the line is an instruction.
 *     - constant_def: If the line is defining a constant.
 *     - empty: If the line is empty.
 *     - error: If an error occurred while processing the line.
 *
 * Algorithm:
 *   1. Check if the line is a comment by examining if it starts with a semicolon. If true, return note.
 *   2. Tokenize the line into words and iterate over each word.
 *   3. For each word:
 *      a. Check if it qualifies as a label. If so:
 *         - If it's the first word, update the AST with the label.
 *         - Otherwise, report an error for an invalid label placement.
 *      b. Check if it matches any directive. If found, update the AST with the directive type.
 *      c. Check if it matches any instruction. If found, update the AST with the instruction type.
 *      d. Check if it's ".define" indicating a constant definition. If true, return constant_def.
 *      e. If none of the above conditions match, report an error for an invalid or unrecognized word.
 *   4. After processing the first word:
 *      - Return error if memory allocation fails.
 *      - If it's the end of the line, return empty.
 *      - If the line contains only a label name, report an error.
 *   5. If none of the above conditions match, return error.
 */

int check_line_type(char **line, LINE_AST *ast)
{
    char *word;
    int word_cnt = 0;
    int dir_type = 0;
    int ins_type = 0;
    int is_label_res;

    if ((*line)[0] == ';')
    {
        return note;
    }

    else
    {
        word = get_word(line); /*get_word function skip white chars and take the next word*/

        while (word)
        {
            word_cnt++;

            if ((is_label_res = is_label(word, pre_line_label)) == NO_ERRORS)
            {
                if (word_cnt == 1)
                {
                    strncpy(ast->label, word, strlen(word) - 1); /*the character ';' It is not part of the label*/
                    ast->label[strlen(word) - 1] = '\0';
                }

                else
                {

                    free(word);
                    strcpy(ast->error_detail, "a label is in an invalid place");
                    return error;
                }
            }

            else if (is_label_res == MEMORY_ALLOCATION_ERROR)
            {
                free(word);
                strcpy(ast->error_detail, "memory allocation failed");
                return error;
            }

            else if ((dir_type = is_directive(word)) != -1)
            {
                ast->operand.directive.type = dir_type;
                free(word);
                return dir;
            }

            else if ((ins_type = is_operation(word)) != -1)
            {
                ast->operand.instruction.type = ins_type;
                free(word);
                return inst;
            }

            else if (strcmp(".define", word) == 0)
            {
                free(word);
                if (word_cnt == 1)
                {
                    return constant_def;
                }

                else
                {
                    strcpy(ast->error_detail, "a label must not be defined in a constant definition line");
                    return error;
                }
            }

            else
            {
                if (word_cnt == 1)
                {
                    strcpy(ast->error_detail, "the first word must be an instruction or directive or .define or label name");
                }

                else
                {
                    strcpy(ast->error_detail, "after defining a label there must be an instruction  or directive");
                }

                free(word);
                return error;
            }

            free(word);
            word = get_word(line);
        }

        if ((*line)[0] == '\0')
        {
            if (word_cnt == 0)
            {
                return empty;
            }

            else /*word_cnt = 1*/
            {
                strcpy(ast->error_detail, "the line contains only label name");
                return error;
            }
        }

        else
        {
            strcpy(ast->error_detail, "memory allocation failed");
            return error;
        }
    }
}

/*
 * Function: check_dir_operand
 * ----------------------------
 * Description:
 *   Checks the validity of the operand for directive statements in the assembly language.
 *   Handles different types of directives such as entry, extern, string, and data.
 *   Updates the AST (Abstract Syntax Tree) with the operand details if valid.
 *
 * Parameters:
 *   - line: A pointer to the current line in the assembly code.
 *   - ast: A pointer to the Abstract Syntax Tree structure to update with operand details.
 *
 * Returns:
 *   - NO_ERROR: If the operand is valid for the directive.
 *   - error: If there's any syntax error or memory allocation failure.
 *
 * Algorithm:
 *   1. Check if the line is empty. If so, return an error indicating that a directive word must be followed by an operand.
 *   2. If the directive type is entry or ext:
 *      - Check if the operand is a valid label. If valid, update the AST with the label operand.
 *   3. If the directive type is string:
 *      - Ensure that the operand starts with a quotation mark.
 *      - Parse the string and check for proper format and content. Update the AST with the string operand.
 *   4. If the directive type is data:
 *      - Iterate over each character in the line.
 *      - If the character is a comma, ensure it's not duplicated between two numbers.
 *      - Parse each word as either an integer or a label. Update the AST with the data operand accordingly.
 *   5. Handle various error conditions such as memory allocation failure, invalid characters, or improper formatting.

 */

int check_dir_operand(char **line, LINE_AST *ast)
{
    char *word;
    int word_len;
    int comma_cnt = 0;
    int curr_num;
    int i;
    int op_cnt = 0;
    int is_label_res;

    if ((**line) == '\0')
    {
        strcpy(ast->error_detail, "a directive word must be followed by an operand");
        return error;
    }

    if ((ast->operand.directive.type == entry) || (ast->operand.directive.type == ext))
    {
        word = get_word(line);

        if (word == NULL)
        {
            strcpy(ast->error_detail, "memory allocation failed");
            return error;
        }

        else if (is_label(word, operand_label) == MEMORY_ALLOCATION_ERROR)
        {
            strcpy(ast->error_detail, "memory allocation failed");
            free(word);
            return error;
        }
        else if (is_label(word, operand_label) == SYNTAX_OR_LOGIC_ERROR)
        {
            strcpy(ast->error_detail, "an operand of entry and extern must be a proper name of a label");
            free(word);
            return error;
        }

        else
        {
            strcpy(ast->operand.directive.operands.label_operand, word);
            free(word);
            return NO_ERRORS;
        }
    }

    else if (ast->operand.directive.type == string)
    {

        if ((*line)[0] != '"')
        {
            strcpy(ast->error_detail, "after the string directive the operand must start with the character \" ");
            return error;
        }

        else
        {
            (*line)++;
            word = get_word(line);
            if (word == NULL)
            {
                if ((**line) == '\0')
                {
                    strcpy(ast->error_detail, "a string directive must have at least one character after the quotation marks ");
                }

                else
                {
                    strcpy(ast->error_detail, "memory allocation failed");
                }

                return error;
            }

            word_len = strlen(word);

            if (word[word_len - 1] != '"')
            {
                strcpy(ast->error_detail, "in the operand of the directive string there is no closing hyphen");
                free(word);
                return error;
            }

            for (i = 0; i < word_len - 1; i++)
            {
                if (isprint(word[i]))
                {
                    ast->operand.directive.operands.str[i] = (int)word[i];
                }

                else
                {
                    strcpy(ast->error_detail, "the operand of the string directive must include only alphabetic letters between the 2 hyphenes");
                    free(word);
                    return error;
                }
            }
            free(word);
            return NO_ERRORS;
        }
    }

    else /*directive is .data*/
    {

        while (((**line) != '\0') && ((**line) != EOF))
        {
            if (isspace(**line))
            {
                (*line)++;
            }

            else if ((**line) == ',')
            {
                if (comma_cnt > 0)
                {
                    strcpy(ast->error_detail, "there are 2 commas between a number and another number");
                    return error;
                }

                else
                {
                    comma_cnt++;
                    (*line)++;
                }
            }

            else
            {
                word = get_word(line);
                if (word == NULL)
                {
                    strcpy(ast->error_detail, "memory allocation failed");
                    return error;
                }

                else if ((curr_num = is_valid_num(word)) != NON_VALID_NUM)
                {
                    op_cnt++;
                    ast->operand.directive.operands.data[op_cnt - 1].data_type = data_int;
                    ast->operand.directive.operands.data[op_cnt - 1].data_value.num = curr_num;
                    ast->operand.directive.operand_counter = op_cnt;
                }

                else if ((is_label_res = is_label(word, operand_label)) == NO_ERRORS)
                {
                    op_cnt++;
                    ast->operand.directive.operands.data[op_cnt - 1].data_type = data_constant;
                    strcpy(ast->operand.directive.operands.data[op_cnt - 1].data_value.constant_name, word);
                    ast->operand.directive.operand_counter = op_cnt;
                }

                else if (is_label_res == MEMORY_ALLOCATION_ERROR)
                {
                    strcpy(ast->error_detail, "memory allocation failed");
                    free(word);
                    return error;
                }

                else
                {
                    strcpy(ast->error_detail, "for the data directive, you can only enter integers that can be represented in 12 bits by the 2's complement method or or words that meet the syntax requirements of a label");
                    free(word);
                    return error;
                }

                free(word);
                comma_cnt = 0;
            }
        }

        if (comma_cnt != 0)
        {
            strcpy(ast->error_detail, "there is a comma after the last number");
            return error;
        }

        return NO_ERRORS;
    }
}

/*
 * Function: check_inst_operand
 * ----------------------------
 * Description: Validates the operands of an instruction based on its opcode type and updates the LINE_AST structure.
 *              Handles memory allocation, operand validation, and error reporting.
 *
 * Parameters:
 *      line: A pointer to a pointer to the current line being processed in the input file.
 *      ast:  A pointer to the LINE_AST structure representing the current line's abstract syntax tree.
 *
 * Returns:
 *      - NO_ERRORS: If operand validation succeeds without errors.
 *      - error:     If errors occur during operand validation, such as memory allocation failure or invalid operands and ect.
 *
 * Algorithm:
 *      1. Determine the number of operands expected by the instruction's opcode type.
 *      2. Iterate over the expected operands and validate each:
 *          - Handle numeric constants or labels prefixed with '#'.
 *          - Process labels or labels with indices.
 *          - Identify registers.
 *      3. Check for validity and appropriateness of operands based on the instruction type.
 *      4. Report any errors encountered during operand validation and return the appropriate error code.
 */

int check_inst_operand(char **line, LINE_AST *ast)
{
    char *word;
    int converted_num1;
    int reg_num;
    int operand_cnt;
    int comma_flag = 0;
    int label_result;      /*store the result of the value return from thr function is_label*/
    int res_label_w_index; /*store the result of the value return from thr function is_label_with_index*/

    /*Initialize opernd_cnt according to the amount of operands that opcode should receive*/

    operand_cnt = (ast->operand.instruction.type >= rts) ? MAX_INST_OPERANDS : ((ast->operand.instruction.type > sub && ast->operand.instruction.type < rts && ast->operand.instruction.type != lea) ? DESTINATION_OPERAND : SOURCE_OPERAND);

    for (; operand_cnt < MAX_INST_OPERANDS; operand_cnt++)
    {
        word = get_word(line);
        if (word == NULL)
        {
            if ((**line) == '\0')
            {
                strcpy(ast->error_detail, "missing operand");
                return error;
            }

            else
            {
                strcpy(ast->error_detail, "memory allocation failed");
                return error;
            }
        }

        else if (word[0] == '#')
        {
            if (((operand_cnt == DESTINATION_OPERAND) && (ast->operand.instruction.type != cmp) && (ast->operand.instruction.type != prn)) ||
                ((operand_cnt == SOURCE_OPERAND) && (ast->operand.instruction.type == lea)))
            {
                strcpy(ast->error_detail, "the operation type received an operand of an inappropriate type");
                free(word);
                return error;
            }

            else if (strlen(word) == 1)
            {
                strcpy(ast->error_detail, "# must be followed by a number or constant");
                free(word);
                return error;
            }

            /*The opernad is number*/
            else if ((converted_num1 = is_valid_num(word + 1)) != NON_VALID_NUM)
            {
                ast->operand.instruction.operands[operand_cnt].type = number;
                ast->operand.instruction.operands[operand_cnt].num = converted_num1;
            }

            /*The operand is constant*/
            else if ((label_result = is_label((word + 1), operand_label)) != SYNTAX_OR_LOGIC_ERROR)
            {
                if (label_result == MEMORY_ALLOCATION_ERROR)
                {
                    strcpy(ast->error_detail, "memory allocation failed");
                    free(word);
                    return error;
                }

                else
                {
                    ast->operand.instruction.operands[operand_cnt].type = constant;
                    strcpy(ast->operand.instruction.operands[operand_cnt].constant_name, (word + 1));
                }
            }

            else
            {
                strcpy(ast->error_detail, "# must be followed by a constant or a number");
                free(word);
                return error;
            }
        }

        else if ((label_result = is_label(word, operand_label)) != SYNTAX_OR_LOGIC_ERROR)
        {
            if (label_result == MEMORY_ALLOCATION_ERROR)
            {
                strcpy(ast->error_detail, "memory allocation failed");
                free(word);
                return error;
            }

            else
            {
                ast->operand.instruction.operands[operand_cnt].type = label;
                strcpy(ast->operand.instruction.operands[operand_cnt].label, word);
            }
        }

        else if ((res_label_w_index = is_label_with_index(word, &(ast->operand.instruction.operands[operand_cnt]))) != SYNTAX_OR_LOGIC_ERROR)
        {
            if (res_label_w_index == MEMORY_ALLOCATION_ERROR)
            {
                strcpy(ast->error_detail, "memory allocation failed");
                free(word);
                return error;
            }

            else if ((operand_cnt == DESTINATION_OPERAND) && ((ast->operand.instruction.type == jmp) || (ast->operand.instruction.type == bne) || (ast->operand.instruction.type == jsr)))
            {
                free(word);
                strcpy(ast->error_detail, "the operation type received an operand of an inappropriate type");
                return error;
            }

            else
            {
                /*The "is_label_with_index" function is responsible for updating the data inside the  ast*/
                ast->operand.instruction.operands[operand_cnt].type = label_with_index;
            }
        }

        else if ((reg_num = is_register(word)) != -1)
        {
            if ((operand_cnt == SOURCE_OPERAND) && (ast->operand.instruction.type == lea))
            {
                strcpy(ast->error_detail, "the operation type received an operand of an inappropriate type");
                free(word);
                return error;
            }
            else
            {

                ast->operand.instruction.operands[operand_cnt].type = reg;
                ast->operand.instruction.operands[operand_cnt].num = reg_num;
            }
        }

        else
        {
            free(word);
            strcpy(ast->error_detail, "the operation type received an operand of an inappropriate type");
            return error;
        }

        free(word);

        if (operand_cnt == SOURCE_OPERAND)
        {
            while (isspace(**line) || (**line) == ',')
            {
                if ((**line) == ',')
                {
                    if (comma_flag > 0)
                    {
                        strcpy(ast->error_detail, "multiple commas between 2 operands");
                        return error;
                    }

                    else
                    {
                        comma_flag++;
                    }
                }

                (*line)++;
            }

            if ((**line) == '\0')
            {
                strcpy(ast->error_detail, "missing operand");
                return error;
            }
        }
    }

    return NO_ERRORS;
}

/*
 * Function: check_constant_def_operand
 * -------------------------------------
 * Description:
 *   Checks and processes operands for constant definition lines in the assembly code.
 *
 * Parameters:
 *   line: Pointer to the line of assembly code.
 *   ast: Pointer to the abstract syntax tree (AST) structure to be updated.
 *
 * Returns:
 *   NO_ERRORS if the constant definition operands are successfully parsed,
 *   error if there is an error parsing the constant definition operands or memory allocation failed.
 *
 * Algorithm:
 *   1. Tokenize the line to separate the constant definition and its operands.
 *   2. Extract the constant name from the token.
 *   3. Extract the constant value from the token.
 *   4. Update the AST structure with the constant name and value.
 *   5. Return the appropriate result.
 */

int check_constant_def_operand(char **line, LINE_AST *ast)
{
    char *word;
    int converted_num;
    int is_label_res;

    if ((**line) == '\0')
    {
        strcpy(ast->error_detail, "a constant definition is missing after the word define");
        return error;
    }

    word = get_word(line);
    if (word == NULL)
    {
        strcpy(ast->error_detail, "memory allocation failed");
        return error;
    }

    else if ((is_label_res = is_label(word, operand_label)) == SYNTAX_OR_LOGIC_ERROR)
    {
        strcpy(ast->error_detail, "the first word after .define does not follow the syntax rules for a label");
        free(word);
        return error;
    }

    else if (is_label_res == MEMORY_ALLOCATION_ERROR)
    {
        strcpy(ast->error_detail, "memory allocation failed");
        free(word);
        return error;
    }

    else
    {
        strcpy(ast->operand.constant.constant_name, word);
        free(word);
    }

    while (isspace(**line))
    {
        (*line)++;
    }

    if ((**line) != '=')
    {
        strcpy(ast->error_detail, "missing the equality sign in a constant definition statment");
        return error;
    }

    (*line)++;

    while (isspace(**line))
    {
        (*line)++;
    }

    if ((**line) == '\0')
    {
        strcpy(ast->error_detail, "missing a  number in a constant definition statement");
        return error;
    }

    word = get_word(line);

    if (word == NULL)
    {
        strcpy(ast->error_detail, "memory allocation failed");
        return error;
    }

    if ((converted_num = is_valid_num(word)) == NON_VALID_NUM)
    {
        strcpy(ast->error_detail, "a no valid number is given in a constant definition statement");
        free(word);
        return error;
    }

    ast->operand.constant.constant_num = converted_num;
    free(word);
    return NO_ERRORS;
}

/*
 * Function: check_operand
 * ------------------------
 * Description:
 *   Checks and processes an operand in the assembly code.
 *
 * Parameters:
 *   line: Pointer to the line of assembly code.
 *   ast: Pointer to the abstract syntax tree (AST) structure to be updated.
 *
 * Returns:
 *   syntax_flag - indicate if there is an error during the proccess
 *   - NO_ERRORS: Operation successful, operands are valid.
 *   - error: Error occurred, operands are invalid or memory allocation failed.
 *
 * Algorithm:
 *   1. Trim leading and trailing whitespace from the operand.
 *   2. Check if the operand is a label with an index.
 *   3. Check if the operand is a register or a valid number.
 *   4. Update the AST structure with the operand information.
 *   5. Return the appropriate result.
 */

int check_operand(char **line, LINE_AST *ast)
{
    int syntax_flag = ast->type;

    /* Skip leading whitespace */
    while (**line && isspace(**line))
    {
        (*line)++;
    }

    if ((**line) == ',')
    {
        strcpy(ast->error_detail, "there is a comma, after an instruction/directive/define");
        return error;
    }

    switch (ast->type)
    {
    case error:
    case empty:
    case note:
        return syntax_flag;
        break;

    case dir:
        syntax_flag = check_dir_operand(line, ast);
        break;

    case inst:
        syntax_flag = check_inst_operand(line, ast);
        break;

    case constant_def:
        syntax_flag = check_constant_def_operand(line, ast);
        break;
    }

    if (syntax_flag == error)
    {
        return error;
    }

    while (**line && isspace(**line))
    {
        (*line)++;
    }

    if ((**line) != '\0')
    {
        strcpy(ast->error_detail, "unexpected characters after operands");
        syntax_flag = error;
    }

    return syntax_flag;
}

/*
 * Function: create_ast_from_text
 * ------------------------------
 * Description:
 *   Creates an abstract syntax tree (AST) for a line of assembly code.
 *
 * Parameters:
 *   line: The line of assembly code.
 *
 * Returns:
 *   An abstract syntax tree (AST) structure representing the line of assembly code.
 *
 * Algorithm:
 *   1. Initialize the syntax flag.
 *   2. Create an empty AST structure.
 *   3. Check the type of line and parse it accordingly.
 *   4. Check the operands of the line.
 *   5. If there is an error in the operands, set the AST type to error.
 *   6. Return the AST structure.
 */

LINE_AST create_ast_from_text(char *line)
{
    int syntax_flag = -1;
    LINE_AST ast = {0};

    ast.type = check_line_type(&line, &ast);
    if (strcmp(ast.error_detail, "memory allocation failed") == 0)
    {
        return ast;
    }

    syntax_flag = check_operand(&line, &ast);

    if (syntax_flag == error)
    {
        ast.type = error;
    }

    return ast;
}

