#pragma once

#include <xcb/xcb.h>

#include "types.h"

void main_loop(state_t *s);

void map_request(state_t *s, xcb_generic_event_t *ev);
void unmap_notify(state_t *s, xcb_generic_event_t *ev);
void configure_request(state_t *s, xcb_generic_event_t *ev);
void client_message(state_t *s, xcb_generic_event_t *ev);
void destroy_notify(state_t *s, xcb_generic_event_t *ev);
void key_press(state_t *s, xcb_generic_event_t *ev);
void button_press(state_t *s, xcb_generic_event_t *ev);
void button_release(state_t *s);
void motion_notify(state_t *s, xcb_generic_event_t *ev);

void send_event(state_t *s, client_t *cl, xcb_atom_t protocol);

#define XCB_LAST_EVENT 35

typedef void (*event_handler_t)(state_t *, xcb_generic_event_t *);
