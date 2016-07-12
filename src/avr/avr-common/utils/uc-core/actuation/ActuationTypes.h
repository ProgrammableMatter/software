/**
 * @author Raoul Rubien  12.07.16.
 */

#pragma once

/**
 * Describes all possible output modes.
 */
typedef enum HeatingLevelType {
    HEATING_LEVEL_TYPE_MAXIMUM = 0x0,
    HEATING_LEVEL_TYPE_STRONG = 0x1,
    HEATING_LEVEL_TYPE_MEDIUM = 0x2,
    HEATING_LEVEL_TYPE_WEAK = 0x3,
} HeatingLevelType;

/**
 * Describes which wires are to be actuated.
 */
typedef struct Actuators {
    /**
     * in case of folding along y-axis
     */
    uint8_t northLeft : 1;
    /**
     * in case of folding along y-axis
     */
    uint8_t northRight : 1;
    /**
     * in case of folding along x-axis
     */
    uint8_t eastLeft : 1;
    /**
     * in case of folding along x-axis
     */
    uint8_t eastRight : 1;
    uint8_t __pad : 4;
} Actuators;

/**
 * Describes the heating mode/output power.
 */
typedef struct HeatingMode {
    uint8_t dutyCycleLevel : 2;
    uint8_t __pad : 6;
} HeatingMode;

/**
 * Describes an actuation command.
 */
typedef struct ActuationCommand {
    Actuators actuators;
    HeatingMode actuationPower;
} ActuationCommand;