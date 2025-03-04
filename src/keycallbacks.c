#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "absent.h"
#include "clients.h"
#include "desktops.h"
#include "keycallbacks.h"
#include "layout.h"
#include "monitors.h"

void run(state_t *s, const char *command) {
  if (fork() == 0) {
    execl("/bin/sh", "sh", "-c", command, (char *)NULL);
    _exit(EXIT_FAILURE);
  }
}

void cyclefocusdown(state_t *s, const char *command) {
  if (s->clients) {
    client_t *cl = !s->focus || !s->focus->next ? s->clients : s->focus->next;
    client_t *next = NULL;

    while (cl) {
      if (cl->monitor == s->monitor_focus &&
          cl->desktop_idx == s->monitor_focus->desktop_idx) {
        next = cl;
        break;
      }
      cl = cl->next;
    }

    if (!next) {
      cl = s->clients;
      while (cl) {
        if (cl->monitor == s->monitor_focus &&
            cl->desktop_idx == s->monitor_focus->desktop_idx) {
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

void cyclefocusup(state_t *s, const char *command) {
  if (s->clients) {
    client_t *cl = s->clients;
    client_t *prev = NULL;
    client_t *target = NULL;

    while (cl) {
      if (cl == s->focus) {
        target = prev;
        break;
      }
      prev = cl->monitor == s->monitor_focus &&
                     cl->desktop_idx == s->monitor_focus->desktop_idx
                 ? cl
                 : prev;
      cl = cl->next;
    }

    if (!target) {
      cl = s->clients;
      while (cl) {
        if (cl->monitor == s->monitor_focus &&
            cl->desktop_idx == s->monitor_focus->desktop_idx) {
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

void setcurrentdesktop(state_t *s, const char *command) {
  if (s->monitor_focus) {
    switch_desktop(s, command);
  }
}

void movefocustodesktop(state_t *s, const char *command) {
  if (s->focus) {
    client_move_to_desktop(s, command);
  }
}

void setlayout(state_t *s, const char *command) {
  if (s->monitor_focus) {
    monitor_t *mon = s->monitor_focus;
    if (strcmp(command, "TILED") == 0) {
      mon->desktops[mon->desktop_idx].layout = TILED;
    } else if (strcmp(command, "VERTICAL") == 0) {
      mon->desktops[mon->desktop_idx].layout = VERTICAL;
    } else if (strcmp(command, "HORIZONTAL") == 0) {
      mon->desktops[mon->desktop_idx].layout = HORIZONTAL;
    }

    make_layout(s);
  }
}

void settiled(state_t *s, const char *command) {
  if (s->focus) {
    s->focus->floating = 0;
    make_layout(s);
  }
}

void swapmainfocus(state_t *s, const char *command) {
  if (s->focus && s->focus->monitor == monitor_contains_cursor(s) &&
      !s->focus->floating) {
    client_t *cl = s->clients;
    while (cl && (cl->fullscreen || cl->floating ||
                  cl->monitor != s->focus->monitor ||
                  cl->desktop_idx != s->focus->desktop_idx)) {
      cl = cl->next;
    }

    if (!cl || cl == s->focus) {
      return;
    } else {
      swap_clients(s, cl, s->focus);
      make_layout(s);
    }
  }
}

void swapfocusdown(state_t *s, const char *command) {
  if (s->focus) {
    client_t *cl = s->focus->next;
    while (cl &&
           (cl->fullscreen || cl->floating || cl->monitor != s->monitor_focus ||
            cl->desktop_idx != s->monitor_focus->desktop_idx)) {
      cl = cl->next;
    }

    if (cl) {
      swap_clients(s, cl, s->focus);
      make_layout(s);
    }
  }
}

void swapfocusup(state_t *s, const char *command) {
  if (s->focus) {
    client_t *prev = NULL;
    client_t *cl = s->clients;
    while (cl != s->focus) {
      if (!cl->fullscreen && !cl->floating && cl->monitor == s->monitor_focus &&
          cl->desktop_idx == s->monitor_focus->desktop_idx) {
        prev = cl;
      }
      cl = cl->next;
    }

    if (prev) {
      swap_clients(s, prev, s->focus);
      make_layout(s);
    }
  }
}

void destroyclient(state_t *s, const char *command) {
  if (s->focus) {
    client_t *next = client_kill_next_focus(s);

    client_kill(s, s->focus);

    if (next) {
      client_focus(s, next);
    }
  }
}

void killclient(state_t *s, const char *command) {
  if (s->focus) {
    client_t *next = client_kill_next_focus(s);

    xcb_kill_client(s->c, s->focus->wid);
    xcb_flush(s->c);

    if (next) {
      client_focus(s, next);
    }
  }
}

void fullscreen(state_t *s, const char *command) {
  if (s->focus) {
    int fullscreen = s->focus->fullscreen == 1 ? 0 : 1;
    client_fullscreen(s, s->focus, fullscreen);
  }
}

void killwm(state_t *s, const char *command) {
  xcb_disconnect(s->c);
  clean(s);
}
