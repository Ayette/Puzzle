/* menu_options.c – Options sub-menu */
#include "../include/game.h"

#define BTN_VOL_UP   0
#define BTN_VOL_DN   1
#define BTN_FULLSCR  2
#define BTN_NORMAL   3
#define BTN_BACK     4
#define BTN_COUNT    5

static Button btns[BTN_COUNT];

void menu_options_enter(Game *g)
{
    int cx = WIN_W / 2;

    button_init(&btns[BTN_VOL_UP],  cx - 210, 180, 190, 50, "Augmenter (+)");
    button_init(&btns[BTN_VOL_DN],  cx +  20, 180, 190, 50, "Diminuer (-)");
    button_init(&btns[BTN_NORMAL],  cx - 210, 380, 190, 50, "Mode normal");
    button_init(&btns[BTN_FULLSCR], cx +  20, 380, 190, 50, "Plein ecran");
    button_init(&btns[BTN_BACK],    WIN_W - 200, WIN_H - 80, 160, 50, "Retour");

    music_play(g, 2);
    g->prevHovered = -1;
}

void menu_options_update(Game *g) { (void)g; }

void menu_options_draw(Game *g)
{
    render_bg(g, 2);

    SDL_Color white  = {255, 255, 255, 255};
    SDL_Color yellow = {255, 220,  50, 255};
    SDL_Color grey   = {200, 200, 200, 255};

    render_text_centered(g, "Options", 40, yellow);

    /* ── Volume section (en haut, bien visible) ── */
    render_text(g, "Volume :", 80, 110, grey);

    /* Barre fond */
    SDL_Rect barBg = {240, 108, 400, 32};
    SDL_SetRenderDrawColor(g->renderer, 20, 20, 40, 210);
    SDL_RenderFillRect(g->renderer, &barBg);

    /* Barre remplie */
    int fillW = (int)(400.0 * g->volume / MIX_MAX_VOLUME);
    SDL_Rect fill = {240, 108, fillW, 32};
    SDL_SetRenderDrawColor(g->renderer, 60, 200, 80, 255);
    SDL_RenderFillRect(g->renderer, &fill);

    /* Bordure barre */
    SDL_SetRenderDrawColor(g->renderer, 100, 200, 120, 255);
    SDL_RenderDrawRect(g->renderer, &barBg);

    /* Pourcentage */
    char volStr[16];
    snprintf(volStr, sizeof(volStr), "%d%%",
             (int)(100.0 * g->volume / MIX_MAX_VOLUME));
    render_text(g, volStr, 655, 110, white);

    /* ── Mode affichage ── */
    render_text(g, "Mode d'affichage :", 80, 340, grey);

    buttons_draw(g, btns, BTN_COUNT);
}

void menu_options_event(Game *g, SDL_Event *e)
{
    if (e->type == SDL_MOUSEMOTION)
        buttons_handle_hover(g, btns, BTN_COUNT,
                             e->motion.x, e->motion.y);

    if (e->type == SDL_MOUSEBUTTONDOWN && e->button.button == SDL_BUTTON_LEFT) {
        int idx = buttons_handle_click(btns, BTN_COUNT,
                                       e->button.x, e->button.y);
        if (idx >= 0 && g->sfxClick)
            Mix_PlayChannel(-1, g->sfxClick, 0);

        switch (idx) {
        case BTN_VOL_UP:
            g->volume = SDL_min(g->volume + 16, MIX_MAX_VOLUME);
            Mix_VolumeMusic(g->volume);
            break;
        case BTN_VOL_DN:
            g->volume = SDL_max(g->volume - 16, 0);
            Mix_VolumeMusic(g->volume);
            break;
        case BTN_FULLSCR:
            g->fullscreen = 1;
            SDL_SetWindowFullscreen(g->window, SDL_WINDOW_FULLSCREEN_DESKTOP);
            break;
        case BTN_NORMAL:
            g->fullscreen = 0;
            SDL_SetWindowFullscreen(g->window, 0);
            break;
        case BTN_BACK:
            g->state = STATE_MAIN_MENU;
            break;
        }
    }

    if (e->type == SDL_KEYDOWN) {
        switch (e->key.keysym.sym) {
        case SDLK_PLUS:
        case SDLK_KP_PLUS:
            g->volume = SDL_min(g->volume + 16, MIX_MAX_VOLUME);
            Mix_VolumeMusic(g->volume);
            break;
        case SDLK_MINUS:
        case SDLK_KP_MINUS:
            g->volume = SDL_max(g->volume - 16, 0);
            Mix_VolumeMusic(g->volume);
            break;
        case SDLK_ESCAPE:
            g->state = STATE_MAIN_MENU;
            break;
        }
    }
}
