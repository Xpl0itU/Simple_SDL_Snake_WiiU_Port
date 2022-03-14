#include "screen.h"

uint8_t *ttfFont;
FT_Library  library;
RGBAColor fcolor = {0xFFFFFFFF};
FT_Face     face;

bool initFont(void* fontBuf, FT_Long fsize) {
    FT_Long size = fsize;
	if (fontBuf) {
		ttfFont = fontBuf;
	} else {
    	OSGetSharedData(2, 0, &ttfFont, &size);
	}

	FT_Error error;
	error = FT_Init_FreeType(&library);
	if (error) return false;

	error = FT_New_Memory_Face(library, ttfFont, size, 0, &face);
	if (error) return false;

	error = FT_Set_Pixel_Sizes(face, 0, 22);   //pixel width, height
	if (error) return false;

	return true;
}

void drawPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
	uint32_t num = (r << 24) | (g << 16) | (b << 8) | a;
	OSScreenPutPixelEx(0, x, y, num);
	OSScreenPutPixelEx(1, x, y, num);
}

void draw_bitmap(FT_Bitmap* bitmap, FT_Int x, FT_Int y) {
	FT_Int i, j, p, q;
	FT_Int x_max;
	FT_Int y_max = y + bitmap->rows;

	switch(bitmap->pixel_mode) {
		case FT_PIXEL_MODE_GRAY:
			x_max = x + bitmap->width;
			for (i = x, p = 0; i < x_max; i++, p++) {
				for (j = y, q = 0; j < y_max; j++, q++) {
					if (i < 0 || j < 0 || i >= 854 || j >= 480) continue;
					uint8_t col = bitmap->buffer[q * bitmap->pitch + p];
					if (col == 0) continue;
					float opacity = col / 255.0;
					drawPixel(i, j, fcolor.r, fcolor.g, fcolor.b, (uint8_t)(fcolor.a * opacity));
				}
			}
			break;
		case FT_PIXEL_MODE_LCD:
			x_max = x + bitmap->width / 3;
			for (i = x, p = 0; i < x_max; i++, p++) {
				for (j = y, q = 0; j < y_max; j++, q++) {
					if (i < 0 || j < 0 || i >= 854 || j >= 480) continue;
					uint8_t cr = bitmap->buffer[q * bitmap->pitch + p * 3];
					uint8_t cg = bitmap->buffer[q * bitmap->pitch + p * 3 + 1];
					uint8_t cb = bitmap->buffer[q * bitmap->pitch + p * 3 + 2];

					if ((cr | cg | cb) == 0) continue;
					drawPixel(i, j, cr, cg, cb, 255);
				}
			}
			break;
		case FT_PIXEL_MODE_BGRA:
			x_max = x + bitmap->width/2;
			for (i = x, p = 0; i < x_max; i++, p++) {
				for (j = y, q = 0; j < y_max; j++, q++) {
					if (i < 0 || j < 0 || i >= 854 || j >= 480) continue;
					uint8_t cb = bitmap->buffer[q * bitmap->pitch + p * 4];
					uint8_t cg = bitmap->buffer[q * bitmap->pitch + p * 4 + 1];
					uint8_t cr = bitmap->buffer[q * bitmap->pitch + p * 4 + 2];
					uint8_t ca = bitmap->buffer[q * bitmap->pitch + p * 4 + 3];

					if ((cr | cg | cb) == 0) continue;
					drawPixel(i, j, cr, cg, cb, ca);
				}
			}
			break;
	}
}


