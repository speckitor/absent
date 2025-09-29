#pragma once

#include "types.h"

void setup_desktop_names(state_t *s, monitor_t *mon);

void switch_desktop(state_t *s, const char *name);
void switch_desktop_by_idx(state_t *s, int desktop_id);
void client_move_to_desktop(state_t *s, const char *name);
