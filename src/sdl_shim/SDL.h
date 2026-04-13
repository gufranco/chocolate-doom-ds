// Minimal SDL.h shim for NDS
#ifndef SDL_H_SHIM
#define SDL_H_SHIM

#include "SDL_stdinc.h"
#include "SDL_endian.h"

// Minimal event types needed by i_input.h function signatures
typedef enum
{
	SDL_KEYDOWN,
	SDL_KEYUP,
	SDL_MOUSEMOTION,
	SDL_MOUSEBUTTONDOWN,
	SDL_MOUSEBUTTONUP,
	SDL_QUIT
} SDL_EventType;

typedef struct
{
	int type;
	int padding[16];
} SDL_Event;

// Minimal joystick types
typedef int SDL_JoystickID;

#endif
