/**
 * @author Raoul Rubien 2016
 */

#ifndef __COMMUNICATION_H
#define __COMMUNICATION_H

#include <avr/io.h>
#include "../../simulation/SimulationMacros.h"
#include "CommunicationTypes.h"
#include "../../common/common.h"
#include "../ParticleParameters.h"

#  ifdef TRY_INLINE_ISR_RELEVANT
#    define FUNC_ATTRS inline
#  else
#    define FUNC_ATTRS
#  endif

#define __RECEPTION_TIMEOUT_COUNTER_MAX 0xf;

/**
 * Translates the hardware counter to a specific port counter according to the provided TimerCounterAdjustment.
 */
FUNC_ATTRS uint16_t __toPortCounter(const volatile uint16_t *hardwareCounter,
                                    const volatile TimerCounterAdjustment *portTimerArguments) {
    uint16_t portCounter = *hardwareCounter + portTimerArguments->receptionOffset;
    if (portCounter >= TIMER_TX_RX_COMPARE_TOP_VALUE) {
        return portCounter - TIMER_TX_RX_COMPARE_TOP_VALUE;
    } else {
        return portCounter;
    }
}

/**
 * Stores the data bit to the reception buffer unless the buffer is saturated.
 */
FUNC_ATTRS void __storeDataBit(volatile RxPort *rxPort, const volatile uint8_t isRisingEdge) {
    // save bit to buffer
    if (!isBufferEndPosition(&(rxPort->buffer.pointer))) {
        if (isRisingEdge) {
            DEBUG_CHAR_OUT('1');
            rxPort->buffer.bytes[rxPort->buffer.pointer.byteNumber] setBit rxPort->buffer.pointer.bitMask;
        } else {
            DEBUG_CHAR_OUT('0');
            rxPort->buffer.bytes[rxPort->buffer.pointer.byteNumber] unsetBit rxPort->buffer.pointer.bitMask;
        }
        bufferBitPointerIncrement(&rxPort->buffer.pointer);
    } else {
        rxPort->isOverflowed = true;
    }
}

/**
 * In case the counter is greater than the compare value (i.e. compare interrupt was shifted) returns
 * the amount counted over else the timer.
 */
FUNC_ATTRS uint16_t __getTrimmedReceptionCounter(void) {

//#ifdef SIMULATION
//    if (TIMER_TX_RX_COUNTER > (2 * TIMER_TX_RX_COMPARE_TOP_VALUE)) {
//        DEBUG_CHAR_OUT('R');
//        IF_DEBUG_SWITCH_TO_ERRONEOUS_STATE;
//    }
//#endif

    if (TIMER_TX_RX_COUNTER > TIMER_TX_RX_COMPARE_TOP_VALUE) {
        return TIMER_TX_RX_COUNTER - TIMER_TX_RX_COMPARE_TOP_VALUE;
    } else {
        return TIMER_TX_RX_COUNTER;
    }
}

/**
 * Resets the counter. In case the interrupt was shifted (less than 1x compare value), the counter is trimmed,
 * else in case of simulation it switches to erroneous state.
 */
FUNC_ATTRS void resetReceptionCounter(void) {
//#ifdef SIMULATION
//    if (TIMER_TX_RX_COUNTER > (2 * TIMER_TX_RX_COMPARE_TOP_VALUE)) {
//        IF_SIMULATION_CHAR_OUT('r');
//        IF_SIMULATION_SWITCH_TO_ERRONEOUS_STATE;
//    }
//#endif
    if (TIMER_TX_RX_COUNTER > TIMER_TX_RX_COMPARE_TOP_VALUE) {
        TIMER_TX_RX_COUNTER -= TIMER_TX_RX_COMPARE_TOP_VALUE;
    } else {
        TIMER_TX_RX_COUNTER = 0;
    }
}


/**
 * increments the timeout counters
 */
FUNC_ATTRS void advanceReceptionTimeoutCounters(void) {

    if (ParticleAttributes.ports.rx.north.isReceiving == 1) {
        ParticleAttributes.ports.rx.north.isDataBuffered = true;
    }
    ParticleAttributes.ports.rx.north.isReceiving >>= 1;

#ifdef SIMULATION
    if (ParticleAttributes.ports.rx.north.isReceiving == 0) {
        DEBUG_CHAR_OUT('U');
    }
#endif

    if (ParticleAttributes.ports.rx.east.isReceiving == 1) {
        ParticleAttributes.ports.rx.east.isDataBuffered = true;
    }
    ParticleAttributes.ports.rx.east.isReceiving >>= 1;


    if (ParticleAttributes.ports.rx.south.isReceiving == 1) {
        ParticleAttributes.ports.rx.south.isDataBuffered = true;
    }

    ParticleAttributes.ports.rx.south.isReceiving >>= 1;
#ifdef SIMULATION
    if (ParticleAttributes.ports.rx.south.isReceiving == 0) {
        DEBUG_CHAR_OUT('V');
    }
#endif

}


/**
 * Handles received data flanks and stores them according to the received time relative to the reception
 * timer/counter.
 */
FUNC_ATTRS void dispatchReceivedDataEdge(volatile RxPort *rxPort,
                                         const bool isRisingEdge) {

    if (rxPort->isDataBuffered) {
        rxPort->isOverflowed = true;
        return;
    }

    uint16_t hardwareCounter = __getTrimmedReceptionCounter();
    uint16_t captureCounter = __toPortCounter(&hardwareCounter, &(rxPort->adjustment));

//    IF_SIMULATION_INT16_OUT(hardwareCounter);
    DEBUG_INT16_OUT(captureCounter);

    if (isNotReceiving(rxPort)) { // if reception is timed out this call is the first signal of a transmission
        if (isRisingEdge == false) {
            DEBUG_CHAR_OUT('S');
            // synchronize the counter offset for this channel
            rxPort->adjustment.receptionOffset = TIMER_TX_RX_COMPARE_TOP_VALUE - hardwareCounter;
        }
    }
    else { // if signal occurs approx. at 1/2 of a package clock
        if ((TX_RX_RECEPTION_CLASSIFICATION_VALUE_LEFT_BORDER <= captureCounter) &&
            (captureCounter <= TX_RX_RECEPTION_CLASSIFICATION_VALUE_RIGHT_BORDER)) {
//            DEBUG_CHAR_OUT('B');
            __storeDataBit(rxPort, isRisingEdge);
        }
        else // if signal occurs approx. at the end/beginning of a package clock
        {
            // re-adjust reception offset
            rxPort->adjustment.receptionOffset = TIMER_TX_RX_COMPARE_TOP_VALUE - hardwareCounter;
//            DEBUG_CHAR_OUT('A');
        }
    }
    rxPort->isReceiving = __RECEPTION_TIMEOUT_COUNTER_MAX;
}

#  ifdef FUNC_ATTRS
#    undef FUNC_ATTRS
#  endif
#endif