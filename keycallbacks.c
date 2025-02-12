#include <stdlib.h>
#include <unistd.h>

#include "absent.h"
#include "clients.h"
#include "config.h"

void spawnclient(state_t *s, const char *command) {
  if (fork() == 0) {
    execl("/bin/sh", "sh", "-c", command, (char *)NULL);
    _exit(EXIT_FAILURE);
  }
}

void cycleclients(state_t *s, const char *command) {
  if (s->clients) {
    uint32_t value_list[] = {XCB_STACK_MODE_ABOVE};
    if (!s->focus || !s->focus->next) {
      xcb_configure_window(s->c, s->clients->wid, XCB_CONFIG_WINDOW_STACK_MODE,
                           value_list);
      client_focus(s, s->clients);
    } else {
      xcb_configure_window(s->c, s->focus->next->wid,
                           XCB_CONFIG_WINDOW_STACK_MODE, value_list);
      client_focus(s, s->focus->next);
    }
  }
}

void cycleclientsback(state_t *s, const char *command) {
  if (s->clients) {
    uint32_t value_list[] = {XCB_STACK_MODE_ABOVE};
    if (!s->focus) {
      xcb_configure_window(s->c, s->clients->wid, XCB_CONFIG_WINDOW_STACK_MODE,
                           value_list);
      client_focus(s, s->clients);
    } else {
      client_t *clients = s->clients->next;
      client_t *prev = s->clients;

      if (prev->wid == s->focus->wid) {
        while (clients) {
          if (!clients->next) {
            xcb_configure_window(s->c, clients->wid,
                                 XCB_CONFIG_WINDOW_STACK_MODE, value_list);
            client_focus(s, clients);
            return;
          }
          clients = clients->next;
        }
      }

      while (clients) {
        if (clients->wid == s->focus->wid) {
          xcb_configure_window(s->c, prev->wid, XCB_CONFIG_WINDOW_STACK_MODE,
                               value_list);
          client_focus(s, prev);
          return;
        }
        prev = clients;
        clients = clients->next;
      }
    }
  }
}

void raiseclient(state_t *s, const char *command) {
  if (s->focus) {
    uint32_t value_list[] = {XCB_STACK_MODE_ABOVE};
    xcb_configure_window(s->c, s->focus->wid, XCB_CONFIG_WINDOW_STACK_MODE,
                         value_list);
    xcb_flush(s->c);
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
