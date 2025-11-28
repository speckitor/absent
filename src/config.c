#include <X11/keysym.h>
#include <X11/XF86keysym.h>

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <xcb/xproto.h>

#define CFG_IMPLEMENTATION
#include "cfg.h"

#include "config.h"
#include "keycallbacks.h"
#include "logs.h"
#include "types.h"

static const keyname_keysym_t key_mapping[] = {
    {"VoidSymbol", XK_VoidSymbol},
    {"BackSpace", XK_BackSpace},
    {"Print", XK_Print},
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
    {"AudioLowerVolume", XF86XK_AudioLowerVolume},
    {"AudioRaiseVolume", XF86XK_AudioRaiseVolume},
    {"AudioMute", XF86XK_AudioMute},
    {"AudioPlay", XF86XK_AudioPlay},
    {"AudioStop", XF86XK_AudioStop},
    {"AudioPrev", XF86XK_AudioPrev},
    {"AudioNext", XF86XK_AudioNext},
    {"AudioRecord", XF86XK_AudioRecord},
    {"AudioRewind", XF86XK_AudioRewind},
    {"AudioForward", XF86XK_AudioForward},
    {"AudioPause", XF86XK_AudioPause},
    {"LaunchMail", XF86XK_Mail},
    {"LaunchMedia", XF86XK_AudioMedia},
    {"LaunchHome", XF86XK_HomePage},
    {"LaunchFavorites", XF86XK_Favorites},
    {"LaunchSearch", XF86XK_Search},
    {"BrightnessUp", XF86XK_MonBrightnessUp},
    {"BrightnessDown", XF86XK_MonBrightnessDown},
    {"ScreenSaver", XF86XK_ScreenSaver},
    {"Sleep", XF86XK_Sleep},
    {"PowerOff", XF86XK_PowerOff},
    {"WakeUp", XF86XK_WakeUp},
    {"Calculator", XF86XK_Calculator},
    {"FileManager", XF86XK_Explorer},
    {"Terminal", XF86XK_Terminal},
    {"WWW", XF86XK_WWW},
    {"Mail", XF86XK_Mail},
    {"Battery", XF86XK_Battery},
    {"Bluetooth", XF86XK_Bluetooth},
    {"WLAN", XF86XK_WLAN},
    {"TouchpadToggle", XF86XK_TouchpadToggle},
    {"TouchpadOn", XF86XK_TouchpadOn},
    {"TouchpadOff", XF86XK_TouchpadOff},
    {"Eject", XF86XK_Eject},
    {"RotateWindows", XF86XK_RotateWindows},
    {"Close", XF86XK_Close},
    {"AudioMicMute", XF86XK_AudioMicMute},
    {"KbdBrightnessUp", XF86XK_KbdBrightnessUp},
    {"KbdBrightnessDown", XF86XK_KbdBrightnessDown},
    {"KbdLightOnOff", XF86XK_KbdLightOnOff}
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
    {"Tiled", TILED},     {"Rtiled", RTILED},         {"Vtiled", VTILED},
    {"Rvtiled", RVTILED}, {"Horizontal", HORIZONTAL}, {"Vertical", VERTICAL},
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
    {"resizemainwindow", resizemainwindow},
    {"swapmainfocus", swapmainfocus},
    {"swapfocusdown", swapfocusdown},
    {"swapfocusup", swapfocusup},
    {"destroyclient", destroyclient},
    {"killclient", killclient},
    {"killwm", killwm},
    {"restartwm", restartwm},
};

static void parse_error(state_t *s, const char *message)
{
    log_msg(s, "%s", message);
    exit(EXIT_FAILURE);
}

static void parse_config_desktop(state_t *s, Cfg_Variable *mon, int i)
{
    char *tmp = cfg_get_string(mon, "mon");
    if (tmp) {
        s->config->desktops[i].monitor_name = strdup(tmp);
    } else {
        parse_error(s, "No monitor name in config");
    }

    Cfg_Variable *mon_desktops = cfg_get_array(mon, "desktop_names");
    if (mon_desktops) {
        size_t count = cfg_get_context_len(mon_desktops);
        for (size_t j = 0; j < count; ++j) {
            s->config->desktops[i].desktop_names[j] = strdup(cfg_get_string_elem(mon_desktops, j));
        }
    }
}

static void parse_config_desktops(state_t *s, Cfg_Variable *global)
{
    Cfg_Variable *desktops = cfg_get_array(global, "desktops");
    if (desktops) {
        size_t count = cfg_get_context_len(desktops);
        for (size_t i = 0; i < count; ++i) {
            Cfg_Variable *mon = cfg_get_struct_elem(desktops, i);
            if (mon) parse_config_desktop(s, mon, i);
        }
        s->config->md_count = count;
    }
}

static void config_keybind_add_mod(uint16_t *mods, const char *mod)
{
    size_t c = 0;
    for (size_t j = 0; j < sizeof(mod_mapping) / sizeof(mod_mapping[0]); ++j, ++c) {
        if (strcmp(mod_mapping[j].name, mod) == 0) {
            *mods |= mod_mapping[j].mod;
        }
    }
}

