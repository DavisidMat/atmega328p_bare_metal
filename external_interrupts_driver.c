#include "external_interrupts_driver.h"

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