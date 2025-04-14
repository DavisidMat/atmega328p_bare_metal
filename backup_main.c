//#include <avr/io.h>

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "ATmega328p_registers.h"


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

bool replace_char_input_string(char *input_string, char char_to_modify, char char_to_place)
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
        // Value swap because the character to replace was found
        *char_position = char_to_place;

        return true;
    }

    // No character found
    return false;
}

void write_16_bit_into_8_bit(uint16_t value_to_write, volatile uint8_t *high_byte, volatile uint8_t *low_byte)
{
     /*
    This function writes a 16-bit value into two 8-bit registers.

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
    }
    strcat(output_string, integer_string); // Appending zeros string to integer string for output.
}



bool setup_external_interrupt(bool enable_external_int_0, bool enable_external_int_1, uint8_t trigger_mode)
{
    /*
    Function specific to the ATMega328p, non portable.
    
    This function sets up the MCU for handling external interrupts coming from pins INT0 and INT1.

    NOTE: Its respective ISR function must be called and defined later.
    Use following statements to define each ISR function respectively
        - "ISR(INT0_vect){}" belongs to the vector "INT0_vect" (vector 2, address 0x0002).
        - "ISR(INT1_vect){}" belongs to the vector "INT1_vect" (vector 3, address 0x0004).

    -------
    Input:
        bool enable_external_int_0
            A boolean specifying if the external interrupt 0 shall be activated.
        bool enable_external_int_1
            A boolean specifying if the external interrupt 1 shall be activated.
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

   // Setting chosen trigger mode for both external interrupts:
   *EICRA = (*EICRA & ~((uint8_t) 0x0)) | ((trigger_mode << 2) | trigger_mode);
        // "(trigger_mode << 2)" sets INT1 (ISC11 and ISC10) and "trigger_mode" sets INT0 (ISC01 and ISC00)

    if (enable_external_int_0 == true)
    {
       set_bit(EIMSK, INT0); // Activating external interrupt-0 hardware
    }

    if (enable_external_int_1 == true)
    {
       set_bit(EIMSK, INT1); // Activating external interrupt-1 hardware
    }
    
    // Initializing Flag register
    *EIFR = (uint8_t) 0;

    return true; // Succesfull setup, no expected strange behavior.
}


void setup_8_bit_timer_interrupt()
{
    /*
    Function specific to the ATMega328p, non portable.

    To be defined 😎🤣.
 
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
 
    // Setting up interrupt routine based on the 8-bit timer/counter
    *TCNT2 = (uint8_t) 0; // Initializing counter register. THIS VALUE MUST BE MODIFIED BEFORE SETTING ANY OTHER REGISTER
    *TIFR2 = (uint8_t) 0; // Initializing Flag register
    *TCCR2A = (uint8_t) 0;
    *TCCR2B = (uint8_t) 0;
        // Setting up mode 2: CTC (Clear timer on compare match) according to the datasheet
    unset_bit(TCCR2B, WGM22);
    set_bit(TCCR2A, WGM21);
    unset_bit(TCCR2A, WGM20);
        // Activating clock prescaler to 1024 division factor according to the datasheet
    set_bit(TCCR2B, CS22);
    set_bit(TCCR2B, CS21);
    set_bit(TCCR2B, CS20);
 
    // Desired value to be compared with the timer
    *OCR2A = (uint8_t) 157;// Sets period to 10 ms;
 
 
 
    // Activate output compare match A Interrupts
    unset_bit(TIMSK2, OCIE2B);
    set_bit(TIMSK2, OCIE2A);
    unset_bit(TIMSK2, TOIE2);
    
    //*TCNT2 = (uint8_t) 0; // WHY IT CREATES A BUG IF THE VALUE IS UPDATED AT THE END???????❌❌❌❌❌❌❌❌
 
}

void setup_8_bit_timer_PWM()
{
    /*
    Function specific to the ATMega328p, non portable.

    To be defined 😎🤣.

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

    // Setting up PWM from 8-bit synchronous timer/counter (OC0A/PD6 and OC0B/PD5 outputs)
        // Setting up mode 7: Fast PWM (with custom top value according to OCR0A. i.e. Custom frequency) according to the datasheet
    set_bit(TCCR0B, WGM02);
    set_bit(TCCR0A, WGM01);
    set_bit(TCCR0A, WGM00);
        // Setting compare output mode for Fast PWM mode to non-inverting mode
    unset_bit(TCCR0A, COM0A1);
    unset_bit(TCCR0A, COM0A0);
    set_bit(TCCR0A, COM0B1);
    set_bit(TCCR0A, COM0B0);
        // Activating clock prescaler to 256 division factor according to the datasheet
    set_bit(TCCR0B, CS02);
    unset_bit(TCCR0B, CS01);
    unset_bit(TCCR0B, CS00);
        // Define TOP counter value (Limit value to count);
    *OCR0A = (uint8_t) 100; // To obtain a 330 Hz with a clock frequency of 62500 Hz

    *OCR0B = (uint8_t) 150; // Default value for Output compare register B (set duty cycle) 😍🤪
}





static uint8_t interrupt_counter_compare_A = 0;
static uint8_t interrupt_counter_compare_B = 0;

static uint8_t increase_duty_cycle = 1;
static uint8_t up_limit_duty_cycle_0 = 0x90;
static uint8_t down_limit_duty_cycle_0 = 0x0;

#define SWITCH_A PORTB, PORTB0 // Arduino Pin 8
#define BUTTON_A PORTD, PORTD2 // Arduino Pin 2
#define BUTTON_B PORTD, PORTD3 // Arduino Pin 3

#define ON_BOARD_LED PORTB, PORTB5
#define LED_A PORTD, PORTD5 // Arduino Pin 3
#define LED_B PORTD, PORTD7 // Arduino Pin 7

#define STATUS_SWITCH_A PINB, PINB0
#define STATUS_BUTTON_A PIND, PIND2
#define STATUS_BUTTON_B PIND, PIND3


void setup_gpio()
{
    /*
    Function specific to the ATMega328p, non portable.

    This function set ups and initilizes the digital pins to be used on the ATmega328p
    */

    unset_bit(DDRB, DDB0); // Setting port SWITCH_A as an input
    unset_bit(DDRD, DDD2); // Setting BUTTON_A port as an input
    unset_bit(DDRD, DDD3); // Setting BUTTON_B port as an input -> Triggerd by external interrupt 1
    set_bit(DDRB, DDB5); // Setting port ON_BOARD_LED as an output
    set_bit(DDRD, DDD5); // Setting port LED_A as an output
    set_bit(DDRD, DDD7); // Setting port LED_B as an output

    unset_bit(SWITCH_A); // Unsetting pull-up resistor
    unset_bit(BUTTON_A); // Unsetting pull-up resistor
    unset_bit(BUTTON_B); // Unsetting pull-up resistor
    unset_bit(ON_BOARD_LED); // Default output value (LED OFF)
    unset_bit(LED_A); // Default output value (LED OFF)
    unset_bit(LED_B); // Default output value (LED OFF)
}

