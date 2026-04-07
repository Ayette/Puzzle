/* menu_saveload.c – Save / Load sub-menu */
#include "../include/game.h"

/* Phase 1: oui/non */
#define BTN_OUI   0
#define BTN_NON   1
/* Phase 2: after OUI */
#define BTN_LOAD  2
#define BTN_NEW   3
#define BTN_BACK  4
#define BTN_COUNT 5

static Button btns[BTN_COUNT];
static int    phase = 0;   /* 0 = oui/non, 1 = load/new */

void menu_saveload_enter(Game *g)
{
    phase = 0;
    g->showConfirm = 0;

    int cx = WIN_W / 2;

    button_init(&btns[BTN_OUI],  cx - 130, 420, 110, 55, "OUI");
    button_init(&btns[BTN_NON],  cx +  20, 420, 110, 55, "NON");
    button_init(&btns[BTN_LOAD], cx - 130, 350, 260, 55, "Charger le jeu");
    button_init(&btns[BTN_NEW],  cx - 130, 430, 260, 55, "Nouvelle partie");
    button_init(&btns[BTN_BACK], 30, WIN_H - 80, 160, 50, "Retour");

    /* Phase 2 buttons hidden initially */
    btns[BTN_LOAD].visible = 0;
    btns[BTN_NEW].visible  = 0;

    music_play(g, 2);
    g->prevHovered = -1;
}

void menu_saveload_draw(Game *g)
{
    render_bg(g, 3);

    SDL_Color white  = {255, 255, 255, 255};
    SDL_Color yellow = {255, 220,  50, 255};

    if (phase == 0) {
        render_text_centered(g, "Sauvegarde", 80, yellow);
        SDL_Rect panel = {WIN_W/2 - 280, 200, 560, 280};
        render_panel(g, &panel, 160);
        render_text_centered(g, "Voulez-vous sauvegarder votre jeu ?", 260, white);
        btns[BTN_OUI].visible = 1;
        btns[BTN_NON].visible = 1;
        btns[BTN_LOAD].visible = 0;
        btns[BTN_NEW].visible  = 0;
    } else {
        render_text_centered(g, "Chargement", 80, yellow);
        SDL_Rect panel = {WIN_W/2 - 180, 300, 360, 220};
        render_panel(g, &panel, 160);
        btns[BTN_OUI].visible  = 0;
        btns[BTN_NON].visible  = 0;
        btns[BTN_LOAD].visible = 1;
        btns[BTN_NEW].visible  = 1;
    }

    buttons_draw(g, btns, BTN_COUNT);
}

void menu_saveload_event(Game *g, SDL_Event *e)
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
        case BTN_OUI:  phase = 1;                     break;
        case BTN_NON:  g->state = STATE_MAIN_MENU;    break;
        case BTN_LOAD: g->state = STATE_ENIGMA;       break; /* load → go to enigma */
        case BTN_NEW:  g->state = STATE_PLAYER_MENU;  break;
        case BTN_BACK: g->state = STATE_MAIN_MENU;    break;
        }
    }

    if (e->type == SDL_KEYDOWN) {
        switch (e->key.keysym.sym) {
        case SDLK_n:      g->state = STATE_PLAYER_MENU; break;
        case SDLK_ESCAPE: g->state = STATE_MAIN_MENU;   break;
        }
    }
}
