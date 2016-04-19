/**
 * @author Raoul Rubien
 * 23.11.16
 */

#ifndef __TIMER_COUNTER_INTERRUPTS_H
#define __TIMER_COUNTER_INTERRUPTS_H

#include <common/PortInteraction.h>
#include "../ParticleParameters.h"
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

#define TIMER_NEIGHBOUR_SENSE_DISABLE __TIMER1_COMPARE_A_INTERRUPT_DISABLE
#define TIMER_NEIGHBOUR_SENSE_ENABLE __TIMER1_COMPARE_A_INTERRUPT_ENABLE

#define TIMER_NEIGHBOUR_SENSE_PAUSE __TIMER1_INTERRUPT_PRESCALER_DISABLE;
#define TIMER_NEIGHBOUR_SENSE_RESUME __TIMER_NEIGHBOUR_SENSE_PRESCALER_ENABLE
#define TIMER_NEIGHBOUR_SENSE_COUNTER TCNT1

/**
 *  define timer / counter 0/1 reception/transmission interrupt macros
 */
#if  defined(__AVR_ATtiny1634__) || defined(__AVR_ATmega16__)

#  define __TIMER_TX_RX_COUNTER_PRESCALER_ENABLE __TIMER1_INTERRUPT_PRESCALER_ENABLE(__PRESCALER_1)
#  define __TIMER_TX_RX_COUNTER_PRESCALER_DISABLE __TIMER1_INTERRUPT_PRESCALER_DISABLE

#  define __TIMER_TX_RX_TIMEOUT_COUNTER_PRESCALER_ENABLE __TIMER0_INTERRUPT_PRESCALER_ENABLE(__PRESCALER_1)
#  define __TIMER_TX_RX_TIMEOUT_COUNTER_PRESCALER_DISABLE __TIMER0_INTERRUPT_PRESCALER_DISABLE

#  define __TIMER_0_TX_RX_SETUP __TIMER0_INTERRUPT_CLEAR_PENDING; \
    __TIMER0_INTERRUPT_OUTPUT_MODE_DISCONNECTED_SETUP; \
    __TIMER0_INTERRUPT_WAVE_GENERATION_MODE_NORMAL_SETUP; \
    __TIMER0_INTERRUPT_COMPARE_VALUE_SETUP(DEFAULT_TX_RX_COMPARE_TOP_VALUE / RX_TIMEOUT_INTERRUPT_VALUE_DIVISOR); \
    __TIMER0_INTERRUPT_PRESCALER_DISABLE; \
    __TIMER0_COMPARE_INTERRUPT_DISABLE

#  define __TIMER_1_TX_RX_SETUP __TIMER1_INTERRUPT_CLEAR_PENDING; \
    __TIMER1_INTERRUPT_OUTPUT_MODE_DISCONNECTED_SETUP; \
    __TIMER1_INTERRUPT_WAVE_GENERATION_MODE_NORMAL_SETUP; \
    __TIMER1_INTERRUPT_COMPARE_VALUE_A_SETUP(DEFAULT_TX_RX_COMPARE_TOP_VALUE); \
    __TIMER1_INTERRUPT_COMPARE_VALUE_B_SETUP(DEFAULT_TX_RX_COMPARE_TOP_VALUE / TX_RX_COUNTER_CENTER_VALUE_DIVISOR); \
    __TIMER1_INTERRUPT_PRESCALER_DISABLE; \
    __TIMER1_COMPARE_A_INTERRUPT_DISABLE; \
    __TIMER1_COMPARE_B_INTERRUPT_DISABLE

#  define TIMER_TX_RX_TIMEOUT_COUNTER TCNT0
#  define TIMER_TX_RX_COUNTER TCNT1

#  define TIMER_TX_RX_COMPARE_TOP_VALUE (OCR1A)
#  define TIMER_TX_RX_COMPARE_CENTER_VALUE (OCR1B)
#  define TIMER_TX_RX_COMPARE_TIMEOIUT_VALUE (OCR0)

#  define TIMER_TX_RX_TIMEOUT_COUNTER_MAX UINT8_MAX
#  define TIMER_TX_RX_COUNTER_MAX UINT16_MAX

#  define TIMER_TX_RX_COUNTER_CLEAR_PENDING_INTERRUPTS __TIMER1_INTERRUPT_CLEAR_PENDING

#  define TIMER_TX_RX_SETUP __TIMER_0_TX_RX_SETUP; __TIMER_1_TX_RX_SETUP; \
    __TIMER_TX_RX_TIMEOUT_COUNTER_PRESCALER_ENABLE; \
    __TIMER_TX_RX_COUNTER_PRESCALER_ENABLE

#  define TIMER_TX_RX_TIMEOUT_COUNTER_PAUSE __TIMER0_INTERRUPT_PRESCALER_DISABLE
#  define TIMER_TX_RX_TIMEOUT_COUNTER_RESUME __TIMER0_INTERRUPT_PRESCALER_ENABLE(__PRESCALER_1);

#  define TIMER_TX_RX_COUNTER_PAUSE __TIMER1_INTERRUPT_PRESCALER_DISABLE
#  define TIMER_TX_RX_COUNTER_RESUME __TIMER1_INTERRUPT_PRESCALER_ENABLE(__PRESCALER_1);

#  define TIMER_TX_RX_DISABLE \
    __TIMER1_COMPARE_A_INTERRUPT_DISABLE; \
    __TIMER1_COMPARE_B_INTERRUPT_DISABLE; \
    __TIMER_TX_RX_COUNTER_PRESCALER_DISABLE

#  define TIMER_TX_RX_ENABLE \
    __TIMER1_COMPARE_A_INTERRUPT_ENABLE; \
    __TIMER1_COMPARE_B_INTERRUPT_ENABLE; \
    __TIMER_TX_RX_COUNTER_PRESCALER_ENABLE

#define TIMER_TX_RX_TIMEOUT_ENABLE __TIMER0_COMPARE_INTERRUPT_ENABLE
#define TIMER_TX_RX_TIMEOUT_DISABLE __TIMER0_COMPARE_INTERRUPT_DISABLE

#  else
#    error
#  endif

#endif
