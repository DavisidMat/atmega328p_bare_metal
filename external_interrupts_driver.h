#ifndef external_interrupts_driver
#define external_interrupts_driver

#include <stdio.h>
#include <stdbool.h>

#include "ATmega328p_registers.h"
#include "register_manipulation.h"

bool setup_external_interrupt(bool choose_external_int, uint8_t trigger_mode);
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

void activate_external_interrupts(bool enable_external_interrupt_0, bool enable_external_interrupt_1);
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

void desactivate_external_interrupts(bool disable_external_interrupt_0, bool disable_external_interrupt_1);
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

bool activate_pin_change_interrupt(uint8_t activate_chosen_interrupt_pin);
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

bool desactivate_pin_change_interrupt(uint8_t desactivate_chosen_interrupt_pin);
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
    
#endif