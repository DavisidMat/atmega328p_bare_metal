#ifndef ATmega328p_registers
#define ATmega328p_registers
/*
Uppercase words are mainly reserved to registers name according to the datasheet of the ATmega328p
The names listed here correspond to the ones listed on the datasheet.
*/

#define SREG ((volatile uint8_t*) 0x5F) // AVR Status Register (SREG register on the datasheet)
    #define SREG_I ((int) 7) // AVR Status Register Bit 7 for enabling interrupts

// Defining Port B GPIO registers

#define PORTB ((volatile uint8_t*) 0x25) // Port register B (PORTB register on the datasheet)
    #define PORTB7 ((int) 7) // PORTB bit 7 (Manipulate bit 7 purpose according to read/write action)
    #define PORTB6 ((int) 6) // PORTB bit 6 (Manipulate bit 6 purpose according to read/write action)
    #define PORTB5 ((int) 5) // PORTB bit 5 (Manipulate bit 5 purpose according to read/write action)
    #define PORTB4 ((int) 4) // PORTB bit 4 (Manipulate bit 4 purpose according to read/write action)
    #define PORTB3 ((int) 3) // PORTB bit 3 (Manipulate bit 3 purpose according to read/write action)
    #define PORTB2 ((int) 2) // PORTB bit 2 (Manipulate bit 2 purpose according to read/write action)
    #define PORTB1 ((int) 1) // PORTB bit 1 (Manipulate bit 1 purpose according to read/write action)
    #define PORTB0 ((int) 0) // PORTB bit 0 (Manipulate bit 0 purpose according to read/write action)

#define DDRB ((volatile uint8_t*) 0x24) // Port B Data direction register (DDRB register on the datasheet)
    #define DDB7 ((int) 7) // DDRB bit 7 (Set bit 7 purpose: Input/Output)
    #define DDB6 ((int) 6) // DDRB bit 6 (Set bit 6 purpose: Input/Output)
    #define DDB5 ((int) 5) // DDRB bit 5 (Set bit 5 purpose: Input/Output)
    #define DDB4 ((int) 4) // DDRB bit 4 (Set bit 4 purpose: Input/Output)
    #define DDB3 ((int) 3) // DDRB bit 3 (Set bit 3 purpose: Input/Output)
    #define DDB2 ((int) 2) // DDRB bit 2 (Set bit 2 purpose: Input/Output)
    #define DDB1 ((int) 1) // DDRB bit 1 (Set bit 1 purpose: Input/Output)
    #define DDB0 ((int) 0) // DDRB bit 0 (Set bit 0 purpose: Input/Output)

#define PINB ((volatile uint8_t*) 0x23) // Port B Input Pins: Read register values (PINB register on the datasheet)
    #define PINB7 ((int) 7) // PINB bit 7 (Read bit 7 state)
    #define PINB6 ((int) 6) // PINB bit 6 (Read bit 6 state)
    #define PINB5 ((int) 5) // PINB bit 5 (Read bit 5 state)
    #define PINB4 ((int) 4) // PINB bit 4 (Read bit 4 state)
    #define PINB3 ((int) 3) // PINB bit 3 (Read bit 3 state)
    #define PINB2 ((int) 2) // PINB bit 2 (Read bit 2 state)
    #define PINB1 ((int) 1) // PINB bit 1 (Read bit 1 state)
    #define PINB0 ((int) 0) // PINB bit 0 (Read bit 0 state)

// Defining Port C GPIO registers

#define PORTC ((volatile uint8_t*) 0x28) // Port register C (PORTC register on the datasheet)
    #define PORTC6 ((int) 6) // PORTC bit 6 (Manipulate bit 6 purpose according to read/write action)
    #define PORTC5 ((int) 5) // PORTC bit 5 (Manipulate bit 5 purpose according to read/write action)
    #define PORTC4 ((int) 4) // PORTC bit 4 (Manipulate bit 4 purpose according to read/write action)
    #define PORTC3 ((int) 3) // PORTC bit 3 (Manipulate bit 3 purpose according to read/write action)
    #define PORTC2 ((int) 2) // PORTC bit 2 (Manipulate bit 2 purpose according to read/write action)
    #define PORTC1 ((int) 1) // PORTC bit 1 (Manipulate bit 1 purpose according to read/write action)
    #define PORTC0 ((int) 0) // PORTC bit 0 (Manipulate bit 0 purpose according to read/write action)

#define DDRC ((volatile uint8_t*) 0x27) // Port C Data direction register (DDRC register on the datasheet)
    #define DDC6 ((int) 6) // DDRC bit 6 (Set bit 6 purpose: Input/Output)
    #define DDC5 ((int) 5) // DDRC bit 5 (Set bit 5 purpose: Input/Output)
    #define DDC4 ((int) 4) // DDRC bit 4 (Set bit 4 purpose: Input/Output)
    #define DDC3 ((int) 3) // DDRC bit 3 (Set bit 3 purpose: Input/Output)
    #define DDC2 ((int) 2) // DDRC bit 2 (Set bit 2 purpose: Input/Output)
    #define DDC1 ((int) 1) // DDRC bit 1 (Set bit 1 purpose: Input/Output)
    #define DDC0 ((int) 0) // DDRC bit 0 (Set bit 0 purpose: Input/Output)

