/* menu_player.c – Player Menu */
#include "../include/game.h"
#include <math.h>

/* Forward declaration */
static void fill_circle_player(SDL_Renderer *r, int cx, int cy, int rad,
                                Uint8 R, Uint8 G, Uint8 B, Uint8 A);

/* Phase 0: mono/multi */
#define BTN_MONO   0
#define BTN_MULTI  1
/* Phase 1: avatar/input selection */
#define BTN_AV1    2
#define BTN_AV2    3
#define BTN_IN1    4
#define BTN_IN2    5
#define BTN_VALID  6
#define BTN_BACK   7
#define BTN_COUNT  8

static Button btns[BTN_COUNT];
static int    phase;          /* 0 = mono/multi, 1 = avatar select */
static int    selAvatar;      /* 1 or 2 */
static int    selInput;       /* 1 or 2 */

void menu_player_enter(Game *g)
{
    phase     = 0;
    selAvatar = 1;
    selInput  = 1;

    int cx = WIN_W / 2;

    button_init(&btns[BTN_MONO],  cx - 150, 350, 180, 55, "Mono Joueur");
    button_init(&btns[BTN_MULTI], cx +  30, 350, 180, 55, "Multi Joueurs");

    /* Phase 1 : avatars cliquables + boutons bas */
    button_init(&btns[BTN_AV1],   cx - 210, 150, 150, 150, "");   /* zone avatar */
    button_init(&btns[BTN_AV2],   cx +  60, 150, 150, 150, "");
    button_init(&btns[BTN_IN1],   cx - 150, 440, 130, 48, "Input 1");
    button_init(&btns[BTN_IN2],   cx +  20, 440, 130, 48, "Input 2");
    button_init(&btns[BTN_VALID], cx -  70, 510, 140, 50, "Valider");
    button_init(&btns[BTN_BACK],  30, WIN_H - 75, 150, 48, "Retour");

    /* hide select-phase buttons initially; avatar zones invisibles */
    for (int i = BTN_AV1; i <= BTN_VALID; i++) btns[i].visible = 0;
    btns[BTN_AV1].visible = 0;
    btns[BTN_AV2].visible = 0;

    music_play(g, 2);
    g->prevHovered = -1;
}

