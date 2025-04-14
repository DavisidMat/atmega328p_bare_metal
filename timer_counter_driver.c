

#include "timer_counter_driver.h"

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
 
volatile uint32_t global_milliseconds_counter = 0; // Global counter, modified by ISR

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