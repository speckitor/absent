#ifndef ABSENT_CONFIG_H
#define ABSENT_CONFIG_H

#include <X11/keysym.h>

#include "keycallbacks.h"
#include "types.h"

// there are TILED, VERTICAL and HORIZONTAL layouts
#define DEFAULT_LAYOUT TILED

// gap between root window and windows layout
#define SCREEN_GAP 0

// gap between windows in layout
#define LAYOUT_GAP 0

// floating number that specifies how tall or wide should be main window
#define MAIN_WINDOW_AREA 0.5

// new window opens as main window
#define SET_NEW_WINDOW_MAIN 0

#define MIN_WINDOW_WIDTH 100
#define MIN_WINDOW_HEIGHT 100

#define BORDER_WIDTH 1
#define FOCUSED_BORDER_COLOR 0xf38ba8
#define UNFOCUSED_BORDER_COLOR 0x9399b2

#define NOMOD XCB_NONE
#define ALT XCB_MOD_MASK_1
#define SUPER XCB_MOD_MASK_4
#define SHIFT XCB_MOD_MASK_SHIFT
#define CONTROL XCB_MOD_MASK_CONTROL

#define BUTTON_MOD SUPER

// enable autostart script running (autostartabsent)
// 1 for enable, 0 for disable
#define ENABLE_AUTOSTART 1

static desktop_config_t desktops[] = {
    {"eDP-1", {"I", "II", "III", "IV", "V"}},
    {"HDMI-1", {"VI", "VII", "VIII", "IX", "X"}},
};

static keybind_t keybinds[] = {
    {XK_Return, SUPER, run, "kitty"},
    {XK_v, SUPER, run, "vesktop"},
    {XK_f, SUPER, run, "firefox"},
    {XK_space, ALT, run, "rofi -show drun"},
    {XK_Print, NOMOD, run,
     "flameshot gui -r | xclip -selection clipboard -t image/png"},
    {XK_j, SUPER, cyclefocusdown, NULL},
    {XK_k, SUPER, cyclefocusup, NULL},
    {XK_t, SUPER | SHIFT, setlayout, "TILED"},
    {XK_v, SUPER | SHIFT, setlayout, "VERTICAL"},
    {XK_h, SUPER | SHIFT, setlayout, "HORIZONTAL"},
    {XK_t, SUPER, settiled, NULL},
    {XK_s, SUPER, swapmainfocus, NULL},
    {XK_j, SUPER | SHIFT, swapfocusdown, NULL},
    {XK_k, SUPER | SHIFT, swapfocusup, NULL},
    {XK_c, SUPER, destroyclient, NULL},
    {XK_c, SUPER | SHIFT, killclient, NULL},
    {XK_Escape, SUPER, killwm, NULL},
    {XK_f, SUPER | SHIFT, fullscreen, NULL},
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
