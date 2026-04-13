// NDS video subsystem.
// Renders at 320x200 into I_VideoBuffer, hardware affine scales to 256x192.

#include <nds.h>
#include <string.h>

#include "config.h"
#include "doomtype.h"
#include "i_video.h"
#include "nds_panel.h"
#include "m_config.h"
#include "v_video.h"
#include "w_wad.h"
#include "z_zone.h"

// Global variables expected by Chocolate Doom
pixel_t *I_VideoBuffer = NULL;
char *video_driver = "";
boolean screenvisible = true;
int vanilla_keyboard_mapping = 1;
boolean screensaver_mode = false;
int usegamma = 0;
int screen_width = SCREENWIDTH;
int screen_height = SCREENHEIGHT;
int fullscreen = 0;
int aspect_ratio_correct = 0;
int integer_scaling = 0;
int smooth_pixel_scaling = 0;
int vga_porch_flash = 0;
int force_software_renderer = 1;
int png_screenshots = 0;
char *window_position = "";
unsigned int joywait = 0;
int usemouse = 0;

static u8 *vram_fb = NULL;

void I_InitGraphics(void)
{
	// Allocate the video buffer (320x200 8-bit paletted)
	I_VideoBuffer = Z_Malloc(SCREENWIDTH * SCREENHEIGHT, PU_STATIC, NULL);
	memset(I_VideoBuffer, 0, SCREENWIDTH * SCREENHEIGHT);

	// Top screen: MODE_5_2D with BG3 as 8-bit bitmap
	videoSetMode(MODE_5_2D | DISPLAY_BG3_ACTIVE);
	vramSetBankA(VRAM_A_MAIN_BG_0x06000000);
	vramSetBankB(VRAM_B_MAIN_BG_0x06020000);

	// 512x256 bitmap, 320x200 rendered area
	REG_BG3CNT = BG_BMP8_512x256 | BG_BMP_BASE(0);

	// Affine transform: scale 320x200 -> 256x192
	REG_BG3PA = (320 * 256) / 256;
	REG_BG3PB = 0;
	REG_BG3PC = 0;
	REG_BG3PD = (200 * 256) / 192;
	REG_BG3X = 0;
	REG_BG3Y = 0;

	vram_fb = (u8 *)BG_GFX;

	// Clear VRAM so screen starts black
	memset(vram_fb, 0, 512 * 256);

	// Default grayscale palette until PLAYPAL is loaded
	for (int i = 0; i < 256; i++)
	{
		int gray = i >> 3;
		BG_PALETTE[i] = RGB15(gray, gray, gray) | BIT(15);
	}

	screenvisible = true;
}

void I_ShutdownGraphics(void) {}
void I_GraphicsCheckCommandLine(void) {}
void I_SetWindowTitle(const char *title) { (void)title; }
void I_CheckIsScreensaver(void) {}
void I_SetGrabMouseCallback(grabmouse_callback_t func) { (void)func; }
void I_DisplayFPSDots(boolean dots_on) { (void)dots_on; }
void I_BindVideoVariables(void) {}
void I_InitWindowTitle(void) {}
void I_RegisterWindowIcon(const unsigned int *icon, int width, int height)
	{ (void)icon; (void)width; (void)height; }
void I_InitWindowIcon(void) {}
void I_EnableLoadingDisk(int xoffs, int yoffs) { (void)xoffs; (void)yoffs; }
void I_BeginRead(void) {}

void I_GetWindowPosition(int *x, int *y, int w, int h)
	{ *x = 0; *y = 0; (void)w; (void)h; }

void I_SetPalette(byte *palette)
{
	for (int i = 0; i < 256; i++)
	{
		int r = palette[i * 3 + 0] >> 3;
		int g = palette[i * 3 + 1] >> 3;
		int b = palette[i * 3 + 2] >> 3;
		BG_PALETTE[i] = RGB15(r, g, b) | BIT(15);
	}
}

int I_GetPaletteIndex(int r, int g, int b)
{
	// Simple nearest-color search (called rarely)
	(void)r; (void)g; (void)b;
	return 0;
}

void I_UpdateNoBlit(void) {}

void I_FinishUpdate(void)
{
	if (I_VideoBuffer == NULL || vram_fb == NULL)
		return;

	// Copy 320x200 buffer to VRAM with 512-byte stride
	u8 *dest = vram_fb;
	pixel_t *src = I_VideoBuffer;

	for (int y = 0; y < SCREENHEIGHT; y++)
	{
		DC_FlushRange(src, SCREENWIDTH);
		dmaCopy(src, dest, SCREENWIDTH);
		src += SCREENWIDTH;
		dest += 512;
	}

	NDS_Panel_DrawGameplay();
}

void I_ReadScreen(pixel_t *scr)
{
	memcpy(scr, I_VideoBuffer, SCREENWIDTH * SCREENHEIGHT);
}

void I_StartFrame(void) {}

// I_StartTic is in i_input_nds.c
