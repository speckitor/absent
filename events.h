#ifndef ABSENT_EVENTS_H
#define ABSENT_EVENTS_H

#include <xcb/xcb.h>

#include "types.h"

void main_loop(state_t *s);

void map_request(state_t *s, xcb_generic_event_t *ev);
void configure_request(state_t *s, xcb_generic_event_t *ev);
void destroy_notify(state_t *s, xcb_generic_event_t *ev);
void key_press(state_t *s, xcb_generic_event_t *ev);
void button_press(state_t *s, xcb_generic_event_t *ev);
void button_release(state_t *s);
void motion_notify(state_t *s, xcb_generic_event_t *ev);
void enter_notify(state_t *s, xcb_generic_event_t *ev);

#endif
