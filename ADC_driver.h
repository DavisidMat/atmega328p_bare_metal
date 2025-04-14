#ifndef ADC_driver
#define ADC_driver

#include <stdio.h>
#include <stdbool.h>

#include "ATmega328p_registers.h"
#include "register_manipulation.h"

bool setup_ADC(bool external_ref_voltage, bool set_auto_trigger, uint8_t clock_prescaler_mode, uint8_t trigger_source);
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

bool change_ADC_channel(uint8_t ADC_channel_num);
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

void read_channel(uint16_t *byte_ADC);
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

bool read_ADC_channel(uint8_t ADC_channel_num, uint16_t *byte_ADC);
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

#endif