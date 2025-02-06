#include <stdio.h>
#include <stdlib.h>

#include <xcb/xcb.h>
#include <xcb/xcb_event.h>
#include <xcb/xcb_icccm.h>
#include <xcb/xproto.h>

#include "clients.h"
#include "config.h"
#include "events.h"
#include "keys.h"
#include "monitors.h"

void main_loop(state_t *s) {
  xcb_generic_event_t *event;
  while ((event = xcb_wait_for_event(s->c))) {
    switch (XCB_EVENT_RESPONSE_TYPE(event)) {
    case XCB_MAP_REQUEST:
      map_request(s, event);
      break;
    case XCB_UNMAP_NOTIFY:
      unmap_notify(s, event);
      break;
    case XCB_CONFIGURE_REQUEST:
      configure_request(s, event);
      break;
    case XCB_DESTROY_NOTIFY:
      destroy_notify(s, event);
      break;
    case XCB_KEY_PRESS:
      key_press(s, event);
      break;
    case XCB_BUTTON_PRESS:
      button_press(s, event);
      break;
    case XCB_BUTTON_RELEASE:
      button_release(s);
      break;
    case XCB_MOTION_NOTIFY:
      motion_notify(s, event);
      break;
    case XCB_ENTER_NOTIFY:
      enter_notify(s, event);
      break;
    }

    free(event);
  }
}

void map_request(state_t *s, xcb_generic_event_t *ev) {
  xcb_map_request_event_t *e = (xcb_map_request_event_t *)ev;

  client_create(s, e->window);
}

void unmap_notify(state_t *s, xcb_generic_event_t *ev) {
  xcb_unmap_notify_event_t *e = (xcb_unmap_notify_event_t *)ev;

  xcb_unmap_window(s->c, e->window);

  client_remove(s, e->window);

  button_release(s);

  xcb_flush(s->c);
}

void configure_request(state_t *s, xcb_generic_event_t *ev) {
  xcb_configure_request_event_t *e = (xcb_configure_request_event_t *)ev;

  uint16_t value_mask = 0;
  uint32_t value_list[7];
  int i = 0;

  client_t *cl = client_from_wid(s, e->window);
  if (cl) {
    return;
  }

  if (e->value_mask & XCB_CONFIG_WINDOW_X) {
    value_mask |= XCB_CONFIG_WINDOW_X;
    value_list[i++] = e->x;
  }
  if (e->value_mask & XCB_CONFIG_WINDOW_Y) {
    value_mask |= XCB_CONFIG_WINDOW_Y;
    value_list[i++] = e->y;
  }
  if (e->value_mask & XCB_CONFIG_WINDOW_WIDTH) {
    value_mask |= XCB_CONFIG_WINDOW_WIDTH;
    value_list[i++] = e->width;
  }
  if (e->value_mask & XCB_CONFIG_WINDOW_HEIGHT) {
    value_mask |= XCB_CONFIG_WINDOW_HEIGHT;
    value_list[i++] = e->height;
  }
  if (e->value_mask & XCB_CONFIG_WINDOW_BORDER_WIDTH) {
    value_mask |= XCB_CONFIG_WINDOW_BORDER_WIDTH;
    value_list[i++] = e->border_width;
  }
  if (e->value_mask & XCB_CONFIG_WINDOW_SIBLING) {
    value_mask |= XCB_CONFIG_WINDOW_SIBLING;
    value_list[i++] = e->sibling;
  }
  if (e->value_mask & XCB_CONFIG_WINDOW_STACK_MODE) {
    value_mask |= XCB_CONFIG_WINDOW_STACK_MODE;
    value_list[i++] = e->stack_mode;
  }

  xcb_configure_window(s->c, e->window, value_mask, value_list);
  xcb_flush(s->c);
}

void destroy_notify(state_t *s, xcb_generic_event_t *ev) {
  xcb_destroy_notify_event_t *e = (xcb_destroy_notify_event_t *)ev;

  client_remove(s, e->event);

  button_release(s);
}

