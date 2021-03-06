/*
 * @author Raoul Rubien 11.10.2016
 *
 * Particle state definition.
 */

#pragma once

/**
 * Possible particle's state machine states are listed in this enum.
 */
typedef enum StateType {
    // uninitialized state
            STATE_TYPE_UNDEFINED = 0,
    // state when particle is initializing
            STATE_TYPE_START,
    // state when particle is fully initialized
            STATE_TYPE_ACTIVE,

    // state when evaluating discovery pulses
            STATE_TYPE_NEIGHBOURS_DISCOVERY,
    // state wen discovery ended
            STATE_TYPE_NEIGHBOURS_DISCOVERED,
    // state of discovery pulsing period post discovery
            STATE_TYPE_DISCOVERY_PULSING,
    // state when discovery pulsing ended
            STATE_TYPE_DISCOVERY_PULSING_DONE,
    // state when discovery is performed but node has no neighbors
            STATE_TYPE_DISCOVERY_DONE_ORPHAN_NODE,
    // state after reset
            STATE_TYPE_RESET,

    // state when waiting for /receiving local address from parent neighbor
            STATE_TYPE_WAIT_FOR_BEING_ENUMERATED,
    // state when local address is assigned successfully
            STATE_TYPE_LOCALLY_ENUMERATED,
    // state when starting neighbour enumeration
            STATE_TYPE_ENUMERATING_NEIGHBOURS,
    // state wen assigning network address to east neighbour
            STATE_TYPE_ENUMERATING_EAST_NEIGHBOUR,
    // state when east enumeration finished
            STATE_TYPE_ENUMERATING_EAST_NEIGHBOUR_DONE,
    // state when assigning network address to south neighbour
            STATE_TYPE_ENUMERATING_SOUTH_NEIGHBOUR,
    // state when south enumeration finished
            STATE_TYPE_ENUMERATING_SOUTH_NEIGHBOUR_DONE,
    // state when neighbour enumeration finished
            STATE_TYPE_ENUMERATING_NEIGHBOURS_DONE,

    // state when last particle sends local address to the origin
            STATE_TYPE_ANNOUNCE_NETWORK_GEOMETRY,
    // state when relaying the network address announcement to origin
            STATE_TYPE_ANNOUNCE_NETWORK_GEOMETRY_RELAY,
    // state when relaying is finished
            STATE_TYPE_ANNOUNCE_NETWORK_GEOMETRY_RELAY_DONE,
    // state when announcing network geometry is finished
            STATE_TYPE_ANNOUNCE_NETWORK_GEOMETRY_DONE,

    // state set by interrupt handler indicating new synchronization to be scheduled
            STATE_TYPE_RESYNC_NEIGHBOUR,
    // state when origin node sends local time to neighbours
            STATE_TYPE_SYNC_NEIGHBOUR,
    // state when origin sending local time to neighbours has finished
            STATE_TYPE_SYNC_NEIGHBOUR_DONE,

//    // working state when origin broadcasts a new network geometry
//            STATE_TYPE_SEND_SET_NETWORK_GEOMETRY,
    // working state when actuation command is executed
            STATE_TYPE_EXECUTE_ACTUATION_COMMAND,

    // working state when transmitting package to north
            STATE_TYPE_SENDING_PACKAGE_TO_NORTH,
    // working state when transmitting package to east
            STATE_TYPE_SENDING_PACKAGE_TO_EAST,
    // working state while transmitting package to east and south simultaneously
            STATE_TYPE_SENDING_PACKAGE_TO_EAST_AND_SOUTH,
    // working state while transmitting package to south
            STATE_TYPE_SENDING_PACKAGE_TO_SOUTH,

    // working state when transmitting package to north followed by preparing for sleep mode
            STATE_TYPE_SENDING_PACKAGE_TO_NORTH_THEN_PREPARE_SLEEP,
    // working state when transmitting package to east followed by preparing for sleep mode
            STATE_TYPE_SENDING_PACKAGE_TO_EAST_THEN_PREPARE_SLEEP,
    // working state while transmitting package to east and south simultaneously followed by preparing for sleep mode
            STATE_TYPE_SENDING_PACKAGE_TO_EAST_AND_SOUTH_THEN_PREPARE_SLEEP,
    // working state while transmitting package to south followed by preparing for sleep mode
            STATE_TYPE_SENDING_PACKAGE_TO_SOUTH_THEN_PREPARE_SLEEP,

    // working state when waiting for commands or executing scheduled tasks
            STATE_TYPE_IDLE,
    // erroneous machine state
            STATE_TYPE_ERRONEOUS,
    // dead lock state; usually before shutdown
            STATE_TYPE_STALE,

    // working state when waiting for all transmissions to be finished followed by sleep mode
            STATE_TYPE_PREPARE_FOR_SLEEP,
    // state when before MCU goes int sleep mode
            STATE_TYPE_SLEEP_MODE,
} StateType;

/**
 * The node type describes node type according to the connectivity detected when discovery process
 * finished.
 */
typedef enum NodeType {
    // invalid or uninitialized note
            NODE_TYPE_INVALID = 0,
    // not connected node
            NODE_TYPE_ORPHAN,
    // node connected at south or south and east
            NODE_TYPE_ORIGIN,
    // node connected at north and south and east
            NODE_TYPE_INTER_HEAD,
    // node connected at north and south
            NODE_TYPE_INTER_NODE,
    // node connected at north
            NODE_TYPE_TAIL,
    // for testing purposes when the node is attached to the NODE_TYPE_ORIGIN node
            NODE_TYPE_MASTER
} NodeType;
