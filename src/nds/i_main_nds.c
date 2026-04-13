// NDS entry point and hardware initialization.

#include <nds.h>
#include <fat.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>

#include "config.h"
#include "doomtype.h"
#include "d_iwad.h"
#include "m_argv.h"

void D_DoomMain(void);

#define DOOM_DIR "/doom/"
#define MAX_WADS 16

static const char *known_iwads[] = {
	"doom2.wad", "plutonia.wad", "tnt.wad", "doom.wad", "doom1.wad",
	"doom2f.wad", "chex.wad", "hacx.wad", "freedoom2.wad",
	"freedoom1.wad", "freedm.wad", NULL
};

static boolean is_known_iwad(const char *name)
{
	for (int i = 0; known_iwads[i] != NULL; i++)
	{
		if (!strcasecmp(name, known_iwads[i]))
			return true;
	}
	return false;
}

// Scan /doom/ for IWAD files. Returns the number found.
#define WAD_PATH_LEN 64

static int find_wads(char found[][WAD_PATH_LEN], int max)
{
	DIR *dir = opendir(DOOM_DIR);
	if (dir == NULL)
		return 0;

	int count = 0;
	struct dirent *entry;

	while ((entry = readdir(dir)) != NULL && count < max)
	{
		if (entry->d_type == DT_REG && is_known_iwad(entry->d_name)
		    && strlen(DOOM_DIR) + strlen(entry->d_name) < WAD_PATH_LEN)
		{
			snprintf(found[count], WAD_PATH_LEN, "%s%s", DOOM_DIR, entry->d_name);
			count++;
		}
	}

	closedir(dir);
	return count;
}

// Show a WAD selection menu on the bottom screen. Returns the index chosen.
static int wad_menu(char wads[][WAD_PATH_LEN], int count)
{
	int selected = 0;

	while (1)
	{
		printf("\x1b[7;1H  Select a WAD file:\n\n");

		for (int i = 0; i < count; i++)
		{
			const char *name = wads[i] + strlen(DOOM_DIR);
			if (i == selected)
				printf("  > %s    \n", name);
			else
				printf("    %s    \n", name);
		}

		swiWaitForVBlank();
		scanKeys();
		u32 keys = keysDown();

		if (keys & KEY_UP)
			selected = (selected - 1 + count) % count;
		if (keys & KEY_DOWN)
			selected = (selected + 1) % count;
		if (keys & KEY_A || keys & KEY_START)
			return selected;
	}
}

int main(void)
{
	defaultExceptionHandler();

	setvbuf(stdout, NULL, _IONBF, 0);

	videoSetModeSub(MODE_0_2D);
	vramSetBankC(VRAM_C_SUB_BG);
	consoleInit(NULL, 0, BgType_Text4bpp, BgSize_T_256x256, 22, 3, false, true);

	printf("\x1b[2J");
	printf("\x1b[1;1H================================");
	printf("\x1b[2;1H  Chocolate Doom DS v0.2.0");
	printf("\x1b[3;1H================================");

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
		printf("\x1b[10;1H  Place .wad files in /doom/");
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

	// Scan for WADs
	char wads[MAX_WADS][WAD_PATH_LEN];
	int wad_count = find_wads(wads, MAX_WADS);

	if (wad_count == 0)
	{
		printf("\x1b[9;1H  No WAD files found in /doom/");
		printf("\x1b[11;1H  Supported WADs:");
		printf("\x1b[12;1H  doom.wad, doom1.wad, doom2.wad");
		printf("\x1b[13;1H  plutonia.wad, tnt.wad");
		printf("\x1b[14;1H  freedoom1.wad, freedoom2.wad");
		printf("\x1b[15;1H  chex.wad, hacx.wad, freedm.wad");
		printf("\x1b[23;1H  Press START to exit.");
		while (1)
		{
			swiWaitForVBlank();
			scanKeys();
			if (keysDown() & KEY_START)
				return 1;
		}
	}

	char *chosen_wad;

	if (wad_count == 1)
	{
		chosen_wad = wads[0];
		printf("\x1b[9;1H  Loading %s", chosen_wad + strlen(DOOM_DIR));
	}
	else
	{
		int choice = wad_menu(wads, wad_count);
		chosen_wad = wads[choice];
		printf("\x1b[2J");
		printf("\x1b[1;1H================================");
		printf("\x1b[2;1H  Chocolate Doom DS v0.2.0");
		printf("\x1b[3;1H================================");
		printf("\x1b[9;1H  Loading %s", chosen_wad + strlen(DOOM_DIR));
	}

	static char *nds_argv[] = { "chocolate-doom-ds", "-iwad", NULL, NULL };
	nds_argv[2] = chosen_wad;
	myargc = 3;
	myargv = nds_argv;

	D_DoomMain();

	return 0;
}
