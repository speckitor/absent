#pragma once

#include <xcb/randr.h>
#include <xcb/xcb.h>

#include "types.h"

void monitors_setup(state_t *s);
monitor_t *monitor_contains_cursor(state_t *s);
