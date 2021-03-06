/**
 * @author Raoul Rubien 2016
 *
 * Particle core implementation.
 */

#pragma once

#include <avr/sleep.h>
#include "common/common.h"
#include "Globals.h"
#include "uc-core/particle/types/ParticleTypes.h"
#include "uc-core/particle/types/ParticleTypesCtors.h"
#include "uc-core/configuration/IoPins.h"
#include "uc-core/delay/delay.h"
#include "uc-core/discovery/Discovery.h"
#include "uc-core/configuration/Particle.h"
#include "uc-core/configuration/Periphery.h"
#include "uc-core/configuration/interrupts/ReceptionPCI.h"
#include "uc-core/configuration/interrupts/TxRxTimer.h"
#include "uc-core/configuration/interrupts/DiscoveryPCI.h"
#include "uc-core/configuration/interrupts/DiscoveryTimer.h"
#include "uc-core/configuration/interrupts/ActuationTimer.h"
#include "uc-core/interrupts/Interrupts.h"
#include "uc-core/communication-protocol/CommunicationProtocol.h"
#include "uc-core/communication-protocol/CommunicationProtocolTypesCtors.h"
#include "uc-core/communication-protocol/CommunicationProtocolPackageTypesCtors.h"
#include "uc-core/actuation/Actuation.h"
#include "Commands.h"
#include "uc-core/periphery/Periphery.h"
#include "uc-core/stdout/Stdout.h"
#include "uc-core/time/Time.h"
#include "uc-core/scheduler/Scheduler.h"
#include "uc-core/scheduler/SchedulerTypesCtors.h"
#include "uc-core/configuration/Evaluation.h"
#include "uc-core/evaluation/Evaluation.h"

/**
 * Disables discovery sensing interrupts.
 */
static void __disableDiscoverySensing(void) {
    DISCOVERY_INTERRUPTS_DISABLE;
}

/**
 * Enables discovery sensing interrupts.
 */
static void __enableDiscoverySensing(void) {
//    DISCOVERY_INTERRUPTS_SETUP;
//    MEMORY_BARRIER;
    DISCOVERY_INTERRUPTS_ENABLE;
}

/**
 * Disables discovery pulsing.
 */
//extern FUNC_ATTRS void __disableDiscoveryPulsing(void);

static void __disableDiscoveryPulsing(void) {
    TIMER_NEIGHBOUR_SENSE_PAUSE;
    TIMER_NEIGHBOUR_SENSE_DISABLE;
    NORTH_TX_LO;
    EAST_TX_HI;
    SOUTH_TX_LO;
}

/**
 * Enables discovery sensing.
 */
static void __enableDiscoveryPulsing(void) {
    TIMER_NEIGHBOUR_SENSE_ENABLE;
    MEMORY_BARRIER;
    TIMER_NEIGHBOUR_SENSE_RESUME;
}

static void __enableAlerts(SchedulerTask *const task) {
    ParticleAttributes.alerts.isRxBufferOverflowEnabled = true;
    ParticleAttributes.alerts.isRxParityErrorEnabled = true;
    ParticleAttributes.alerts.isGenericErrorEnabled = true;
    // to remove compiler warning, clearing this flag is redundant
    task->isEnabled = false;
}

/**
 * Sets up ports and interrupts but does not enable the global interrupt (I-flag in SREG).
 */
