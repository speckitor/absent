#ifndef ABSENT_KEYS_H
#define ABSENT_KEYS_H

#include <xcb/xcb.h>

#include "types.h"

xcb_keycode_t *get_keycode(state_t *s, xcb_keysym_t keysym);
int key_cmp(state_t *s, keybind_t keybind, xcb_keycode_t event_keycode,
            uint16_t mod);

#endif
