#include <SDL2/SDL.h>

static const char key_map[] = {
    SDL_SCANCODE_X, /* $0 -> X */
    SDL_SCANCODE_1, /* $1 -> 1 */
    SDL_SCANCODE_2, /* $2 -> 2 */
    SDL_SCANCODE_3, /* $3 -> 3 */
    SDL_SCANCODE_Q, /* $4 -> Q */
    SDL_SCANCODE_W, /* $5 -> W */
    SDL_SCANCODE_E, /* $6 -> E */
    SDL_SCANCODE_A, /* $7 -> A */
    SDL_SCANCODE_S, /* $8 -> S */
    SDL_SCANCODE_D, /* $9 -> D */
    SDL_SCANCODE_Z, /* $A -> Z */
    SDL_SCANCODE_C, /* $B -> C */
    SDL_SCANCODE_4, /* $C -> 4 */
    SDL_SCANCODE_R, /* $D -> R */
    SDL_SCANCODE_F, /* $E -> F */
    SDL_SCANCODE_V, /* $F -> V */
};

int
key_down(char key)
{
    const Uint8* state = SDL_GetKeyboardState(NULL);
    return state[key_map[key]];
}

int
wait_key_press(void)
{
    while (1) {
        SDL_Event event;
        if (SDL_PollEvent(&event) && event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_x: return 0x0;
                case SDLK_1: return 0x1;
                case SDLK_2: return 0x2;
                case SDLK_3: return 0x3;
                case SDLK_q: return 0x4;
                case SDLK_w: return 0x5;
                case SDLK_e: return 0x6;
                case SDLK_a: return 0x7;
                case SDLK_s: return 0x8;
                case SDLK_d: return 0x9;
                case SDLK_z: return 0xA;
                case SDLK_c: return 0xB;
                case SDLK_4: return 0xC;
                case SDLK_r: return 0xD;
                case SDLK_f: return 0xE;
                case SDLK_v: return 0xF;
            }
        }
    }
}
