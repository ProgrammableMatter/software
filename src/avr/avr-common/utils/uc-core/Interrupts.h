/**
 * @author Raoul Rubien
 * 23.11.15.
 */

#ifndef PARTICLE_INTERRUPTS_H
#define PARTICLE_INTERRUPTS_H

#include <avr/interrupt.h>
#include <inttypes.h>
#include <common/PortInteraction.h>
#include "ParticleParameters.h"

/**
 * Interrupt vectors renamed for convenience.
 */
#ifdef __AVR_ATtiny1634__
#  define NORTH_PIN_CHANGE_INTERRUPT_VECT PCINT1_vect
#  define EAST_PIN_CHANGE_INTERRUPT_VECT PCINT0_vect
#  define SOUTH_PIN_CHANGE_INTERRUPT_VECT PCINT2_vect
#  define TX_RX_TIMER_EIGHTH_INTERRUPT_VECT TIM0_COMPA_vect
#else
#  if defined(__AVR_ATmega16__)
#    define NORTH_PIN_CHANGE_INTERRUPT_VECT INT2_vect
#    define EAST_PIN_CHANGE_INTERRUPT_VECT INT1_vect
#    define SOUTH_PIN_CHANGE_INTERRUPT_VECT INT0_vect
#    define TX_RX_TIMER_EIGHTH_INTERRUPT_VECT TIMER0_COMP_vect
#  else
#    error
#  endif
#endif

#if defined(__AVR_ATtiny1634__) || defined(__AVR_ATmega16__)
#  define TX_RX_TIMER_TOP_INTERRUPT_VECT TIMER1_COMPA_vect
#  define TX_RX_TIMER_CENTER_INTERRUPT_VECT TIMER1_COMPB_vect
#endif

/**
 * define reception pin interrupt macros
 */
// for production MCU
#ifdef __AVR_ATtiny1634__
#  define RX_INTERRUPTS_CLEAR_PENDING GIFR = bit(PCIF0) | bit(PCIF1) | bit(PCIF2) | bit(INTF0)
#  define RX_INT_ENABLE GIMSK setBit (bit(PCIE0) | bit(PCIE1) | bit(PCIE2))

#  define RX_NORTH_INTERRUPT_DISABLE PCMSK2 unsetBit bit(PCINT17)
#  define RX_NORTH_INTERRUPT_ENABLE PCMSK2 setBit bit(PCINT17)
#  define RX_SOUTH_INTERRUPT_DISABLE PCMSK0 setBit bit(PCINT4)
#  define RX_SOUTH_INTERRUPT_ENABLE PCMSK0 setBit bit(PCINT4)
#  define RX_EAST_INTERRUPT_DISABLE PCMSK1 unsetBit bit(PCINT8)
#  define RX_EAST_INTERRUPT_ENABLE PCMSK1 setBit bit(PCINT8)

#  define RX_INTERRUPTS_ENABLE RX_NORTH_INTERRUPT_ENABLE; RX_SOUTH_INTERRUPT_ENABLE; RX_EAST_INTERRUPT_ENABLE
#  define RX_INTERRUPTS_DISABLE RX_NORTH_INTERRUPT_DISABLE; RX_SOUTH_INTERRUPT_DISABLE; RX_EAST_INTERRUPT_DISABLE

#  define RX_INTERRUPTS_SETUP RX_INTERRUPTS_CLEAR_PENDING; RX_INT_ENABLE

#else
// for simulator MCU
#  if defined(__AVR_ATmega16__)
#    define RX_INTERRUPTS_CLEAR_PENDING GIFR = bit(INTF0) | bit(INTF1) | bit(INTF2)

//ISC2 - 0: on falling edge, 1: on rising edge
//(ISCx1, ISCx0) - (0,1): on logic change
#    define __RX_INTERRUPTS_SENSE_CONTROL_SETUP MCUCR  setBit (0 << ISC11) | (1 << ISC10) | (0 << ISC01) | (1 << ISC00); \
MCUCSR setBit (1 << ISC2)

