#ifndef ABSENT_TYPES_H
#define ABSENT_TYPES_H

#include <xcb/xcb.h>
#include <xcb/xproto.h>

typedef struct state_t state_t;

typedef void (*keycallback_t)(state_t *s, const char *command);

typedef struct {
  unsigned long key;
  uint16_t mod;
  keycallback_t callback;
  const char *command;
} keybind_t;

typedef enum { MAIN_TILED, VERTICAL, HORIZONTAL } layout_t;

typedef struct monitor_t monitor_t;

typedef struct client_t client_t;

struct monitor_t {
  layout_t layout;

  int x, y;
  int width, height;

  monitor_t *next;
};

typedef struct {
  int min_width;
  int min_height;
  int max_width;
  int max_height;
} size_hints_t;

struct client_t {
  xcb_window_t wid;

  int oldx, oldy;
  int oldwidth, oldheight;

  int x, y;
  int width, height;

  size_hints_t size_hints;

  int fullscreen, floating, tile;

  monitor_t *monitor;

  client_t *next;
};

typedef enum {
  CORNER_NONE,
  TOP_LEFT,
  TOP_RIGHT,
  BOTTOM_LEFT,
  BOTTOM_RIGHT
} corner_t;

typedef struct {
  int pressed_button;
  int root_x;
  int root_y;
  corner_t resizingcorner;
} mouse_t;

typedef enum {
  ICCCM_PROTOCOLS = 0,
  ICCCM_DELETE_WINDOW,
  ICCCM_TAKE_FOCUS,
  ICCCM_COUNT_ATOMS
} icccm_atoms_t;

typedef enum {
  EWMH_SUPPORTED = 0,
  EWMH_CLIENT_LIST,
  EWMH_NAME,
  EWMH_ACTIVE_WINDOW,
  EWMH_STATE,
  EWMH_FULLSCREEN,
  EWMH_WINDOW_TYPE,
  EWMH_WINDOW_TYPE_NORMAL,
  EWMH_CHECK,
  EWMH_COUNT_ATOMS
} ewmh_atoms_t;

struct state_t {
  xcb_connection_t *c;
  xcb_screen_t *screen;
  xcb_window_t root;

  xcb_atom_t icccm[ICCCM_COUNT_ATOMS];
  xcb_atom_t ewmh[EWMH_COUNT_ATOMS];

  client_t *clients;
  client_t *focus;

  monitor_t *monitors;
  monitor_t *monitor_focus;

  float lastmotiontime;
  mouse_t *mouse;

  int cursor_grabbed;
};

#endif
