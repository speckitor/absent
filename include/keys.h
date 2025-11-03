#ifndef KEYS_H_
#define KEYS_H_

#include <xcb/xcb.h>

#include "types.h"

xcb_keycode_t *get_keycode(state_t *s, xcb_keysym_t keysym);
int key_cmp(state_t *s, keybind_t keybind, xcb_keycode_t event_keycode, uint16_t mod);
uint16_t get_modifier_from_keysym(state_t *s, xcb_keysym_t mod_keysym);

#endif // KEYS_H_
