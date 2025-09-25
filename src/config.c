#include <X11/keysym.h>
#include <libconfig.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "config.h"
#include "keycallbacks.h"
#include "types.h"

static const keyname_keysym_t key_mapping[] = {
    {"VoidSymbol", XK_VoidSymbol},
    {"BackSpace", XK_BackSpace},
    {"Tab", XK_Tab},
    {"Linefeed", XK_Linefeed},
    {"Clear", XK_Clear},
    {"Return", XK_Return},
    {"Pause", XK_Pause},
    {"Scroll_Lock", XK_Scroll_Lock},
    {"Sys_Req", XK_Sys_Req},
    {"Escape", XK_Escape},
    {"Delete", XK_Delete},
    {"Home", XK_Home},
    {"Left", XK_Left},
    {"Up", XK_Up},
    {"Right", XK_Right},
    {"Down", XK_Down},
    {"Page_Up", XK_Page_Up},
    {"Page_Down", XK_Page_Down},
    {"End", XK_End},
    {"Begin", XK_Begin},
    {"Space", XK_space},
    {"Exclam", XK_exclam},
    {"Quotedbl", XK_quotedbl},
    {"NumberSign", XK_numbersign},
    {"Dollar", XK_dollar},
    {"Percent", XK_percent},
    {"Ampersand", XK_ampersand},
    {"Apostrophe", XK_apostrophe},
    {"Parenleft", XK_parenleft},
    {"Parenright", XK_parenright},
    {"Asterisk", XK_asterisk},
    {"Plus", XK_plus},
    {"Comma", XK_comma},
    {"Minus", XK_minus},
    {"Period", XK_period},
    {"Slash", XK_slash},
    {"0", XK_0},
    {"1", XK_1},
    {"2", XK_2},
    {"3", XK_3},
    {"4", XK_4},
    {"5", XK_5},
    {"6", XK_6},
    {"7", XK_7},
    {"8", XK_8},
    {"9", XK_9},
    {"Colon", XK_colon},
    {"Semicolon", XK_semicolon},
    {"Less", XK_less},
    {"Equal", XK_equal},
    {"Greater", XK_greater},
    {"Question", XK_question},
    {"At", XK_at},
    {"BracketLeft", XK_bracketleft},
    {"Backslash", XK_backslash},
    {"BracketRight", XK_bracketright},
    {"AsciiCircum", XK_asciicircum},
    {"Underscore", XK_underscore},
    {"Grave", XK_grave},
    {"A", XK_a},
    {"B", XK_b},
    {"C", XK_c},
    {"D", XK_d},
    {"E", XK_e},
    {"F", XK_f},
    {"G", XK_g},
    {"H", XK_h},
    {"I", XK_i},
    {"J", XK_j},
    {"K", XK_k},
    {"L", XK_l},
    {"M", XK_m},
    {"N", XK_n},
    {"O", XK_o},
    {"P", XK_p},
    {"Q", XK_q},
    {"R", XK_r},
    {"S", XK_s},
    {"T", XK_t},
    {"U", XK_u},
    {"V", XK_v},
    {"W", XK_w},
    {"X", XK_x},
    {"Y", XK_y},
    {"Z", XK_z},
    {"BraceLeft", XK_braceleft},
    {"Bar", XK_bar},
    {"BraceRight", XK_braceright},
    {"AsciiTilde", XK_asciitilde},
    {"Num_Lock", XK_Num_Lock},
    {"KP_0", XK_KP_0},
    {"KP_1", XK_KP_1},
    {"KP_2", XK_KP_2},
    {"KP_3", XK_KP_3},
    {"KP_4", XK_KP_4},
    {"KP_5", XK_KP_5},
    {"KP_6", XK_KP_6},
    {"KP_7", XK_KP_7},
    {"KP_8", XK_KP_8},
    {"KP_9", XK_KP_9},
    {"KP_Decimal", XK_KP_Decimal},
    {"KP_Divide", XK_KP_Divide},
    {"KP_Multiply", XK_KP_Multiply},
    {"KP_Subtract", XK_KP_Subtract},
    {"KP_Add", XK_KP_Add},
    {"KP_Enter", XK_KP_Enter},
    {"KP_Equal", XK_KP_Equal},
    {"F1", XK_F1},
    {"F2", XK_F2},
    {"F3", XK_F3},
    {"F4", XK_F4},
    {"F5", XK_F5},
    {"F6", XK_F6},
    {"F7", XK_F7},
    {"F8", XK_F8},
    {"F9", XK_F9},
    {"F10", XK_F10},
    {"F11", XK_F11},
    {"F12", XK_F12},
    {"F13", XK_F13},
    {"F14", XK_F14},
    {"F15", XK_F15},
    {"F16", XK_F16},
    {"F17", XK_F17},
    {"F18", XK_F18},
    {"F19", XK_F19},
    {"F20", XK_F20},
    {"F21", XK_F21},
    {"F22", XK_F22},
    {"F23", XK_F23},
    {"F24", XK_F24},
    {"F25", XK_F25},
    {"F26", XK_F26},
    {"F27", XK_F27},
    {"F28", XK_F28},
    {"F29", XK_F29},
    {"F30", XK_F30},
    {"F31", XK_F31},
    {"F32", XK_F32},
    {"F33", XK_F33},
    {"F34", XK_F34},
    {"F35", XK_F35},
    {"Shift_L", XK_Shift_L},
    {"Shift_R", XK_Shift_R},
    {"Control_L", XK_Control_L},
    {"Control_R", XK_Control_R},
    {"Caps_Lock", XK_Caps_Lock},
    {"Shift_Lock", XK_Shift_Lock},
    {"Meta_L", XK_Meta_L},
    {"Meta_R", XK_Meta_R},
    {"Alt_L", XK_Alt_L},
    {"Alt_R", XK_Alt_R},
    {"Super_L", XK_Super_L},
    {"Super_R", XK_Super_R},
    {"Hyper_L", XK_Hyper_L},
    {"Hyper_R", XK_Hyper_R},
};