#    define RX_NORTH_INTERRUPT_DISABLE GICR unsetBit bit(INT0);
#    define RX_NORTH_INTERRUPT_ENABLE GICR setBit bit(INT0);
#    define RX_SOUTH_INTERRUPT_DISABLE GICR setBit bit(INT1);
#    define RX_SOUTH_INTERRUPT_ENABLE GICR unsetBit bit(INT1);
#    define RX_EAST_INTERRUPT_DISABLE GICR setBit bit(INT2);
#    define RX_EAST_INTERRUPT_ENABLE GICR unsetBit bit(INT2);

#    define RX_INTERRUPTS_ENABLE GICR setBit ((1 << INT0) | (1 << INT1) | (1 << INT2))
#    define RX_INTERRUPTS_DISABLE GICR unsetBit ((1 << INT0) | (1 << INT1) | (1 << INT2))

#    define RX_INTERRUPTS_SETUP RX_INTERRUPTS_CLEAR_PENDING; __RX_INTERRUPTS_SENSE_CONTROL_SETUP

#  else
#    error
#  endif
#endif

/**
 * define timer / counter interrupt macros
 */
#if  defined(__AVR_ATtiny1634__) || defined(__AVR_ATmega16__)
//#  define __PRESCALER_DISCONNECTED (0 << CS02) | (0 << CS01) | (0 << CS00)
#  define __PRESCALER_1 (0 << CS02) | (0 << CS01) | (1 << CS00)
#  define __PRESCALER_8 (0 << CS02) | (1 << CS01) | (0 << CS00)
//#  define __PRESCALER_64 (0 << CS02) | (1 << CS01) | (1 << CS00)
//#  define __PRESCALER_256 (1 << CS02) | (0 << CS01) | (0 << CS00)
//#  define __PRESCALER_1024 (1 << CS02) | (0 << CS01) | (1 << CS00)
#endif

/**
 * define timer / counter 0 interrupt macros for RX
 */
#ifdef __AVR_ATtiny1634__
#    define __TIMER0_INTERRUPT_CLEAR_PENDING TIFR = (1 << OCF0A)
#  define __TIMER0_INTERRUPT_OUTPUT_MODE_DISCONNECTED_SETUP TCCR0A unsetBit \
    ((1 << COM0A1) | (1 << COM0A0) | (1 << COM0B1) | (1 << COM0B0))
#  define __TIMER0_INTERRUPT_WAVE_GENERATION_MODE_NORMAL_SETUP TCCR0A unsetBit \
        ((1 << WGM01) | (1 << WGM00)); TCCR0B unsetBit (1 << WGM02)
#  define __TIMER0_INTERRUPT_COMPARE_VALUE_SETUP(compareValue) OCR0A = compareValue
#  define __TIMER0_INTERRUPT_PRESCALER_DISABLE TCCR0B  unsetBit ((1 << CS02) | (1 << CS01) | (1 << CS00))
#  define __TIMER0_INTERRUPT_PRESCALER_ENABLE(prescaler) TCCR0B setBit (prescaler)
#  define __TIMER0_COMPARE_INTERRUPT_ENABLE TIMSK setBit (1 << OCIE0A)
#  define __TIMER0_COMPARE_INTERRUPT_DISABLE TIMSK unsetBit (1 << OCIE0A)
#else
#  ifdef __AVR_ATmega16__
#    define __TIMER0_INTERRUPT_CLEAR_PENDING TIFR = (1 << OCF0)
#    define __TIMER0_INTERRUPT_OUTPUT_MODE_DISCONNECTED_SETUP TCCR0 unsetBit \
      ((1 << COM00) | (1 << COM01))
#    define __TIMER0_INTERRUPT_WAVE_GENERATION_MODE_NORMAL_SETUP TCCR0 unsetBit \
    ((1 << WGM00) | (1 << WGM01))
#    define __TIMER0_INTERRUPT_COMPARE_VALUE_SETUP(compareValue) OCR0 = compareValue
#    define __TIMER0_INTERRUPT_PRESCALER_DISABLE TCCR0 unsetBit((1 << CS02) | (1 << CS01) | (1 << CS00))
#    define __TIMER0_INTERRUPT_PRESCALER_ENABLE(prescaler) TCCR0 setBit (prescaler)
#    define __TIMER0_COMPARE_INTERRUPT_ENABLE TIMSK setBit (1 << OCIE0)
#    define __TIMER0_COMPARE_INTERRUPT_DISABLE TIMSK unsetBit (1 << OCIE0)
#  else
#    error
#  endif
#endif

