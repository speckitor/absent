#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <xcb/xcb.h>
#include <xcb/xcb_icccm.h>
#include <xcb/xproto.h>

#include "clients.h"
#include "desktops.h"
#include "events.h"
#include "keys.h"
#include "layout.h"
#include "monitors.h"
#include "types.h"
#include "logs.h"

static void map_request(state_t *s, xcb_generic_event_t *ev)
{
    xcb_map_request_event_t *e = (xcb_map_request_event_t *)ev;

    client_create(s, e->window);
}

static void button_release(state_t *s, xcb_generic_event_t *ev)
{
    (void)ev;

    s->mouse->resizingcorner = CORNER_NONE;
    s->mouse->pressed_button = 0;

    xcb_flush(s->c);
}

static void unmap_notify(state_t *s, xcb_generic_event_t *ev)
{
    xcb_unmap_notify_event_t *e = (xcb_unmap_notify_event_t *)ev;

    client_t *cl = client_from_wid(s, e->window);

    if (!cl || cl->hidden) {
        return;
    }

    if (cl->monitor != s->monitor_focus) {
        s->monitor_focus = cl->monitor;
    }

    client_t *next = cl == s->focus ? client_kill_next_focus(s) : NULL;

    client_remove(s, e->window);

    if (!cl->floating && !cl->fullscreen) {
        make_layout(s);
    }

    button_release(s, NULL);

    clients_update_ewmh(s);

    s->monitor_focus = monitor_contains_cursor(s);

    if (next) {
        client_focus(s, next);
    }

    xcb_flush(s->c);
}

static void configure_request(state_t *s, xcb_generic_event_t *ev)
{
    xcb_configure_request_event_t *e = (xcb_configure_request_event_t *)ev;

    uint16_t value_mask = 0;
    uint32_t value_list[7];
    int i = 0;

    client_t *cl = client_from_wid(s, e->window);
    if (cl) {
        return;
    }

    if (e->value_mask & XCB_CONFIG_WINDOW_X) {
        value_mask |= XCB_CONFIG_WINDOW_X;
        value_list[i++] = e->x;
    }
    if (e->value_mask & XCB_CONFIG_WINDOW_Y) {
        value_mask |= XCB_CONFIG_WINDOW_Y;
        value_list[i++] = e->y;
    }
    if (e->value_mask & XCB_CONFIG_WINDOW_WIDTH) {
        value_mask |= XCB_CONFIG_WINDOW_WIDTH;
        value_list[i++] = e->width;
    }
    if (e->value_mask & XCB_CONFIG_WINDOW_HEIGHT) {
        value_mask |= XCB_CONFIG_WINDOW_HEIGHT;
        value_list[i++] = e->height;
    }
    if (e->value_mask & XCB_CONFIG_WINDOW_BORDER_WIDTH) {
        value_mask |= XCB_CONFIG_WINDOW_BORDER_WIDTH;
        value_list[i++] = e->border_width;
    }
    if (e->value_mask & XCB_CONFIG_WINDOW_SIBLING) {
        value_mask |= XCB_CONFIG_WINDOW_SIBLING;
        value_list[i++] = e->sibling;
    }
    if (e->value_mask & XCB_CONFIG_WINDOW_STACK_MODE) {
        value_mask |= XCB_CONFIG_WINDOW_STACK_MODE;
        value_list[i++] = e->stack_mode;
    }

    xcb_configure_window(s->c, e->window, value_mask, value_list);
    xcb_flush(s->c);
}

static void client_message(state_t *s, xcb_generic_event_t *ev)
{
    xcb_client_message_event_t *e = (xcb_client_message_event_t *)ev;

    if (e->type == s->ewmh[EWMH_CURRENT_DESKTOP]) {
        switch_desktop_by_idx(s, e->data.data32[0]);
        return;
    }

    client_t *cl = client_from_wid(s, e->window);

    if (!cl) {
        return;
    }

    if (e->type == s->ewmh[EWMH_STATE]) {
        if (e->data.data32[1] == s->ewmh[EWMH_FULLSCREEN] ||
            e->data.data32[2] == s->ewmh[EWMH_FULLSCREEN]) {
            if (e->data.data32[0] == 1 || (e->data.data32[0] == 2 && !cl->fullscreen)) {
                client_fullscreen(s, cl, true);
            }
        }
    } else if (e->type == s->ewmh[EWMH_ACTIVE_WINDOW]) {
        if (s->focus != cl && !cl->hidden && cl->floating) {
            client_focus(s, cl);
        }
    }

    xcb_flush(s->c);
}

static void destroy_notify(state_t *s, xcb_generic_event_t *ev)
{
    xcb_destroy_notify_event_t *e = (xcb_destroy_notify_event_t *)ev;

    client_t *cl = client_from_wid(s, e->event);
    client_t *next = cl == s->focus ? client_kill_next_focus(s) : NULL;

    client_remove(s, e->event);

    if (next) {
        client_focus(s, next);
    }

    button_release(s, NULL);
}

static void key_press(state_t *s, xcb_generic_event_t *ev)
{
    xcb_key_press_event_t *e = (xcb_key_press_event_t *)ev;

    xcb_timestamp_t current_time = e->time;

    if ((current_time - s->lastkeypresstime) <= 50) {
        return;
    }

    s->lastkeypresstime = current_time;

    s->monitor_focus = monitor_contains_cursor(s);

    int length = sizeof(s->config->keybinds) / sizeof(s->config->keybinds[0]);
    for (int i = 0; i < length; i++) {
        if (key_cmp(s, s->config->keybinds[i], e->detail, e->state)) {
            s->config->keybinds[i].callback(s, s->config->keybinds[i].param);
        }
    }
}

