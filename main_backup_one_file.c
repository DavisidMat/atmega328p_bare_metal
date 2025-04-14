//#include <avr/io.h>

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
//#include <math.h>

#include "ATmega328p_registers.h"


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

void reverse(char* str, int len) {
    /*
    From ChatGPT
    */
    int i = 0, j = len - 1;
    char tmp;
    while (i < j) {
        tmp = str[i];
        str[i] = str[j];
        str[j] = tmp;
        i++;
        j--;
    }
}

int intToStr(int x, char* str, int min_digits) {
    /*
    From ChatGPT
    */
    int i = 0;
    if (x == 0) str[i++] = '0';
    else {
        while (x != 0) {
            str[i++] = (x % 10) + '0';
            x /= 10;
        }
    }

    while (i < min_digits)
        str[i++] = '0';

    reverse(str, i);
    str[i] = '\0';
    return i;
}

void ftoa(float n, char* res, int afterpoint) {
    /*
    From ChatGPT
    */
    // Handle negative numbers
    int i = 0;
    if (n < 0) {
        res[i++] = '-';
        n = -n;
    }

    // Extract integer part
    int ipart = (int)n;

    // Extract fractional part as an integer
    float fpart = n - (float)ipart;
    int factor = 1;
    for (int j = 0; j < afterpoint; j++)
        factor *= 10;

    int fpart_int = (int)(fpart * factor + 0.5); // rounding

    // Convert integer part to string
    i += intToStr(ipart, res + i, 0);

    // Add dot
    res[i++] = '.';

    // Convert fractional part
    intToStr(fpart_int, res + i, afterpoint);
}

#define BUFFER_SIZE 16
typedef struct ring_buffer_t
{
    /*
    This struct will contain a ring (or circular) data structure.
    */
    uint8_t *buffer; // A pointer to the data structure.
    uint16_t write_index; // Enumeration of the write position index.
    uint16_t read_index; // Enumeration of the read position index.
    uint16_t size; // The size of the data structure (number of bytes).
} ring_buffer_t;

void setup_ring_buffer(ring_buffer_t *ring_buffer, uint8_t *buffer, uint16_t buffer_size)
{
    /*
    This function initializes the ring buffer data structure type.

    This function will initialize the values of the read and write indexes (which point to buffer array slots),
    it will asign its respective buffer and its size.

    -------
    Input:
        ring_buffer_t *ring_buffer
            A ring_buffer_t custom struct which will allocate all the information and properties associated to this data structure.
        uint8_t *buffer
            An array of integers that conforms the buffer (expecting to allocate only 8-bit values per array slot).
        uint16_t buffer_size
            The size of "uint8_t *buffer" (expecting to not exceed 65536 according to the 16-bit capacity).

    -------
    Output:
        ring_buffer->buffer
            A pointer from the struct allocating the buffer.
        ring_buffer->write_index
            A default value for the write index.
        ring_buffer->read_index
            A default value for the read index.
        ring_buffer->size
            The size of the buffer.
    */

   ring_buffer->buffer = buffer;
   ring_buffer->write_index = 0;
   ring_buffer->read_index = 0;
   ring_buffer->size = buffer_size;
}

bool ring_buffer_emptiness(ring_buffer_t *ring_buffer)
{
    /*
    This function checks if the ring buffer is empty (contains no information to read).

    -------
    Input:
        ring_buffer_t *ring_buffer
            A ring_buffer_t custom struct which will allocate all the information and properties associated to this data structure.
    -------
    Output:
        bool buffer_emptiness
            A boolean specifying true or false for emptiness condition.
        
    */

    // bool buffer_emptiness:
    return (ring_buffer->write_index == ring_buffer->read_index);
    /* This function will be used to avoid a read operation when the buffer is empty.
        If a read operation is called, the next position will be empty, and it will fullfil the requirements for non-availability (resulting in an error). */
}

bool read_ring_buffer(ring_buffer_t *ring_buffer, uint8_t *byte_read)
{
    /*
    This function reads and frees the data allocated in the ring buffer data structure.

    -------
    Input:
        ring_buffer_t *ring_buffer
            A ring_buffer_t custom struct which will allocate all the information and properties associated to this data structure.
        uint8_t *byte_read
            A pointer of a char where the read data will be stored.
    -------
    Output:
        bool
            A boolean specifying true or false if something was read from the buffer.
        uint8_t *byte_read
            A modified value of the content of this pointer.  
    */

   // Checking buffer emptiness

    uint16_t instance_read_index = ring_buffer->read_index;
    // A local copy of read index is created in case an interruption modifies its original value while this function is being executed.
    // This critical condition can vulnerate the execution if it happens after the following verification (the if statement).
   if (ring_buffer_emptiness(ring_buffer) == true)
   {
        return false;
   }

    *byte_read = ring_buffer->buffer[instance_read_index];
    if (instance_read_index == (ring_buffer->size - 1))
    {
        instance_read_index = 0;
    }
    else
    {
        instance_read_index++;
    }

    ring_buffer->read_index = instance_read_index; // read index value update
    return true;
}

bool write_ring_buffer(ring_buffer_t *ring_buffer, uint8_t *byte_to_read)
{
    /*
    This function stores (or writes) incoming data from the 8-bit buffer register to the ring buffer data structure.

    -------
    Input:
        ring_buffer_t *ring_buffer
            A ring_buffer_t custom struct which will allocate all the information and properties associated to this data structure.
        uint8_t *byte_to_read
            The address of the 8-bit buffer register that stores the incoming data (to be stored in the ring_buffer).
    -------
    Output:
        bool
            A boolean specifying true or false if something was written to the buffer (in case space and data is available).
    */

   // Storing a local copy of current index state if interrupt occurs.

    uint16_t instance_write_index = ring_buffer->write_index;

    uint16_t future_write_index = instance_write_index + 1;
    if (instance_write_index == (ring_buffer->size - 1)) // Verification to avoid index overflow
    {
        future_write_index = 0;
    }

   if (ring_buffer->read_index == future_write_index)
   {
        // The buffer is full and cannot allocate more data. Future data will be ignored until the buffer is freed.
        return false;
   }
    ring_buffer->buffer[instance_write_index] = *byte_to_read;
    
    ring_buffer->write_index = future_write_index; // write index value update
    return true;
}

// In this case, the ring buffer data structure will be used to storre only incoming data (to stack in an array every incoming message before it
// is flushed at the moment of reading the content of the memory)

#define CLOCK_FREQUENCY 16000000
#define PRESCALE_VALUE 10 // Equivalent to dividing by 1024
#define INTERRUPTION_TIME ((CLOCK_FREQUENCY>>PRESCALE_VALUE))

#define ISR(interrupt_vector) \
    void interrupt_vector(void)  __attribute__( (signal) );\
    void interrupt_vector(void)
    // Start ISR definition

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


bool setup_external_interrupt(bool choose_external_int, uint8_t trigger_mode)
{
    /*
    Function specific to the ATMega328p, non portable.
    
    This function sets up the MCU for handling external interrupts coming from pins INT0 and INT1.

    "bool choose_external_int = false" activates external interrupt-0 and "bool choose_external_int = true" activates external interrupt-1 respectively 

    NOTE: Its respective ISR function must be called and defined later.
    Use following statements to define each ISR function respectively
        - "ISR(INT0_vect){}" belongs to the vector "INT0_vect" (vector 2, address 0x0002).
        - "ISR(INT1_vect){}" belongs to the vector "INT1_vect" (vector 3, address 0x0004).

    -------
    Input:
        bool choose_external_int
            A boolean specifying the desired external interrupt to activate. False is 0 and True is 1.
        uint8_t trigger_mode
            An integer that will specify the desired trigger mode action for starting the ISR (for both external inputs).

    Trigger Modes:
    --------------
    The chosen mode of the following list will be applied for both external interrupts (however, the MCU allows the chose to be individual).

        - 0x0 = The low level of INT0/INT1 generates an interrupt request.
        - 0x1 = Any logical change on INT0/INT1 generates an interrupt request.
        - 0x2 = The falling edge of INT0/INT1 generates an interrupt request.
        - 0x3 = The rising edge of INT0/INT1 generates an interrupt request.

    Normally, if a switch is attached to trigger this interrupt, falling edge mode is a good choice.
    -------
    Output:
        bool
            A boolean specifying if the chosen mode ore the setup are correct. 
    */

   if (trigger_mode > 0x4) 
    /*  Checking if chosen trigger mode is valid, to prevent applying
        a mask that can corrupt the bits of the rest of the register. */
   {
    return false; // Invalid chosen mode
   }

   // Initializing Flag register
   *EIFR = (uint8_t) 0;

   // Setting chosen trigger mode for the chosen external interrupt:
   if (choose_external_int == true)
   {
        // External interrupt 1 chosen
        *EICRA = *EICRA | (trigger_mode << 2);
   }
   else
   {
        // External interrupt 0 chosen
        *EICRA = *EICRA | (trigger_mode);
   }


    if (choose_external_int == true)
    {
        set_bit(EIMSK, INT1); // Activating external interrupt-1 hardware interrupt
    }
    else
    {
        set_bit(EIMSK, INT0); // Activating external interrupt-0 hardware interrupt
    }

    return true; // Succesfull setup, no expected strange behavior.
}

