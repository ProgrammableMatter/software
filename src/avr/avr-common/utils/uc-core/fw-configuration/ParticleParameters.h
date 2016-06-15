/**
 * @author Raoul Rubien 2016
 */

#pragma once

/**
 * The maximum numbers of pulses generated for neighbour discovery.
 */
#define RX_DISCOVERY_PULSE_COUNTER_MAX 10

/**
 * Minimum signals to be detected per communication channel until this side is recognized as
 * connected to a neighbour.
 */
#define MIN_RX_NEIGHBOUR_SIGNALS_SENSE ((uint8_t)(10))
/**
 * Earliest particle loop when local node discovery may be finished.
 */
#define MIN_NEIGHBOURS_DISCOVERY_LOOPS ((uint8_t)(50))
/**
 * Latest particle loop when local node discovery is to be aborted.
 */
#define MAX_NEIGHBOURS_DISCOVERY_LOOPS ((uint8_t)(100))
/**
 * Latest particle loop when pulsing to neighbours is to be deactivated.
 */
#define MAX_NEIGHBOUR_PULSING_LOOPS ((uint8_t)(254))

/**
 * Neighbour discovery counter 1 compare A value.
 */
#define DEFAULT_NEIGHBOUR_SENSING_COUNTER_COMPARE_VALUE ((uint16_t)0x80)

/**
 * Initial value for reception counter compare value. Should be greater than 450 and less than 2032. Note
 * that (2*DEFAULT_TX_RX_COMPARE_TOP_VALUE + 2*ParticleAttributes.rxArguments.receptionDelta) must
 * fit into uint16_t.
 */
#define DEFAULT_TX_RX_COMPARE_TOP_VALUE ((uint16_t) 2000)


#define DEFAULT_MAX_SHORT_RECEPTION_SNAPSHOT_DISTANCE ((uint16_t)(0.76 * DEFAULT_TX_RX_COMPARE_TOP_VALUE))
#define DEFAULT_MAX_LONG_RECEPTION_SNAPSHOT_DISTANCE ((uint16_t)(1.33 * DEFAULT_TX_RX_COMPARE_TOP_VALUE))

/**
 * Accepted deviation (constant) on reception:
 * Signals received within [DEFAULT_TX_RX_COMPARE_TOP_VALUE +/- TX_RX_RECEPTION_DELTA_DIVISOR ] time span are
 * classified synchronization/rectification bits.
 * Signals received within [DEFAULT_TX_RX_COMPARE_TOP_VALUE / DEFAULT_TX_RX_COUNTER_CENTER_VALUE_DIVISOR +/- TX_RX_RECEPTION_DELTA_DIVISOR ] time span are classified as data bits.
 */
#define TX_RX_RECEPTION_DELTA_VALUE_DIVISOR 4

/**
 * Divisor for calculating the center of DEFAULT_TX_RX_COMPARE_TOP_VALUE.
 */
#define TX_RX_COUNTER_CENTER_VALUE_DIVISOR 2

/**
 * The center interrupt is pre-scheduled by the offset to adjust the interrupt latency.
 */
#define TX_RX_COUNTER_CENTER_INTERRUPT_LATENCY_ADJUSTMENT 0


/**
 * The left, center and right borders are classification limits which are used to determine whether a received
 * signal represents a data bit or is the coding clock/rectification flank.
 */
#define TX_RX_RECEPTION_CLASSIFICATION_VALUE_LEFT_BORDER ((uint16_t)(DEFAULT_TX_RX_COMPARE_TOP_VALUE / TX_RX_RECEPTION_DELTA_VALUE_DIVISOR))
#define TX_RX_RECEPTION_CLASSIFICATION_VALUE_CENTER ((uint16_t)(DEFAULT_TX_RX_COMPARE_TOP_VALUE / TX_RX_COUNTER_CENTER_VALUE_DIVISOR))
#define TX_RX_RECEPTION_CLASSIFICATION_VALUE_RIGHT_BORDER ((uint16_t)(TX_RX_RECEPTION_CLASSIFICATION_VALUE_CENTER + TX_RX_RECEPTION_CLASSIFICATION_VALUE_LEFT_BORDER))


/**
 * Initial counter compare interrupt value for updating the ongoing reception timeout.
 */
// TODO: timeout has been removed from timer/counter0 interrupt -> remove timeout related macros
#define RX_TIMEOUT_INTERRUPT_VALUE_DIVISOR 8

/**
 * Heartbeat LED toggles every main-loop count
 */
#define HEARTBEAT_LOOP_COUNT_TOGGLE ((uint8_t)20)