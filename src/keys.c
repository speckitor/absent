#include <stdlib.h>

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
    int result;
    xcb_keycode_t *keycode = get_keycode(s, keybind.key);

    if (!keycode) {
        return 0;
    }

    optional_modifiers_t m = s->opt_mods;

    if (event_keycode == *keycode && (mod & ~(m.num_lock | m.caps_lock | m.scroll_lock)) == keybind.mods) {
        return 1;
    }

    return 0;
}

uint16_t get_modifier_from_keysym(state_t *s, xcb_keysym_t mod_keysym)
{
    uint16_t mod = 0;
    xcb_keycode_t *keycodes = NULL;
    xcb_keycode_t *mod_keycodes = NULL;
    xcb_get_modifier_mapping_reply_t *reply = NULL;
    xcb_key_symbols_t *symbols = xcb_key_symbols_alloc(s->c);

    if ((keycodes = xcb_key_symbols_get_keycode(symbols, mod_keysym)) == NULL ||
        (reply = xcb_get_modifier_mapping_reply(s->c, xcb_get_modifier_mapping(s->c), NULL)) == NULL ||
        (reply->keycodes_per_modifier < 1) ||
        (mod_keycodes = xcb_get_modifier_mapping_keycodes(reply)) == NULL) {
        xcb_key_symbols_free(symbols);
        free(keycodes);
        free(reply);
        return mod;
    }

    unsigned num_mod = xcb_get_modifier_mapping_keycodes_length(reply) / reply->keycodes_per_modifier;
    for (unsigned i = 0; i < num_mod; ++i) {
        for (unsigned j = 0; j < reply->keycodes_per_modifier; ++j) {
            xcb_keycode_t mk = mod_keycodes[i * reply->keycodes_per_modifier + j];
            if (mk == XCB_NO_SYMBOL) {
                continue;
            }
            for (xcb_keycode_t *k = keycodes; *k != XCB_NO_SYMBOL; k++) {
                if (*k == mk) {
                    mod |= (1 << i);
                }
            }
        }
    }
    
    xcb_key_symbols_free(symbols);
    free(keycodes);
    free(reply);
    return mod;
}
