// NDS entry point and hardware initialization.

#include <nds.h>
#include <fat.h>
#include <stdio.h>

#include "config.h"
#include "doomtype.h"
#include "m_argv.h"

void D_DoomMain(void);

int main(void)
{
	defaultExceptionHandler();

	// Force unbuffered stdout so all printf appears immediately
	setvbuf(stdout, NULL, _IONBF, 0);

	// Console on sub screen (survives main screen video mode change)
	videoSetModeSub(MODE_0_2D);
	vramSetBankC(VRAM_C_SUB_BG);
	consoleInit(NULL, 0, BgType_Text4bpp, BgSize_T_256x256, 22, 3, false, true);

	printf("\x1b[2J");
	printf("\x1b[1;1H================================");
	printf("\x1b[2;1H  Chocolate Doom DS v0.1.0");
	printf("\x1b[3;1H================================");

	// Detect hardware
	if (isDSiMode())
	{
		setCpuClock(true);
		printf("\x1b[5;1H  Hardware: DSi (16 MB, 134 MHz)");
	}
	else
	{
		printf("\x1b[5;1H  Hardware: DS Lite (4 MB, 67 MHz)");
	}

	printf("\x1b[7;1H  Initializing FAT...");

	if (!fatInitDefault())
	{
		printf(" FAILED!");
		printf("\x1b[9;1H  Insert SD card with WAD files.");
		printf("\x1b[10;1H  Place doom1.wad in /doom/");
		printf("\x1b[23;1H  Press START to exit.");
		while (1)
		{
			swiWaitForVBlank();
			scanKeys();
			if (keysDown() & KEY_START)
				return 1;
		}
	}

	printf(" OK");
	printf("\x1b[8;1H  Starting Doom...");

	// Set up command line
	// -iwad: explicit WAD path (Chocolate Doom's finder doesn't search /doom/)
	static char *nds_argv[] = { "chocodoom-ds", "-iwad", "/doom/doom1.wad", NULL };
	myargc = 3;
	myargv = nds_argv;

	D_DoomMain();

	return 0;
}
