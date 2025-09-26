#pragma once

#include <stdbool.h>
#include <stdio.h>

#include <xcb/xcb.h>
#include <xcb/xproto.h>

typedef struct state_t state_t;

typedef void (*keycallback_t)(state_t *s, const char *param);

typedef struct {
    unsigned long key;
    uint16_t mods;
    keycallback_t callback;
    char *param;
} keybind_t;

typedef struct {
    char *monitor_name;
    char *desktop_names[10];
} desktop_config_t;

typedef enum {
    TILED = 0,
    RTILED,
    VTILED,
    RVTILED,
    VERTICAL,
    HORIZONTAL,
    LAYOUTS_NUMBER
} layout_t;

typedef struct {
    const char *name;
    unsigned long keysym;
} keyname_keysym_t;

typedef struct {
    const char *name;
    int button;
} buttonname_buttonnum_t;

typedef struct {
    const char *name;
    uint16_t mod;
} modname_mod_t;

typedef struct {
    const char *name;
    layout_t layout;
} layoutname_layout_t;

typedef struct {
    const char *name;
    keycallback_t callback;
} cbname_cb_t;

typedef struct {
    char *autostart;

    layout_t default_layout;
    int screen_gap;
    int layout_gap;
    double main_window_area;
    int set_new_window_main;

    int min_window_width;
    int min_window_height;
    int border_width;
    int focused_border_color;
    int unfocused_border_color;

    int move_window_step;
    uint16_t button_mod;
    int move_button;
    int resize_button;
    int pointer_update_time;

    desktop_config_t desktops[8];
    keybind_t keybinds[256];
} wm_config_t;

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

    double main_window_area;

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
    EWMH_WINDOW_TYPE_DIALOG,
    EWMH_WINDOW_TYPE_DOCK,
    EWMH_STRUT_PARTIAL,
    EWMH_CHECK,
    EWMH_COUNT_ATOMS
} ewmh_atoms_t;

struct state_t {
    FILE *log;

    wm_config_t *config;

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

    xcb_timestamp_t lastkeypresstime;
    xcb_timestamp_t lastmotiontime;
    mouse_t *mouse;
};
