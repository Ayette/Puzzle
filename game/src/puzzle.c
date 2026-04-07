/* puzzle.c – Puzzle classique corrigé
 *
 * FIX COHÉRENCE : les tenons (tab[4]) sont stockés DANS chaque PuzzlePiece.
 * Ainsi, quand une pièce est déplacée, ses tenons la suivent.
 *
 * Layout :
 *   GAUCHE  : grille vide (zone de placement)
 *   DROITE  : plateau des pièces mélangées
 *   BOUTON SUIVANT : coin haut-gauche, visible après complétion
 */
#include "../include/game.h"
#include <math.h>

/* ── Dimensions ──────────────────────────────────────────────── */
#define GRID_X     20
#define GRID_Y    100           /* grille plus basse = alignée avec plateau */
#define GRID_W    480
#define GRID_H    480
#define PIECE_W   (GRID_W / GRID_SIZE)   /* 160 */
#define PIECE_H   (GRID_H / GRID_SIZE)   /* 160 */

/* Plateau droit : slots exactement PIECE_W × PIECE_H avec petit gap */
#define TRAY_X    540
#define TRAY_Y    100           /* même niveau que la grille */
#define TRAY_COLS  GRID_SIZE
#define TRAY_GAP   6            /* espace entre slots */
#define TRAY_SW   (PIECE_W + TRAY_GAP)   /* 166 → slot width  */
#define TRAY_SH   (PIECE_H + TRAY_GAP)   /* 166 → slot height */

#define SNAP_DIST  (PIECE_W * 2 / 3)
#define TAB_R       14

/* ── État local ──────────────────────────────────────────────── */
static int    draggingIdx = -1;
static Button btnReset;
static Button btnMenu;
static Button btnNext;

static SDL_Rect traySlots[PIECE_COUNT];

/* ── Construire les slots du plateau ────────────────────────── */
static void build_tray_slots(void)
{
    for (int i = 0; i < PIECE_COUNT; i++) {
        int r = i / TRAY_COLS;
        int c = i % TRAY_COLS;
        /* Chaque slot = exactement PIECE_W × PIECE_H,
           séparé de TRAY_GAP pixels des voisins              */
        traySlots[i] = (SDL_Rect){
            TRAY_X + c * TRAY_SW + TRAY_GAP/2,
            TRAY_Y + r * TRAY_SH + TRAY_GAP/2,
            PIECE_W,   /* taille exacte — aucun débordement */
            PIECE_H
        };
    }
}

/* ── Générer les tenons et les stocker dans chaque pièce ─────── */
/* CLEF DU FIX : on stocke tab[] dans g->pieces[idx].tab         */
static void generate_piece_tabs(Game *g, int level)
{
    /* Tableau temporaire pour calculer les dépendances entre voisins */
    int tmp[PIECE_COUNT][4];

    srand((unsigned)(level * 7919));
    for (int row = 0; row < GRID_SIZE; row++) {
        for (int col = 0; col < GRID_SIZE; col++) {
            int idx = row * GRID_SIZE + col;

            /* TOP : opposé du bas du voisin du dessus */
            tmp[idx][0] = (row == 0) ? 0 : -tmp[(row-1)*GRID_SIZE+col][2];

            /* LEFT : opposé du droit du voisin de gauche */
            tmp[idx][3] = (col == 0) ? 0 : -tmp[row*GRID_SIZE+(col-1)][1];

            /* RIGHT : bord externe = 0, sinon aléatoire */
            tmp[idx][1] = (col == GRID_SIZE-1) ? 0 : (rand()%2 ? 1 : -1);

            /* BOTTOM : bord externe = 0, sinon aléatoire */
            tmp[idx][2] = (row == GRID_SIZE-1) ? 0 : (rand()%2 ? 1 : -1);

            /* Copier dans la pièce */
            for (int s = 0; s < 4; s++)
                g->pieces[idx].tab[s] = tmp[idx][s];
        }
    }
    srand((unsigned)time(NULL));
}