void activate_external_interrupts(bool enable_external_interrupt_0, bool enable_external_interrupt_1)
{
    /*
    Function specific to the ATMega328p, non portable.

    This function activates the ISR related to external interrupts.
    It modifies the Mask register from this peripherial
 
    -------
    Input:
        bool enable_external_interrupt_0
            A boolean specifying if external_interrupt_0 shall be activated.
        bool enable_external_interrupt_1
            A boolean specifying if external_interrupt_1 shall be activated.
        
    */

    if (enable_external_interrupt_0 == true)
    {
        set_bit(EIMSK,INT0);
    }
    if (enable_external_interrupt_1 == true)
    {
        set_bit(EIMSK,INT1);
    }
}

void desactivate_external_interrupts(bool disable_external_interrupt_0, bool disable_external_interrupt_1)
{
    /*
    Function specific to the ATMega328p, non portable.

    This function desactivates the ISR related to external interrupts.
    It modifies the Mask register from this peripherial
 
    -------
    Input:
        bool disable_external_interrupt_0
            A boolean specifying if external_interrupt_0 shall be desactivated.
        bool disable_external_interrupt_1
            A boolean specifying if external_interrupt_1 shall be desactivated.
        
    */

    if (disable_external_interrupt_0 == true)
    {
        unset_bit(EIMSK,INT0);
    }
    if (disable_external_interrupt_1 == true)
    {
        unset_bit(EIMSK,INT1);
    }
}

bool activate_pin_change_interrupt(uint8_t activate_chosen_interrupt_pin)
{
    /*
    Function specific to the ATMega328p, non portable.

    This function activates the pin change interrupt of the chosen input pin.

    Use the following statements to define each ISR function respectively:
        - "ISR(PCINT0_vect){}" belongs to the vector "PCINT0" (vector 4, address 0x0006).
        - "ISR(PCINT1_vect){}" belongs to the vector "PCINT1" (vector 5, address 0x0008).
        - "ISR(PCINT2_vect){}" belongs to the vector "PCINT2" (vector 6, address 0x000A).

    Table of available pin change inputs:

        - (0 - 7) -> Belong to Pin Change Interrupt 0.
        - (8 - 14) -> Belong to Pin Change Interrupt 1.
        - (16 - 23) -> Belong to Pin Change Interrupt 2.
 
    -------
    Input:
        uint8_t activate_chosen_interrupt_pin
            An integer specifying the input pin that will trigger the ISR.
 
    -------
    Output:
        bool
            A boolean specifying if the execution of this function ended successfully.

    */

    if ((activate_chosen_interrupt_pin > 23) | (activate_chosen_interrupt_pin == 15))
    {
        return false;
    }

    if (activate_chosen_interrupt_pin < 8)
    {
        set_bit(PCICR, PCIE0);
        set_bit(PCMSK0, activate_chosen_interrupt_pin);
    }
    else if (activate_chosen_interrupt_pin < 15)
    {
        set_bit(PCICR, PCIE1);
        set_bit(PCMSK1, activate_chosen_interrupt_pin - 8);
    }
    else if (activate_chosen_interrupt_pin < 24)
    {
        set_bit(PCICR, PCIE2);
        set_bit(PCMSK2, activate_chosen_interrupt_pin - 16);
    }

    return true;
}

bool desactivate_pin_change_interrupt(uint8_t desactivate_chosen_interrupt_pin)
{
    /*
    Function specific to the ATMega328p, non portable.

    This function desactivates the pin change interrupt of the chosen input pin.

    Table of available pin change inputs:

        - (0 - 7) -> Belong to Pin Change Interrupt 0.
        - (8 - 14) -> Belong to Pin Change Interrupt 1.
        - (16 - 23) -> Belong to Pin Change Interrupt 2.
 
    -------
    Input:
        uint8_t desactivate_chosen_interrupt_pin
            An integer specifying the input pin to disable its ISR.
 
    -------
    Output:
        bool
            A boolean specifying if the execution of this function ended successfully.

    */

    if ((desactivate_chosen_interrupt_pin > 23) | (desactivate_chosen_interrupt_pin == 15))
    {
        return false;
    }

    if (desactivate_chosen_interrupt_pin < 8)
    {
        unset_bit(PCICR, PCIE0);
        unset_bit(PCMSK0, desactivate_chosen_interrupt_pin);
    }
    else if (desactivate_chosen_interrupt_pin < 15)
    {
        unset_bit(PCICR, PCIE1);
        unset_bit(PCMSK1, desactivate_chosen_interrupt_pin - 8);
    }
    else if (desactivate_chosen_interrupt_pin < 24)
    {
        unset_bit(PCICR, PCIE2);
        unset_bit(PCMSK2, desactivate_chosen_interrupt_pin - 16);
    }

    return true;
}

bool setup_8_bit_timer_counter_0(uint8_t activated_channels, uint8_t setup_mode, uint8_t clock_prescaler_mode, bool output_inverting_mode,
                                 uint8_t match_val_channel_A, uint8_t match_val_channel_B, uint8_t counter_interrupt_enabled_modes)
{
    /*
    Function specific to the ATMega328p, non portable.

    This functions sets up the 8-bit timer/counter_0 peripherial. The function summarizes below all its possible functions.
    This peripherial controls OC0A/PD6 and OC0B/PD5 outputs for output waveform generation.

    NOTE: "uint8_t match_val_channel_A" defines the TOP counter value for Waveform generation modes "0x5" and "0x7".

    Use the following statements to define each ISR function respectively:
        - "ISR(TIMER0_COMPA_vect){}" belongs to the vector "TIMER0 COMPA" (vector 15, address 0x001C).
        - "ISR(TIMER0_COMPB_vect){}" belongs to the vector "TIMER0 COMPB" (vector 16, address 0x001E).
        - "ISR(TIMER0_OVF_vect){}" belongs to the vector "TIMER0 OVF" (vector 17, address 0x0020).

    I. List of channels/outputs to enable (For "uint8_t activated_channels"):
        - 0x0 = No channels activated (For only timer/counter use, no output waveform).
            In this mode, "OC0B/PD5" and "OC0A/PD6" pins can be used as digital I/O.
        - 0x1 = Channel "OC0A/PD6" activated for waveform generation output
        - 0x2 = Channel "OC0B/PD5" activated for waveform generation output
        - 0x3 = Channels "OC0A/PD6" and "OC0B/PD5" activated for waveform generation output

    II. List of Waveform generation mode table (For "uint8_t setup_mode"):
        - 0x0 = Normal (Counter range: 0x00 - 0xFF)
        - 0x1 = PWM phase correct (Counter range: 0x00 - 0xFF)
        - 0x2 = CTC (Clear timer on compare match) (Counter range: 0x00 - OCRA value)
        - 0x3 = Fast PWM (Counter range: 0x00 - 0xFF)
        - 0x4 = N/A (Reserved)
        - 0x5 = PWM phase correct (Counter range: 0x00 - OCRA value)
        - 0x6 = N/A (Reserved)
        - 0x7 = Fast PWM (Counter range: 0x00 - OCRA value)
         
    III. List of clock prescaler modes (to divide clock operation frequency) (For uint8_t clock_prescaler_mode):
        - 0x0 = No clock source (Time/Counter disabled)
        - 0x1 = No prescaling for on-board clock
        - 0x2 = On-board clock / 8
        - 0x3 = On-board clock / 64
        - 0x4 = On-board clock / 256
        - 0x5 = On-board clock / 1024
        - 0x6 = External clock source on T0 pin. Clock on falling edge
        - 0x7 = External clock source on T0 pin. Clock on rising edge

    IV. List of interrupt enabled modes (For "uint8_t counter_interrupt_enabled_modes"):
        - 0x0 = No interrupts enabled
        - 0x1 = Enable counter overflow interrupt
        - 0x2 = Enable value compare match A interrupt
        - 0x3 = Enable counter overflow interrupt and value compare match A interrupt
        - 0x4 = Enable value compare match B interrupt
        - 0x5 = Enable counter overflow interrupt and value compare match B interrupt
        - 0x6 = Enable value compare match B interrupt and value compare match A interrupt
        - 0x7 = Enable counter overflow interrupt, value compare match B interrupt and value compare match A interrupt


    -------
    Input:
        uint8_t activated_channels
            An integer containing the selection of activated channels for waveform generation (according to list I.)
        uint8_t setup_mode
            An integer containing the selection of the desired output mode for waveform generation (according to list II.)
        uint8_t clock_prescaler_mode
             An integer containing the selection of clock prescaler for dividing the clock frequency (according to list III.)
        bool output_inverting_mode
            A boolean specifying if the output waveform generation will be in inverting mode (applies for both outputs)
        uint8_t match_val_channel_A
            An integer containing the desired value in which the compare match will trigger (affects output of channel A)
        uint8_t match_val_channel_B
            An integer containing the desired value in which the compare match will trigger (affects output of channel B)
        uint8_t counter_interrupt_enabled_modes
            An integer containing the selection of interrupt enabled modes (according to list IV.)

    -------
    Output:
        bool
            A boolean specifying true or false if the setup of counter/timer_0 was valid/successfull.
    */

    // Verifying function input args correctness:
    if (activated_channels > 0x3)
    {
        return false; // Wrong or invalid activated channels chosen
    }
    
    else if ((setup_mode > 0x7) | (setup_mode == 0x4) | (setup_mode == 0x6))
    {
        return false; // Wrong or invalid waveform generation mode chosen
    }
    else if ((match_val_channel_A > 0xFF) | (match_val_channel_B > 0xFF))
    {
        return false; // Invalid counter match value (Exceeds register bit size)
    }
    else if (clock_prescaler_mode > 0x7)
    {
        return false; // Invalid input clock prescaler mode
    }
    else if (counter_interrupt_enabled_modes > 0x7)
    {
        return false; // Wrong or invalid selection for enabled interrupts
    }
    

    // Initializing timer/counter registers
    *TCNT0 = 0x0; // Setting timer to 0 (The counter MUST always be initialized first before any other setup)
    *TIFR0 = 0x0; // Setting interrupts flag register to 0
    *TCCR0A = 0x0; // Setting Timer/Counter control register A to 0
    *TCCR0B = 0x0; // Setting Timer/Counter control register B to 0
    
    /*
    Setting up Output mode
    */

    // The following chunk of code is organized in such way because two separate registers need to be modified
    if (setup_mode == 0x5) // PWM phase correct (Counter range: 0x00 - OCRA value)
    {
        set_bit(TCCR0B, WGM02);
        unset_bit(TCCR0A, WGM01);
        set_bit(TCCR0A, WGM00);
    }
    else if (setup_mode == 0x7) // Fast PWM (Counter range: 0x00 - OCRA value)
    {
        set_bit(TCCR0B, WGM02);
        set_bit(TCCR0A, WGM01);
        set_bit(TCCR0A, WGM00);
    }
    else
    {
        *TCCR0A = (*TCCR0A & ~((uint8_t) 0xF)) | setup_mode; // To only write the desired value bits without overwriting the register
    }

    /*
    Activating output channels according to "uint8_t activated_channels" selection
    */
    // "activated_channels == 0x0" is meant to be used only to use the timer/counter without overriding output channels functionality (no output waveform generation) 

    if ((activated_channels == 0x1) | (activated_channels == 0x3))
    {
        // Setting OC0A/PD6 pin as output for Waveform generation
        set_bit(DDRD, DDD6);
    
        set_bit(TCCR0A, COM0A1);
        if (output_inverting_mode == true)
        {
            set_bit(TCCR0A, COM0A0);
        }
        // Else condition of this conditional will set non-inverting mode as default condition
    }

    if ((activated_channels == 0x2) | (activated_channels == 0x3))
    {
        // Setting OC0B/PD5 pin as output for Waveform generation
        set_bit(DDRD, DDD5);
    
        set_bit(TCCR0A, COM0B1);
        if (output_inverting_mode == true)
        {
            set_bit(TCCR0A, COM0B0);
        }
        // Else condition of this conditional will set non-inverting mode as default condition
    }


    // Setting clock prescaler mode:
    *TCCR0B = (*TCCR0B & ~((uint8_t) 0xF)) | clock_prescaler_mode; // To only write the desired value bits without overwriting the register


    // Setting match counter registers
    *OCR0A = match_val_channel_A;
    *OCR0B = match_val_channel_B;

    // Enabling chosen interrupts for ISRs:
    *TIMSK0 = (*TIMSK0 & ~((uint8_t) 0xF)) | counter_interrupt_enabled_modes; // To only write the desired value bits without overwriting the register

    return true;
}

