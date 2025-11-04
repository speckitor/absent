#ifndef LOGS_H_
#define LOGS_H_

#include "types.h"

int log_init(state_t *s, const char *path);
void log_close(state_t *s);
void log_msg(state_t *s, const char *fmt, ...);

#endif // LOGS_H_
