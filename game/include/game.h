#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ─── Window ─────────────────────────────────── */
#define WIN_W   1024
#define WIN_H   768
#define WIN_TITLE "PuzzleQuest"

/* ─── Game states ─────────────────────────────── */
typedef enum {
    STATE_MAIN_MENU = 0,
    STATE_OPTIONS,
    STATE_SAVE_LOAD,
    STATE_SAVE_LOAD_CONFIRM,   /* after clicking OUI */
    STATE_PLAYER_MENU,
    STATE_PLAYER_SELECT,       /* avatar/input selection */
    STATE_SCORES_INPUT,        /* name entry */
    STATE_SCORES_DISPLAY,      /* top-3 with music */
    STATE_ENIGMA,
    STATE_QUIZ,
    STATE_PUZZLE,
    STATE_EXIT
} GameState;

/* ─── Button ──────────────────────────────────── */
#define MAX_BUTTONS 10
typedef struct {
    SDL_Rect  rect;
    char      label[64];
    int       hovered;
    int       visible;
} Button;

/* ─── Score entry ─────────────────────────────── */
#define SCORE_FILE "scores.txt"
#define MAX_SCORES 100
typedef struct {
    char name[64];
    int  score;
} ScoreEntry;

/* ─── Puzzle piece ────────────────────────────── */
#define GRID_SIZE   3          /* 3×3 grid */
#define PIECE_COUNT (GRID_SIZE * GRID_SIZE)
typedef struct {
    SDL_Rect src;              /* source rect in original image */
    SDL_Rect dst;              /* current screen position       */
    SDL_Rect target;           /* correct screen position       */
    int      correct;          /* 1 if snapped in place         */
    int      dragging;
    int      offsetX, offsetY;
    int      tab[4];           /* tenons: top=0 right=1 bot=2 left=3 */
} PuzzlePiece;

/* ─── Quiz question ───────────────────────────── */
typedef struct {
    char question[256];
    char answers[3][128];
    int  correct_idx;          /* 0,1,2 */
} QuizQuestion;

/* ─── Global game context ─────────────────────── */
typedef struct {
    SDL_Window   *window;
    SDL_Renderer *renderer;
    TTF_Font     *font;
    TTF_Font     *fontSmall;

    /* Textures – backgrounds */
    SDL_Texture  *bg[7];       /* bg[1]..bg[6] = menu1..menu6 */

    /* Puzzle images */
    SDL_Texture  *puzzleImg[16]; /* [1]..[15] */

    /* Sounds */
    Mix_Music    *music[5];    /* music1..music4, [0] unused   */
    Mix_Chunk    *sfxClick;
    Mix_Chunk    *sfxHover;

    /* State */
    GameState     state;
    GameState     prevState;   /* for Back button */

    /* Options */
    int           volume;      /* 0..128 */
    int           fullscreen;

    /* Player */
    char          playerName[64];
    int           selectedAvatar; /* 1 or 2 */
    int           multiplayer;
    int           nameLen;

    /* Score */
    int           currentScore;

    /* Puzzle */
    int           currentLevel;   /* 1..15 */
    PuzzlePiece   pieces[PIECE_COUNT];
    int           puzzleComplete;

    /* Quiz */
    int           quizIdx;

    /* Flags */
    int           hoverPlayed;
    int           prevHovered;   /* button index or -1 */
    int           showConfirm;   /* OUI clicked in save/load */
} Game;

/* ─── Prototypes ──────────────────────────────── */
/* init.c */
int  game_init(Game *g);
void game_cleanup(Game *g);

/* assets.c */
int  assets_load(Game *g);
void assets_unload(Game *g);

/* button.c */
void button_init(Button *b, int x, int y, int w, int h, const char *label);
void buttons_draw(Game *g, Button *btns, int count);
int  buttons_handle_hover(Game *g, Button *btns, int count, int mx, int my);
int  buttons_handle_click(Button *btns, int count, int mx, int my);

/* render.c */
void render_text(Game *g, const char *text, int x, int y, SDL_Color col);
void render_text_centered(Game *g, const char *text, int y, SDL_Color col);
void render_text_small(Game *g, const char *text, int x, int y, SDL_Color col);
void render_text_small_centered(Game *g, const char *text, int y, SDL_Color col);
void render_panel(Game *g, SDL_Rect *r, int alpha);
void render_bg(Game *g, int idx);

/* menu_main.c */
void menu_main_enter(Game *g);
void menu_main_update(Game *g);
void menu_main_draw(Game *g);
void menu_main_event(Game *g, SDL_Event *e);

/* menu_options.c */
void menu_options_enter(Game *g);
void menu_options_update(Game *g);
void menu_options_draw(Game *g);
void menu_options_event(Game *g, SDL_Event *e);

/* menu_saveload.c */
void menu_saveload_enter(Game *g);
void menu_saveload_draw(Game *g);
void menu_saveload_event(Game *g, SDL_Event *e);

/* menu_player.c */
void menu_player_enter(Game *g);
void menu_player_draw(Game *g);
void menu_player_event(Game *g, SDL_Event *e);

/* menu_scores.c */
void menu_scores_enter(Game *g);
void menu_scores_draw(Game *g);
void menu_scores_event(Game *g, SDL_Event *e);
void scores_save(const char *name, int score);
void scores_load_top3(ScoreEntry out[3], int *count);

/* menu_enigma.c */
void menu_enigma_enter(Game *g);
void menu_enigma_draw(Game *g);
void menu_enigma_event(Game *g, SDL_Event *e);

/* quiz.c */
void quiz_enter(Game *g);
void quiz_draw(Game *g);
void quiz_event(Game *g, SDL_Event *e);

/* puzzle.c */
void puzzle_enter(Game *g);
void puzzle_draw(Game *g);
void puzzle_event(Game *g, SDL_Event *e);
void puzzle_init_level(Game *g);

/* music.c */
void music_play(Game *g, int idx);   /* 1..4 */
void music_stop(Game *g);

#endif /* GAME_H */
