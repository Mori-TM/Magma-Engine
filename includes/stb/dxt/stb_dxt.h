#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"
#include "math.h"
#include "string.h"
#include "stdbool.h"

// compression mode (bitflags)
#define STB_DXT_NORMAL    0
#define STB_DXT_DITHER    1   // use dithering. dubious win. never use for normal maps and the like!
#define STB_DXT_HIGHQUAL  2   // high quality mode, does two refinement steps instead of 1. ~30-40% slower.

#define STBD_ABS(i)           abs(i)
#define STBD_FABS(x)          fabs(x)
#define STBD_MEMSET           memset

// #define STB_DXT_USE_ROUNDING_BIAS

int stb__Mul8Bit(int a, int b);

void stb__From16Bit(unsigned char *out, unsigned short v);

unsigned short stb__As16Bit(int r, int g, int b);

// linear interpolation at 1/3 point between a and b, using desired rounding type
int stb__Lerp13(int a, int b);

// lerp RGB color
void stb__Lerp13RGB(unsigned char *out, unsigned char *p1, unsigned char *p2);

// compute table to reproduce constant colors as accurately as possible
void stb__PrepareOptTable(unsigned char *Table,const unsigned char *expand,int size);

void stb__EvalColors(unsigned char *color,unsigned short c0,unsigned short c1);

// Block dithering function. Simply dithers a block to 565 RGB.
// (Floyd-Steinberg)
void stb__DitherBlock(unsigned char *dest, unsigned char *block);

// The color matching function
unsigned int stb__MatchColorsBlock(unsigned char *block, unsigned char *color,int dither);

// The color optimization function. (Clever code, part 1)
void stb__OptimizeColorsBlock(unsigned char *block, unsigned short *pmax16, unsigned short *pmin16);

int stb__sclamp(float y, int p0, int p1);

// The refinement function. (Clever code, part 2)
// Tries to optimize colors to suit block contents better.
// (By solving a least squares system via normal equations+Cramer's rule)
int stb__RefineBlock(unsigned char *block, unsigned short *pmax16, unsigned short *pmin16, unsigned int mask);

// Color block compression
void stb__CompressColorBlock(unsigned char *dest, unsigned char *block, int mode);

// Alpha block compression (this is easy for a change)
void stb__CompressAlphaBlock(unsigned char *dest,unsigned char *src, int stride);

void stb__InitDXT();

void stb_compress_dxt_block(unsigned char *dest, const unsigned char *src, int alpha, int mode);

void stb_compress_bc4_block(unsigned char *dest, const unsigned char *src);

void stb_compress_bc5_block(unsigned char *dest, const unsigned char *src);

int imin(int x, int y);

void extract_block(const unsigned char *src, int x, int y, int w, int h, unsigned char *block);

// Get the size of the destination pixels (# bytes)
#ifdef _WIN32
__declspec(dllexport) 
#endif
uint64_t get_compress_pixels_dst_size_bytes(uint64_t w, uint64_t h, bool is_rgba);

// Destination must already be allocated
#ifdef _WIN32
__declspec(dllexport) 
#endif
bool compress_pixels(unsigned char* dst, const unsigned char* src, uint64_t w, uint64_t h, bool is_rgba);
