#ifndef generic_functions
#define generic_functions

#include <stdio.h>
#include <stdbool.h>

// Set buffer size for reading files/programs output (It must be greater than the size of the string read using "fgets()")
#define BUFFER_SIZE 100

// Set buffer size for command line instruction to execute (the prompt)
#define TERMINAL_BUFFER_SIZE 300

// Set string array default quantity of elements
#define ARRAY_MAX_ELEMENTS 100
// Set string array element size
#define STRING_BUFFER_SIZE 20

void clear_input_buffer();
    /*
    This function clears the input buffer calling many times getchar()
    */

void clean_input_string(char *input_string, int char_to_replace);
    /*
    This function replaces the desired character on the string, by a null character '\0'.
    This function is practical to replace the default end-of-line character '/n' written by default when fgets() is used.

    -------
    Input:
        char *input_string
            The input string which contains an end-of-line character to modify.
        int char_to_replace
            An integer containing the number of the character in the ASCII table to replace/remove by '\0'.

    -------
    Output:
        char *input_string
            A modification of the value of the input argument "char *input_string".
    */

void clear_string(char *target_string);
    /*
    This functions clears the content of a string setting the NULL character to the first element of the array
    (The computer will therefore understand that the variable has available storage).

    This function will not overwrite the content of the string.

    ------
    Input:
        char *target_string
            A string whose content will be discarded

    ------
    Output:
        char *target_string
            A modification of the value of the input argument "char *target_string".
    */

bool replace_char_input_string(char *input_string, char char_to_modify, char char_to_place, size_t buffer_size);
    /*
    This function replaces the desired character on the string, by a custom character.
    It replaces the first character found.

    -------
    Input:
        char *input_string
            The input string which contains an end-of-line character to modify.
        char char_to_modify
            A char containing the number of the character in the ASCII table to replace/remove by the custom char_to_place.
        char char_to_place
            A char containing the character to place.
        size_t buffer_size
            Size of char *input_string in bytes.

    -------
    Output:
        char *input_string
            A modification of the value of the input argument "char *input_string".
        bool
            A boolean specifying if the target char was found and replaced.
    */

void set_integer_string_fixed_length(char integer_string[], char output_string[], uint8_t expected_integer_char_length);
    /*
    This function is meant to be used after using "itoa()" for converting an integer to a string.
    This functions rewrites the input integer string into a format with fixed length.
    Examples of use:
        - Input string to 4 byte length: "15" -> "0015".
        - Input string to 4 byte length: "0" -> "0000".
        - Input string to 4 byte length: "1023" -> "1023".

    -------
    Input:
        char integer_string[]
            A char array containing the input integer string to rewrite.
        char output_string[]
            An empty char array that will store the rewritten integer string.
        uint8_t expected_integer_char_length
            The expected char length of the integer.
                Example: "1023" has 4 chars. "253" has 3 but it's desired to be "0253", hence 4.

    This function modifies the content of the pointer variables.
    */

#endif