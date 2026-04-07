/* menu_enigma.c – Enigma sub-menu */
#include "../include/game.h"

#define BTN_QUIZ   0
#define BTN_PUZZLE 1
#define BTN_BACK   2
#define BTN_COUNT  3

static Button btns[BTN_COUNT];

void menu_enigma_enter(Game *g)
{
    int cx = WIN_W / 2;
    button_init(&btns[BTN_QUIZ],   cx - 160, 380, 140, 60, "Quiz");
    button_init(&btns[BTN_PUZZLE], cx +  20, 380, 140, 60, "Puzzle");
    button_init(&btns[BTN_BACK],   30, WIN_H - 80, 160, 50, "Retour");

    music_stop(g);   /* no music in enigma menu per spec */
    g->prevHovered = -1;
}

void menu_enigma_draw(Game *g)
{
    render_bg(g, 6);

    SDL_Color yellow = {255, 220,  50, 255};
    SDL_Color white  = {255, 255, 255, 255};

    render_text_centered(g, "Enigme", 80, yellow);
    render_text_centered(g, "Choisissez votre mode de jeu", 200, white);

    buttons_draw(g, btns, BTN_COUNT);
}

void menu_enigma_event(Game *g, SDL_Event *e)
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
        case BTN_QUIZ:   g->state = STATE_QUIZ;   break;
        case BTN_PUZZLE: g->state = STATE_PUZZLE; break;
        case BTN_BACK:   g->state = STATE_MAIN_MENU; break;
        }
    }

    if (e->type == SDL_KEYDOWN && e->key.keysym.sym == SDLK_ESCAPE)
        g->state = STATE_EXIT;
}
