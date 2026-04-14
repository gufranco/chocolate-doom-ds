// ARM7 core for Chocolate Doom DS.
// Based on the BlocksDS default ARM7 implementation.
// Enables MaxMod for sound effect playback via soundPlaySample().

#include <nds.h>
#include <maxmod7.h>

volatile bool exit_loop = false;

void power_button_callback(void)
{
	exit_loop = true;
}

void vblank_handler(void)
{
	inputGetAndSend();
}

int main(void)
{
#if defined(DEBUG_LIBS)
	defaultExceptionHandler();
#endif

	enableSound();
	readUserSettings();
	ledBlink(0);
	touchInit();

	irqInit();
	fifoInit();

	installSoundFIFO();
	installSystemFIFO();

	if (isDSiMode())
		installCameraFIFO();

	mmInstall(FIFO_MAXMOD);

	setPowerButtonCB(power_button_callback);
	initClockIRQTimer(LIBNDS_DEFAULT_TIMER_RTC);

	irqSet(IRQ_VBLANK, vblank_handler);
	irqEnable(IRQ_VBLANK);

	while (!exit_loop)
	{
		const uint16_t key_mask = KEY_SELECT | KEY_START | KEY_L | KEY_R;
		uint16_t keys_pressed = ~REG_KEYINPUT;

		if ((keys_pressed & key_mask) == key_mask)
			exit_loop = true;

		swiWaitForVBlank();
	}

	return 0;
}