/* ── Dessiner un avatar SDL2 pur ──────────────────────────────── */
/* type 0 = Guerrier (rouge/or), type 1 = Mage (bleu/violet)      */
static void draw_avatar(SDL_Renderer *ren, int x, int y, int size,
                        int type, int selected)
{
    int hw = size / 2;
    int cx = x + hw;

    /* Couleurs selon type */
    SDL_Color skinCol  = {210, 170, 120, 255};
    SDL_Color bodyCol  = type == 0 ? (SDL_Color){180, 40,  40, 255}
                                   : (SDL_Color){ 40, 40, 180, 255};
    SDL_Color accentCol= type == 0 ? (SDL_Color){220,180,  30, 255}
                                   : (SDL_Color){160, 60, 220, 255};
    SDL_Color hairCol  = type == 0 ? (SDL_Color){ 80, 40,   0, 255}
                                   : (SDL_Color){ 20, 20,  60, 255};

    /* Cadre sélection */
    if (selected) {
        SDL_SetRenderDrawColor(ren, 50, 220, 50, 255);
        SDL_Rect sel = {x-4, y-4, size+8, size+8};
        SDL_RenderDrawRect(ren, &sel);
        SDL_Rect sel2 = {x-6, y-6, size+12, size+12};
        SDL_RenderDrawRect(ren, &sel2);
    }

    /* Fond avatar */
    SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(ren, 20, 20, 50, 200);
    SDL_Rect bg = {x, y, size, size};
    SDL_RenderFillRect(ren, &bg);
    SDL_SetRenderDrawColor(ren, accentCol.r, accentCol.g, accentCol.b, 180);
    SDL_RenderDrawRect(ren, &bg);

    /* ── Corps (torse) ── */
    SDL_Rect torso = {x + size/4, y + size*2/5, size/2, size*2/5};
    SDL_SetRenderDrawColor(ren, bodyCol.r, bodyCol.g, bodyCol.b, 255);
    SDL_RenderFillRect(ren, &torso);

    /* Détail torse : bande centrale */
    SDL_Rect band = {cx - 4, torso.y, 8, torso.h};
    SDL_SetRenderDrawColor(ren, accentCol.r, accentCol.g, accentCol.b, 255);
    SDL_RenderFillRect(ren, &band);

    /* Épaules */
    SDL_Rect shL = {x + size/8,     y + size*2/5 - 8, size/6, 14};
    SDL_Rect shR = {x + size*11/16, y + size*2/5 - 8, size/6, 14};
    SDL_SetRenderDrawColor(ren, accentCol.r, accentCol.g, accentCol.b, 255);
    SDL_RenderFillRect(ren, &shL);
    SDL_RenderFillRect(ren, &shR);

    /* Bras gauche */
    SDL_Rect armL = {x + size/8, y + size*2/5, size/8, size/3};
    SDL_SetRenderDrawColor(ren, bodyCol.r, bodyCol.g, bodyCol.b, 230);
    SDL_RenderFillRect(ren, &armL);
    /* Main gauche */
    SDL_Rect handL = {armL.x - 3, armL.y + armL.h, armL.w + 6, 12};
    SDL_SetRenderDrawColor(ren, skinCol.r, skinCol.g, skinCol.b, 255);
    SDL_RenderFillRect(ren, &handL);

    /* Bras droit */
    SDL_Rect armR = {x + size*11/16, y + size*2/5, size/8, size/3};
    SDL_SetRenderDrawColor(ren, bodyCol.r, bodyCol.g, bodyCol.b, 230);
    SDL_RenderFillRect(ren, &armR);
    /* Main droite */
    SDL_Rect handR = {armR.x - 3, armR.y + armR.h, armR.w + 6, 12};
    SDL_SetRenderDrawColor(ren, skinCol.r, skinCol.g, skinCol.b, 255);
    SDL_RenderFillRect(ren, &handR);

    /* Arme : épée (guerrier) ou bâton (mage) */
    if (type == 0) {
        /* Épée */
        SDL_SetRenderDrawColor(ren, 200, 200, 220, 255);
        SDL_RenderDrawLine(ren, armR.x + armR.w + 5, armR.y - 20,
                                armR.x + armR.w + 5, armR.y + armR.h + 15);
        SDL_SetRenderDrawColor(ren, accentCol.r, accentCol.g, accentCol.b, 255);
        SDL_RenderDrawLine(ren, armR.x - 2, armR.y + armR.h/2,
                                armR.x + armR.w + 12, armR.y + armR.h/2);
    } else {
        /* Bâton magique */
        SDL_SetRenderDrawColor(ren, 100, 60, 20, 255);
        SDL_RenderDrawLine(ren, armR.x + 4, armR.y - 30,
                                armR.x + 4, armR.y + armR.h + 10);
        /* Orbe brillant */
        fill_circle_player(ren, armR.x + 4, armR.y - 30, 8,
                           accentCol.r, accentCol.g, accentCol.b, 255);
    }

    /* ── Tête ── */
    int headR = size / 7;
    int headY = y + size/5;
    /* Cou */
    SDL_Rect neck = {cx - 5, headY + headR * 2 - 4, 10, 12};
    SDL_SetRenderDrawColor(ren, skinCol.r, skinCol.g, skinCol.b, 255);
    SDL_RenderFillRect(ren, &neck);
    /* Visage (cercle approché) */
    fill_circle_player(ren, cx, headY + headR, headR,
                       skinCol.r, skinCol.g, skinCol.b, 255);

    /* Yeux */
    SDL_SetRenderDrawColor(ren, 30, 30, 80, 255);
    SDL_Rect eyeL = {cx - headR/2 - 3, headY + headR - 3, 5, 4};
    SDL_Rect eyeR = {cx + headR/2 - 2, headY + headR - 3, 5, 4};
    SDL_RenderFillRect(ren, &eyeL);
    SDL_RenderFillRect(ren, &eyeR);

    /* Bouche */
    SDL_SetRenderDrawColor(ren, 160, 80, 60, 255);
    SDL_RenderDrawLine(ren, cx - 5, headY + headR + 5, cx + 5, headY + headR + 5);

    /* Cheveux / casque */
    if (type == 0) {
        /* Casque guerrier */
        SDL_Rect helm = {cx - headR - 2, headY, headR*2 + 4, headR};
        SDL_SetRenderDrawColor(ren, accentCol.r, accentCol.g, accentCol.b, 255);
        SDL_RenderFillRect(ren, &helm);
        /* Panache */
        SDL_SetRenderDrawColor(ren, 200, 30, 30, 255);
        SDL_RenderDrawLine(ren, cx, headY - 2, cx, headY - 14);
        SDL_RenderDrawLine(ren, cx-1, headY - 2, cx-1, headY - 14);
    } else {
        /* Capuche mage */
        SDL_Rect hood = {cx - headR - 4, headY - 6, headR*2 + 8, headR + 8};
        SDL_SetRenderDrawColor(ren, hairCol.r, hairCol.g, hairCol.b, 200);
        SDL_RenderFillRect(ren, &hood);
        /* Bord capuche */
        SDL_SetRenderDrawColor(ren, accentCol.r, accentCol.g, accentCol.b, 200);
        SDL_RenderDrawRect(ren, &hood);
    }

    /* ── Bas du corps ── */
    SDL_Rect legs = {x + size/4, y + size*4/5, size/2, size/5};
    SDL_SetRenderDrawColor(ren, type == 0 ? 60 : 30,
                                type == 0 ? 30 : 30,
                                type == 0 ? 30 : 80, 255);
    SDL_RenderFillRect(ren, &legs);
    /* Séparation jambes */
    SDL_SetRenderDrawColor(ren, 10, 10, 20, 255);
    SDL_RenderDrawLine(ren, cx, legs.y, cx, legs.y + legs.h);
}

