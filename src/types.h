#ifndef ABSENT_TYPES_H
#define ABSENT_TYPES_H

#include <stdbool.h>

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

typedef struct {
    const char monitor_name[16];
    const char *desktop_names[10];
} desktop_config_t;

typedef enum { 
    TILED = 0, 
    VERTICAL, 
    HORIZONTAL, 
    PSEUDOFULLSCREEN,
    LAYOUTS_NUMBER 
} layout_t;

typedef struct monitor_t monitor_t;

typedef struct client_t client_t;

typedef struct {
    int desktop_id;
    char name[16];
    layout_t layout;
} desktop_t;

typedef struct {
    int left, right, top, bottom;
} padding_t;

struct monitor_t {
    desktop_t *desktops;
    int desktop_idx;
    int number_desktops;

    padding_t padding;
    int x, y;
    int width, height;

    monitor_t *next;
};

typedef struct {
    int min_width, min_height;
    int max_width, max_height;
} size_hints_t;

struct client_t {
    xcb_window_t wid;

    int oldx, oldy;
    int oldwidth, oldheight;

    int x, y;
    int width, height;

    size_hints_t size_hints;

    bool fullscreen, floating, hidden;

    monitor_t *monitor;
    int desktop_idx;

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
    EWMH_CURRENT_DESKTOP,
    EWMH_NUMBER_OF_DESKTOPS,
    EWMH_DESKTOP_NAMES,
    EWMH_NAME,
    EWMH_ACTIVE_WINDOW,
    EWMH_STATE,
    EWMH_FULLSCREEN,
    EWMH_WINDOW_TYPE,
    EWMH_WINDOW_TYPE_DESKTOP,
    EWMH_WINDOW_TYPE_UTILITY,
    EWMH_WINDOW_TYPE_SPLASH,
    EWMH_WINDOW_TYPE_MENU,
    EWMH_WINDOW_TYPE_TOOLBAR,
    EWMH_WINDOW_TYPE_NORMAL,
    EWMH_WINODW_TYPE_DIALOG,
    EWMH_WINDOW_TYPE_DOCK,
    EWMH_STRUT_PARTIAL,
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

    int number_desktops;

    float lastmotiontime;
    mouse_t *mouse;
};

#endif
