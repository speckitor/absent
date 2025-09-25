#include <stdarg.h>

#include "logs.h"

int log_init(state_t *s, const char *path)
{
    s->log = fopen(path, "w");
    if (s->log) {
        setvbuf(s->log, NULL, _IONBF, 0);
        return 1;
    }
    return 0;
}

void log_close(state_t *s)
{
    if (s->log)
        fclose(s->log);
}

void log_msg(state_t *s, const char *fmt, ...)
{
    if (!s->log)
        return;

    time_t t = time(NULL);
    struct tm *tm = localtime(&t);

    fprintf(s->log, "[%02d:%02d:%02d] ", tm->tm_hour, tm->tm_min, tm->tm_sec);

    va_list args;
    va_start(args, fmt);
    vfprintf(s->log, fmt, args);
    va_end(args);

    fprintf(s->log, "\n");
}
