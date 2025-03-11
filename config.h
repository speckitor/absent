#ifndef ABSENT_CONFIG_H
#define ABSENT_CONFIG_H

#include <X11/keysym.h>

#include "src/keycallbacks.h"
#include "src/types.h"

// use 1 to enable, 0 to disable

// there are TILED, VERTICAL, HORIZONTAL and PSEUDOFULLSCREEN layouts
#define DEFAULT_LAYOUT TILED
// gap between screen edges and windows
#define SCREEN_GAP 10

// gap between windows in layout
#define LAYOUT_GAP 5

// floating number that specifies how tall or wide should be main window
// (currently only in tiled layout)
#define MAIN_WINDOW_AREA 0.5

// new window opens as main layout window
#define SET_NEW_WINDOW_MAIN 0

// specify the min size for windows which doesn't do it on its own
#define MIN_WINDOW_WIDTH 100
#define MIN_WINDOW_HEIGHT 100

// windows border settings
// width in pixels, colors in format: 0x(hex_color)
#define BORDER_WIDTH 1
#define FOCUSED_BORDER_COLOR 0xf38ba8
#define UNFOCUSED_BORDER_COLOR 0x9399b2

// modifiers
#define NOMOD XCB_NONE
#define ALT XCB_MOD_MASK_1
#define SUPER XCB_MOD_MASK_4
#define SHIFT XCB_MOD_MASK_SHIFT
#define CONTROL XCB_MOD_MASK_CONTROL

// modifiers you will use to move/resize windows with left/right mouse buttons
#define BUTTON_MOD SUPER

// enable autostart script running (autostartabsent)
#define ENABLE_AUTOSTART 1

// specifies desktops for each monitor, check monitors list with "xrandr"
// maximum number of desktops for one monitor is 10
// the default desktop names for monitor is numbers from 1 to 10
static const desktop_config_t desktops[] = {
    {"DP-1", {"I", "II", "III", "IV", "V"}},
    {"DP-2", {"VI", "VII", "VIII", "IX", "X"}},
};

// specifies keybinds
// available functions:
// run - execute command
// cyclefocusdown, cyclefocusup - change focused window
// setlayout - change layout
// setfocustiled - add focused window to layout if it's floating
// setfocusfullscreen - enable/disable fullscreen mode for fucosed window
// swapmainfocus - swap focused window with main window in layout
// destroyclient - kill focused window
// killclient - kill focused window process
// killwm - kill window manager
// setcurrentdesktop - change current desktop
// movefocustodesktop - moves focused window to chosen desktop
static const keybind_t keybinds[] = {
    {XK_Return, SUPER, run, "kitty"},
    {XK_b, SUPER, run, "chromium"},
    {XK_j, SUPER, cyclefocusdown, NULL},
    {XK_k, SUPER, cyclefocusup, NULL},
    {XK_t, SUPER | SHIFT, setlayout, "TILED"},
    {XK_v, SUPER | SHIFT, setlayout, "VERTICAL"},
    {XK_h, SUPER | SHIFT, setlayout, "HORIZONTAL"},
    {XK_p, SUPER | SHIFT, setlayout, "PSEUDOFULLSCREEN"},
    {XK_t, SUPER, setfocustiled, NULL},
    {XK_f, SUPER, setfocusfullscreen, NULL},
    {XK_s, SUPER, swapmainfocus, NULL},
    {XK_j, SUPER | SHIFT, swapfocusdown, NULL},
    {XK_k, SUPER | SHIFT, swapfocusup, NULL},
    {XK_c, SUPER, destroyclient, NULL},
    {XK_c, SUPER | SHIFT, killclient, NULL},
    {XK_Escape, SUPER, killwm, NULL},
    {XK_1, SUPER, setcurrentdesktop, "I"},
    {XK_2, SUPER, setcurrentdesktop, "II"},
    {XK_3, SUPER, setcurrentdesktop, "III"},
    {XK_4, SUPER, setcurrentdesktop, "IV"},
    {XK_5, SUPER, setcurrentdesktop, "V"},
    {XK_6, SUPER, setcurrentdesktop, "VI"},
    {XK_7, SUPER, setcurrentdesktop, "VII"},
    {XK_8, SUPER, setcurrentdesktop, "VIII"},
    {XK_9, SUPER, setcurrentdesktop, "IX"},
    {XK_0, SUPER, setcurrentdesktop, "X"},
    {XK_1, SUPER | SHIFT, movefocustodesktop, "I"},
    {XK_2, SUPER | SHIFT, movefocustodesktop, "II"},
    {XK_3, SUPER | SHIFT, movefocustodesktop, "III"},
    {XK_4, SUPER | SHIFT, movefocustodesktop, "IV"},
    {XK_5, SUPER | SHIFT, movefocustodesktop, "V"},
    {XK_6, SUPER | SHIFT, movefocustodesktop, "VI"},
    {XK_7, SUPER | SHIFT, movefocustodesktop, "VII"},
    {XK_8, SUPER | SHIFT, movefocustodesktop, "VIII"},
    {XK_9, SUPER | SHIFT, movefocustodesktop, "IX"},
    {XK_0, SUPER | SHIFT, movefocustodesktop, "X"},
};
#endif
