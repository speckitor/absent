#define MIN_WIDTH 300
#define MIN_HEIGHT 150

#define BORDER_WIDTH 3
#define FOCUSED_BORDER_COLOR 0xf38ba8
#define UNFOCUSED_BORDER_COLOR 0x9399b2

#define ALT     XCB_MOD_MASK_1
#define SUPER   XCB_MOD_MASK_4
#define SHIFT   XCB_MOD_MASK_SHIFT
#define CONTROL XCB_MOD_MASK_CONTROL

#define BUTTON_MOD XCB_MOD_MASK_4

// enable autostart script running (autostartabsent)
// 1 for enable, 0 for disable
#define ENABLE_AUTOSTART 1

keybind_t keybinds[] = {
	{XK_t, SUPER, spawnclient, "kitty"},
	{XK_f, SUPER, spawnclient, "firefox"},
	{XK_r, SUPER, spawnclient, "rofi -show drun"},
	{XK_c, SUPER, killclient, NULL},
	{XK_Escape, SUPER, killwm, NULL},
	{XK_f, SUPER | SHIFT, fullscreen, NULL}
};
