#include <SDL2/SDL.h>
#include <SDL_FontCache.h>

#include <coreinit/memory.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define SCREEN_WIDTH 960
#define SCREEN_HEIGHT 720

const SDL_Color BLACK = { .r = 0, .g = 0, .b = 0, .a = 255 };
const SDL_Color WHITE = { .r = 255, .g = 255, .b = 255, .a = 255 };

const SDL_Color RED = { .r = 255, .g = 0, .b = 0, .a = 255 };
const SDL_Color MAROON =  { .r = 128, .g = 0, .b = 0, .a = 255 };

const SDL_Color BLUE = { .r = 0, .g = 0, .b = 255, .a = 255 };
const SDL_Color NAVY = { .r = 0, .g = 0, .b = 128, .a = 255 };

const SDL_Color LIME = { .r = 0, .g = 255, .b = 0, .a = 255 };
const SDL_Color GREEN = { .r = 0, .g = 128, .b = 0, .a = 255 };

#define screenColorToSDLcolor(color) (SDL_Color){ .a = color & 0xFFu, .b = (color & 0x0000FF00u) >> 8, .g = (color & 0x00FF0000u) >> 16, .r = (color & 0xFF000000u) >> 24 }

void draw_screen(SDL_Renderer *renderer)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
}

#define FRAME_RATE 60

int main()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) != 0) {
        fprintf(stderr, "\nUnable to initialize SDL: %s\n", SDL_GetError());
        return 1;
    }
    atexit(SDL_Quit);

    SDL_Window *window =
        SDL_CreateWindow("Snake",
                SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                SCREEN_WIDTH, SCREEN_HEIGHT,
                SDL_WINDOW_SHOWN
        );
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1,
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    // make the scaled rendering look smoother.
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);

    int quit = 0;
    draw_screen(renderer);

    while (!quit) {        

        //code
        void *ttf;
        size_t size;
        OSGetSharedData(OS_SHAREDDATATYPE_FONT_STANDARD, 0, &ttf, &size);
        SDL_RWops *rw = SDL_RWFromMem(ttf, size);      
        FC_Font* font = FC_CreateFont();  
        FC_LoadFont_RW(font, renderer, rw, 1, 28, screenColorToSDLcolor(0xFFFFFFFF), TTF_STYLE_NORMAL); 
        FC_Draw(font, renderer, 0, 0, "This is %s.\n It works.", "example text"); 
        

        draw_screen(renderer);
        SDL_RenderPresent(renderer);
        
        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    return 0;
} 
