#ifndef ABSENT_KEYCALLBACKS_H
#define ABSENT_KEYCALLBACKS_H

#include <xcb/xcb.h>
#include <xcb/xproto.h>

#include "types.h"

static char *layout_names[LAYOUTS_NUMBER] = {
    [TILED] = "TILED",
    [VERTICAL] = "VERTICAL",
    [HORIZONTAL] = "HORIZONTAL",
    [PSEUDOFULLSCREEN] = "PSEUDOFULLSCREEN",
};

void run(state_t *s, const char *command);
void cyclefocusdown(state_t *s, const char *command);
void cyclefocusup(state_t *s, const char *command);
void setcurrentdesktop(state_t *s, const char *command);
void movefocustodesktop(state_t *s, const char *command);
void setlayout(state_t *s, const char *command);
void settiled(state_t *s, const char *command);
void swapmainfocus(state_t *s, const char *command);
void swapfocusdown(state_t *s, const char *command);
void swapfocusup(state_t *s, const char *command);
void destroyclient(state_t *s, const char *command);
void killclient(state_t *s, const char *command);
void fullscreen(state_t *s, const char *command);
void killwm(state_t *s, const char *command);

#endif