static void __initParticle(void) {
    // ---------------- basic setup ----------------
    setupUart();
    LED_STATUS1_OFF;
    LED_STATUS2_OFF;
    LED_STATUS3_OFF;
    LED_STATUS4_OFF;

    TEST_POINT1_LO;
    TEST_POINT2_LO;

    DELAY_MS_1;
    DELAY_MS_1;

    // ---------------- ISR setup ----------------
    // RX_INTERRUPTS_SETUP; // configure input pins interrupts
    // RX_INTERRUPTS_ENABLE; // enable input pin interrupts
    DISCOVERY_INTERRUPTS_SETUP; // configure pin change interrupt
    TIMER_NEIGHBOUR_SENSE_SETUP; // configure timer interrupt for neighbour sensing
    LOCAL_TIME_INTERRUPT_COMPARE_DISABLE; // prepare local time timer interrupt
    ACTUATOR_INTERRUPT_SETUP; // prepare actuation interrupt
    sleep_disable();
    set_sleep_mode(SLEEP_MODE_PWR_DOWN); // set sleep mode to power down

    // ---------------- add tasks to scheduler ----------------
    // task for clearing leds before the basic process starts
#ifndef PERIPHERY_REMOVE_IMPL
    addSingleShotTask(SCHEDULER_TASK_ID_SETUP_LEDS, setupLedsState, 128);
#endif
    addSingleShotTask(SCHEDULER_TASK_ID_ENABLE_ALERTS, __enableAlerts, 255);

    // add toggle led task: for heartbeat indication
    // addCyclicTask(SCHEDULER_TASK_ID_HEARTBEAT_LED_TOGGLE, heartBeatToggle, 300, 400);

#ifdef EVALUATION_SIMPLE_SYNC_AND_ACTUATION
    // add cyclic but count limited sync. time task
    addCyclicTask(SCHEDULER_TASK_ID_SYNC_PACKAGE,
                  sendNextSyncTimePackageTask,
                  SYNCHRONIZATION_TYPES_CTORS_FIRST_SYNC_PACKAGE_LOCAL_TIME,
                  ParticleAttributes.timeSynchronization.fastSyncPackageSeparation);
    taskEnableNodeTypeLimit(SCHEDULER_TASK_ID_SYNC_PACKAGE, NODE_TYPE_ORIGIN);
    taskEnableCountLimit(SCHEDULER_TASK_ID_SYNC_PACKAGE,
                         ParticleAttributes.timeSynchronization.totalFastSyncPackagesToTransmit);

    // prepare but disable task, is enabled by last call of sync. task
    addCyclicTask(SCHEDULER_TASK_ID_HEAT_WIRES, heatWiresTask,
                  SYNCHRONIZATION_TYPES_CTORS_FIRST_SYNC_PACKAGE_LOCAL_TIME, 1500);
    taskEnableNodeTypeLimit(SCHEDULER_TASK_ID_HEAT_WIRES, NODE_TYPE_ORIGIN);
    taskDisable(SCHEDULER_TASK_ID_HEAT_WIRES);
#endif
#ifdef EVALUATION_SYNC_CYCLICALLY
    addCyclicTask(SCHEDULER_TASK_ID_SYNC_PACKAGE, sendNextSyncTimePackageTask, 350, 100);
    taskEnableNodeTypeLimit(SCHEDULER_TASK_ID_SYNC_PACKAGE, NODE_TYPE_ORIGIN);
#endif
#ifdef EVALUATION_SYNC_WITH_CYCLIC_UPDATE_TIME_REQUEST_FLAG
    addCyclicTask(SCHEDULER_TASK_ID_SYNC_PACKAGE, sendSyncTimePackageAndUpdateRequestFlagTask, 350,
                  ParticleAttributes.timeSynchronization.fastSyncPackageSeparation);
    taskEnableNodeTypeLimit(SCHEDULER_TASK_ID_SYNC_PACKAGE, NODE_TYPE_ORIGIN);
    taskEnableCountLimit(SCHEDULER_TASK_ID_SYNC_PACKAGE, 5);
#endif
#ifdef EVALUATION_SYNC_WITH_CYCLIC_UPDATE_TIME_REQUEST_FLAG_IN_PHASE_SHIFTING
    addCyclicTask(SCHEDULER_TASK_ID_SYNC_PACKAGE,
                  sendSyncTimePackageAndUpdateRequestFlagForInPhaseShiftingEvaluationTask, 350,
                  ParticleAttributes.timeSynchronization.fastSyncPackageSeparation);
    taskEnableNodeTypeLimit(SCHEDULER_TASK_ID_SYNC_PACKAGE, NODE_TYPE_ORIGIN);
    taskEnableCountLimit(SCHEDULER_TASK_ID_SYNC_PACKAGE, 30);
#else

#ifdef EVALUATION_SYNC_WITH_CYCLIC_UPDATE_TIME_REQUEST_FLAG_THEN_ACTUATE_ONCE
    addCyclicTask(SCHEDULER_TASK_ID_SYNC_PACKAGE,
                  sendSyncTimeAndActuateOnceTask,
                  SYNCHRONIZATION_TYPES_CTORS_FIRST_SYNC_PACKAGE_LOCAL_TIME,
                  ParticleAttributes.timeSynchronization.fastSyncPackageSeparation);
    taskEnableNodeTypeLimit(SCHEDULER_TASK_ID_SYNC_PACKAGE, NODE_TYPE_ORIGIN);
    taskEnableCountLimit(SCHEDULER_TASK_ID_SYNC_PACKAGE,
                         ParticleAttributes.timeSynchronization.totalFastSyncPackagesToTransmit);
#endif

#endif
}

/**
 * Disables all reception interrupts.
 */
static void __disableReceptionPinChangeInterrupts(void) {
    DEBUG_CHAR_OUT('O');
    RX_NORTH_INTERRUPT_DISABLE;
    DEBUG_CHAR_OUT('Q');
    RX_EAST_INTERRUPT_DISABLE;
    DEBUG_CHAR_OUT('N');
    RX_SOUTH_INTERRUPT_DISABLE;
}

/**
 * Clears pending and enables reception interrupts for north port if connected.
 */
static void __enableReceptionHardwareNorth(void) {
    if (ParticleAttributes.discoveryPulseCounters.north.isConnected) {
        DEBUG_CHAR_OUT('o');
        RX_NORTH_INTERRUPT_CLEAR_PENDING;
        MEMORY_BARRIER;
        RX_NORTH_INTERRUPT_ENABLE;
    }
}

/**
 * Clears pending and enables reception interrupts for east port if connected.
 */
static void __enableReceptionHardwareEast(void) {
    if (ParticleAttributes.discoveryPulseCounters.east.isConnected) {
        DEBUG_CHAR_OUT('q');
        RX_EAST_INTERRUPT_CLEAR_PENDING;
        MEMORY_BARRIER;
        RX_EAST_INTERRUPT_ENABLE;
    }
}

/**
 * Clears pending and enables reception interrupts for south port if connected.
 */
static void __enableReceptionHardwareSouth(void) {
    if (ParticleAttributes.discoveryPulseCounters.south.isConnected) {
        DEBUG_CHAR_OUT('m');
        RX_SOUTH_INTERRUPT_CLEAR_PENDING;
        MEMORY_BARRIER;
        RX_SOUTH_INTERRUPT_ENABLE;
    }
}

/**
 * Sets up and enables the tx/rx timer.
 */
static void __enableTxRxTimer(void) {
    TIMER_TX_RX_COUNTER_SETUP;
    MEMORY_BARRIER;
    TIMER_TX_RX_COUNTER_ENABLE;
}

/**
 * Disables the signal generating transmission interrupt.
 */
//static  void __disableTransmission(void) {
//    TIMER_TX_RX_DISABLE_COMPARE_INTERRUPT;
//}

