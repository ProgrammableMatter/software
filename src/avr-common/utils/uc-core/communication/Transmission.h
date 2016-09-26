/**
 * @author Raoul Rubien 05.2016
 *
 * Transmission related implementation.
 */

#pragma once

#include "common/common.h"
#include "uc-core/configuration/interrupts/TxRxTimer.h"
#include "simulation/SimulationMacros.h"

/**
 * Schedules the next transmission interrupt.
 */
void scheduleNextTxInterrupt(void) {
    TIMER_TX_RX_COMPARE_VALUE += ParticleAttributes.communication.timerAdjustment.transmissionClockDelayHalf
                                 + ParticleAttributes.communication.timerAdjustment.transmissionClockShift;
}

/**
 * Schedules the next transmission start interrupt.
 */
void scheduleStartTxInterrupt(void) {
    uint16_t counter = TIMER_TX_RX_COUNTER_VALUE;
    TIMER_TX_RX_COMPARE_VALUE = counter -
                                (counter %
                                 ParticleAttributes.communication.timerAdjustment.transmissionClockDelay)
                                + 2 * ParticleAttributes.communication.timerAdjustment.transmissionClockDelay
                                + ParticleAttributes.communication.timerAdjustment.transmissionClockShift;
    MEMORY_BARRIER;
    TIMER_TX_RX_ENABLE_COMPARE_INTERRUPT;
}

/**
 * Activates the designated port to contribute to the next transmission until
 * there is no more data to transmit.
 * @param port the designated transmission port to read the buffer and transmit from
 */
void enableTransmission(TxPort *const port) {
    DEBUG_CHAR_OUT('t');
    port->isTxClockPhase = true;
    port->isTransmitting = true;
    port->isDataBuffered = true;
    scheduleStartTxInterrupt();
}
