/* button.c – button helpers */
#include "../include/game.h"

/* Colour palette */
#define COL_BTN_NORMAL  {  30,  30,  50, 200}
#define COL_BTN_HOVER   {  80, 140, 220, 230}
#define COL_BTN_BORDER  { 120, 180, 255, 255}
#define COL_TEXT_NORMAL {230, 230, 230, 255}
#define COL_TEXT_HOVER  {255, 255, 255, 255}

/* Forward declaration so buttons_draw can call it */
void render_text_centered_in_rect(Game *g, const char *text,
                                  SDL_Rect *rect, SDL_Color col);

void button_init(Button *b, int x, int y, int w, int h, const char *label)
{
    b->rect    = (SDL_Rect){x, y, w, h};
    b->hovered = 0;
    b->visible = 1;
    strncpy(b->label, label, sizeof(b->label) - 1);
    b->label[sizeof(b->label) - 1] = '\0';
}

void buttons_draw(Game *g, Button *btns, int count)
{
    SDL_Color colNormal = COL_BTN_NORMAL;
    SDL_Color colHover  = COL_BTN_HOVER;
    SDL_Color colBorder = COL_BTN_BORDER;
    SDL_Color colTextN  = COL_TEXT_NORMAL;
    SDL_Color colTextH  = COL_TEXT_HOVER;

    for (int i = 0; i < count; i++) {
        if (!btns[i].visible) continue;

        SDL_Color bg   = btns[i].hovered ? colHover  : colNormal;
        SDL_Color txtC = btns[i].hovered ? colTextH  : colTextN;

        /* Shadow */
        SDL_Rect shadow = {btns[i].rect.x + 4, btns[i].rect.y + 4,
                           btns[i].rect.w,     btns[i].rect.h};
        SDL_SetRenderDrawColor(g->renderer, 0, 0, 0, 100);
        SDL_RenderFillRect(g->renderer, &shadow);

        /* Fill */
        SDL_SetRenderDrawBlendMode(g->renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(g->renderer, bg.r, bg.g, bg.b, bg.a);
        SDL_RenderFillRect(g->renderer, &btns[i].rect);

        /* Border */
        SDL_SetRenderDrawColor(g->renderer,
                               colBorder.r, colBorder.g, colBorder.b, colBorder.a);
        SDL_RenderDrawRect(g->renderer, &btns[i].rect);

        /* Label */
        render_text_centered_in_rect(g, btns[i].label, &btns[i].rect, txtC);
    }
}

/* Render text centred inside a rect */
void render_text_centered_in_rect(Game *g, const char *text,
                                  SDL_Rect *rect, SDL_Color col)
{
    if (!g->font || !text || text[0] == '\0') return;
    SDL_Surface *surf = TTF_RenderUTF8_Blended(g->font, text, col);
    if (!surf) return;
    SDL_Texture *tex = SDL_CreateTextureFromSurface(g->renderer, surf);
    if (tex) {
        SDL_Rect dst = {
            rect->x + (rect->w - surf->w) / 2,
            rect->y + (rect->h - surf->h) / 2,
            surf->w, surf->h
        };
        SDL_RenderCopy(g->renderer, tex, NULL, &dst);
        SDL_DestroyTexture(tex);
    }
    SDL_FreeSurface(surf);
}

/* Returns index of newly hovered button or -1; plays hover SFX once */
int buttons_handle_hover(Game *g, Button *btns, int count, int mx, int my)
{
    int hovered = -1;
    for (int i = 0; i < count; i++) {
        if (!btns[i].visible) { btns[i].hovered = 0; continue; }
        SDL_Point p = {mx, my};
        btns[i].hovered = SDL_PointInRect(&p, &btns[i].rect);
        if (btns[i].hovered) hovered = i;
    }

    /* Play hover sound only on newly entered button */
    if (hovered != g->prevHovered && hovered >= 0) {
        if (g->sfxHover) Mix_PlayChannel(-1, g->sfxHover, 0);
    }
    g->prevHovered = hovered;
    return hovered;
}

/* Returns index of clicked button or -1 */
int buttons_handle_click(Button *btns, int count, int mx, int my)
{
    SDL_Point p = {mx, my};
    for (int i = 0; i < count; i++) {
        if (!btns[i].visible) continue;
        if (SDL_PointInRect(&p, &btns[i].rect)) return i;
    }
    return -1;
}
