// Minimal SDL_endian.h shim for NDS (ARM is little-endian)
#ifndef SDL_ENDIAN_H_SHIM
#define SDL_ENDIAN_H_SHIM

#define SDL_LIL_ENDIAN 1234
#define SDL_BIG_ENDIAN 4321
#define SDL_BYTEORDER SDL_LIL_ENDIAN

// NDS is little-endian, so LE swaps are no-ops
#define SDL_SwapLE16(x) (x)
#define SDL_SwapLE32(x) (x)

// BE swaps needed for MIDI file parsing
static inline unsigned short SDL_SwapBE16(unsigned short x)
{
	return (x >> 8) | (x << 8);
}

static inline unsigned int SDL_SwapBE32(unsigned int x)
{
	return ((x >> 24) & 0xFF) |
	       ((x >> 8)  & 0xFF00) |
	       ((x << 8)  & 0xFF0000) |
	       ((x << 24) & 0xFF000000);
}

#endif