#define UART_BAUD_RATE 9600
#define BAUD_RATE_9600 103

static ring_buffer_t ring_buffer = {0};
static uint8_t data_buffer[BUFFER_SIZE] = {'\0'};

static uint8_t dummy_byte = '\0';
static uint8_t input_string[12] = {'\0'};




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

    write_16_bit_into_8_bit(baud_rate_mode, UBRR0H, UBRR0L); // Writing Baud Rate value

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


int main(int argc, char *argv[])
{
    setup_gpio();
    
    /*
    The execution of the interrupt routine depends on the state of register TIFR0 wich depends on TIMSK0, TCCR0B, TCCR0A and SREG (previously configured)
    */
    setup_8_bit_timer_interrupt();
    setup_8_bit_timer_PWM();
    setup_UART(true, true, BAUD_RATE_9600);
    setup_external_interrupt(false, true, 0x2);

    setup_ADC(false, true, 0x4, 0x0); // Using internal voltage reference (VCC = 5v)
    
    //cli(); // Deactivates interruptions
    

    //change_ADC_channel(0x1);
    //transmit_USART("Indica \n");
    
    sei(); // Enables interruptions
    while (1)
    {
        
    }
    return 0;
}

// Beginning of ISRs declarations

ISR(TIMER2_COMPA_vect)
{
    /*
    Function specific to the ATMega328p, non portable.

    This interrupt service routine belongs to the vector "TIMER2_COMPA_vect" (vector 7, address 0x000E).
    
    This routine will :😎
    */
   

    //🤷‍♂️🤷‍♂️🤷‍♂️🤷‍♂️🤷‍♂️🤷‍♂️🤷‍♂️🤷‍♂️🤷‍♂️
    char adc_read[5]={'\0'};
    char message_z[11]={};
     char message_full[11]={};

    uint16_t my_data = 0;
           
    read_ADC_channel(0x0, &my_data);
    itoa(my_data, adc_read, 10);
    set_integer_string_fixed_length(adc_read, message_full, 4);
    replace_char_input_string(message_full, '\0', ';');

    read_ADC_channel(0x1, &my_data);
    itoa(my_data, adc_read, 10);
    set_integer_string_fixed_length(adc_read, message_z, 4);
    replace_char_input_string(message_z, '\0', '\n');
    strcat(message_full, message_z);
    transmit_USART(message_full);
            
    //🤷‍♂️🤷‍♂️🤷‍♂️🤷‍♂️🤷‍♂️🤷‍♂️🤷‍♂️🤷‍♂️🤷‍♂️

    interrupt_counter_compare_A++;
    
    if (status_bit(STATUS_SWITCH_A) == false)
    {
        if (interrupt_counter_compare_A == 25)
        {   
            //toggle_bit(LED_B); // Swap LED state

            interrupt_counter_compare_A = 0;
        } 
    } 

    if (increase_duty_cycle == 1)
    {
        if ((*OCR0B) < up_limit_duty_cycle_0)
        {
            (*OCR0B)++; // Called each 10ms
            return;
        }
        else
        {
            increase_duty_cycle = 0;
        }
    }
    if (increase_duty_cycle == 0)
    {
        if ((*OCR0B) > down_limit_duty_cycle_0)
        {
            (*OCR0B)--; // Called each 10ms
            return;
        }
        else
        {
            increase_duty_cycle = 1;
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

ISR(INT1_vect)
{
    /*
    Function specific to the ATMega328p, non portable.

    This interrupt service routine belongs to the vector "INT1_vect" (vector 3, address 0x0004).
    
    This routine will be called when BUTTON_B is pressed.
    */
   
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
            toggle_bit(LED_B);
            // This condition happens when the buffer is empty and hence, there's nothing to read
        }
    }
    
}


// To compile the code and obtain .hex file
/*
C:\Users\A0089761\scoop\apps\avr-gcc\14.1.0\bin\avr-gcc.exe -mmcu=atmega328p -Os main.c -o micro_demo.elf
C:\Users\A0089761\scoop\apps\avr-gcc\14.1.0\bin\avr-objcopy.exe -O ihex -R .eeprom micro_demo.elf micro_demo.hex
*/

// To flash the hex file into the microcontroller:
/*
C:\Users\A0089761\scoop\apps\avr-gcc\14.1.0\bin\avrdude.exe -c arduino -p m328p -U flash:w:micro_demo.hex -P COM6
*/