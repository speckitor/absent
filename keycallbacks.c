#include <stdlib.h>
#include <unistd.h>

#include "absent.h"
#include "clients.h"
#include "config.h"
#include "layout.h"

void spawnclient(state_t *s, const char *command) {
  if (fork() == 0) {
    execl("/bin/sh", "sh", "-c", command, (char *)NULL);
    _exit(EXIT_FAILURE);
  }
}

void cycleclients(state_t *s, const char *command) {
  if (s->clients) {
    client_t *cl = !s->focus || !s->focus->next ? s->clients : s->focus->next;
    client_t *next = NULL;

    while (cl) {
      if (cl->monitor == s->monitor_focus) {
        next = cl;
        break;
      }
      cl = cl->next;
    }

    if (!next) {
      cl = s->clients;
      while (cl) {
        if (cl->monitor == s->monitor_focus) {
          next = cl;
          break;
        }
        cl = cl->next;
      }
    }

    if (next) {
      client_focus(s, next);
    }
  }
}

void cycleclientsback(state_t *s, const char *command) {
  if (s->clients) {
    client_t *cl = s->clients;
    client_t *prev = NULL;
    client_t *target = NULL;

    while (cl) {
      if (cl == s->focus) {
        target = prev;
        break;
      }
      prev = cl->monitor == s->monitor_focus ? cl : prev;
      cl = cl->next;
    }

    if (!target) {
      cl = s->clients;
      while (cl) {
        if (cl->monitor == s->monitor_focus) {
          target = cl;
        }
        cl = cl->next;
      }
    }

    if (target) {
      client_focus(s, target);
    }
  }
}

void settiled(state_t *s, const char *command) {
  if (s->focus) {
    s->focus->floating = 0;
    make_layout(s);
  }
}

void destroyclient(state_t *s, const char *command) {
  if (s->focus) {
    client_kill(s, s->focus);
  }
}

void killclient(state_t *s, const char *command) {
  if (s->focus) {
    xcb_kill_client(s->c, s->focus->wid);
    xcb_flush(s->c);
  }
}

void fullscreen(state_t *s, const char *command) {
  int fullscreen = s->focus->fullscreen == 1 ? 0 : 1;
  client_fullscreen(s, s->focus, fullscreen);
}

void killwm(state_t *s, const char *command) {
  xcb_disconnect(s->c);
  clean(s);
}
