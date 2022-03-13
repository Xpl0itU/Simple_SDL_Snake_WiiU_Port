#include <SDL2/SDL.h>
#include <SDL_FontCache.h>

#include <coreinit/memory.h>
#include <whb/proc.h>

#define SCREEN_WIDTH 960
#define SCREEN_HEIGHT 720

#define screenColorToSDLcolor(color) (SDL_Color){ .a = color & 0xFFu, .b = (color & 0x0000FF00u) >> 8, .g = (color & 0x00FF0000u) >> 16, .r = (color & 0xFF000000u) >> 24 }
#define FONT_SIZE 28
#define MAX_CHARS 152 //TODO: This is here for historical reasons and only valid for spaces now
#define MAX_LINES 28

#define ALIGNED_RIGHT MAX_CHARS
#define ALIGNED_CENTER MAX_CHARS + 1
#define FRAME_RATE 60

extern const SDL_Point screen;
extern FC_Font *font;
extern int32_t spaceWidth;
extern SDL_Renderer *renderer;

void textToFrameCut(int line, int column, const char *str, int maxWidth);