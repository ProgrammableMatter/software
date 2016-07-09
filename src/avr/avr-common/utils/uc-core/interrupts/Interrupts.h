/**
 * @author Raoul Rubien 2015
 */
#pragma once

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <uc-core/particle/ParticleStateTypes.h>

#include "Vectors.h"
#include "TimerCounter.h"
#include "Reception.h"
#include "uc-core/communication/Transmission.h"
#include "uc-core/particle/Globals.h"
#include "uc-core/discovery/Discovery.h"
#include "uc-core/configuration/IoPins.h"
#include "uc-core/communication/ManchesterDecoding.h"
#include "uc-core/communication/ManchesterCoding.h"

#ifdef SIMULATION

#  include "simulation/SimulationMacros.h"

#endif

#define TIMER_NEIGHBOUR_SENSE_COUNTER_ON_INTERRUPT_ROLLBACK 12

extern FUNC_ATTRS void __handleInputInterrupt(volatile DiscoveryPulseCounter *discoveryPulseCounter,
                                              volatile RxPort *rxPort,
                                              const bool isRxHigh);
/**
 * Handles interrupt in input pins according to the particle state.
 */
FUNC_ATTRS void __handleInputInterrupt(volatile DiscoveryPulseCounter *discoveryPulseCounter,
                                       volatile RxPort *rxPort,
                                       const bool isRxHigh) {
    uint16_t timerCounterValue = TIMER_TX_RX_COUNTER_VALUE;
    switch (ParticleAttributes.node.state) {
        // on discovery pulse
        case STATE_TYPE_NEIGHBOURS_DISCOVERY:
            if (!isRxHigh) {
                dispatchFallingDiscoveryEdge(discoveryPulseCounter);
            }
            break;

        default:
            switch (ParticleAttributes.communication.xmissionState) {
                // on data received
                case STATE_TYPE_XMISSION_TYPE_ENABLED_TX_RX:
                case STATE_TYPE_XMISSION_TYPE_ENABLED_RX:
                    captureSnapshot(&timerCounterValue, isRxHigh, &rxPort->snapshotsBuffer);
                    break;

                default:
                    break;
            }
            break;
    }
}

extern FUNC_ATTRS void scheduleNextTransmission(void);
/**
 * schedules the next transmission interrupt on available data to be sent.
 */
FUNC_ATTRS void scheduleNextTransmission(void) {
    if (ParticleAttributes.communication.ports.tx.north.isTransmitting ||
        ParticleAttributes.communication.ports.tx.east.isTransmitting ||
        ParticleAttributes.communication.ports.tx.south.isTransmitting) {
        scheduleNextTxInterrupt();
    } else {
        TIMER_TX_RX_DISABLE_COMPARE_INTERRUPT;
    }
}


/**
 * north RX pin change interrupt on logical pin change
 * int. #19
 */
ISR(NORTH_PIN_CHANGE_INTERRUPT_VECT) {
    if (ParticleAttributes.protocol.isBroadcastEnabled) {
        if (NORTH_RX_IS_HI) {
            simultaneousTxLoImpl();
        } else {
            simultaneousTxHiImpl();
        }
    }
    __handleInputInterrupt(&ParticleAttributes.discoveryPulseCounters.north,
                           &ParticleAttributes.communication.ports.rx.north,
                           NORTH_RX_IS_HI);
    RX_NORTH_INTERRUPT_CLEAR_PENDING;
}

/**
 * east RX pin change interrupt on logical pin change
 * int. #3
 */
ISR(EAST_PIN_CHANGE_INTERRUPT_VECT) {
    __handleInputInterrupt(&ParticleAttributes.discoveryPulseCounters.east,
                           &ParticleAttributes.communication.ports.rx.east,
                           EAST_RX_IS_HI);
    RX_EAST_INTERRUPT_CLEAR_PENDING;
}

