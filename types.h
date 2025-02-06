#ifndef ABSENT_TYPES_H
#define ABSENT_TYPES_H

#include <xcb/xcb.h>

typedef struct state_t state_t;

typedef void (*keycallback_t)(state_t *s, const char *command);

typedef struct {
  unsigned long key;
  uint16_t mod;
  keycallback_t callback;
  const char *command;
} keybind_t;

typedef struct monitor_t monitor_t;

struct monitor_t {
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

typedef struct client_t client_t;

struct client_t {
  xcb_window_t wid;

  int x, y;
  int width, height;

  size_hints_t size_hints;

  int fullscreen;

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

struct state_t {
  xcb_connection_t *c;
  xcb_screen_t *screen;
  xcb_window_t root;

  xcb_atom_t wm_protocols_atom;
  xcb_atom_t wm_delete_window_atom;
  xcb_atom_t wm_take_focus_atom;

  client_t *clients;
  client_t *focus;

  monitor_t *monitors;
  monitor_t *monitor_focus;

  float lastmotiontime;
  mouse_t *mouse;
};

#endif
