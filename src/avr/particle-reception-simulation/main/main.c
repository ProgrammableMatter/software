/**
 * @author Raoul Rubien 2016
 */
#include <avr/interrupt.h>
#include <common/common.h>
#include "uc-core/IoDefinitions.h"
#include "uc-core/InterruptDefinitions.h"
#include "uc-core/Interrupts.c"
#include "uc-core/Particle.h"


/**
 * A mocked up particle loop. It puts the particle in an initialized reception state.
 */
int particleLoop(void) {
    SREG setBit bit(SREG_I);
    forever {
        particleTick();
        if (ParticleAttributes.node.state == STATE_TYPE_ERRONEOUS) {
            return 1;
        }
    }
}


int main(void) {
    IO_PORTS_SETUP; // configure input/output pins
    // setup and enable reception (on pin change) interrupts
    RX_INTERRUPTS_SETUP;
    RX_INTERRUPTS_ENABLE;
    // setup and enable reception counter interrupt
    TIMER_TX_RX_SETUP;
    TIMER_TX_RX_ENABLE;

    constructParticleState(&ParticleAttributes);
    ParticleAttributes.node.state = STATE_TYPE_IDLE;
    ParticleAttributes.discoveryPulseCounters.loopCount = UINT8_MAX;

    return particleLoop();
}