static void parse_config_keybind(state_t *s, Cfg_Variable *keybind, size_t i)
{
    const char *key = cfg_get_string(keybind, "key");
    if (key) {
        for (size_t j = 0; j < sizeof(key_mapping) / sizeof(key_mapping[0]); ++j) {
            if (strcmp(key_mapping[j].name, key) == 0) {
                s->config->keybinds[i].key = key_mapping[j].keysym;
            }
        }
    } else {
        parse_error(s, "No key name in config");
    }

    s->config->keybinds[i].mods = 0;
    Cfg_Variable *mods = cfg_get_array(keybind, "mods");
    if (mods) {
        int count = cfg_get_context_len(mods);
        for (int j = 0; j < count; ++j) {
            const char *mod = cfg_get_string_elem(mods, j);
            config_keybind_add_mod(&s->config->keybinds[i].mods, mod);
        }
    }

    const char *action = cfg_get_string(keybind, "action");
    if (!action) {
        parse_error(s, "No action in config");
    } else {
        for (size_t j = 0; j < sizeof(callback_mapping) / sizeof(callback_mapping[0]); ++j) {
            if (strcmp(callback_mapping[j].name, action) == 0) {
                s->config->keybinds[i].callback = callback_mapping[j].callback;
            }
        }
    }

    const char *param = cfg_get_string(keybind, "param");
    if (!param) {
        s->config->keybinds[i].param = NULL;
    } else {
        s->config->keybinds[i].param = strdup(param);
    }
}

static void parse_config_keybinds(state_t *s, Cfg_Variable *global)
{
    Cfg_Variable *keybinds = cfg_get_array(global, "keybinds");
    if (keybinds) {
        size_t count = cfg_get_context_len(keybinds);
        for (size_t i = 0; i < count; ++i) {
            Cfg_Variable *keybind = cfg_get_struct_elem(keybinds, i);
            if (keybind) parse_config_keybind(s, keybind, i);
        }
        s->config->kb_count = count;
    } else {
        parse_error(s, "No keybinds in config");
    }
}

void parse_config_file(state_t *s)
{
    s->config = malloc(sizeof(wm_config_t));
    if (!s->config) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    Cfg_Config *cfg = cfg_config_init();

    const char *home = getenv("HOME");
    char path[512];
    bool loaded = false;

    if (home) {
        snprintf(path, sizeof(path), "%s/.config/absent/absent.cfg", home);
        if (cfg_load_file(cfg, path) == 0) {
            loaded = true;
        } else {
            log_msg(s, "Local config parsing error. %s", cfg_err_message(cfg));
        }
    }

    if (!loaded) {
        cfg_config_deinit(cfg);
        cfg = cfg_config_init();
        const char *etcpath = "/etc/absent/absent.cfg";
        if (cfg_load_file(cfg, etcpath) == CFG_ERROR_NONE) {
            loaded = true;
        } else {
            log_msg(s, "Global config parsing error. %s", cfg_err_message(cfg));
        }
    }

    if (!loaded) {
        cfg_config_deinit(cfg);
        exit(EXIT_FAILURE);
    }

    Cfg_Variable *global = cfg_global_context(cfg);

    const char *autostart = cfg_get_string(global, "autostart");
    if (!autostart) {
        s->config->autostart = "";
    } else {
        s->config->autostart = strdup(autostart);
    }

    const char *default_layout = cfg_get_string(global, "default_layout");
    if (!default_layout) {
        s->config->default_layout = TILED;
    } else {
        for (size_t i = 0; i < (sizeof(layout_mapping) / sizeof(layout_mapping[0])); ++i) {
            if (strcmp(layout_mapping[i].name, default_layout) == 0) {
                s->config->default_layout = layout_mapping[i].layout;
                break;
            }
        }
    }

    s->config->screen_gap = cfg_get_int(global, "screen_gap");
    s->config->layout_gap = cfg_get_int(global, "layout_gap");
    s->config->min_window_width = cfg_get_int(global, "min_window_width");
    s->config->min_window_height = cfg_get_int(global, "min_window_height");
    s->config->border_width = cfg_get_int(global, "border_width");

    const char *fbc = cfg_get_string(global, "focused_border_color");
    s->config->focused_border_color = (int)strtol(fbc, NULL, 16);

    const char *ufbc = cfg_get_string(global, "unfocused_border_color");
    s->config->unfocused_border_color = (int)strtol(ufbc, NULL, 16);

    int put = cfg_get_int(global, "pointer_update_time");
    s->config->pointer_update_time = put > 0 ? put : 10;

    double mwa = cfg_get_double(global, "main_window_area");
    s->config->main_window_area = mwa > 0.0 && mwa < 1.0 ? mwa : 0.5;
    
    s->config->set_new_window_main = cfg_get_bool(global, "set_new_window_main");

    const char *button_mod = cfg_get_string(global, "button_mod");
    if (!button_mod) {
        s->config->button_mod = XCB_MOD_MASK_4;
    } else {
        for (size_t i = 0; i < (sizeof(mod_mapping) / sizeof(mod_mapping[0])); ++i) {
            if (strcmp(mod_mapping[i].name, button_mod) == 0) {
                s->config->button_mod = mod_mapping[i].mod;
                break;
            }
        }
    }

    const char *move_button = cfg_get_string(global, "move_button");
    if (!move_button) {
        s->config->move_button = 1;
    } else {
        for (size_t i = 0; i < (sizeof(button_mapping) / sizeof(button_mapping[0])); ++i) {
            if (strcmp(button_mapping[i].name, move_button) == 0) {
                s->config->move_button = button_mapping[i].button;
                break;
            }
        }
    }

    const char *resize_button = cfg_get_string(global, "resize_button");
    if (!resize_button) {
        s->config->resize_button = 2;
    } else {
        for (size_t i = 0; i < (sizeof(button_mapping) / sizeof(button_mapping[0])); ++i) {
            if (strcmp(button_mapping[i].name, resize_button) == 0) {
                s->config->resize_button = button_mapping[i].button;
                break;
            }
        }
    }

    parse_config_desktops(s, global);
    parse_config_keybinds(s, global);

    cfg_config_deinit(cfg);

    log_msg(s, "Config loaded");
}