/* ── Dessiner une pièce (tenons attachés à la pièce) ────────── */
static void draw_piece(SDL_Renderer *ren,
                       SDL_Texture  *img,
                       PuzzlePiece  *p,
                       int           dragging)
{
    SDL_Rect *src = &p->src;
    SDL_Rect  dst = {p->dst.x, p->dst.y, PIECE_W, PIECE_H};
    int x = dst.x, y = dst.y, w = dst.w, h = dst.h;
    int mx = x + w/2, my = y + h/2;

    /* Corps */
    SDL_RenderCopy(ren, img, src, &dst);

    /* Tenons : projeter la portion de l'image hors du rectangle */
    /* TOP */
    if (p->tab[0] == 1) {
        SDL_Rect s = {src->x + src->w/2 - TAB_R, src->y,          TAB_R*2, TAB_R};
        SDL_Rect d = {mx - TAB_R,                 y - TAB_R,       TAB_R*2, TAB_R};
        SDL_RenderCopy(ren, img, &s, &d);
    }
    /* RIGHT */
    if (p->tab[1] == 1) {
        SDL_Rect s = {src->x + src->w - TAB_R, src->y + src->h/2 - TAB_R, TAB_R, TAB_R*2};
        SDL_Rect d = {x + w,                   my - TAB_R,                  TAB_R, TAB_R*2};
        SDL_RenderCopy(ren, img, &s, &d);
    }
    /* BOTTOM */
    if (p->tab[2] == 1) {
        SDL_Rect s = {src->x + src->w/2 - TAB_R, src->y + src->h - TAB_R, TAB_R*2, TAB_R};
        SDL_Rect d = {mx - TAB_R,                 y + h,                    TAB_R*2, TAB_R};
        SDL_RenderCopy(ren, img, &s, &d);
    }
    /* LEFT */
    if (p->tab[3] == 1) {
        SDL_Rect s = {src->x,    src->y + src->h/2 - TAB_R, TAB_R, TAB_R*2};
        SDL_Rect d = {x - TAB_R, my - TAB_R,                 TAB_R, TAB_R*2};
        SDL_RenderCopy(ren, img, &s, &d);
    }

    /* Bordure */
    if (p->correct)    SDL_SetRenderDrawColor(ren,  50,220, 50,255);
    else if (dragging) SDL_SetRenderDrawColor(ren, 255,200,  0,255);
    else               SDL_SetRenderDrawColor(ren, 170,170,210,180);
    SDL_RenderDrawRect(ren, &dst);
}

/* ── puzzle_init_level ───────────────────────────────────────── */
void puzzle_init_level(Game *g)
{
    g->puzzleComplete = 0;
    draggingIdx       = -1;

    int imgW, imgH;
    SDL_QueryTexture(g->puzzleImg[g->currentLevel], NULL, NULL, &imgW, &imgH);

    /* Générer les tenons et les stocker dans chaque pièce */
    generate_piece_tabs(g, g->currentLevel);
    build_tray_slots();

    /* Ordre mélangé pour distribuer les pièces dans les slots */
    int order[PIECE_COUNT];
    for (int i = 0; i < PIECE_COUNT; i++) order[i] = i;
    for (int i = PIECE_COUNT-1; i > 0; i--) {
        int j = rand() % (i+1);
        int t = order[i]; order[i] = order[j]; order[j] = t;
    }

    for (int row = 0; row < GRID_SIZE; row++) {
        for (int col = 0; col < GRID_SIZE; col++) {
            int idx = row * GRID_SIZE + col;

            /* Source image (fraction de l'image originale) */
            g->pieces[idx].src = (SDL_Rect){
                col * imgW / GRID_SIZE,
                row * imgH / GRID_SIZE,
                imgW / GRID_SIZE,
                imgH / GRID_SIZE
            };
            /* Target = position correcte dans la grille */
            g->pieces[idx].target = (SDL_Rect){
                GRID_X + col * PIECE_W,
                GRID_Y + row * PIECE_H,
                PIECE_W, PIECE_H
            };
            /* Position initiale = slot du plateau (mélangé)
               Les slots font exactement PIECE_W × PIECE_H → pas de correction */
            g->pieces[idx].dst      = traySlots[order[idx]];
            /* dst.w et dst.h sont déjà PIECE_W / PIECE_H dans traySlots */
            g->pieces[idx].correct  = 0;
            g->pieces[idx].dragging = 0;
            /* Note: tab[] déjà rempli par generate_piece_tabs */
        }
    }
}

