#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <xcb/xproto.h>

#include "clients.h"
#include "config.h"
#include "events.h"
#include "layout.h"
#include "monitors.h"
#include "types.h"

void client_create(state_t *s, xcb_window_t wid) {
  s->monitor_focus = monitor_contains_cursor(s);

  client_t *cl = calloc(1, sizeof(client_t));

  cl->wid = wid;

  cl->fullscreen = 0;
  cl->floating = 0;

  cl->monitor = s->monitor_focus;

  client_t *clients = s->clients;

  if (!clients) {
    cl->next = s->clients;
    s->clients = cl;
  } else {
    while (clients->next) {
      clients = clients->next;
    }
    clients->next = cl;
    cl->next = NULL;
  }

  client_set_size_hints(s, cl);

  cl->x = s->monitor_focus->x;
  cl->y = s->monitor_focus->y;
  cl->width = s->monitor_focus->width;
  cl->height = s->monitor_focus->height;

  uint32_t value_list[5];
  uint32_t value_mask = XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y |
                        XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT |
                        XCB_CONFIG_WINDOW_BORDER_WIDTH;
  value_list[0] = cl->x;
  value_list[1] = cl->y;
  value_list[2] = cl->width;
  value_list[3] = cl->height;
  value_list[4] = BORDER_WIDTH;
  xcb_configure_window(s->c, wid, value_mask, value_list);

  value_list[0] = UNFOCUSED_BORDER_COLOR;
  xcb_change_window_attributes(s->c, wid, XCB_CW_BORDER_PIXEL, value_list);

  xcb_generic_error_t *error;

  xcb_get_property_cookie_t cookie = xcb_get_property(
      s->c, 0, wid, s->ewmh[EWMH_STATE], XCB_ATOM_ATOM, 0, sizeof(xcb_atom_t));
  xcb_get_property_reply_t *reply =
      xcb_get_property_reply(s->c, cookie, &error);

  if (reply) {
    if (reply->type == XCB_ATOM_ATOM && reply->format == 32 &&
        reply->value_len > 0) {
      xcb_atom_t state = *(xcb_atom_t *)xcb_get_property_value(reply);
      if (state == s->ewmh[EWMH_FULLSCREEN]) {
        client_fullscreen(s, cl, 1);
      }
    }
    free(reply);
  }

  if (error) {
    free(error);
  }

  xcb_map_window(s->c, wid);

  grab_buttons(s, cl);

  clients_update_ewmh(s);

  make_layout(s);

  if (client_contains_cursor(s, cl)) {
    client_focus(s, cl);
  }

  xcb_flush(s->c);
}

void client_set_size_hints(state_t *s, client_t *cl) {
  xcb_size_hints_t size_hints;

  xcb_get_property_cookie_t cookie =
      xcb_icccm_get_wm_normal_hints(s->c, cl->wid);

  if (xcb_icccm_get_wm_normal_hints_reply(s->c, cookie, &size_hints, NULL)) {
    if (size_hints.flags & XCB_ICCCM_SIZE_HINT_P_MIN_SIZE) {
      cl->size_hints.min_width = size_hints.min_width;
      cl->size_hints.min_height = size_hints.min_height;
    } else {
      cl->size_hints.min_width = MIN_WINDOW_WIDTH;
      cl->size_hints.min_height = MIN_WINDOW_HEIGHT;
    }

    if (size_hints.flags & XCB_ICCCM_SIZE_HINT_P_MAX_SIZE) {
      cl->size_hints.max_width = size_hints.max_width;
      cl->size_hints.max_height = size_hints.max_height;
    } else {
      cl->size_hints.max_width = 10000;
      cl->size_hints.max_height = 10000;
    }
  } else {
    cl->size_hints.min_width = MIN_WINDOW_WIDTH;
    cl->size_hints.min_height = MIN_WINDOW_HEIGHT;
    cl->size_hints.max_width = 10000;
    cl->size_hints.max_height = 10000;
  }
}

