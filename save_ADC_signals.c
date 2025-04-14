//#include <avr/io.h>

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
//#include <math.h>

#include "ATmega328p_registers.h"
#include "register_manipulation.h"
#include "ring_buffer_data_structure.h"
#include "external_interrupts_driver.h"
#include "timer_counter_driver.h"
#include "UART_driver.h"
#include "ADC_driver.h"
#include "generic_functions.h"




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


#define ISR(interrupt_vector) \
    void interrupt_vector(void)  __attribute__( (signal) );\
    void interrupt_vector(void)
    // Start ISR definition

#define CLOCK_FREQUENCY 16000000
#define PRESCALE_VALUE 10 // Equivalent to dividing by 1024
#define INTERRUPTION_TIME ((CLOCK_FREQUENCY>>PRESCALE_VALUE))



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


static uint8_t input_string[12] = {'\0'};

static uint16_t instantaneous_input_capture_count = 0;

//volatile static uint32_t global_milliseconds_counter = 0; //🤔🤔🤔🤔
extern volatile uint32_t global_milliseconds_counter;

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


    setup_ADC(false, true, 0x4, 0x0); // Using internal voltage reference (VCC = 5v)
    change_ADC_channel(0x1);
    
    sei(); // Enables interruptions

    uint32_t current_time = global_milliseconds_counter;
    uint32_t previous_time = current_time;


    int32_t signal_period_raw[2] = {0};
    int32_t signal_duty_cycle_raw[2] = {0};
    uint8_t system_clock_prescaler = 8;
    uint8_t chosen_signal_channel = 0;

    transmit_USART("Starting\n");
    delay_milliseconds(3000);

    transmit_USART("uwu omeee\n");
    while (1)
    {
        
        current_time = global_milliseconds_counter;

        
        if ((current_time - previous_time) >= 500)
        {         
            previous_time = current_time;

            char adc_read[5]={'\0'};
            char message_z[6]={'\0'};
            char message_full[21]={'\0'};

            uint16_t my_data = 0;
            
            read_ADC_channel(0x4, &my_data);
            utoa(my_data, adc_read, 10);
            set_integer_string_fixed_length(adc_read, message_full, 4);
            replace_char_input_string(message_full, '\0', ';', size(message_full));
            
            read_ADC_channel(0x3, &my_data);
            utoa(my_data, adc_read, 10);
            set_integer_string_fixed_length(adc_read, message_z, 4);
            replace_char_input_string(message_z, '\0', ';', size(message_z));
            strcat(message_full, message_z);

            read_ADC_channel(0x4, &my_data);
            utoa(my_data, adc_read, 10);
            set_integer_string_fixed_length(adc_read, message_z, 4);
            replace_char_input_string(message_z, '\0', ';', size(message_z));
            strcat(message_full, message_z);
            
            read_ADC_channel(0x5, &my_data);
            utoa(my_data, adc_read, 10);
            set_integer_string_fixed_length(adc_read, message_z, 4);
            replace_char_input_string(message_z, '\0', '\n', size(message_z));
            strcat(message_full, message_z);
            transmit_USART(message_full);
            
        }
            
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




// To compile the code and obtain .hex file
/*
C:\Users\A0089761\scoop\apps\avr-gcc\14.1.0\bin\avr-gcc.exe -mmcu=atmega328p -Os save_ADC_signals.c register_manipulation.c ring_buffer_data_structure.c external_interrupts_driver.c timer_counter_driver.c UART_driver.c ADC_driver.c generic_functions.c -o micro_demo.elf
C:\Users\A0089761\scoop\apps\avr-gcc\14.1.0\bin\avr-objcopy.exe -O ihex -R .eeprom micro_demo.elf micro_demo.hex
*/

// To flash the hex file into the microcontroller:
/*
C:\Users\A0089761\scoop\apps\avr-gcc\14.1.0\bin\avrdude.exe -c arduino -p m328p -U flash:w:micro_demo.hex -P COM6
*/