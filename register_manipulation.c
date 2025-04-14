#include <stdio.h>

#include "register_manipulation.h"

int status_bit(volatile uint8_t *pointer_register, int desired_bit_position)
{
    /*
    This function checks the status of the desired bit (gives its current value).

    -------
    Input:
        volatile uint8_t *pointer_register
            A pointer storing the address of the bit to modifiy.
        int desired_bit_position
            An integer containing the position of the bit to modify (zero-indexed).

    -------
    Output:
        int bit_status
            An integer containing the current state of the bit of interest.
    */

    int bit_status = *pointer_register & (1 << (desired_bit_position));
    return bit_status;
}

void set_bit(volatile uint8_t *pointer_register, int desired_bit_position)
{
    /*
    This function sets the desired bit (gives value 1).

    -------
    Input:
        volatile uint8_t *pointer_register
            A pointer storing the address of the bit to modifiy.
        int desired_bit_position
            An integer containing the position of the bit to modify (zero-indexed).

    This function makes changes directly into the hardware of the microcontroller (modifies a register).
    */

    *pointer_register = *pointer_register | (1 << (desired_bit_position));
}

void unset_bit(volatile uint8_t *pointer_register, int desired_bit_position)
{
    /*
    This function unsets the desired bit (gives value 0).

    -------
    Input:
        volatile uint8_t *pointer_register
            A pointer storing the address of the bit to modifiy.
        int desired_bit_position
            An integer containing the position of the bit to modify (zero-indexed).

    This function makes changes directly into the hardware of the microcontroller (modifies a register).
    */
   
    *pointer_register = *pointer_register &~ (1 << (desired_bit_position));
}

void toggle_bit(volatile uint8_t *pointer_register, int desired_bit_position)
{
    /*
    This function toggles the current state of the desired bit (inverses its current value).

    -------
    Input:
        volatile uint8_t *pointer_register
            A pointer storing the address of the bit to modifiy.
        int desired_bit_position
            An integer containing the position of the bit to modify (zero-indexed).

    This function makes changes directly into the hardware of the microcontroller (modifies a register).
    */
   
    *pointer_register = *pointer_register ^ (1 << (desired_bit_position));
}

void write_16_bit_with_8_bit(uint16_t value_to_write, volatile uint8_t *high_byte, volatile uint8_t *low_byte)
{
     /*
    This function writes a 16-bit value with two 8-bit registers.

    Note: It is adviced for many MCUs (like Atmega series), to write first high byte before the low byte (for 8-bit architectures).

    -------
    Input:
        uint16_t value_to_write
            The desired 16-bit value to write into the two 8-bit registers.
        volatile uint8_t *high_byte
            The address of the high byte register to write.
        volatile uint8_t *low_byte
            The address of the low byte register to write.

    This function modifies the content of the pointer variables.
    */

    *high_byte = (uint8_t) (value_to_write >> 8); // Displaces the content and discard the bits to the left.
    *low_byte = (uint8_t) value_to_write; // Discards the bits of the left to take only the low_byte chunk.
}

void read_16_bit_with_8_bit(volatile uint8_t *low_byte, volatile uint8_t *high_byte, uint16_t *address_to_write)
{
     /*
    This function reads a 16-bit value using two 8-bit registers.

    Note: It is adviced for many MCUs (like Atmega series), to read first low byte before the high byte (for 8-bit architectures).

    -------
    Input:
        volatile uint8_t *low_byte
            The address of the low byte register to read.
        volatile uint8_t *high_byte
            The address of the high byte register to read.
        uint16_t *address_to_write
            The address where the 16-bit register value will be stored (containing the read operation).

    This function modifies the content of the pointer variables.
    */

   // 8 bit byte : uint8_t byte_low = *low_byte;
   // 8 bit byte : uint8_t byte_high = *high_byte;
   // 16 bit byte : *address_to_write = byte_low | (byte_high << 8);

    *address_to_write = *low_byte | (*high_byte << 8);
}