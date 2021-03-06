/**
 * @author Raoul Rubien  13.07.2016
 *
 * Particle core related implementation that is ought to be passed as pointer or callback.
 */

#pragma once

#include "uc-core/communication/ManchesterDecoding.h"
#include "uc-core/communication-protocol/Interpreter.h"

/**
 * North port reception implementation.
 */
void receiveNorth(void) {
    if (ParticleAttributes.discoveryPulseCounters.north.isConnected) {
        manchesterDecodeBuffer(&ParticleAttributes.directionOrientedPorts.north,
                               interpretRxBuffer);
    }
}

/**
 * East port reception implementation.
 */
void receiveEast(void) {
    if (ParticleAttributes.discoveryPulseCounters.east.isConnected) {
        manchesterDecodeBuffer(&ParticleAttributes.directionOrientedPorts.east,
                               interpretRxBuffer);
    }
}

/**
 * South port reception implementation.
 */
void receiveSouth(void) {
    if (ParticleAttributes.discoveryPulseCounters.south.isConnected) {
        manchesterDecodeBuffer(&ParticleAttributes.directionOrientedPorts.south,
                               interpretRxBuffer);
    }
}
