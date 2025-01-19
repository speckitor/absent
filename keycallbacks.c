void spawnclient(state_t *s, const char *command) {
  if (fork() == 0) {
    execl("/bin/sh", "sh", "-c", command, (char *) NULL);
		_exit(EXIT_FAILURE);
  }
}

void killclient(state_t *s, const char *command) {
	if (s->focus) {
  	xcb_kill_client(s->c, s->focus->wid);
  	xcb_flush(s->c);
	}
}

void fullscreen(state_t *s, const char *command) {
	if (!s->focus) {
		return;
	}

	if (s->focus->fullscreen) {
		uint32_t value_mask = XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y | XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT;
		uint32_t value_list[] = {s->focus->x, s->focus->y, s->focus->width, s->focus->height};
		xcb_configure_window(s->c, s->focus->wid, value_mask, value_list);
		xcb_flush(s->c);
		s->focus->fullscreen = 0;
	} else {
		uint32_t value_mask = XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y | XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT;
		uint32_t value_list[] = {0 - BORDER_WIDTH, 0 - BORDER_WIDTH, s->screen->width_in_pixels + BORDER_WIDTH, s->screen->height_in_pixels + BORDER_WIDTH};
		xcb_configure_window(s->c, s->focus->wid, value_mask, value_list);
		xcb_flush(s->c);
		s->focus->fullscreen = 1;
	}
}

void killwm(state_t *s, const char *command) { 
	xcb_disconnect(s->c); 
	clean(s);
}