#define PINC ((volatile uint8_t*) 0x26) // Port C Input Pins: Read register values (PINC register on the datasheet)
    #define PINC6 ((int) 6) // PINC bit 6 (Read bit 6 state)
    #define PINC5 ((int) 5) // PINC bit 5 (Read bit 5 state)
    #define PINC4 ((int) 4) // PINC bit 4 (Read bit 4 state)
    #define PINC3 ((int) 3) // PINC bit 3 (Read bit 3 state)
    #define PINC2 ((int) 2) // PINC bit 2 (Read bit 2 state)
    #define PINC1 ((int) 1) // PINC bit 1 (Read bit 1 state)
    #define PINC0 ((int) 0) // PINC bit 0 (Read bit 0 state)

// Defining Port D GPIO registers

#define PORTD ((volatile uint8_t*) 0x2B) // Port register D (PORTD register on the datasheet)
    #define PORTD7 ((int) 7) // PORTD bit 7 (Manipulate bit 7 purpose according to read/write action)
    #define PORTD6 ((int) 6) // PORTD bit 6 (Manipulate bit 6 purpose according to read/write action)
    #define PORTD5 ((int) 5) // PORTD bit 5 (Manipulate bit 5 purpose according to read/write action)
    #define PORTD4 ((int) 4) // PORTD bit 4 (Manipulate bit 4 purpose according to read/write action)
    #define PORTD3 ((int) 3) // PORTD bit 3 (Manipulate bit 3 purpose according to read/write action)
    #define PORTD2 ((int) 2) // PORTD bit 2 (Manipulate bit 2 purpose according to read/write action)
    #define PORTD1 ((int) 1) // PORTD bit 1 (Manipulate bit 1 purpose according to read/write action)
    #define PORTD0 ((int) 0) // PORTD bit 0 (Manipulate bit 0 purpose according to read/write action)

#define DDRD ((volatile uint8_t*) 0x2A) // Port D Data direction register (DDRD register on the datasheet)
    #define DDD7 ((int) 7) // DDRD bit 7 (Set bit 7 purpose: Input/Output)
    #define DDD6 ((int) 6) // DDRD bit 6 (Set bit 6 purpose: Input/Output)
    #define DDD5 ((int) 5) // DDRD bit 5 (Set bit 5 purpose: Input/Output)
    #define DDD4 ((int) 4) // DDRD bit 4 (Set bit 4 purpose: Input/Output)
    #define DDD3 ((int) 3) // DDRD bit 3 (Set bit 3 purpose: Input/Output)
    #define DDD2 ((int) 2) // DDRD bit 2 (Set bit 2 purpose: Input/Output)
    #define DDD1 ((int) 1) // DDRD bit 1 (Set bit 1 purpose: Input/Output)
    #define DDD0 ((int) 0) // DDRD bit 0 (Set bit 0 purpose: Input/Output)

#define PIND ((volatile uint8_t*) 0x29) // Port D Input Pins: Read register values (PIND register on the datasheet)
    #define PIND7 ((int) 7) // PIND bit 7 (Read bit 7 state)
    #define PIND6 ((int) 6) // PIND bit 6 (Read bit 6 state)
    #define PIND5 ((int) 5) // PIND bit 5 (Read bit 5 state)
    #define PIND4 ((int) 4) // PIND bit 4 (Read bit 4 state)
    #define PIND3 ((int) 3) // PIND bit 3 (Read bit 3 state)
    #define PIND2 ((int) 2) // PIND bit 2 (Read bit 2 state)
    #define PIND1 ((int) 1) // PIND bit 1 (Read bit 1 state)
    #define PIND0 ((int) 0) // PIND bit 0 (Read bit 0 state)

#define sei() __asm__ __volatile__ ("sei" ::: "memory") /* Set Global Interrupt Flag
Equivalent to setting bit I, Global Interrupt Enable:
    set_bit(SREG, SREG_I); */

#define cli() __asm__ __volatile__ ("cli" ::: "memory") /* Clear Global Interrupt Flag
Equivalent to unsetting bit I, Global Interrupt Enable:
    unset_bit(SREG, SREG_I); */

/*
    8 bit Timer/Counter-0 registers
*/ 

// 8 bit Timer/Counter-0 vector registers
#define TIMER0_COMPA_vect (__vector_14) // Memory address 0x001C, to handle interuption: compare match A ("TIMER0 COMPA" register on the datasheet, vector 15)
#define TIMER0_COMPB_vect (__vector_15) // Memory address 0x001E, to handle interuption: compare match B ("TIMER0 COMPB" register on the datasheet, vector 16)
#define TIMER0_OVF_vect (__vector_16) // Memory address 0x0020, to handle interuption: overflow ("TIMER0 OVF" register on the datasheet, vector 17)

