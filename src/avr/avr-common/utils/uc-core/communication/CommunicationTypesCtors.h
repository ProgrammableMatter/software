/**
 * @author Raoul Rubien 2016
 */
#pragma once

#include "CommunicationTypes.h"
#include "ManchesterDecodingTypesCtors.h"


extern FUNC_ATTRS void constructBufferBitPointer(volatile BufferBitPointer *o);

FUNC_ATTRS void constructBufferBitPointer(volatile BufferBitPointer *o) {
    o->byteNumber = 0;
    o->bitMask = 1;
}

extern FUNC_ATTRS void constructPortBuffer(volatile PortBuffer *o);

FUNC_ATTRS void constructPortBuffer(volatile PortBuffer *o) {
    for (uint8_t i = 0; i < sizeof(o->bytes); i++) {
        o->bytes[i] = 0;
    }
    o->bytes[0] = 0x1;
    constructBufferBitPointer(&(o->pointer));
}

extern FUNC_ATTRS void constructTxPort(volatile TxPort *o);

FUNC_ATTRS void constructTxPort(volatile TxPort *o) {
    constructPortBuffer(&(o->buffer));
    constructBufferBitPointer(&o->dataEndPos);
    // TODO: why is the offset 1 instead of 2 (size of BufferBitPointer dataEndPos)
    *((uint8_t *) (&o->dataEndPos + 1)) = 0x00;
//    o->enableTransmission = false;
//    o->retainTransmission = false;
//    o->isTransmitting = false;
}

extern FUNC_ATTRS void constructTxPorts(volatile TxPorts *o);

FUNC_ATTRS void constructTxPorts(volatile TxPorts *o) {
    constructTxPort(&(o->north));
    constructTxPort(&(o->east));
    constructTxPort(&(o->south));
}

extern FUNC_ATTRS void constructRxPort(volatile RxPort *o);

FUNC_ATTRS void constructRxPort(volatile RxPort *o) {
    constructRxSnapshotBuffer(&o->snapshotsBuffer);
    constructPortBuffer(&(o->buffer));
    o->receptionOffset = 0;
    o->isOverflowed = false;
    o->isDataBuffered = false;
}

extern FUNC_ATTRS void constructTimerCounterAdjustment(volatile TimerCounterAdjustment *o);

FUNC_ATTRS void constructTimerCounterAdjustment(volatile TimerCounterAdjustment *o) {
    o->maxCounterValue = DEFAULT_TX_RX_COMPARE_TOP_VALUE;
    o->maxShortIntervalDuration = DEFAULT_MAX_SHORT_RECEPTION_SNAPSHOT_DISTANCE;
    o->maxLongIntervalDuration = DEFAULT_MAX_LONG_RECEPTION_SNAPSHOT_DISTANCE;
}

extern FUNC_ATTRS void constructRxPorts(volatile RxPorts *o);

FUNC_ATTRS void constructRxPorts(volatile RxPorts *o) {
    constructTimerCounterAdjustment(&o->timerAdjustment);
    constructRxPort(&(o->north));
    constructRxPort(&(o->east));
    constructRxPort(&(o->south));
}

extern FUNC_ATTRS void constructPorts(volatile Ports *o);

FUNC_ATTRS void constructPorts(volatile Ports *o) {
    constructTxPorts(&(o->tx));
    constructRxPorts(&(o->rx));
    o->xmissionState = STATE_TYPE_XMISSION_TYPE_DISABLED_TX_RX;
}
