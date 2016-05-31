//
// Created by Raoul Rubien on 25.05.16.
//

#ifndef __PROJECT_INTERPRETER_H__
#define __PROJECT_INTERPRETER_H__

#include "InterpreterTypes.h"
#include "CommunicationProtocolTypes.h"

#  ifdef TRY_INLINE
#    define FUNC_ATTRS inline
#  else
#    define FUNC_ATTRS
#  endif

/**
 * returns true if the buffer keeps reasonable number of bits, else false and
 * releases the buffer
 */
FUNC_ATTRS uint8_t __isReasonableBufferSizeOrClearBuffer(volatile RxPort *o) {
    // TODO: replace hardcoded values by macros
    if (o->isReceiving == false) {
        if (o->isDataBuffered) {
            if (o->buffer.pointer.bitMask == (1 << 2)) { // packages with bit mask 0x04
                // 2, 3, 4, 6, 7
                // !1, !5, !8
                if (o->buffer.pointer.byteNumber != 1 && o->buffer.pointer.byteNumber != 5 &&
                    o->buffer.pointer.byteNumber != 8) {
                    return true;
                }
            } else if (o->buffer.pointer.bitMask == (1 << 6)) { // packages with bit mask 0x40
                // 1, 3, 5
                if (o->buffer.pointer.byteNumber == 1 || o->buffer.pointer.byteNumber == 3 ||
                    o->buffer.pointer.byteNumber == 5) {
                    return true;
                }
            } else if (o->buffer.pointer.bitMask == (1 << 7)) { // packages with bit mask 0x80
                // 0, 2, 4
                if (o->buffer.pointer.byteNumber == 0 || o->buffer.pointer.byteNumber == 2 ||
                    o->buffer.pointer.byteNumber == 4) {
                    return true;
                }
            }
            // TODO: possible race condition: testing & clearing flags vs. reception
            // release buffer
            o->isOverflowed = false;
            o->isDataBuffered = false;
            IF_SIMULATION_CHAR_OUT('p');
        }
    }
    IF_SIMULATION_CHAR_OUT('P');
    return false;
}

/**
 * interpret the interpreter buffer according to the particle state as context
 */
FUNC_ATTRS void __interpretBufferContextSensitive(volatile RxPort *rxPort, volatile Package *package) {

    switch (ParticleAttributes.node.state) { // switch according to node state context

        // state: wait for being enumerated
        case STATE_TYPE_WAIT_FOR_BEING_ENUMERATED:
            // expect enumeration package with new address to assign locally
            switch (package->asHeader.headerId) {
                case PACKAGE_HEADER_ID_TYPE_ENUMERATE:
                    ParticleAttributes.node.address.row = package->asDedicatedHeader.addressRow0;
                    ParticleAttributes.node.address.column = package->asDedicatedHeader.addressColumn0;
                    ParticleAttributes.node.state = STATE_TYPE_WAIT_FOR_BEING_ENUMERATED_SEND_ACK_RESPONSE_TO_PARENT;
                    clearReceptionBuffer(rxPort);
                    break;
                default:
                    // otherwise remain in same state
                    clearReceptionBuffer(rxPort);
                    break;
            }
            break;

            // state: wait for ack response from parent to finish enumeration
        case STATE_TYPE_WAIT_FOR_BEING_ENUMERATED_ACK_RESPONSE_FROM_PARENT:
            // expect ack package from parent, otherwise switch back to waiting for enumeration state
            switch (package->asHeader.headerId) {
                case PACKAGE_HEADER_ID_TYPE_ACK:
                    clearReceptionBuffer(rxPort);
                    // data ok, switch to next state
                    ParticleAttributes.node.state = STATE_TYPE_LOCALLY_ENUMERATED;
                    break;
                case PACKAGE_HEADER_ID_TYPE_ENUMERATE:
                    // on address reassignment, do not clear buffer but switch state
                    ParticleAttributes.node.state = STATE_TYPE_WAIT_FOR_BEING_ENUMERATED;
                    break;
                default:
                    // on any other package, clear buffer and switch state
                    clearReceptionBuffer(rxPort);
                    ParticleAttributes.node.state = STATE_TYPE_WAIT_FOR_BEING_ENUMERATED;
                    break;
            }
            break;

            // state: wait for ack response with 2 byte data describing neighbour's address
        case STATE_TYPE_ENUMERATING_EAST_WAIT_UNTIL_ACK_RESPONSE_FROM_EAST:
            // if data is ok, then switch to next state, otherwise re-start enumeration
            switch (package->asHeader.headerId) {
                case PACKAGE_HEADER_ID_TYPE_ACK_WITH_DATA:
                    if (package->asACKData19.dataLsb == ParticleAttributes.node.address.row &&
                        package->asACKData19.dataCeb == (ParticleAttributes.node.address.column + 1)) {
                        // data ok, switch to next state
                        ParticleAttributes.node.state = STATE_TYPE_ENUMERATING_EAST_SEND_ACK_RESPONSE_TO_EAST;
                    }
                    clearReceptionBuffer(rxPort);
                    break;
                default:
                    // otherwise re-start the enumeration
                    clearReceptionBuffer(rxPort);
                    ParticleAttributes.node.state = STATE_TYPE_ENUMERATING_EAST_NEIGHBOUR;
                    break;
            }
            break;
        default:
            break;
    }
}

/**
 * Read, parse and interpret the specified buffer.
 */
FUNC_ATTRS void __interpretBuffer(volatile RxPort *rxPort) {
    // TODO: remove check for received size to the appropriate state, because then
    // the size can be checked context sensitive
    if (!__isReasonableBufferSizeOrClearBuffer(rxPort)) {
        return;
    }

    Package *package = (Package *) rxPort->buffer.bytes;
    __interpretBufferContextSensitive(rxPort, package);
}

///**
// * interpret reception buffers of all connected ports
// */
//FUNC_ATTRS void interpretRxBuffers(void) {
//    if (ParticleAttributes.discoveryPulseCounters.north.isConnected) {
//        __interpretBuffer(&ParticleAttributes.ports.rx.north);
//    }
//    if (ParticleAttributes.discoveryPulseCounters.east.isConnected) {
//        __interpretBuffer(&ParticleAttributes.ports.rx.east);
//    }
//    if (ParticleAttributes.discoveryPulseCounters.south.isConnected) {
//        __interpretBuffer(&ParticleAttributes.ports.rx.south);
//    }
//}


FUNC_ATTRS void interpretNorthRxBuffer(void) {
//    if (ParticleAttributes.discoveryPulseCounters.north.isConnected) {
    __interpretBuffer(&ParticleAttributes.ports.rx.north);
//    }
}

FUNC_ATTRS void interpretEastRxBuffer(void) {
//    if (ParticleAttributes.discoveryPulseCounters.east.isConnected) {
        __interpretBuffer(&ParticleAttributes.ports.rx.east);
//    }
}

FUNC_ATTRS void interpretSouthRxBuffer(void) {
//    if (ParticleAttributes.discoveryPulseCounters.south.isConnected) {
        __interpretBuffer(&ParticleAttributes.ports.rx.south);
//    }
}

#  ifdef FUNC_ATTRS
#    undef FUNC_ATTRS
#  endif
#endif