bool setup_16_bit_timer_counter_1(uint8_t activated_channels, uint8_t setup_mode, uint8_t clock_prescaler_mode, bool output_inverting_mode,
                                  uint16_t match_val_channel_A, uint16_t match_val_channel_B, uint8_t counter_interrupt_enabled_modes)
{
    /*
    Function specific to the ATMega328p, non portable.

    This functions sets up the 16-bit timer/counter_1 peripherial. The function summarizes below all its possible functions.
    This peripherial controls OC1A/PB1 and OC1B/PB2 outputs for output waveform generation.

    NOTE: "uint8_t match_val_channel_A" defines the TOP counter value for Waveform generation modes "0x5" and "0x7".

    Use the following statements to define each ISR function respectively:
        - "ISR(TIMER1_CAPT_vect){}" belongs to the vector "TIMER1 CAPT" (vector 11, address 0x0014).
        - "ISR(TIMER1_COMPA_vect){}" belongs to the vector "TIMER1 COMPA" (vector 12, address 0x0016).
        - "ISR(TIMER1_COMPB_vect){}" belongs to the vector "TIMER1 COMPB" (vector 13, address 0x0018).
        - "ISR(TIMER1_OVF_vect){}" belongs to the vector "TIMER1 OVF" (vector 14, address 0x001A).

    I. List of channels/outputs to enable (For "uint8_t activated_channels"):
        - 0x0 = No channels activated (For only timer/counter use, no output waveform).
            In this mode, "OC1B/PB2" and "OC1A/PB1" pins can be used as digital I/O.
        - 0x1 = Channel "OC1A/PB1" activated for waveform generation output
        - 0x2 = Channel "OC1B/PB2" activated for waveform generation output
        - 0x3 = Channels "OC1A/PB1" and "OC1B/PB2" activated for waveform generation output

    II. List of Waveform generation mode table (For "uint8_t setup_mode"):
        - 0x0 = Normal (Counter range: 0x0000 - 0xFFFF)
        - 0x1 = PWM phase correct, 8-bit (Counter range: 0x0000 - 0x00FF)
        - 0x2 = PWM phase correct, 9-bit (Counter range: 0x0000 - 0x01FF)
        - 0x3 = PWM phase correct, 10-bit (Counter range: 0x0000 - 0x03FF)
        - 0x4 = CTC (Clear timer on compare match) (Counter range: 0x0000 - OCRA value)
        - 0x5 = Fast PWM, 8-bit (Counter range: 0x0000 - 0x00FF)
        - 0x6 = Fast PWM, 9-bit (Counter range: 0x0000 - 0x01FF)
        - 0x7 = Fast PWM, 10-bit (Counter range: 0x0000 - 0x03FF)
        - 0x8 = PWM, phase and frequency correct (Counter range: 0x0000 - ICR1)
        - 0x9 = PWM, phase and frequency correct (Counter range: 0x0000 - OCRA)
        - 0x10 = PWM phase correct (Counter range: 0x0000 - ICR1)
        - 0x11 = PWM phase correct (Counter range: 0x0000 - OCRA)
        - 0x12 = CTC (Clear timer on compare match) (Counter range: 0x0000 - ICR1 value)
        - 0x13 = N/A (Reserved)
        - 0x14 = Fast PWM (Counter range: 0x0000 - ICR1)
        - 0x15 = Fast PWM (Counter range: 0x0000 - OCRA)

    III. List of clock prescaler modes (to divide clock operation frequency) (For uint8_t clock_prescaler_mode):
        - 0x0 = No clock source (Time/Counter disabled)
        - 0x1 = No prescaling for on-board clock
        - 0x2 = On-board clock / 8
        - 0x3 = On-board clock / 64
        - 0x4 = On-board clock / 256
        - 0x5 = On-board clock / 1024
        - 0x6 = External clock source on T1 pin. Clock on falling edge
        - 0x7 = External clock source on T1 pin. Clock on rising edge

    IV. List of interrupt enabled modes (For "uint8_t counter_interrupt_enabled_modes"):
        - 0x0 = No interrupts enabled
        - 0x1 = Enable counter overflow interrupt
        - 0x2 = Enable value compare match A interrupt
        - 0x3 = Enable counter overflow interrupt and value compare match A interrupt
        - 0x4 = Enable value compare match B interrupt
        - 0x5 = Enable counter overflow interrupt and value compare match B interrupt
        - 0x6 = Enable value compare match B interrupt and value compare match A interrupt
        - 0x7 = Enable counter overflow interrupt, value compare match B interrupt and value compare match A interrupt


    -------
    Input:
        uint8_t activated_channels
            An integer containing the selection of activated channels for waveform generation (according to list I.)
        uint8_t setup_mode
            An integer containing the selection of the desired output mode for waveform generation (according to list II.)
        uint8_t clock_prescaler_mode
            An integer containing the selection of clock prescaler for dividing the clock frequency (according to list III.)
        bool output_inverting_mode
            A boolean specifying if the output waveform generation will be in inverting mode (applies for both outputs)
        uint16_t match_val_channel_A
            An integer containing the desired value in which the compare match will trigger (affects output of channel A)
        uint16_t match_val_channel_B
            An integer containing the desired value in which the compare match will trigger (affects output of channel B)
        uint8_t counter_interrupt_enabled_modes
            An integer containing the selection of interrupt enabled modes (according to list IV.)

    -------
    Output:
        bool
            A boolean specifying true or false if the setup of counter/timer_1 was valid/successfull.
    */

    // Verifying function input args correctness:
    if (activated_channels > 0x3)
    {
        return false; // Wrong or invalid activated channels chosen
    }

    else if ((setup_mode > 0x15) | (setup_mode == 0x13))
    {
        return false; // Wrong or invalid waveform generation mode chosen
    }
    else if ((match_val_channel_A > 0xFFFF) | (match_val_channel_B > 0xFFFF))
    {
        return false; // Invalid counter match value (Exceeds register bit size)
    }
    else if (clock_prescaler_mode > 0x7)
    {
        return false; // Invalid input clock prescaler mode
    }
    else if (counter_interrupt_enabled_modes > 0x7)
    {
        return false; // Wrong or invalid selection for enabled interrupts
    }


    // Initializing timer/counter registers
    write_16_bit_with_8_bit(0x0, TCNT1H, TCNT1L); // Setting timer to 0 (The counter MUST always be initialized first before any other setup)
    *TIFR1 = 0x0; // Setting interrupts flag register to 0
    *TCCR1A = 0x0; // Setting Timer/Counter control register A to 0
    *TCCR1B = 0x0; // Setting Timer/Counter control register B to 0
    *TCCR1C = 0x0; // Setting Timer/Counter control register C to 0

    /*
    Setting up Output mode
    */

    // The following chunk of code is organized in such way because two separate registers need to be modified

    // Applying masks to set the chosen setup mode
    uint8_t control_register_i_mask = (setup_mode) & ((uint8_t) 0x3); // 0x3 = 0b0011 to clean the content of the mask except bits 1 and 0.
    uint8_t control_register_ii_mask = (setup_mode < 1) & ((uint8_t) 0x18); // 0x18 = 0b11000 to clean the content of the mask except bits 4 and 3.

    *TCCR1A = (*TCCR1A | control_register_i_mask); // To only write the desired value bits of the first counter control register
    *TCCR1B = (*TCCR1B | control_register_ii_mask); // To only write the desired value bits of the second counter control register


    /*
    Activating output channels according to "uint8_t activated_channels" selection
    */
    // "activated_channels == 0x0" is meant to be used only to use the timer/counter without overriding output channels functionality (no output waveform generation) 

    if ((activated_channels == 0x1) | (activated_channels == 0x3))
    {
        // Setting OC1A/PB1 pin as output for Waveform generation
        set_bit(DDRB, DDB1);

        set_bit(TCCR1A, COM1A1);
        if (output_inverting_mode == true)
        {
            set_bit(TCCR1A, COM1A0);
        }
        // Else condition of this conditional will set non-inverting mode as default condition
    }

    if ((activated_channels == 0x2) | (activated_channels == 0x3))
    {
        // Setting OC1B/PB2 pin as output for Waveform generation
        set_bit(DDRB, DDB2);

        set_bit(TCCR1A, COM1B1);
        if (output_inverting_mode == true)
        {
            set_bit(TCCR1A, COM1B0);
        }
        // Else condition of this conditional will set non-inverting mode as default condition
    }


    // Setting clock prescaler mode:
    *TCCR1B = (*TCCR1B & ~((uint8_t) 0xF)) | clock_prescaler_mode; // To only write the desired value bits without overwriting the register


    // Setting match counter registers
    write_16_bit_with_8_bit(match_val_channel_A, OCR1AH, OCR1AL);
    write_16_bit_with_8_bit(match_val_channel_B, OCR1BH, OCR1BL);

    // Enabling chosen counter interrupts for ISRs:
    *TIMSK1 = (*TIMSK1 & ~((uint8_t) 0xF)) | counter_interrupt_enabled_modes; // To only write the desired value bits without overwriting the register

    return true;
}