#define TCCR0A ((volatile uint8_t*) 0x44) // Timer/Counter-0 Control register A (TCCR0A register on the datasheet)
// The set up of the bits of this register depends on the application
    #define COM0A1 ((int) 7) // TCCR0A bit 7 (Compare Match Output A Mode)
    #define COM0A0 ((int) 6) // TCCR0A bit 6 (Compare Match Output A Mode)
    #define COM0B1 ((int) 5) // TCCR0A bit 5 (Compare Match Output B Mode)
    #define COM0B0 ((int) 4) // TCCR0A bit 4 (Compare Match Output B Mode)
    #define WGM01 ((int) 1) // TCCR0A bit 1 (Waveform Generation Mode)
    #define WGM00 ((int) 0) // TCCR0A bit 0 (Waveform Generation Mode)

#define TCCR0B ((volatile uint8_t*) 0x45) // Timer/Counter-0 Control register B (TCCR0B register on the datasheet)
// The set up of the bits of this register depends on the application
    #define FOC0A ((int) 7) // TCCR0B bit 7 (Force Output Compare A)
    #define FOC0B ((int) 6) // TCCR0B bit 6 (Force Output Compare B)
    #define WGM02 ((int) 3) // TCCR0B bit 3 (Waveform Generation Mode)
    #define CS02 ((int) 2) // TCCR0B bit 2 (Clock prescale select)
    #define CS01 ((int) 1) // TCCR0B bit 1 (Clock prescale select)
    #define CS00 ((int) 0) // TCCR0B bit 0 (Clock prescale select)
    
#define TCNT0 ((volatile uint8_t*) 0x46) // Timer/Counter-0 Register (TCNT0 register on the datasheet)

#define OCR0A ((volatile uint8_t*) 0x47) // Output Compare Register A Timer/Counter-0 (OCR0A register on the datasheet)

#define OCR0B ((volatile uint8_t*) 0x48) // Output Compare Register B Timer/Counter-0 (OCR0B register on the datasheet)

#define TIMSK0 ((volatile uint8_t*) 0x6E) // Timer/Counter interrupt mask register used to activate the interrupt mode (TIMSK0 register on the datasheet)
    #define OCIE0B ((int) 2) // TIMSK0 bit 2 (Timer/Counter-0 Output Compare Match B Interrupt Enable)
    #define OCIE0A ((int) 1) // TIMSK0 bit 1 (Timer/Counter-0 Output Compare Match A Interrupt Enable)
    #define TOIE0 ((int) 0) // TIMSK0 bit 0 (Timer/Counter-0 Overflow Interrupt Enable)

#define TIFR0 ((volatile uint8_t*) 0x35) // Timer/Counter-0 Interupt Flag Register (TIFR0 register on the datasheet)
    #define OCF0B ((int) 2) // TIFR0 bit 2 (Timer/Counter-0 Output Compare Match B Flag)
    #define OCF0A ((int) 1) // TIFR0 bit 1 (Timer/Counter-0 Output Compare Match A Flag)
    #define TOV0 ((int) 0) // TIFR0 bit 0 (Timer/Counter-0 Overflow Flag)

/*
    16 bit Timer/Counter-1 registers
*/

// 16 bit Timer/Counter-1 vector registers
#define TIMER1_CAPT_vect (__vector_10) // Memory address 0x0014, to handle interuption: capture event ("TIMER1 CAPT" register on the datasheet, vector 11)
#define TIMER1_COMPA_vect (__vector_11) // Memory address 0x0016, to handle interuption: compare match A ("TIMER1 COMPA" register on the datasheet, vector 12)
#define TIMER1_COMPB_vect (__vector_12) // Memory address 0x0018, to handle interuption: compare match B ("TIMER1 COMPB" register on the datasheet, vector 13)
#define TIMER1_OVF_vect (__vector_13) // Memory address 0x001A, to handle interuption: overflow ("TIMER1 OVF" register on the datasheet, vector 14)

#define TCCR1A ((volatile uint8_t*) 0x80) // Timer/Counter-1 Control register A (TCCR1A register on the datasheet)
// The set up of the bits of this register depends on the application
    #define COM1A1 ((int) 7) // TCCR1A bit 7 (Compare Output Mode for Channel A)
    #define COM1A0 ((int) 6) // TCCR1A bit 6 (Compare Output Mode for Channel A)
    #define COM1B1 ((int) 5) // TCCR1A bit 5 (Compare Output Mode for Channel B)
    #define COM1B0 ((int) 4) // TCCR1A bit 4 (Compare Output Mode for Channel B)
    #define WGM11 ((int) 1) // TCCR1A bit 1 (Waveform Generation Mode)
    #define WGM10 ((int) 0) // TCCR1A bit 0 (Waveform Generation Mode)

