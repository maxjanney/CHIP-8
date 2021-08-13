#include "display.h"

int
display_create(struct display* d)
{
    if (SDL_Init(SDL_INIT_VIDEO)) {
        return 1;
    }

    d->window = SDL_CreateWindow("CHIP-8",
                                 SDL_WINDOWPOS_CENTERED,
                                 SDL_WINDOWPOS_CENTERED,
                                 640,
                                 320,
                                 SDL_WINDOW_SHOWN);
    if (!d->window) {
        return 1;
    }

    d->renderer = SDL_CreateRenderer(d->window, -1, SDL_RENDERER_ACCELERATED);
    if (!d->renderer) {
        return 1;
    }

    d->texture = SDL_CreateTexture(d->renderer,
                                   SDL_PIXELFORMAT_RGBA8888,
                                   SDL_TEXTUREACCESS_STREAMING,
                                   SCREEN_WIDTH,
                                   SCREEN_HEIGHT);
    if (!d->texture) {
        return 1;
    }

    return 0;
}

void
display_destroy(struct display* d)
{
    if (d->texture) {
        SDL_DestroyTexture(d->texture);
    }

    if (d->renderer) {
        SDL_DestroyRenderer(d->renderer);
    }

    if (d->window) {
        SDL_DestroyWindow(d->window);
    }

    SDL_Quit();
}

void
display_render(struct display* d, uint8_t* from)
{
    int pitch;
    uint8_t* pixels;

    SDL_LockTexture(d->texture, NULL, (void**)&pixels, &pitch);
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        Uint32* p = (Uint32*)(pixels + pitch * y);
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            *p = from[x + y * SCREEN_WIDTH] ? -1 : 0;
            p += 1;
        }
    }
    SDL_UnlockTexture(d->texture);

    SDL_RenderClear(d->renderer);
    SDL_RenderCopy(d->renderer, d->texture, NULL, NULL);
    SDL_RenderPresent(d->renderer);
}
