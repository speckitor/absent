#include "layout.h"
#include "config.h"
#include "types.h"

void make_layout(state_t *s) {
  if (!s->monitor_focus) {
    return;
  }

  client_t *cl = s->clients;
  monitor_t *mon = s->monitor_focus;

  int length = 0;

  while (cl) {
    if (cl->monitor == mon && !cl->floating && !cl->fullscreen) {
      length++;
    }
    cl = cl->next;
  }

  if (length == 0) {
    return;
  }

  switch (mon->layout) {
  case MAIN_TILED:
    main_tiled(s, length);
    break;
  case VERTICAL:
    main_tiled(s, length);
    break;
  case HORIZONTAL:
    main_tiled(s, length);
    break;
  }
}

void main_tiled(state_t *s, int length) {
  client_t *cl = s->clients;
  int i, mw, ty, x, y, w, h;

  mw = length > 1 ? s->monitor_focus->width * MAIN_WINDOW_AREA -
                        (LAYOUT_GAP / 2) - SCREEN_GAP
                  : s->monitor_focus->width - (2 * SCREEN_GAP);
  ty = SCREEN_GAP + 1;

  for (i = 0, cl = next_tiled(s, cl); cl; cl = next_tiled(s, cl->next), i++) {
    if (i < 1) {
      x = s->monitor_focus->x + SCREEN_GAP;
      y = s->monitor_focus->y + ty;
      w = mw - (2 * BORDER_WIDTH);
      h = s->monitor_focus->height - (2 * BORDER_WIDTH) - (2 * SCREEN_GAP) - 1;
      client_move_resize(s, cl, x, y, w, h);
    } else {
      x = s->monitor_focus->x + mw + LAYOUT_GAP + SCREEN_GAP;
      y = s->monitor_focus->y + ty;
      w = s->monitor_focus->width - mw - LAYOUT_GAP - (2 * SCREEN_GAP) -
          (2 * BORDER_WIDTH);
      h = (s->monitor_focus->height - ty - SCREEN_GAP) / (length - i) -
          (2 * BORDER_WIDTH) - 1;
      client_move_resize(s, cl, x, y, w, h);
      ty += cl->height + 2 * BORDER_WIDTH + LAYOUT_GAP;
    }
  }
}

client_t *next_tiled(state_t *s, client_t *cl) {
  while (cl &&
         (cl->floating || cl->fullscreen || s->monitor_focus != cl->monitor)) {
    cl = cl->next;
  }
  return cl;
}

void client_move_resize(state_t *s, client_t *cl, int x, int y, int width,
                        int height) {
  cl->x = x;
  cl->y = y;
  cl->width = width;
  cl->height = height;

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

void swap_clients(state_t *s, client_t *cl1, client_t *cl2) {
  if (cl1 == cl2) {
    return;
  }

  client_t *prev1, *prev2, *tmp;
  prev1 = NULL;
  prev2 = NULL;
  tmp = s->clients;

  while (tmp && tmp != cl1) {
    prev1 = tmp;
    tmp = tmp->next;
  }
  if (!tmp) {
    return;
  }

  tmp = s->clients;
  while (tmp && tmp != cl2) {
    prev2 = tmp;
    tmp = tmp->next;
  }
  if (!tmp) {
    return;
  }

  if (prev1) {
    prev1->next = cl2;
  } else {
    s->clients = cl2;
  }

  if (prev2) {
    prev2->next = cl1;
  } else {
    s->clients = cl1;
  }

  tmp = cl1->next;
  cl1->next = cl2->next;
  cl2->next = tmp;
}
