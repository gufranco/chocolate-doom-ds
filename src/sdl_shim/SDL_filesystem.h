// Minimal SDL_filesystem.h shim for NDS
#ifndef SDL_FILESYSTEM_H_SHIM
#define SDL_FILESYSTEM_H_SHIM

#include <stdlib.h>
#include <string.h>

// NDS has no user home directory. Return /doom/ as config path.
static inline char *SDL_GetPrefPath(const char *org, const char *app)
{
	(void)org; (void)app;
	char *path = (char *)malloc(16);
	if (path) strcpy(path, "/doom/");
	return path;
}

#define SDL_free free

#endif
