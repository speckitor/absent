void main_loop();

void map_request(state_t *s, xcb_generic_event_t *ev);
void configure_request(state_t *s, xcb_generic_event_t *ev);
void destroy_notify(state_t *s, xcb_generic_event_t *ev);
void key_press(state_t *s, xcb_generic_event_t *ev);
void button_press(state_t *s, xcb_generic_event_t *ev);
void button_release(state_t *s);
void motion_notify(state_t *s, xcb_generic_event_t *ev);
void enter_notify(state_t *s, xcb_generic_event_t *ev);
