/* render.c – text / background rendering helpers */
#include "../include/game.h"

/* Render text at absolute position */
void render_text(Game *g, const char *text, int x, int y, SDL_Color col)
{
    if (!g->font || !text || text[0] == '\0') return;
    SDL_Surface *surf = TTF_RenderUTF8_Blended(g->font, text, col);
    if (!surf) return;
    SDL_Texture *tex = SDL_CreateTextureFromSurface(g->renderer, surf);
    if (tex) {
        SDL_Rect dst = {x, y, surf->w, surf->h};
        SDL_RenderCopy(g->renderer, tex, NULL, &dst);
        SDL_DestroyTexture(tex);
    }
    SDL_FreeSurface(surf);
}

/* Render text horizontally centred at given y */
void render_text_centered(Game *g, const char *text, int y, SDL_Color col)
{
    if (!g->font || !text || text[0] == '\0') return;
    SDL_Surface *surf = TTF_RenderUTF8_Blended(g->font, text, col);
    if (!surf) return;
    SDL_Texture *tex = SDL_CreateTextureFromSurface(g->renderer, surf);
    if (tex) {
        SDL_Rect dst = {(WIN_W - surf->w) / 2, y, surf->w, surf->h};
        SDL_RenderCopy(g->renderer, tex, NULL, &dst);
        SDL_DestroyTexture(tex);
    }
    SDL_FreeSurface(surf);
}

/* Render small text (fontSmall) horizontally centred at given y */
void render_text_small_centered(Game *g, const char *text, int y, SDL_Color col)
{
    if (!g->fontSmall || !text || text[0] == '\0') return;
    SDL_Surface *surf = TTF_RenderUTF8_Blended(g->fontSmall, text, col);
    if (!surf) return;
    SDL_Texture *tex = SDL_CreateTextureFromSurface(g->renderer, surf);
    if (tex) {
        SDL_Rect dst = {(WIN_W - surf->w) / 2, y, surf->w, surf->h};
        SDL_RenderCopy(g->renderer, tex, NULL, &dst);
        SDL_DestroyTexture(tex);
    }
    SDL_FreeSurface(surf);
}

/* Render small text at absolute position */
void render_text_small(Game *g, const char *text, int x, int y, SDL_Color col)
{
    if (!g->fontSmall || !text || text[0] == '\0') return;
    SDL_Surface *surf = TTF_RenderUTF8_Blended(g->fontSmall, text, col);
    if (!surf) return;
    SDL_Texture *tex = SDL_CreateTextureFromSurface(g->renderer, surf);
    if (tex) {
        SDL_Rect dst = {x, y, surf->w, surf->h};
        SDL_RenderCopy(g->renderer, tex, NULL, &dst);
        SDL_DestroyTexture(tex);
    }
    SDL_FreeSurface(surf);
}

/* Render background; idx = 1..6 */
void render_bg(Game *g, int idx)
{
    if (idx < 1 || idx > 6 || !g->bg[idx]) {
        /* Fallback: dark gradient */
        SDL_SetRenderDrawColor(g->renderer, 10, 10, 30, 255);
        SDL_RenderClear(g->renderer);
        return;
    }
    SDL_Rect dst = {0, 0, WIN_W, WIN_H};
    SDL_RenderCopy(g->renderer, g->bg[idx], NULL, &dst);
}

/* Draw a semi-transparent dark overlay panel */
void render_panel(Game *g, SDL_Rect *r, int alpha)
{
    SDL_SetRenderDrawBlendMode(g->renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(g->renderer, 0, 0, 0, alpha);
    SDL_RenderFillRect(g->renderer, r);
}
