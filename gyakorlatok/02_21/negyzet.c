#include <SDL2/SDL.h>

int main (int argc, char** argv)
{
    SDL_Window* w = NULL;
    w = SDL_CreateWindow
    (
        "Negyzet rajzolasa", SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        1280,
        720,
        SDL_WINDOW_SHOWN
    );

    SDL_Renderer* re = NULL;
    re =  SDL_CreateRenderer(w, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawColor(re, 0, 0, 0, 0);
    SDL_RenderClear(re);

    SDL_Rect rect;
    rect.x = 270;
    rect.y = 170;
    rect.w = 100;
    rect.h = 100;

    SDL_SetRenderDrawColor(re, 255, 255, 255, 255 );
    SDL_RenderFillRect(re, &rect);
    SDL_RenderPresent(re);
    SDL_Delay(5000);
    SDL_DestroyWindow(w);
    SDL_Quit();

    return 0;
}
