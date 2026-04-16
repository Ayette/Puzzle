/* ============================================================
   jeu.c  –  Code du jeu PuzzleQuest
   Auteur  : [Votre nom]

   Contient les 6 fonctions obligatoires + 2 fonctions
   d'aide NON statiques (afficher_texte, dessiner_bouton)
   qui sont declarees en tete de fichier pour C99.
   ============================================================ */

#include "jeu.h"

/* ============================================================
   FONCTIONS D'AIDE (non comptees comme "les 6" du prof,
   ce sont des utilitaires SDL2 de base, nivel prog1)
   ============================================================ */

/* Afficher du texte avec une police donnee */
void afficher_texte(Jeu *j, TTF_Font *police,
                    const char *texte, int x, int y,
                    SDL_Color couleur)
{
    SDL_Surface *surf;
    SDL_Texture *tex;
    SDL_Rect     rect;

    if (police == NULL || texte == NULL) return;
    surf = TTF_RenderUTF8_Blended(police, texte, couleur);
    if (surf == NULL) return;
    tex = SDL_CreateTextureFromSurface(j->rendu, surf);
    if (tex != NULL) {
        rect.x = x; rect.y = y;
        rect.w = surf->w; rect.h = surf->h;
        SDL_RenderCopy(j->rendu, tex, NULL, &rect);
        SDL_DestroyTexture(tex);
    }
    SDL_FreeSurface(surf);
}

/* Afficher du texte centre horizontalement */
void afficher_centre(Jeu *j, TTF_Font *police,
                     const char *texte, int y,
                     SDL_Color couleur)
{
    SDL_Surface *surf;
    SDL_Texture *tex;
    SDL_Rect     rect;

    if (police == NULL || texte == NULL) return;
    surf = TTF_RenderUTF8_Blended(police, texte, couleur);
    if (surf == NULL) return;
    tex = SDL_CreateTextureFromSurface(j->rendu, surf);
    if (tex != NULL) {
        rect.x = (LARGEUR - surf->w) / 2;
        rect.y = y;
        rect.w = surf->w; rect.h = surf->h;
        SDL_RenderCopy(j->rendu, tex, NULL, &rect);
        SDL_DestroyTexture(tex);
    }
    SDL_FreeSurface(surf);
}

/* Dessiner un bouton simple : fond + bordure + texte */
void dessiner_bouton(Jeu *j, SDL_Rect zone,
                     const char *texte, int survole)
{
    SDL_Color couleur_texte = {255, 255, 255, 255};
    SDL_Surface *surf;
    SDL_Rect     pos_txt;
    SDL_Texture *tex;

    /* Fond du bouton */
    SDL_SetRenderDrawBlendMode(j->rendu, SDL_BLENDMODE_BLEND);
    if (survole) {
        SDL_SetRenderDrawColor(j->rendu, 80, 140, 220, 230);
    } else {
        SDL_SetRenderDrawColor(j->rendu, 30, 30, 50, 200);
    }
    SDL_RenderFillRect(j->rendu, &zone);

    /* Bordure */
    SDL_SetRenderDrawColor(j->rendu, 120, 180, 255, 255);
    SDL_RenderDrawRect(j->rendu, &zone);

    /* Texte centre dans le bouton */
    if (texte != NULL && strlen(texte) > 0 && j->petite != NULL) {
        surf = TTF_RenderUTF8_Blended(j->petite, texte, couleur_texte);
        if (surf != NULL) {
            tex = SDL_CreateTextureFromSurface(j->rendu, surf);
            if (tex != NULL) {
                pos_txt.x = zone.x + (zone.w - surf->w) / 2;
                pos_txt.y = zone.y + (zone.h - surf->h) / 2;
                pos_txt.w = surf->w;
                pos_txt.h = surf->h;
                SDL_RenderCopy(j->rendu, tex, NULL, &pos_txt);
                SDL_DestroyTexture(tex);
            }
            SDL_FreeSurface(surf);
        }
    }
}

/* Verifier si la souris est sur une zone */
int survole(SDL_Rect zone)
{
    int mx, my;
    SDL_Point p;
    SDL_GetMouseState(&mx, &my);
    p.x = mx; p.y = my;
    return SDL_PointInRect(&p, &zone);
}

/* Afficher le fond d'un ecran (index 1..6) */
void afficher_fond(Jeu *j, int index)
{
    SDL_Rect tout = {0, 0, LARGEUR, HAUTEUR};
    if (index >= 1 && index <= 6 && j->fond[index] != NULL)
        SDL_RenderCopy(j->rendu, j->fond[index], NULL, &tout);
}

/* Initialiser une piece du puzzle a sa position initiale */
void init_pieces(Jeu *j)
{
    int imgW, imgH;
    int i, r, c;
    int taille_slot = TAILLE_PIECE + ECART_SLOT;

    /* Tableau d'ordre melange */
    int ordre[NB_PIECES];
    for (i = 0; i < NB_PIECES; i++) ordre[i] = i;
    /* Melange Fisher-Yates */
    for (i = NB_PIECES - 1; i > 0; i--) {
        int j2 = rand() % (i + 1);
        int tmp = ordre[i]; ordre[i] = ordre[j2]; ordre[j2] = tmp;
    }

    SDL_QueryTexture(j->img_puzzle[j->niveau],
                     NULL, NULL, &imgW, &imgH);

    for (r = 0; r < TAILLE_GRILLE; r++) {
        for (c = 0; c < TAILLE_GRILLE; c++) {
            int idx = r * TAILLE_GRILLE + c;

            /* Portion de l'image originale */
            j->pieces[idx].source.x = c * imgW / TAILLE_GRILLE;
            j->pieces[idx].source.y = r * imgH / TAILLE_GRILLE;
            j->pieces[idx].source.w = imgW / TAILLE_GRILLE;
            j->pieces[idx].source.h = imgH / TAILLE_GRILLE;

            /* Position cible dans la grille */
            j->pieces[idx].cible.x = GRILLE_X + c * TAILLE_PIECE;
            j->pieces[idx].cible.y = GRILLE_Y + r * TAILLE_PIECE;
            j->pieces[idx].cible.w = TAILLE_PIECE;
            j->pieces[idx].cible.h = TAILLE_PIECE;

            /* Position initiale : slot melange dans le plateau */
            j->pieces[idx].position.x = PLATEAU_X
                + (ordre[idx] % TAILLE_GRILLE) * taille_slot
                + ECART_SLOT / 2;
            j->pieces[idx].position.y = PLATEAU_Y
                + (ordre[idx] / TAILLE_GRILLE) * taille_slot
                + ECART_SLOT / 2;
            j->pieces[idx].position.w = TAILLE_PIECE;
            j->pieces[idx].position.h = TAILLE_PIECE;

            j->pieces[idx].en_place = 0;
            j->pieces[idx].drag     = 0;
            j->pieces[idx].ox       = 0;
            j->pieces[idx].oy       = 0;
        }
    }
    j->termine = 0;
}


/* ============================================================
   FONCTION 1 : initialiser_jeu
   Ouvre SDL2, cree la fenetre, charge tous les assets
   ============================================================ */
