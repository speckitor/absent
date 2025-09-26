#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "absent.h"
#include "clients.h"
#include "config.h"
#include "desktops.h"
#include "keycallbacks.h"
#include "layout.h"
#include "monitors.h"

static const char *layout_names[LAYOUTS_NUMBER] = {
    [TILED] = "Tiled",
    [RTILED] = "Rtiled",
    [VERTICAL] = "Vertical",
    [HORIZONTAL] = "Horizontal",
};

void run(state_t *s, const char *command)
{
    (void)s;

    if (fork() == 0) {
        execl("/bin/sh", "sh", "-c", command, (char *)NULL);
        _exit(EXIT_FAILURE);
    }
}

static int count_clients_on_desktop(state_t *s)
{
    int count = 0;

    if (s->clients) {
        client_t *cl = s->clients;
        while (cl) {
            if (cl->monitor == s->monitor_focus &&
                cl->desktop_idx == s->monitor_focus->desktop_idx) {
                count++;
            }
            cl = cl->next;
        }
    }

    return count;
}

void cyclefocusdown(state_t *s, const char *command)
{
    (void)command;

    if (s->clients && count_clients_on_desktop(s) > 0) {
        if (s->focus && s->focus->monitor != s->monitor_focus) {
            client_unfocus(s);
        }

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

void cyclefocusup(state_t *s, const char *command)
{
    (void)command;

    if (s->clients) {
        if (s->focus && s->focus->monitor != s->monitor_focus) {
            client_unfocus(s);
        }

        client_t *cl = s->clients;
        client_t *prev = NULL;
        client_t *target = NULL;

        while (cl) {
            if (cl == s->focus) {
                target = prev;
                break;
            }
            prev =
                cl->monitor == s->monitor_focus && cl->desktop_idx == s->monitor_focus->desktop_idx
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

void setcurrentdesktop(state_t *s, const char *command)
{
    if (s->monitor_focus) {
        switch_desktop(s, command);

        client_t *cl = s->clients;
        while (cl) {
            if (cl->monitor == s->monitor_focus &&
                cl->desktop_idx == s->monitor_focus->desktop_idx) {
                client_focus(s, cl);
                break;
            }
            cl = cl->next;
        }
    }
}

void movefocustodesktop(state_t *s, const char *command)
{
    if (s->focus) {
        client_move_to_desktop(s, command);
    }
}

void setlayout(state_t *s, const char *command)
{
    if (s->monitor_focus) {
        monitor_t *mon = s->monitor_focus;

        for (int i = 0; i < LAYOUTS_NUMBER; i++) {
            if (strcmp(command, layout_names[i]) == 0) {
                mon->desktops[mon->desktop_idx].layout = i;
                make_layout(s);
                break;
            }
        }
    }
}

void setfocustiled(state_t *s, const char *command)
{
    (void)command;

    if (s->focus && (s->focus->floating || s->focus->fullscreen)) {
        if (s->focus->floating) {
            s->focus->floating = false;

            s->focus->oldx = s->focus->x;
            s->focus->oldy = s->focus->y;
            s->focus->oldwidth = s->focus->width;
            s->focus->oldheight = s->focus->height;
        } else {
            client_fullscreen(s, s->focus, false);
        }

        make_layout(s);
    }
}

void setfocusfullscreen(state_t *s, const char *command)
{
    (void)command;

    if (s->focus) {
        int fullscreen = s->focus->fullscreen != true;
        client_fullscreen(s, s->focus, fullscreen);
    }
}

void movefocusdir(state_t *s, const char *command)
{
    (void)command;

    if (s->focus) {
        int dx = 0;
        int dy = 0;

        if (strcmp(command, "Left") == 0) {
            dx = -s->config->move_window_step;
        } else if (strcmp(command, "Right") == 0) {
            dx = s->config->move_window_step;
        } else if (strcmp(command, "Up") == 0) {
            dy = -s->config->move_window_step;
        } else if (strcmp(command, "Down") == 0) {
            dy = s->config->move_window_step;
        }

        uint32_t value_list[1] = {XCB_STACK_MODE_ABOVE};
        xcb_configure_window(s->c, s->focus->wid, XCB_CONFIG_WINDOW_STACK_MODE, value_list);

        if (s->focus->fullscreen) {
            s->focus->fullscreen = false;
            xcb_change_property(s->c, XCB_PROP_MODE_REPLACE, s->focus->wid,
                                s->ewmh[EWMH_FULLSCREEN], XCB_ATOM_ATOM, 32, 0, 0);

            uint32_t value_mask = XCB_CONFIG_WINDOW_BORDER_WIDTH;
            uint32_t value_list[] = {s->config->border_width};
            xcb_configure_window(s->c, s->focus->wid, value_mask, value_list);
        }

        xcb_flush(s->c);

        s->focus->floating = true;
        client_move(s, s->focus, s->focus->x + dx, s->focus->y + dy);

        make_layout(s);
    }
}

void swapmainfocus(state_t *s, const char *command)
{
    (void)command;

    if (s->focus && s->focus->monitor == monitor_contains_cursor(s) && !s->focus->floating) {
        client_t *cl = s->clients;
        while (cl && (cl->fullscreen || cl->floating || cl->monitor != s->focus->monitor ||
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

void swapfocusdown(state_t *s, const char *command)
{
    (void)command;

    if (s->clients && s->focus && count_clients_on_desktop(s) > 1) {
        client_t *cl = s->focus->next;
        while (cl && (cl->fullscreen || cl->floating || cl->monitor != s->monitor_focus ||
                      cl->desktop_idx != s->monitor_focus->desktop_idx)) {
            cl = cl->next;
        }

        if (cl) {
            swap_clients(s, cl, s->focus);
            make_layout(s);
        }
    }
}

void swapfocusup(state_t *s, const char *command)
{
    (void)command;

    if (s->clients && s->focus && count_clients_on_desktop(s) > 1) {
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

void destroyclient(state_t *s, const char *command)
{
    (void)command;

    if (s->focus) {
        client_kill(s, s->focus);
    }
}

void killclient(state_t *s, const char *command)
{
    (void)command;

    if (s->focus) {
        xcb_kill_client(s->c, s->focus->wid);
        xcb_flush(s->c);
    }
}

void killwm(state_t *s, const char *command)
{
    (void)command;

    xcb_disconnect(s->c);
    clean(s);
    exit(EXIT_SUCCESS);
}

void restartwm(state_t *s, const char *command)
{
    (void)command;

    int old_sg = s->config->screen_gap;

    clean_config(s);

    parse_config_file(s);

    grab_keys(s);

    int dsg = s->config->screen_gap - old_sg;

    monitor_t *mon_focus = s->monitor_focus;
    monitor_t *mon = s->monitors;
    while (mon) {
        mon->padding.top += dsg;
        mon->padding.bottom += dsg;
        mon->padding.left += dsg;
        mon->padding.right += dsg;
        s->monitor_focus = mon;
        make_layout(s);
        mon = mon->next;
    }

    s->monitor_focus = mon_focus;

    client_t *cl = s->clients;
    uint32_t value_mask;
    uint32_t value_list[1];
    while (cl) {
        cl->size_hints.min_width = s->config->min_window_width;
        cl->size_hints.min_height = s->config->min_window_height;

        if (!cl->fullscreen) {
            value_mask = XCB_CONFIG_WINDOW_BORDER_WIDTH;
            value_list[0] = s->config->border_width;
            xcb_configure_window(s->c, cl->wid, value_mask, value_list);
        }

        if (s->focus != cl) {
            value_list[0] = s->config->unfocused_border_color;
            xcb_change_window_attributes(s->c, cl->wid, XCB_CW_BORDER_PIXEL, value_list);
        } else {
            value_list[0] = s->config->focused_border_color;
            xcb_change_window_attributes(s->c, cl->wid, XCB_CW_BORDER_PIXEL, value_list);
        }

        cl = cl->next;
    }
}
