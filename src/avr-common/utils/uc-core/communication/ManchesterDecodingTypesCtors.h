/**
 * @author Raoul Rubien 2016
 *
 * Manchester types constructor implementation.
 */

#pragma once

#include "ManchesterDecodingTypes.h"

/**
 * constructor function
 * @param o reference to the object to construct
 */
void constructSnapshot(Snapshot *const o) {
    o->isRisingEdge = false;
    o->timerValue = 0;
}

/**
 * constructor function
 * @param o reference to the object to construct
 */
void constructManchesterDecoderState(ManchesterDecoderStates *const o) {
    o->decodingState = DECODER_STATE_TYPE_START;
    o->phaseState = 0;
}

/**
 * constructor function
 * @param o reference to the object to construct
 */
void constructRxSnapshotBuffer(RxSnapshotBuffer *const o) {
    constructManchesterDecoderState(&o->decoderStates);
    o->startIndex = 0;
    o->endIndex = 0;
    o->temporarySnapshotTimerValue = 0;
    o->isOverflowed = false;
    o->numberHalfCyclesPassed = 0;
}
