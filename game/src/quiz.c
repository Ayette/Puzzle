/* quiz.c – Quiz mode : 30 questions sur le film 300
 * A chaque partie, 5 questions sont tirées aléatoirement parmi les 30.
 */
#include "../include/game.h"

/* ── 30 questions sur le film 300 ──────────────────────────── */
#define TOTAL_QUESTIONS 30
#define QUIZ_SIZE        5   /* questions par session */

static QuizQuestion allQuestions[TOTAL_QUESTIONS] = {
    /* 0 */
    {"Quel roi spartiate mene les 300 guerriers ?",
     {"Leonidas", "Xerxes", "Themistocle"}, 0},
    /* 1 */
    {"Combien de guerriers Spartiates combattent aux Thermopyles ?",
     {"100", "300", "500"}, 1},
    /* 2 */
    {"Qui est le roi perse dans le film 300 ?",
     {"Darius", "Xerxes", "Cyrus"}, 1},
    /* 3 */
    {"Dans quelle bataille se deroule le film 300 ?",
     {"Marathon", "Salamine", "Thermopyles"}, 2},
    /* 4 */
    {"Quel acteur joue le role de Leonidas ?",
     {"Brad Pitt", "Gerard Butler", "Russell Crowe"}, 1},
    /* 5 */
    {"Quelle est la fameuse replique de Leonidas face aux fleches perses ?",
     {"Ils mourront debout", "Alors on se battra a l'ombre", "Jamais !"}, 1},
    /* 6 */
    {"Comment s'appelle la femme de Leonidas ?",
     {"Helena", "Gorgo", "Cassandra"}, 1},
    /* 7 */
    {"En quelle annee se deroule la bataille des Thermopyles ?",
     {"480 av. J-C", "330 av. J-C", "550 av. J-C"}, 0},
    /* 8 */
    {"Quel traitre spartiate revele le chemin secret aux Perses ?",
     {"Theron", "Ephialtes", "Daxos"}, 1},
    /* 9 */
    {"Xerxes se presente comme quoi dans le film ?",
     {"Roi des rois", "Dieu vivant", "Roi des rois et dieu vivant"}, 2},
    /* 10 */
    {"Combien d'hommes Xerxes pretend-il avoir dans son armee ?",
     {"Un million", "Un million d'hommes", "Des millions"}, 2},
    /* 11 */
    {"Quel est le nom du passage defendu par les Spartiates ?",
     {"Les Thermopyles", "Le Bosphore", "L'Hellesont"}, 0},
    /* 12 */
    {"Quel est le sort final de Leonidas ?",
     {"Il survit et rentre a Sparte", "Il est capture", "Il meurt au combat"}, 2},
    /* 13 */
    {"Comment les Spartiates forment-ils leur defense principale ?",
     {"La phalange", "La tortue romaine", "La ligne de feu"}, 0},
    /* 14 */
    {"Qui raconte l'histoire aux soldats spartiates avant la bataille de Platees ?",
     {"Gorgo", "Daxos", "Dilios"}, 2},
    /* 15 */
    {"De quelle cite viennent les allies grecs de Leonidas ?",
     {"Athenes", "Arcadie", "Corinthe"}, 1},
    /* 16 */
    {"Que font les Spartiates des messagers perses venus demander la capitulation ?",
     {"Ils les emprisonnent", "Ils les renvoient", "Ils les jettent dans un puits"}, 2},
    /* 17 */
    {"Comment s'appelle le film realise par Zack Snyder en 2006 ?",
     {"Troie", "300", "Alexandre"}, 1},
    /* 18 */
    {"Sur quel ouvrage graphique est base le film 300 ?",
     {"Un roman de Frank Miller", "Une BD de Frank Miller", "Un manga"}, 1},
    /* 19 */
    {"Quel age ont les Spartiates quand ils commencent l'entrainement militaire ?",
     {"5 ans", "7 ans", "10 ans"}, 1},
    /* 20 */
    {"Comment appelle-t-on l'entrainement militaire des jeunes Spartiates ?",
     {"L'agoge", "Le gymnasium", "L'ephebeia"}, 0},
    /* 21 */
    {"Quelle unite d'elite perse Leonidas affronte-t-il directement ?",
     {"Les Immortels", "Les Janissaires", "Les Cataphractaires"}, 0},
    /* 22 */
    {"Quelle est la duree approximative du film 300 ?",
     {"1h17", "1h57", "2h30"}, 1},
    /* 23 */
    {"Quel effet visuel caracteristique est utilise dans le film 300 ?",
     {"Motion capture totale", "Bullet time et desaturation", "Animation 3D pure"}, 1},
    /* 24 */
    {"Xerxes est represente comme etant d'une taille inhabituelle. Combien mesure-t-il ?",
     {"2 metres", "Plus de 2m50", "3 metres"}, 1},
    /* 25 */
    {"Comment Leonidas justifie-t-il de n'amener que 300 hommes ?",
     {"C'est un ordre du conseil", "Il dit qu'il va juste se promener",
      "Les oracles ont interdit la guerre"}, 2},
    /* 26 */
    {"Quel est le symbole sur le bouclier des Spartiates ?",
     {"Un aigle", "La lettre Lambda (L)", "Un lion"}, 1},
    /* 27 */
    {"Comment se nomme le conseil de vieillards spartiates qui s'oppose a Leonidas ?",
     {"L'Agora", "Les Ephores", "Le Senat"}, 1},
    /* 28 */
    {"Qu'est-ce qu'un hoplite ?",
     {"Un cavalier grec", "Un fantassin grec lourdement arme", "Un archer"}, 1},
    /* 29 */
    {"Apres la mort de Leonidas, qui continue la guerre contre les Perses ?",
     {"Gorgo avec l'armee grecque reunifiee", "Personne", "Les Atheniens seuls"}, 0},
};

