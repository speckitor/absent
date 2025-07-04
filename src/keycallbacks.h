#pragma once

#include <xcb/xcb.h>
#include <xcb/xproto.h>

#include "types.h"

void run(state_t *s, const char *command);

void cyclefocusdown(state_t *s, const char *command);
void cyclefocusup(state_t *s, const char *command);

void setcurrentdesktop(state_t *s, const char *command);
void movefocustodesktop(state_t *s, const char *command);

void setlayout(state_t *s, const char *command);

void setfocustiled(state_t *s, const char *command);
void setfocusfullscreen(state_t *s, const char *command);

void movefocusdir(state_t *s, const char *command);

void swapmainfocus(state_t *s, const char *command);
void swapfocusdown(state_t *s, const char *command);
void swapfocusup(state_t *s, const char *command);

void destroyclient(state_t *s, const char *command);
void killclient(state_t *s, const char *command);
void killwm(state_t *s, const char *command);
