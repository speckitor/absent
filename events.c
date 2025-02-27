#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <xcb/xcb.h>
#include <xcb/xcb_icccm.h>
#include <xcb/xproto.h>

#include "clients.h"
#include "config.h"
#include "events.h"
#include "keys.h"
#include "layout.h"
#include "monitors.h"
#include "types.h"

void main_loop(state_t *s) {
  xcb_generic_event_t *event;
  while (s->c && !xcb_connection_has_error(s->c)) {
    event = xcb_wait_for_event(s->c);

    uint8_t event_type = event->response_type & ~0x80;

    if (event_type < XCB_LAST_EVENT && handlers[event_type]) {
      handlers[event_type](s, event);
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

  client_t *cl = client_from_wid(s, e->window);

  if (!cl || cl->hidden) {
    return;
  }

  if (cl->monitor != s->monitor_focus) {
    s->monitor_focus = cl->monitor;
  }

  xcb_unmap_window(s->c, e->window);

  client_remove(s, e->window);

  if (!cl->floating) {
    make_layout(s);
  }

  button_release(s);

  clients_update_ewmh(s);

  s->monitor_focus = monitor_contains_cursor(s);

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

void client_message(state_t *s, xcb_generic_event_t *ev) {
  xcb_client_message_event_t *e = (xcb_client_message_event_t *)ev;

  client_t *cl = client_from_wid(s, e->window);

  if (!cl) {
    return;
  }

  if (e->type == s->ewmh[EWMH_STATE]) {
    if (e->data.data32[1] == s->ewmh[EWMH_FULLSCREEN] ||
        e->data.data32[2] == s->ewmh[EWMH_FULLSCREEN]) {
      if (e->data.data32[0] == 1 ||
          (e->data.data32[0] == 2 && !cl->fullscreen)) {
        client_fullscreen(s, cl, 1);
      }
    }
  } else if (e->type == s->ewmh[EWMH_ACTIVE_WINDOW]) {
    if (s->focus != cl) {
      client_focus(s, cl);
    }
  }

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

  s->monitor_focus = monitor_contains_cursor(s);
  xcb_change_property(
      s->c, XCB_PROP_MODE_REPLACE, s->root, s->ewmh[EWMH_CURRENT_DESKTOP],
      XCB_ATOM_CARDINAL, 32, 1,
      &s->monitor_focus->desktops[s->monitor_focus->desktop_idx].desktop_id);
}

void button_press(state_t *s, xcb_generic_event_t *ev) {
  xcb_button_press_event_t *e = (xcb_button_press_event_t *)ev;

  s->monitor_focus = monitor_contains_cursor(s);
  xcb_change_property(
      s->c, XCB_PROP_MODE_REPLACE, s->root, s->ewmh[EWMH_CURRENT_DESKTOP],
      XCB_ATOM_CARDINAL, 32, 1,
      &s->monitor_focus->desktops[s->monitor_focus->desktop_idx].desktop_id);

  client_t *cl = client_from_wid(s, e->event);

  if (!cl || !client_contains_cursor(s, cl)) {
    return;
  }

  uint16_t value_list[] = {XCB_STACK_MODE_ABOVE};
  xcb_configure_window(s->c, e->event, XCB_CONFIG_WINDOW_STACK_MODE,
                       value_list);

  if (!s->focus || e->event != s->focus->wid) {
    client_focus(s, cl);
    xcb_allow_events(s->c, XCB_ALLOW_REPLAY_POINTER, XCB_CURRENT_TIME);
    xcb_flush(s->c);
  }

  if (e->state & BUTTON_MOD) {
    cl->floating = 1;
    make_layout(s);
    s->mouse->pressed_button = e->detail;
    s->mouse->root_x = e->root_x;
    s->mouse->root_y = e->root_y;

    if (s->focus->fullscreen) {
      s->focus->fullscreen = 0;
      uint32_t value_list[] = {BORDER_WIDTH};
      xcb_configure_window(s->c, s->focus->wid, XCB_CONFIG_WINDOW_BORDER_WIDTH,
                           value_list);
    }
  }
}

void button_release(state_t *s) {
  s->mouse->resizingcorner = CORNER_NONE;

  s->mouse->pressed_button = 0;

  xcb_flush(s->c);
}

void motion_notify(state_t *s, xcb_generic_event_t *ev) {
  xcb_motion_notify_event_t *e = (xcb_motion_notify_event_t *)ev;

  uint32_t current_time = e->time;
  if ((current_time - s->lastmotiontime) <= 10) {
    return;
  }

  if (s->mouse->pressed_button == 0) {
    return;
  }

  if (s->mouse->pressed_button == 1) {
    s->monitor_focus = monitor_contains_cursor(s);
    s->focus->monitor = s->monitor_focus;
    s->focus->desktop_idx = s->monitor_focus->desktop_idx;
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

void send_event(state_t *s, client_t *cl, xcb_atom_t protocol) {
  int has_prot = 0;
  xcb_icccm_get_wm_protocols_reply_t reply;

  if (xcb_icccm_get_wm_protocols_reply(
          s->c,
          xcb_icccm_get_wm_protocols(s->c, cl->wid, s->icccm[ICCCM_PROTOCOLS]),
          &reply, NULL)) {
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
    e.type = s->icccm[ICCCM_PROTOCOLS];
    e.data.data32[0] = protocol;
    e.data.data32[1] = XCB_TIME_CURRENT_TIME;
    xcb_send_event(s->c, 0, cl->wid, XCB_EVENT_MASK_NO_EVENT, (const char *)&e);
  }
}