static const buttonname_buttonnum_t button_mapping[] = {
    {"LeftButton", 1},
    {"MiddleButton", 2},
    {"RightButton", 3},
};

static const modname_mod_t mod_mapping[] = {
    {"Super", XCB_MOD_MASK_4},
    {"Alt", XCB_MOD_MASK_1},
    {"Shift", XCB_MOD_MASK_SHIFT},
    {"Control", XCB_MOD_MASK_CONTROL},
};

static const layoutname_layout_t layout_mapping[] = {
    {"Tiled", TILED},
    {"Rtiled", RTILED},
    {"Horizontal", HORIZONTAL},
    {"Vertical", VERTICAL},
};

static const cbname_cb_t callback_mapping[] = {
    {"run", run},
    {"cyclefocusdown", cyclefocusdown},
    {"cyclefocusup", cyclefocusup},
    {"setcurrentdesktop", setcurrentdesktop},
    {"movefocustodesktop", movefocustodesktop},
    {"setlayout", setlayout},
    {"setfocustiled", setfocustiled},
    {"setfocusfullscreen", setfocusfullscreen},
    {"movefocusdir", movefocusdir},
    {"swapmainfocus", swapmainfocus},
    {"swapfocusdown", swapfocusdown},
    {"swapfocusup", swapfocusup},
    {"destroyclient", destroyclient},
    {"killclient", killclient},
    {"killwm", killwm},
    {"restartwm", restartwm},
};

static void parse_variable_err(char *error_field)
{
    fprintf(stderr, "No '%s' setting in config file.\n", error_field);
    exit(EXIT_FAILURE);
}

static void parse_config_int(state_t *s, config_t *cfg, char *name, int *variable)
{
    if (!config_lookup_int(cfg, name, variable)) {
        parse_variable_err(name);
    }
}

static void parse_config_float(state_t *s, config_t *cfg, char *name, double *variable)
{
    if (!config_lookup_float(cfg, name, variable)) {
        parse_variable_err(name);
    }
}

