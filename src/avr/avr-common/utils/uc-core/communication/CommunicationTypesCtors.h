/**
 * @author Raoul Rubien 2016
 */
#pragma once

#include "CommunicationTypes.h"
#include "ManchesterDecodingTypesCtors.h"


extern CTOR_ATTRS void constructBufferBitPointer(volatile BufferBitPointer *o);
/**
 * constructor function
 */
CTOR_ATTRS void constructBufferBitPointer(volatile BufferBitPointer *o) {
    o->byteNumber = 0;
    o->bitMask = 1;
}

extern CTOR_ATTRS void constructPortBuffer(volatile PortBuffer *o);
/**
 * constructor function
 */
CTOR_ATTRS void constructPortBuffer(volatile PortBuffer *o) {
    for (uint8_t i = 0; i < sizeof(o->bytes); i++) {
        o->bytes[i] = 0;
    }
//    o->bytes[0] = 0x1;
    constructBufferBitPointer(&(o->pointer));
}

extern CTOR_ATTRS void constructTxPort(volatile TxPort *o);
/**
 * constructor function
 */
CTOR_ATTRS void constructTxPort(volatile TxPort *o) {
    constructPortBuffer(&(o->buffer));
    constructBufferBitPointer(&o->dataEndPos);
    // TODO: why is the offset 1 instead of 2 (size of BufferBitPointer dataEndPos)
    *((uint8_t *) (&o->dataEndPos + 1)) = 0x00;
    o->buffer.bytes[0] = 0x1;
    o->isTxClockPhase = false;
}

extern CTOR_ATTRS void constructTxPorts(volatile TxPorts *o);
/**
 * constructor function
 */
CTOR_ATTRS void constructTxPorts(volatile TxPorts *o) {
    constructTxPort(&o->north);
    constructTxPort(&o->east);
    constructTxPort(&o->south);
    o->simultaneous = (TxPort *) &o->east;
}

extern CTOR_ATTRS void constructRxPort(volatile RxPort *o);
/**
 * constructor function
 */
CTOR_ATTRS void constructRxPort(volatile RxPort *o) {
    constructRxSnapshotBuffer(&o->snapshotsBuffer);
    constructPortBuffer(&(o->buffer));
    o->receptionOffset = 0;
    o->isOverflowed = false;
    o->isDataBuffered = false;
}

extern CTOR_ATTRS void constructTimerCounterAdjustment(volatile TimerCounterAdjustment *o);
/**
 * constructor function
 */
CTOR_ATTRS void constructTimerCounterAdjustment(volatile TimerCounterAdjustment *o) {
    o->maxShortIntervalDurationOvertimePercentageRatio = DEFAULT_MAX_SHORT_RECEPTION_OVERTIME_PERCENTAGE_RATIO;
    o->maxShortIntervalDuration =
            (DEFAULT_MAX_SHORT_RECEPTION_OVERTIME_PERCENTAGE_RATIO / 100.0) * DEFAULT_TX_RX_CLOCK_DELAY;
    o->maxLongIntervalDurationOvertimePercentageRatio = DEFAULT_MAX_LONG_RECEPTION_OVERTIME_PERCENTAGE_RATIO;
    o->maxLongIntervalDuration =
            (DEFAULT_MAX_LONG_RECEPTION_OVERTIME_PERCENTAGE_RATIO / 100.0) * DEFAULT_TX_RX_CLOCK_DELAY;

    o->transmissionClockDelay = DEFAULT_TX_RX_CLOCK_DELAY;
    o->transmissionClockDelayHalf = DEFAULT_TX_RX_CLOCK_DELAY >> 1;
    o->newTransmissionClockDelay = 0;
    o->isTransmissionClockDelayUpdateable = false;

    o->transmissionClockShift = 0;
    o->newTransmissionClockShift = 0;
    o->isTransmissionClockShiftUpdateable = false;
}

extern CTOR_ATTRS void constructRxPorts(volatile RxPorts *o);
/**
 * constructor function
 */
CTOR_ATTRS void constructRxPorts(volatile RxPorts *o) {
    constructRxPort(&(o->north));
    constructRxPort(&(o->east));
    constructRxPort(&(o->south));
}

extern CTOR_ATTRS void constructCommunicationPorts(volatile CommunicationPorts *o);
/**
 * constructor function
 */
CTOR_ATTRS void constructCommunicationPorts(volatile CommunicationPorts *o) {
    constructTxPorts(&(o->tx));
    constructRxPorts(&(o->rx));
}

extern CTOR_ATTRS void constructCommunication(volatile Communication *o);
/**
 * constructor function
 */
CTOR_ATTRS void constructCommunication(volatile Communication *o) {
    constructTimerCounterAdjustment(&o->timerAdjustment);
    o->xmissionState = STATE_TYPE_XMISSION_TYPE_DISABLED_TX_RX;
    constructCommunicationPorts(&o->ports);
}