/* cercle rempli helper local */
static void fill_circle_player(SDL_Renderer *r, int cx, int cy, int rad,
                                Uint8 R, Uint8 G, Uint8 B, Uint8 A)
{
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(r, R, G, B, A);
    for (int dy = -rad; dy <= rad; dy++) {
        int dx = (int)sqrt((double)(rad*rad - dy*dy));
        SDL_RenderDrawLine(r, cx-dx, cy+dy, cx+dx, cy+dy);
    }
}

void menu_player_draw(Game *g)
{
    render_bg(g, 4);

    SDL_Color white  = {255, 255, 255, 255};
    SDL_Color yellow = {255, 220,  50, 255};
    SDL_Color green  = {100, 230, 100, 255};
    SDL_Color grey   = {190, 190, 210, 255};

    render_text_centered(g, "Sous-Menu Joueur", 30, yellow);

    if (phase == 0) {
        for (int i = BTN_MONO; i <= BTN_MULTI; i++) btns[i].visible = 1;
        for (int i = BTN_AV1;  i <= BTN_VALID; i++) btns[i].visible = 0;

        render_text_centered(g, "Choisissez votre mode de jeu", 200, white);

    } else {
        for (int i = BTN_MONO; i <= BTN_MULTI; i++) btns[i].visible = 0;
        /* Avatar zones : visibles pour clic mais pas dessinées comme boutons */
        btns[BTN_AV1].visible  = 0;
        btns[BTN_AV2].visible  = 0;
        btns[BTN_IN1].visible  = 1;
        btns[BTN_IN2].visible  = 1;
        btns[BTN_VALID].visible = 1;

        char modeStr[64];
        snprintf(modeStr, sizeof(modeStr), "Mode: %s",
                 g->multiplayer ? "Multi Joueurs" : "Mono Joueur");
        render_text_centered(g, modeStr, 80, grey);

        /* ── Avatars dessinés en SDL2 ── */
        int avSize = 150;
        int av1X   = WIN_W/2 - avSize - 60;
        int av2X   = WIN_W/2 + 60;
        int avY    = 150;   /* bien en dessous du texte mode */

        /* Cadres de sélection avant l'avatar */
        draw_avatar(g->renderer, av1X, avY, avSize, 0, selAvatar == 1);
        draw_avatar(g->renderer, av2X, avY, avSize, 1, selAvatar == 2);

        /* Noms sous les avatars */
        SDL_Color nameCol1 = (selAvatar == 1) ? green : white;
        SDL_Color nameCol2 = (selAvatar == 2) ? green : white;
        render_text(g, "Guerrier", av1X + avSize/2 - 38, avY + avSize + 8, nameCol1);
        render_text(g, "Mage",     av2X + avSize/2 - 22, avY + avSize + 8, nameCol2);

        /* Sélection info — bien en dessous des avatars */
        char avStr[64];
        snprintf(avStr, sizeof(avStr), "Selectionne : %s",
                 selAvatar == 1 ? "Guerrier" : "Mage");
        render_text_centered(g, avStr, avY + avSize + 50, green);

        render_text_centered(g, "(Appuyez Entree pour valider)", avY + avSize + 90, grey);
    }

    buttons_draw(g, btns, BTN_COUNT);
}

void menu_player_event(Game *g, SDL_Event *e)
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
        case BTN_MONO:
            g->multiplayer = 0;
            phase = 1;
            break;
        case BTN_MULTI:
            g->multiplayer = 1;
            phase = 1;
            break;
        case BTN_AV1:  selAvatar = 1; break;
        case BTN_AV2:  selAvatar = 2; break;
        case BTN_IN1:  selInput  = 1; break;
        case BTN_IN2:  selInput  = 2; break;
        case BTN_VALID:
            g->selectedAvatar = selAvatar;
            g->state = STATE_SCORES_INPUT;
            break;
        case BTN_BACK:
            g->state = STATE_MAIN_MENU;
            break;
        }
    }

    if (e->type == SDL_KEYDOWN) {
        switch (e->key.keysym.sym) {
        case SDLK_RETURN:
        case SDLK_KP_ENTER:
            if (phase == 1) {
                g->selectedAvatar = selAvatar;
                g->state = STATE_SCORES_INPUT;
            }
            break;
        case SDLK_ESCAPE:
            g->state = STATE_MAIN_MENU;
            break;
        }
    }
}
