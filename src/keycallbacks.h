#ifndef KEYCALLBACKS_H_
#define KEYCALLBACKS_H_

#include "types.h"

void run(state_t *s, const char *param);

void cyclefocusdown(state_t *s, const char *param);
void cyclefocusup(state_t *s, const char *param);

void setcurrentdesktop(state_t *s, const char *param);
void movefocustodesktop(state_t *s, const char *param);

void setlayout(state_t *s, const char *param);

void setfocustiled(state_t *s, const char *param);
void setfocusfullscreen(state_t *s, const char *param);

void resizemainwindow(state_t *s, const char *param);
void swapmainfocus(state_t *s, const char *param);
void swapfocusdown(state_t *s, const char *param);
void swapfocusup(state_t *s, const char *param);

void destroyclient(state_t *s, const char *param);
void killclient(state_t *s, const char *param);

void killwm(state_t *s, const char *param);
void restartwm(state_t *s, const char *param);

#endif // KEYCALLBACKS_H_
