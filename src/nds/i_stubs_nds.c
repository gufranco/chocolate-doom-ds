// Stubs for functions not needed on NDS.
// Joystick, mouse acceleration, network SDL module, etc.

#include <stddef.h>
#include "config.h"
#include "doomtype.h"
#include "d_event.h"
#include "i_joystick.h"
#include "net_io.h"

// Joystick
void I_InitJoystick(void) {}
void I_ShutdownJoystick(void) {}
void I_UpdateJoystick(void) {}
void I_BindJoystickVariables(void) {}

// Input variables
void I_BindInputVariables(void) {}
int mouse_acceleration = 0;
int mouse_threshold = 0;

// Endoom screen
void I_Endoom(byte *data) { (void)data; }

// Network SDL transport (NDS has no network)
net_module_t net_sdl_module = { NULL, NULL, NULL, NULL, NULL };

// NET_WaitForLaunch (from net_gui.c which needs SDL GUI)
void NET_WaitForLaunch(void) {}

// I_StartMultiGlob stub (glob not needed on NDS)
#include "i_glob.h"
#include <stdarg.h>
glob_t *I_StartMultiGlob(const char *directory, int flags,
                          const char *glob, ...)
{
	(void)directory; (void)flags; (void)glob;
	return NULL;
}

const char *I_NextGlob(glob_t *glob)
{
	(void)glob;
	return NULL;
}

void I_EndGlob(glob_t *glob)
{
	(void)glob;
}
