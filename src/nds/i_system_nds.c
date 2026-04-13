// NDS system services: memory, error handling, misc.

#include <nds.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "config.h"
#include "doomtype.h"
#include "d_event.h"
#include "d_ticcmd.h"
#include "i_system.h"
#include "i_timer.h"
#include "m_config.h"
#include "m_misc.h"

#define MAX_ATEXIT_FUNCS 32

static atexit_func_t atexit_funcs[MAX_ATEXIT_FUNCS];
static int num_atexit_funcs = 0;

void I_AtExit(atexit_func_t func, boolean run_if_error)
{
	(void)run_if_error;
	if (num_atexit_funcs < MAX_ATEXIT_FUNCS)
		atexit_funcs[num_atexit_funcs++] = func;
}

void I_Init(void)
{
	I_InitTimer();
}

byte *I_ZoneBase(int *size)
{
	int heap_size;

	if (isDSiMode())
		heap_size = 10 * 1024 * 1024;
	else
		heap_size = 2 * 1024 * 1024 + 512 * 1024;

	byte *zone = NULL;
	while (heap_size > 1024 * 1024 && zone == NULL)
	{
		zone = (byte *)malloc(heap_size);
		if (zone == NULL)
			heap_size -= 256 * 1024;
	}

	if (zone == NULL)
	{
		printf("\n  Out of memory!");
		while (1) swiWaitForVBlank();
	}

	*size = heap_size;
	return zone;
}

boolean I_ConsoleStdout(void)
{
	return true;
}

ticcmd_t *I_BaseTiccmd(void)
{
	static ticcmd_t emptycmd;
	return &emptycmd;
}

void I_Quit(void)
{
	for (int i = num_atexit_funcs - 1; i >= 0; i--)
		atexit_funcs[i]();
	exit(0);
}

void I_Error(const char *error, ...)
{
	va_list args;

	consoleDemoInit();
	printf("\x1b[2J");
	printf("\x1b[1;1H  FATAL ERROR:\n\n  ");

	va_start(args, error);
	vprintf(error, args);
	va_end(args);

	printf("\n\n  Press START to exit.");

	while (1)
	{
		swiWaitForVBlank();
		scanKeys();
		if (keysDown() & KEY_START)
			break;
	}

	for (int i = num_atexit_funcs - 1; i >= 0; i--)
		atexit_funcs[i]();
	exit(1);
}

void I_Tactile(int on, int off, int total)
{
	(void)on; (void)off; (void)total;
}

void *I_Realloc(void *ptr, size_t size)
{
	return realloc(ptr, size);
}

boolean I_GetMemoryValue(unsigned int offset, void *value, int size)
{
	(void)offset; (void)value; (void)size;
	return false;
}

void I_BindVariables(void) {}

void I_PrintStartupBanner(const char *gamedescription)
{
	printf("\x1b[9;1H  %s", gamedescription);
}

void I_PrintBanner(const char *text)
{
	printf("  %s\n", text);
}

void I_PrintDivider(void)
{
	printf("  ================================\n");
}
