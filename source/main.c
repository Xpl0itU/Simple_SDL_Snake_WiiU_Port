#include "screen.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

const SDL_Point screen = { .x = 1280, .y = 720 };
FC_Font *font = NULL;
int32_t spaceWidth;
SDL_Renderer *renderer;
SDL_Texture *display;

void draw_screen(SDL_Renderer *renderer)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
}

int main()
{
    WHBProcInit();
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) != 0) {
        fprintf(stderr, "\nUnable to initialize SDL: %s\n", SDL_GetError());
        return 1;
    }
    atexit(SDL_Quit);

    SDL_Window *window =
        SDL_CreateWindow("SDL",
                SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                SCREEN_WIDTH, SCREEN_HEIGHT,
                SDL_WINDOW_SHOWN
        );
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1,
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    display = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH, SCREEN_HEIGHT);
    SDL_SetRenderTarget(renderer, display);

    // make the scaled rendering look smoother.
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);

    draw_screen(renderer);

    while (WHBProcIsRunning()) {        

        //code
        void *ttf;
        size_t size;
        OSGetSharedData(OS_SHAREDDATATYPE_FONT_STANDARD, 0, &ttf, &size);
        FC_Font* font = FC_CreateFont();  
        SDL_RWops *rw = SDL_RWFromMem(ttf, size);      
        FC_LoadFont_RW(font, renderer, rw, 1, 28, screenColorToSDLcolor(0xFFFFFFFF), TTF_STYLE_NORMAL); 
        
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);

        textToFrameCut(23, 13, "Disclaimer", 0);
        
        SDL_SetRenderTarget(renderer, NULL);        
        SDL_RenderCopy(renderer, display, NULL, NULL);

        SDL_RenderPresent(renderer);

        SDL_Delay(16);
    }

    WHBProcShutdown();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    return 0;
} 