bool setup_8_bit_timer_counter_2(uint8_t activated_channels, uint8_t setup_mode, uint8_t clock_prescaler_mode, bool output_inverting_mode,
                                 uint8_t match_val_channel_A, uint8_t match_val_channel_B, uint8_t counter_interrupt_enabled_modes)
{
    /*
    Function specific to the ATMega328p, non portable.

    This functions sets up the 8-bit timer/counter_2 peripherial. The function summarizes below all its possible functions.
    This peripherial controls OC2A/PB3 and OC2B/PD3 outputs for output waveform generation.

    NOTE: "uint8_t match_val_channel_A" defines the TOP counter value for Waveform generation modes "0x5" and "0x7".

    Use the following statements to define each ISR function respectively:
        - "ISR(TIMER2_COMPA_vect){}" belongs to the vector "TIMER2 COMPA" (vector 8, address 0x000E).
        - "ISR(TIMER2_COMPB_vect){}" belongs to the vector "TIMER2 COMPB" (vector 9, address 0x0010).
        - "ISR(TIMER2_OVF_vect){}" belongs to the vector "TIMER2 OVF" (vector 10, address 0x0012).

    I. List of channels/outputs to enable (For "uint8_t activated_channels"):
        - 0x0 = No channels activated (For only timer/counter use, no output waveform).
            In this mode, "OC2B/PD5" and "OC2A/PD3" pins can be used as digital I/O.
        - 0x1 = Channel "OC2A/PB3" activated for waveform generation output
        - 0x2 = Channel "OC2B/PD3" activated for waveform generation output
        - 0x3 = Channels "OC2A/PB3" and "OC2B/PD3" activated for waveform generation output

    II. List of Waveform generation mode table (For "uint8_t setup_mode"):
        - 0x0 = Normal (Counter range: 0x00 - 0xFF)
        - 0x1 = PWM phase correct (Counter range: 0x00 - 0xFF)
        - 0x2 = CTC (Clear timer on compare match) (Counter range: 0x00 - OCRA value)
        - 0x3 = Fast PWM (Counter range: 0x00 - 0xFF)
        - 0x4 = N/A (Reserved)
        - 0x5 = PWM phase correct (Counter range: 0x00 - OCRA value)
        - 0x6 = N/A (Reserved)
        - 0x7 = Fast PWM (Counter range: 0x00 - OCRA value)

    III. List of clock prescaler modes (to divide clock operation frequency) (For uint8_t clock_prescaler_mode):
        - 0x0 = No clock source (Time/Counter disabled)
        - 0x1 = No prescaling for chosen clock
        - 0x2 = Chosen clock / 8
        - 0x3 = Chosen clock / 32
        - 0x4 = Chosen clock / 64
        - 0x5 = Chosen clock / 128
        - 0x6 = Chosen clock / 256
        - 0x7 = Chosen clock / 1024

    IV. List of interrupt enabled modes (For "uint8_t counter_interrupt_enabled_modes"):
        - 0x0 = No interrupts enabled
        - 0x1 = Enable counter overflow interrupt
        - 0x2 = Enable value compare match A interrupt
        - 0x3 = Enable counter overflow interrupt and value compare match A interrupt
        - 0x4 = Enable value compare match B interrupt
        - 0x5 = Enable counter overflow interrupt and value compare match B interrupt
        - 0x6 = Enable value compare match B interrupt and value compare match A interrupt
        - 0x7 = Enable counter overflow interrupt, value compare match B interrupt and value compare match A interrupt


    -------
    Input:
        uint8_t activated_channels
            An integer containing the selection of activated channels for waveform generation (according to list I.)
        uint8_t setup_mode
            An integer containing the selection of the desired output mode for waveform generation (according to list II.)
        uint8_t clock_prescaler_mode
            An integer containing the selection of clock prescaler for dividing the clock frequency (according to list III.)
        bool output_inverting_mode
            A boolean specifying if the output waveform generation will be in inverting mode (applies for both outputs)
        uint8_t match_val_channel_A
            An integer containing the desired value in which the compare match will trigger (affects output of channel A)
        uint8_t match_val_channel_B
            An integer containing the desired value in which the compare match will trigger (affects output of channel B)
        uint8_t counter_interrupt_enabled_modes
            An integer containing the selection of interrupt enabled modes (according to list IV.)

    -------
    Output:
        bool
            A boolean specifying true or false if the setup of counter/timer_2 was valid/successfull.
    */

    // Verifying function input args correctness:
    if (activated_channels > 0x3)
    {
        return false; // Wrong or invalid activated channels chosen
    }

    else if ((setup_mode > 0x7) | (setup_mode == 0x4) | (setup_mode == 0x6))
    {
        return false; // Wrong or invalid waveform generation mode chosen
    }
    else if ((match_val_channel_A > 0xFF) | (match_val_channel_B > 0xFF))
    {
        return false; // Invalid counter match value (Exceeds register bit size)
    }
    else if (clock_prescaler_mode > 0x7)
    {
        return false; // Invalid input clock prescaler mode
    }
    else if (counter_interrupt_enabled_modes > 0x7)
    {
        return false; // Wrong or invalid selection for enabled interrupts
    }


    // Initializing timer/counter registers
    *TCNT2 = 0x0; // Setting timer to 0 (The counter MUST always be initialized first before any other setup)
    *TIFR2 = 0x0; // Setting interrupts flag register to 0
    *TCCR2A = 0x0; // Setting Timer/Counter control register A to 0
    *TCCR2B = 0x0; // Setting Timer/Counter control register B to 0

    /*
    Setting up Output mode
    */

    // The following chunk of code is organized in such way because two separate registers need to be modified
    if (setup_mode == 0x5) // PWM phase correct (Counter range: 0x00 - OCRA value)
    {
        set_bit(TCCR2B, WGM22);
        unset_bit(TCCR2A, WGM21);
        set_bit(TCCR2A, WGM20);
    }
    else if (setup_mode == 0x7) // Fast PWM (Counter range: 0x00 - OCRA value)
    {
        set_bit(TCCR2B, WGM22);
        set_bit(TCCR2A, WGM21);
        set_bit(TCCR2A, WGM20);
    }
    else
    {
        *TCCR2A = (*TCCR2A & ~((uint8_t) 0xF)) | setup_mode; // To only write the desired value bits without overwriting the register
    }

    /*
    Activating output channels according to "uint8_t activated_channels" selection
    */
    // "activated_channels == 0x0" is meant to be used only to use the timer/counter without overriding output channels functionality (no output waveform generation) 

    if ((activated_channels == 0x1) | (activated_channels == 0x3))
    {
        // Setting OC2A/PB3 pin as output for Waveform generation
        set_bit(DDRB, DDB3);

        set_bit(TCCR2A, COM2A1);
        if (output_inverting_mode == true)
        {
            set_bit(TCCR2A, COM2A0);
        }
        // Else condition of this conditional will set non-inverting mode as default condition
    }

    if ((activated_channels == 0x2) | (activated_channels == 0x3))
    {
        // Setting OC2B/PD3 pin as output for Waveform generation
        set_bit(DDRD, DDD3);

        set_bit(TCCR2A, COM2B1);
        if (output_inverting_mode == true)
        {
            set_bit(TCCR2A, COM2B0);
        }
        // Else condition of this conditional will set non-inverting mode as default condition
    }


    // Setting clock prescaler mode:
    *TCCR2B = (*TCCR2B & ~((uint8_t) 0xF)) | clock_prescaler_mode; // To only write the desired value bits without overwriting the register

    // Setting match counter registers
    *OCR2A = match_val_channel_A;
    *OCR2B = match_val_channel_B;

    // Enabling chosen interrupts for ISRs:
    *TIMSK2 = (*TIMSK2 & ~((uint8_t) 0xF)) | counter_interrupt_enabled_modes; // To only write the desired value bits without overwriting the register

    return true;
}