static void parse_config_bool(state_t *s, config_t *cfg, char *name, int *variable)
{
    if (!config_lookup_bool(cfg, name, variable)) {
        parse_variable_err(name);
    }
}

static void parse_config_desktop(state_t *s, config_t *cfg, config_setting_t *mon, int i)
{
    const char *tmp;
    config_setting_lookup_string(mon, "mon", &tmp);
    if (!tmp) {
        parse_variable_err("mon");
    } else {
        s->config->desktops[i].monitor_name = strdup(tmp);
    }

    config_setting_t *mon_desktops = config_setting_lookup(mon, "desktop_names");
    if (mon_desktops) {
        int count = config_setting_length(mon_desktops);
        for (int j = 0; j < count; ++j) {
            s->config->desktops[i].desktop_names[j] =
                strdup(config_setting_get_string_elem(mon_desktops, j));
        }
    } else {
        parse_variable_err("desktop_names");
    }
}

static void parse_config_desktops(state_t *s, config_t *cfg)
{
    config_setting_t *desktops = config_lookup(cfg, "desktops");
    if (desktops) {
        int count = config_setting_length(desktops);
        for (int i = 0; i < count; ++i) {
            config_setting_t *mon = config_setting_get_elem(desktops, i);
            parse_config_desktop(s, cfg, mon, i);
        }
    } else {
        parse_variable_err("desktops");
    }
}

static void config_keybind_add_mod(state_t *s, uint16_t *mods, const char *mod)
{
    for (int j = 0; j < sizeof(mod_mapping) / sizeof(mod_mapping[0]); ++j) {
        if (strcmp(mod_mapping[j].name, mod) == 0) {
            *mods |= mod_mapping[j].mod;
        }
    }
}

static void parse_config_keybind(state_t *s, config_t *cfg, config_setting_t *keybind, int i)
{
    const char *key;
    if (!config_setting_lookup_string(keybind, "key", &key)) {
        parse_variable_err("key");
    } else {
        for (int j = 0; j < sizeof(key_mapping) / sizeof(key_mapping[0]); ++j) {
            if (strcmp(key_mapping[j].name, key) == 0) {
                s->config->keybinds[i].key = key_mapping[j].keysym;
            }
        }
    }

    s->config->keybinds[i].mods = 0;
    config_setting_t *mods = config_setting_lookup(keybind, "mods");
    if (mods) {
        int count = config_setting_length(mods);
        for (int j = 0; j < count; ++j) {
            const char *mod = config_setting_get_string_elem(mods, j);
            config_keybind_add_mod(s, &s->config->keybinds[i].mods, mod);
        }
    } else {
        parse_variable_err("mods");
    }

    if (s->config->keybinds[i].mods == 0) {
        fprintf(stderr, "No modifiers provided\n");
        exit(EXIT_FAILURE);
    }

    const char *action;
    if (!config_setting_lookup_string(keybind, "action", &action)) {
        parse_variable_err("action");
    } else {
        for (int j = 0; j < sizeof(callback_mapping) / sizeof(callback_mapping[0]); ++j) {
            if (strcmp(callback_mapping[j].name, action) == 0) {
                s->config->keybinds[i].callback = callback_mapping[j].callback;
            }
        }
    }

    const char *param;
    if (!config_setting_lookup_string(keybind, "param", &param)) {
        s->config->keybinds[i].param = NULL;
    } else {
        s->config->keybinds[i].param = strdup(param);
    }
}

static void parse_config_keybinds(state_t *s, config_t *cfg)
{
    config_setting_t *keybinds = config_lookup(cfg, "keybinds");
    if (keybinds) {
        int count = config_setting_length(keybinds);
        for (int i = 0; i < count; ++i) {
            config_setting_t *keybind = config_setting_get_elem(keybinds, i);
            parse_config_keybind(s, cfg, keybind, i);
        }
    } else {
        parse_variable_err("keybinds");
    }
}

/*
 * Shitty function, more fragments must
 * be extracted into separate functions.
 */
