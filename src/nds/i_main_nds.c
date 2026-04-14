// NDS entry point: replaces SDL's main() from upstream Chocolate Doom.
//
// This file initializes the NDS hardware, scans the SD card for WAD files,
// presents a selection menu if multiple IWADs are found, then hands control
// to D_DoomMain() which runs the normal Chocolate Doom startup and game loop.
//
// Hardware setup sequence:
//   1. Configure the sub screen as a 4BPP text console (32x24 ANSI grid).
//   2. Mount the FAT filesystem on the SD card via fatInitDefault().
//   3. Scan /doom/ for files matching known IWAD filenames.
//   4. Single WAD: auto-load. Multiple WADs: show a D-pad menu.
//   5. Build a fake argc/argv array and call D_DoomMain().

#include <nds.h>
#include <fat.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>

#include "config.h"
#include "doomtype.h"
#include "d_iwad.h"
#include "m_argv.h"
#include "nds_panel.h"

void D_DoomMain(void);

#define DOOM_DIR "/doom/"

static const char *known_iwads[] = {
	"doom2.wad", "plutonia.wad", "tnt.wad", "doom.wad", "doom1.wad",
	"doom2f.wad", "chex.wad", "hacx.wad", "freedoom2.wad",
	"freedoom1.wad", "freedm.wad", NULL
};

// Case-insensitive match against the known IWAD list.
// WAD filenames on FAT SD cards can appear in any case depending on
// the tool that wrote them, so we compare case-insensitively.
static boolean is_known_iwad(const char *name)
{
	for (int i = 0; known_iwads[i] != NULL; i++)
	{
		if (!strcasecmp(name, known_iwads[i]))
			return true;
	}
	return false;
}

// Scan the /doom/ directory for regular files whose names match a known
// IWAD. Returns the number of WADs found (up to max). Each match is
// stored as a full path, e.g. "/doom/doom2.wad".
static int find_wads(char found[][NDS_WAD_PATH_LEN], int max)
{
	DIR *dir = opendir(DOOM_DIR);
	if (dir == NULL)
		return 0;

	int count = 0;
	struct dirent *entry;

	while ((entry = readdir(dir)) != NULL && count < max)
	{
		if (entry->d_type == DT_REG && is_known_iwad(entry->d_name)
		    && strlen(DOOM_DIR) + strlen(entry->d_name) < NDS_WAD_PATH_LEN)
		{
			snprintf(found[count], NDS_WAD_PATH_LEN, "%s%s", DOOM_DIR, entry->d_name);
			count++;
		}
	}

	closedir(dir);
	return count;
}

static int wad_menu(char wads[][NDS_WAD_PATH_LEN], int count)
{
	int selected = 0;

	while (1)
	{
		NDS_Panel_DrawWADMenu(wads, count, selected);

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

	// Initialize the text console on the sub (bottom) screen.
	// BgType_Text4bpp selects a 4 bits-per-pixel tiled background, which
	// libnds configures as a 32x24 character grid with ANSI escape code
	// support. The last two arguments (false, true) place this console on
	// the sub engine so the top screen remains free for the 3D framebuffer.
	videoSetModeSub(MODE_0_2D);
	vramSetBankC(VRAM_C_SUB_BG);
	consoleInit(NULL, 0, BgType_Text4bpp, BgSize_T_256x256, 22, 3, false, true);

	NDS_Panel_Init();
	NDS_Panel_DrawBoot();

	// Hardware detection
	if (isDSiMode())
	{
		setCpuClock(true);
		NDS_Panel_BootStatus("Hardware", "DSi (16 MB, 134 MHz)", true);
	}
	else
	{
		NDS_Panel_BootStatus("Hardware", "DS Lite (4 MB, 67 MHz)", true);
	}

	// Mount the FAT filesystem on the SD card (or DLDI-patched slot).
	// This must succeed before any fopen/opendir call. Without it the
	// NDS has no access to the card's file system.
	boolean fat_ok = fatInitDefault();
	NDS_Panel_BootStatus("FAT", fat_ok ? "OK" : "FAILED", fat_ok);

	if (!fat_ok)
	{
		NDS_Panel_BootError("Insert SD card with WAD files.");
		NDS_Panel_BootError("Place .wad files in /doom/");
		while (1)
		{
			swiWaitForVBlank();
			scanKeys();
			if (keysDown() & KEY_START)
				return 1;
		}
	}

	// Scan /doom/ for WAD files matching known IWADs.
	char wads[NDS_MAX_WADS][NDS_WAD_PATH_LEN];
	int wad_count = find_wads(wads, NDS_MAX_WADS);

	if (wad_count == 0)
	{
		NDS_Panel_BootError("No WAD files found in /doom/");
		NDS_Panel_BootError("");
		NDS_Panel_BootError("Supported WADs:");
		NDS_Panel_BootError("doom.wad doom1.wad doom2.wad");
		NDS_Panel_BootError("plutonia.wad tnt.wad");
		NDS_Panel_BootError("freedoom1.wad freedoom2.wad");
		while (1)
		{
			swiWaitForVBlank();
			scanKeys();
			if (keysDown() & KEY_START)
				return 1;
		}
	}

	// Single WAD on the card: skip the menu and auto-load it.
	// Multiple WADs: present a D-pad selection menu.
	char *chosen_wad;

	if (wad_count == 1)
	{
		chosen_wad = wads[0];
		const char *name = strrchr(chosen_wad, '/');
		name = name ? name + 1 : chosen_wad;
		NDS_Panel_BootStatus("WAD", name, true);
	}
	else
	{
		int choice = wad_menu(wads, wad_count);
		chosen_wad = wads[choice];
	}

	NDS_Panel_SetWAD(chosen_wad);
	NDS_Panel_DrawLoading(chosen_wad);

	// Chocolate Doom expects a standard argc/argv command line.
	// We synthesize one: {"chocolate-doom-ds", "-iwad", "/doom/chosen.wad"}.
	// myargc/myargv are Doom globals that replace the real argc/argv
	// throughout the engine.
	static char *nds_argv[] = { "chocolate-doom-ds", "-iwad", NULL, NULL };
	nds_argv[2] = chosen_wad;
	myargc = 3;
	myargv = nds_argv;

	D_DoomMain();

	return 0;
}
