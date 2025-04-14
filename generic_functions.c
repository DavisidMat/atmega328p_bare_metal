#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "generic_functions.h"

void clear_input_buffer()
{
    /*
    This function clears the input buffer calling many times getchar()
    */
   int buffer_char = 0;
   while((buffer_char = getchar()) != '\n' && buffer_char != EOF);
}

void clean_input_string(char *input_string, int char_to_replace)
{
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

    char *char_position = NULL;
    // The following instruction will save in memory the position of the desired character to replace
    char_position = strchr(input_string, char_to_replace);

    if (char_position != NULL) /// Checks if the desired character was found
    {
        // Value swap because the character to replace was found
        *char_position = '\0';
    }
}

void clear_string(char *target_string)
{
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

   target_string[0] = '\0';
}

bool replace_char_input_string(char *input_string, char char_to_modify, char char_to_place, size_t buffer_size)
{
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

    char *char_position = NULL;
    // The following instruction will save in memory the position of the desired character to replace
    char_position = strchr(input_string, char_to_modify);

    if (char_position != NULL) /// Checks if the desired character was found
    {
        size_t offset_from_target = char_position - input_string; // Reading and calculating the difference between the addresses

        if (offset_from_target + 1 < buffer_size) // To avoid exceeding the input_string size.
        {
            // Value swap because the character to replace was found
            *char_position = char_to_place;

            *(char_position + 1) = '\0'; // To ensure adding null character at the end

            return true;
        }
    }

    // No character found
    return false;
}

void set_integer_string_fixed_length(char integer_string[], char output_string[], uint8_t expected_integer_char_length)
{
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

   uint8_t significant_digits = 0; // To count the number of significative digits (from right lo left, the significative digits).
   output_string[0] = '\0'; // Initializing output string

    for (uint8_t char_position = 0; char_position < expected_integer_char_length + 1; char_position++)
    {
        if (integer_string[char_position] != '\0')
        {
            significant_digits++;
        }
        else
        {
            break;
        }
    }

    if (significant_digits < expected_integer_char_length)
    {
        for (uint8_t char_position = 0; char_position < (expected_integer_char_length - significant_digits ); char_position++)
        {
            output_string[char_position] = '0'; // Creating zeros string to append to integer string.
        }
        output_string[(expected_integer_char_length - significant_digits )] = '\0'; // To limit the acces to the next adresseses in case the output variable is recycled.
    }
    strcat(output_string, integer_string); // Appending zeros string to integer string for output.
}