/* ── Questions tirées pour cette session ───────────────────── */
static int    sessionIdx[QUIZ_SIZE];   /* indices dans allQuestions */
static int    currentQ;                /* 0..QUIZ_SIZE-1             */
static int    answered;                /* 0=attente, 1=correct, 2=faux */
static Uint32 answerTime;

#define BTN_A     0
#define BTN_B     1
#define BTN_C     2
#define BTN_BACK  3
#define BTN_COUNT 4

static Button btns[BTN_COUNT];

/* ── Fisher-Yates pour tirer 5 questions parmi 30 ──────────── */
static void pick_questions(void)
{
    int pool[TOTAL_QUESTIONS];
    for (int i = 0; i < TOTAL_QUESTIONS; i++) pool[i] = i;
    /* Mélange partiel : on n'a besoin que des QUIZ_SIZE premiers */
    for (int i = 0; i < QUIZ_SIZE; i++) {
        int j = i + rand() % (TOTAL_QUESTIONS - i);
        int t = pool[i]; pool[i] = pool[j]; pool[j] = t;
        sessionIdx[i] = pool[i];
    }
}

void quiz_enter(Game *g)
{
    pick_questions();
    currentQ  = 0;
    answered  = 0;
    music_play(g, 4);
    g->prevHovered = -1;

    int y = 500, w = 240, h = 55;
    button_init(&btns[BTN_A],    55,         y, w, h, "");
    button_init(&btns[BTN_B],    WIN_W/2-120, y, w, h, "");
    button_init(&btns[BTN_C],    WIN_W-295,   y, w, h, "");
    button_init(&btns[BTN_BACK], 30, WIN_H - 75, 160, 45, "Quitter");
}

static void load_current_buttons(void)
{
    if (currentQ < QUIZ_SIZE) {
        QuizQuestion *q = &allQuestions[sessionIdx[currentQ]];
        strncpy(btns[BTN_A].label, q->answers[0], 63);
        strncpy(btns[BTN_B].label, q->answers[1], 63);
        strncpy(btns[BTN_C].label, q->answers[2], 63);
        btns[BTN_A].label[63] = btns[BTN_B].label[63] = btns[BTN_C].label[63] = '\0';
    }
}

