void client_create(state_t *s, xcb_window_t wid);
void client_remove(state_t *s, client_t *cl);
client_t *client_from_wid(state_t *s, xcb_window_t wid);
void client_move(state_t *s, client_t *cl, int x, int y);
void client_resize(state_t *s, client_t *cl, xcb_motion_notify_event_t *e);
void client_unfocus(state_t *s);
void client_focus(state_t *s, client_t *cl);
int client_contains_cursor(state_t *s, client_t *cl);
