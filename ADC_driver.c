#include "UART_driver.h"

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