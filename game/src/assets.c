/* assets.c – load / unload all textures and sounds */
#include "../include/game.h"

/* Helper: load texture, print error and return NULL on failure */
static SDL_Texture *load_tex(SDL_Renderer *r, const char *path)
{
    SDL_Texture *t = IMG_LoadTexture(r, path);
    if (!t)
        fprintf(stderr, "Failed to load texture '%s': %s\n",
                path, IMG_GetError());
    return t;
}

int assets_load(Game *g)
{
    char path[256];
    int  ok = 1;

    /* ── Menu backgrounds bg[1]..bg[6] ── */
    for (int i = 1; i <= 6; i++) {
        snprintf(path, sizeof(path), "assets/images/menu%d.png", i);
        g->bg[i] = load_tex(g->renderer, path);
        if (!g->bg[i]) ok = 0;
    }

    /* ── Puzzle images puzzleImg[1]..puzzleImg[15] ── */
    for (int i = 1; i <= 15; i++) {
        snprintf(path, sizeof(path), "assets/images/img-%d.png", i);
        g->puzzleImg[i] = load_tex(g->renderer, path);
        if (!g->puzzleImg[i]) ok = 0;
    }

    /* ── Music music[1]..music[4] ── */
    const char *musicFiles[] = {
        NULL,                       /* [0] unused */
        "assets/sounds/music1.mp3", /* main menu  */
        "assets/sounds/music2.mp3", /* options    */
        "assets/sounds/music3.mp3", /* victory    */
        "assets/sounds/music4.mp3"  /* quiz       */
    };
    for (int i = 1; i <= 4; i++) {
        g->music[i] = Mix_LoadMUS(musicFiles[i]);
        if (!g->music[i]) {
            fprintf(stderr, "Failed to load music '%s': %s\n",
                    musicFiles[i], Mix_GetError());
            ok = 0;
        }
    }

    /* ── SFX ── */
    g->sfxClick = Mix_LoadWAV("assets/sounds/click.wav");
    if (!g->sfxClick) {
        fprintf(stderr, "Failed to load click.wav: %s\n", Mix_GetError());
        ok = 0;
    }

    g->sfxHover = Mix_LoadWAV("assets/sounds/hover.wav");
    if (!g->sfxHover) {
        fprintf(stderr, "Failed to load hover.wav: %s\n", Mix_GetError());
        ok = 0;
    }

    return ok;
}

void assets_unload(Game *g)
{
    for (int i = 1; i <= 6;  i++) if (g->bg[i])         SDL_DestroyTexture(g->bg[i]);
    for (int i = 1; i <= 15; i++) if (g->puzzleImg[i])   SDL_DestroyTexture(g->puzzleImg[i]);
    for (int i = 1; i <= 4;  i++) if (g->music[i])       Mix_FreeMusic(g->music[i]);
    if (g->sfxClick) Mix_FreeChunk(g->sfxClick);
    if (g->sfxHover) Mix_FreeChunk(g->sfxHover);
}