void setup_input_capture_unit(bool select_rising_edge_triggering, bool use_analog_comparator, bool activate_input_toggle)
{
    /*
    Function specific to the ATMega328p, non portable.

    This function must be called after "bool setup_16_bit_timer_counter_1(...)"

    This function sets up the input capture unit, to be used for measuring fast digital signals. According to the settings,
    the timestamp of the event will be copied from the counter1 register and saved to ICR1.

    Use the following statements to define each ISR function respectively:
        - "ISR(TIMER1_CAPT_vect){}" belongs to the vector "TIMER1 CAPT" (vector 11, address 0x0014).

    -------
    Input:
        
        bool select_rising_edge_triggering
            A boolean specifying if the edge trigger mode is rising. If false, the mode will be set to falling.
        bool use_analog_comparator
            A boolean specifying if the source trigger pin will be the analog comparator. If false, ICP1 is used.
        bool activate_input_toggle
            A boolean specifying if the input signal source will change later (change of pins for reading multiple channels).
    */
   
    set_bit(TCCR1B, ICNC1); // Activating input capture noise canceller (Note: The reading will be delayed by four system clock cycles while the signal is filtered)

    if (select_rising_edge_triggering == true)
    {
        set_bit(TCCR1B, ICES1); // Setting edge detection to rising
    }
    else
    {
        unset_bit(TCCR1B, ICES1); // Setting edge detection to falling
    }

    if (activate_input_toggle == true)
    {
        unset_bit(DDRB, DDB0); // Setting ICP1 pin as input
        set_bit(PORTB, PORTB0); // setting pull up resistor

        unset_bit(DDRD, DDD7); // Setting Analog Comparator negative input pin as input
        set_bit(PORTD, PORTD7); // // setting pull up resistor
    }
    if (use_analog_comparator == true)
    {
        /*
        NOTE:
        Because in this configuration the positive input from the analog comparator is constant, and the input signal goes into the negative input,
        the output signal will be inverted.
            Output = 0 -> V+ < V-
            Output = 1 -> V+ > V-
        */
       toggle_bit(TCCR1B, ICES1); // Equivalent to calling swap_input_capture_edge_trigger_mode().
       // Hence, the reason of inverting the edge trigger mode to obtain the same original signal behavior

        // When the analog comparator multiplexer is disabled, its negative input will be pin PD7/AIN1
        unset_bit(DDRD, DDD7); // Setting Analog Comparator negative input pin as input
        set_bit(PORTD, PORTD7); // // setting pull up resistor

        unset_bit(ADCSRB, ACME); // Multiplexer disabled

        unset_bit(ACSR, ACD); // Activate analog comparator (to not turn off its power)
        set_bit(ACSR, ACIC); // To enable the analog comparator for the input Capture Unit
        set_bit(ACSR, ACBG); // Setting positive input of the analog comparator equal to voltage bandgap (reference Vcc) (PD6/AIN0 is avaiable)
    }
    else
    {
        unset_bit(DDRB, DDB0); // Setting ICP1 pin as input
        set_bit(PORTB, PORTB0); // setting pull up resistor
    }

    set_bit(TIMSK1, ICIE1); // Activating input capture interrupt
    set_bit(TIFR1, ICF1); // Clearing ICU interrupt flag
}

void swap_input_capture_edge_trigger_mode(void)
{
    /*
    Function specific to the ATMega328p, non portable.

    This function will toggle the current state of the edge detector for the input capture unit (belonging to the 16-bit timer/counter_1).
    This will change the trigger mode of the input capture unit.

    This function toggles the state of the edge detector mode bit.

    -------
    Input:
        No input arguments
    */

    toggle_bit(TCCR1B, ICES1);
    set_bit(TIFR1, ICF1); // Clearing ICU interrupt flag
}

void change_input_capture_unit_trigger_source(bool use_analog_comparator_input)
{
    /*
    Function specific to the ATMega328p, non portable.

    This function should be called after calling "void setup_input_capture_unit(..., bool activate_input_toggle)"", and "setting bool activate_input_toggle = true".

    This function will toggle the current state of the edge detector for the input capture unit (belonging to the 16-bit timer/counter_1).
    This will change the trigger mode of the input capture unit.

    This function toggles the state of the edge detector mode bit.

    -------
    Input:
        bool use_analog_comparator_input
            A boolean specifying if the source trigger pin will be the analog comparator. If false, ICP1 is used.
    */

    if (use_analog_comparator_input == true)
    {
        set_bit(ACSR, ACIC); // To enable the analog comparator for the input Capture Unit
        /*
        NOTE:
        Because in this configuration the positive input from the analog comparator is constant, and the input signal goes into the negative input,
        the output signal will be inverted.
            Output = 0 -> V+ < V-
            Output = 1 -> V+ > V-
        */
        swap_input_capture_edge_trigger_mode(); // Hence, the reason of inverting the edge trigger mode
    }
    else
    {
        unset_bit(ACSR, ACIC); // To disable the analog comparator for the input Capture Unit (Use ICP1 pin)

        swap_input_capture_edge_trigger_mode(); // Because the previous input was the analog comparator, the edge trigger logic needs to be inverted.
    }
    set_bit(TIFR1, ICF1); // Clearing ICU interrupt flag
}


bool change_PWM_duty_cycle(volatile uint8_t *chosen_PWM_channel, uint8_t duty_cycle_value)
{
    /*
    Function specific to the ATMega328p, non portable.

    Choosing the selected PWM channel will modify the compare match register to change the timing on the counter match event.

    CURRENTLY ONLY FUNCTIONA FOR 8-bit counter/timers

    -------
    Input:
        volatile uint8_t *chosen_PWM_channel
            An int containing the name of the register that modifies the compare match register value.
        uint8_t duty_cycle_value
            An int containing the new value of the compare match register.

    */
   #define PIN_6_PWM OCR0A // PD6 - Arduino Pin D6
   #define PIN_5_PWM OCR0B // PD5 - Arduino Pin D5
   #define PIN_9_PWM OCR1AH // PB1 - Arduino Pin D9
   #define PIN_10_PWM OCR0AH // PB0 - Arduino Pin D10
   #define PIN_11_PWM OCR2A // PB3 - Arduino Pin D11
   #define PIN_3_PWM OCR2B // PD3 - Arduino Pin D3

    // Chosen PWM port output
    if (chosen_PWM_channel == OCR1AH)
    {
        write_16_bit_with_8_bit(duty_cycle_value, OCR1AH, OCR1AL);
    }
    else if (chosen_PWM_channel == OCR1BH)
    {
        write_16_bit_with_8_bit(duty_cycle_value, OCR1BH, OCR1BL);
    }
    else
    {
        *chosen_PWM_channel = duty_cycle_value;
    }
    return true;
}

volatile static uint32_t global_milliseconds_counter = 0;

void delay_milliseconds(uint16_t delay_time)
{
    /*
    Classic delay function to freeze the code execution during the specified time (in milliseconds).

    This functions works along to a ISR. Interrupts must be activated.

    -------
    Input:
        uint16_t delay_time
            An integer specifiyng the desired delay time for stopping the code execution
    */

    uint32_t start_time = global_milliseconds_counter;

    while ((global_milliseconds_counter - start_time) < delay_time);
    // The global time counter interupt will be continuously called.
}

static uint8_t interrupt_counter_compare_A = 0;
static uint8_t interrupt_counter_compare_B = 0;


static uint8_t interrupt_counter0_compare_A = 0;

static bool increase_duty_cycle = true;
static uint8_t up_limit_duty_cycle_0 = 0x90;
static uint8_t down_limit_duty_cycle_0 = 0x0;

#define SWITCH_A PORTB, PORTB4 // Arduino Pin 12
#define BUTTON_A PORTD, PORTD2 // Arduino Pin 2
#define BUTTON_B PORTD, PORTD3 // Arduino Pin 3

#define ON_BOARD_LED PORTB, PORTB5
#define LED_A PORTD, PORTD5 // Arduino Pin 3
#define LED_B PORTD, PORTD4 // Arduino Pin 4

#define STATUS_SWITCH_A PINB, PINB4
#define STATUS_BUTTON_A PIND, PIND2
#define STATUS_BUTTON_B PIND, PIND3


void setup_gpio()
{
    /*
    Function specific to the ATMega328p, non portable.

    This function set ups and initilizes the digital pins to be used on the ATmega328p
    */

    unset_bit(DDRB, DDB4); // Setting port SWITCH_A as an input
    unset_bit(DDRD, DDD2); // Setting BUTTON_A port as an input -> Triggerd by external interrupt 0
    unset_bit(DDRD, DDD3); // Setting BUTTON_B port as an input -> Triggerd by external interrupt 1
    set_bit(DDRB, DDB5); // Setting port ON_BOARD_LED as an output
    //set_bit(DDRD, DDD5); // Setting port LED_A as an output
    set_bit(DDRD, DDD4); // Setting port LED_B as an output

    //unset_bit(SWITCH_A); // Unsetting pull-up resistor
    unset_bit(BUTTON_A); // Unsetting pull-up resistor
    unset_bit(BUTTON_B); // Unsetting pull-up resistor
    unset_bit(ON_BOARD_LED); // Default output value (LED OFF)
    //unset_bit(LED_A); // Default output value (LED OFF)
    unset_bit(LED_B); // Default output value (LED OFF)
}

