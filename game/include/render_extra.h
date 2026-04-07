#ifndef RENDER_EXTRA_H
#define RENDER_EXTRA_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "../include/game.h"

/* Declared here, defined in button.c */
void render_text_centered_in_rect(Game *g, const char *text,
                                  SDL_Rect *rect, SDL_Color col);

/* Declared here, defined in render.c */
void render_text_small(Game *g, const char *text, int x, int y, SDL_Color col);
void render_text_small_centered(Game *g, const char *text, int y, SDL_Color col);
void render_panel(Game *g, SDL_Rect *r, int alpha);

#endif