void client_kill(state_t *s, client_t *cl) {
  client_unfocus(s);

  int has_del_atom = 0;
  xcb_icccm_get_wm_protocols_reply_t reply;
  if (xcb_icccm_get_wm_protocols_reply(
          s->c,
          xcb_icccm_get_wm_protocols_unchecked(s->c, cl->wid,
                                               s->icccm[ICCCM_PROTOCOLS]),
          &reply, NULL)) {
    for (int i = 0; i < reply.atoms_len; i++) {
      if (reply.atoms[i] == s->icccm[ICCCM_DELETE_WINDOW]) {
        has_del_atom = 1;
        break;
      }
    }
    xcb_icccm_get_wm_protocols_reply_wipe(&reply);
  }

  if (has_del_atom) {
    xcb_client_message_event_t e;
    e.response_type = XCB_CLIENT_MESSAGE;
    e.window = cl->wid;
    e.format = 32;
    e.type = s->icccm[ICCCM_PROTOCOLS];
    e.data.data32[0] = s->icccm[ICCCM_DELETE_WINDOW];
    e.data.data32[1] = XCB_TIME_CURRENT_TIME;
    xcb_send_event(s->c, 0, cl->wid, XCB_EVENT_MASK_NO_EVENT, (const char *)&e);
  } else {
    xcb_kill_client(s->c, cl->wid);
  }

  xcb_flush(s->c);
}

void client_remove(state_t *s, xcb_window_t wid) {
  client_t *clients = s->clients;
  client_t **prev = &s->clients;

  if (!clients) {
    return;
  }

  while (clients) {
    if (clients->wid == wid) {
      *prev = clients->next;
      free(clients);
      return;
    }
    prev = &clients->next;
    clients = clients->next;
  }
}

client_t *client_from_wid(state_t *s, xcb_window_t wid) {
  client_t *cl = s->clients;

  while (cl != NULL) {
    if (cl->wid == wid) {
      break;
    }
    cl = cl->next;
  }

  return cl;
}

void client_move(state_t *s, client_t *cl, int x, int y) {
  uint32_t value_mask = XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y;
  uint32_t value_list[] = {x, y};
  xcb_configure_window(s->c, cl->wid, value_mask, value_list);

  cl->x = x;
  cl->y = y;

  cl->monitor = monitor_contains_cursor(s);

  client_configure(s, cl);
}

void client_apply_size(state_t *s, client_t *cl, int x, int y, int width,
                       int height) {

  x = width > cl->size_hints.min_width ? x : cl->x;
  y = height > cl->size_hints.min_height ? y : cl->y;
  width = width > cl->size_hints.min_width ? width : cl->width;
  height = height > cl->size_hints.min_height ? height : cl->height;

  cl->x = width < cl->size_hints.max_width ? x : cl->x;
  cl->y = height < cl->size_hints.max_height ? y : cl->y;
  cl->width = width < cl->size_hints.max_width ? width : cl->width;
  cl->height = height < cl->size_hints.max_height ? height : cl->height;

  uint32_t value_mask;
  uint32_t value_list[4];

  value_mask = XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y |
               XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT;
  value_list[0] = cl->x;
  value_list[1] = cl->y;
  value_list[2] = cl->width;
  value_list[3] = cl->height;
  xcb_configure_window(s->c, cl->wid, value_mask, value_list);

  client_configure(s, cl);

  xcb_flush(s->c);
}

void client_resize(state_t *s, client_t *cl, xcb_motion_notify_event_t *e) {
  int half_window_width = cl->width / 2;
  int half_window_height = cl->height / 2;

  int relative_x = s->mouse->root_x - cl->x;
  int relative_y = s->mouse->root_y - cl->y;

  if (s->mouse->resizingcorner == CORNER_NONE) {
    if (relative_x >= half_window_width && relative_y >= half_window_height) {
      s->mouse->resizingcorner = BOTTOM_RIGHT;
    } else if (relative_x >= half_window_width &&
               relative_y < half_window_height) {
      s->mouse->resizingcorner = TOP_RIGHT;
    } else if (relative_x < half_window_width &&
               relative_y >= half_window_height) {
      s->mouse->resizingcorner = BOTTOM_LEFT;
    } else {
      s->mouse->resizingcorner = TOP_LEFT;
    }
  }

  int new_x, new_y, new_width, new_height;

  switch (s->mouse->resizingcorner) {
  case CORNER_NONE:
    return;
  case BOTTOM_RIGHT:
    new_x = cl->x;
    new_y = cl->y;
    new_width = cl->width + (e->root_x - s->mouse->root_x);
    new_height = cl->height + (e->root_y - s->mouse->root_y);
    break;
  case TOP_RIGHT:
    new_x = cl->x;
    new_y = cl->y + (e->root_y - s->mouse->root_y);
    new_width = cl->width + (e->root_x - s->mouse->root_x);
    new_height = cl->height - (e->root_y - s->mouse->root_y);
    break;
  case BOTTOM_LEFT:
    new_x = cl->x + (e->root_x - s->mouse->root_x);
    new_y = cl->y;
    new_width = cl->width - (e->root_x - s->mouse->root_x);
    new_height = cl->height + (e->root_y - s->mouse->root_y);
    break;
  case TOP_LEFT:
    new_x = cl->x + (e->root_x - s->mouse->root_x);
    new_y = cl->y + (e->root_y - s->mouse->root_y);
    new_width = cl->width - (e->root_x - s->mouse->root_x);
    new_height = cl->height - (e->root_y - s->mouse->root_y);
    break;
  }

  client_apply_size(s, cl, new_x, new_y, new_width, new_height);
}

