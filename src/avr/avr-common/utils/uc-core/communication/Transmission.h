/**
 * @author Raoul Rubien 05.2016
 *
 * Transmission related implementation.
 */

#pragma once

#include "uc-core/configuration/interrupts/TxRxTimer.h"
#include "simulation/SimulationMacros.h"

/**
 * Schedules the next transmission interrupt.
 */
extern FUNC_ATTRS void scheduleNextTxInterrupt(void);

FUNC_ATTRS void scheduleNextTxInterrupt(void) {
    TIMER_TX_RX_COMPARE_VALUE += ParticleAttributes.communication.timerAdjustment.transmissionClockDelayHalf
                                 + ParticleAttributes.communication.timerAdjustment.transmissionClockShift;
}

/**
 * Schedules the next transmission start interrupt.
 */
extern FUNC_ATTRS void scheduleStartTxInterrupt(void);

FUNC_ATTRS void scheduleStartTxInterrupt(void) {
    uint16_t counter = TIMER_TX_RX_COUNTER_VALUE;
    TIMER_TX_RX_COMPARE_VALUE = counter -
                                (counter %
                                 ParticleAttributes.communication.timerAdjustment.transmissionClockDelay)
                                + 2 * ParticleAttributes.communication.timerAdjustment.transmissionClockDelay
                                + ParticleAttributes.communication.timerAdjustment.transmissionClockShift;
    TIMER_TX_RX_ENABLE_COMPARE_INTERRUPT;
}

/**
 * Activates the designated port to contribute to the next transmission until
 * there is no more data to transmit.
 * @param port the designated transmissoin port to read the buffer and transmit from
 */
extern FUNC_ATTRS void enableTransmission(volatile TxPort *port);

FUNC_ATTRS void enableTransmission(volatile TxPort *port) {
    port->isTxClockPhase = true;
    port->isTransmitting = true;
    port->isDataBuffered = true;
    scheduleStartTxInterrupt();
}