/**
 * south RX pin change interrupt on logical pin change
 * int. #2
 */
ISR(SOUTH_PIN_CHANGE_INTERRUPT_VECT) {
    __handleInputInterrupt(&ParticleAttributes.discoveryPulseCounters.south,
                           &ParticleAttributes.communication.ports.rx.south,
                           SOUTH_RX_IS_HI);
    RX_SOUTH_INTERRUPT_CLEAR_PENDING;
}

/**
 * int. #7
 */
ISR(TX_TIMER_INTERRUPT_VECT) {

    switch (ParticleAttributes.node.state) {
        case STATE_TYPE_START:
        case STATE_TYPE_ACTIVE:
            break;

            // on generate discovery pulse
        case STATE_TYPE_NEIGHBOURS_DISCOVERY:
        case STATE_TYPE_NEIGHBOURS_DISCOVERED:
        case STATE_TYPE_DISCOVERY_PULSING:
            NORTH_TX_TOGGLE;
            SOUTH_TX_TOGGLE;
            EAST_TX_TOGGLE;
            break;

            // otherwise process transmission
        default:
            switch (ParticleAttributes.communication.xmissionState) {
                case STATE_TYPE_XMISSION_TYPE_ENABLED_TX_RX:
                case STATE_TYPE_XMISSION_TYPE_ENABLED_TX:
                    if (ParticleAttributes.protocol.isSimultaneousTransmissionEnabled) {
                        transmit(ParticleAttributes.communication.ports.tx.simultaneous, simultaneousTxHiImpl,
                                 simultaneousTxLoImpl);
                    } else {
                        transmit(&ParticleAttributes.communication.ports.tx.north, northTxHiImpl,
                                 northTxLoImpl);
                        transmit(&ParticleAttributes.communication.ports.tx.east, eastTxHiImpl, eastTxLoImpl);
                        transmit(&ParticleAttributes.communication.ports.tx.south, southTxHiImpl,
                                 southTxLoImpl);
                    }
                    scheduleNextTransmission();
                    break;

                default:
                    break;
            }
            break;
    }
}


/**
 * int. #8
 */
EMPTY_INTERRUPT(ACTUATOR_TIMER_INTERRUPT_VECT)
//{
//}

EMPTY_INTERRUPT(__UNUSED_TIMER1_OVERFLOW_INTERRUPT_VECT)

# ifdef SIMULATION

const char isrVector0Msg[] PROGMEM = "BAD ISR";
/**
 * On external pin, power-on reset, brown-out reset, watchdog reset.
 */
ISR(_VECTOR(0)) {
    writeToUart((PGM_P) pgm_read_word(&(isrVector0Msg)));
    IF_DEBUG_SWITCH_TO_ERRONEOUS_STATE;
}

/**
 * on timer 0 compare
 * int #19
 */
ISR(__UNUSED_TIMER0_COMPARE_INTERRUPT_VECT) {
    writeToUart((PGM_P) pgm_read_word(&(isrVector0Msg)));
    IF_DEBUG_SWITCH_TO_ERRONEOUS_STATE;
}

/**
 * on timer 0 overflow
 * int #9
 */
ISR(__UNUSED_TIMER0_OVERFLOW_INTERRUPT_VECT) {
    writeToUart((PGM_P) pgm_read_word(&(isrVector0Msg)));
    IF_DEBUG_SWITCH_TO_ERRONEOUS_STATE;
}

ISR(BADISR_vect) {
    IF_DEBUG_SWITCH_TO_ERRONEOUS_STATE;
}

#  else

EMPTY_INTERRUPT(_VECTOR(0))

EMPTY_INTERRUPT(__UNUSED_TIMER0_COMPARE_INTERRUPT_VECT)

EMPTY_INTERRUPT(__UNUSED_TIMER0_OVERFLOW_INTERRUPT_VECT)

EMPTY_INTERRUPT(BADISR_vect)

#  endif