int initialiser_jeu(Jeu *j)
{
    char chemin[256];
    int  i;

    /* Mettre toute la structure a zero */
    memset(j, 0, sizeof(Jeu));

    /* Valeurs de depart */
    j->volume  = 80;
    j->niveau  = 1;
    j->etat    = MENU_PRINCIPAL;
    j->avatar  = 1;

    /* Initialiser SDL2 */
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        printf("Erreur SDL : %s\n", SDL_GetError());
        return 0;
    }

    /* Creer la fenetre */
    j->fenetre = SDL_CreateWindow("PuzzleQuest",
                     SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                     LARGEUR, HAUTEUR, 0);
    if (j->fenetre == NULL) {
        printf("Erreur fenetre : %s\n", SDL_GetError());
        return 0;
    }

    /* Creer le rendu graphique */
    j->rendu = SDL_CreateRenderer(j->fenetre, -1,
                   SDL_RENDERER_ACCELERATED);
    if (j->rendu == NULL) {
        printf("Erreur rendu : %s\n", SDL_GetError());
        return 0;
    }

    /* Charger le module image */
    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);

    /* Charger le module son */
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    Mix_Init(MIX_INIT_MP3);

    /* Charger le module texte */
    TTF_Init();

    /* Charger les polices */
    j->police = TTF_OpenFont("assets/fonts/font.ttf", 28);
    j->petite = TTF_OpenFont("assets/fonts/font.ttf", 19);
    if (j->police == NULL) {
        printf("Erreur police : %s\n", TTF_GetError());
        return 0;
    }

    /* Charger les images de fond menu1.png .. menu6.png */
    for (i = 1; i <= 6; i++) {
        snprintf(chemin, sizeof(chemin),
                 "assets/images/menu%d.png", i);
        j->fond[i] = IMG_LoadTexture(j->rendu, chemin);
    }

    /* Charger les images puzzle img-1.png .. img-4.png */
    for (i = 1; i <= 4; i++) {
        snprintf(chemin, sizeof(chemin),
                 "assets/images/img-%d.png", i);
        j->img_puzzle[i] = IMG_LoadTexture(j->rendu, chemin);
    }

    /* Charger les musiques */
    for (i = 1; i <= 4; i++) {
        snprintf(chemin, sizeof(chemin),
                 "assets/sounds/music%d.mp3", i);
        j->musique[i] = Mix_LoadMUS(chemin);
    }

    /* Charger les effets sonores */
    j->son_clic   = Mix_LoadWAV("assets/sounds/click.wav");
    j->son_survol = Mix_LoadWAV("assets/sounds/hover.wav");

    /* Graine aleatoire */
    srand((unsigned int)time(NULL));

    /* Lancer la musique du menu */
    if (j->musique[1] != NULL) {
        Mix_VolumeMusic(j->volume);
        Mix_PlayMusic(j->musique[1], -1);
    }

    return 1;
}


/* ============================================================
   FONCTION 2 : charger_questions
   Lire le fichier questions.txt ligne par ligne.

   Format d'une question dans le fichier :
     La question
     Reponse A
     Reponse B
     Reponse C
     0            (indice bonne reponse: 0, 1 ou 2)
                  (ligne vide)
   ============================================================ */
int charger_questions(Jeu *j, const char *fichier)
{
    FILE *f;
    int   nb = 0;
    char  ligne[300];

    f = fopen(fichier, "r");
    if (f == NULL) {
        printf("Impossible d'ouvrir %s\n", fichier);
        return 0;
    }

    while (nb < MAX_QUESTIONS) {

        /* Lire la question */
        if (fgets(ligne, sizeof(ligne), f) == NULL) break;
        ligne[strcspn(ligne, "\n")] = '\0';
        if (strlen(ligne) == 0) continue;
        strncpy(j->questions[nb].texte, ligne, 255);

        /* Lire les 3 reponses */
        if (fgets(ligne, sizeof(ligne), f) == NULL) break;
        ligne[strcspn(ligne, "\n")] = '\0';
        strncpy(j->questions[nb].choix[0], ligne, 127);

        if (fgets(ligne, sizeof(ligne), f) == NULL) break;
        ligne[strcspn(ligne, "\n")] = '\0';
        strncpy(j->questions[nb].choix[1], ligne, 127);

        if (fgets(ligne, sizeof(ligne), f) == NULL) break;
        ligne[strcspn(ligne, "\n")] = '\0';
        strncpy(j->questions[nb].choix[2], ligne, 127);

        /* Lire l'indice de la bonne reponse */
        if (fgets(ligne, sizeof(ligne), f) == NULL) break;
        j->questions[nb].bonne = atoi(ligne);

        /* Ligne vide separatrice */
        fgets(ligne, sizeof(ligne), f);

        nb++;
    }

    fclose(f);
    j->nb_questions = nb;
    printf("%d questions chargees.\n", nb);
    return nb;
}


/* ============================================================
   FONCTION 3 : dessiner_jeu
   Afficher l'ecran selon l'etat du jeu
   ============================================================ */