#define UART_BAUD_RATE 9600
#define BAUD_RATE_9600 103
#define BAUD_RATE_38400 0x019
#define BAUD_RATE_76800 0x00C

static ring_buffer_t ring_buffer = {0};
static uint8_t data_buffer[BUFFER_SIZE] = {'\0'};

static uint8_t dummy_byte = '\0';
static uint8_t input_string[12] = {'\0'};

static uint16_t instantaneous_input_capture_count = 0;





void setup_UART(bool receive_data, bool transmit_data, uint16_t baud_rate_mode)
{
    /*
    Function specific to the ATMega328p, non portable.

    This functions sets up UART protocol for reception and transmission of 8 bits data.
    As default setting, the protocol is set with Even parity mode, 1 stop bit, and normal transmission speed.

    Use the following statements to define each ISR function respectively
        - "ISR(USART_RX_vect){}" belongs to the vector "USART_RX_vect" (vector 19, address 0x0024).
            Main ISR used to handle and store incoming data in a data buffer.
                NOTE: If data reception is expected, this ISR must be present.
        - "ISR(USART_UDRE_vect){}" belongs to the vector "USART_UDRE_vect" (vector 20, address 0x0026).
        - "ISR(USART_TX_vect){}" belongs to the vector "USART_TX_vect" (vector 21, address 0x0028).

    -------
    Input:
        bool receive_data
            A boolean speciying if data recepction is desired.
        bool transmit_data
            A boolean speciying if data transmission is desired.
        uint16_t baud_rate_mode
            An integer specifying the baud rate speed to set according to the MCU datasheet and the on-board oscillator.
            For 16MHz crystals operating at normal transmission speed:
                - 2400 bps ->0x1A0
                - 4800 bps ->0x0CF
                - 9600 bps ->0x067
                - 14.4k bps ->0x044
                - 19.2k bps ->0x033
                - 28.8k bps ->0x022
                - 38.4k bps ->0x019
                - 57.6k bps ->0x010
                - 76.8k bps ->0x00C
                - 115.2k bps ->0x008
                - 230.4k bps ->0x003
                - 250k bps ->0x003
                - 0.5M bps -> 0x001
                - 1M bps -> 0x000
            According to the desired mode, assign the Hex value to "uint16_t baud_rate_mode".
    */

    write_16_bit_with_8_bit(baud_rate_mode, UBRR0H, UBRR0L); // Writing Baud Rate value

    /* In this case, the ring buffer data structure will be used to storre only incoming data (to stack in an array every incoming message before it
        is flushed at the moment of reading the content of the memory)*/
    setup_ring_buffer(&ring_buffer, data_buffer, BUFFER_SIZE);

    // Selecting USART mode
    unset_bit(UCSR0C, UMSEL01); // To select USART mode: UART (Asynchronous)
    unset_bit(UCSR0C, UMSEL00); // To select USART mode: UART (Asynchronous)

    unset_bit(UCSR0C, UCPOL0); // Clock Polarity. Written to zero because of UART mode

    // Setting baud rate
    unset_bit(UCSR0A, U2X0); // U2X0 = 0: Normal USART Transmission Speed

    // Selecting Parity mode for transmission and reception (for error detection)
    set_bit(UCSR0C, UPM01); // To select Parity mode: Even
    unset_bit(UCSR0C, UPM00); // To select Parity mode: Even

    // Set number of stop bits for the frame (1 is equal to two stop bits)
    unset_bit(UCSR0C, USBS0); // Stop Bit Select to 1 bit

    // Setting up message bit-lenght
    unset_bit(UCSR0B, TXB80); // Transmit Data Bit 8 (Ninth bit for sending 9-bit messages)
        // Selecting 8-bit size for transmission characters
    unset_bit(UCSR0B, UCSZ02);
    set_bit(UCSR0C, UCSZ01);
    set_bit(UCSR0C, UCSZ00);


 if (receive_data == true)
    {
        set_bit(UCSR0B, RXEN0); // Enable data reception
    }
    if (transmit_data == true)
    {
        set_bit(UCSR0B, TXEN0); // Enable data transmission
    }
    // If receive and transmit are enable, UART is in full duplex mode

    set_bit(UCSR0B, RXCIE0); // To enable RX Complete Interrupt
    unset_bit(UCSR0B, TXCIE0); // To enable TX Complete Interrupt
    unset_bit(UCSR0B, UDRIE0); // To enable USART Data Register Empty Interrupt

}

bool receive_USART(void)
{
    /*
    Function specific to the ATMega328p, non portable.

    Called for storing an incoming byte from the UART receive/transmit register.
    This function receives one byte at a time from the USART buffer, and saves it in a ring buffer data structure.

    -------
    Output:
        bool
            A boolean specifying if the input data from the buffer was stored (in case the ring buffer has available space).
            If false, the ring buffer is full and incoming data will be lost.
    */
    return write_ring_buffer(&ring_buffer, (uint8_t*) UDR0);
}

bool read_byte_USART(uint8_t* byte_read)
{
    /*
    This function reads one byte at a time of the input buffer data structure, and it saves afterwards the value in uint8_t* byte_read.
    When the data from the buffer is read, its space is freed.

    -------
    Input:
        uint8_t* byte_read
            A pointer of an integer where the read byte will be stored.
    -------
    Output:
        bool
            A boolean specifying true or false if something was read and stored from the buffer.
    */
    return read_ring_buffer(&ring_buffer, byte_read);
}

uint8_t read_USART(uint8_t read_char_array[], uint8_t char_array_size, uint8_t length_to_read)
{
    /*
    This function calls "bool read_byte_USART(uint8_t* byte_read)" the number of times the expected message length, to save in each call the data.
    This function may be used when the message length is known or when the loaded char array has enough space.

    -------
    Input:
        uint8_t read_char_array[]
            A char array where the data from the input buffer will be stored.
        uint8_t char_array_size
            An integer specifying the total length of "uint8_t read_char_array[]".
        uint8_t length_to_read
            An integer specifying the intended or maximum quantity of characters to read from the input buffer.
            Its value should be less or equal to "uint8_t char_array_size".

    -------
    Output:
        uint8_t
            An integer specifying the number of bytes read, freed and stored from the input buffer.
            NOTE: If the return value is greater to 0, the function works to detect and save any data from the input buffer.
    */

   if (length_to_read == 0 || (length_to_read > char_array_size - 1)) // Taking into account the null character at the end.
   {
    return 0; // Nothing to read
   }
   
   for (uint8_t scan_index = 0; scan_index < length_to_read; scan_index++)
   {
    if (read_byte_USART(&read_char_array[scan_index]) == false)
    {
        // This condition happens when the lenght of the expected message cannot be obtained because the buffer is empty.
        return scan_index;
    }
   }
   return length_to_read;
}

void transmit_byte_USART(uint8_t char_to_transmit)
{
    /*
    Function specific to the ATMega328p, non portable.

    This function sends the input byte "uint8_t char_to_transmit" to the UART receive/transmit register for transmission.
    This function was made to be used by a high level function lo send strings.

    -------
    Input:
        uint8_t char_to_transmit
            A char containing the byte to transmit/send to the output register.
    */

    while (!status_bit(UCSR0A, UDRE0)); // Waits until transmit buffer is ready to receive data
    // Transmiting a message through UART protocol
    *UDR0 = char_to_transmit;           
}

void transmit_USART(uint8_t transmit_string[])
{
    /*
    This function calls multiple times "void transmit_byte_USART(uint8_t char_to_transmit)" for transmitting the input string.

     -------
    Input:
        uint8_t transmit_string[]
            An array of chars containing the string (or message) to transmit/send to the output register.
    */
   for (uint8_t char_index = 0; !(transmit_string[char_index] == '\0'); char_index++)
   {
    // This loop breaks once the null character is found.
        transmit_byte_USART(transmit_string[char_index]);
   }
}