void key_press(state_t *s, xcb_generic_event_t *ev) {
  xcb_key_press_event_t *e = (xcb_key_press_event_t *)ev;

  int length = sizeof(keybinds) / sizeof(keybinds[0]);
  for (int i = 0; i < length; i++) {
    if (key_cmp(s, keybinds[i], e->detail, e->state)) {
      keybinds[i].callback(s, keybinds[i].command);
    }
  }

  int valid_child = is_window_valid(s, e->child);

  if (!s->focus && e->child != e->root && valid_child) {
    client_t *cl = client_from_wid(s, e->child);
    client_focus(s, cl);
  }
}

void button_press(state_t *s, xcb_generic_event_t *ev) {
  xcb_button_press_event_t *e = (xcb_button_press_event_t *)ev;

  if (!client_contains_cursor(s, s->focus) || s->focus->fullscreen) {
    return;
  }

  uint32_t value_list[] = {XCB_STACK_MODE_ABOVE};

  xcb_configure_window(s->c, s->focus->wid, XCB_CONFIG_WINDOW_STACK_MODE,
                       value_list);

  xcb_flush(s->c);

  s->mouse->pressed_button = e->detail;
  s->mouse->root_x = e->root_x;
  s->mouse->root_y = e->root_y;
}

void button_release(state_t *s) {
  s->mouse->resizingcorner = CORNER_NONE;

  s->mouse->pressed_button = 0;

  xcb_flush(s->c);
}

void motion_notify(state_t *s, xcb_generic_event_t *ev) {
  xcb_motion_notify_event_t *e = (xcb_motion_notify_event_t *)ev;

  s->monitor_focus = monitor_contains_cursor(s);

  if (s->mouse->pressed_button == 0) {
    return;
  }

  uint32_t current_time = e->time;
  if ((current_time - s->lastmotiontime) <= 10) {
    return;
  }

  if (s->mouse->pressed_button == 1) {
    int x = s->focus->x + (e->root_x - s->mouse->root_x);
    int y = s->focus->y + (e->root_y - s->mouse->root_y);
    client_move(s, s->focus, x, y);
  } else if (s->mouse->pressed_button == 3) {
    client_resize(s, s->focus, e);
  }

  xcb_flush(s->c);

  s->mouse->root_x = e->root_x;
  s->mouse->root_y = e->root_y;
  s->lastmotiontime = current_time;
}

void enter_notify(state_t *s, xcb_generic_event_t *ev) {
  xcb_enter_notify_event_t *e = (xcb_enter_notify_event_t *)ev;

  client_t *cl = client_from_wid(s, e->event);

  client_focus(s, cl);
}

int is_window_valid(state_t *s, xcb_window_t wid) {
  xcb_get_window_attributes_cookie_t cookie =
      xcb_get_window_attributes(s->c, wid);
  xcb_get_window_attributes_reply_t *reply =
      xcb_get_window_attributes_reply(s->c, cookie, NULL);

  if (reply) {
    free(reply);
    return 1;
  } else {
    return 0;
  }
}

void send_event(state_t *s, client_t *cl, xcb_atom_t protocol) {
  int has_prot;
  xcb_icccm_get_wm_protocols_reply_t reply;

  if (xcb_icccm_get_wm_protocols_reply(
          s->c, xcb_icccm_get_wm_protocols(s->c, cl->wid, protocol), &reply,
          NULL)) {
    for (int i = 0; i < reply.atoms_len; i++) {
      if (reply.atoms[i] == protocol) {
        has_prot = 1;
        break;
      }
    }
    xcb_icccm_get_wm_protocols_reply_wipe(&reply);
  }

  if (has_prot) {
    xcb_client_message_event_t e;
    e.response_type = XCB_CLIENT_MESSAGE;
    e.window = cl->wid;
    e.format = 32;
    e.type = s->wm_protocols_atom;
    e.data.data32[0] = protocol;
    e.data.data32[1] = XCB_TIME_CURRENT_TIME;
    xcb_send_event(s->c, 0, cl->wid, XCB_EVENT_MASK_NO_EVENT, (const char *)&e);
  }
}
