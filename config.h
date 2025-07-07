#ifndef ABSENT_CONFIG_H
#define ABSENT_CONFIG_H

#include <X11/keysym.h>

#include "src/keycallbacks.h"
#include "src/types.h"

/* use 1 to enable, 0 to disable */

/* there are TILED, RTILED, VERTICAL, HORIZONTAL layouts */
#define DEFAULT_LAYOUT RTILED

/* gap between screen edges and windows */
#define SCREEN_GAP 10

/* gap between windows in layout */
#define LAYOUT_GAP 5

/*
 * floating-point number that specifies how tall or wide should be main window
 * (currently only in tiled layout)
 */
#define MAIN_WINDOW_AREA 0.5

/* new window opens as main layout window */
#define SET_NEW_WINDOW_MAIN 0

/* specify how many pixels window moves with moving keybinds */
#define MOVE_WINODOW_STEP 150

/* specify the min size for windows which doesn't do it on its own */
#define MIN_WINDOW_WIDTH 100
#define MIN_WINDOW_HEIGHT 100

/*
 * windows border settings
 * width in pixels, colors in format: 0x(hex_color)
 */
#define BORDER_WIDTH 1
#define FOCUSED_BORDER_COLOR 0xf38ba8
#define UNFOCUSED_BORDER_COLOR 0x9399b2

/* modifiers */
#define NOMOD XCB_NONE
#define ALT XCB_MOD_MASK_1
#define SUPER XCB_MOD_MASK_4
#define SHIFT XCB_MOD_MASK_SHIFT
#define CONTROL XCB_MOD_MASK_CONTROL

/* modifiers you will use to move/resize windows with left/right mouse buttons */
#define BUTTON_MOD SUPER

/* enable autostart script running (autostartabsent) */
#define ENABLE_AUTOSTART 1

/* set time for updating pointer position (in milliseconds) */
#define POINTER_UPDATE_TIME 10

/*
 * specifies desktops for each monitor, check monitors list with "xrandr"
 * maximum number of desktops for one monitor is 10
 * the default desktop names for monitor is numbers from 1 to 10
 */
static const desktop_config_t desktops[] = {
    {"DP-1", {"1", "2", "3", "4", "5"}},
    {"DP-2", {"6", "7", "8", "9", "10"}},
};

/*
 * specifies keybinds
 * available functions:
 * run - execute command
 * cyclefocusdown, cyclefocusup - change focused window
 * setlayout - change layout
 * setfocustiled - add focused window to layout if it's floating
 * setfocusfullscreen - enable/disable fullscreen mode for fucosed window
 * movefocusdir - move focus window in 1 of 4 directions (LEFT, RIGHT, UP, DOWN)
 * swapmainfocus - swap focused window with main window in layout
 * swapfocusdown, swapfocusup - swap focused window with next/previous window in layout
 * destroyclient - kill focused window
 * killclient - kill focused window process
 * killwm - kill window manager
 * setcurrentdesktop - change current desktop
 * movefocustodesktop - moves focused window to chosen desktop
 */
static const keybind_t keybinds[] = {
    {XK_Return, SUPER, run, "alacritty"},
    {XK_b, SUPER, run, "firefox"},
    {XK_r, SUPER, run, "rofi -show drun"},
    {XK_j, SUPER, cyclefocusdown, NULL},
    {XK_k, SUPER, cyclefocusup, NULL},
    {XK_t, SUPER | SHIFT, setlayout, "TILED"},
    {XK_r, SUPER | SHIFT, setlayout, "RTILED"},
    {XK_v, SUPER | SHIFT, setlayout, "VERTICAL"},
    {XK_h, SUPER | SHIFT, setlayout, "HORIZONTAL"},
    {XK_t, SUPER, setfocustiled, NULL},
    {XK_f, SUPER, setfocusfullscreen, NULL},
    {XK_h, ALT, movefocusdir, "LEFT"},
    {XK_j, ALT, movefocusdir, "DOWN"},
    {XK_k, ALT, movefocusdir, "UP"},
    {XK_l, ALT, movefocusdir, "RIGHT"},
    {XK_s, SUPER, swapmainfocus, NULL},
    {XK_j, SUPER | SHIFT, swapfocusdown, NULL},
    {XK_k, SUPER | SHIFT, swapfocusup, NULL},
    {XK_c, SUPER, destroyclient, NULL},
    {XK_c, SUPER | SHIFT, killclient, NULL},
    {XK_Escape, SUPER, killwm, NULL},
    {XK_1, SUPER, setcurrentdesktop, "1"},
    {XK_2, SUPER, setcurrentdesktop, "2"},
    {XK_3, SUPER, setcurrentdesktop, "3"},
    {XK_4, SUPER, setcurrentdesktop, "4"},
    {XK_5, SUPER, setcurrentdesktop, "5"},
    {XK_6, SUPER, setcurrentdesktop, "6"},
    {XK_7, SUPER, setcurrentdesktop, "7"},
    {XK_8, SUPER, setcurrentdesktop, "8"},
    {XK_9, SUPER, setcurrentdesktop, "9"},
    {XK_0, SUPER, setcurrentdesktop, "10"},
    {XK_1, SUPER | SHIFT, movefocustodesktop, "1"},
    {XK_2, SUPER | SHIFT, movefocustodesktop, "2"},
    {XK_3, SUPER | SHIFT, movefocustodesktop, "3"},
    {XK_4, SUPER | SHIFT, movefocustodesktop, "4"},
    {XK_5, SUPER | SHIFT, movefocustodesktop, "5"},
    {XK_6, SUPER | SHIFT, movefocustodesktop, "6"},
    {XK_7, SUPER | SHIFT, movefocustodesktop, "7"},
    {XK_8, SUPER | SHIFT, movefocustodesktop, "8"},
    {XK_9, SUPER | SHIFT, movefocustodesktop, "9"},
    {XK_0, SUPER | SHIFT, movefocustodesktop, "10"},
};
#endif
