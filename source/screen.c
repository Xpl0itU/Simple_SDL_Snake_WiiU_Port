#include "screen.h"

void textToFrameCut(int line, int column, const char *str, int maxWidth)
{
	++line;
	line *= FONT_SIZE;
	line -= 7;
	SDL_Rect text = { .w = FC_GetWidth(font, str), .h = FONT_SIZE, .y = line };

	int i = strlen(str);
	char cpy[++i ];
	if(maxWidth != 0 && text.w > maxWidth)
	{
		char *tmp = cpy;
		OSBlockMove(tmp, str, i, false);
		tmp += i;

		*--tmp = '\0';
		*--tmp = '.';
		*--tmp = '.';
		*--tmp = '.';

		char *tmp2;
		text.w = FC_GetWidth(font, cpy);
		while(text.w > maxWidth)
		{
			tmp2 = tmp;
			*--tmp = '.';
			++tmp2;
			*++tmp2 = '\0';
			text.w = FC_GetWidth(font, cpy);
		}

		if(*--tmp == ' ')
		{
			*tmp = '.';
			tmp[3] = '\0';
		}

		str = cpy;
	}

	switch(column)
	{
		case ALIGNED_CENTER:
			text.x = (screen.x >> 1) - (text.w >> 1);
			break;
		case ALIGNED_RIGHT:
			text.x = screen.x - text.w - FONT_SIZE;
			break;
		default:
			column *= spaceWidth;
			text.x = column + FONT_SIZE;
	}

	FC_DrawBoxColor(font, renderer, text, screenColorToSDLcolor(0xFFFFFFFF), str);
}