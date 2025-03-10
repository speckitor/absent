#ifndef ABSENT_EVENTS_H
#define ABSENT_EVENTS_H

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

event_handler_t handlers[XCB_LAST_EVENT] = {
    [XCB_MAP_REQUEST] = map_request,
    [XCB_UNMAP_NOTIFY] = unmap_notify,
    [XCB_CONFIGURE_REQUEST] = configure_request,
    [XCB_CLIENT_MESSAGE] = client_message,
    [XCB_DESTROY_NOTIFY] = destroy_notify,
    [XCB_KEY_PRESS] = key_press,
    [XCB_BUTTON_PRESS] = button_press,
    [XCB_BUTTON_RELEASE] = (event_handler_t)button_release,
    [XCB_MOTION_NOTIFY] = motion_notify,
};

#endif