#define TCCR1B ((volatile uint8_t*) 0x81) // Timer/Counter-1 Control register B (TCCR1B register on the datasheet)
// The set up of the bits of this register depends on the application
    #define ICNC1 ((int) 7) // TCCR1B bit 7 (Input Capture Noise Canceler)
    #define ICES1 ((int) 6) // TCCR1B bit 6 (Input Capture Edge Select)
    #define WGM13 ((int) 4) // TCCR1B bit 4 (Waveform Generation Mode)
    #define WGM12 ((int) 3) // TCCR1B bit 3 (Waveform Generation Mode)
    #define CS12 ((int) 2) // TCCR1B bit 2 (Clock Select)
    #define CS11 ((int) 1) // TCCR1B bit 1 (Clock Select)
    #define CS10 ((int) 0) // TCCR1B bit 0 (Clock Select)

#define TCCR1C ((volatile uint8_t*) 0x82) // Timer/Counter-1 Control register C (TCCR1C register on the datasheet)
// The set up of the bits of this register depends on the application
    #define FOC1A ((int) 7) // TCCR1C bit 7 (Force Output Compare for Channel A)
    #define FOC1B ((int) 6) // TCCR1C bit 6 (Force Output Compare for Channel B)

#define TCNT1H ((volatile uint8_t*) 0x85) // Timer/Counter-1 High Register (TCNT1H register on the datasheet)
#define TCNT1L ((volatile uint8_t*) 0x84) // Timer/Counter-1 Low Register (TCNT1L register on the datasheet)

#define OCR1AH ((volatile uint8_t*) 0x89) // Output Compare High Register A Timer/Counter-1 (OCR1AH register on the datasheet)
#define OCR1AL ((volatile uint8_t*) 0x88) // Output Compare Low Register A Timer/Counter-1 (OCR1AL register on the datasheet)

#define OCR1BH ((volatile uint8_t*) 0x8B) // Output Compare High Register B Timer/Counter-1 (OCR1BH register on the datasheet)
#define OCR1BL ((volatile uint8_t*) 0x8A) // Output Compare Low Register B Timer/Counter-1 (OCR1BL register on the datasheet)

#define ICR1H ((volatile uint8_t*) 0x87) // Input Capture High Register 1 (ICR1H register on the datasheet)
#define ICR1L ((volatile uint8_t*) 0x86) // Input Capture Low Register 1 Timer/Counter-1 (ICR1L register on the datasheet)

#define TIMSK1 ((volatile uint8_t*) 0x6F) // Timer/Counter interrupt mask register used to activate the interrupt mode (TIMSK1 register on the datasheet)
    #define ICIE1 ((int) 5) // TIMSK1 bit 5 (Timer/Counter-1 Input Capture Interrupt Enable)
    #define OCIE1B ((int) 2) // TIMSK1 bit 2 (Timer/Counter-1 Output Compare Match B Interrupt Enable)
    #define OCIE1A ((int) 1) // TIMSK1 bit 1 (Timer/Counter-1 Output Compare Match A Interrupt Enable)
    #define TOIE1 ((int) 0) // TIMSK1 bit 0 (Timer/Counter-1 Overflow Interrupt Enable)

#define TIFR1 ((volatile uint8_t*) 0x36) // Timer/Counter-1 Interupt Flag Register (TIFR1 register on the datasheet)
    #define ICF1 ((int) 5) // TIFR1 bit 5 (Timer/Counter-1 Input Capture Flag)
    #define OCF1B ((int) 2) // TIFR1 bit 2 (Timer/Counter-1 Output Compare Match B Flag)
    #define OCF1A ((int) 1) // TIFR1 bit 1 (Timer/Counter-1 Output Compare Match A Flag)
    #define TOV1 ((int) 0) // TIFR1 bit 0 (Timer/Counter-1 Overflow Flag)

/*
    8 bit Timer/Counter-2 registers
*/ 

// 8 bit Timer/Counter-2 vector registers
#define TIMER2_COMPA_vect (__vector_7) // Memory address 0x000E, to handle interuption: compare match A ("TIMER2 COMPA" register on the datasheet, vector 8)
#define TIMER2_COMPB_vect (__vector_8) // Memory address 0x0010, to handle interuption: compare match B ("TIMER2 COMPB" register on the datasheet, vector 9)
#define TIMER2_OVF_vect (__vector_9) // Memory address 0x0012, to handle interuption: overflow ("TIMER2 OVF" register on the datasheet, vector 10)

#define TCCR2A ((volatile uint8_t*) 0xB0) // Timer/Counter-2 Control register A (TCCR2A register on the datasheet)
// The set up of the bits of this register depends on the application
    #define COM2A1 ((int) 7) // TCCR2A bit 7 (Compare Match Output A Mode)
    #define COM2A0 ((int) 6) // TCCR2A bit 6 (Compare Match Output A Mode)
    #define COM2B1 ((int) 5) // TCCR2A bit 5 (Compare Match Output B Mode)
    #define COM2B0 ((int) 4) // TCCR2A bit 4 (Compare Match Output B Mode)
    #define WGM21 ((int) 1) // TCCR2A bit 1 (Waveform Generation Mode)
    #define WGM20 ((int) 0) // TCCR2A bit 0 (Waveform Generation Mode)

