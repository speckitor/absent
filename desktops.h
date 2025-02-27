#ifndef ABSENT_DESKTOPS_H
#define ABSENT_DESKTOPS_H

#include "types.h"

void setup_desktop_names(state_t *s, monitor_t *mon);
void switch_desktop(state_t *s, const char *name);
void client_move_to_desktop(state_t *s, const char *name);

void hide_client(state_t *s, client_t *cl);
void show_client(state_t *s, client_t *cl);

#endif
