#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xcb/randr.h>
#include <xcb/xcb.h>

#include "../config.h"
#include "absent.h"
#include "desktops.h"
#include "monitors.h"
#include "types.h"

void monitors_setup(state_t *s) {
    const xcb_query_extension_reply_t *extension = xcb_get_extension_data(s->c, &xcb_randr_id);

    if (!extension || !extension->present) {
        xcb_disconnect(s->c);
        return;
    }

    xcb_randr_get_screen_resources_current_cookie_t res_cookie = xcb_randr_get_screen_resources_current(s->c, s->root);
    xcb_randr_get_screen_resources_current_reply_t *res_reply = xcb_randr_get_screen_resources_current_reply(s->c, res_cookie, NULL);

    if (!res_reply) {
        xcb_disconnect(s->c);
        clean(s);
        return;
    }

    int32_t number_outputs = xcb_randr_get_screen_resources_current_outputs_length(res_reply);
    xcb_randr_output_t *outputs = xcb_randr_get_screen_resources_current_outputs(res_reply);

    for (int i = 0; i < number_outputs; i++) {
        xcb_randr_get_output_info_cookie_t output_cookie = xcb_randr_get_output_info(s->c, outputs[i], XCB_CURRENT_TIME);
        xcb_randr_get_output_info_reply_t *output_reply = xcb_randr_get_output_info_reply(s->c, output_cookie, NULL);

        if (!output_reply || output_reply->crtc == XCB_NONE) {
            continue;
        }

        xcb_randr_get_crtc_info_cookie_t crtc_cookie = xcb_randr_get_crtc_info(s->c, output_reply->crtc, XCB_CURRENT_TIME);
        xcb_randr_get_crtc_info_reply_t *crtc_reply = xcb_randr_get_crtc_info_reply(s->c, crtc_cookie, NULL);

        if (!crtc_reply) {
            free(output_reply);
            continue;
        }

        const char *monitor_name = (const char *)xcb_randr_get_output_info_name(output_reply);

        monitor_t *monitor = calloc(1, sizeof(monitor_t));

        monitor->padding.top = SCREEN_GAP;
        monitor->padding.left = SCREEN_GAP;
        monitor->padding.bottom = SCREEN_GAP;
        monitor->padding.right = SCREEN_GAP;

        monitor->x = crtc_reply->x;
        monitor->y = crtc_reply->y;
        monitor->width = crtc_reply->width;
        monitor->height = crtc_reply->height;

        monitor->next = s->monitors;
        s->monitors = monitor;

        int desktops_setuped = 0;

        for (long unsigned int j = 0; j < sizeof(desktops) / sizeof(desktop_config_t); j++) {
            if (strncmp(monitor_name, desktops[j].monitor_name,
                strlen(desktops[j].monitor_name)) == 0) {
                desktops_setuped = 1;

                int number_desktops = 0;

                for (long unsigned int k = 0; k < sizeof(desktops[j].desktop_names) /
                     sizeof(desktops[j].desktop_names[0]); k++) {
                    if (!desktops[j].desktop_names[k]) {
                        break;
                    }
                    number_desktops++;
                }

                monitor->desktops = calloc(number_desktops, sizeof(desktop_t));
                monitor->number_desktops = number_desktops;

                for (int k = 0; k < number_desktops; k++) {
                    monitor->desktops[k].desktop_id = s->number_desktops;
                    s->number_desktops++;
                    snprintf(monitor->desktops[k].name, sizeof(monitor->desktops[k].name), "%s", desktops[j].desktop_names[k]);
                    monitor->desktops[k].layout = DEFAULT_LAYOUT;
                }
            }
        }

        if (!desktops_setuped) {
            int number_desktops = 10;

            monitor->desktops = calloc(number_desktops, sizeof(desktop_t));
            monitor->number_desktops = number_desktops;

            for (int k = 0; k < number_desktops; k++) {
                monitor->desktops[k].desktop_id = s->number_desktops;
                s->number_desktops++;
                snprintf(monitor->desktops[k].name, sizeof(monitor->desktops[k].name), "%d", k + 1);
                monitor->desktops[k].layout = DEFAULT_LAYOUT;
            }
        }

        monitor->desktop_idx = 0;
        setup_desktop_names(s, monitor);

        free(crtc_reply);
        free(output_reply);
    }

    free(res_reply);
    xcb_flush(s->c);
}

monitor_t *monitor_contains_cursor(state_t *s) {
    xcb_query_pointer_reply_t *reply = xcb_query_pointer_reply(s->c, xcb_query_pointer(s->c, s->root), NULL);

    if (!reply) {
        return s->monitors;
    }

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
