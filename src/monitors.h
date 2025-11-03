#ifndef MONITORS_H_
#define MONITORS_H_

#include "types.h"

void monitors_setup(state_t *s);
monitor_t *monitor_contains_cursor(state_t *s);

#endif // MONITORS_H_