/**
 * Sets up and enables reception for all connected ports.
 */
static void __enableReceptionHardwareForConnectedPorts(void) {
//    RX_INTERRUPTS_SETUP;
//    MEMORY_BARRIER;
//    RX_INTERRUPTS_ENABLE;
    __enableReceptionHardwareNorth();
    __enableReceptionHardwareEast();
    __enableReceptionHardwareSouth();
}

/**
 * Disables reception interrupts.
 */

void __disableReception(void) {
    __disableReceptionPinChangeInterrupts();
}

/**
 * Enables reception timer and interrupts.
 */

void __enableReception(void) {
    __enableTxRxTimer();
    __enableReceptionHardwareForConnectedPorts();
}

/**
 * Increments the discovery loop counter.
 */
static void __discoveryLoopCount(void) {
    if (ParticleAttributes.discoveryPulseCounters.loopCount < (UINT8_MAX)) {
        ParticleAttributes.discoveryPulseCounters.loopCount++;
    }
}

/**
 * Sets the correct address if this node is the origin node.
 */
static void __updateOriginNodeAddress(void) {
    if (ParticleAttributes.node.type == NODE_TYPE_ORIGIN) {
        ParticleAttributes.node.address.row = 1;
        ParticleAttributes.node.address.column = 1;
    }
}

/**
 * Handles (state driven) the wait for being enumerated node state.
 */
static void __handleWaitForBeingEnumerated(void) {
    CommunicationProtocolPortState *commPortState = &ParticleAttributes.protocol.ports.north;
    if (commPortState->stateTimeoutCounter == 0 &&
        commPortState->receptionistState != COMMUNICATION_RECEPTIONIST_STATE_TYPE_TRANSMIT_ACK &&
        commPortState->receptionistState !=
        COMMUNICATION_RECEPTIONIST_STATE_TYPE_TRANSMIT_ACK_WAIT_TX_FINISHED) {
        // on timeout: fall back to start state
        DEBUG_CHAR_OUT('r');
        commPortState->receptionistState = COMMUNICATION_RECEPTIONIST_STATE_TYPE_RECEIVE;
        commPortState->stateTimeoutCounter = COMMUNICATION_PROTOCOL_TIMEOUT_COUNTER_MAX;
        if (commPortState->reTransmissions > 0) {
            commPortState->reTransmissions--;
        }
        DEBUG_CHAR_OUT('b');
    }

    if (commPortState->reTransmissions == 0) {
        // on retransmissions consumed: sort cut the global state machine to erroneous state
        ParticleAttributes.node.state = STATE_TYPE_ERRONEOUS;
        return;
    }

    switch (commPortState->receptionistState) {
        // receive
        case COMMUNICATION_RECEPTIONIST_STATE_TYPE_RECEIVE:
            ParticleAttributes.directionOrientedPorts.north.receivePimpl();
            break;
            // transmit ack with local address
        case COMMUNICATION_RECEPTIONIST_STATE_TYPE_TRANSMIT_ACK:
            constructEnumerationACKWithAddressToParentPackage();
            enableTransmission(&ParticleAttributes.communication.ports.tx.north);
            DEBUG_CHAR_OUT('g');
            commPortState->receptionistState = COMMUNICATION_RECEPTIONIST_STATE_TYPE_TRANSMIT_ACK_WAIT_TX_FINISHED;
            break;
            // wait for tx finished
        case COMMUNICATION_RECEPTIONIST_STATE_TYPE_TRANSMIT_ACK_WAIT_TX_FINISHED:
            if (ParticleAttributes.communication.ports.tx.north.isTransmitting) {
                break;
            }
            clearReceptionPortBuffer(&ParticleAttributes.communication.ports.rx.north);
            DEBUG_CHAR_OUT('h');
            commPortState->receptionistState = COMMUNICATION_RECEPTIONIST_STATE_TYPE_WAIT_FOR_RESPONSE;
            break;
            // wait for response
        case COMMUNICATION_RECEPTIONIST_STATE_TYPE_WAIT_FOR_RESPONSE:
            ParticleAttributes.directionOrientedPorts.north.receivePimpl();
            break;

        case COMMUNICATION_RECEPTIONIST_STATE_TYPE_IDLE:
            break;
    }
}

/**
 * Handles (state driven) neighbour synchronization node states.
 * @param endState the state when handler has finished
 */
static void __handleSynchronizeNeighbour(const StateType endState) {
    CommunicationProtocolPortState *commPortState = ParticleAttributes.directionOrientedPorts.simultaneous.protocol;
    TxPort *txPort = ParticleAttributes.directionOrientedPorts.simultaneous.txPort;
    switch (commPortState->initiatorState) {
        // transmit local time simultaneously on east and south ports
        case COMMUNICATION_INITIATOR_STATE_TYPE_TRANSMIT:
            constructSyncTimePackage(txPort,
                                     ParticleAttributes.timeSynchronization.isNextSyncPackageTimeUpdateRequest);
            enableTransmission(txPort);
            commPortState->initiatorState = COMMUNICATION_INITIATOR_STATE_TYPE_TRANSMIT_WAIT_FOR_TX_FINISHED;
            break;

            // wait for tx finished
        case COMMUNICATION_INITIATOR_STATE_TYPE_TRANSMIT_WAIT_FOR_TX_FINISHED:
            if (txPort->isTransmitting) {
                break;
            }
            commPortState->initiatorState = COMMUNICATION_INITIATOR_STATE_TYPE_IDLE;
            goto __COMMUNICATION_INITIATOR_STATE_TYPE_IDLE;
            break;

        case COMMUNICATION_INITIATOR_STATE_TYPE_WAIT_FOR_RESPONSE:
        case COMMUNICATION_INITIATOR_STATE_TYPE_TRANSMIT_ACK:
        case COMMUNICATION_INITIATOR_STATE_TYPE_TRANSMIT_ACK_WAIT_FOR_TX_FINISHED:
        __COMMUNICATION_INITIATOR_STATE_TYPE_IDLE:
        case COMMUNICATION_INITIATOR_STATE_TYPE_IDLE:
            ParticleAttributes.node.state = endState;
            break;
    }
}

