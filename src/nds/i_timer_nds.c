// NDS timer subsystem.
// Uses VBlank interrupt counting for robust, race-free timing.
// VBlank fires at 59.8261 Hz on NDS hardware.

#include <nds.h>

#include "config.h"
#include "i_timer.h"

#define NDS_VBLANK_HZ 60

static volatile unsigned int vblank_count = 0;

static void vblank_handler(void)
{
	vblank_count++;
}

void I_InitTimer(void)
{
	irqSet(IRQ_VBLANK, vblank_handler);
	irqEnable(IRQ_VBLANK);
}

int I_GetTime(void)
{
	return (int)(vblank_count * TICRATE / NDS_VBLANK_HZ);
}

int I_GetTimeMS(void)
{
	return (int)(vblank_count * 1000 / NDS_VBLANK_HZ);
}

void I_Sleep(int ms)
{
	int frames = ms / 17;
	if (frames < 1)
		frames = 1;
	for (int i = 0; i < frames; i++)
		swiWaitForVBlank();
}

void I_WaitVBL(int count)
{
	for (int i = 0; i < count; i++)
		swiWaitForVBlank();
}