/**
 * define timer / counter 1 interrupt macros
 */
#if  defined(__AVR_ATtiny1634__) || defined(__AVR_ATmega16__)
#  define __TIMER1_INTERRUPT_CLEAR_PENDING TIFR = ((1 << OCF1B) | (1 << OCF1A))
#  define __TIMER1_INTERRUPT_OUTPUT_MODE_DISCONNECTED_SETUP TCCR1A unsetBit \
    ((1 << COM1A1) | (1 << COM1A0) | (1 << COM1B1) | (1 << COM1B0))

#  define __TIMER1_INTERRUPT_WAVE_GENERATION_MODE_NORMAL_SETUP TCCR1A unsetBit \
    ((1 << WGM11) | (1 << WGM10)); \
    TCCR1B unsetBit ((1 << WGM13) | (1 << WGM12));

#  define __TIMER1_INTERRUPT_PRESCALER_ENABLE(prescaler) TCCR1B setBit(prescaler)

#  define __TIMER1_INTERRUPT_COMPARE_VALUE_A_SETUP(compareValue) OCR1A = compareValue
#  define __TIMER1_INTERRUPT_COMPARE_VALUE_B_SETUP(compareValue) OCR1B = compareValue

#  define __TIMER1_INTERRUPT_PRESCALER_DISABLE TCCR1B unsetBit((1 << CS02) | (1 << CS01) | (1 << CS00))

#  define __TIMER1_COMPARE_A_INTERRUPT_ENABLE TIMSK setBit bit(OCIE1A)
#  define __TIMER1_COMPARE_A_INTERRUPT_DISABLE TIMSK unsetBit bit(OCIE1A)

#  define __TIMER1_COMPARE_B_INTERRUPT_ENABLE TIMSK setBit bit(OCIE1B)
#  define __TIMER1_COMPARE_B_INTERRUPT_DISABLE TIMSK unsetBit bit(OCIE1B)
#else
#  error
#endif

/**
 * define timer / counter 1 neighbour discovery interrupt macros
 */
#define __TIMER_NEIGHBOUR_SENSE_PRESCALER_ENABLE __TIMER1_INTERRUPT_PRESCALER_ENABLE(__PRESCALER_8)

#define TIMER_NEIGHBOUR_SENSE_SETUP __TIMER1_INTERRUPT_CLEAR_PENDING; \
    __TIMER1_INTERRUPT_OUTPUT_MODE_DISCONNECTED_SETUP; \
    __TIMER1_INTERRUPT_WAVE_GENERATION_MODE_NORMAL_SETUP; \
    __TIMER1_INTERRUPT_COMPARE_VALUE_A_SETUP(DEFAULT_NEIGHBOUR_SENSING_COUNTER_COMPARE_VALUE); \
    __TIMER1_INTERRUPT_PRESCALER_DISABLE; \
    __TIMER1_COMPARE_A_INTERRUPT_ENABLE

#define TIMER_NEIGHBOUR_SENSE_DISABLE __TIMER1_INTERRUPT_PRESCALER_DISABLE; __TIMER1_COMPARE_A_INTERRUPT_DISABLE
#define TIMER_NEIGHBOUR_SENSE_ENABLE __TIMER1_COMPARE_A_INTERRUPT_ENABLE; __TIMER_NEIGHBOUR_SENSE_PRESCALER_ENABLE
#define TIMER_NEIGHBOUR_SENSE_COUNTER TCNT1

/**
 *  define timer / counter 0/1 reception/transmission interrupt macros
 */
#if  defined(__AVR_ATtiny1634__) || defined(__AVR_ATmega16__)