void dessiner_jeu(Jeu *j)
{
    SDL_Color blanc  = {255, 255, 255, 255};
    SDL_Color jaune  = {255, 220,  50, 255};
    SDL_Color vert   = { 50, 220,  50, 255};
    SDL_Color rouge  = {220,  50,  50, 255};
    SDL_Color gris   = {160, 160, 200, 255};
    SDL_Color cyan   = { 80, 200, 220, 255};

    char buf[256];
    int  i, taille_slot;

    /* Vider l'ecran */
    SDL_SetRenderDrawColor(j->rendu, 10, 10, 25, 255);
    SDL_RenderClear(j->rendu);

    /* ====================================================
       AFFICHAGE SELON L'ETAT
       ==================================================== */

    /* ---- MENU PRINCIPAL ---- */
    if (j->etat == MENU_PRINCIPAL) {

        afficher_fond(j, 1);
        afficher_centre(j, j->police, "PuzzleQuest", 80, jaune);
        afficher_centre(j, j->petite,
            "Le jeu de puzzles et enigmes", 118, blanc);

        /* 5 boutons verticaux a gauche */
        SDL_Rect b[5];
        const char *labels[5] = {"Jouer","Options",
                                  "Meilleurs Scores","Histoire","Quitter"};
        for (i = 0; i < 5; i++) {
            b[i].x = 70; b[i].y = 220 + i * 70;
            b[i].w = 220; b[i].h = 55;
            dessiner_bouton(j, b[i], labels[i], survole(b[i]));
        }
    }

    /* ---- OPTIONS ---- */
    else if (j->etat == MENU_OPTIONS) {

        afficher_fond(j, 2);
        afficher_centre(j, j->police, "Options", 40, jaune);

        /* Barre de volume */
        afficher_texte(j, j->petite, "Volume :", 80, 110, gris);
        SDL_Rect fond_barre = {240, 108, 400, 32};
        SDL_SetRenderDrawColor(j->rendu, 20, 20, 40, 210);
        SDL_RenderFillRect(j->rendu, &fond_barre);
        SDL_Rect barre = {240, 108, 400 * j->volume / 128, 32};
        SDL_SetRenderDrawColor(j->rendu, 60, 200, 80, 255);
        SDL_RenderFillRect(j->rendu, &barre);
        SDL_SetRenderDrawColor(j->rendu, 100, 200, 120, 255);
        SDL_RenderDrawRect(j->rendu, &fond_barre);
        snprintf(buf, sizeof(buf), "%d%%", 100 * j->volume / 128);
        afficher_texte(j, j->petite, buf, 655, 110, blanc);

        /* Boutons options */
        int cx = LARGEUR / 2;
        SDL_Rect bvup = {cx-210,180,190,50};
        SDL_Rect bvdn = {cx+ 20,180,190,50};
        SDL_Rect bnor = {cx-210,380,190,50};
        SDL_Rect bple = {cx+ 20,380,190,50};
        SDL_Rect bret = {LARGEUR-200,HAUTEUR-80,160,50};

        afficher_texte(j, j->petite, "Mode d'affichage :", 80, 345, gris);
        dessiner_bouton(j, bvup, "Augmenter (+)", survole(bvup));
        dessiner_bouton(j, bvdn, "Diminuer (-)",  survole(bvdn));
        dessiner_bouton(j, bnor, "Mode normal",   survole(bnor));
        dessiner_bouton(j, bple, "Plein ecran",   survole(bple));
        dessiner_bouton(j, bret, "Retour",         survole(bret));
    }

    /* ---- SAUVEGARDE ---- */
    else if (j->etat == MENU_SAUVEGARDE) {

        afficher_fond(j, 3);

        if (j->phase == 0) {
            afficher_centre(j, j->police, "Sauvegarde", 80, jaune);
            SDL_Rect panneau = {LARGEUR/2-280, 200, 560, 280};
            SDL_SetRenderDrawBlendMode(j->rendu, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(j->rendu, 0, 0, 0, 160);
            SDL_RenderFillRect(j->rendu, &panneau);
            afficher_centre(j, j->police,
                "Voulez-vous sauvegarder votre jeu ?", 270, blanc);

            SDL_Rect boui = {LARGEUR/2-130, 420, 110, 55};
            SDL_Rect bnon = {LARGEUR/2+  20, 420, 110, 55};
            dessiner_bouton(j, boui, "OUI", survole(boui));
            dessiner_bouton(j, bnon, "NON", survole(bnon));
        } else {
            afficher_centre(j, j->police, "Chargement", 80, jaune);
            SDL_Rect bcha = {LARGEUR/2-130, 350, 260, 55};
            SDL_Rect bnew = {LARGEUR/2-130, 430, 260, 55};
            SDL_Rect bret = {30, HAUTEUR-80, 160, 50};
            dessiner_bouton(j, bcha, "Charger le jeu",  survole(bcha));
            dessiner_bouton(j, bnew, "Nouvelle partie", survole(bnew));
            dessiner_bouton(j, bret, "Retour",           survole(bret));
        }
    }

    /* ---- MENU JOUEUR ---- */
    else if (j->etat == MENU_JOUEUR) {

        afficher_fond(j, 4);
        afficher_centre(j, j->police, "Choix du Joueur", 30, jaune);

        if (j->phase == 0) {
            /* Phase 0 : solo ou multi */
            afficher_centre(j, j->police, "Mode de jeu :", 200, blanc);
            SDL_Rect bsolo  = {LARGEUR/2-170, 350, 150, 55};
            SDL_Rect bmulti = {LARGEUR/2+  20, 350, 150, 55};
            SDL_Rect bret   = {30, HAUTEUR-75, 150, 48};
            dessiner_bouton(j, bsolo,  "Solo",   survole(bsolo));
            dessiner_bouton(j, bmulti, "Multi",  survole(bmulti));
            dessiner_bouton(j, bret,   "Retour", survole(bret));
        } else {
            /* Phase 1 : choisir avatar */
            snprintf(buf, sizeof(buf), "Mode : %s",
                     j->multijoueur ? "Multi" : "Solo");
            afficher_centre(j, j->petite, buf, 78, gris);
            afficher_centre(j, j->petite,
                "Choisissez votre avatar :", 110, blanc);

            /* Avatar 1 : Guerrier */
            int av1x = LARGEUR/2 - 200;
            int av2x = LARGEUR/2 + 50;
            int avy  = 150, avs = 150;

            SDL_Rect cad1 = {av1x, avy, avs, avs};
            SDL_SetRenderDrawBlendMode(j->rendu, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(j->rendu, 20, 20, 50, 200);
            SDL_RenderFillRect(j->rendu, &cad1);
            if (j->avatar == 1)
                SDL_SetRenderDrawColor(j->rendu, 50, 220, 50, 255);
            else
                SDL_SetRenderDrawColor(j->rendu, 100, 100, 180, 255);
            SDL_RenderDrawRect(j->rendu, &cad1);

            /* Corps guerrier */
            SDL_Rect tete1  = {av1x+50, avy+15,  50, 50};
            SDL_Rect corps1 = {av1x+35, avy+70,  80, 60};
            SDL_Rect jamb1  = {av1x+35, avy+130, 80, 20};
            SDL_SetRenderDrawColor(j->rendu, 210,170,120,255);
            SDL_RenderFillRect(j->rendu, &tete1);
            SDL_SetRenderDrawColor(j->rendu, 180, 40, 40,255);
            SDL_RenderFillRect(j->rendu, &corps1);
            SDL_SetRenderDrawColor(j->rendu, 60, 30, 30,255);
            SDL_RenderFillRect(j->rendu, &jamb1);
            SDL_SetRenderDrawColor(j->rendu, 200,200,220,255);
            SDL_RenderDrawLine(j->rendu, av1x+120,avy+60, av1x+130,avy+115);

            afficher_texte(j, j->petite, "Guerrier",
                av1x+avs/2-38, avy+avs+5,
                (j->avatar==1) ? vert : blanc);

            /* Avatar 2 : Mage */
            SDL_Rect cad2 = {av2x, avy, avs, avs};
            SDL_SetRenderDrawColor(j->rendu, 20, 20, 50, 200);
            SDL_RenderFillRect(j->rendu, &cad2);
            if (j->avatar == 2)
                SDL_SetRenderDrawColor(j->rendu, 50, 220, 50, 255);
            else
                SDL_SetRenderDrawColor(j->rendu, 100, 100, 180, 255);
            SDL_RenderDrawRect(j->rendu, &cad2);

            SDL_Rect tete2  = {av2x+50, avy+15, 50, 50};
            SDL_Rect capuch = {av2x+44, avy+5,  62, 30};
            SDL_Rect corps2 = {av2x+35, avy+70, 80, 60};
            SDL_Rect orbe   = {av2x+117,avy+10, 16, 16};
            SDL_SetRenderDrawColor(j->rendu, 210,170,120,255);
            SDL_RenderFillRect(j->rendu, &tete2);
            SDL_SetRenderDrawColor(j->rendu, 20, 20, 80, 220);
            SDL_RenderFillRect(j->rendu, &capuch);
            SDL_SetRenderDrawColor(j->rendu, 40, 40,180,255);
            SDL_RenderFillRect(j->rendu, &corps2);
            SDL_SetRenderDrawColor(j->rendu, 100, 60, 20,255);
            SDL_RenderDrawLine(j->rendu, av2x+125,avy+20, av2x+125,avy+115);
            SDL_SetRenderDrawColor(j->rendu, 160, 60,220,255);
            SDL_RenderFillRect(j->rendu, &orbe);

            afficher_texte(j, j->petite, "Mage",
                av2x+avs/2-22, avy+avs+5,
                (j->avatar==2) ? vert : blanc);

            snprintf(buf, sizeof(buf), "Selectionne : %s",
                     j->avatar==1 ? "Guerrier" : "Mage");
            afficher_centre(j, j->petite, buf, avy+avs+50, vert);

            SDL_Rect bval = {LARGEUR/2-70, avy+avs+90, 140, 48};
            dessiner_bouton(j, bval, "Valider", survole(bval));
            afficher_centre(j, j->petite,
                "(Appuyez Entree pour valider)", 620, blanc);
        }
    }

    /* ---- SCORES ---- */
    else if (j->etat == MENU_SCORES) {

        afficher_fond(j, 5);
        afficher_centre(j, j->police, "Meilleurs Scores", 40, jaune);

        if (j->phase == 0) {
            /* Saisie du nom */
            afficher_centre(j, j->police, "Entrez votre nom :", 220, blanc);
            SDL_Rect boite = {LARGEUR/2-180, 280, 360, 55};
            SDL_SetRenderDrawColor(j->rendu, 20, 20, 50, 200);
            SDL_RenderFillRect(j->rendu, &boite);
            SDL_SetRenderDrawColor(j->rendu, 100, 160, 255, 255);
            SDL_RenderDrawRect(j->rendu, &boite);
            snprintf(buf, sizeof(buf), "%s_", j->nom_joueur);
            afficher_texte(j, j->police, buf,
                           boite.x+12, boite.y+12, blanc);
            SDL_Rect bval = {LARGEUR/2-80, 460, 160, 55};
            SDL_Rect bret = {30, HAUTEUR-80, 160, 50};
            dessiner_bouton(j, bval, "Valider", survole(bval));
            dessiner_bouton(j, bret, "Retour",  survole(bret));
        } else {
            /* Afficher top 3 */
            Score tous[MAX_SCORES];
            int   nb = 0;
            Score tmp_s;
            int   x, y;

            FILE *f = fopen("scores.txt", "r");
            if (f != NULL) {
                while (nb < MAX_SCORES &&
                       fscanf(f, "%63s %d",
                              tous[nb].nom, &tous[nb].pts) == 2)
                    nb++;
                fclose(f);
            }
            /* Tri a bulles decroissant */
            for (x = 0; x < nb-1; x++)
                for (y = x+1; y < nb; y++)
                    if (tous[y].pts > tous[x].pts) {
                        tmp_s   = tous[x];
                        tous[x] = tous[y];
                        tous[y] = tmp_s;
                    }

            {
                const char *med[3] = {"1er :  ","2eme : ","3eme : "};
                SDL_Color  col[3]  = {jaune, blanc, gris};
                int        nb3     = (nb < 3) ? nb : 3;
                for (i = 0; i < nb3; i++) {
                    snprintf(buf, sizeof(buf), "%s%s  -  %d pts",
                             med[i], tous[i].nom, tous[i].pts);
                    afficher_centre(j, j->police, buf,
                                    220 + i*90, col[i]);
                }
                if (nb3 == 0)
                    afficher_centre(j, j->police,
                        "Aucun score enregistre.", 280, blanc);
            }

            SDL_Rect bret = {30, HAUTEUR-80, 160, 50};
            SDL_Rect bqui = {LARGEUR-200, HAUTEUR-80, 160, 50};
            dessiner_bouton(j, bret, "Retour",  survole(bret));
            dessiner_bouton(j, bqui, "Quitter", survole(bqui));
            afficher_centre(j, j->petite,
                "(appuyer 'e' pour les enigmes)", 530, gris);
        }
    }

    /* ---- ENIGME ---- */
    else if (j->etat == MENU_ENIGME) {

        afficher_fond(j, 6);
        afficher_centre(j, j->police, "Enigme", 80, jaune);
        afficher_centre(j, j->police,
            "Choisissez votre mode", 140, blanc);

        int cx = LARGEUR / 2;
        SDL_Rect bqui = {cx-160, 380, 140, 60};
        SDL_Rect bpuz = {cx+ 20, 380, 140, 60};
        SDL_Rect bret = {30, HAUTEUR-75, 150, 48};
        dessiner_bouton(j, bqui, "Quiz",   survole(bqui));
        dessiner_bouton(j, bpuz, "Puzzle", survole(bpuz));
        dessiner_bouton(j, bret, "Retour", survole(bret));
    }

    /* ---- QUIZ ---- */
    else if (j->etat == MODE_QUIZ) {

        afficher_fond(j, 6);
        afficher_centre(j, j->police,
            "QUIZ - Les 300 de Sparte", 25, jaune);

        if (j->q_actuelle >= NB_Q_SESSION) {
            /* Fin du quiz */
            SDL_Rect pan = {LARGEUR/2-280, 200, 560, 300};
            SDL_SetRenderDrawColor(j->rendu, 0, 0, 0, 200);
            SDL_RenderFillRect(j->rendu, &pan);
            SDL_SetRenderDrawColor(j->rendu, 50, 220, 50, 200);
            SDL_RenderDrawRect(j->rendu, &pan);
            afficher_centre(j, j->police, "Quiz termine !", 240, vert);
            snprintf(buf, sizeof(buf), "Score : %d pts", j->score);
            afficher_centre(j, j->police, buf, 300, jaune);
            afficher_centre(j, j->petite, "Molon Labe !", 360, blanc);
            SDL_Rect bmenu = {30, HAUTEUR-75, 150, 48};
            dessiner_bouton(j, bmenu, "Menu", survole(bmenu));
        } else {
            Question *q = &j->questions[j->session[j->q_actuelle]];

            snprintf(buf, sizeof(buf), "Question %d / %d",
                     j->q_actuelle+1, NB_Q_SESSION);
            afficher_centre(j, j->petite, buf, 75, blanc);
            snprintf(buf, sizeof(buf), "Score: %d", j->score);
            afficher_texte(j, j->petite, buf, LARGEUR-180, 75, jaune);

            /* Bandeau question */
            SDL_Rect bande = {40, 120, LARGEUR-80, 110};
            SDL_SetRenderDrawColor(j->rendu, 10, 10, 40, 200);
            SDL_RenderFillRect(j->rendu, &bande);
            SDL_SetRenderDrawColor(j->rendu, 80, 140, 255, 255);
            SDL_RenderDrawRect(j->rendu, &bande);

            /* Afficher la question (coupe a 58 chars) */
            if ((int)strlen(q->texte) <= 58) {
                afficher_texte(j, j->police, q->texte,
                               bande.x+14, bande.y+35, blanc);
            } else {
                char l1[60], l2[80];
                strncpy(l1, q->texte, 58); l1[58] = '\0';
                strncpy(l2, q->texte+58, 79); l2[79] = '\0';
                afficher_texte(j, j->petite, l1,
                               bande.x+14, bande.y+18, blanc);
                afficher_texte(j, j->petite, l2,
                               bande.x+14, bande.y+52, blanc);
            }

            /* Citation spartiate */
            SDL_Rect deco = {40, 250, LARGEUR-80, 38};
            SDL_SetRenderDrawColor(j->rendu, 10, 10, 30, 160);
            SDL_RenderFillRect(j->rendu, &deco);
            afficher_centre(j, j->petite,
                "\"Molon Labe\" - Leonidas, Roi de Sparte", 257, cyan);

            /* 3 boutons reponse */
            SDL_Rect br[3];
            br[0].x = 55;            br[0].y = 490;
            br[0].w = 240;           br[0].h = 55;
            br[1].x = LARGEUR/2-120; br[1].y = 490;
            br[1].w = 240;           br[1].h = 55;
            br[2].x = LARGEUR-295;   br[2].y = 490;
            br[2].w = 240;           br[2].h = 55;

            for (i = 0; i < 3; i++)
                dessiner_bouton(j, br[i], q->choix[i], survole(br[i]));

            SDL_Rect bqui = {30, HAUTEUR-75, 150, 48};
            dessiner_bouton(j, bqui, "Quitter", survole(bqui));

            /* Feedback */
            if (j->repondu == 1)
                afficher_centre(j, j->police,
                    "Bonne reponse ! +10 pts", 440, vert);
            if (j->repondu == 2)
                afficher_centre(j, j->police,
                    "Mauvaise reponse...", 440, rouge);
        }
    }

    /* ---- PUZZLE ---- */
    else if (j->etat == MODE_PUZZLE) {

        SDL_SetRenderDrawColor(j->rendu, 15, 15, 35, 255);
        SDL_RenderClear(j->rendu);

        taille_slot = TAILLE_PIECE + ECART_SLOT;

        /* Titre et score */
        snprintf(buf, sizeof(buf),
                 "Puzzle - Niveau %d / 4", j->niveau);
        afficher_texte(j, j->police, buf,
                       LARGEUR/2-130, 25, jaune);
        snprintf(buf, sizeof(buf), "Score: %d", j->score);
        afficher_texte(j, j->police, buf, LARGEUR-170, 25, blanc);

        /* Bouton Suivant (haut gauche, visible si puzzle fini) */
        if (j->termine) {
            SDL_Rect bsuiv = {GRILLE_X, 20, 155, 38};
            dessiner_bouton(j, bsuiv, ">> Suivant", survole(bsuiv));
        }

        /* Grille gauche vide */
        afficher_texte(j, j->petite, "Zone de placement",
                       GRILLE_X, GRILLE_Y-22, gris);
        {
            int r, c;
            for (r = 0; r < TAILLE_GRILLE; r++) {
                for (c = 0; c < TAILLE_GRILLE; c++) {
                    SDL_Rect cel = {
                        GRILLE_X + c*TAILLE_PIECE,
                        GRILLE_Y + r*TAILLE_PIECE,
                        TAILLE_PIECE, TAILLE_PIECE
                    };
                    SDL_SetRenderDrawBlendMode(j->rendu,SDL_BLENDMODE_BLEND);
                    SDL_SetRenderDrawColor(j->rendu, 25,25,60,220);
                    SDL_RenderFillRect(j->rendu, &cel);
                    SDL_SetRenderDrawColor(j->rendu, 85,85,155,255);
                    SDL_RenderDrawRect(j->rendu, &cel);
                }
            }
        }

        /* Plateau droit */
        SDL_Rect fond_plateau = {
            PLATEAU_X-6,   PLATEAU_Y-28,
            TAILLE_GRILLE*taille_slot+12,
            TAILLE_GRILLE*taille_slot+34
        };
        SDL_SetRenderDrawColor(j->rendu, 20,20,50,210);
        SDL_RenderFillRect(j->rendu, &fond_plateau);
        SDL_SetRenderDrawColor(j->rendu, 90,90,170,255);
        SDL_RenderDrawRect(j->rendu, &fond_plateau);
        afficher_texte(j, j->petite, "Pieces",
                       PLATEAU_X+60, PLATEAU_Y-24, gris);

        /* Slots vides du plateau */
        for (i = 0; i < NB_PIECES; i++) {
            SDL_Rect slot = {
                PLATEAU_X+(i%TAILLE_GRILLE)*taille_slot+ECART_SLOT/2,
                PLATEAU_Y+(i/TAILLE_GRILLE)*taille_slot+ECART_SLOT/2,
                TAILLE_PIECE, TAILLE_PIECE
            };
            SDL_SetRenderDrawColor(j->rendu, 30,30,65,180);
            SDL_RenderFillRect(j->rendu, &slot);
            SDL_SetRenderDrawColor(j->rendu, 55,55,105,200);
            SDL_RenderDrawRect(j->rendu, &slot);
        }

        /* Dessiner les pieces (sauf celle en drag) */
        if (j->img_puzzle[j->niveau] != NULL) {
            for (i = 0; i < NB_PIECES; i++) {
                if (j->pieces[i].drag) continue;
                SDL_Rect dst = {
                    j->pieces[i].position.x,
                    j->pieces[i].position.y,
                    TAILLE_PIECE, TAILLE_PIECE
                };
                SDL_RenderCopy(j->rendu, j->img_puzzle[j->niveau],
                               &j->pieces[i].source, &dst);
                if (j->pieces[i].en_place)
                    SDL_SetRenderDrawColor(j->rendu, 50,220,50,255);
                else
                    SDL_SetRenderDrawColor(j->rendu,170,170,210,180);
                SDL_RenderDrawRect(j->rendu, &dst);
            }
            /* Piece en drag par-dessus */
            for (i = 0; i < NB_PIECES; i++) {
                if (!j->pieces[i].drag) continue;
                SDL_Rect dst = {
                    j->pieces[i].position.x,
                    j->pieces[i].position.y,
                    TAILLE_PIECE, TAILLE_PIECE
                };
                SDL_RenderCopy(j->rendu, j->img_puzzle[j->niveau],
                               &j->pieces[i].source, &dst);
                SDL_SetRenderDrawColor(j->rendu,255,200,0,255);
                SDL_RenderDrawRect(j->rendu, &dst);
            }
        }

        /* Boutons Reset et Menu */
        {
            int bx = PLATEAU_X + TAILLE_GRILLE*taille_slot/2 - 70;
            int by = PLATEAU_Y + TAILLE_GRILLE*taille_slot + 15;
            SDL_Rect breset = {bx,      by,      140, 45};
            SDL_Rect bmenu  = {bx, by + 55, 140, 45};
            dessiner_bouton(j, breset, "Reset", survole(breset));
            dessiner_bouton(j, bmenu,  "Menu",  survole(bmenu));
        }

        afficher_texte(j, j->petite,
            "Glissez les pieces dans la grille  |  R = Reset",
            GRILLE_X, GRILLE_Y+480+12, blanc);

        /* Banniere si termine */
        if (j->termine) {
            SDL_Rect ban = {GRILLE_X, HAUTEUR/2-50, 480, 100};
            SDL_SetRenderDrawColor(j->rendu, 0,0,0,220);
            SDL_RenderFillRect(j->rendu, &ban);
            SDL_SetRenderDrawColor(j->rendu, 50,220,50,255);
            SDL_RenderDrawRect(j->rendu, &ban);
            afficher_centre(j, j->police,
                "Puzzle Complete ! +20 pts", HAUTEUR/2-24, vert);
            afficher_centre(j, j->petite,
                "Cliquez '>> Suivant' en haut a gauche",
                HAUTEUR/2+22, blanc);
        }
    }

    /* Afficher le rendu a l'ecran */
    SDL_RenderPresent(j->rendu);
}


/* ============================================================
   FONCTION 4 : gerer_evenements
   Lire et traiter tous les evenements SDL2
   ============================================================ */
void gerer_evenements(Jeu *j)
{
    SDL_Event ev;
    int mx, my, i, k;
    SDL_Point clic;
    int taille_slot;

    /* Auto-avance du quiz apres 1.8 secondes */
    if (j->etat == MODE_QUIZ && j->repondu &&
        SDL_GetTicks() - j->t_reponse > 1800) {
        j->q_actuelle++;
        j->repondu = 0;
    }

    /* Lire tous les evenements en attente */
    while (SDL_PollEvent(&ev)) {

        /* Fermeture de la fenetre */
        if (ev.type == SDL_QUIT) {
            j->etat = QUITTER;
            return;
        }

        /* ======== CLAVIER ======== */
        if (ev.type == SDL_KEYDOWN) {
            SDL_Keycode t = ev.key.keysym.sym;

            /* Echap : retour au menu ou quitter */
            if (t == SDLK_ESCAPE) {
                if (j->etat == MENU_PRINCIPAL) j->etat = QUITTER;
                else j->etat = MENU_PRINCIPAL;
                return;
            }
            /* Menu principal */
            if (j->etat == MENU_PRINCIPAL) {
                if (t == SDLK_j) { j->etat = MENU_SAUVEGARDE; j->phase = 0; }
                if (t == SDLK_o) j->etat = MENU_OPTIONS;
                if (t == SDLK_m) { j->etat = MENU_SCORES; j->phase = 0; }
            }
            /* Options : volume */
            if (j->etat == MENU_OPTIONS) {
                if (t == SDLK_PLUS  || t == SDLK_KP_PLUS) {
                    j->volume = (j->volume+16>128)?128:j->volume+16;
                    Mix_VolumeMusic(j->volume);
                }
                if (t == SDLK_MINUS || t == SDLK_KP_MINUS) {
                    j->volume = (j->volume-16<0)?0:j->volume-16;
                    Mix_VolumeMusic(j->volume);
                }
            }
            /* Sauvegarde N = nouvelle partie */
            if (j->etat == MENU_SAUVEGARDE && t == SDLK_n) {
                j->etat = MENU_JOUEUR; j->phase = 0;
            }
            /* Joueur Entree = valider */
            if (j->etat == MENU_JOUEUR && j->phase == 1 &&
                (t == SDLK_RETURN || t == SDLK_KP_ENTER)) {
                j->etat = MENU_SCORES; j->phase = 0;
            }
            /* Scores e = enigmes */
            if (j->etat == MENU_SCORES && j->phase == 1 && t == SDLK_e)
                j->etat = MENU_ENIGME;
            /* Scores Entree = valider nom */
            if (j->etat == MENU_SCORES && j->phase == 0 &&
                (t == SDLK_RETURN || t == SDLK_KP_ENTER) &&
                j->len_nom > 0) {
                sauvegarder_score(j->nom_joueur, j->score);
                j->phase = 1;
                if (j->musique[3]) {
                    Mix_HaltMusic();
                    Mix_PlayMusic(j->musique[3], -1);
                }
            }
            /* Backspace : effacer un caractere du nom */
            if (j->etat == MENU_SCORES && j->phase == 0 &&
                t == SDLK_BACKSPACE && j->len_nom > 0) {
                j->len_nom--;
                j->nom_joueur[j->len_nom] = '\0';
            }
            /* Puzzle R = reset */
            if (j->etat == MODE_PUZZLE && t == SDLK_r)
                init_pieces(j);
        }

        /* ======== SAISIE TEXTE (nom du joueur) ======== */
        if (ev.type == SDL_TEXTINPUT &&
            j->etat == MENU_SCORES && j->phase == 0 &&
            j->len_nom < 63) {
            int len = (int)strlen(ev.text.text);
            if (j->len_nom + len < 63) {
                strcat(j->nom_joueur, ev.text.text);
                j->len_nom += len;
            }
        }

        /* ======== CLIC SOURIS ======== */
        if (ev.type == SDL_MOUSEBUTTONDOWN &&
            ev.button.button == SDL_BUTTON_LEFT) {

            mx = ev.button.x;
            my = ev.button.y;
            clic.x = mx; clic.y = my;

            if (j->son_clic) Mix_PlayChannel(-1, j->son_clic, 0);

            /* --- Menu principal --- */
            if (j->etat == MENU_PRINCIPAL) {
                SDL_Rect b[5];
                for (i = 0; i < 5; i++) {
                    b[i].x=70; b[i].y=220+i*70; b[i].w=220; b[i].h=55;
                }
                if      (SDL_PointInRect(&clic,&b[0])){
                    j->etat=MENU_SAUVEGARDE; j->phase=0;
                }
                else if (SDL_PointInRect(&clic,&b[1])){
                    j->etat=MENU_OPTIONS;
                    if(j->musique[2]){Mix_HaltMusic();Mix_PlayMusic(j->musique[2],-1);}
                }
                else if (SDL_PointInRect(&clic,&b[2])){
                    j->etat=MENU_SCORES; j->phase=0;
                }
                else if (SDL_PointInRect(&clic,&b[4])) j->etat=QUITTER;
            }

            /* --- Options --- */
            else if (j->etat == MENU_OPTIONS) {
                int cx = LARGEUR/2;
                SDL_Rect bvu={cx-210,180,190,50}, bvd={cx+20,180,190,50};
                SDL_Rect bno={cx-210,380,190,50}, bpl={cx+20,380,190,50};
                SDL_Rect brt={LARGEUR-200,HAUTEUR-80,160,50};
                if      (SDL_PointInRect(&clic,&bvu)){
                    j->volume=(j->volume+16>128)?128:j->volume+16;
                    Mix_VolumeMusic(j->volume);
                }
                else if (SDL_PointInRect(&clic,&bvd)){
                    j->volume=(j->volume-16<0)?0:j->volume-16;
                    Mix_VolumeMusic(j->volume);
                }
                else if (SDL_PointInRect(&clic,&bpl)){
                    j->plein_ecran=1;
                    SDL_SetWindowFullscreen(j->fenetre,SDL_WINDOW_FULLSCREEN_DESKTOP);
                }
                else if (SDL_PointInRect(&clic,&bno)){
                    j->plein_ecran=0;
                    SDL_SetWindowFullscreen(j->fenetre,0);
                }
                else if (SDL_PointInRect(&clic,&brt)){
                    j->etat=MENU_PRINCIPAL;
                    if(j->musique[1]){Mix_HaltMusic();Mix_PlayMusic(j->musique[1],-1);}
                }
            }

            /* --- Sauvegarde --- */
            else if (j->etat == MENU_SAUVEGARDE) {
                if (j->phase == 0) {
                    SDL_Rect boui={LARGEUR/2-130,420,110,55};
                    SDL_Rect bnon={LARGEUR/2+ 20,420,110,55};
                    if      (SDL_PointInRect(&clic,&boui)) j->phase=1;
                    else if (SDL_PointInRect(&clic,&bnon)) j->etat=MENU_PRINCIPAL;
                } else {
                    SDL_Rect bch={LARGEUR/2-130,350,260,55};
                    SDL_Rect bnw={LARGEUR/2-130,430,260,55};
                    SDL_Rect brt={30,HAUTEUR-80,160,50};
                    if      (SDL_PointInRect(&clic,&bnw)){j->etat=MENU_JOUEUR;j->phase=0;}
                    else if (SDL_PointInRect(&clic,&bch)) j->etat=MENU_ENIGME;
                    else if (SDL_PointInRect(&clic,&brt)) j->etat=MENU_PRINCIPAL;
                }
            }

            /* --- Joueur --- */
            else if (j->etat == MENU_JOUEUR) {
                if (j->phase == 0) {
                    SDL_Rect bs={LARGEUR/2-170,350,150,55};
                    SDL_Rect bm={LARGEUR/2+ 20,350,150,55};
                    SDL_Rect br={30,HAUTEUR-75,150,48};
                    if      (SDL_PointInRect(&clic,&bs)){j->multijoueur=0;j->phase=1;j->avatar=1;}
                    else if (SDL_PointInRect(&clic,&bm)){j->multijoueur=1;j->phase=1;j->avatar=1;}
                    else if (SDL_PointInRect(&clic,&br)) j->etat=MENU_PRINCIPAL;
                } else {
                    int avs=150;
                    SDL_Rect ba1={LARGEUR/2-200,150,avs,avs};
                    SDL_Rect ba2={LARGEUR/2+ 50,150,avs,avs};
                    SDL_Rect bvl={LARGEUR/2- 70,390,140,48};
                    if      (SDL_PointInRect(&clic,&ba1)) j->avatar=1;
                    else if (SDL_PointInRect(&clic,&ba2)) j->avatar=2;
                    else if (SDL_PointInRect(&clic,&bvl)){j->etat=MENU_SCORES;j->phase=0;}
                }
            }

            /* --- Scores --- */
            else if (j->etat == MENU_SCORES) {
                if (j->phase == 0) {
                    SDL_Rect bv={LARGEUR/2-80,460,160,55};
                    SDL_Rect br={30,HAUTEUR-80,160,50};
                    if (SDL_PointInRect(&clic,&bv) && j->len_nom>0) {
                        sauvegarder_score(j->nom_joueur, j->score);
                        j->phase=1;
                        if(j->musique[3]){Mix_HaltMusic();Mix_PlayMusic(j->musique[3],-1);}
                    }
                    else if (SDL_PointInRect(&clic,&br)) j->etat=MENU_PRINCIPAL;
                } else {
                    SDL_Rect br={30,HAUTEUR-80,160,50};
                    SDL_Rect bq={LARGEUR-200,HAUTEUR-80,160,50};
                    if      (SDL_PointInRect(&clic,&br)) j->etat=MENU_PRINCIPAL;
                    else if (SDL_PointInRect(&clic,&bq)) j->etat=QUITTER;
                }
            }

            /* --- Enigme --- */
            else if (j->etat == MENU_ENIGME) {
                int cx=LARGEUR/2;
                SDL_Rect bqz={cx-160,380,140,60};
                SDL_Rect bpz={cx+ 20,380,140,60};
                SDL_Rect brt={30,HAUTEUR-75,150,48};
                if (SDL_PointInRect(&clic,&bqz)) {
                    /* Tirer 5 questions aleatoires */
                    int pool[MAX_QUESTIONS];
                    int n = j->nb_questions;
                    for (i=0;i<n;i++) pool[i]=i;
                    for (i=0;i<NB_Q_SESSION && i<n;i++){
                        int r2=i+rand()%(n-i);
                        int t=pool[i];pool[i]=pool[r2];pool[r2]=t;
                        j->session[i]=pool[i];
                    }
                    j->q_actuelle=0; j->repondu=0;
                    j->etat=MODE_QUIZ;
                    if(j->musique[4]){Mix_HaltMusic();Mix_PlayMusic(j->musique[4],-1);}
                }
                else if (SDL_PointInRect(&clic,&bpz)){
                    init_pieces(j);
                    j->etat=MODE_PUZZLE;
                }
                else if (SDL_PointInRect(&clic,&brt)) j->etat=MENU_PRINCIPAL;
            }

            /* --- Quiz : clic sur une reponse --- */
            else if (j->etat == MODE_QUIZ && !j->repondu &&
                     j->q_actuelle < NB_Q_SESSION) {
                SDL_Rect br[3];
                SDL_Rect bqt={30,HAUTEUR-75,150,48};
                br[0].x=55;            br[0].y=490; br[0].w=240; br[0].h=55;
                br[1].x=LARGEUR/2-120; br[1].y=490; br[1].w=240; br[1].h=55;
                br[2].x=LARGEUR-295;   br[2].y=490; br[2].w=240; br[2].h=55;
                if (SDL_PointInRect(&clic,&bqt)) {
                    j->etat=MENU_PRINCIPAL;
                } else {
                    Question *q=&j->questions[j->session[j->q_actuelle]];
                    for (i=0;i<3;i++) {
                        if (SDL_PointInRect(&clic,&br[i])) {
                            j->repondu = (i==q->bonne) ? 1 : 2;
                            if (j->repondu==1) j->score+=10;
                            j->t_reponse=SDL_GetTicks();
                        }
                    }
                }
            }

            /* --- Puzzle : boutons et debut drag --- */
            else if (j->etat == MODE_PUZZLE) {
                taille_slot = TAILLE_PIECE + ECART_SLOT;
                int bx = PLATEAU_X+TAILLE_GRILLE*taille_slot/2-70;
                int by = PLATEAU_Y+TAILLE_GRILLE*taille_slot+15;
                SDL_Rect bsuiv = {GRILLE_X, 20, 155, 38};
                SDL_Rect breset= {bx,      by,      140, 45};
                SDL_Rect bmenu = {bx, by+55, 140, 45};

                if (j->termine && SDL_PointInRect(&clic,&bsuiv)) {
                    j->niveau++;
                    if (j->niveau > 4) {
                        j->etat=MENU_SCORES; j->phase=0;
                    } else {
                        init_pieces(j);
                    }
                } else if (SDL_PointInRect(&clic,&breset)) {
                    init_pieces(j);
                } else if (SDL_PointInRect(&clic,&bmenu)) {
                    j->etat=MENU_PRINCIPAL;
                } else {
                    /* Debut drag */
                    for (i=NB_PIECES-1;i>=0;i--) {
                        if (j->pieces[i].en_place) continue;
                        SDL_Rect h={j->pieces[i].position.x,
                                    j->pieces[i].position.y,
                                    TAILLE_PIECE,TAILLE_PIECE};
                        if (SDL_PointInRect(&clic,&h)) {
                            j->pieces[i].drag=1;
                            j->pieces[i].ox=mx-j->pieces[i].position.x;
                            j->pieces[i].oy=my-j->pieces[i].position.y;
                            break;
                        }
                    }
                }
            }
        } /* fin SDL_MOUSEBUTTONDOWN */

        /* ======== DEPLACEMENT (drag) ======== */
        if (ev.type == SDL_MOUSEMOTION && j->etat == MODE_PUZZLE) {
            mx=ev.motion.x; my=ev.motion.y;
            for (i=0;i<NB_PIECES;i++) {
                if (j->pieces[i].drag) {
                    j->pieces[i].position.x = mx - j->pieces[i].ox;
                    j->pieces[i].position.y = my - j->pieces[i].oy;
                }
            }
        }

        /* ======== RELACHER (drop) ======== */
        if (ev.type == SDL_MOUSEBUTTONUP &&
            ev.button.button == SDL_BUTTON_LEFT &&
            j->etat == MODE_PUZZLE) {

            mx=ev.button.x; my=ev.button.y;
            taille_slot = TAILLE_PIECE + ECART_SLOT;

            for (i=0;i<NB_PIECES;i++) {
                int px, py, meilleur_d, ml, mc, r2, c2, occupant;

                if (!j->pieces[i].drag) continue;
                j->pieces[i].drag=0;

                /* Centre de la piece */
                px = j->pieces[i].position.x + TAILLE_PIECE/2;
                py = j->pieces[i].position.y + TAILLE_PIECE/2;

                /* Cellule la plus proche */
                meilleur_d = 999999; ml=-1; mc=-1;
                for (r2=0;r2<TAILLE_GRILLE;r2++) {
                    for (c2=0;c2<TAILLE_GRILLE;c2++) {
                        int gx=GRILLE_X+c2*TAILLE_PIECE+TAILLE_PIECE/2;
                        int gy=GRILLE_Y+r2*TAILLE_PIECE+TAILLE_PIECE/2;
                        int d=abs(px-gx)+abs(py-gy);
                        if (d<meilleur_d){meilleur_d=d;ml=r2;mc=c2;}
                    }
                }

                if (meilleur_d < TAILLE_PIECE && ml >= 0) {
                    SDL_Rect cel={GRILLE_X+mc*TAILLE_PIECE,
                                  GRILLE_Y+ml*TAILLE_PIECE,
                                  TAILLE_PIECE,TAILLE_PIECE};
                    /* Chercher occupant */
                    occupant=-1;
                    for (k=0;k<NB_PIECES;k++){
                        if(k==i||j->pieces[k].en_place) continue;
                        if(j->pieces[k].position.x==cel.x &&
                           j->pieces[k].position.y==cel.y){
                            occupant=k; break;
                        }
                    }
                    /* Swap */
                    if (occupant>=0 && !j->pieces[occupant].en_place) {
                        SDL_Rect prev=j->pieces[i].position;
                        j->pieces[occupant].position=prev;
                        j->pieces[occupant].en_place=
                            (prev.x==j->pieces[occupant].cible.x &&
                             prev.y==j->pieces[occupant].cible.y);
                    }
                    j->pieces[i].position=cel;
                    j->pieces[i].en_place=
                        (cel.x==j->pieces[i].cible.x &&
                         cel.y==j->pieces[i].cible.y);
                } else {
                    /* Retourner dans un slot libre */
                    int k2;
                    for (k2=0;k2<NB_PIECES;k2++){
                        SDL_Rect slot={
                            PLATEAU_X+(k2%TAILLE_GRILLE)*taille_slot+ECART_SLOT/2,
                            PLATEAU_Y+(k2/TAILLE_GRILLE)*taille_slot+ECART_SLOT/2,
                            TAILLE_PIECE,TAILLE_PIECE
                        };
                        int occ=0;
                        for(k=0;k<NB_PIECES;k++){
                            if(k==i||j->pieces[k].en_place) continue;
                            if(j->pieces[k].position.x==slot.x &&
                               j->pieces[k].position.y==slot.y){occ=1;break;}
                        }
                        if (!occ){j->pieces[i].position=slot; break;}
                    }
                    j->pieces[i].en_place=0;
                }

                /* Verifier si tout est en place */
                {
                    int complet=1;
                    for(k=0;k<NB_PIECES;k++)
                        if(!j->pieces[k].en_place){complet=0;break;}
                    if(complet){
                        j->termine=1; j->score+=20;
                        if(j->musique[3]){
                            Mix_HaltMusic();
                            Mix_PlayMusic(j->musique[3],-1);
                        }
                    }
                }
            }
        } /* fin drop */

    } /* fin while PollEvent */
}


/* ============================================================
   FONCTION 5 : sauvegarder_score
   Ajouter un score dans scores.txt
   ============================================================ */
int sauvegarder_score(const char *nom, int pts)
{
    FILE *f = fopen("scores.txt", "a");
    if (f == NULL) {
        printf("Erreur : impossible d'ecrire scores.txt\n");
        return 0;
    }
    fprintf(f, "%s %d\n", nom, pts);
    fclose(f);
    return 1;
}


/* ============================================================
   FONCTION 6 : nettoyer_jeu
   Liberer toute la memoire et fermer SDL2
   ============================================================ */
void nettoyer_jeu(Jeu *j)
{
    int i;

    Mix_HaltMusic();

    if (j->petite) TTF_CloseFont(j->petite);
    if (j->police) TTF_CloseFont(j->police);

    for (i=1;i<=6;i++) if(j->fond[i]) SDL_DestroyTexture(j->fond[i]);
    for (i=1;i<=4;i++) if(j->img_puzzle[i]) SDL_DestroyTexture(j->img_puzzle[i]);
    for (i=1;i<=4;i++) if(j->musique[i]) Mix_FreeMusic(j->musique[i]);

    if (j->son_clic)   Mix_FreeChunk(j->son_clic);
    if (j->son_survol) Mix_FreeChunk(j->son_survol);

    TTF_Quit();
    Mix_CloseAudio();
    Mix_Quit();
    IMG_Quit();

    if (j->rendu)   SDL_DestroyRenderer(j->rendu);
    if (j->fenetre) SDL_DestroyWindow(j->fenetre);

    SDL_Quit();
    printf("Jeu ferme proprement.\n");
}
