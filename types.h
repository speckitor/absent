typedef struct state_t state_t;

typedef void (*keycallback_t) (state_t* s, const char *command);

typedef struct {
	unsigned long key;
	uint16_t mod;
	keycallback_t callback;
	const char *command;
} keybind_t;

typedef struct client_t client_t;

struct client_t {
	xcb_window_t wid;

	int x;
	int y;
	int width;
	int height;

	int fullscreen;

	client_t *next;
};

typedef enum {
	CORNER_NONE,
	TOP_LEFT,
	TOP_RIGHT,
	BOTTOM_LEFT,
	BOTTOM_RIGHT
} corner_t;

typedef struct {
  int pressed_button;
  int root_x;
  int root_y;
	corner_t resizingcorner;
} mouse_t;

struct state_t {
	xcb_connection_t *c;
	xcb_screen_t *screen;
	xcb_window_t root;

  xcb_atom_t wm_protocols_atom;
  xcb_atom_t wm_delete_window_atom;

	client_t* clients;
	client_t* focus;

	float lastmotiontime;
	mouse_t *m;
};

void clean(state_t *s);