#define TCCR2B ((volatile uint8_t*) 0xB1) // Timer/Counter-2 Control register B (TCCR2B register on the datasheet)
// The set up of the bits of this register depends on the application
    #define FOC2A ((int) 7) // TCCR2B bit 7 (Force Output Compare A)
    #define FOC2B ((int) 6) // TCCR2B bit 6 (Force Output Compare B)
    #define WGM22 ((int) 3) // TCCR2B bit 3 (Waveform Generation Mode)
    #define CS22 ((int) 2) // TCCR2B bit 2 (Clock prescale select)
    #define CS21 ((int) 1) // TCCR2B bit 1 (Clock prescale select)
    #define CS20 ((int) 0) // TCCR2B bit 0 (Clock prescale select)
    
#define TCNT2 ((volatile uint8_t*) 0xB2) // Timer/Counter-2 Register (TCNT2 register on the datasheet)

#define OCR2A ((volatile uint8_t*) 0xB3) // Output Compare Register A Timer/Counter-2 (OCR2A register on the datasheet)

#define OCR2B ((volatile uint8_t*) 0xB4) // Output Compare Register B Timer/Counter-2 (OCR2B register on the datasheet)

#define TIMSK2 ((volatile uint8_t*) 0x70) // Timer/Counter interrupt mask register used to activate the interrupt mode (TIMSK2 register on the datasheet)
    #define OCIE2B ((int) 2) // TIMSK2 bit 2 (Timer/Counter-2 Output Compare Match B Interrupt Enable)
    #define OCIE2A ((int) 1) // TIMSK2 bit 1 (Timer/Counter-2 Output Compare Match A Interrupt Enable)
    #define TOIE2 ((int) 0) // TIMSK2 bit 0 (Timer/Counter-2 Overflow Interrupt Enable)

#define TIFR2 ((volatile uint8_t*) 0x37) // Timer/Counter-2 Interupt Flag Register (TIFR2 register on the datasheet)
    #define OCF2B ((int) 2) // TIFR2 bit 2 (Timer/Counter-2 Output Compare Match B Flag)
    #define OCF2A ((int) 1) // TIFR2 bit 1 (Timer/Counter-2 Output Compare Match A Flag)
    #define TOV2 ((int) 0) // TIFR2 bit 0 (Timer/Counter-2 Overflow Flag)

#define ASSR ((volatile uint8_t*) 0xB6) // Asynchronous Status Register (ASSR register on the datasheet)
    #define EXCLK ((int) 6) // ASSR bit 6 (Enable External Clock Input)
    #define AS2 ((int) 5) // ASSR bit 5 (Asynchronous Timer/Counter-2)
    #define TCN2UB ((int) 4) // ASSR bit 4 (Timer/Counter-2 Update Busy)
    #define OCR2AUB ((int) 3) // ASSR bit 3 (Output Compare Register-2 Update Busy)
    #define OCR2BUB ((int) 2) // ASSR bit 2 (Output Compare Register-2 Update Busy)
    #define TCR2AUB ((int) 1) // ASSR bit 1 (Timer/Counter-2 Control Register Update Busy)
    #define TCR2BUB ((int) 0) // ASSR bit 0 (Timer/Counter-2 Control Register Update Busy)

#define GTCCR ((volatile uint8_t*) 0x43) // Asynchronous Status Register - General Timer/Counter Control Register (GTCCR register on the datasheet)
    #define TSM ((int) 7) // GTCCR bit 7 (Timer/Counter Synchronization Mode)
    #define PSRASY ((int) 1) // GTCCR bit 1 (Prescaler Reset Timer/Counter-2)
    #define PSRSYNC ((int) 0) // GTCCR bit 0 (Prescaler Reset)

/*
    External Interrupt Registers
*/

// External Interrupt vectors
#define INT0_vect (__vector_1) // Memory address 0x0002, to handle interuption: External interrupt request 0 ("INT0" register on the datasheet, vector 2)
#define INT1_vect (__vector_2) // Memory address 0x0004, to handle interuption: External interrupt request 1 ("INT1" register on the datasheet, vector 3)

#define EICRA ((volatile uint8_t*) 0x69) // External Interrupt Control Register A (EICRA register on the datasheet)
    #define ISC11 ((int) 3) // EICRA bit 3 (Interrupt Sense Control 1 Bit 1)
    #define ISC10 ((int) 2) // EICRA bit 2 (Interrupt Sense Control 1 Bit 0)
    #define ISC01 ((int) 1) // EICRA bit 1 (Interrupt Sense Control 0 Bit 1)
    #define ISC00 ((int) 0) // EICRA bit 0 (Interrupt Sense Control 0 Bit 0)

#define EIMSK ((volatile uint8_t*) 0x3D) // External Interrupt Mask Register (EIMSK register on the datasheet)
    #define INT1 ((int) 1) // EIMSK bit 1 (External Interrupt Request 1 Enable)
    #define INT0 ((int) 0) // EIMSK bit 0 (External Interrupt Request  Enable)