void parse_config_file(state_t *s)
{
    s->config = calloc(1, sizeof(wm_config_t));
    if (!s->config) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    config_t cfg;
    config_init(&cfg);

    const char *home = getenv("HOME");
    char path[512];
    bool loaded = false;

    if (home) {
        snprintf(path, sizeof(path), "%s/.config/absent/absent.cfg", home);
        if (config_read_file(&cfg, path)) {
            loaded = true;
        } else {
            fprintf(stderr, "Error reading home config at %s:%d - %s\n", config_error_file(&cfg),
                    config_error_line(&cfg), config_error_text(&cfg));
        }
    }

    if (!loaded) {
        const char *etcpath = "/etc/absent/absent.cfg";
        if (config_read_file(&cfg, etcpath)) {
            loaded = true;
        } else {
            fprintf(stderr, "Error reading etc config at %s:%d - %s\n", config_error_file(&cfg),
                    config_error_line(&cfg), config_error_text(&cfg));
        }
    }

    if (!loaded) {
        config_destroy(&cfg);
        exit(EXIT_FAILURE);
    }

    const char *autostart;
    if (!config_lookup_string(&cfg, "autostart", &autostart)) {
        parse_variable_err("autostart");
    } else {
        s->config->autostart = strdup(autostart);
        if (!s->config->autostart) {
            perror("strdup error");
            exit(EXIT_FAILURE);
        }
    }

    const char *default_layout;
    if (!config_lookup_string(&cfg, "default_layout", &default_layout)) {
        parse_variable_err("default_layout");
    } else {
        for (int i = 0; i < (sizeof(layout_mapping) / sizeof(layout_mapping[0])); ++i) {
            if (strcmp(layout_mapping[i].name, default_layout) == 0) {
                s->config->default_layout = layout_mapping[i].layout;
                break;
            }
        }
    }

    parse_config_int(s, &cfg, "screen_gap", &s->config->screen_gap);
    parse_config_int(s, &cfg, "layout_gap", &s->config->layout_gap);
    parse_config_int(s, &cfg, "min_window_width", &s->config->min_window_width);
    parse_config_int(s, &cfg, "min_window_height", &s->config->min_window_height);
    parse_config_int(s, &cfg, "border_width", &s->config->border_width);
    parse_config_int(s, &cfg, "focused_border_color", &s->config->focused_border_color);
    parse_config_int(s, &cfg, "unfocused_border_color", &s->config->unfocused_border_color);
    parse_config_int(s, &cfg, "move_window_step", &s->config->move_window_step);
    parse_config_int(s, &cfg, "pointer_update_time", &s->config->pointer_update_time);

    parse_config_float(s, &cfg, "main_window_area", &s->config->main_window_area);

    parse_config_bool(s, &cfg, "set_new_window_main", &s->config->set_new_window_main);

    const char *button_mod;
    if (!config_lookup_string(&cfg, "button_mod", &button_mod)) {
        parse_variable_err("button_mod");
    } else {
        for (int i = 0; i < (sizeof(mod_mapping) / sizeof(mod_mapping[0])); ++i) {
            if (strcmp(mod_mapping[i].name, button_mod) == 0) {
                s->config->button_mod = mod_mapping[i].mod;
                break;
            }
        }
    }

    const char *move_button;
    if (!config_lookup_string(&cfg, "move_button", &move_button)) {
        parse_variable_err("move_button");
    } else {
        for (int i = 0; i < (sizeof(button_mapping) / sizeof(button_mapping[0])); ++i) {
            if (strcmp(button_mapping[i].name, move_button) == 0) {
                s->config->move_button = button_mapping[i].button;
                break;
            }
        }
    }

    const char *resize_button;
    if (!config_lookup_string(&cfg, "resize_button", &resize_button)) {
        parse_variable_err("resize_button");
    } else {
        for (int i = 0; i < (sizeof(button_mapping) / sizeof(button_mapping[0])); ++i) {
            if (strcmp(button_mapping[i].name, resize_button) == 0) {
                s->config->resize_button = button_mapping[i].button;
                break;
            }
        }
    }

    parse_config_desktops(s, &cfg);
    parse_config_keybinds(s, &cfg);

    config_destroy(&cfg);
}
