#ifndef ABSENT_KEYCALLBACKFUNCS_H
#define ABSENT_KEYCALLBACKFUNCS_H

#include "types.h"

void run(state_t *s, const char *command);
void cyclefocusdown(state_t *s, const char *command);
void cyclefocusup(state_t *s, const char *command);
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
