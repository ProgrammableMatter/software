/**
 * @author Raoul Rubien 2016
 */

#pragma once

#include "Communication.h"
#include "ManchesterDecodingTypes.h"
#include "uc-core/interrupts/TimerCounter.h"

extern FUNC_ATTRS void constructSnapshot(volatile Snapshot *o);

FUNC_ATTRS void constructSnapshot(volatile Snapshot *o) {
    o->isRisingEdge = false;
    o->timerValue = 0;
}

extern FUNC_ATTRS void constructManchesterDecoderState(volatile ManchesterDecoderStates *o);

FUNC_ATTRS void constructManchesterDecoderState(volatile ManchesterDecoderStates *o) {
    o->decodingState = DECODER_STATE_TYPE_START;
    o->phaseState = 0;
}

extern FUNC_ATTRS void constructRxSnapshotBuffer(volatile RxSnapshotBuffer *o);

FUNC_ATTRS void constructRxSnapshotBuffer(volatile RxSnapshotBuffer *o) {
//    for (uint8_t idx = 0; idx < (sizeof(o->snapshots) / sizeof(Snapshot)); idx++) {
//        constructSnapshot(&o->snapshots[idx]);
//    }
    constructManchesterDecoderState(&o->decoderStates);
    o->startIndex = 0;
    o->endIndex = 0;
    o->temporaryDequeueRegister = 0;
}

///**
// * Translates the absolute captured hardware counter to a relative port specific counter according to the provided TimerCounterAdjustment.
// */
//FUNC_ATTRS uint16_t __toRelativeSnapshotValue(const volatile uint16_t *hardwareCounter,
//                                              const volatile uint16_t *receptionOffset) {
//    uint16_t portCounter = *hardwareCounter + *receptionOffset;
//    if (portCounter >= TIMER_TX_RX_COMPARE_TOP_VALUE) {
//        return portCounter - TIMER_TX_RX_COMPARE_TOP_VALUE;
//    } else {
//        return portCounter;
//    }
//}

///**
// * In case the counter is greater than the compare value (i.e. compare interrupt was shifted) returns
// * the amount counted over else the timer.
// */
//FUNC_ATTRS uint16_t __getTrimmedReceptionCounter(void) {
//
////#ifdef SIMULATION
////    if (TIMER_TX_RX_COUNTER > (2 * TIMER_TX_RX_COMPARE_TOP_VALUE)) {
////        DEBUG_CHAR_OUT('R');
////        IF_DEBUG_SWITCH_TO_ERRONEOUS_STATE;
////    }
////#endif
//
//    if (TIMER_TX_RX_COUNTER > TIMER_TX_RX_COMPARE_TOP_VALUE) {
//        return TIMER_TX_RX_COUNTER - TIMER_TX_RX_COMPARE_TOP_VALUE;
//    } else {
//        return TIMER_TX_RX_COUNTER;
//    }
//}
