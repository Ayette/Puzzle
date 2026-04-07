/* init.c – SDL2 init / cleanup */
#include "../include/game.h"

int game_init(Game *g)
{
    memset(g, 0, sizeof(Game));
    g->volume      = MIX_MAX_VOLUME;
    g->currentLevel = 1;
    g->prevHovered  = -1;

    /* SDL core */
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "SDL_Init error: %s\n", SDL_GetError());
        return 0;
    }

    /* Window */
    g->window = SDL_CreateWindow(
        WIN_TITLE,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WIN_W, WIN_H, 0);
    if (!g->window) {
        fprintf(stderr, "SDL_CreateWindow error: %s\n", SDL_GetError());
        return 0;
    }

    /* Renderer */
    g->renderer = SDL_CreateRenderer(
        g->window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!g->renderer) {
        fprintf(stderr, "SDL_CreateRenderer error: %s\n", SDL_GetError());
        return 0;
    }

    /* SDL_image */
    if (!(IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) &
          (IMG_INIT_PNG | IMG_INIT_JPG))) {
        fprintf(stderr, "IMG_Init error: %s\n", IMG_GetError());
        return 0;
    }

    /* SDL_mixer */
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        fprintf(stderr, "Mix_OpenAudio error: %s\n", Mix_GetError());
        return 0;
    }
    Mix_Init(MIX_INIT_MP3);

    /* SDL_ttf */
    if (TTF_Init() < 0) {
        fprintf(stderr, "TTF_Init error: %s\n", TTF_GetError());
        return 0;
    }

    /* Fonts */
    g->font      = TTF_OpenFont("assets/fonts/font.ttf", 28);
    g->fontSmall = TTF_OpenFont("assets/fonts/font.ttf", 20);
    if (!g->font || !g->fontSmall) {
        fprintf(stderr, "TTF_OpenFont error: %s\n", TTF_GetError());
        return 0;
    }

    srand((unsigned int)time(NULL));
    return 1;
}

void game_cleanup(Game *g)
{
    assets_unload(g);

    if (g->fontSmall) TTF_CloseFont(g->fontSmall);
    if (g->font)      TTF_CloseFont(g->font);

    TTF_Quit();
    Mix_CloseAudio();
    Mix_Quit();
    IMG_Quit();

    if (g->renderer) SDL_DestroyRenderer(g->renderer);
    if (g->window)   SDL_DestroyWindow(g->window);
    SDL_Quit();
}