/**
 * Handles the sync neighbour done state.
 * In general it simply switches to the specified state,
 * but if simulation/test macros are defined it redirects to other states accordingly.
 * @param endState the state after this call if no test macros defined
 */
static void __handleSynchronizeNeighbourDoneOrRunTest(const StateType endState) {
#if defined(SIMULATION_SET_NEW_NETWORK_GEOMETRY_TEST)
    if (ParticleAttributes.node.type == NODE_TYPE_ORIGIN) {
        ParticleAttributes.protocol.networkGeometry.rows = 2;
        ParticleAttributes.protocol.networkGeometry.columns = 1;
        DELAY_MS_1;
        setNewNetworkGeometry();
        return;
    }
#elif defined(SIMULATION_HEAT_WIRES_TEST)
    if (ParticleAttributes.node.type == NODE_TYPE_ORIGIN) {
        Actuators actuators;
        actuators.northLeft = true;
        actuators.northRight = true;
        NodeAddress nodeAddress;
        nodeAddress.row = 2;
        nodeAddress.column = 2;
        DELAY_MS_1;
        sendHeatWires(&nodeAddress, &actuators, 5, 2);
        return;
    }
#elif defined(SIMULATION_HEAT_WIRES_RANGE_TEST)
    if (ParticleAttributes.node.type == NODE_TYPE_ORIGIN) {
        Actuators actuators;
        actuators.northLeft = false;
        actuators.northRight = true;
        NodeAddress fromAddress;
        fromAddress.row = 1;
        fromAddress.column = 2;
        NodeAddress toAddress;
        toAddress.row = 2;
        toAddress.column = 2;
        DELAY_MS_1;
        sendHeatWiresRange(&fromAddress, &toAddress, &actuators, 50000, 10);
        return;
    }
#elif defined(SIMULATION_HEAT_WIRES_MODE_TEST)
    if (ParticleAttributes.node.type == NODE_TYPE_ORIGIN) {
        DELAY_MS_1;
        sendHeatWiresModePackage(HEATING_LEVEL_TYPE_STRONG);
        return;
    }
#elif defined(SIMULATION_SEND_HEADER_TEST)
    if (ParticleAttributes.node.type == NODE_TYPE_ORIGIN) {
        HeaderPackage package;
        package.id = PACKAGE_HEADER_ID_HEADER;
        package.enableBroadcast = true;
        DELAY_MS_1;
        sendHeaderPackage(&package);
        return;
    }
#endif
    ParticleAttributes.node.state = endState;
}

/**
 * Handles (state driven) relaying network geometry communication states.
 * @param endState the state when handler finished
 */
static void __handleRelayAnnounceNetworkGeometry(const StateType endState) {
    CommunicationProtocolPortState *commPortState = &ParticleAttributes.protocol.ports.north;
    switch (commPortState->initiatorState) {
        case COMMUNICATION_INITIATOR_STATE_TYPE_TRANSMIT:
            enableTransmission(&ParticleAttributes.communication.ports.tx.north);
            commPortState->initiatorState = COMMUNICATION_INITIATOR_STATE_TYPE_TRANSMIT_WAIT_FOR_TX_FINISHED;
            break;

            // wait for tx finished
        case COMMUNICATION_INITIATOR_STATE_TYPE_TRANSMIT_WAIT_FOR_TX_FINISHED:
            if (ParticleAttributes.communication.ports.tx.north.isTransmitting) {
                break;
            }
            commPortState->initiatorState = COMMUNICATION_INITIATOR_STATE_TYPE_IDLE;
            goto __COMMUNICATION_INITIATOR_STATE_TYPE_IDLE;
            break;

        case COMMUNICATION_INITIATOR_STATE_TYPE_WAIT_FOR_RESPONSE:
        case COMMUNICATION_INITIATOR_STATE_TYPE_TRANSMIT_ACK:
        case COMMUNICATION_INITIATOR_STATE_TYPE_TRANSMIT_ACK_WAIT_FOR_TX_FINISHED:
        __COMMUNICATION_INITIATOR_STATE_TYPE_IDLE:
        case COMMUNICATION_INITIATOR_STATE_TYPE_IDLE:
            ParticleAttributes.node.state = endState;
            break;
    }
}


/**
 * Handles (event driven) network geometry announcement states.
 * @param endState the state when handler finished
 */
