#ifndef EVENTS_H_
#define EVENTS_H_

#include "types.h"

void send_event(state_t *s, client_t *cl, xcb_atom_t protocol);

void main_loop(state_t *s);

#define XCB_LAST_EVENT 35

typedef void (*event_handler_t)(state_t *, xcb_generic_event_t *);

#endif // EVENTS_H_
