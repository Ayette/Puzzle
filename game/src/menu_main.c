/* menu_main.c – Main Menu */
#include "../include/game.h"

/* Button indices */
#define BTN_PLAY    0
#define BTN_OPTIONS 1
#define BTN_SCORES  2
#define BTN_STORY   3
#define BTN_QUIT    4
#define BTN_COUNT   5

static Button btns[BTN_COUNT];

void menu_main_enter(Game *g)
{
    int cx = 80;   /* left column x */
    int bw = 220;
    int bh = 55;
    int startY = 240;
    int gap    = 70;

    button_init(&btns[BTN_PLAY],    cx, startY + 0*gap, bw, bh, "Jouer");
    button_init(&btns[BTN_OPTIONS], cx, startY + 1*gap, bw, bh, "Options");
    button_init(&btns[BTN_SCORES],  cx, startY + 2*gap, bw, bh, "Meilleurs Scores");
    button_init(&btns[BTN_STORY],   cx, startY + 3*gap, bw, bh, "Histoire");
    button_init(&btns[BTN_QUIT],    cx, startY + 4*gap, bw, bh, "Quitter");

    music_play(g, 1);
    g->prevHovered = -1;
}

void menu_main_update(Game *g)
{
    (void)g;
}

void menu_main_draw(Game *g)
{
    render_bg(g, 1);

    /* Title / game name */
    SDL_Color white  = {255, 255, 255, 255};
    SDL_Color yellow = {255, 220,  50, 255};

    /* Titre du jeu – sans panel opaque */
    render_text_centered(g, "PuzzleQuest", 110, yellow);
    render_text_small_centered(g, "Le jeu ultime de puzzles & enigmes", 155, white);

    /* Buttons */
    buttons_draw(g, btns, BTN_COUNT);
}

void menu_main_event(Game *g, SDL_Event *e)
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
        case BTN_PLAY:    g->state = STATE_SAVE_LOAD;    break;
        case BTN_OPTIONS: g->state = STATE_OPTIONS;      break;
        case BTN_SCORES:  g->state = STATE_SCORES_INPUT; break;
        case BTN_STORY:   /* story: same as scores for now */ break;
        case BTN_QUIT:    g->state = STATE_EXIT;         break;
        }
    }

    if (e->type == SDL_KEYDOWN) {
        switch (e->key.keysym.sym) {
        case SDLK_j:      g->state = STATE_SAVE_LOAD;    break;
        case SDLK_o:      g->state = STATE_OPTIONS;      break;
        case SDLK_m:      g->state = STATE_SCORES_INPUT; break;
        case SDLK_ESCAPE: g->state = STATE_EXIT;         break;
        }
    }
}
