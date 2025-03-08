#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>

#include "include/general.h"

/*
 * Function: free_ext_list
 * -----------------------------------
 * Description:
 *   Frees the memory allocated for the linked list of external symbols.
 * Parameters:
 *   - head: Pointer to the head of the linked list of external symbols.
 * Returns: None
 * Algorithm:
 *   - Iterate through the linked list of external symbols.
 *   - For each symbol, iterate through its associated addresses and free them.
 *   - Free the external symbol node itself.
 */

void free_ext_list(EXT_SYMBOL **head)
{
    EXT_SYMBOL *current = *head;
    EXT_SYMBOL *temp;
    EXT_ADDRESS *address_current;
    EXT_ADDRESS *address_temp;

    while (current != NULL)
    {
        temp = current;
        current = current->next_ext;

        /*Free the linked list of addresses for each external symbol*/
        address_current = temp->address;
        while (address_current != NULL)
        {
            address_temp = address_current;
            address_current = address_current->next_address;
            free(address_temp);
        }

        temp->address = NULL;
        /*Free the external symbol node*/
        free(temp);
    }
}

/*
 * Function: free_entries_list
 * -----------------------------------
 * Description:
 *   Frees the memory allocated for a linked list of entry symbol pointers.
 * Parameters:
 *   - head: A pointer to the head of the linked list of entry nodes.
 * Returns: None
 * Algorithm:
 *   - Iterate through the linked list of entry nodes.
 *   - Free each node.
 */

void free_entries_list(ENTRY_NODE *head)
{
    ENTRY_NODE *temp;
    while (head != NULL)
    {
        temp = head;
        head = head->next;
        free(temp);
    }
}

/*
 * Function: free_symbol_table
 * -----------------------------------
 * Description:
 *   Frees the symbol table and initializes it to NULL.
 * Parameters:
 *   - symbol_table: The symbol table for which the allocated memory should be freed.
 * Returns: None
 * Algorithm:
 *   - Iterate through each hash table entry.
 *   - For each entry, iterate through the linked list of symbols and free each node.
 *   - Set the hash table entry to NULL after freeing all nodes.
 */

void free_symbol_table(SYMBOL **symbol_table)
{
    int i;
    SYMBOL *curr_symbol;
    SYMBOL *temp;

    for (i = 0; i < HASH_TABLE_SIZE; i++)
    {
        curr_symbol = symbol_table[i];
        while (curr_symbol != NULL)
        {
            temp = curr_symbol;
            curr_symbol = curr_symbol->next;
            free(temp);
        }
        /*Set the hash table entry to NULL after freeing all nodes*/
        symbol_table[i] = NULL;
    }
}

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
 * Algorithm:
 *   - Allocate memory for the concatenated string based on the lengths of the input strings.
 *   - Copy the content of the original string to the new string.
 *   - Concatenate the second string to the new string.
 *   - Return the connected string.
 */

char *dynamic_strcat(char *first_string, char *second_string)
{
    char *connected_string;
    int connected_string_size;

    connected_string_size = (strlen(first_string) + strlen(second_string) + 1);

    connected_string = (char *)malloc(connected_string_size * sizeof(char));
    if (connected_string == NULL)
    {
        printf("Error in: %s , memory allocation failed \n", first_string);
        return NULL;
    }

    strcpy(connected_string, first_string);
    strcat(connected_string, second_string);

    return connected_string;
}

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
 * Algorithm:
 *   - Calculate the hash value for the provided name using the djb2 algorithm.
 *   - Traverse the linked list at the corresponding hash index to find the matching node.
 *   - Return the pointer to the found node, or NULL if not found.
 */

SYMBOL *symbol_lookup(char *name, SYMBOL *hash_table[HASH_TABLE_SIZE])
{
    unsigned int index = hash(name);
    SYMBOL *current = hash_table[index];
    while (current != NULL)
    {
        if (strcmp(name, current->name) == 0)
        {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

/*
 * Function: hash
 * -----------------------------------
 * Description:
 *   Allocates for a name, index in the hash table.
 * Parameters:
 *   - name: The name for which to calculate the hash value.
 * Returns:
 *   - The index in the hash table.
 * Algorithm:
 *   - Calculate the hash value for the provided name using the djb2 algorithm.
 *   - Return the index in the hash table.
 */

unsigned int hash(char *name)
{
    unsigned int hash_value;

    for (hash_value = 5381; *name != '\0'; name++)
    {
        hash_value = ((hash_value << 5) + hash_value) + *name; /*hash * 33 + c*/
    }

    return hash_value % HASH_TABLE_SIZE;
}

/*
 * Function: get_word
 * -----------------------------------
 * Description:
 *   Skips white characters and returns the next word in the string.
 * Parameters:
 *   - line: The string from which the words will be extracted.
 * Returns:
 *   - The next word in the string, or NULL in case of end of string or memory allocation error.
 * Algorithm:
 *   - Skip leading whitespace and commas.
 *   - Allocate memory for the word.
 *   - Extract characters for the word until whitespace, comma, or end of string is encountered.
 *   - Null-terminate the word and return it.
 */

char *get_word(char **line)
{
    char *word = NULL;
    char *temp = NULL;
    int word_length = STANDARD_WORD_SIZE;
    int char_cnt = 0;

    /*Skip leading whitespace*/
    while (isspace(**line) || (**line) == ',')
    {
        (*line)++;
    }

    /*Check for the end of the line*/
    if (**line == '\0')
    {
        return NULL; /*No more words*/
    }

    /*Allocate memory for the word*/
    word = (char *)malloc(word_length * sizeof(char));

    if (word == NULL)
    {
        return NULL;
    }

    /*Extract characters for the word*/
    while (**line && !isspace(**line) && (**line) != ',')
    {

        if (char_cnt == word_length - 1)
        {
            word_length = word_length + STANDARD_WORD_SIZE;
            temp = (char *)realloc(word, word_length * sizeof(char));

            if (temp == NULL)
            {

                free(word);
                return NULL;
            }
            word = temp;
        }

        word[char_cnt] = **line;
        char_cnt++;
        (*line)++;
    }

    /*Null-terminate the word*/
    word[char_cnt] = '\0';

    return word;
}

/**
 * Function: is_directive
 * -----------------------------------
 * Description:
 *   Checks if a given word represents a directive and returns its index.
 * Parameters:
 *   - word: A string representing a potential directive.
 * Returns:
 *   - The index of the directive if found, or -1 otherwise.
 * Algorithm:
 *   - Compare the provided word with predefined assembly directives.
 *   - Return the index of the directive if found, or -1 otherwise.
 */

int is_directive(char *word)
{
    int i;
    char *directives[4] = {".data", ".string", ".entry", ".extern"};
    for (i = 0; i < 4; i++)
    {
        if (strcmp(word, directives[i]) == 0)
        {
            return i;
        }
    }

    return -1;
}

/**
 * Function: is_operation
 * -----------------------------------
 * Description:
 *   Checks if a given word represents an operation and returns its index.
 * Parameters:
 *   - word: A string representing a potential assembly operation mnemonic.
 * Returns:
 *   - The index of the operation if found, or -1 otherwise.
 * Algorithm:
 *   - Compare the provided word with predefined assembly operations.
 *   - Return the index of the operation if found, or -1 otherwise.
 * */

int is_operation(char *word)
{
    int i;
    char *operations[16] = {"mov", "cmp", "add", "sub", "not", "clr", "lea", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "hlt"};
    for (i = 0; i < 16; i++)
    {
        if (strcmp(word, operations[i]) == 0)
        {
            return i;
        }
    }

    return -1;
}
