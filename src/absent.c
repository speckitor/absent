#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <xcb/xcb.h>
#include <xcb/xcb_cursor.h>
#include <xcb/xproto.h>

#include "../config.h"
#include "absent.h"
#include "desktops.h"
#include "events.h"
#include "keys.h"
#include "monitors.h"
#include "types.h"

void setup(state_t *s) {
  s->c = xcb_connect(NULL, NULL);

  if (xcb_connection_has_error(s->c)) {
    exit(EXIT_FAILURE);
  }
  s->screen = xcb_setup_roots_iterator(xcb_get_setup(s->c)).data;
  s->root = s->screen->root;

  s->clients = NULL;
  s->focus = NULL;

  s->monitors = NULL;
  s->monitor_focus = NULL;

  s->number_desktops = 0;

  s->lastmotiontime = 0.0;
  s->mouse = calloc(1, sizeof(mouse_t));
  s->mouse->pressed_button = 0;
  s->mouse->root_x = 0;
  s->mouse->root_y = 0;
  s->mouse->resizingcorner = CORNER_NONE;

  setup_atoms(s);

  size_t length = sizeof(keybinds) / sizeof(keybinds[0]);
  for (int i = 0; i < length; i++) {
    xcb_keycode_t *keycode = get_keycode(s, keybinds[i].key);
    if (keycode) {
      xcb_grab_key(s->c, 0, s->root, keybinds[i].mod, *keycode,
                   XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC);
    }
  }

  xcb_cursor_context_t *ctx;
  xcb_cursor_context_new(s->c, s->screen, &ctx);
  xcb_cursor_t cursor = xcb_cursor_load_cursor(ctx, "left_ptr");
  xcb_change_window_attributes(s->c, s->root, XCB_CW_CURSOR, &cursor);
  xcb_cursor_context_free(ctx);

  uint32_t value_list[] = {
      XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT | XCB_EVENT_MASK_STRUCTURE_NOTIFY |
      XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY | XCB_EVENT_MASK_ENTER_WINDOW |
      XCB_EVENT_MASK_FOCUS_CHANGE | XCB_EVENT_MASK_BUTTON_PRESS |
      XCB_EVENT_MASK_BUTTON_RELEASE | XCB_EVENT_MASK_POINTER_MOTION};
  xcb_change_window_attributes_checked(s->c, s->root, XCB_CW_EVENT_MASK,
                                       value_list);

  value_list[0] = s->screen->black_pixel;

  xcb_change_window_attributes(s->c, s->root, XCB_CW_BACK_PIXEL, value_list);

  xcb_set_input_focus(s->c, XCB_INPUT_FOCUS_POINTER_ROOT, s->root,
                      XCB_CURRENT_TIME);

  if (ENABLE_AUTOSTART) {
    if (fork() == 0) {
      execl("/bin/sh", "sh", "-c", "autostartabsent", (char *)NULL);
      _exit(EXIT_FAILURE);
    }
  }

  monitors_setup(s);
  s->monitor_focus = monitor_contains_cursor(s);

  xcb_change_property(
      s->c, XCB_PROP_MODE_REPLACE, s->root, s->ewmh[EWMH_CURRENT_DESKTOP],
      XCB_ATOM_CARDINAL, 32, 1,
      &s->monitor_focus->desktops[s->monitor_focus->desktop_idx].desktop_id);

  xcb_change_property(s->c, XCB_PROP_MODE_REPLACE, s->root,
                      s->ewmh[EWMH_NUMBER_OF_DESKTOPS], XCB_ATOM_CARDINAL, 32,
                      1, &s->number_desktops);

  xcb_flush(s->c);
}

xcb_atom_t get_atom(state_t *s, char *name) {
  xcb_intern_atom_reply_t *atom_reply = xcb_intern_atom_reply(
      s->c, xcb_intern_atom(s->c, 0, strlen(name), name), NULL);
  xcb_atom_t atom = atom_reply ? atom_reply->atom : XCB_ATOM_NONE;
  free(atom_reply);
  return atom;
}

