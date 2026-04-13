// NDS input subsystem.
// Maps DS buttons directly to gamekeydown[] and posts events for menus.

#include <nds.h>

#include "config.h"
#include "doomtype.h"
#include "doomkeys.h"
#include "d_event.h"
#include "SDL.h"
#include "i_video.h"

// Chocolate Doom uses D_PostEvent for the event queue
void D_PostEvent(event_t *ev);

// Joystick stubs (defined in i_joystick.c on SDL platforms)
int use_analog = 0;
int joystick_move_sensitivity = 0;
int joystick_turn_sensitivity = 0;

// Direct access to key state (defined in g_game.c or d_event.c)
// Chocolate Doom uses the event system properly, so we post events.

// Button-to-key mapping
typedef struct
{
	int nds_key;
	int doom_key;
} keymap_t;

static const keymap_t keymap[] = {
	{ KEY_UP,     KEY_UPARROW },
	{ KEY_DOWN,   KEY_DOWNARROW },
	{ KEY_LEFT,   KEY_LEFTARROW },
	{ KEY_RIGHT,  KEY_RIGHTARROW },
	{ KEY_A,      KEY_RCTRL },     // Fire
	{ KEY_B,      ' ' },           // Use/Open (space)
	{ KEY_X,      KEY_RSHIFT },    // Run
	{ KEY_Y,      '1' },           // Weapon 1
	{ KEY_L,      ',' },           // Strafe left
	{ KEY_R,      '.' },           // Strafe right
	{ KEY_START,  KEY_ESCAPE },    // Menu
	{ KEY_SELECT, KEY_TAB },       // Automap
};

#define NUM_KEYMAPS (sizeof(keymap) / sizeof(keymap[0]))

static int prev_buttons = 0;

void I_StartTic(void)
{
	scanKeys();
	int held = keysHeld();
	int pressed = held & ~prev_buttons;
	int released = prev_buttons & ~held;
	prev_buttons = held;

	event_t ev;

	for (unsigned int i = 0; i < NUM_KEYMAPS; i++)
	{
		if (pressed & keymap[i].nds_key)
		{
			ev.type = ev_keydown;
			ev.data1 = keymap[i].doom_key;
			ev.data2 = ev.data3 = 0;
			D_PostEvent(&ev);
		}
		if (released & keymap[i].nds_key)
		{
			ev.type = ev_keyup;
			ev.data1 = keymap[i].doom_key;
			ev.data2 = ev.data3 = 0;
			D_PostEvent(&ev);
		}
	}

	// A button also sends ENTER for menu confirmation
	if (pressed & KEY_A)
	{
		ev.type = ev_keydown;
		ev.data1 = KEY_ENTER;
		ev.data2 = ev.data3 = 0;
		D_PostEvent(&ev);
	}
	if (released & KEY_A)
	{
		ev.type = ev_keyup;
		ev.data1 = KEY_ENTER;
		ev.data2 = ev.data3 = 0;
		D_PostEvent(&ev);
	}

	// Touch screen as mouse for turning
	static int touch_active = 0;
	static int touch_last_x = 0;
	if (held & KEY_TOUCH)
	{
		touchPosition touch;
		touchRead(&touch);
		if (touch_active)
		{
			int dx = touch.px - touch_last_x;
			if (dx != 0)
			{
				ev.type = ev_mouse;
				ev.data1 = 0;
				ev.data2 = dx * 8;
				ev.data3 = 0;
				D_PostEvent(&ev);
			}
		}
		touch_last_x = touch.px;
		touch_active = 1;
	}
	else
	{
		touch_active = 0;
	}
}

// Text input stubs (used by save game name entry in menus)
void I_StartTextInput(int x1, int y1, int x2, int y2)
{
	(void)x1; (void)y1; (void)x2; (void)y2;
}

void I_StopTextInput(void) {}

// SDL input handler stubs (declared in i_input.h but not used on NDS)
void I_HandleKeyboardEvent(SDL_Event *sdlevent) { (void)sdlevent; }
void I_HandleMouseEvent(SDL_Event *sdlevent) { (void)sdlevent; }
