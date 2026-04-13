// NDS timer subsystem.

#include <nds.h>

#include "config.h"
#include "i_timer.h"

static unsigned int start_ticks = 0;

// NDS timer tick rate: BUS_CLOCK/1024 = ~32728 Hz
static inline unsigned int NDS_GetTicks(void)
{
	return TIMER0_DATA | (TIMER1_DATA << 16);
}

// Returns milliseconds since init
static unsigned int NDS_GetMS(void)
{
	// 32728 ticks/sec, so ticks * 1000 / 32728
	// Simplified: ticks / 33 (close approximation)
	unsigned int ticks = NDS_GetTicks() - start_ticks;
	return ticks * 1000 / 32729;
}

void I_InitTimer(void)
{
	// Cascaded 32-bit free-running counter
	TIMER0_DATA = 0;
	TIMER0_CR = TIMER_ENABLE | TIMER_DIV_1024;
	TIMER1_DATA = 0;
	TIMER1_CR = TIMER_ENABLE | TIMER_CASCADE;

	start_ticks = NDS_GetTicks();
}

int I_GetTime(void)
{
	return (int)(NDS_GetMS() * TICRATE / 1000);
}

int I_GetTimeMS(void)
{
	return (int)NDS_GetMS();
}

void I_Sleep(int ms)
{
	unsigned int target = NDS_GetMS() + ms;
	while (NDS_GetMS() < target)
		swiWaitForVBlank();
}

void I_WaitVBL(int count)
{
	for (int i = 0; i < count; i++)
		swiWaitForVBlank();
}