void setup_atoms(state_t *s) {
  s->icccm[ICCCM_PROTOCOLS] = get_atom(s, "WM_PROTOCOLS");
  s->icccm[ICCCM_DELETE_WINDOW] = get_atom(s, "WM_DELETE_WINDOW");
  s->icccm[ICCCM_TAKE_FOCUS] = get_atom(s, "WM_TAKE_FOCUS");

  s->ewmh[EWMH_SUPPORTED] = get_atom(s, "_NET_WM_SUPPORTED");
  s->ewmh[EWMH_CLIENT_LIST] = get_atom(s, "_NET_CLIENT_LIST");
  s->ewmh[EWMH_CURRENT_DESKTOP] = get_atom(s, "_NET_CURRENT_DESKTOP");
  s->ewmh[EWMH_NUMBER_OF_DESKTOPS] = get_atom(s, "_NET_NUMBER_OF_DESKTOPS");
  s->ewmh[EWMH_DESKTOP_NAMES] = get_atom(s, "_NET_DESKTOP_NAMES");
  s->ewmh[EWMH_NAME] = get_atom(s, "_NET_WM_NAME");
  s->ewmh[EWMH_ACTIVE_WINDOW] = get_atom(s, "_NET_ACTIVE_WINDOW");
  s->ewmh[EWMH_STATE] = get_atom(s, "_NET_WM_STATE");
  s->ewmh[EWMH_FULLSCREEN] = get_atom(s, "_NET_WM_STATE_FULLSCREEN");
  s->ewmh[EWMH_WINDOW_TYPE] = get_atom(s, "_NET_WM_WINDOW_TYPE");
  s->ewmh[EWMH_WINDOW_TYPE_DESKTOP] = get_atom(s, "_NET_WM_WINDOW_TYPE_DESKTOP");
  s->ewmh[EWMH_WINDOW_TYPE_UTILITY] =
      get_atom(s, "_NET_WM_WINDOW_TYPE_UTILITY");
  s->ewmh[EWMH_WINDOW_TYPE_SPLASH] = get_atom(s, "_NET_WM_WINDOW_TYPE_SPLASH");
  s->ewmh[EWMH_WINDOW_TYPE_MENU] = get_atom(s, "_NET_WM_WINDOW_TYPE_MENU");
  s->ewmh[EWMH_WINDOW_TYPE_TOOLBAR] =
      get_atom(s, "_NET_WM_WINDOW_TYPE_TOOLBAR");
  s->ewmh[EWMH_WINDOW_TYPE_NORMAL] = get_atom(s, "_NET_WM_WINDOW_TYPE_NORMAL");
  s->ewmh[EWMH_WINODW_TYPE_DIALOG] = get_atom(s, "_NET_WM_WINDOW_TYPE_DIALOG");
  s->ewmh[EWMH_WINDOW_TYPE_DOCK] = get_atom(s, "_NET_WM_WINDOW_TYPE_DOCK");
  s->ewmh[EWMH_STRUT_PARTIAL] = get_atom(s, "_NET_WM_STRUT_PARTIAL");
  s->ewmh[EWMH_CHECK] = get_atom(s, "_NET_SUPPORTING_WM_CHECK");

  xcb_window_t checkwid = xcb_generate_id(s->c);
  xcb_create_window(s->c, XCB_COPY_FROM_PARENT, checkwid, s->root, 0, 0, 1, 1,
                    0, XCB_WINDOW_CLASS_INPUT_ONLY, XCB_COPY_FROM_PARENT, 0,
                    NULL);

  xcb_change_property(s->c, XCB_PROP_MODE_REPLACE, checkwid,
                      s->ewmh[EWMH_CHECK], XCB_ATOM_WINDOW, 32, 1, &checkwid);

  xcb_change_property(s->c, XCB_PROP_MODE_REPLACE, checkwid, s->ewmh[EWMH_NAME],
                      get_atom(s, "UTF8_STRING"), 8, strlen("absent"),
                      "absent");

  xcb_change_property(s->c, XCB_PROP_MODE_REPLACE, s->root, s->ewmh[EWMH_CHECK],
                      XCB_ATOM_WINDOW, 32, 1, &checkwid);

  xcb_change_property(s->c, XCB_PROP_MODE_REPLACE, s->root,
                      s->ewmh[EWMH_SUPPORTED], XCB_ATOM_WINDOW, 32,
                      EWMH_COUNT_ATOMS, s->ewmh);

  xcb_delete_property(s->c, s->root, s->ewmh[EWMH_CLIENT_LIST]);

  xcb_flush(s->c);
}

void clean(state_t *s) { 
  free(s->mouse); 
  client_t *cl = s->clients; 
  client_t *next;

  while (cl) {
    next = cl->next;
    free(cl);
    cl = next;
  }

  monitor_t *monitors = s->monitors;
  monitor_t *next_monitor;

  while (monitors) {
    next_monitor = monitors->next;
    free(monitors->desktops);
    free(monitors);
    monitors = next_monitor;
  }

  free(s);
}

int main() {
  state_t *state = calloc(1, sizeof(state_t));

  setup(state);

  main_loop(state);

  xcb_disconnect(state->c);

  clean(state);

  return 0;
}
