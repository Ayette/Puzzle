/* menu_scores.c – Scores sub-menu */
#include "../include/game.h"

#define BTN_VALID  0
#define BTN_BACK   1
#define BTN_QUIT   2
#define BTN_COUNT  3

static Button btns[BTN_COUNT];
static int    showing;   /* 0 = input, 1 = display top3 */

/* ── File I/O ──────────────────────────────────────────── */
void scores_save(const char *name, int score)
{
    FILE *f = fopen(SCORE_FILE, "a");
    if (!f) return;
    fprintf(f, "%s %d\n", name, score);
    fclose(f);
}

void scores_load_top3(ScoreEntry out[3], int *count)
{
    ScoreEntry all[MAX_SCORES];
    int        total = 0;

    FILE *f = fopen(SCORE_FILE, "r");
    if (f) {
        while (total < MAX_SCORES &&
               fscanf(f, "%63s %d", all[total].name, &all[total].score) == 2)
            total++;
        fclose(f);
    }

    /* Bubble sort descending */
    for (int i = 0; i < total - 1; i++)
        for (int j = i + 1; j < total; j++)
            if (all[j].score > all[i].score) {
                ScoreEntry tmp = all[i];
                all[i] = all[j];
                all[j] = tmp;
            }

    *count = SDL_min(total, 3);
    for (int i = 0; i < *count; i++) out[i] = all[i];
}

/* ── State ─────────────────────────────────────────────── */
void menu_scores_enter(Game *g)
{
    showing     = 0;
    g->nameLen  = (int)strlen(g->playerName);

    int cx = WIN_W / 2;
    button_init(&btns[BTN_VALID], cx - 80, 460, 160, 55, "Valider");
    button_init(&btns[BTN_BACK],  30, WIN_H - 80, 160, 50, "Retour");
    button_init(&btns[BTN_QUIT],  WIN_W - 200, WIN_H - 80, 160, 50, "Quitter");

    btns[BTN_QUIT].visible = 0;

    music_stop(g);   /* no music at input phase */
    g->prevHovered = -1;
}

void menu_scores_draw(Game *g)
{
    render_bg(g, 5);

    SDL_Color white  = {255, 255, 255, 255};
    SDL_Color yellow = {255, 220,  50, 255};
    SDL_Color cyan   = {100, 220, 220, 255};

    if (!showing) {
        /* ── Name input ── */
        render_text_centered(g, "Meilleurs Scores", 80, yellow);
        render_text_centered(g, "Saisir Nom / Pseudo :", 220, white);

        SDL_Rect inputBox = {WIN_W/2 - 180, 280, 360, 55};
        SDL_SetRenderDrawColor(g->renderer, 20, 20, 50, 200);
        SDL_RenderFillRect(g->renderer, &inputBox);
        SDL_SetRenderDrawColor(g->renderer, 100, 160, 255, 255);
        SDL_RenderDrawRect(g->renderer, &inputBox);

        /* text + cursor */
        char display[130];
        snprintf(display, sizeof(display), "%s_", g->playerName);
        render_text(g, display, inputBox.x + 12, inputBox.y + 12, white);

        btns[BTN_VALID].visible = 1;
        btns[BTN_BACK].visible  = 1;
        btns[BTN_QUIT].visible  = 0;
    } else {
        /* ── Top 3 display ── */
        render_text_centered(g, "Meilleurs Scores", 80, yellow);

        ScoreEntry top[3];
        int        cnt = 0;
        scores_load_top3(top, &cnt);

        const char *medals[] = {"★  1er: ", "★  2eme: ", "★  3eme: "};
        for (int i = 0; i < cnt; i++) {
            char line[128];
            snprintf(line, sizeof(line), "%s%s — %d pts",
                     medals[i], top[i].name, top[i].score);
            int y = 220 + i * 80;
            render_text_centered(g, line, y, (i == 0) ? yellow : cyan);
        }
        if (cnt == 0)
            render_text_centered(g, "Aucun score enregistre.", 280, white);

        render_text_centered(g, "(Appuyez 'e' pour le menu Enigme)", 530, white);

        btns[BTN_VALID].visible = 0;
        btns[BTN_BACK].visible  = 1;
        btns[BTN_QUIT].visible  = 1;
    }

    buttons_draw(g, btns, BTN_COUNT);
}

void menu_scores_event(Game *g, SDL_Event *e)
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
        case BTN_VALID:
            if (!showing && g->nameLen > 0) {
                scores_save(g->playerName, g->currentScore);
                showing = 1;
                music_play(g, 3);
            }
            break;
        case BTN_BACK:
            g->state = STATE_MAIN_MENU;
            break;
        case BTN_QUIT:
            g->state = STATE_EXIT;
            break;
        }
    }

    if (e->type == SDL_KEYDOWN) {
        if (!showing) {
            SDL_Keycode sym = e->key.keysym.sym;
            if (sym == SDLK_RETURN || sym == SDLK_KP_ENTER) {
                if (g->nameLen > 0) {
                    scores_save(g->playerName, g->currentScore);
                    showing = 1;
                    music_play(g, 3);
                }
            } else if (sym == SDLK_BACKSPACE) {
                if (g->nameLen > 0) {
                    g->nameLen--;
                    g->playerName[g->nameLen] = '\0';
                }
            } else if (sym == SDLK_ESCAPE) {
                g->state = STATE_MAIN_MENU;
            }
        } else {
            /* showing top3 */
            switch (e->key.keysym.sym) {
            case SDLK_e:      g->state = STATE_ENIGMA;    break;
            case SDLK_ESCAPE: g->state = STATE_EXIT;      break;
            default: break;
            }
        }
    }

    if (e->type == SDL_TEXTINPUT && !showing) {
        /* Only accept printable chars */
        if (g->nameLen < 63) {
            int len = (int)strlen(e->text.text);
            if (g->nameLen + len < 63) {
                strcat(g->playerName, e->text.text);
                g->nameLen += len;
            }
        }
    }
}
