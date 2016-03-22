/**
 * @author Raoul Rubien 2015
 */

#ifndef PARTICLE_TYPES_H
#define PARTICLE_TYPES_H

#include "./communication/CommunicationTypes.h"
#include <common/PortInteraction.h>

/**
 * Possible particle's state machine states are listed in this enum.
 */
typedef enum {
    STATE_TYPE_START, // particle is initialized
    STATE_TYPE_ACTIVE, // start neighbour discovery
    STATE_TYPE_NEIGHBOURS_DISCOVERY, // evaluates comm. port pulse counters
    STATE_TYPE_NEIGHBOURS_DISCOVERED, // discovery ended
    STATE_TYPE_DISCOVERY_PULSING, // keep pulsing post discovery
    STATE_TYPE_RESET, // state after reset command is received
    STATE_TYPE_WAIT_FOR_BEING_ENUMERATED, // waiting for network address assignment
    STATE_TYPE_ENUMERATING_SOUTH_NEIGHBOUR, // assigning network address to south neighbour
    STATE_TYPE_ENUMERATING_EAST_NEIGHBOUR, // assigning network address to east neighbour
    STATE_TYPE_ENUMERATED, // local address assigned and neighbour/s enumerated
    STATE_TYPE_IDLE, // waiting for commands
    STATE_TYPE_INTERPRET_COMMAND, // interpret command
    STATE_TYPE_TX_START, // transmitting data in buffers
    STATE_TYPE_TX_DONE, // transmission done, buffer empty
    STATE_TYPE_FORWARD_PKG,
    STATE_TYPE_SCHEDULE_COMMAND,
    STATE_TYPE_EXECUTE_COMMAND,
    STATE_TYPE_COMMAND_SCHEDULED_ACK,
    STATE_TYPE_ERRONEOUS // erroneous state machine state
} StateType;

/**
 * The node type describes node's connectivity or in other words the position in the network.
 */
typedef enum {
    NODE_TYPE_ORPHAN, // no connection
    NODE_TYPE_ORIGIN, // connected at south or south and east
    NODE_TYPE_INTER_HEAD, // connected at north and south and east
    NODE_TYPE_INTER_NODE, // connected at north and south
    NODE_TYPE_TAIL, // connected at north
    NODE_TYPE_INVALID // invalid note type
} NodeType;

/**
 * Stores the last flank state since the pin change interrupt hardware has no sense control. The state
 * is updated at any pin change interrupt accordingly.
typedef struct {
    unsigned char north : 1;
    unsigned char south : 1;
    unsigned char east : 1;
    unsigned char : 5;
} RxInterruptFlankStates;
 */


/**
 * A pulse counter and it's connectivity state.
 */
typedef struct {
    unsigned char counter : 4; //pulse counter
    unsigned char isConnected : 1; // connectivity flag
} PulseCounter;

/**
 * Stores the amount of incoming pulses for each communication channel. The isConnected flags are set
 * if the number of incoming pulses exceeds a specific threshold.
 */
typedef struct {
    PulseCounter north;
    PulseCounter south;
    PulseCounter east;
    unsigned char loopCount; // particle loop counter
} RxDiscoveryPulseCounter;

/**
 * The node address in the network. It is spread from the origin node which assigns itself
 * the first address (row=1, column=1).
 */
typedef struct {
    unsigned char row;
    unsigned char column;
} NodeId;

/**
 * The global particle state with references to the most important states, buffers, counters,
 * etc.
 */
typedef struct {
    StateType state;
    NodeType type;
    NodeId nodeId;
    TxBitBuffer txBitBuffer;
    RxBitBuffer rxBitBuffer;
    RxDiscoveryPulseCounter rxDiscoveryPulseCounters;
    ReceptionCounterParameters rxCounterArguments;
} ParticleState;

#endif