static void button_press(state_t *s, xcb_generic_event_t *ev)
{
    xcb_button_press_event_t *e = (xcb_button_press_event_t *)ev;

    s->monitor_focus = monitor_contains_cursor(s);
    xcb_change_property(s->c, XCB_PROP_MODE_REPLACE, s->root, s->ewmh[EWMH_CURRENT_DESKTOP],
                        XCB_ATOM_CARDINAL, 32, 1,
                        &s->monitor_focus->desktop_id);

    client_t *cl = client_from_wid(s, e->event);

    if (!cl || !client_contains_cursor(s, cl)) {
        return;
    }

    uint16_t value_list[] = {XCB_STACK_MODE_ABOVE};
    xcb_configure_window(s->c, e->event, XCB_CONFIG_WINDOW_STACK_MODE, value_list);

    if (!s->focus || e->event != s->focus->wid) {
        client_focus(s, cl);
        xcb_allow_events(s->c, XCB_ALLOW_REPLAY_POINTER, XCB_CURRENT_TIME);
        xcb_flush(s->c);
    }

    optional_modifiers_t m = s->opt_mods;

    if ((e->state & ~(m.num_lock | m.caps_lock | m.scroll_lock)) == s->config->button_mod) {
        cl->floating = true;
        make_layout(s);
        s->mouse->pressed_button = e->detail;
        s->mouse->root_x = e->root_x;
        s->mouse->root_y = e->root_y;

        if (s->focus->fullscreen) {
            s->focus->fullscreen = false;
            uint32_t value_list[] = {s->config->border_width};
            xcb_configure_window(s->c, s->focus->wid, XCB_CONFIG_WINDOW_BORDER_WIDTH, value_list);
        }
    }
}

static void motion_notify(state_t *s, xcb_generic_event_t *ev)
{
    xcb_motion_notify_event_t *e = (xcb_motion_notify_event_t *)ev;

    xcb_timestamp_t current_time = e->time;
    if ((current_time - s->lastmotiontime) <= s->config->pointer_update_time) {
        return;
    }

    if (s->mouse->pressed_button == 0) {
        return;
    }

    if (s->mouse->pressed_button == s->config->move_button) {
        s->monitor_focus = monitor_contains_cursor(s);
        s->focus->monitor = s->monitor_focus;
        s->focus->desktop_idx = s->monitor_focus->desktop_idx;

        int x = s->focus->x + (e->root_x - s->mouse->root_x);
        int y = s->focus->y + (e->root_y - s->mouse->root_y);

        client_move(s, s->focus, x, y);
    } else if (s->mouse->pressed_button == s->config->resize_button) {
        client_resize(s, s->focus, e);
    }

    xcb_flush(s->c);

    s->mouse->root_x = e->root_x;
    s->mouse->root_y = e->root_y;
    s->lastmotiontime = current_time;
}

void send_event(state_t *s, client_t *cl, xcb_atom_t protocol)
{
    int has_prot = 0;
    xcb_icccm_get_wm_protocols_reply_t reply;

    if (xcb_icccm_get_wm_protocols_reply(
            s->c, xcb_icccm_get_wm_protocols(s->c, cl->wid, s->icccm[ICCCM_PROTOCOLS]), &reply,
            NULL)) {
        for (size_t i = 0; i < reply.atoms_len; i++) {
            if (reply.atoms[i] == protocol) {
                has_prot = 1;
                break;
            }
        }
        xcb_icccm_get_wm_protocols_reply_wipe(&reply);
    }

    if (has_prot) {
        xcb_client_message_event_t e;
        e.response_type = XCB_CLIENT_MESSAGE;
        e.window = cl->wid;
        e.format = 32;
        e.type = s->icccm[ICCCM_PROTOCOLS];
        e.data.data32[0] = protocol;
        e.data.data32[1] = XCB_TIME_CURRENT_TIME;
        xcb_send_event(s->c, 0, cl->wid, XCB_EVENT_MASK_NO_EVENT, (const char *)&e);
    }
}

static const event_handler_t handlers[XCB_LAST_EVENT] = {
    [XCB_MAP_REQUEST] = map_request,
    [XCB_UNMAP_NOTIFY] = unmap_notify,
    [XCB_CONFIGURE_REQUEST] = configure_request,
    [XCB_CLIENT_MESSAGE] = client_message,
    [XCB_DESTROY_NOTIFY] = destroy_notify,
    [XCB_KEY_PRESS] = key_press,
    [XCB_BUTTON_PRESS] = button_press,
    [XCB_BUTTON_RELEASE] = button_release,
    [XCB_MOTION_NOTIFY] = motion_notify,
};

void main_loop(state_t *s)
{
    xcb_generic_event_t *event;
    while (s->c && !xcb_connection_has_error(s->c)) {
        event = xcb_wait_for_event(s->c);

        if (!event) {
            log_msg(s, "XCB connection lost or failed\n");
            break;
        }

        uint8_t event_type = event->response_type & ~0x80;

        if (event_type < XCB_LAST_EVENT && handlers[event_type]) {
            handlers[event_type](s, event);
        }

        free(event);
    }
}
