#ifndef _DISPLAY_H
#define _DISPLAY_H

#include <SDL2/SDL.h>
#include <stdint.h>

#define SCREEN_WIDTH  64
#define SCREEN_HEIGHT 32
#define SCREEN_SIZE   (SCREEN_WIDTH * SCREEN_HEIGHT)

#define WINDOW_WIDTH  640
#define WINDOW_HEIGHT 320

struct display
{
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
};

int
display_create(struct display*);

void
display_destroy(struct display*);

void
display_render(struct display*, uint8_t*);

#endif
