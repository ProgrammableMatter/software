/**
 * @author Raoul Rubien 2016
 */

#pragma once

#include "Communication.h"
#include "ManchesterDecodingTypes.h"
#include "uc-core/interrupts/TimerCounter.h"
#include "simulation/SimulationMacros.h"

#ifdef TRY_INLINE
#  define FUNC_ATTRS inline
#else
#  define FUNC_ATTRS
#endif


/**
 * Resets the decoder phase state do default.
 */
#define __resetDecoderPhaseState(decoderPhaseState) \
    (decoderPhaseState = 0)

/**
 * evaluates to true if the phase state indicates a phase at data interval, else to false
 * return values:
 * true     ... indicates a data phase
 * false    ...  indicates a clock phase
 */
#define __isDataPhase(manchesterDecoderPhaseState) \
    (manchesterDecoderPhaseState == 1)

/**
 * updates the phase state by a short interval
 */
#define __phaseStateAdvanceShortInterval(manchesterDecoderPhaseState) \
    (manchesterDecoderPhaseState += 1)

/**
 * updates the phase state by a long interval
 */
#define __phaseStateAdvanceLongInterval(manchesterDecoderPhaseState) \
    (manchesterDecoderPhaseState += 2)


/**
 * Stores the data bit to the reception buffer unless the buffer is saturated.
 */
