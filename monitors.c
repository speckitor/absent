#include <stdlib.h>

#include "absent.h"
#include "monitors.h"

void monitors_setup(state_t *s) {
  const xcb_query_extension_reply_t *extension =
      xcb_get_extension_data(s->c, &xcb_randr_id);

  if (!extension || !extension->present) {
    xcb_disconnect(s->c);
    return;
  }

  xcb_randr_get_monitors_cookie_t monitors_cookie =
      xcb_randr_get_monitors(s->c, s->root, 1);
  xcb_randr_get_monitors_reply_t *monitors_reply =
      xcb_randr_get_monitors_reply(s->c, monitors_cookie, NULL);

  if (!monitors_reply) {
    xcb_disconnect(s->c);
    clean(s);
    return;
  }

  int monitors_length = xcb_randr_get_monitors_monitors_length(monitors_reply);
  xcb_randr_monitor_info_iterator_t monitors_iter =
      xcb_randr_get_monitors_monitors_iterator(monitors_reply);

  while (monitors_length) {

    monitor_t *monitor = calloc(1, sizeof(monitor_t));

    monitor->x = monitors_iter.data->x;
    monitor->y = monitors_iter.data->y;
    monitor->width = monitors_iter.data->width;
    monitor->height = monitors_iter.data->height;
    monitor->clients = NULL;
    monitor->next = s->monitors;

    s->monitors = monitor;

    xcb_randr_monitor_info_next(&monitors_iter);
    monitors_length--;
  }

  free(monitors_reply);
  xcb_flush(s->c);
}

monitor_t *monitor_contains_cursor(state_t *s) {
  xcb_query_pointer_reply_t *reply =
      xcb_query_pointer_reply(s->c, xcb_query_pointer(s->c, s->root), NULL);

  if (reply) {
    monitor_t *monitor = s->monitors;
    while (monitor) {
      if (reply->root_x >= monitor->x && reply->root_y >= monitor->y &&
          reply->root_x <= monitor->x + monitor->width &&
          reply->root_y <= monitor->y + monitor->height) {
        free(reply);
        return monitor;
      }
      monitor = monitor->next;
    }
  }

  free(reply);
  return s->monitors;
}
