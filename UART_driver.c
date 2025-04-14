

#include "UART_driver.h"

static ring_buffer_t ring_buffer = {0};
static uint8_t data_buffer[BUFFER_SIZE] = {'\0'};

static uint8_t dummy_byte = '\0';


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