#include <stdlib.h>
#include <xcb/xcb_icccm.h>

#include "clients.h"
#include "config.h"

void client_create(state_t *s, xcb_window_t wid) {
  xcb_map_window(s->c, wid);

  uint32_t value_list[2];

  value_list[0] = BORDER_WIDTH;
  xcb_configure_window(s->c, wid, XCB_CONFIG_WINDOW_BORDER_WIDTH, value_list);

  value_list[0] = UNFOCUSED_BORDER_COLOR;
  value_list[1] = XCB_EVENT_MASK_ENTER_WINDOW;
  xcb_change_window_attributes(
      s->c, wid, XCB_CW_BORDER_PIXEL | XCB_CW_EVENT_MASK, value_list);

  xcb_get_geometry_reply_t *attrs =
      xcb_get_geometry_reply(s->c, xcb_get_geometry(s->c, wid), NULL);

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

  xcb_flush(s->c);

  if (!attrs) {
    return;
  }

  client_t *cl = calloc(1, sizeof(client_t));

  cl->wid = wid;
  cl->x = attrs->x;
  cl->y = attrs->y;
  cl->width = attrs->width;
  cl->height = attrs->height;
  cl->fullscreen = 0;
  cl->next = s->clients;

  s->clients = cl;

  if (client_contains_cursor(s, cl) || !s->focus) {
    client_focus(s, cl);
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

void client_remove(state_t *s, client_t *cl) {
  client_t *clients = s->clients;
  client_t **prev = &s->clients;

  if (!clients) {
    return;
  }

  while (clients) {
    if (clients->wid == cl->wid) {
      *prev = cl->next;
      free(cl);
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
}

void client_resize(state_t *s, client_t *cl, xcb_motion_notify_event_t *e) {
  int half_window_width = cl->width / 2;
  int half_window_height = cl->height / 2;

  int relative_x = s->m->root_x - cl->x;
  int relative_y = s->m->root_y - cl->y;

  if (s->m->resizingcorner == CORNER_NONE) {
    if (relative_x >= half_window_width && relative_y >= half_window_height) {
      s->m->resizingcorner = BOTTOM_RIGHT;
    } else if (relative_x >= half_window_width &&
               relative_y < half_window_height) {
      s->m->resizingcorner = TOP_RIGHT;
    } else if (relative_x < half_window_width &&
               relative_y >= half_window_height) {
      s->m->resizingcorner = BOTTOM_LEFT;
    } else {
      s->m->resizingcorner = TOP_LEFT;
    }
  }

  int new_width, new_height, new_x, new_y;

  uint32_t value_mask;
  uint32_t value_list[4];
  switch (s->m->resizingcorner) {
  case BOTTOM_RIGHT:
    new_width = cl->width + (e->root_x - s->m->root_x);
    new_height = cl->height + (e->root_y - s->m->root_y);

    new_width = new_width > MIN_WIDTH ? new_width : MIN_WIDTH;
    new_height = new_height > MIN_HEIGHT ? new_height : MIN_HEIGHT;

    cl->width = new_width;
    cl->height = new_height;

    value_mask = XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT;
    value_list[0] = new_width;
    value_list[1] = new_height;
    xcb_configure_window(s->c, cl->wid, value_mask, value_list);
    break;
  case TOP_RIGHT:
    new_width = cl->width + (e->root_x - s->m->root_x);
    new_height = cl->height - (e->root_y - s->m->root_y);
    new_y = cl->y + (e->root_y - s->m->root_y);

    new_width = new_width > MIN_WIDTH ? new_width : MIN_WIDTH;
    new_height = new_height > MIN_HEIGHT ? new_height : MIN_HEIGHT;
    new_y = new_height > MIN_HEIGHT ? new_y : cl->y;

    cl->width = new_width;
    cl->height = new_height;
    cl->y = new_y;

    value_mask = XCB_CONFIG_WINDOW_Y | XCB_CONFIG_WINDOW_WIDTH |
                 XCB_CONFIG_WINDOW_HEIGHT;
    value_list[0] = new_y;
    value_list[1] = new_width;
    value_list[2] = new_height;
    xcb_configure_window(s->c, cl->wid, value_mask, value_list);
    break;
  case BOTTOM_LEFT:
    new_width = cl->width - (e->root_x - s->m->root_x);
    new_height = cl->height + (e->root_y - s->m->root_y);
    new_x = cl->x + (e->root_x - s->m->root_x);

    new_width = new_width > MIN_WIDTH ? new_width : MIN_WIDTH;
    new_height = new_height > MIN_HEIGHT ? new_height : MIN_HEIGHT;
    new_x = new_width > MIN_WIDTH ? new_x : cl->x;

    cl->width = new_width;
    cl->height = new_height;
    cl->x = new_x;

    value_mask = XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_WIDTH |
                 XCB_CONFIG_WINDOW_HEIGHT;
    value_list[0] = new_x;
    value_list[1] = new_width;
    value_list[2] = new_height;
    xcb_configure_window(s->c, cl->wid, value_mask, value_list);
    break;
  case TOP_LEFT:
    new_width = cl->width - (e->root_x - s->m->root_x);
    new_height = cl->height - (e->root_y - s->m->root_y);
    new_x = cl->x + (e->root_x - s->m->root_x);
    new_y = cl->y + (e->root_y - s->m->root_y);

    new_width = new_width > MIN_WIDTH ? new_width : MIN_WIDTH;
    new_height = new_height > MIN_HEIGHT ? new_height : MIN_HEIGHT;
    new_x = new_width > MIN_WIDTH ? new_x : cl->x;
    new_y = new_height > MIN_HEIGHT ? new_y : cl->y;

    cl->width = new_width;
    cl->height = new_height;
    cl->x = new_x;
    cl->y = new_y;

    value_mask = XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y |
                 XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT;
    value_list[0] = new_x;
    value_list[1] = new_y;
    value_list[2] = new_width;
    value_list[3] = new_height;
    xcb_configure_window(s->c, cl->wid, value_mask, value_list);
    break;
  case CORNER_NONE:
    break;
  }

  xcb_flush(s->c);
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

  uint32_t value_list[] = {FOCUSED_BORDER_COLOR};
  xcb_change_window_attributes(s->c, cl->wid, XCB_CW_BORDER_PIXEL, value_list);
  xcb_set_input_focus(s->c, XCB_INPUT_FOCUS_POINTER_ROOT, cl->wid,
                      XCB_CURRENT_TIME);
  xcb_flush(s->c);

  s->focus = cl;
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