bool setup_ADC(bool external_ref_voltage, bool set_auto_trigger, uint8_t clock_prescaler_mode, uint8_t trigger_source)
{
    /*
    Function specific to the ATMega328p, non portable.

    This function sets up the ADC peripherial to read analog signals.
    The default settings will give right adjust result by default, and ADC ISR is desactivated.

    Use the following statements to define each ISR function respectively
        - "ISR(ADC_vect){}" belongs to the vector "ADC_vect" (vector 22, address 0x002A).
            Use this ISR when an ADC conversion is done.
    
    
    List of clock prescalers mode table:
        - 0x0 = 2
        - 0x1 = 2
        - 0x2 = 4
        - 0x3 = 8
        - 0x4 = 16
        - 0x5 = 32
        - 0x6 = 64
        - 0x7 = 128

    List of trigger sources mode table:
        - 0x0 = Free running mode
        - 0x1 = Analog comparator
        - 0x2 = External interrupt request 0
        - 0x3 = Timer/Counter 0 compare match A
        - 0x4 = Timer/Counter 0 overflow
        - 0x5 = Timer/Counter 1 compare match B
        - 0x6 = Timer/Counter 1 overflow
        - 0x7 = Timer/Counter 1 capture event

    -------
    Input:
        bool external_ref_voltage
            A boolean for specifying if an external voltage reference is present.
        bool set_auto_trigger
            A boolean for specifying if auto trigger mode is active.
        uint8_t clock_prescaler_mode
            An integer specifying the chosen prescaler mode to divide the ADC clock frequency (see table above to choose an hex value).
        uint8_t trigger_source
            An interger specifying the trigger source for starting a new ADC conversion (see table above to choose an hex value).

    -------
    Output:
        bool
            A boolean specifying true or false if the setup of the ADC was succesful.
    */

   if ((clock_prescaler_mode > 0x7) | (trigger_source > 0x7))
    /*  Checking if input prescaler mode and trigger source are valid, to prevent applying
        a mask that can corrupt the bits of the rest of the register. */
   {
    return false;
   }
   // Initializing registers
   *ADMUX = (uint8_t) 0;
   *ADCSRA = (uint8_t) 0;
   *ADCSRB = (uint8_t) 0;
   *DIDR0 = (uint8_t) 0;

   // Setting voltage reference for ADC
   unset_bit(ADMUX,REFS1);
   if (external_ref_voltage == true)
   {
        unset_bit(ADMUX,REFS0);
   }
   else
   {
        set_bit(ADMUX,REFS0);
   }
    
    unset_bit(ADMUX,ADLAR); // Right Adjust Result by default

    if (set_auto_trigger == true)
    {
        set_bit(ADCSRA, ADATE);
    }
    else
    {
        unset_bit(ADCSRA, ADATE);
        trigger_source = (uint8_t) 0x0; // Default case when auto trigger mode is disabled
    }
    
    unset_bit(ADCSRA, ADIE); // Activate ISR when ADC conversion is done
    unset_bit(ADCSRA, ADIF); // Initialize interrupt flag
    
    // Setting ADC clock prescaler (0x7 is a mask for clearing all the possible values)
    *ADCSRA = (*ADCSRA & ~((uint8_t) 0x7)) | clock_prescaler_mode; // To only write the desired value bits without overwriting the register
    
    // Setting ADC source trigger mode (0x7 is a mask for clearing all the possible values)
    *ADCSRB = (*ADCSRB & ~((uint8_t) 0x7)) | trigger_source; // To only write the desired value bits without overwriting the register
    
    set_bit(ADCSRA, ADEN); // To enable ADC
    set_bit(ADCSRA, ADSC); // Initialize ADC running first conversion

    return true;
}

bool change_ADC_channel(uint8_t ADC_channel_num)
{
    /*
    Function specific to the ATMega328p, non portable.

    This function selects an ADC channel where analog signals will be read and processsed by the ADC hardware.
    Call this function everytime a channel is changed.
    The ATMega328p MCU has up to 9 ADC channels.
    -------
    Input:
        uint8_t ADC_channel_num
            An integer specifying the ADC channel to select accordingly to the need.

    -------
    Output:
        bool
            A boolean specifying true or false if the input ADC channel is valid.
    */ 
   if (ADC_channel_num > 0xF)
   {
        return false; // To prevent applying a mask that can corrupt the bits of the rest of the register.
   }
   // (0xF is a mask for clearing all the possble values)

   *ADMUX = (*ADMUX & ~((uint8_t) 0xF)) | ADC_channel_num; // To only write the desired value bits without overwriting the register
   *DIDR0 = 0; // Resetting register

   if (ADC_channel_num <= 0x5)
   {
        set_bit(DIDR0, ADC_channel_num);
   }

    set_bit(ADCSRA, ADSC); // Starting ADC initialization for chosen channel

    while (!status_bit(ADCSRA, ADIF)); // Waits until ADC conversion is done
    set_bit(ADCSRA, ADIF);

    return true;
}

void read_channel(uint16_t *byte_ADC)
{
    /*
    Function specific to the ATMega328p, non portable.

    This function was written for right adjusted data on the ADC data registers (high and low respectively).
    This function needs to be called everytime an ADC conversion is needed. Moreover, the reading of the ADC data registers
    frees their content for further conversions.

    If multiple conversion need to be done without changing the current chosen ADC channel, this function may be called multiple times.
    
    -------
    Input:
        uint16_t *byte_ADC
            An integer pointer where the analog-to-digital numeric value will be stored.
    */ 

   set_bit(ADCSRA, ADSC); // Starting ADC conversion
   
   while (!status_bit(ADCSRA, ADIF)); // Waits until ADC conversion is done
   set_bit(ADCSRA, ADIF);

   // 8 bit byte : high byte ADC = *ADCH;
   // 8 bit byte : low byte ADC = *ADCL;

    *byte_ADC = *ADCL | (*ADCH << 8);
}

bool read_ADC_channel(uint8_t ADC_channel_num, uint16_t *byte_ADC)
{
    /*
    This function is meant to be called when different ADC channels need to be called consecutively (Intended to be use for Free running mode).
    Note: Everytime a channel is changed, an additional conversion needs to be done to discard faulty or misleading information provided during the channel setup.

    If only one channel is meant to be called multiple times, use functions:
        - "bool change_ADC_channel(uint8_t ADC_channel_num)" one time for selecting the channel. 
        - "bool read_channel(uint16_t *byte_ADC)" Multiple times according to the need.
    This is recommended for faster and better performance.
    
    -------
    Input:
        uint8_t ADC_channel_num
            An integer specifying the ADC channel to select accordingly to the need.
        uint16_t *byte_ADC
            An integer pointer where the analog-to-digital numeric value will be stored.
    
    -------
    Output:
        bool
           A boolean specifying true or false if the input ADC channel is valid.
    */

    if (change_ADC_channel(ADC_channel_num) == false)
    {
        return false;
    }

    uint16_t dummy_reading = 0;
    read_channel(&dummy_reading); // First reading to be discarded because of channel initialization

    read_channel(byte_ADC);
    
    return true;
}


volatile static bool ICU_t_or_AC_f_pin = false;
volatile static bool counter_ICU_overflow = false;
volatile static uint8_t edges_measured = 0;
volatile static bool completed_signal_frame_edges = false;

volatile static uint32_t icu_read_signal_edges[3]={0};
#define COUNTER_ICU_TOP (uint16_t) 0xFFFF

int main(int argc, char *argv[])
{
    cli(); // Deactivates interruptions
    setup_gpio();

    uint8_t counter0_limit = 249; // An overflow each 1000 microseconds (Verified with oscilloscope)
    setup_8_bit_timer_counter_0(0x0, 0x2, 0x3, false, counter0_limit, 0x0, 0x2); // To count time

    
    setup_8_bit_timer_counter_2(0x1, 0x3, 0x6, false, interrupt_counter0_compare_A, 0x0, 0x2);

    setup_UART(true, true, BAUD_RATE_38400);
    //setup_external_interrupt(false, 0x2); // Activating external interrupt-0
    //setup_external_interrupt(true, 0x2); // Activating external interrupt-1
    //activate_pin_change_interrupt(0); // Activating Pin change interrupt for PB0

    setup_16_bit_timer_counter_1(0x0, 0x4, 0x2, false, COUNTER_ICU_TOP, 0x00, 0x1);
    setup_input_capture_unit(true, true, true); // Rising detection is a must!
    
    //setup_ADC(false, true, 0x4, 0x0); // Using internal voltage reference (VCC = 5v)
    //change_ADC_channel(0x1);
    
    sei(); // Enables interruptions

    uint32_t current_time = global_milliseconds_counter;
    uint32_t previous_time = current_time;


    int32_t signal_period_raw[2] = {0};
    int32_t signal_duty_cycle_raw[2] = {0};
    uint8_t system_clock_prescaler = 8;
    uint8_t chosen_signal_channel = 0;

    transmit_USART("Starting\n");
    delay_milliseconds(3000);

    
    while (1)
    {
        
        current_time = global_milliseconds_counter;

        // log_PWM_differential_duty_cycle_signals();
        
        
        if (completed_signal_frame_edges == true)
        {
            cli(); // Desactivates interruptions

            signal_period_raw[chosen_signal_channel] =  (icu_read_signal_edges[2] - icu_read_signal_edges[0]);
            signal_duty_cycle_raw[chosen_signal_channel] = (icu_read_signal_edges[1] - icu_read_signal_edges[0]);

            if (chosen_signal_channel == 0)
            {
                chosen_signal_channel = 1;
                sei(); // Enables interruptions
            }
            else
            {
                chosen_signal_channel = 0;

                char int_to_char_val[20] = {'\0'};
                char calculation_chunk[20] = {'\0'};
                char calculation_string[100] = {'\0'};
       
                // To write first channel 
                itoa(signal_period_raw[0], calculation_string, 10);
                //set_integer_string_fixed_length(int_to_char_val, calculation_string, 8);
                replace_char_input_string(calculation_string, '\0', ';', sizeof(calculation_string));

                itoa(signal_duty_cycle_raw[0], calculation_chunk, 10);
                //set_integer_string_fixed_length(int_to_char_val, calculation_chunk, 8);
                replace_char_input_string(calculation_chunk, '\0', ';', sizeof(calculation_chunk));
                strcat(calculation_string, calculation_chunk);

                itoa(signal_period_raw[1], calculation_chunk, 10);
                //set_integer_string_fixed_length(int_to_char_val, calculation_chunk, 8);
                replace_char_input_string(calculation_chunk, '\0', ';', sizeof(calculation_chunk));
                strcat(calculation_string, calculation_chunk);

                itoa(signal_duty_cycle_raw[1], calculation_chunk, 10);
                //set_integer_string_fixed_length(int_to_char_val, calculation_chunk, 8);
                replace_char_input_string(calculation_chunk, '\0', '\n', sizeof(calculation_chunk));
                strcat(calculation_string, calculation_chunk);

                sei(); // Enables interruptions
                transmit_USART(calculation_string);
            }   
            completed_signal_frame_edges = false;
        }
        
        /*
        char to_print[11]={};
        itoa(current_time, to_print, 10);
        replace_char_input_string(to_print, '\0', '\n');
        transmit_USART(to_print);
        */

        /*
        if ((current_time - previous_time) >= 500)
        {
            
            previous_time = current_time;

            char adc_read[5]={'\0'};
            char message_z[6]={'\0'};
            char message_full[21]={'\0'};

            uint16_t my_data = 0;

            
            char my_string[10] = {'\0'};

            utoa(test_int, my_string, 10);

            

            replace_char_input_string(my_string, '\0', '\n');
            transmit_USART(my_string);
            */

            /*
            read_ADC_channel(0x4, &my_data);
            utoa(my_data, adc_read, 10);
            set_integer_string_fixed_length(adc_read, message_full, 4);
            replace_char_input_string(message_full, '\0', ';');
            
            read_ADC_channel(0x3, &my_data);
            utoa(my_data, adc_read, 10);
            set_integer_string_fixed_length(adc_read, message_z, 4);
            replace_char_input_string(message_z, '\0', ';');
            strcat(message_full, message_z);

            read_ADC_channel(0x4, &my_data);
            utoa(my_data, adc_read, 10);
            set_integer_string_fixed_length(adc_read, message_z, 4);
            replace_char_input_string(message_z, '\0', ';');
            strcat(message_full, message_z);
            
            read_ADC_channel(0x5, &my_data);
            utoa(my_data, adc_read, 10);
            set_integer_string_fixed_length(adc_read, message_z, 4);
            replace_char_input_string(message_z, '\0', '\n');
            strcat(message_full, message_z);
            transmit_USART(message_full);
            
        }
            */
    }
    return 0;
}

