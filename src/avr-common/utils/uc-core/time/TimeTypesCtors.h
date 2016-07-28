/**
 * @author Raoul Rubien  12.07.2016
 *
 * Time types constructor implementation.
 */

#pragma once

#include "TimeTypes.h"
#include "uc-core/configuration/Time.h"

/**
 * constructor function
 * @param o the object to construct
 */
extern CTOR_ATTRS void constructLocalTimeTracking(volatile LocalTimeTracking *o);

extern CTOR_ATTRS void constructLocalTimeTracking(volatile LocalTimeTracking *o) {
    o->numTimePeriodsPassed = 0;
    o->timePeriodInterruptDelay = LOCAL_TIME_DEFAULT_INTERRUPT_DELAY;
}