void client_fullscreen(state_t *s, client_t *cl, int fullscreen) {
  if (!cl) {
    return;
  }

  if (!fullscreen) {
    xcb_change_property(s->c, XCB_PROP_MODE_REPLACE, cl->wid,
                        s->ewmh[EWMH_FULLSCREEN], XCB_ATOM_ATOM, 32, 1,
                        &s->ewmh[EWMH_FULLSCREEN]);

    uint32_t value_mask = XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y |
                          XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT |
                          XCB_CONFIG_WINDOW_BORDER_WIDTH;
    uint32_t value_list[] = {cl->oldx, cl->oldy, cl->oldwidth, cl->oldheight,
                             BORDER_WIDTH};
    xcb_configure_window(s->c, cl->wid, value_mask, value_list);
    xcb_flush(s->c);

    cl->x = cl->oldx;
    cl->y = cl->oldy;
    cl->width = cl->oldwidth;
    cl->height = cl->oldheight;

    cl->fullscreen = 0;
  } else {
    xcb_change_property(s->c, XCB_PROP_MODE_REPLACE, cl->wid,
                        s->ewmh[EWMH_FULLSCREEN], XCB_ATOM_ATOM, 32, 0, 0);

    uint32_t value_mask = XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y |
                          XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT |
                          XCB_CONFIG_WINDOW_BORDER_WIDTH;
    uint32_t value_list[] = {cl->monitor->x, cl->monitor->y, cl->monitor->width,
                             cl->monitor->height, 0};
    xcb_configure_window(s->c, cl->wid, value_mask, value_list);
    xcb_flush(s->c);

    cl->oldx = cl->x;
    cl->oldy = cl->y;
    cl->oldwidth = cl->width;
    cl->oldheight = cl->height;

    cl->x = cl->monitor->x;
    cl->y = cl->monitor->y;
    cl->width = cl->monitor->width;
    cl->height = cl->monitor->height;

    cl->fullscreen = 1;
  }

  client_configure(s, cl);
}

void client_configure(state_t *s, client_t *cl) {
  if (!cl) {
    return;
  }

  xcb_configure_notify_event_t e;

  e.response_type = XCB_CONFIGURE_NOTIFY;
  e.event = cl->wid;
  e.window = cl->wid;
  e.x = cl->x;
  e.y = cl->y;
  e.width = cl->width;
  e.height = cl->height;
  e.border_width = BORDER_WIDTH;
  e.above_sibling = XCB_NONE;
  e.override_redirect = 0;

  xcb_send_event(s->c, 0, cl->wid, XCB_EVENT_MASK_STRUCTURE_NOTIFY,
                 (const char *)&e);
}

void client_unfocus(state_t *s) {
  if (!s->focus) {
    return;
  }

  uint32_t value_list[] = {UNFOCUSED_BORDER_COLOR};
  xcb_change_window_attributes(s->c, s->focus->wid, XCB_CW_BORDER_PIXEL,
                               value_list);

  xcb_set_input_focus(s->c, XCB_INPUT_FOCUS_POINTER_ROOT, s->root,
                      XCB_CURRENT_TIME);

  xcb_delete_property(s->c, s->root, s->ewmh[EWMH_ACTIVE_WINDOW]);

  xcb_flush(s->c);

  client_t *cl = s->focus;
  s->focus = NULL;

  grab_buttons(s, cl);
}

