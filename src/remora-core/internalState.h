#ifndef INTERNAL_STATE_H
#define INTERNAL_STATE_H

#include <cstdint>
#include "configuration.h"

struct InternalState {
    volatile int32_t jointStepCount[Config::joints];
};

extern InternalState gInternalState;

#endif
