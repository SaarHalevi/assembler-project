#ifndef FRONT_END_H
#define FRONT_END_H

#include "general.h"

#define MAX_ERR_DETAIL_LEN 175 /*Details about the error type will be a maximum of 100 characters*/
#define MAX_INST_OPERANDS 2
#define NON_VALID_NUM 3000 /*3000 cannot be represented in the 2's complement method by 12 bits*/
#define MAX_DATA_OPERANDS 50

typedef struct INST_OPERAND INST_OPERAND;
typedef struct DATA DATA;
typedef struct LINE_AST LINE_AST;
typedef struct DIRECTIVE DIRECTIVE;
typedef struct INSTRUCTION INSTRUCTION;
typedef struct CONSTANT_DEFINITION CONSTANT_DEFINITION;

struct INST_OPERAND
{
    enum
    {
        none,
        number,
        constant,
        reg,
        label,
        label_with_index
    } type;

    int num;
    char label[MAX_LABEL_LEN];
    char constant_name[MAX_LABEL_LEN];
};

struct DATA
{
    enum
    {
        data_int,
        data_constant
    } data_type;

    union
    {
        int num;
        char constant_name[MAX_LABEL_LEN];
    } data_value;
};

struct DIRECTIVE
{
    enum
    {
        data,
        string,
        entry,
        ext
    } type;

    int operand_counter;

    union operands
    {
        DATA data[MAX_DATA_OPERANDS];      /* for data directive */
        int str[MAX_LABEL_LEN];            /* store the asci code of the strings chars */
        char label_operand[MAX_LABEL_LEN]; /* Store the label name of the extern/entry */

    } operands;
};

struct INSTRUCTION
{
    enum
    {
        mov,
        cmp,
        add,
        sub,
        not,
        clr,
        lea,
        inc,
        dec,
        jmp,
        bne,
        red,
        prn,
        jsr,
        rts,
        hlt
    } type;

    INST_OPERAND operands[MAX_INST_OPERANDS];
};

struct CONSTANT_DEFINITION
{
    char constant_name[MAX_LABEL_LEN];
    int constant_num;
};

struct LINE_AST
{
    char error_detail[MAX_ERR_DETAIL_LEN + 1];
    char label[MAX_LABEL_LEN + 1];

    enum
    {
        error,
        empty,
        note,
        dir,
        inst,
        constant_def
    } type;

    union operand
    {
        DIRECTIVE directive;
        INSTRUCTION instruction;
        CONSTANT_DEFINITION constant;
    } operand;
};

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
 */

int is_label(char *word, int label_position);

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
 */

int is_label_with_index(char *word, INST_OPERAND *operand);

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
 */

int is_register(char *word);

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
 */

int is_valid_num(char *word);

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
 */

int check_line_type(char **line, LINE_AST *ast);

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
 */

int check_dir_operand(char **line, LINE_AST *ast);

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
 */

int check_inst_operand(char **line, LINE_AST *ast);

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
 */

int check_constant_def_operand(char **line, LINE_AST *ast);

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
 */

int check_operand(char **line, LINE_AST *ast);

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
 */

LINE_AST create_ast_from_text(char *line);

#endif

