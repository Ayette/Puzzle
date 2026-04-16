/* ============================================================
   jeu.h  –  Declarations du jeu PuzzleQuest
   Auteur  : [Votre nom]

   Contient :
     - Les constantes du jeu
     - La structure Puzzle (exigence du prof)
     - Les autres structures necessaires
     - Les declarations des 6 fonctions
   ============================================================ */

#ifndef JEU_H
#define JEU_H

/* --- Bibliotheques SDL2 --- */
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

/* --- Bibliotheques C standard --- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

/* ============================================================
   CONSTANTES
   ============================================================ */
#define LARGEUR          1024
#define HAUTEUR           768
#define TAILLE_GRILLE       3       /* grille 3x3                */
#define NB_PIECES           9       /* 3 x 3 pieces              */
#define GRILLE_X           20       /* position grille gauche    */
#define GRILLE_Y          100
#define TAILLE_PIECE      160       /* 480 / 3                   */
#define PLATEAU_X         540       /* position plateau droit    */
#define PLATEAU_Y         100
#define ECART_SLOT          6       /* espace entre les slots    */
#define MAX_QUESTIONS      50
#define NB_Q_SESSION        5       /* questions par partie      */
#define MAX_SCORES         20

/* Etats du jeu - simples constantes int (pas d'enum) */
#define MENU_PRINCIPAL      0
#define MENU_OPTIONS        1
#define MENU_SAUVEGARDE     2
#define MENU_JOUEUR         3
#define MENU_SCORES         4
#define MENU_ENIGME         5
#define MODE_QUIZ           6
#define MODE_PUZZLE         7
#define QUITTER             8

/* ============================================================
   STRUCTURE PUZZLE  (exigee par le professeur)
   Represente UNE piece du puzzle
   ============================================================ */
typedef struct puzzle {
    SDL_Rect source;       /* portion de l'image originale    */
    SDL_Rect position;     /* position actuelle a l'ecran     */
    SDL_Rect cible;        /* position correcte dans la grille*/
    int      en_place;     /* 1 = correctement posee          */
    int      drag;         /* 1 = en cours de deplacement     */
    int      ox;           /* offset souris x lors du drag    */
    int      oy;           /* offset souris y lors du drag    */
} Puzzle;

/* ============================================================
   STRUCTURE QUESTION  (lue depuis questions.txt)
   ============================================================ */
typedef struct {
    char texte[256];
    char choix[3][128];
    int  bonne;
} Question;

/* ============================================================
   STRUCTURE SCORE
   ============================================================ */
typedef struct {
    char nom[64];
    int  pts;
} Score;

/* ============================================================
   STRUCTURE JEU  (contexte global passe par pointeur)
   Remplace toutes les variables globales interdites
   ============================================================ */
typedef struct {
    SDL_Window    *fenetre;
    SDL_Renderer  *rendu;
    TTF_Font      *police;
    TTF_Font      *petite;

    SDL_Texture   *fond[7];          /* fond[1..6] = menu1..6   */
    SDL_Texture   *img_puzzle[5];    /* img_puzzle[1..4]        */
    Mix_Music     *musique[5];       /* musique[1..4]           */
    Mix_Chunk     *son_clic;
    Mix_Chunk     *son_survol;

    int  etat;                       /* MENU_xxx ou MODE_xxx    */
    int  phase;                      /* sous-etape d'un menu    */
    int  volume;                     /* 0..128                  */
    int  plein_ecran;

    char nom_joueur[64];
    int  len_nom;
    int  avatar;                     /* 1=guerrier 2=mage       */
    int  multijoueur;
    int  score;

    Puzzle    pieces[NB_PIECES];
    int       niveau;                /* 1..4                    */
    int       termine;               /* puzzle complete ?        */

    Question  questions[MAX_QUESTIONS];
    int       nb_questions;
    int       session[NB_Q_SESSION]; /* indices tires           */
    int       q_actuelle;
    int       repondu;               /* 0/1=ok/2=faux           */
    unsigned  t_reponse;
} Jeu;

/* ============================================================
   LES 6 FONCTIONS  (limite du prof)
   ============================================================ */
int  initialiser_jeu(Jeu *j);
int  charger_questions(Jeu *j, const char *fichier);
void dessiner_jeu(Jeu *j);
void gerer_evenements(Jeu *j);
int  sauvegarder_score(const char *nom, int pts);
void nettoyer_jeu(Jeu *j);

#endif