int ttfPrintString(int x, int y, char *string, bool wWrap, bool ceroX) {
	FT_GlyphSlot slot = face->glyph;
	FT_Error error;
	int pen_x = x, pen_y = y;
	FT_UInt previous_glyph;

    while(*string) {
		uint32_t buf = *string++;
		int dy = 0;

		if ((buf >> 6) == 3) {
			if ((buf & 0xF0) == 0xC0) {
				uint8_t b1 = buf & 0xFF, b2 = *string++;
				if ((b2 & 0xC0) == 0x80) b2 &= 0x3F;
				buf = ((b1 & 0xF) << 6) | b2;
			} else if ((buf & 0xF0) == 0xD0) {
				uint8_t b1 = buf & 0xFF, b2 = *string++;
				if ((b2 & 0xC0) == 0x80) b2 &= 0x3F;
				buf = 0x400 | ((b1 & 0xF) << 6) | b2;
			} else if ((buf & 0xF0) == 0xE0) {
				uint8_t b1 = buf & 0xFF, b2 = *string++, b3 = *string++;
				if ((b2 & 0xC0) == 0x80) b2 &= 0x3F;
				if ((b3 & 0xC0) == 0x80) b3 &= 0x3F;
				buf = ((b1 & 0xF) << 12) | (b2 << 6) | b3;
			}
		} else if (buf & 0x80) {
			string++;
			continue;
		}

		if (buf == '\n') {
			pen_y += (face->size->metrics.height >> 6);
			if (ceroX) pen_x = 0;
			else pen_x = x;
			continue;
		}

		//error = FT_Load_Char(face, buf, FT_LOAD_RENDER);

        FT_UInt glyph_index;
		glyph_index = FT_Get_Char_Index(face, buf);

		if (FT_HAS_KERNING(face)) {
			FT_Vector vector;
			FT_Get_Kerning(face, previous_glyph, glyph_index, FT_KERNING_DEFAULT, &vector);
			pen_x += (vector.x >> 6);
			dy = vector.y >> 6;
		}

		error = FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);//FT_LOAD_COLOR);//
		if (error)
			continue;

		error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);//FT_RENDER_MODE_LCD);//
		if (error)
			continue;

		if ((pen_x + (slot->advance.x >> 6)) > 853) {
			if (wWrap) {
				pen_y += (face->size->metrics.height >> 6);
				if (ceroX) pen_x = 0;
				else pen_x = x;
			} else {
				return pen_x;
			}
		}

		draw_bitmap(&slot->bitmap, pen_x + slot->bitmap_left, (face->height >> 6) + pen_y - slot->bitmap_top);

		pen_x += (slot->advance.x >> 6);
		previous_glyph = glyph_index;
	}
	return pen_x;
}

int ttfStringWidth(char *string, int8_t part) {
	FT_GlyphSlot slot = face->glyph;
	FT_Error error;
	int pen_x = 0, max_x = 0, spart = 1;
	FT_UInt previous_glyph;

    while(*string) {
		uint32_t buf = *string++;
		if ((buf >> 6) == 3) {
			if ((buf & 0xF0) == 0xC0) {
				uint8_t b1 = buf & 0xFF, b2 = *string++;
				if ((b2 & 0xC0) == 0x80) b2 &= 0x3F;
				buf = ((b1 & 0xF) << 6) | b2;
			} else if ((buf & 0xF0) == 0xD0) {
				uint8_t b1 = buf & 0xFF, b2 = *string++;
				if ((b2 & 0xC0) == 0x80) b2 &= 0x3F;
				buf = 0x400 | ((b1 & 0xF) << 6) | b2;
			} else if ((buf & 0xF0) == 0xE0) {
				uint8_t b1 = buf & 0xFF, b2 = *string++, b3 = *string++;
				if ((b2 & 0xC0) == 0x80) b2 &= 0x3F;
				if ((b3 & 0xC0) == 0x80) b3 &= 0x3F;
				buf = ((b1 & 0xF) << 12) | (b2 << 6) | b3;
			}
		} else if (buf & 0x80) {
			string++;
			continue;
		}

		//error = FT_Load_Char(face, buf, FT_LOAD_RENDER);

        FT_UInt glyph_index;
		glyph_index = FT_Get_Char_Index(face, buf);

		if (FT_HAS_KERNING(face)) {
			FT_Vector vector;
			FT_Get_Kerning(face, previous_glyph, glyph_index, FT_KERNING_DEFAULT, &vector);
			pen_x += (vector.x >> 6);
		}

		if (buf == '\n') {
			if (part != 0) {
				if ((part > 0) && (spart == part)) return pen_x;
				if (part == -2) max_x = max(pen_x, max_x);
				pen_x = 0;
				spart++;
			}
			continue;
		}

		error = FT_Load_Glyph(face, glyph_index, FT_LOAD_BITMAP_METRICS_ONLY);
		if (error)
			continue;

		pen_x += (slot->advance.x >> 6);
		previous_glyph = glyph_index;
	}
	if (spart < part) pen_x = 0;
	return max(pen_x, max_x);
}

void console_print_pos_aligned(int y, uint16_t offset, uint8_t align, const char* format, ...) {
	char* tmp = NULL;
	int x = 0;

	va_list va;
	va_start(va, format);
	if ((vasprintf(&tmp, format, va) >= 0) && tmp) {
		switch(align) {
			case 0: x = (offset * 12); break;
			case 1: x = (853 - ttfStringWidth(tmp, -2)) / 2; break;
			case 2: x = 853 - (offset * 12) - ttfStringWidth(tmp, 0); break;
			default:  x = (853 - ttfStringWidth(tmp, -2)) / 2; break;
		}
		ttfPrintString(x, (y + 1) * 24, tmp, false, false);
	}
	va_end(va);
	if (tmp) free(tmp);
}