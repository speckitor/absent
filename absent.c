#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <xcb/xcb.h>
#include <X11/keysym.h>
#include <xcb/xcb_keysyms.h>
#include <xcb/xcb_util.h>
#include <xcb/xproto.h>
#include <xkbcommon/xkbcommon.h>
#include <xcb/xcb_cursor.h>

#include "types.h"
#include "events.h"
#include "clients.h"
#include "keycallbacks.h"
#include "keys.h"

#include "config.h"

#include "events.c"
#include "clients.c"
#include "keycallbacks.c"

void setup(state_t *s) {
	s->c = xcb_connect(NULL, NULL);
	if (xcb_connection_has_error(s->c)) {
		exit(EXIT_FAILURE);
	}
  s->screen = xcb_setup_roots_iterator(xcb_get_setup(s->c)).data;
  s->root = s->screen->root;

	s->clients = NULL;
	s->focus = NULL;

	s->lastmotiontime = 0.0;
	s->m = calloc(1, sizeof(mouse_t));
	s->m->pressed_button = 0;
	s->m->root_x = 0;
	s->m->root_y = 0;
	s->m->resizingcorner = CORNER_NONE;

	size_t length = sizeof(keybinds) / sizeof(keybinds[0]);
  for (int i = 0; i < length; i++) {
		xcb_keycode_t *keycode = get_keycode(s, keybinds[i].key);
		if (keycode) {
			xcb_grab_key(s->c, 0, s->root, keybinds[i].mod, *keycode, XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC);
		}
  }

	xcb_cursor_context_t *ctx;
	xcb_cursor_context_new(s->c, s->screen, &ctx);	
	xcb_cursor_t cursor = xcb_cursor_load_cursor(ctx, "left_ptr");
	xcb_change_window_attributes(s->c, s->root, XCB_CW_CURSOR, &cursor);
	xcb_cursor_context_free(ctx);

	uint32_t value_list[] = {
		XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT |
		XCB_EVENT_MASK_STRUCTURE_NOTIFY | 
		XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY
	};
	xcb_change_window_attributes_checked(s->c, s->root, XCB_CW_EVENT_MASK, value_list);

	xcb_set_input_focus(s->c, XCB_INPUT_FOCUS_POINTER_ROOT, s->root, XCB_CURRENT_TIME);

  xcb_flush(s->c);
}

void clean(state_t *s) {
	free(s->m);
	
	client_t *cl = s->clients;
	client_t *next;
	while (cl) {
		next = cl->next;
		free(cl);
		cl = next;
	}
}

int main() {
	state_t *state = calloc(1, sizeof(state_t));

  setup(state);

  main_loop(state);

  xcb_disconnect(state->c);

	clean(state);

  return 0;
}