#define EIFR ((volatile uint8_t*) 0x3C) // External Interrupt Flag Register (EIFR register on the datasheet)
    #define INTF1 ((int) 1) // EIFR bit 1 (External Interrupt Flag 1)
    #define INTF0 ((int) 0) // EIFR bit 0 (External Interrupt Flag 0)

/*
    Pin Change Interrupt Registers
*/

// Pin Change Interrupt vectors
#define PCINT0_vect (__vector_3) // Memory address 0x0006, to handle interuption: Pin change interrupt request 0 ("PCINT0" register on the datasheet, vector 4)
#define PCINT1_vect (__vector_4) // Memory address 0x0008, to handle interuption: Pin change interrupt request 1 ("PCINT1" register on the datasheet, vector 5)
#define PCINT2_vect (__vector_5) // Memory address 0x000A, to handle interuption: Pin change interrupt request 2 ("PCINT2" register on the datasheet, vector 6)

#define PCICR ((volatile uint8_t*) 0x68) // Pin Change Interrupt Control Register (PCICR register on the datasheet)
    #define PCIE2 ((int) 2) // PCICR bit 2 (Pin Change Interrupt Enable 2)
    #define PCIE1 ((int) 1) // PCICR bit 1 (Pin Change Interrupt Enable 1)
    #define PCIE0 ((int) 0) // PCICR bit 0 (Pin Change Interrupt Enable 0)
    
#define PCIFR ((volatile uint8_t*) 0x3B) // Pin Change Interrupt Flag Register (PCIFR register on the datasheet)
    #define PCIF2 ((int) 2) // PCIFR bit 2 (Pin Change Interrupt Flag 2)
    #define PCIF1 ((int) 1) // PCIFR bit 1 (Pin Change Interrupt Flag 1)
    #define PCIF0 ((int) 0) // PCIFR bit 0 (Pin Change Interrupt Flag 0)

#define PCMSK2 ((volatile uint8_t*) 0x6D) // Pin Change Mask Register 2 (PCMSK2 register on the datasheet)
    #define PCINT23 ((int) 7) // PCMSK2 bit 7 (Pin Change Enable Mask Pin PCINT23)
    #define PCINT22 ((int) 6) // PCMSK2 bit 6 (Pin Change Enable Mask Pin PCINT22)
    #define PCINT21 ((int) 5) // PCMSK2 bit 5 (Pin Change Enable Mask Pin PCINT21)
    #define PCINT20 ((int) 4) // PCMSK2 bit 4 (Pin Change Enable Mask Pin PCINT20)
    #define PCINT19 ((int) 3) // PCMSK2 bit 3 (Pin Change Enable Mask Pin PCINT19)
    #define PCINT18 ((int) 2) // PCMSK2 bit 2 (Pin Change Enable Mask Pin PCINT18)
    #define PCINT17 ((int) 1) // PCMSK2 bit 1 (Pin Change Enable Mask Pin PCINT17)
    #define PCINT16 ((int) 0) // PCMSK2 bit 0 (Pin Change Enable Mask Pin PCINT16)

#define PCMSK1 ((volatile uint8_t*) 0x6C) // Pin Change Mask Register 1 (PCMSK1 register on the datasheet)
    #define PCINT14 ((int) 6) // PCMSK1 bit 6 (Pin Change Enable Mask Pin PCINT14)
    #define PCINT13 ((int) 5) // PCMSK1 bit 5 (Pin Change Enable Mask Pin PCINT13)
    #define PCINT12 ((int) 4) // PCMSK1 bit 4 (Pin Change Enable Mask Pin PCINT12)
    #define PCINT11 ((int) 3) // PCMSK1 bit 3 (Pin Change Enable Mask Pin PCINT11)
    #define PCINT10 ((int) 2) // PCMSK1 bit 2 (Pin Change Enable Mask Pin PCINT10)
    #define PCINT9 ((int) 1) // PCMSK1 bit 1 (Pin Change Enable Mask Pin PCINT9)
    #define PCINT8 ((int) 0) // PCMSK1 bit 0 (Pin Change Enable Mask Pin PCINT8)

#define PCMSK0 ((volatile uint8_t*) 0x6B) // Pin Change Mask Register 0 (PCMSK0 register on the datasheet)
    #define PCINT7 ((int) 7) // PCMSK0 bit 7 (Pin Change Enable Mask Pin PCINT7)
    #define PCINT6 ((int) 6) // PCMSK0 bit 6 (Pin Change Enable Mask Pin PCINT6)
    #define PCINT5 ((int) 5) // PCMSK0 bit 5 (Pin Change Enable Mask Pin PCINT5)
    #define PCINT4 ((int) 4) // PCMSK0 bit 4 (Pin Change Enable Mask Pin PCINT4)
    #define PCINT3 ((int) 3) // PCMSK0 bit 3 (Pin Change Enable Mask Pin PCINT3)
    #define PCINT2 ((int) 2) // PCMSK0 bit 2 (Pin Change Enable Mask Pin PCINT2)
    #define PCINT1 ((int) 1) // PCMSK0 bit 1 (Pin Change Enable Mask Pin PCINT1)
    #define PCINT0 ((int) 0) // PCMSK0 bit 0 (Pin Change Enable Mask Pin PCINT0)

