/*
 * main.c – PuzzleQuest entry point
 * Compilation: see Makefile
 */
#include "include/game.h"
#include "include/render_extra.h"

/* ── State-enter dispatcher ──────────────────────────────────── */
static void enter_state(Game *g, GameState new_state)
{
    g->prevState = g->state;
    g->state     = new_state;

    switch (new_state) {
    case STATE_MAIN_MENU:    menu_main_enter(g);    break;
    case STATE_OPTIONS:      menu_options_enter(g); break;
    case STATE_SAVE_LOAD:
    case STATE_SAVE_LOAD_CONFIRM:
                             menu_saveload_enter(g); break;
    case STATE_PLAYER_MENU:
    case STATE_PLAYER_SELECT:
                             menu_player_enter(g);  break;
    case STATE_SCORES_INPUT:
    case STATE_SCORES_DISPLAY:
                             menu_scores_enter(g);  break;
    case STATE_ENIGMA:       menu_enigma_enter(g);  break;
    case STATE_QUIZ:         quiz_enter(g);         break;
    case STATE_PUZZLE:       puzzle_enter(g);       break;
    default: break;
    }
}

/* ── Draw dispatcher ─────────────────────────────────────────── */
static void draw_state(Game *g)
{
    switch (g->state) {
    case STATE_MAIN_MENU:          menu_main_draw(g);    break;
    case STATE_OPTIONS:            menu_options_draw(g); break;
    case STATE_SAVE_LOAD:
    case STATE_SAVE_LOAD_CONFIRM:  menu_saveload_draw(g); break;
    case STATE_PLAYER_MENU:
    case STATE_PLAYER_SELECT:      menu_player_draw(g);  break;
    case STATE_SCORES_INPUT:
    case STATE_SCORES_DISPLAY:     menu_scores_draw(g);  break;
    case STATE_ENIGMA:             menu_enigma_draw(g);  break;
    case STATE_QUIZ:               quiz_draw(g);         break;
    case STATE_PUZZLE:             puzzle_draw(g);       break;
    default: break;
    }
}

/* ── Event dispatcher ────────────────────────────────────────── */
static void handle_event(Game *g, SDL_Event *e)
{
    switch (g->state) {
    case STATE_MAIN_MENU:          menu_main_event(g, e);    break;
    case STATE_OPTIONS:            menu_options_event(g, e); break;
    case STATE_SAVE_LOAD:
    case STATE_SAVE_LOAD_CONFIRM:  menu_saveload_event(g, e); break;
    case STATE_PLAYER_MENU:
    case STATE_PLAYER_SELECT:      menu_player_event(g, e);  break;
    case STATE_SCORES_INPUT:
    case STATE_SCORES_DISPLAY:     menu_scores_event(g, e);  break;
    case STATE_ENIGMA:             menu_enigma_event(g, e);  break;
    case STATE_QUIZ:               quiz_event(g, e);         break;
    case STATE_PUZZLE:             puzzle_event(g, e);       break;
    default: break;
    }
}

/* ── Main ────────────────────────────────────────────────────── */
int main(int argc, char *argv[])
{
    (void)argc; (void)argv;

    Game g;
    if (!game_init(&g)) {
        fprintf(stderr, "Initialization failed.\n");
        return 1;
    }

    if (!assets_load(&g)) {
        fprintf(stderr, "Warning: some assets could not be loaded.\n");
        /* Continue anyway – missing textures show as black rects */
    }

    /* Enable text input for name entry */
    SDL_StartTextInput();

    /* Enter initial state */
    enter_state(&g, STATE_MAIN_MENU);

    /* ── Game loop ── */
    while (g.state != STATE_EXIT) {
        /* Remember state before events (to detect transitions) */
        GameState stateBeforeEvents = g.state;

        /* Event handling */
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                g.state = STATE_EXIT;
                break;
            }
            handle_event(&g, &e);
        }

        /* Detect state transition → call enter function */
        if (g.state != stateBeforeEvents && g.state != STATE_EXIT)
            enter_state(&g, g.state);

        /* Render */
        SDL_SetRenderDrawColor(g.renderer, 0, 0, 0, 255);
        SDL_RenderClear(g.renderer);

        draw_state(&g);

        SDL_RenderPresent(g.renderer);
        SDL_Delay(16);  /* ~60 fps */
    }

    SDL_StopTextInput();
    game_cleanup(&g);
    return 0;
}