void puzzle_enter(Game *g)
{
    if (g->currentLevel < 1 || g->currentLevel > 4)
        g->currentLevel = 1;
    puzzle_init_level(g);
    music_stop(g);
    g->prevHovered = -1;

    /* Boutons sous le plateau droit */
    int trayRight = TRAY_X + TRAY_COLS * TRAY_SW + TRAY_GAP/2;
    int trayBot   = TRAY_Y + GRID_SIZE * TRAY_SH + TRAY_GAP/2;
    int bx        = TRAY_X + (TRAY_COLS * TRAY_SW) / 2 - 70;
    button_init(&btnReset, bx,      trayBot + 10, 140, 45, "Reset");
    button_init(&btnMenu,  bx, trayBot + 65, 140, 45, "Menu");

    /* Bouton Suivant : haut-gauche, caché */
    button_init(&btnNext, GRID_X, 20, 155, 38, ">> Suivant");
    btnNext.visible = 0;

    (void)trayRight;
}

/* ── check_complete ──────────────────────────────────────────── */
static int check_complete(Game *g)
{
    for (int i = 0; i < PIECE_COUNT; i++)
        if (!g->pieces[i].correct) return 0;
    return 1;
}

/* ── puzzle_draw ─────────────────────────────────────────────── */
void puzzle_draw(Game *g)
{
    SDL_SetRenderDrawColor(g->renderer, 15, 15, 35, 255);
    SDL_RenderClear(g->renderer);

    SDL_Color white  = {255,255,255,255};
    SDL_Color yellow = {255,220, 50,255};
    SDL_Color green  = { 50,220, 50,255};
    SDL_Color grey   = {150,150,190,255};

    /* Titre décalé à droite pour laisser place au bouton Suivant */
    char title[64];
    snprintf(title, sizeof(title), "Puzzle — Niveau %d / 4", g->currentLevel);
    render_text(g, title, WIN_W/2 - 130, 25, yellow);

    char sc[32];
    snprintf(sc, sizeof(sc), "Score: %d", g->currentScore);
    render_text(g, sc, WIN_W - 170, 25, white);

    /* ── Grille gauche : cellules vides ── */
    render_text_small(g, "Zone de placement", GRID_X, GRID_Y - 22, grey);
    for (int row = 0; row < GRID_SIZE; row++) {
        for (int col = 0; col < GRID_SIZE; col++) {
            SDL_Rect cell = {
                GRID_X + col * PIECE_W,
                GRID_Y + row * PIECE_H,
                PIECE_W, PIECE_H
            };
            SDL_SetRenderDrawBlendMode(g->renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(g->renderer, 25, 25, 60, 220);
            SDL_RenderFillRect(g->renderer, &cell);
            SDL_SetRenderDrawColor(g->renderer, 85, 85, 155, 255);
            SDL_RenderDrawRect(g->renderer, &cell);
            SDL_Rect inner = {cell.x+3, cell.y+3, cell.w-6, cell.h-6};
            SDL_SetRenderDrawColor(g->renderer, 45, 45, 90, 130);
            SDL_RenderDrawRect(g->renderer, &inner);
        }
    }

    /* ── Plateau droit ── */
    int trayBgW = TRAY_COLS * TRAY_SW + TRAY_GAP;
    int trayBgH = GRID_SIZE  * TRAY_SH + TRAY_GAP + 10;
    SDL_Rect trayBg = {TRAY_X - 4, TRAY_Y - 28, trayBgW + 8, trayBgH + 28};
    SDL_SetRenderDrawBlendMode(g->renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(g->renderer, 20, 20, 50, 210);
    SDL_RenderFillRect(g->renderer, &trayBg);
    SDL_SetRenderDrawColor(g->renderer, 90, 90, 170, 255);
    SDL_RenderDrawRect(g->renderer, &trayBg);
    render_text_small(g, "Pieces",
                      TRAY_X + trayBgW/2 - 25, TRAY_Y - 24, grey);

    /* Slots vides (PIECE_W × PIECE_H exactement) */
    for (int i = 0; i < PIECE_COUNT; i++) {
        SDL_SetRenderDrawColor(g->renderer, 30, 30, 65, 180);
        SDL_RenderFillRect(g->renderer, &traySlots[i]);
        SDL_SetRenderDrawColor(g->renderer, 55, 55, 105, 200);
        SDL_RenderDrawRect(g->renderer, &traySlots[i]);
    }

    /* ── Pièces non-draguées ── */
    SDL_Texture *img = g->puzzleImg[g->currentLevel];
    for (int i = 0; i < PIECE_COUNT; i++) {
        if (g->pieces[i].dragging) continue;
        draw_piece(g->renderer, img, &g->pieces[i], 0);
    }

    /* ── Pièce draguée (par-dessus tout) ── */
    if (draggingIdx >= 0)
        draw_piece(g->renderer, img, &g->pieces[draggingIdx], 1);

    /* ── Boutons ── */
    Button allBtns[3] = {btnReset, btnMenu, btnNext};
    buttons_draw(g, allBtns, 3);

    /* Instructions */
    render_text_small(g, "Glissez les pieces dans la grille  |  R = Reset",
                      GRID_X, GRID_Y + GRID_H + 12, white);

    /* ── Bannière complétion ── */
    if (g->puzzleComplete) {
        btnNext.visible = 1;
        SDL_Rect banner = {GRID_X, WIN_H/2 - 50, GRID_W, 100};
        render_panel(g, &banner, 220);
        SDL_SetRenderDrawColor(g->renderer, 50,220,50,255);
        SDL_RenderDrawRect(g->renderer, &banner);
        render_text_centered(g, "Puzzle Complete ! +20 pts", WIN_H/2 - 24, green);
        render_text_small_centered(g, "Cliquez '>> Suivant' (haut gauche)", WIN_H/2 + 20, white);
    }
}

/* ── puzzle_event ────────────────────────────────────────────── */
void puzzle_event(Game *g, SDL_Event *e)
{
    if (e->type == SDL_KEYDOWN) {
        if (e->key.keysym.sym == SDLK_ESCAPE) { g->state = STATE_MAIN_MENU; return; }
        if (e->key.keysym.sym == SDLK_r)      { puzzle_init_level(g); btnNext.visible = 0; return; }
    }

    if (e->type == SDL_MOUSEMOTION) {
        SDL_Point pt = {e->motion.x, e->motion.y};
        btnReset.hovered = SDL_PointInRect(&pt, &btnReset.rect);
        btnMenu.hovered  = SDL_PointInRect(&pt, &btnMenu.rect);
        btnNext.hovered  = SDL_PointInRect(&pt, &btnNext.rect);
    }

    if (e->type == SDL_MOUSEBUTTONDOWN && e->button.button == SDL_BUTTON_LEFT) {
        int mx = e->button.x, my = e->button.y;
        SDL_Point pt = {mx, my};

        /* Bouton Suivant */
        if (g->puzzleComplete && btnNext.visible && SDL_PointInRect(&pt, &btnNext.rect)) {
            if (g->sfxClick) Mix_PlayChannel(-1, g->sfxClick, 0);
            g->currentLevel++;
            if (g->currentLevel > 4) {
                g->state = STATE_SCORES_INPUT;
            } else {
                puzzle_init_level(g);
                btnNext.visible = 0;
            }
            return;
        }

        if (SDL_PointInRect(&pt, &btnReset.rect)) {
            if (g->sfxClick) Mix_PlayChannel(-1, g->sfxClick, 0);
            puzzle_init_level(g); btnNext.visible = 0; return;
        }
        if (SDL_PointInRect(&pt, &btnMenu.rect)) {
            if (g->sfxClick) Mix_PlayChannel(-1, g->sfxClick, 0);
            g->state = STATE_MAIN_MENU; return;
        }

        /* Début drag : chercher pièce cliquée */
        for (int i = PIECE_COUNT-1; i >= 0; i--) {
            if (g->pieces[i].correct) continue;
            SDL_Rect hit = {g->pieces[i].dst.x, g->pieces[i].dst.y, PIECE_W, PIECE_H};
            if (SDL_PointInRect(&pt, &hit)) {
                draggingIdx = i;
                g->pieces[i].dragging = 1;
                g->pieces[i].offsetX  = mx - g->pieces[i].dst.x;
                g->pieces[i].offsetY  = my - g->pieces[i].dst.y;
                break;
            }
        }
    }

    if (e->type == SDL_MOUSEMOTION && draggingIdx >= 0) {
        g->pieces[draggingIdx].dst.x = e->motion.x - g->pieces[draggingIdx].offsetX;
        g->pieces[draggingIdx].dst.y = e->motion.y - g->pieces[draggingIdx].offsetY;
    }

    if (e->type == SDL_MOUSEBUTTONUP && e->button.button == SDL_BUTTON_LEFT
        && draggingIdx >= 0)
    {
        PuzzlePiece *dragged = &g->pieces[draggingIdx];
        dragged->dragging = 0;

        int dcx = dragged->dst.x + PIECE_W/2;
        int dcy = dragged->dst.y + PIECE_H/2;

        /* Trouver la cellule grille la plus proche */
        int bestDist = 999999, bestRow = -1, bestCol = -1;
        for (int row = 0; row < GRID_SIZE; row++) {
            for (int col = 0; col < GRID_SIZE; col++) {
                int gx = GRID_X + col * PIECE_W + PIECE_W/2;
                int gy = GRID_Y + row * PIECE_H + PIECE_H/2;
                int d  = abs(dcx - gx) + abs(dcy - gy);
                if (d < bestDist) { bestDist = d; bestRow = row; bestCol = col; }
            }
        }

        int snapped = 0;

        if (bestDist < SNAP_DIST && bestRow >= 0) {
            SDL_Rect cell = {
                GRID_X + bestCol * PIECE_W,
                GRID_Y + bestRow * PIECE_H,
                PIECE_W, PIECE_H
            };

            /* Trouver l'occupant éventuel de cette cellule */
            int occupant = -1;
            for (int j = 0; j < PIECE_COUNT; j++) {
                if (j == draggingIdx) continue;
                if (g->pieces[j].dst.x == cell.x &&
                    g->pieces[j].dst.y == cell.y) { occupant = j; break; }
            }

            if (occupant >= 0 && !g->pieces[occupant].correct) {
                /* Swap : l'occupant prend la place du dragged */
                SDL_Rect prev = {dragged->dst.x, dragged->dst.y, PIECE_W, PIECE_H};
                g->pieces[occupant].dst = prev;
                g->pieces[occupant].correct =
                    (prev.x == g->pieces[occupant].target.x &&
                     prev.y == g->pieces[occupant].target.y);
            }

            dragged->dst     = cell;
            dragged->correct = (cell.x == dragged->target.x &&
                                cell.y == dragged->target.y);
            snapped = 1;
        }

        /* Pas snapé → renvoyer dans un slot libre du plateau */
        if (!snapped) {
            for (int s = 0; s < PIECE_COUNT; s++) {
                int occ = 0;
                for (int j = 0; j < PIECE_COUNT; j++) {
                    if (j == draggingIdx || g->pieces[j].correct) continue;
                    if (g->pieces[j].dst.x == traySlots[s].x &&
                        g->pieces[j].dst.y == traySlots[s].y) { occ = 1; break; }
                }
                if (!occ) {
                    /* traySlots[s] a déjà w=PIECE_W, h=PIECE_H */
                    dragged->dst = traySlots[s];
                    break;
                }
            }
            dragged->correct = 0;
        }

        draggingIdx = -1;

        if (check_complete(g)) {
            g->puzzleComplete = 1;
            g->currentScore  += 20;
            music_play(g, 3);
        }
    }
}