/*
    USART Protocol Registers
*/

// USART Interrupt vectors
#define USART_RX_vect (__vector_18) // Memory address 0x0024, to handle interuption: Data reception ("USART, RX" register on the datasheet, vector 19)
#define USART_UDRE_vect (__vector_19) // Memory address 0x0026, to handle interuption: Data transmission complete - Data register empty  ("USART, UDRE" register on the datasheet, vector 20)
#define USART_TX_vect (__vector_20) // Memory address 0x0028, to handle interuption: UART buffer ready ("USART, TX" register on the datasheet, vector 21)

#define UDR0 ((volatile uint8_t*) 0xC6) // USART I/O Data Register 0/n (UDR0 / UDRn register on the datasheet)
    // This register contains transmit (TXB) or receive (RXB) data.

#define  UCSR0A ((volatile uint8_t*) 0xC0) // USART Control and Status Register 0/n A (UCSR0A / UCSRnA register on the datasheet)
    #define RXC0 ((int) 7) // UCSR0A bit 7 (RXCn: USART Receive Complete)
    #define TXC0 ((int) 6) // UCSR0A bit 6 (TXCn: USART Transmit Complete)
    #define UDRE0 ((int) 5) // UCSR0A bit 5 (UDREn: USART Data Register Empty)
    #define FE0 ((int) 4) // UCSR0A bit 4 (FEn: Frame Error)
    #define DOR0 ((int) 3) // UCSR0A bit 3 (DORn: Data OverRun)
    #define UPE0 ((int) 2) // UCSR0A bit 2 (UPEn: USART Parity Error)
    #define U2X0 ((int) 1) // UCSR0A bit 1 (U2Xn: Double the USART Transmission Speed)
    #define MPCM0 ((int) 0) // UCSR0A bit 0 (MPCMn: Multi-processor Communication Mode)

#define  UCSR0B ((volatile uint8_t*) 0xC1) // USART Control and Status Register 0/n B (UCSR0B / UCSRnB register on the datasheet)
    #define RXCIE0 ((int) 7) // UCSR0B bit 7 (RXCIEn: RX Complete Interrupt Enable n)
    #define TXCIE0 ((int) 6) // UCSR0B bit 6 (TXCIEn: TX Complete Interrupt Enable n)
    #define UDRIE0 ((int) 5) // UCSR0B bit 5 (UDRIEn: USART Data Register Empty Interrupt Enable n)
    #define RXEN0 ((int) 4) // UCSR0B bit 4 (RXENn: Receiver Enable n)
    #define TXEN0 ((int) 3) // UCSR0B bit 3 (TXENn: Transmitter Enable n)
    #define UCSZ02 ((int) 2) // UCSR0B bit 2 (UCSZn2: Character Size n)
    #define RXB80 ((int) 1) // UCSR0B bit 1 (RXB8n: Receive Data Bit 8 n)
    #define TXB80 ((int) 0) // UCSR0B bit 0 (TXB8n: Transmit Data Bit 8 n)

#define  UCSR0C ((volatile uint8_t*) 0xC2) // USART Control and Status Register 0/n C (UCSR0C / UCSRnC register on the datasheet)
    #define UMSEL01 ((int) 7) // UCSR0C bit 7 (UMSELn1 USART Mode Select)
    #define UMSEL00 ((int) 6) // UCSR0C bit 6 (UMSELn0 USART Mode Select)
    #define UPM01 ((int) 5) // UCSR0C bit 5 (UPMn1: Parity Mode)
    #define UPM00 ((int) 4) // UCSR0C bit 4 (UPMn0: Parity Mode)
    #define USBS0 ((int) 3) // UCSR0C bit 3 (USBSn: Stop Bit Select)
    #define UCSZ01 ((int) 2) // UCSR0C bit 2 (UCSZn1: Character Size)
        #define UDORD0 ((int) 2) // UCSR0C bit 2 (UDORDn: Data Order) (Exclusive for USART SPI)
    #define UCSZ00 ((int) 1) // UCSR0C bit 1 (UCSZn0: Character Size)
        #define UCPHA0 ((int) 1) // UCSR0C bit 1 (UCPHAn: Clock Phase) (Exclusive for USART SPI)
    #define UCPOL0 ((int) 0) // UCSR0C bit 0 (UCPOLn: Clock Polarity)

// USART Baud Rate Registers (12 bit register)
#define  UBRR0H ((volatile uint8_t*) 0xC5) // USART High Baud Rate Register (UBRR0H / UBRRnH register on the datasheet)
#define  UBRR0L ((volatile uint8_t*) 0xC4) // USART Low Baud Rate Register (UBRR0L / UBRRnL register on the datasheet)

/*
    Analog Comparator Registers
*/

// Analog Comparator Interrupt vector
#define ANALOG_COMP_vect (__vector_23) // Memory address 0x002E, to handle interuption: Analog Comparator ("Analog Comparator" register on the datasheet, vector 24)