#  define __DEFAULT_TX_RX_COMPARE_A_VALUE DEFAULT_TX_RX_COMPARE_TOP_VALUE
#  define __DEFAULT_TX_COMPARE_B_VALUE (uint16_t)(__DEFAULT_TX_RX_COMPARE_A_VALUE/2)
#  define __DEFAULT_RX_COMPARE_A_VALUE (uint8_t)(__DEFAULT_TX_RX_COMPARE_A_VALUE/8)

#  define __TIMER_TX_RX_PRESCALER_ENABLE __TIMER0_INTERRUPT_PRESCALER_ENABLE(__PRESCALER_1); \
    __TIMER1_INTERRUPT_PRESCALER_ENABLE(__PRESCALER_1)

#  define __TIMER_0_TX_RX_SETUP __TIMER0_INTERRUPT_CLEAR_PENDING; \
    __TIMER0_INTERRUPT_OUTPUT_MODE_DISCONNECTED_SETUP; \
    __TIMER0_INTERRUPT_WAVE_GENERATION_MODE_NORMAL_SETUP; \
    __TIMER0_INTERRUPT_COMPARE_VALUE_SETUP(__DEFAULT_RX_COMPARE_A_VALUE); \
    __TIMER0_INTERRUPT_PRESCALER_DISABLE; \
    __TIMER0_COMPARE_INTERRUPT_DISABLE

#  define __TIMER_1_TX_RX_SETUP __TIMER1_INTERRUPT_CLEAR_PENDING; \
    __TIMER1_INTERRUPT_OUTPUT_MODE_DISCONNECTED_SETUP; \
    __TIMER1_INTERRUPT_WAVE_GENERATION_MODE_NORMAL_SETUP; \
    __TIMER1_INTERRUPT_COMPARE_VALUE_A_SETUP(__DEFAULT_TX_RX_COMPARE_A_VALUE); \
    __TIMER1_INTERRUPT_COMPARE_VALUE_B_SETUP(__DEFAULT_TX_COMPARE_B_VALUE); \
    __TIMER1_INTERRUPT_PRESCALER_DISABLE; \
    __TIMER1_COMPARE_A_INTERRUPT_DISABLE; \
    __TIMER1_COMPARE_B_INTERRUPT_DISABLE

#  define TIMER_TX_RX_COUNTER0 TCNT0
#  define TIMER_TX_RX_COUNTER1 TCNT1

#  define TIMER_TX_RX_COUNTER0_MAX UINT8_MAX
#  define TIMER_TX_RX_COUNTER1_MAX UINT16_MAX

#  define TIMER_TX_RX_COUNTER1_CLEAR_PENDING_INTERRUPTS __TIMER1_INTERRUPT_CLEAR_PENDING

#  define TIMER_TX_RX_SETUP __TIMER_0_TX_RX_SETUP; __TIMER_1_TX_RX_SETUP; \
    __TIMER_TX_RX_PRESCALER_ENABLE

#  define TIMER_TX_RX_COUNTER0_PAUSE __TIMER0_INTERRUPT_PRESCALER_DISABLE
#  define TIMER_TX_RX_COUNTER1_PAUSE __TIMER1_INTERRUPT_PRESCALER_DISABLE

#  define TIMER_TX_RX_COUNTER0_RESUME __TIMER0_INTERRUPT_PRESCALER_ENABLE(__PRESCALER_1);
#  define TIMER_TX_RX_COUNTER1_RESUME __TIMER1_INTERRUPT_PRESCALER_ENABLE(__PRESCALER_1);

#  define TIMER_TX_RX_DISABLE __TIMER0_COMPARE_INTERRUPT_DISABLE; \
    __TIMER1_COMPARE_A_INTERRUPT_DISABLE; \
    __TIMER1_COMPARE_B_INTERRUPT_DISABLE; \
    __TIMER0_INTERRUPT_PRESCALER_DISABLE; \
    __TIMER1_INTERRUPT_PRESCALER_DISABLE

#  define TIMER_TX_RX_ENABLE __TIMER0_COMPARE_INTERRUPT_ENABLE; \
    __TIMER1_COMPARE_A_INTERRUPT_ENABLE; \
    __TIMER1_COMPARE_B_INTERRUPT_ENABLE; \
    __TIMER_TX_RX_PRESCALER_ENABLE
#  else
#    error
#  endif

#endif
