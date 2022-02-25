#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>

int main(int argc, char* argv[])
{
    int error_code, i;
    SDL_Window* w;
    bool  need_run = true;
    SDL_Event event;
    SDL_Renderer* r;

    error_code = SDL_Init(SDL_INIT_EVERYTHING);
    if (error_code != 0) {
        printf("SDL inicializacios hiba: %s\n", SDL_GetError());
        return error_code;
    }

    w = SDL_CreateWindow(
        "vonal",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        720, 720, 0
    );

    r = SDL_CreateRenderer(w, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawColor(r, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(r);

    SDL_SetRenderDrawColor(r, 0, 255, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawLine(r, 200, 0, 200, 200);
    SDL_RenderPresent(r);

    while (need_run) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_KEYDOWN:
                case SDL_SCANCODE_Q:
                    need_run = false;
                    break;
                case SDL_QUIT:
                    need_run = false;
                    break;
            }
        }
    }

    SDL_DestroyRenderer(r);
    SDL_DestroyWindow(w);
    SDL_Quit();

    return 0;
}