#define ACSR ((volatile uint8_t*) 0x50) // Analog Comparator Control and Status Register (ACSR register on the datasheet)
    #define ACD ((int) 7) // ACSR bit 7 (Analog Comparator Disable)
    #define ACBG ((int) 6) // ACSR bit 6 (Analog Comparator Bandgap Select)
    #define ACO ((int) 5) // ACSR bit 5 (Analog Comparator Output)
    #define ACI ((int) 4) // ACSR bit 4 (Analog Comparator Interrupt Flag)
    #define ACIE ((int) 3) // ACSR bit 3 (Analog Comparator Interrupt Enable)
    #define ACIC ((int) 2) // ACSR bit 2 (Analog Comparator Input Capture Enable)
    #define ACIS1 ((int) 1) // ACSR bit 1 (Analog Comparator Interrupt Mode Select)
    #define ACIS0 ((int) 0) // ACSR bit 0 (Analog Comparator Interrupt Mode Select)
    
#define DIDR1 ((volatile uint8_t*) 0x7F) // Digital Input Disable Register 1 (DIDR1 register on the datasheet)
    #define AIN1D ((int) 1) // DIDR1 bit 1 (Digital Input Disable)
    #define AIN0D ((int) 0) // DIDR1 bit 0 (Digital Input Disable)

/*
    Analog to Digital Converter (ADC) Registers
*/

// ADC Interrupt vectors
#define ADC_vect (__vector_21) // Memory address 0x002A, to handle interuption: ADC conversion complete ("ADC conversion complete" register on the datasheet, vector 22)

#define ADMUX ((volatile uint8_t*) 0x7C) // ADC Multiplexer Selection Register (ADMUX register on the datasheet)
    #define REFS1 ((int) 7) // ADMUX bit 7 (Voltage Reference Selection Bit 1)
    #define REFS0 ((int) 6) // ADMUX bit 6 (Voltage Reference Selection Bit 0)
    #define ADLAR ((int) 5) // ADMUX bit 5 (ADC Left Adjust Result)
    #define MUX3 ((int) 3) // ADMUX bit 3 (Analog Channel Selection Bit 3)
    #define MUX2 ((int) 2) // ADMUX bit 2 (Analog Channel Selection Bit 2)
    #define MUX1 ((int) 1) // ADMUX bit 1 (Analog Channel Selection Bit 1)
    #define MUX0 ((int) 0) // ADMUX bit 0 (Analog Channel Selection Bit 0)

#define ADCSRA ((volatile uint8_t*) 0x7A) // ADC Control and Status Register A (ADCSRA register on the datasheet)
    #define ADEN ((int) 7) // ADCSRA bit 7 (ADC Enable)
    #define ADSC ((int) 6) // ADCSRA bit 6 (ADC Start Conversion)
    #define ADATE ((int) 5) // ADCSRA bit 5 (ADC Auto Trigger Enable)
    #define ADIF ((int) 4) // ADCSRA bit 4 (ADC Interrupt Flag)
    #define ADIE ((int) 3) // ADCSRA bit 3 (ADC Interrupt Enable)
    #define ADPS2 ((int) 2) // ADCSRA bit 2 (ADC Prescaler Select Bit 2)
    #define ADPS1 ((int) 1) // ADCSRA bit 1 (ADC Prescaler Select Bit 1)
    #define ADPS0 ((int) 0) // ADCSRA bit 0 (ADC Prescaler Select Bit 0)

#define ADCSRB ((volatile uint8_t*) 0x7B) // ADC Control and Status Register B (ADCSRB register on the datasheet)
    #define ACME ((int) 6) // ADCSRB bit 6 (Analog Comparator Multiplexer Enable)
    #define ADTS2 ((int) 2) // ADCSRB bit 2 (ADC Auto Trigger Source Bit 2)
    #define ADTS1 ((int) 1) // ADCSRB bit 1 (ADC Auto Trigger Source Bit 1)
    #define ADTS0 ((int) 0) // ADCSRB bit 0 (ADC Auto Trigger Source Bit 0)

#define ADCH ((volatile uint8_t*) 0x79) // ADC Data Register High (ADCH register on the datasheet)
#define ADCL ((volatile uint8_t*) 0x78) // ADC Data Register Low (ADCL register on the datasheet)

#define DIDR0 ((volatile uint8_t*) 0x7E) // Digital Input Disable Register 0 (DIDR0 register on the datasheet)
    #define ADC5D ((int) 5) // DIDR0 bit 5 (Digital Input Disable Bit 5)
    #define ADC4D ((int) 4) // DIDR0 bit 4 (Digital Input Disable Bit 4)
    #define ADC3D ((int) 3) // DIDR0 bit 3 (Digital Input Disable Bit 3)
    #define ADC2D ((int) 2) // DIDR0 bit 2 (Digital Input Disable Bit 2)
    #define ADC1D ((int) 1) // DIDR0 bit 1 (Digital Input Disable Bit 1)
    #define ADC0D ((int) 0) // DIDR0 bit 0 (Digital Input Disable Bit 0)

#endif