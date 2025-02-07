#include <stdio.h>
#include <stdlib.h>

#include <xcb/xcb.h>
#include <xcb/xcb_icccm.h>
#include <xcb/xproto.h>

#include "clients.h"
#include "config.h"
#include "events.h"
#include "monitors.h"

void client_create(state_t *s, xcb_window_t wid) {
  xcb_map_window(s->c, wid);

  s->monitor_focus = monitor_contains_cursor(s);

  xcb_get_geometry_reply_t *attrs =
      xcb_get_geometry_reply(s->c, xcb_get_geometry(s->c, wid), NULL);

  client_t *cl = calloc(1, sizeof(client_t));

  cl->wid = wid;

  cl->fullscreen = 0;

  cl->monitor = s->monitor_focus;

  cl->next = s->clients;
  s->clients = cl;

  client_set_size_hints(s, cl);
  cl->width = attrs->width;
  cl->height = attrs->height;
  cl->x = s->monitor_focus->x + (s->monitor_focus->width - cl->width) / 2;
  cl->y = s->monitor_focus->y + (s->monitor_focus->height - cl->height) / 2;

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
  value_list[1] = XCB_EVENT_MASK_ENTER_WINDOW | XCB_EVENT_MASK_FOCUS_CHANGE;
  xcb_change_window_attributes(
      s->c, wid, XCB_CW_BORDER_PIXEL | XCB_CW_EVENT_MASK, value_list);

  xcb_grab_button(s->c, 0, wid,
                  XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE |
                      XCB_EVENT_MASK_BUTTON_MOTION,
                  XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC, s->root, XCB_NONE,
                  1, BUTTON_MOD);
  xcb_grab_button(s->c, 0, wid,
                  XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE |
                      XCB_EVENT_MASK_BUTTON_MOTION,
                  XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC, s->root, XCB_NONE,
                  3, BUTTON_MOD);

  if (client_contains_cursor(s, cl) || !s->focus) {
    client_focus(s, cl);
  } else if (s->focus) {
    client_focus(s, s->focus);
  } else {
    xcb_set_input_focus(s->c, XCB_INPUT_FOCUS_POINTER_ROOT, s->root,
                        XCB_CURRENT_TIME);
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
  if (xcb_icccm_get_wm_protocols_reply(s->c,
                                       xcb_icccm_get_wm_protocols_unchecked(
                                           s->c, cl->wid, s->wm_protocols_atom),
                                       &reply, NULL)) {
    for (int i = 0; i < reply.atoms_len; i++) {
      if (reply.atoms[i] == s->wm_delete_window_atom) {
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
    e.type = s->wm_protocols_atom;
    e.data.data32[0] = s->wm_delete_window_atom;
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

void client_unfocus(state_t *s) {
  if (!s->focus) {
    return;
  }

  uint32_t value_list[] = {UNFOCUSED_BORDER_COLOR};
  xcb_change_window_attributes(s->c, s->focus->wid, XCB_CW_BORDER_PIXEL,
                               value_list);
  xcb_flush(s->c);

  s->focus = NULL;
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
  send_event(s, cl, s->wm_take_focus_atom);
  uint32_t value_list[] = {FOCUSED_BORDER_COLOR};
  xcb_change_window_attributes(s->c, cl->wid, XCB_CW_BORDER_PIXEL, value_list);
  xcb_flush(s->c);
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