void quiz_draw(Game *g)
{
    render_bg(g, 6);

    SDL_Color white  = {255, 255, 255, 255};
    SDL_Color yellow = {255, 220,  50, 255};
    SDL_Color green  = { 50, 220,  50, 255};
    SDL_Color red    = {220,  50,  50, 255};
    SDL_Color cyan   = { 80, 200, 220, 255};

    /* Titre */
    render_text_centered(g, "QUIZ — Les 300 de Sparte", 25, yellow);

    if (currentQ >= QUIZ_SIZE) {
        /* ── Fin du quiz ── */
        SDL_Rect panel = {WIN_W/2-280, 200, 560, 300};
        render_panel(g, &panel, 200);
        SDL_SetRenderDrawColor(g->renderer, 50,220,50,200);
        SDL_RenderDrawRect(g->renderer, &panel);
        render_text_centered(g, "Quiz termine !", 230, green);
        char sc[64];
        snprintf(sc, sizeof(sc), "Score total : %d pts", g->currentScore);
        render_text_centered(g, sc, 290, yellow);
        render_text_centered(g, "Molon Labe ! (Venez les prendre !)", 360, cyan);
        render_text_centered(g, "(ESC pour quitter)", 430, white);
        Button backOnly = btns[BTN_BACK];
        backOnly.visible = 1;
        buttons_draw(g, &backOnly, 1);
        return;
    }

    QuizQuestion *q = &allQuestions[sessionIdx[currentQ]];

    /* Compteur */
    char counter[32];
    snprintf(counter, sizeof(counter), "Question %d / %d", currentQ + 1, QUIZ_SIZE);
    render_text_centered(g, counter, 75, white);

    /* Score */
    char sc[32];
    snprintf(sc, sizeof(sc), "Score: %d", g->currentScore);
    render_text(g, sc, WIN_W - 180, 75, yellow);

    /* ── Bandeau question ── */
    SDL_Rect band = {40, 130, WIN_W - 80, 120};
    SDL_SetRenderDrawBlendMode(g->renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(g->renderer, 10, 10, 40, 200);
    SDL_RenderFillRect(g->renderer, &band);
    SDL_SetRenderDrawColor(g->renderer, 80, 140, 255, 255);
    SDL_RenderDrawRect(g->renderer, &band);

    /* Question (wrap simple à 55 chars) */
    int qlen = (int)strlen(q->question);
    int lw   = 55;
    int line = 0;
    for (int s = 0; s < qlen; s += lw) {
        char buf[64] = {0};
        int end = s + lw > qlen ? qlen : s + lw;
        strncpy(buf, q->question + s, end - s);
        render_text(g, buf, band.x + 14, band.y + 18 + line * 36, white);
        line++;
    }

    /* ── Décor thématique : quote spartiate ── */
    SDL_Rect deco = {40, 270, WIN_W - 80, 40};
    SDL_SetRenderDrawColor(g->renderer, 10, 10, 30, 160);
    SDL_RenderFillRect(g->renderer, &deco);
    render_text_small_centered(g, "\"Molon Labe\" — Leonidas I, Roi de Sparte", 278, cyan);

    /* ── Boutons réponses ── */
    load_current_buttons();

    /* Colorer les boutons selon feedback */
    if (answered) {
        int correct_idx = q->correct_idx;
        /* Bouton correct en vert, mauvais choix en rouge */
        for (int i = BTN_A; i <= BTN_C; i++) {
            if (i == correct_idx) {
                /* On ne peut pas changer la couleur via Button directement,
                   on dessine un overlay */
                SDL_SetRenderDrawBlendMode(g->renderer, SDL_BLENDMODE_BLEND);
                SDL_SetRenderDrawColor(g->renderer, 0, 200, 0, 80);
                SDL_RenderFillRect(g->renderer, &btns[i].rect);
            }
        }
    }

    buttons_draw(g, btns, BTN_COUNT);

    /* Feedback texte */
    if (answered == 1) render_text_centered(g, "Bonne reponse ! +10 pts", 455, green);
    if (answered == 2) render_text_centered(g, "Mauvaise reponse...",      455, red);

    /* Instruction */
    render_text_small_centered(g, "Choisissez la bonne reponse", 590, white);
}

void quiz_event(Game *g, SDL_Event *e)
{
    /* Auto-avance après 1.8s */
    if (answered && SDL_GetTicks() - answerTime > 1800) {
        currentQ++;
        answered = 0;
    }

    if (e->type == SDL_MOUSEMOTION)
        buttons_handle_hover(g, btns, BTN_COUNT, e->motion.x, e->motion.y);

    if (e->type == SDL_MOUSEBUTTONDOWN && e->button.button == SDL_BUTTON_LEFT) {
        if (answered) return;

        int idx = buttons_handle_click(btns, BTN_COUNT, e->button.x, e->button.y);
        if (idx >= 0 && g->sfxClick)
            Mix_PlayChannel(-1, g->sfxClick, 0);

        if (idx == BTN_BACK) { g->state = STATE_MAIN_MENU; return; }

        if (idx >= BTN_A && idx <= BTN_C && currentQ < QUIZ_SIZE) {
            int correct = allQuestions[sessionIdx[currentQ]].correct_idx;
            if (idx == correct) {
                answered = 1;
                g->currentScore += 10;
            } else {
                answered = 2;
            }
            answerTime = SDL_GetTicks();
        }
    }

    if (e->type == SDL_KEYDOWN && e->key.keysym.sym == SDLK_ESCAPE)
        g->state = STATE_MAIN_MENU;
}
