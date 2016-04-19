/**
 * @author Raoul Rubien 2016
 */

#ifndef __COMMUNICATION_TYPES_H
#define __COMMUNICATION_TYPES_H

#include <inttypes.h>
#include <stdbool.h>
#include "../../simulation/SimulationMacros.h"

#  define FUNC_ATTRS // inline

/**
 * Describes a bit within a 4 byte buffer.
 */
typedef struct {
    uint8_t byteNumber : 2; // byte number
    uint8_t __pad: 6;
    uint8_t bitMask; // the bit in the byte
} BufferBitPointer; // 1 + 1 = 2 bytes total

/**
 * Provides a linear 4 byte buffer and a bit pointer per communication channel.
 * The struct is used for transmission and reception as well.
 */
typedef struct {
    uint8_t bytes[4]; // reception buffer
    BufferBitPointer pointer; // points to the next free position
} PortBuffer; // 4 + 2 = 6 bytes total


typedef struct {
    PortBuffer buffer;
} TxPort; // 6 bytes total

typedef struct {
    TxPort north;
    TxPort east;
    TxPort south;
} TxPorts; // 3 * 6 = 18 bytes total

typedef struct {
    uint16_t receptionOffset; // synchronization offset of fist received bit relative to compare counter
    uint16_t estimatedCounterTop; // the estimated reception counter's top value
    uint16_t estimatedCounterCenter;
    uint16_t receptionDelta; // time span for reception classification: center|top +/- delta
#ifdef IS_SIMULATION
    int8_t estimatedTopCounterOffset; // an approximated counter offset
    //uint8_t estimatedScaleFactor; // scale factor according to the approximated counter offset
// factor to percent: {(0.5 - (uint8_max - estimatedScaleFactor) / uint8_max) / 2.0]% in [-0.25, #0.25]
#endif
} TimerCounterAdjustment; // 2 + 1 + (+ 1) = 4 bytes total


typedef struct {
    // port specific reception offsets and factors according to tis port's reception
    TimerCounterAdjustment adjustment;
    PortBuffer buffer;
    uint8_t isReceiving : 4; // is decremented on each expected coding flank, set to top on reception
    uint8_t __pad: 4;
} RxPort; // 4 + 6 + 1 = 11 bytes total


typedef struct {
    RxPort north;
    RxPort east;
    RxPort south;
} RxPorts; // 3 * 11 = 33 bytes total

typedef struct {
    TxPorts tx;
    RxPorts rx;
} Ports; // 18 + 33 = 51 bytes total


/**
 * returns true if the pointer points at the very first position, else false
 */
FUNC_ATTRS uint8_t isBufferEmpty(volatile BufferBitPointer *bufferPointer) {
    if (bufferPointer->bitMask == 0 && bufferPointer->byteNumber == 0) {
        return true;
    }
    return false;
}

/**
 * returns true if the pointer points at the very last position, else false
 */
FUNC_ATTRS uint8_t isBufferFull(volatile BufferBitPointer *bufferPointer) {
    if (bufferPointer->bitMask == (1 << 7) &&
        bufferPointer->byteNumber == sizeof(((PortBuffer *) 0)->bytes)) {
        return true;
    }
    return false;
}

#include "../ParticleParameters.h"

FUNC_ATTRS void constructTimerCounterAdjustment(volatile TimerCounterAdjustment *o) {
    o->receptionOffset = 0;
    o->estimatedCounterTop = DEFAULT_TX_RX_COMPARE_TOP_VALUE;
    o->estimatedCounterCenter = DEFAULT_TX_RX_COMPARE_TOP_VALUE / TX_RX_COUNTER_CENTER_VALUE_DIVISOR;
    o->receptionDelta = DEFAULT_TX_RX_COMPARE_TOP_VALUE / TX_RX_RECEPTION_DELTA_VALUE_DIVISOR - 1;
#ifdef IS_SIMULATION
    o->estimatedTopCounterOffset = 0;
//    o->estimatedScaleFactor = 127;
#endif
}

FUNC_ATTRS void constructBufferBitPointer(volatile BufferBitPointer *o) {
    o->byteNumber = 0;
    o->bitMask = 1;
}

FUNC_ATTRS void constructPortBuffer(volatile PortBuffer *o) {
    for (int i = 0; i < sizeof(o->bytes); i++) {
        o->bytes[i] = 0;
    }
    constructBufferBitPointer(&(o->pointer));
}

FUNC_ATTRS void constructTxPort(volatile TxPort *o) {
    constructPortBuffer(&(o->buffer));
}

FUNC_ATTRS void constructTxPorts(volatile TxPorts *o) {
    constructTxPort(&(o->north));
    constructTxPort(&(o->east));
    constructTxPort(&(o->south));
}

FUNC_ATTRS void constructRxPort(volatile RxPort *o) {
    constructTimerCounterAdjustment(&(o->adjustment));
    o->isReceiving = 0;
    constructPortBuffer(&(o->buffer));
}

FUNC_ATTRS void constructRxPorts(volatile RxPorts *o) {
    constructRxPort(&(o->north));
    constructRxPort(&(o->east));
    constructRxPort(&(o->south));
}

FUNC_ATTRS void constructPorts(volatile Ports *o) {
    constructTxPorts(&(o->tx));
    constructRxPorts(&(o->rx));
}

#  ifdef FUNC_ATTRS
#    undef FUNC_ATTRS
#  endif
#endif