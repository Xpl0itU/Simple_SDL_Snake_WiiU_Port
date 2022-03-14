#define __STDC_WANT_LIB_EXT2__ 1
#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <malloc.h>

#include <ft2build.h>
#include FT_FREETYPE_H

typedef union _RGBAColor {
    uint32_t c;
    struct {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;
    };
} RGBAColor;

#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif

bool initFont(void* fontBuf, FT_Long fsize);
void drawPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
void draw_bitmap(FT_Bitmap* bitmap, FT_Int x, FT_Int y);
int ttfPrintString(int x, int y, char *string, bool wWrap, bool ceroX);
int ttfStringWidth(char *string, int8_t part);
void console_print_pos_aligned(int y, uint16_t offset, uint8_t align, const char* format, ...);