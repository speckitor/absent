#include "layout.h"
#include "clients.h"
#include "config.h"
#include "monitors.h"
#include "types.h"

void make_layout(state_t *s) {
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
  s->monitor_focus = monitor_contains_cursor(s);

  client_t *cl = s->clients;
  int i, mw, h, ty;

  mw = length > 1 ? s->monitor_focus->width / 2 : s->monitor_focus->width;

  for (i = ty = 0, cl = next_tiled(cl); cl; cl = next_tiled(cl->next), i++) {
    if (i < 1) {
      client_move_resize(s, cl, s->monitor_focus->x, s->monitor_focus->y,
                         mw - (2 * BORDER_WIDTH),
                         s->monitor_focus->height - (2 * BORDER_WIDTH));
    } else {
      h = (s->monitor_focus->height - ty) / (length - i);
      client_move_resize(s, cl, s->monitor_focus->x + mw,
                         s->monitor_focus->y + ty,
                         s->monitor_focus->width - (2 * BORDER_WIDTH) - mw,
                         h - (2 * BORDER_WIDTH));
      ty += cl->height + 2 * BORDER_WIDTH;
    }
  }
}

client_t *next_tiled(client_t *cl) {
  while (cl && (cl->floating || cl->fullscreen)) {
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
}
