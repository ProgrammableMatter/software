/*
 * @author Raoul Rubien 18.09.2016
 *
 * Scheduler types constructor implementation.
 */

#pragma once

#include "SchedulerTypes.h"
#include "common/common.h"

/**
* constructor function
* @param o the object to construct
**/
void constructSchedulerTask(SchedulerTask *const o) {
    o->startTimestamp = 0;
    o->endTimestamp = 0;
    o->state = STATE_TYPE_IDLE;
    o->startAction = NULL;
    o->endAction = NULL;
    o->isTimeLimited = false;
    o->isStateLimited = false;
    o->isEnabled = false;
    o->isExecuted = false;
    o->isStarted = false;
    o->isStartActionExecuted = false;
    o->isEndActionExecuted = false;
}

/**
* constructor function
* @param o the object to construct
**/
void constructScheduler(Scheduler *const o) {
    for (uint8_t idx = 0; idx < SCHEDULER_MAX_TASKS; idx++) {
        constructSchedulerTask(&o->tasks[idx]);
    }
    o->lastCallToScheduler = 0;
}