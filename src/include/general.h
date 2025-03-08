#ifndef GENERAL_H
#define GENERAL_H

#define STANDARD_WORD_SIZE 5
#define MAX_LINE_LEN 82
#define MAX_LABEL_LEN 32        /*31 places for the maximum length of a label and one more for '\0' */
#define HASH_TABLE_SIZE 19      /*Prime number to reduce collisions in the hash function*/
#define MAX_PROGRAM_LENGTH 3996 /*The maximum program length is 4096, but the first 100 memory cells are unused*/
#define DESTINATION_OPERAND 1
#define SOURCE_OPERAND 0
#define MEMORY_ALLOCATION_ERROR -2
#define NO_ERRORS 1
#define SYNTAX_OR_LOGIC_ERROR 0

typedef struct TRANSLATION_UNIT TRANSLATION_UNIT;

typedef struct SYMBOL SYMBOL;

typedef struct EXT_SYMBOL EXT_SYMBOL;

typedef struct EXT_ADDRESS EXT_ADDRESS;

typedef struct ENTRY_NODE ENTRY_NODE;

struct SYMBOL
{
    char name[MAX_LABEL_LEN];

    enum
    {
        extern_sym,
        entry_sym_without_definition,
        data_entry_sym,
        inst_entry_sym,
        data_sym,
        inst_sym,
        const_sym
    } type;

    int address;
    int value; /*in case of constant*/

    struct SYMBOL *next;
};

struct EXT_SYMBOL
{
    char ext_name[MAX_LABEL_LEN];
    EXT_ADDRESS *address; /*A pointer to a linked list containing all the addresses that the label is in*/
    struct EXT_SYMBOL *next_ext;
};

struct EXT_ADDRESS
{
    int num;
    struct EXT_ADDRESS *next_address;
};

struct ENTRY_NODE
{
    SYMBOL *symbol;
    struct ENTRY_NODE *next;
};

struct TRANSLATION_UNIT
{
    short instruction_array[MAX_PROGRAM_LENGTH];
    short data_array[MAX_PROGRAM_LENGTH];
    int ic;
    int dc;
    SYMBOL *symbol_table[HASH_TABLE_SIZE];
    EXT_SYMBOL *ext_list;
    int external_counter;
    ENTRY_NODE *entries_list;
    int entries_counter;
};

/*
 * Function: free_ext_list
 * -----------------------------------
 * Description:
 *   Frees the memory allocated for the linked list of external symbols.
 * Parameters:
 *   - head: Pointer to the head of the linked list of external symbols.
 * Returns: None
 */

void free_ext_list(EXT_SYMBOL **head);

/*
 * Function: free_entries_list
 * -----------------------------------
 * Description:
 *   Frees the memory allocated for a linked list of entry symbol pointers.
 * Parameters:
 *   - head: A pointer to the head of the linked list of entry nodes.
 * Returns: None
 */

void free_entries_list(ENTRY_NODE *head);

/*
 * Function: free_symbol_table
 * -----------------------------------
 * Description:
 *   Frees the symbol table and initializes it to NULL.
 * Parameters:
 *   - symbol_table: The symbol table for which the allocated memory should be freed.
 * Returns: None
 */

void free_symbol_table(SYMBOL **symbol_table);

/*
 * Function: dynamic_strcat
 * -----------------------------------
 * Description:
 *   Concatenates two strings dynamically and returns the result.
 * Parameters:
 *   - first_string: The original string.
 *   - second_string: The string to add to the original.
 * Returns:
 *   - The connected string if memory allocation was successful, NULL otherwise.
 */

char *dynamic_strcat(char *first_string, char *second_string);

/*
 * Function: symbol_lookup
 * -----------------------------------
 * Description:
 *   Looks up a node in a hash table based on the provided name.
 * Parameters:
 *   - name: The name to be looked up in the hash table.
 *   - hash_table: An array of pointers to nodes representing the hash table.
 * Returns:
 *   - A pointer to the found node if it exists, or NULL if the node is not found.
 */
SYMBOL *symbol_lookup(char *name, SYMBOL *hash_table[HASH_TABLE_SIZE]); /*need to change to generic*/

/*
 * Function: hash
 * -----------------------------------
 * Description:
 *   Allocates for a name, index in the hash table.
 * Parameters:
 *   - name: The name for which to calculate the hash value.
 * Returns:
 *   - The index in the hash table.
 */

unsigned int hash(char *word);

/*
 * Function: get_word
 * -----------------------------------
 * Description:
 *   Skips white characters and returns the next word in the string.
 * Parameters:
 *   - line: The string from which the words will be extracted.
 * Returns:
 *   - The next word in the string, or NULL in case of end of string or memory allocation error.
 */

char *get_word(char **line);

/*
 * Function: is_directive
 * -----------------------------------
 * Description:
 *   Checks if a given word represents a directive and returns its index.
 * Parameters:
 *   - word: A string representing a potential directive.
 * Returns:
 *   - The index of the directive if found, or -1 otherwise.
 */

int is_directive(char *word);

/*
 * Function: is_operation
 * -----------------------------------
 * Description:
 *   Checks if a given word represents an operation and returns its index.
 * Parameters:
 *   - word: A string representing a potential assembly operation mnemonic.
 * Returns:
 *   - The index of the operation if found, or -1 otherwise.
 */

int is_operation(char *word);

#endif
