#ifndef ABSENT_CONFIG_H
#define ABSENT_CONFIG_H

#include <X11/keysym.h>

#include "keycallbackfuncs.h"
#include "types.h"

// there are MAIN_TILED, VERTICAL and HORIZONTAL layouts
#define DEFAULT_LAYOUT MAIN_TILED

// gap between root window and windows layout
#define SCREEN_GAP 10

// gap between windows in layout
#define LAYOUT_GAP 5

// floating number that specifies how tall or wide should be main window
#define MAIN_WINDOW_AREA 0.55

// new window opens as main window
#define SET_NEW_WINDOW_MAIN 0

#define MIN_WINDOW_WIDTH 100
#define MIN_WINDOW_HEIGHT 100

#define BORDER_WIDTH 1
#define FOCUSED_BORDER_COLOR 0xf38ba8
#define UNFOCUSED_BORDER_COLOR 0x9399b2

#define ALT XCB_MOD_MASK_1
#define SUPER XCB_MOD_MASK_4
#define SHIFT XCB_MOD_MASK_SHIFT
#define CONTROL XCB_MOD_MASK_CONTROL

#define BUTTON_MOD SUPER

// enable autostart script running (autostartabsent)
// 1 for enable, 0 for disable
#define ENABLE_AUTOSTART 1

static keybind_t keybinds[] = {{XK_Return, SUPER, spawnclient, "kitty"},
                               {XK_v, SUPER, spawnclient, "vesktop"},
                               {XK_f, SUPER, spawnclient, "firefox"},
                               {XK_space, ALT, spawnclient, "rofi -show drun"},
                               {XK_j, SUPER, cyclefocusdown, NULL},
                               {XK_k, SUPER, cyclefocusup, NULL},
                               {XK_t, SUPER, settiled, NULL},
                               {XK_s, SUPER, swapmainfocus, NULL},
                               {XK_j, SUPER | SHIFT, swapfocusdown, NULL},
                               {XK_k, SUPER | SHIFT, swapfocusup, NULL},
                               {XK_c, SUPER, destroyclient, NULL},
                               {XK_c, SUPER | SHIFT, killclient, NULL},
                               {XK_Escape, SUPER, killwm, NULL},
                               {XK_f, SUPER | SHIFT, fullscreen, NULL}};
#endif
