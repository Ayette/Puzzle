/* ============================================================
   main.c  –  Point d'entree du programme PuzzleQuest
   Auteur  : [Votre nom]

   Ce fichier contient uniquement la fonction main().
   Il appelle les 6 fonctions definies dans jeu.c.

   Structure du programme :
     1. Initialiser le jeu
     2. Charger les questions depuis questions.txt
     3. Boucle principale : dessiner + gerer les evenements
     4. Nettoyer et quitter

   Compilation : make
   Execution   : ./puzzlequest
   ============================================================ */

#include "jeu.h"

int main(void)
{
    /* Declarer la variable principale du jeu
       (remplace toutes les variables globales) */
    Jeu mon_jeu;

    /* --- Etape 1 : Initialiser SDL2 et charger les assets --- */
    if (initialiser_jeu(&mon_jeu) == 0) {
        /* Si l'initialisation echoue, on arrete tout */
        printf("Echec de l'initialisation. Verifiez SDL2.\n");
        return 1;
    }

    /* --- Etape 2 : Charger les questions depuis le fichier --- */
    if (charger_questions(&mon_jeu, "questions.txt") == 0) {
        /* Le jeu peut continuer sans questions (pas fatal) */
        printf("Attention : aucune question chargee.\n");
    }

    /* --- Etape 3 : Activer la saisie de texte (pour le nom) --- */
    SDL_StartTextInput();

    /* ===================================================
       BOUCLE PRINCIPALE DU JEU
       Tourne jusqu'a ce que l'etat passe a ETAT_QUITTER
       =================================================== */
    while (mon_jeu.etat != QUITTER) {

        /* Limiter a environ 60 images par seconde */
        SDL_Delay(16);

        /* Afficher l'ecran courant */
        dessiner_jeu(&mon_jeu);

        /* Lire et traiter les actions de l'utilisateur */
        gerer_evenements(&mon_jeu);
    }

    /* --- Etape 4 : Liberer la memoire et fermer SDL2 --- */
    SDL_StopTextInput();
    nettoyer_jeu(&mon_jeu);

    return 0; /* programme termine sans erreur */
}
