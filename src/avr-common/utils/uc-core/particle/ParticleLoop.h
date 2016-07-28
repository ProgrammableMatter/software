/**
 * @author Raoul Rubien 2016
 *
 * The main loop implementation.
 */

#pragma once

#include "ParticleCore.h"

/**
 * The main particle loop. It repetitively calls the state driven particle core implementation.
 */
extern inline void particleLoop(void);

inline void particleLoop(void) {
    IO_PORTS_SETUP; // configure input/output pins
    constructParticle(&ParticleAttributes);
    ParticleAttributes.node.state = STATE_TYPE_START;
    forever {
        particleTick();
    }
}