static void __handleSendAnnounceNetworkGeometry(const StateType endState) {
    TxPort *txPort = &ParticleAttributes.communication.ports.tx.north;
    CommunicationProtocolPortState *commPortState = &ParticleAttributes.protocol.ports.north;

    switch (ParticleAttributes.protocol.ports.north.initiatorState) {
        case COMMUNICATION_INITIATOR_STATE_TYPE_TRANSMIT:
            constructAnnounceNetworkGeometryPackage(ParticleAttributes.node.address.row,
                                                    ParticleAttributes.node.address.column);
            enableTransmission(txPort);
            commPortState->initiatorState = COMMUNICATION_INITIATOR_STATE_TYPE_TRANSMIT_WAIT_FOR_TX_FINISHED;
            break;

            // wait for tx finished
        case COMMUNICATION_INITIATOR_STATE_TYPE_TRANSMIT_WAIT_FOR_TX_FINISHED:
            if (txPort->isTransmitting) {
                break;
            }
            commPortState->initiatorState = COMMUNICATION_INITIATOR_STATE_TYPE_IDLE;
            goto __COMMUNICATION_INITIATOR_STATE_TYPE_IDLE;
            break;

        case COMMUNICATION_INITIATOR_STATE_TYPE_WAIT_FOR_RESPONSE:
        case COMMUNICATION_INITIATOR_STATE_TYPE_TRANSMIT_ACK:
        case COMMUNICATION_INITIATOR_STATE_TYPE_TRANSMIT_ACK_WAIT_FOR_TX_FINISHED:
        __COMMUNICATION_INITIATOR_STATE_TYPE_IDLE:
        case COMMUNICATION_INITIATOR_STATE_TYPE_IDLE:
            ParticleAttributes.node.state = endState;
            break;
    }
}

/**
 * Advance communication timeout counters.
 */
static void __advanceCommunicationProtocolCounters(void) {
    if (!ParticleAttributes.communication.ports.tx.north.isTransmitting &&
        ParticleAttributes.protocol.ports.north.stateTimeoutCounter > 0) {
        ParticleAttributes.protocol.ports.north.stateTimeoutCounter--;
    }
    if (!ParticleAttributes.communication.ports.tx.east.isTransmitting &&
        ParticleAttributes.protocol.ports.east.stateTimeoutCounter > 0) {
        ParticleAttributes.protocol.ports.east.stateTimeoutCounter--;
    }
    if (!ParticleAttributes.communication.ports.tx.south.isTransmitting &&
        ParticleAttributes.protocol.ports.south.stateTimeoutCounter > 0) {
        ParticleAttributes.protocol.ports.south.stateTimeoutCounter--;
    }
}

/**
 * Handles discovery states, classifies the local node type and switches to next state.
 */
static void __handleNeighboursDiscovery(void) {
    __discoveryLoopCount();


//    if (ParticleAttributes.discoveryPulseCounters.loopCount == 0) {
//    LED_STATUS1_TOGGLE;
//    }

    if (ParticleAttributes.discoveryPulseCounters.loopCount >= MAX_NEIGHBOURS_DISCOVERY_LOOPS) {
//        LED_STATUS1_ON;
        // on discovery timeout
        __disableDiscoverySensing();
        ParticleAttributes.node.state = STATE_TYPE_NEIGHBOURS_DISCOVERED;
    } else if (ParticleAttributes.discoveryPulseCounters.loopCount >=
               // on min. discovery loops exceeded
               MIN_NEIGHBOURS_DISCOVERY_LOOPS) {
        if (updateAndDetermineNodeType()) {
            // on distinct discovery
            __disableDiscoverySensing();
            ParticleAttributes.node.state = STATE_TYPE_NEIGHBOURS_DISCOVERED;
        }
        else {
            PARTICLE_DISCOVERY_LOOP_DELAY;
        }

        // show discovery status
        if (ParticleAttributes.discoveryPulseCounters.north.isConnected) {
            LED_STATUS1_ON;
        }
        if (ParticleAttributes.discoveryPulseCounters.east.isConnected) {
            LED_STATUS3_ON;
        }
        if (ParticleAttributes.discoveryPulseCounters.south.isConnected) {
            LED_STATUS4_ON;
        }
        __updateOriginNodeAddress();
    }
}

/**
 * Handles the post discovery extended pulsing period with subsequent switch to next state.
 */
static void __handleDiscoveryPulsing(void) {
    if (ParticleAttributes.discoveryPulseCounters.loopCount >= MAX_NEIGHBOUR_PULSING_LOOPS) {
        __disableDiscoveryPulsing();
        ParticleAttributes.node.state = STATE_TYPE_DISCOVERY_PULSING_DONE;
    } else {
        __discoveryLoopCount();
    }
}

/**
 * Handle the discovery to enumeration state switch.
 */
static void __handleDiscoveryPulsingDone(void) {

    if (ParticleAttributes.node.type == NODE_TYPE_ORIGIN) {
        ParticleAttributes.node.state = STATE_TYPE_ENUMERATING_NEIGHBOURS;
        PARTICLE_DISCOVERY_PULSE_DONE_POST_DELAY;
    } else if (ParticleAttributes.node.type == NODE_TYPE_ORPHAN) {
        ParticleAttributes.node.state = STATE_TYPE_DISCOVERY_DONE_ORPHAN_NODE;
        return;
    } else {
        setReceptionistStateStart(&ParticleAttributes.protocol.ports.north);
        ParticleAttributes.node.state = STATE_TYPE_WAIT_FOR_BEING_ENUMERATED;
        DEBUG_CHAR_OUT('W');
    }
    __enableReception();
    clearReceptionBuffers();
}

