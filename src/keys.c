#include <xcb/xcb_keysyms.h>

#include "keys.h"

xcb_keycode_t *get_keycode(state_t *s, xcb_keysym_t keysym)
{
    xcb_key_symbols_t *keysyms = xcb_key_symbols_alloc(s->c);
    xcb_keycode_t *keycode;

    if (!keysyms) {
        keycode = NULL;
    } else {
        keycode = xcb_key_symbols_get_keycode(keysyms, keysym);
    }

    xcb_key_symbols_free(keysyms);

    return keycode;
}

int key_cmp(state_t *s, keybind_t keybind, xcb_keycode_t event_keycode, uint16_t mod)
{
    xcb_keycode_t *keycode = get_keycode(s, keybind.key);

    if (!keycode) {
        return 0;
    }

    if (*keycode == event_keycode && keybind.mod == mod) {
        return 1;
    }

    return 0;
}