// Beginning of ISRs declarations

ISR(TIMER0_COMPA_vect)
{
    /*
    Function specific to the ATMega328p, non portable.

    This interrupt service routine belongs to the vector "TIMER0_OVF_vect" (vector 17, address 0x0020).
    
    This routine will be called every 1000 microseconds.

    Note: This ISR needs to be brief
    */
   global_milliseconds_counter ++;
}

ISR(TIMER1_CAPT_vect)
{
    /*
    Function specific to the ATMega328p, non portable.

    This interrupt service routine belongs to the vector "TIMER1 CAPT" (vector 11, address 0x0014).
    
    This routine will be called whenever the input capture unit trigger mode matches the signal input.

    Note: This ISR needs to be brief
    */

    // Save timestamp
    read_16_bit_with_8_bit(ICR1L, ICR1H, &instantaneous_input_capture_count);

    if (completed_signal_frame_edges == false)
    {
        if (counter_ICU_overflow == true)
        {
            icu_read_signal_edges[edges_measured] = instantaneous_input_capture_count + COUNTER_ICU_TOP;
        }
        else
        {
            icu_read_signal_edges[edges_measured] = instantaneous_input_capture_count;
        }

        edges_measured++;

        if (edges_measured == 3)
        {
            // End of signal frame reached
            edges_measured = 0;
  
            if (ICU_t_or_AC_f_pin == true)
            {
                change_input_capture_unit_trigger_source(true);
                ICU_t_or_AC_f_pin = false;
            }
            else
            {
                change_input_capture_unit_trigger_source(false);
                ICU_t_or_AC_f_pin = true;
            }

            completed_signal_frame_edges = true;
            counter_ICU_overflow = false;
        }
        else
        {
            swap_input_capture_edge_trigger_mode();
        }
    }
}

ISR(TIMER1_OVF_vect)
{
    /*
    Function specific to the ATMega328p, non portable.

    This interrupt service routine belongs to the vector "TIMER1_OVF_vect" (vector 14, address 0x001A).
    This routine will be called when timer/counter1 overflows.
    */
   counter_ICU_overflow = true;  
}


ISR(TIMER2_COMPA_vect)
{
    /*
    Function specific to the ATMega328p, non portable.

    This interrupt service routine belongs to the vector "TIMER2_COMPA_vect" (vector 7, address 0x000E).
    
    This routine will :😎
    */
   

    interrupt_counter_compare_A++;
    
    /*
    if (status_bit(STATUS_SWITCH_A) == false)
    {
        if (interrupt_counter_compare_A == 25)
        {   
            //toggle_bit(LED_B); // Swap LED state

            interrupt_counter_compare_A = 0;
        } 
    }
    */

    if (increase_duty_cycle == true)
    {
        if ((interrupt_counter0_compare_A) < up_limit_duty_cycle_0)
        {
            interrupt_counter0_compare_A++; // Called each 10ms
            change_PWM_duty_cycle(PIN_11_PWM ,interrupt_counter0_compare_A);
            return;
        }
        else
        {
            increase_duty_cycle = false;
        }
    }
    if (increase_duty_cycle == false)
    {
        if ((interrupt_counter0_compare_A) > down_limit_duty_cycle_0)
        {
            interrupt_counter0_compare_A--; // Called each 10ms
            change_PWM_duty_cycle(PIN_11_PWM, interrupt_counter0_compare_A);
            return;
        }
        else
        {
            increase_duty_cycle = true;
        }
    }
}

/*
ISR(TIMER0_COMPB_vect)
{
    interrupt_counter_compare_B++;

    if (interrupt_counter_compare_B == 100)
    {
        toggle_bit(LED_A); // Set LED ON
        interrupt_counter_compare_B = 0;
    }
}
*/

ISR(USART_RX_vect)
{
   /*
   Function specific to the ATMega328p, non portable.

    This interrupt service routine belongs to the vector "USART_RX_vect" (vector 19, address 0x0024).
    
    Routine called when there's incoming data to USART hardware.
    */
    if (receive_USART() == false) // To be called everytime an incoming byte must be stored.
    {
        // Buffer is full and not available
        set_bit(ON_BOARD_LED);
    }
    else
    {
        unset_bit(ON_BOARD_LED);
    }
}

ISR(INT0_vect)
{
    /*
    Function specific to the ATMega328p, non portable.

    This interrupt service routine belongs to the vector "INT0_vect" (vector 2, address 0x0002).
    
    This routine will be called when BUTTON_A is pressed.
    */

    unset_bit(ON_BOARD_LED);
    toggle_bit(LED_B);
    
}

ISR(INT1_vect)
{
    /*
    Function specific to the ATMega328p, non portable.

    This interrupt service routine belongs to the vector "INT1_vect" (vector 3, address 0x0004).
    
    This routine will be called when BUTTON_B is pressed.
    */

    set_bit(ON_BOARD_LED);
    toggle_bit(LED_B);
    
    
}

ISR(PCINT0_vect)
{
    /*
    Function specific to the ATMega328p, non portable.

    This interrupt service routine belongs to the vector "PCINT0_vect" (vector 4, address 0x0006).
    
    This routine will be called when SWITCH A state changes.
    */
   
   if (status_bit(STATUS_SWITCH_A) == false)
   {
    toggle_bit(LED_B);
   }
   else
   {
    
   }
}


/*
ISR(TIMER1_CAPT_vect)
{
    
    //Function specific to the ATMega328p, non portable.

    //This interrupt service routine belongs to the vector "TIMER1_CAPT_vect" (vector 11, address 0x0014).
    
    //This routine will be called when the input capture from the 16-bit counter/timer_1 peripherial is triggered.
    //

   read_16_bit_with_8_bit(ICR1L, ICR1H, &instantaneous_input_capture_count); //🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔
}
*/


/*
ISR(INT1_vect)
{
   
    if (status_bit(STATUS_SWITCH_A) == false)
    {
        // Replies back the input
        if (read_byte_USART(&dummy_byte))
        {
            transmit_byte_USART(dummy_byte);
        }
    }
    else
    {
        // Answers according to the input
        uint8_t characters_read = read_USART(input_string, 12, 6);
        if (characters_read > 0)
        {
            if (!strcmp(input_string, "Marina"))
            {
                transmit_USART("del Mar\n");
            }
            else if (!strcmp(input_string, "Melody"))
            {
                transmit_USART("Nombre lindo\n");
            }
            else if (!strcmp(input_string, "Camila"))
            {
                transmit_USART("Bandida xd\n");
            }
            else
            {
                transmit_USART("Ninguna opcion\n");
            }
        }
        else
        {
             transmit_USART("No hay nada xd\n");
            // This condition happens when the buffer is empty and hence, there's nothing to read
        }
    }
    
}
*/


// To compile the code and obtain .hex file
/*
C:\Users\A0089761\scoop\apps\avr-gcc\14.1.0\bin\avr-gcc.exe -mmcu=atmega328p -Os main_backup_one_file.c -o micro_demo.elf
C:\Users\A0089761\scoop\apps\avr-gcc\14.1.0\bin\avr-objcopy.exe -O ihex -R .eeprom micro_demo.elf micro_demo.hex
*/

// To flash the hex file into the microcontroller:
/*
C:\Users\A0089761\scoop\apps\avr-gcc\14.1.0\bin\avrdude.exe -c arduino -p m328p -U flash:w:micro_demo.hex -P COM6
*/