#include "layout.h"
#include "../config.h"
#include "monitors.h"
#include "types.h"

static void (*layout_functions[LAYOUTS_NUMBER])(state_t *s, int number) = {
    [TILED] = tiled,
    [VERTICAL] = vertical,
    [HORIZONTAL] = horizontal,
};

void make_layout(state_t *s) {
    if (!s->monitor_focus) {
        return;
    }

    client_t *cl = s->clients;
    monitor_t *mon = s->monitor_focus;

    int number = 0;

    while (cl) {
        if (cl->monitor == mon && cl->desktop_idx == mon->desktop_idx &&
            !cl->floating && !cl->fullscreen && !cl->hidden) {
            number++;
        }
        cl = cl->next;
    }

    if (number == 0) {
        return;
    }

    for (layout_t i = 0; i < LAYOUTS_NUMBER; ++i) {
        if (mon->desktops[mon->desktop_idx].layout == i) {
            layout_functions[i](s, number); 
        } 
    } 

    xcb_flush(s->c);
}

void tiled(state_t *s, int number) {
    client_t *cl = s->clients;
    int i, mw, ty, x, y, w, h;

    int bw = 2 * BORDER_WIDTH;
    int lg = LAYOUT_GAP;
    monitor_t *mon = s->monitor_focus;
    padding_t pad = mon->padding;

    mw = number > 1 ? (mon->width - pad.left - pad.right) * MAIN_WINDOW_AREA : mon->width - pad.left - pad.right;
    ty = pad.top;

    for (i = 0, cl = next_tiled(s, cl); cl; cl = next_tiled(s, cl->next), i++) {
        if (i < 1) {
            x = mon->x + pad.left;
            y = mon->y + ty;
            w = mw - bw;
            h = mon->height - bw - pad.top - pad.bottom;
            client_move_resize(s, cl, x, y, w, h);
        } else {
            x = mon->x + mw + lg + pad.left;
            y = mon->y + ty;
            w = mon->width - mw - bw - lg - pad.left - pad.right;
            h = (mon->height - ty - pad.bottom) / (number - i) - bw;
            client_move_resize(s, cl, x, y, w, h);
            ty += h + bw + lg;
        }
    }
}

void vertical(state_t *s, int number) {
    client_t *cl = s->clients;
    int i, tx, x, y, w, h;

    int bw = 2 * BORDER_WIDTH;
    int lg = (number > 1) ? LAYOUT_GAP : 0;
    monitor_t *mon = s->monitor_focus;
    padding_t pad = mon->padding;

    tx = pad.left;
    y = mon->y + pad.top;
    h = mon->height - bw - pad.top - pad.bottom;

    for (i = 0, cl = next_tiled(s, cl); cl; cl = next_tiled(s, cl->next), i++) {
        x = mon->x + tx;
        w = (mon->width - tx - pad.right) / (number - i) - bw;
        client_move_resize(s, cl, x, y, w, h);
        tx += w + bw + lg;
    }
}

void horizontal(state_t *s, int number) {
    client_t *cl = s->clients;
    int i, ty, x, y, w, h;

    int bw = 2 * BORDER_WIDTH;
    int lg = (number > 1) ? LAYOUT_GAP : 0;
    monitor_t *mon = s->monitor_focus;
    padding_t pad = mon->padding;

    ty = pad.top;
    x = mon->x + pad.left;
    w = mon->width - bw - pad.left - pad.right;

    for (i = 0, cl = next_tiled(s, cl); cl; cl = next_tiled(s, cl->next), i++) {
        y = mon->y + ty;
        h = (mon->height - ty - pad.bottom) / (number - i) - bw;
        client_move_resize(s, cl, x, y, w, h);
        ty += h + bw + lg;
    }
}

client_t *next_tiled(state_t *s, client_t *cl) {
  while (cl && (cl->floating || 
                cl->fullscreen || 
                s->monitor_focus != cl->monitor ||
                s->monitor_focus->desktop_idx != cl->desktop_idx)) {
    cl = cl->next;
  }
  return cl;
}

void client_move_resize(state_t *s, client_t *cl, int x, int y, int width,
                        int height) {
    cl->x = x;
    cl->y = y;
    cl->width = width;
    cl->height = height;

    uint32_t value_mask;
    uint32_t value_list[4];

    value_mask = 
        XCB_CONFIG_WINDOW_X |
        XCB_CONFIG_WINDOW_Y |
        XCB_CONFIG_WINDOW_WIDTH | 
        XCB_CONFIG_WINDOW_HEIGHT;
    value_list[0] = cl->x;
    value_list[1] = cl->y;
    value_list[2] = cl->width;
    value_list[3] = cl->height;
    xcb_configure_window(s->c, cl->wid, value_mask, value_list);
    xcb_flush(s->c);
}

void swap_clients(state_t *s, client_t *cl1, client_t *cl2) {
    if (cl1 == cl2) {
        return;
    }

    client_t *prev1, *prev2, *tmp;
    prev1 = NULL;
    prev2 = NULL;
    tmp = s->clients;

    while (tmp && tmp != cl1) {
        prev1 = tmp;
        tmp = tmp->next;
    }
    if (!tmp) {
        return;
    }

    tmp = s->clients;
    while (tmp && tmp != cl2) {
        prev2 = tmp;
        tmp = tmp->next;
    }
    if (!tmp) {
        return;
    }

    if (prev1) {
        prev1->next = cl2;
    } else {
        s->clients = cl2;
    }

    if (prev2) {
        prev2->next = cl1;
    } else {
        s->clients = cl1;
    }

    tmp = cl1->next;
    cl1->next = cl2->next;
    cl2->next = tmp;
}