///**
// * Sends the set network geometry package and switches to the given state.
// * @param rows the new network geometry rows
// * @param cols the new network geometry rows
// * @param endState the state when handler finished
// */
//static void __handleSetNetworkGeometry(const uint8_t rows, const uint8_t cols,
//                                       const StateType endState) {
//    CommunicationProtocolPortState *commPortState = ParticleAttributes.directionOrientedPorts.simultaneous.protocol;
//    TxPort *txPort = ParticleAttributes.directionOrientedPorts.simultaneous.txPort;
//    switch (commPortState->initiatorState) {
//        case COMMUNICATION_INITIATOR_STATE_TYPE_TRANSMIT:
//            constructSetNetworkGeometryPackage(txPort, rows, cols);
//            enableTransmission(txPort);
//            commPortState->initiatorState = COMMUNICATION_INITIATOR_STATE_TYPE_TRANSMIT_WAIT_FOR_TX_FINISHED;
//            break;
//
//            // wait for tx finished
//        case COMMUNICATION_INITIATOR_STATE_TYPE_TRANSMIT_WAIT_FOR_TX_FINISHED:
//            if (txPort->isTransmitting) {
//                break;
//            }
//            commPortState->initiatorState = COMMUNICATION_INITIATOR_STATE_TYPE_IDLE;
//            goto __COMMUNICATION_INITIATOR_STATE_TYPE_IDLE;
//            break;
//
//        case COMMUNICATION_INITIATOR_STATE_TYPE_WAIT_FOR_RESPONSE:
//        case COMMUNICATION_INITIATOR_STATE_TYPE_TRANSMIT_ACK:
//        case COMMUNICATION_INITIATOR_STATE_TYPE_TRANSMIT_ACK_WAIT_FOR_TX_FINISHED:
//        __COMMUNICATION_INITIATOR_STATE_TYPE_IDLE:
//        case COMMUNICATION_INITIATOR_STATE_TYPE_IDLE:
//            ParticleAttributes.node.state = endState;
//            break;
//    }
//}

/**
 * State driven handling of sending package states without ack requests.
 * @param port the designated port
 * @param endState the end state when handler finished
 */
static void __handleSendPackage(DirectionOrientedPort *const port, const StateType endState) {
    CommunicationProtocolPortState *commPortState = port->protocol;
    switch (commPortState->initiatorState) {
        case COMMUNICATION_INITIATOR_STATE_TYPE_TRANSMIT:
            enableTransmission(port->txPort);
            commPortState->initiatorState = COMMUNICATION_INITIATOR_STATE_TYPE_TRANSMIT_WAIT_FOR_TX_FINISHED;
            break;

            // wait for tx finished
        case COMMUNICATION_INITIATOR_STATE_TYPE_TRANSMIT_WAIT_FOR_TX_FINISHED:
            if (port->txPort->isTransmitting) {
                break;
            }
            commPortState->initiatorState = COMMUNICATION_INITIATOR_STATE_TYPE_IDLE;
            goto __COMMUNICATION_INITIATOR_STATE_TYPE_IDLE;
            break;

        case COMMUNICATION_INITIATOR_STATE_TYPE_WAIT_FOR_RESPONSE:
        case COMMUNICATION_INITIATOR_STATE_TYPE_TRANSMIT_ACK:
        case COMMUNICATION_INITIATOR_STATE_TYPE_TRANSMIT_ACK_WAIT_FOR_TX_FINISHED:
        __COMMUNICATION_INITIATOR_STATE_TYPE_IDLE:
        case COMMUNICATION_INITIATOR_STATE_TYPE_IDLE:
            ParticleAttributes.node.state = endState;
            break;
    }
}

/**
 * Callback when actuation command has finished.
 */
static void __onActuationDoneCallback(void) {
    ParticleAttributes.node.state = STATE_TYPE_IDLE;
}

/**
 * Checks whether an actuation is to be executed. Switches state if an actuation
 * command is scheduled and the current local time indicates an actuation start.
 */
static void __handleIsActuationCommandPeriod(void) {
    if (ParticleAttributes.actuationCommand.isScheduled &&
        ParticleAttributes.actuationCommand.executionState == ACTUATION_STATE_TYPE_IDLE) {
        if (ParticleAttributes.actuationCommand.actuationStart.periodTimeStamp <=
            ParticleAttributes.localTime.numTimePeriodsPassed) {
            ParticleAttributes.node.state = STATE_TYPE_EXECUTE_ACTUATION_COMMAND;
        }
    }
}


/**
 * The core function is called cyclically in the particle loop. It implements the
 * behaviour of the particle.
 */
