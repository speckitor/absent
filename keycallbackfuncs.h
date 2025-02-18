#ifndef ABSENT_KEYCALLBACKFUNCS_H
#define ABSENT_KEYCALLBACKFUNCS_H

#include "types.h"

void spawnclient(state_t *s, const char *command);
void cycleclients(state_t *s, const char *command);
void cycleclientsback(state_t *s, const char *command);
void settiled(state_t *s, const char *command);
void setmaintiled(state_t *s, const char *command);
void destroyclient(state_t *s, const char *command);
void killclient(state_t *s, const char *command);
void fullscreen(state_t *s, const char *command);
void killwm(state_t *s, const char *command);

#endif
