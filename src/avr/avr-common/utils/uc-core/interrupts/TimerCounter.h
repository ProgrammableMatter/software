/**
 * @author Raoul Rubien
 * 23.11.16
 */

#pragma once

#include "common/PortInteraction.h"
#include "uc-core/fw-configuration/ParticleParameters.h"
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
#    define __TIMER0_INTERRUPT_CLEAR_PENDING ((TIFR & (1 << OCF0A)) != 0) ? TIFR = (1 << OCF0A) : 0
#  define __TIMER0_INTERRUPT_OUTPUT_MODE_DISCONNECTED_SETUP TCCR0A unsetBit \
    ((1 << COM0A1) | (1 << COM0A0) | (1 << COM0B1) | (1 << COM0B0))
#  define __TIMER0_INTERRUPT_WAVE_GENERATION_MODE_NORMAL_SETUP TCCR0A unsetBit \
        ((1 << WGM01) | (1 << WGM00)); TCCR0B unsetBit (1 << WGM02)
#  define __TIMER0_INTERRUPT_COMPARE_VALUE_SETUP(compareValue) (TIMER_TX_RX_COMPARE_TIMEOUT_VALUE = compareValue)
#  define __TIMER0_INTERRUPT_PRESCALER_DISABLE TCCR0B  unsetBit ((1 << CS02) | (1 << CS01) | (1 << CS00))
#  define __TIMER0_INTERRUPT_PRESCALER_ENABLE(prescaler) TCCR0B setBit (prescaler)
#  define __TIMER0_COMPARE_INTERRUPT_ENABLE TIMSK setBit (1 << OCIE0A)
#  define __TIMER0_COMPARE_INTERRUPT_DISABLE TIMSK unsetBit (1 << OCIE0A)
#else
#  ifdef __AVR_ATmega16__
#    define __TIMER0_INTERRUPT_CLEAR_PENDING ((TIFR & (1 << OCF0)) != 0) ? TIFR = (1 << OCF0) : 0
#    define __TIMER0_INTERRUPT_OUTPUT_MODE_DISCONNECTED_SETUP TCCR0 unsetBit \
      ((1 << COM00) | (1 << COM01))
#    define __TIMER0_INTERRUPT_WAVE_GENERATION_MODE_NORMAL_SETUP TCCR0 unsetBit \
    ((1 << WGM00) | (1 << WGM01))
#    define __TIMER0_INTERRUPT_COMPARE_VALUE_SETUP(compareValue) (TIMER_TX_RX_COMPARE_TIMEOUT_VALUE = compareValue)
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

#  define __TIMER1_INTERRUPT_CLEAR_PENDING_COMPARE_A ((TIFR & (1 << OCF1A)) != 0) ? TIFR = (1 << OCF1A) : 0
#  define __TIMER1_INTERRUPT_CLEAR_PENDING_COMPARE_B ((TIFR & (1 << OCF1B)) != 0) ? TIFR = (1 << OCF1B) : 0

#  define __TIMER1_INTERRUPT_CLEAR_PENDING \
    __TIMER1_INTERRUPT_CLEAR_PENDING_COMPARE_A; \
    __TIMER1_INTERRUPT_CLEAR_PENDING_COMPARE_B

#  define __TIMER1_INTERRUPT_OUTPUT_MODE_DISCONNECTED_SETUP TCCR1A unsetBit \
    ((1 << COM1A1) | (1 << COM1A0) | (1 << COM1B1) | (1 << COM1B0))

#  define __TIMER1_INTERRUPT_WAVE_GENERATION_MODE_CTC_SETUP TCCR1A unsetBit \
    ((1 << WGM11) | (1 << WGM10)); \
    TCCR1B unsetBit ((1 << WGM13) | (1 << WGM12)); \
    TCCR1B setBit ((1 << WGM12))

#  define __TIMER1_INTERRUPT_WAVE_GENERATION_MODE_NORMAL_SETUP TCCR1A unsetBit \
    ((1 << WGM11) | (1 << WGM10)); \
    TCCR1B unsetBit ((1 << WGM13) | (1 << WGM12))


#  define __TIMER1_INTERRUPT_PRESCALER_ENABLE(prescaler) TCCR1B setBit(prescaler)

#  define __TIMER1_INTERRUPT_COMPARE_VALUE_A_SETUP(compareValue) OCR1A = (compareValue)
#  define __TIMER1_INTERRUPT_COMPARE_VALUE_B_SETUP(compareValue) OCR1B = (compareValue)

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
    __TIMER1_INTERRUPT_WAVE_GENERATION_MODE_CTC_SETUP; \
    __TIMER1_INTERRUPT_COMPARE_VALUE_A_SETUP(DEFAULT_NEIGHBOUR_SENSING_COUNTER_COMPARE_VALUE); \
    __TIMER1_INTERRUPT_PRESCALER_DISABLE; \
    __TIMER1_COMPARE_A_INTERRUPT_ENABLE

#define TIMER_NEIGHBOUR_SENSE_DISABLE __TIMER1_COMPARE_A_INTERRUPT_DISABLE
#define TIMER_NEIGHBOUR_SENSE_ENABLE __TIMER1_COMPARE_A_INTERRUPT_ENABLE

