#include <string.h>

#include "SDL2/SDL.h"

#include "frontend.h"

static SDL_Window *window = NULL;
static SDL_Surface *windowSurface = NULL;
static SDL_Surface *screenSurface = NULL;

static int screenWidth = 720;
static int screenHeight = 400;

int frontend_init();
int frontend_resize(int width, int height);
void frontend_update(const color_t *pixels);

int frontend_init() {
    if(SDL_Init(SDL_INIT_VIDEO)) {
        fprintf(stderr, "Failed to initialize SDL2: %s\n", SDL_GetError());
        return 1;
    }

    window = SDL_CreateWindow(
        "PCBox",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        screenWidth,
        screenHeight,
        SDL_WINDOW_RESIZABLE
    );

    if(!window) {
        fprintf(stderr, "Failed to create window: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    windowSurface = SDL_GetWindowSurface(window);

    if(!windowSurface) {
        fprintf(stderr, "Failed to get SDL_Window surface: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    screenSurface = SDL_CreateRGBSurface(
        0,
        screenWidth,
        screenHeight,
        32,
        0x000000ff,
        0x0000ff00,
        0x00ff0000,
        0xff000000
    );

    if(!screenSurface) {
        fprintf(stderr, "Failed to create screen surface: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    return 0;
}

int frontend_resize(int width, int height) {
    SDL_FreeSurface(screenSurface);

    screenSurface = SDL_CreateRGBSurface(
        0,
        width,
        height,
        32,
        0x000000ff,
        0x0000ff00,
        0x00ff0000,
        0xff000000
    );

    if(!screenSurface) {
        fprintf(stderr, "Failed to resize screen surface: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_SetWindowSize(window, width, height);

    screenWidth = width;
    screenHeight = height;

    return 0;
}

void frontend_update(const color_t *pixels) {
    memcpy(screenSurface->pixels, pixels, screenWidth * screenHeight * 4);
}
