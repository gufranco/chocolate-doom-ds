// NDS bottom screen panel: boot, WAD selector, and gameplay HUD.

#ifndef NDS_PANEL_H
#define NDS_PANEL_H

#include "doomtype.h"

#define NDS_WAD_PATH_LEN 64
#define NDS_MAX_WADS 16

void NDS_Panel_Init(void);
void NDS_Panel_DrawBoot(void);
void NDS_Panel_BootStatus(const char *label, const char *value, boolean ok);
void NDS_Panel_BootError(const char *message);
void NDS_Panel_DrawLoading(const char *wadname);

void NDS_Panel_DrawWADMenu(char wads[][NDS_WAD_PATH_LEN], int count, int selected);

void NDS_Panel_SetWAD(const char *wadname);
void NDS_Panel_SetMap(const char *mapname);
void NDS_Panel_DrawGameplay(void);

void NDS_Panel_FatalError(const char *error);

int NDS_SoundCacheCount(void);

#endif