FUNC_ATTRS void __storeDataBit(volatile RxPort *rxPort, const volatile uint8_t isRisingEdge) {
    // save bit to buffer
    if (!isBufferEndPosition(&rxPort->buffer.pointer)) {
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

///**
// * re-sets the iterator to the start position
// */
//FUNC_ATTRS void __rxSnapshotBufferResetIterator(volatile RxSnapshotBuffer *o) {
//    o->iteratorIndex = o->startIndex;
//}

/**
 * Releases the 1st element from the the queue.
 */
FUNC_ATTRS void __rxSnapshotBufferDequeue(volatile RxSnapshotBuffer *o) {
    o->startIndex++;
}

/**
 * Same as dequeue but does not modify the queue.
 */
FUNC_ATTRS volatile Snapshot *__rxSnapshotBufferPeek(volatile RxSnapshotBuffer *o) {
    return &o->snapshots[o->startIndex];
}

///**
// * returns the current element and post-increments the iterator
// */
//FUNC_ATTRS volatile Snapshot *__rxSnapshotBufferGetAndIncrement(volatile RxSnapshotBuffer *o) {
//    return &o->snapshots[o->iteratorIndex++];
//}

///**
// * returns the previous element
// */
//FUNC_ATTRS volatile Snapshot *__rxSnapshotBufferGetPrevious(volatile RxSnapshotBuffer *o) {
//    return &o->snapshots[o->iteratorIndex - 1];
//}

///**
// * returns the current element
// */
//FUNC_ATTRS volatile Snapshot *__rxSnapshotBufferGetCurrent(volatile RxSnapshotBuffer *o) {
//    return &o->snapshots[o->iteratorIndex];
//}

///**
// * returns the next element
// */
//FUNC_ATTRS volatile Snapshot *__rxSnapshotBufferGetNext(volatile RxSnapshotBuffer *o) {
//    return &o->snapshots[o->iteratorIndex + 1];
//}

///**
// * Returns true if a previous value is available.
// * Note: The implementation is undefined for iterator position outside the buffer.
// */
//FUNC_ATTRS bool __rxSnapshotBufferHasPrevious(volatile RxSnapshotBuffer *o) {
//    return o->iteratorIndex != o->startIndex;
//}

///**
// * Returns true if the current value is available.
// * Note: The implementation is undefined for iterator position outside the buffer.
// */
//FUNC_ATTRS bool __rxSnapshotBufferHasCurrent(volatile RxSnapshotBuffer *o) {
//    return o->iteratorIndex != (o->endIndex - 1);
//}

///**
// * Returns true if the next value is available.
// * Note: The implementation is undefined for iterator position outside the buffer.
// */
//FUNC_ATTRS bool __rxSnapshotBufferHasNext(volatile RxSnapshotBuffer *o) {
//    return o->iteratorIndex != (o->endIndex - 2);
//}

///**
// * releases the buffer
// */
//FUNC_ATTRS void __rxSnapshotBufferClear(volatile RxSnapshotBuffer *o) {
//    o->startIndex = o->endIndex;
//}

/**
 * Returns true if the buffer is empty.
 */
FUNC_ATTRS bool __rxSnapshotBufferIsEmpty(volatile RxSnapshotBuffer *o) {
    return o->startIndex == o->endIndex;
}

/**
 * Returns the positive difference of two uint16_t values.
 */
FUNC_ATTRS void __absDifference(volatile uint16_t *a, uint16_t *b, uint16_t *result) {
    if (*a > *b) {
        *result = *a - *b;
    } else {
        *result = *b - *a;
    }
}

/**
 * appends the current TIMER_TX_RX_COUNTER value and the specified flank to the snapshot buffer
 */
FUNC_ATTRS void captureSnapshot(const bool isRisingEdge,
                                volatile RxSnapshotBuffer *snapshotBuffer) {
    volatile Snapshot *snapshot = &(snapshotBuffer->snapshots[snapshotBuffer->endIndex]);
    snapshot->timerValue = TIMER_TX_RX_COUNTER >> 1;
    snapshot->isRisingEdge = isRisingEdge;
    ++snapshotBuffer->endIndex;
}

/**
 * decodes the specified buffer's snapshots to bits and bytes
 */
// TODO: implementation pending
FUNC_ATTRS void manchesterDecodeBuffer(volatile RxPort *rxPort) {

    if (rxPort->isDataBuffered == true) {
        return;
    }

    switch (rxPort->snapshotsBuffer.decoderStates.decodingState) {

        case DECODER_STATE_TYPE_START:
            if (__rxSnapshotBufferIsEmpty(&rxPort->snapshotsBuffer) == false) {
                volatile Snapshot *snapshot = __rxSnapshotBufferPeek(&rxPort->snapshotsBuffer);
                if (snapshot->isRisingEdge == false) {
                    bufferBitPointerStart(&rxPort->buffer.pointer);
                    rxPort->isDataBuffered = false;
                    __resetDecoderPhaseState(rxPort->snapshotsBuffer.decoderStates.phaseState);
                    rxPort->snapshotsBuffer.temporaryDequeueRegister = snapshot->timerValue;
                    __rxSnapshotBufferDequeue(&rxPort->snapshotsBuffer);
                    rxPort->snapshotsBuffer.decoderStates.decodingState = DECODER_STATE_TYPE_DECODING;
                }
            }
            break;

        case DECODER_STATE_TYPE_DECODING:
            while (__rxSnapshotBufferIsEmpty(&rxPort->snapshotsBuffer) == false) {
                volatile Snapshot *snapshot = __rxSnapshotBufferPeek(&rxPort->snapshotsBuffer);
                uint16_t timerValue = snapshot->timerValue << 1;
                uint16_t difference;
                __absDifference(&rxPort->snapshotsBuffer.temporaryDequeueRegister,
                                (uint16_t *) &timerValue, &difference);
                if (difference <=
                    ParticleAttributes.ports.rx.timerAdjustment.maxShortIntervalDuration) { // on short interval
                    __phaseStateAdvanceShortInterval(rxPort->snapshotsBuffer.decoderStates.phaseState);
                } else if (difference <=
                           ParticleAttributes.ports.rx.timerAdjustment.maxLongIntervalDuration) { // on long interval
                    __phaseStateAdvanceLongInterval(rxPort->snapshotsBuffer.decoderStates.phaseState);
                } else { // on timeout
                    rxPort->isDataBuffered = true;
                    __rxSnapshotBufferDequeue(&rxPort->snapshotsBuffer);
                    rxPort->snapshotsBuffer.decoderStates.decodingState = DECODER_STATE_TYPE_START;
                }

                if (__isDataPhase(rxPort->snapshotsBuffer.decoderStates.phaseState)) {
                    __storeDataBit(rxPort, snapshot->isRisingEdge);
                }

                rxPort->snapshotsBuffer.temporaryDequeueRegister = snapshot->timerValue;
                __rxSnapshotBufferDequeue(&rxPort->snapshotsBuffer);
            }
            break;

        default:
            break;

    }
}

#ifdef FUNC_ATTRS
#  undef FUNC_ATTRS
#endif