static inline void process(void) {
    // DEBUG_CHAR_OUT('P');
    // ---------------- init states ----------------

    switch (ParticleAttributes.node.state) {
        case STATE_TYPE_RESET:
            __disableReception();
            constructParticle(&ParticleAttributes);
            ParticleAttributes.node.state = STATE_TYPE_START;
            goto __STATE_TYPE_START;
            break;

        __STATE_TYPE_START:
        case STATE_TYPE_START:
            __initParticle();
            ParticleAttributes.node.state = STATE_TYPE_ACTIVE;
            goto __STATE_TYPE_ACTIVE;
            break;

        __STATE_TYPE_ACTIVE:
        case STATE_TYPE_ACTIVE:
            ParticleAttributes.node.state = STATE_TYPE_NEIGHBOURS_DISCOVERY;
            __enableDiscoverySensing();
            MEMORY_BARRIER;
            __enableDiscoveryPulsing();
            SEI;
            goto __STATE_TYPE_NEIGHBOURS_DISCOVERY;
            break;

            // ---------------- boot states: discovery ----------------
        __STATE_TYPE_NEIGHBOURS_DISCOVERY:
        case STATE_TYPE_NEIGHBOURS_DISCOVERY:
            __handleNeighboursDiscovery();
            break;

        case STATE_TYPE_NEIGHBOURS_DISCOVERED:
            ParticleAttributes.node.state = STATE_TYPE_DISCOVERY_PULSING;
            goto __STATE_TYPE_DISCOVERY_PULSING;
            break;

        __STATE_TYPE_DISCOVERY_PULSING:
        case STATE_TYPE_DISCOVERY_PULSING:
            __handleDiscoveryPulsing();
            break;

        case STATE_TYPE_DISCOVERY_PULSING_DONE:
            __handleDiscoveryPulsingDone();
            break;

        case STATE_TYPE_DISCOVERY_DONE_ORPHAN_NODE:
            blinkKnightRidersKittForever();
            break;

            // ---------------- boot states: local enumeration ----------------

            // wait for incoming address from north neighbour
        case STATE_TYPE_WAIT_FOR_BEING_ENUMERATED:
            __handleWaitForBeingEnumerated();
            break;

        case STATE_TYPE_LOCALLY_ENUMERATED:
            ParticleAttributes.node.state = STATE_TYPE_ENUMERATING_NEIGHBOURS;
            goto __STATE_TYPE_ENUMERATING_NEIGHBOURS;
            break;

            // ---------------- boot states: neighbour enumeration ----------------

        __STATE_TYPE_ENUMERATING_NEIGHBOURS:
        case STATE_TYPE_ENUMERATING_NEIGHBOURS:
            setInitiatorStateStart(&ParticleAttributes.protocol.ports.east);
            DEBUG_CHAR_OUT('E');
            ParticleAttributes.node.state = STATE_TYPE_ENUMERATING_EAST_NEIGHBOUR;
            goto __STATE_TYPE_ENUMERATING_EAST_NEIGHBOUR;
            break;

            // ---------------- boot states: east neighbour enumeration ----------------

        __STATE_TYPE_ENUMERATING_EAST_NEIGHBOUR:
        case STATE_TYPE_ENUMERATING_EAST_NEIGHBOUR:
            handleEnumerateNeighbour(&ParticleAttributes.directionOrientedPorts.east,
                                     ParticleAttributes.node.address.row,
                                     ParticleAttributes.node.address.column + 1,
                                     STATE_TYPE_ENUMERATING_EAST_NEIGHBOUR_DONE);
            __advanceCommunicationProtocolCounters();
            break;

        case STATE_TYPE_ENUMERATING_EAST_NEIGHBOUR_DONE:
            setInitiatorStateStart(&ParticleAttributes.protocol.ports.south);
            DEBUG_CHAR_OUT('e');
            ParticleAttributes.node.state = STATE_TYPE_ENUMERATING_SOUTH_NEIGHBOUR;
            goto __STATE_TYPE_ENUMERATING_SOUTH_NEIGHBOUR;
            break;

            // ---------------- boot states: south neighbour enumeration ----------------

        __STATE_TYPE_ENUMERATING_SOUTH_NEIGHBOUR:
        case STATE_TYPE_ENUMERATING_SOUTH_NEIGHBOUR:
            handleEnumerateNeighbour(&ParticleAttributes.directionOrientedPorts.south,
                                     ParticleAttributes.node.address.row + 1,
                                     ParticleAttributes.node.address.column,
                                     STATE_TYPE_ENUMERATING_SOUTH_NEIGHBOUR_DONE);
            __advanceCommunicationProtocolCounters();
            break;

        case STATE_TYPE_ENUMERATING_SOUTH_NEIGHBOUR_DONE:
            ParticleAttributes.node.state = STATE_TYPE_ENUMERATING_NEIGHBOURS_DONE;
            goto __STATE_TYPE_ENUMERATING_NEIGHBOURS_DONE;
            break;

        __STATE_TYPE_ENUMERATING_NEIGHBOURS_DONE:
        case STATE_TYPE_ENUMERATING_NEIGHBOURS_DONE:
            setInitiatorStateStart(&ParticleAttributes.protocol.ports.north);
            ParticleAttributes.node.state = STATE_TYPE_ANNOUNCE_NETWORK_GEOMETRY;
            enableLocalTimeInterrupt();
            goto __STATE_TYPE_ANNOUNCE_NETWORK_GEOMETRY;
            break;

            // ---------------- boot states: network geometry detection/announcement ----------------

        __STATE_TYPE_ANNOUNCE_NETWORK_GEOMETRY:
        case STATE_TYPE_ANNOUNCE_NETWORK_GEOMETRY:
            // on right most and bottom most node
            if ((ParticleAttributes.node.type == NODE_TYPE_TAIL) &&
                (true == ParticleAttributes.protocol.hasNetworkGeometryDiscoveryBreadCrumb)) {
                __handleSendAnnounceNetworkGeometry(STATE_TYPE_ANNOUNCE_NETWORK_GEOMETRY_DONE);
            } else {
                ParticleAttributes.node.state = STATE_TYPE_ANNOUNCE_NETWORK_GEOMETRY_DONE;
                goto __STATE_TYPE_ANNOUNCE_NETWORK_GEOMETRY_DONE;
            }
            break;

        __STATE_TYPE_ANNOUNCE_NETWORK_GEOMETRY_DONE:
        case STATE_TYPE_ANNOUNCE_NETWORK_GEOMETRY_DONE:
            ParticleAttributes.node.state = STATE_TYPE_IDLE;
            goto __STATE_TYPE_IDLE;
            break;

        case STATE_TYPE_ANNOUNCE_NETWORK_GEOMETRY_RELAY:
            __handleRelayAnnounceNetworkGeometry(STATE_TYPE_ANNOUNCE_NETWORK_GEOMETRY_RELAY_DONE);
            break;

        case STATE_TYPE_ANNOUNCE_NETWORK_GEOMETRY_RELAY_DONE:
            ParticleAttributes.node.state = STATE_TYPE_IDLE;
            goto __STATE_TYPE_IDLE;
            break;

            // ---------------- working states: sync neighbour ----------------

        case STATE_TYPE_RESYNC_NEIGHBOUR:
            setInitiatorStateStart(ParticleAttributes.directionOrientedPorts.simultaneous.protocol);
            ParticleAttributes.node.state = STATE_TYPE_SYNC_NEIGHBOUR;
            break;

        case STATE_TYPE_SYNC_NEIGHBOUR:
            __handleSynchronizeNeighbour(STATE_TYPE_SYNC_NEIGHBOUR_DONE);
            break;

        case STATE_TYPE_SYNC_NEIGHBOUR_DONE:
            __handleSynchronizeNeighbourDoneOrRunTest(STATE_TYPE_IDLE);
            ParticleAttributes.timeSynchronization.isNextSyncPackageTransmissionEnabled = true;
            break;

//            // ---------------- working states: set network geometry----------------
//        case STATE_TYPE_SEND_SET_NETWORK_GEOMETRY:
//            __handleSetNetworkGeometry(ParticleAttributes.protocol.networkGeometry.rows,
//                                       ParticleAttributes.protocol.networkGeometry.columns,
//                                       STATE_TYPE_IDLE);
//            break;

            // ---------------- working states: execute actuation command----------------

        case STATE_TYPE_EXECUTE_ACTUATION_COMMAND:
            handleExecuteActuation(__onActuationDoneCallback);
            break;

            // ---------------- working states: sending package ----------------

        case STATE_TYPE_SENDING_PACKAGE_TO_NORTH:
            __handleSendPackage(&ParticleAttributes.directionOrientedPorts.north,
                                STATE_TYPE_IDLE);
            break;

        case STATE_TYPE_SENDING_PACKAGE_TO_EAST:
            __handleSendPackage(&ParticleAttributes.directionOrientedPorts.east,
                                STATE_TYPE_IDLE);
            break;

        case STATE_TYPE_SENDING_PACKAGE_TO_EAST_AND_SOUTH:
            __handleSendPackage(&ParticleAttributes.directionOrientedPorts.simultaneous,
                                STATE_TYPE_IDLE);
            break;

        case STATE_TYPE_SENDING_PACKAGE_TO_SOUTH:
            __handleSendPackage(&ParticleAttributes.directionOrientedPorts.south,
                                STATE_TYPE_IDLE);
            break;

        case STATE_TYPE_SENDING_PACKAGE_TO_NORTH_THEN_PREPARE_SLEEP:
            __handleSendPackage(&ParticleAttributes.directionOrientedPorts.north,
                                STATE_TYPE_PREPARE_FOR_SLEEP);
            break;

        case STATE_TYPE_SENDING_PACKAGE_TO_EAST_THEN_PREPARE_SLEEP:
            __handleSendPackage(&ParticleAttributes.directionOrientedPorts.east,
                                STATE_TYPE_PREPARE_FOR_SLEEP);
            break;

        case STATE_TYPE_SENDING_PACKAGE_TO_EAST_AND_SOUTH_THEN_PREPARE_SLEEP:
            __handleSendPackage(&ParticleAttributes.directionOrientedPorts.simultaneous,
                                STATE_TYPE_PREPARE_FOR_SLEEP);
            break;

        case STATE_TYPE_SENDING_PACKAGE_TO_SOUTH_THEN_PREPARE_SLEEP:
            __handleSendPackage(&ParticleAttributes.directionOrientedPorts.south,
                                STATE_TYPE_PREPARE_FOR_SLEEP);
            break;


            // ---------------- working states: receiving/interpreting commands ----------------

        __STATE_TYPE_IDLE:
        case STATE_TYPE_IDLE:
            ParticleAttributes.directionOrientedPorts.north.receivePimpl();
            ParticleAttributes.directionOrientedPorts.east.receivePimpl();
            ParticleAttributes.directionOrientedPorts.south.receivePimpl();
            __handleIsActuationCommandPeriod();

            // future time stamp dependent execution should be better placed in the scheduler
            processScheduler();
//            shiftConsumableLocalTimeTrackingClockLagUnitsToIsr();
//            // TODO: evaluation code
//            if (ParticleAttributes.localTime.numTimePeriodsPassed > 255) {
//                blinkAddressNonblocking();
//                blinkTimeIntervalNonblocking();
//                ParticleAttributes.periphery.isTxSouthToggleEnabled = true;
//
//            }
            break;

            // ---------------- standby states: sleep mode related states ----------------

        case STATE_TYPE_PREPARE_FOR_SLEEP:
            if (ParticleAttributes.directionOrientedPorts.north.txPort->isTransmitting ||
                ParticleAttributes.directionOrientedPorts.east.txPort->isTransmitting ||
                ParticleAttributes.directionOrientedPorts.south.txPort->isTransmitting) {
                break;
            }
            ParticleAttributes.node.state = STATE_TYPE_SLEEP_MODE;
            break;

        case STATE_TYPE_SLEEP_MODE:
            DEBUG_CHAR_OUT('z');
            sleep_enable();
            MEMORY_BARRIER;
            CLI;
            MEMORY_BARRIER;
            sleep_cpu();
            sleep_disable();
            DEBUG_CHAR_OUT('Z');
            break;

            // ---------------- erroneous/dead end states ----------------
        case STATE_TYPE_UNDEFINED:
        case STATE_TYPE_ERRONEOUS:
        case STATE_TYPE_STALE:
//        __STATE_TYPE_STALE:
            break;
    }
}
