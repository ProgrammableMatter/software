/**
 * @author Raoul Rubien on 21.11.15.
 */

#ifndef PROJECT_PARTICLETYPES_H_H
#define PROJECT_PARTICLETYPES_H_H

typedef enum {
    STATE_TYPE_START,
    STATE_TYPE_ACTIVE,
    STATE_TYPE_NEIGHBOURS_DISCOVERY,
    STATE_TYPE_WAITING,
    STATE_TYPE_RESET,
    STATE_TYPE_ENUMERATING,
    STATE_TYPE_ENUMERATED,
    STATE_TYPE_IDLE,
    STATE_TYPE_INTERPRETE_COMMAND,
    STATE_TYPE_RX_A,
    STATE_TYPE_RX_B,
    STATE_TYPE_FORWARD_PKG,
    STATE_TYPE_SCHEDULE_COMMAND,
    STATE_TYPE_EXECUTE_COMMAND,
    STATE_TYPE_COMMAND_SCHEDULED_ACK
} StateType;

typedef enum {
    NODE_TYPE_ORPHAN, NODE_TYPE_HEAD, NODE_TYPE_INTER_NODE, NODE_TYPE_TAIL
} NodeType;

typedef struct {
    StateType state;
    NodeType type;
    unsigned char nodeId;
    unsigned char northRxEvents;
    unsigned char southRxEvents;
} ParticleState;

#endif