#define TIMER_NEIGHBOUR_SENSE_PAUSE __TIMER1_INTERRUPT_PRESCALER_DISABLE;
#define TIMER_NEIGHBOUR_SENSE_RESUME __TIMER_NEIGHBOUR_SENSE_PRESCALER_ENABLE
#define TIMER_NEIGHBOUR_SENSE_COUNTER_VALUE TCNT1

/**
 *  define timer / counter 0/1 reception/transmission interrupt macros
 */
#if  defined(__AVR_ATtiny1634__) || defined(__AVR_ATmega16__)

#  define __TIMER_TX_RX_COUNTER_PRESCALER_ENABLE __TIMER1_INTERRUPT_PRESCALER_ENABLE(__PRESCALER_1)
#  define __TIMER_TX_RX_COUNTER_PRESCALER_DISABLE __TIMER1_INTERRUPT_PRESCALER_DISABLE

#  define __TIMER_TX_RX_TIMEOUT_COUNTER_PRESCALER_ENABLE __TIMER0_INTERRUPT_PRESCALER_ENABLE(__PRESCALER_1)
#  define __TIMER_TX_RX_TIMEOUT_COUNTER_PRESCALER_DISABLE __TIMER0_INTERRUPT_PRESCALER_DISABLE

#  define __TIMER_0_TX_RX_SETUP __TIMER0_INTERRUPT_CLEAR_PENDING; \
    TCNT0 = 0; \
    __TIMER0_INTERRUPT_OUTPUT_MODE_DISCONNECTED_SETUP; \
    __TIMER0_INTERRUPT_WAVE_GENERATION_MODE_NORMAL_SETUP; \
    __TIMER0_INTERRUPT_COMPARE_VALUE_SETUP(DEFAULT_TX_RX_COMPARE_TOP_VALUE / RX_TIMEOUT_INTERRUPT_VALUE_DIVISOR); \
    __TIMER0_INTERRUPT_PRESCALER_DISABLE; \
    __TIMER0_COMPARE_INTERRUPT_DISABLE

//
#  define __TIMER_1_TX_RX_SETUP __TIMER1_INTERRUPT_CLEAR_PENDING; \
    TCNT1 = 0; \
    __TIMER1_INTERRUPT_OUTPUT_MODE_DISCONNECTED_SETUP; \
    __TIMER1_INTERRUPT_WAVE_GENERATION_MODE_NORMAL_SETUP; \
    __TIMER1_INTERRUPT_PRESCALER_DISABLE; \
    __TIMER1_COMPARE_A_INTERRUPT_DISABLE; \
    __TIMER1_COMPARE_B_INTERRUPT_DISABLE

#  define TIMER_ACTUATOR_COUNTER_VALUE (TCNT0)
#  define TIMER_TX_RX_COUNTER_VALUE (TCNT1)

#  define TIMER_TX_RX_COMPARE_VALUE (OCR1A)
#  define TIMER_TX_RX__UNUSED__COMPARE_VALUE (OCR1B)
#  ifdef __AVR_ATtiny1634__
#    define TIMER_ACTUATOR_COMPARE_VALUE (OCR0A)
#  else
#     define TIMER_ACTUATOR_COMPARE_VALUE (OCR0)
#  endif

#  define TIMER_ACTUATOR_COUNTER_PAUSE __TIMER0_INTERRUPT_PRESCALER_DISABLE
#  define TIMER_ACTUATOR_COUNTER_DISABLE __TIMER0_INTERRUPT_PRESCALER_DISABLE
#  define TIMER_ACTUATOR_COUNTER_RESUME __TIMER0_INTERRUPT_PRESCALER_ENABLE(__PRESCALER_1);
#  define TIMER_ACTUATOR_COUNTER_ENABLE __TIMER0_INTERRUPT_PRESCALER_ENABLE(__PRESCALER_1);

#  define TIMER_TX_RX_COUNTER_CLEAR_PENDING_INTERRUPTS __TIMER1_INTERRUPT_CLEAR_PENDING
#  define TIMER_TX_RX_COUNTER_SETUP \
    __TIMER_1_TX_RX_SETUP; \
    __TIMER_TX_RX_COUNTER_PRESCALER_DISABLE
#  define TIMER_TX_RX_COUNTER_PAUSE __TIMER1_INTERRUPT_PRESCALER_DISABLE
#  define TIMER_TX_RX_COUNTER_DISABLE __TIMER1_INTERRUPT_PRESCALER_DISABLE
#  define TIMER_TX_RX_COUNTER_ENABLE __TIMER1_INTERRUPT_PRESCALER_ENABLE(__PRESCALER_1);
#  define TIMER_TX_RX_COUNTER_RESUME __TIMER1_INTERRUPT_PRESCALER_ENABLE(__PRESCALER_1);

#  define TIMER_TX_RX_DISABLE_COMPARE_INTERRUPT \
    __TIMER1_COMPARE_A_INTERRUPT_DISABLE

#  define TIMER_TX_RX_ENABLE_COMPARE_INTERRUPT \
    TIMER_TX_RX_COUNTER_CLEAR_PENDING_INTERRUPTS; \
    __TIMER1_COMPARE_A_INTERRUPT_ENABLE

#define TIMER_ACTUATOR_COMPARE_ENABLE __TIMER0_COMPARE_INTERRUPT_ENABLE
#define TIMER_ACTUATOR_COMPARE_DISABLE __TIMER0_COMPARE_INTERRUPT_DISABLE

#  else
#    error
#  endif
