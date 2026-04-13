// Minimal SDL_stdinc.h shim for NDS
#ifndef SDL_STDINC_H_SHIM
#define SDL_STDINC_H_SHIM

#include <stdlib.h>
#include <string.h>

#define SDL_qsort qsort
#define SDL_free free
#define SDL_malloc malloc

#endif