void client_focus(state_t *s, client_t *cl) {
  if (!cl) {
    return;
  }

  if (s->focus) {
    client_unfocus(s);
  }

  s->monitor_focus = cl->monitor;
  s->focus = cl;

  xcb_set_input_focus(s->c, XCB_INPUT_FOCUS_POINTER_ROOT, cl->wid,
                      XCB_CURRENT_TIME);

  xcb_change_property(s->c, XCB_PROP_MODE_REPLACE, cl->wid,
                      s->ewmh[EWMH_ACTIVE_WINDOW], XCB_ATOM_WINDOW, 32, 1,
                      &cl->wid);

  send_event(s, cl, s->icccm[ICCCM_TAKE_FOCUS]);

  uint32_t value_list[] = {FOCUSED_BORDER_COLOR};
  xcb_change_window_attributes(s->c, cl->wid, XCB_CW_BORDER_PIXEL, value_list);

  value_list[0] = XCB_STACK_MODE_ABOVE;
  xcb_configure_window(s->c, s->focus->wid, XCB_CONFIG_WINDOW_STACK_MODE,
                       value_list);

  grab_buttons(s, s->focus);

  xcb_flush(s->c);
}

void grab_buttons(state_t *s, client_t *cl) {
  if (!cl) {
    return;
  }

  xcb_ungrab_button(s->c, XCB_BUTTON_INDEX_ANY, cl->wid, XCB_MOD_MASK_ANY);

  if (!s->focus || s->focus != cl) {
    xcb_grab_button(s->c, 0, cl->wid, XCB_EVENT_MASK_BUTTON_PRESS,
                    XCB_GRAB_MODE_SYNC, XCB_GRAB_MODE_ASYNC, XCB_NONE, XCB_NONE,
                    XCB_BUTTON_INDEX_ANY, XCB_NONE);
    xcb_grab_button(s->c, 0, cl->wid,
                    XCB_EVENT_MASK_BUTTON_PRESS |
                        XCB_EVENT_MASK_BUTTON_RELEASE |
                        XCB_EVENT_MASK_BUTTON_MOTION,
                    XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC, XCB_NONE,
                    XCB_NONE, 1, BUTTON_MOD);
    xcb_grab_button(s->c, 0, cl->wid,
                    XCB_EVENT_MASK_BUTTON_PRESS |
                        XCB_EVENT_MASK_BUTTON_RELEASE |
                        XCB_EVENT_MASK_BUTTON_MOTION,
                    XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC, XCB_NONE,
                    XCB_NONE, 3, BUTTON_MOD);
  } else {
    xcb_grab_button(s->c, 0, cl->wid,
                    XCB_EVENT_MASK_BUTTON_PRESS |
                        XCB_EVENT_MASK_BUTTON_RELEASE |
                        XCB_EVENT_MASK_BUTTON_MOTION,
                    XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC, XCB_NONE,
                    XCB_NONE, 1, BUTTON_MOD);
    xcb_grab_button(s->c, 0, cl->wid,
                    XCB_EVENT_MASK_BUTTON_PRESS |
                        XCB_EVENT_MASK_BUTTON_RELEASE |
                        XCB_EVENT_MASK_BUTTON_MOTION,
                    XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC, XCB_NONE,
                    XCB_NONE, 3, BUTTON_MOD);
  }
}

int client_contains_cursor(state_t *s, client_t *cl) {
  xcb_query_pointer_reply_t *reply =
      xcb_query_pointer_reply(s->c, xcb_query_pointer(s->c, s->root), NULL);

  int ret = 0;

  if (reply) {
    if (reply->root_x >= cl->x && reply->root_y >= cl->y &&
        reply->root_x <= cl->x + cl->width &&
        reply->root_y <= cl->y + cl->height)
      ret = 1;
  }

  free(reply);
  return ret;
}

void clients_update_ewmh(state_t *s) {
  xcb_delete_property(s->c, s->root, s->ewmh[EWMH_CLIENT_LIST]);

  client_t *cl = s->clients;
  while (cl) {
    xcb_change_property(s->c, XCB_PROP_MODE_APPEND, s->root,
                        s->ewmh[EWMH_CLIENT_LIST], XCB_ATOM_WINDOW, 32, 1,
                        &cl->wid);
    cl = cl->next;
  }
}
