#ifndef ABSENT_CONFIG_H
#define ABSENT_CONFIG_H

#include <X11/keysym.h>

#include "keycallbackfuncs.h"
#include "types.h"

// there are MAIN_TILED, VERTICAL and HORIZONTAL layouts
#define DEFAULT_LAYOUT MAIN_TILED

// gap between root window and windows layout
#define WINDOWS_SCREEN_GAP 0

// gap between windows in layout
#define WINDOWS_LAYOUT_GAP 0

#define MIN_WINDOW_WIDTH 100
#define MIN_WINDOW_HEIGHT 100

#define BORDER_WIDTH 3
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
                               {XK_j, SUPER, cycleclients, NULL},
                               {XK_k, SUPER, cycleclientsback, NULL},
                               {XK_t, SUPER, settiled, NULL},
                               {XK_c, SUPER, destroyclient, NULL},
                               {XK_c, SUPER | SHIFT, killclient, NULL},
                               {XK_Escape, SUPER, killwm, NULL},
                               {XK_f, SUPER | SHIFT, fullscreen, NULL}};
#endif
