#include "desktops.h"
#include "absent.h"
#include "clients.h"
#include "layout.h"
#include "types.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <xcb/xcb.h>
#include <xcb/xproto.h>

void setup_desktop_names(state_t *s, monitor_t *mon) {
  size_t length = 0;
  for (int i = 0; i < mon->number_desktops; i++) {
    length += strlen(mon->desktops[i].name) + 1;
  }

  char *names = malloc(length);
  char *ptr = names;
  for (int i = 0; i < mon->number_desktops; i++) {
    strcpy(ptr, mon->desktops[i].name);
    ptr += strlen(mon->desktops[i].name) + 1;
  }

  xcb_change_property(s->c, XCB_PROP_MODE_APPEND, s->root,
                      s->ewmh[EWMH_DESKTOP_NAMES], get_atom(s, "UTF8_STRING"),
                      8, length, names);

  free(names);
}

void switch_desktop(state_t *s, const char *name) {
  monitor_t *mon = s->monitor_focus;
  int desktop_idx = -1;
  int desktop_id = -1;
  for (int i = 0; i < mon->number_desktops; i++) {
    if (strcmp(mon->desktops[i].name, name) == 0 && mon->desktop_idx != i) {
      desktop_idx = i;
      desktop_id = mon->desktops[i].desktop_id;
      break;
    }
  }

  if (desktop_id == -1) {
    mon = s->monitors;
    while (mon) {
      if (mon == s->monitor_focus) {
        mon = mon->next;
        continue;
      }
      for (int i = 0; i < mon->number_desktops; i++) {
        if (strcmp(mon->desktops[i].name, name) == 0) {
          desktop_idx = i;
          desktop_id = mon->desktops[i].desktop_id;
          break;
        }
      }
      if (desktop_id != -1) {
        break;
      }
      mon = mon->next;
    }
  }

  if (desktop_id == -1) {
    return;
  }

  client_unfocus(s);

  client_t *cl = s->clients;

  while (cl) {
    if (cl->monitor == mon && cl->desktop_idx == mon->desktop_idx &&
        !cl->hidden && cl->desktop_idx != desktop_idx) {
      hide_client(s, cl);
    } else if (cl->monitor == mon && cl->desktop_idx == desktop_idx &&
               cl->hidden) {
      show_client(s, cl);
    }
    cl = cl->next;
  }

  if (s->monitor_focus != mon) {
    s->monitor_focus = mon;
    xcb_warp_pointer(s->c, XCB_NONE, s->root, 0, 0, 0, 0,
                     mon->x + mon->width / 2, mon->y + mon->height / 2);
  }

  mon->desktop_idx = desktop_idx;
  xcb_change_property(s->c, XCB_PROP_MODE_REPLACE, s->root,
                      s->ewmh[EWMH_CURRENT_DESKTOP], XCB_ATOM_CARDINAL, 32, 1,
                      &desktop_id);
  make_layout(s);
  xcb_flush(s->c);
}

void switch_desktop_by_idx(state_t *s, int desktop_id) {
  monitor_t *mon = s->monitors;
  int desktop_idx = -1;
  
  while (mon) {
    for (int i = 0; i < mon->number_desktops; i++) {
      if (mon->desktops[i].desktop_id == desktop_id) {
        desktop_idx = i;
        break;  
      }
    }
    
    if (desktop_idx != -1) {
      break;
    }
    mon = mon->next;
  }

  if (desktop_idx == -1) {
    return;
  }

  client_unfocus(s);

  client_t *cl = s->clients;

  while (cl) {
    if (cl->monitor == mon && cl->desktop_idx == mon->desktop_idx &&
        !cl->hidden && cl->desktop_idx != desktop_idx) {
      hide_client(s, cl);
    } else if (cl->monitor == mon && cl->desktop_idx == desktop_idx &&
               cl->hidden) {
      show_client(s, cl);
    }
    cl = cl->next;
  }

  if (s->monitor_focus != mon) {
    s->monitor_focus = mon;
    xcb_warp_pointer(s->c, XCB_NONE, s->root, 0, 0, 0, 0,
                     mon->x + mon->width / 2, mon->y + mon->height / 2);
  }

  mon->desktop_idx = desktop_idx;
  xcb_change_property(s->c, XCB_PROP_MODE_REPLACE, s->root,
                      s->ewmh[EWMH_CURRENT_DESKTOP], XCB_ATOM_CARDINAL, 32, 1,
                      &desktop_id);
  make_layout(s);
  xcb_flush(s->c);
}

void client_move_to_desktop(state_t *s, const char *name) {
  monitor_t *mon = s->monitor_focus;
  int desktop_idx = -1;
  int desktop_id = -1;
  for (int i = 0; i < mon->number_desktops; i++) {
    if (strcmp(mon->desktops[i].name, name) == 0 && mon->desktop_idx != i) {
      desktop_idx = i;
      desktop_id = mon->desktops[i].desktop_id;
      break;
    }
  }

  if (desktop_id == -1) {
    mon = s->monitors;
    while (mon) {
      if (mon == s->monitor_focus) {
        mon = mon->next;
        continue;
      }
      for (int i = 0; i < mon->number_desktops; i++) {
        if (strcmp(mon->desktops[i].name, name) == 0) {
          desktop_idx = i;
          desktop_id = mon->desktops[i].desktop_id;
          break;
        }
      }
      if (desktop_id != -1) {
        break;
      }
      mon = mon->next;
    }
  }

  if (desktop_id == -1) {
    return;
  }

  s->focus->monitor = mon;
  s->focus->desktop_idx = desktop_idx;

  if (mon->desktop_idx != desktop_idx) {
    hide_client(s, s->focus);
  }

  make_layout(s);

  if (mon != s->monitor_focus) {
    if (s->focus->floating) {
      int x, y;
      x = s->focus->x - s->monitor_focus->x + mon->x;
      y = s->focus->y - s->monitor_focus->y + mon->y;
      client_move_resize(s, s->focus, x, y, s->focus->width, s->focus->height);
    } else if (s->focus->fullscreen) {
      client_move_resize(s, s->focus, mon->x, mon->y, mon->width, mon->height);
    }

    mon = s->monitor_focus;
    s->monitor_focus = s->focus->monitor;
    make_layout(s);
    s->monitor_focus = mon;
  }

  client_unfocus(s);

  xcb_flush(s->c);
}

void hide_client(state_t *s, client_t *cl) {
  cl->hidden = 1;
  xcb_unmap_window(s->c, cl->wid);
}

void show_client(state_t *s, client_t *cl) {
  cl->hidden = 0;
  xcb_map_window(s->c, cl->wid);
}
