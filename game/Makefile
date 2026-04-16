# ============================================================
# Makefile  –  PuzzleQuest
# Auteur : [Votre nom]
#
# Utilisation :
#   make        → compile le projet
#   make GEN    → compile (alias demande par le prof)
#   make clean  → supprime les fichiers generes
#   make run    → compile et lance le jeu
# ============================================================

# Compilateur et options
CC      = gcc
CFLAGS  = -Wall -Wextra -std=c99

# Bibliotheques SDL2 necessaires
LIBS    = -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf -lm

# Nom de l'executable final
PROG    = puzzlequest

# Fichiers sources (.c)
SOURCES = main.c jeu.c

# Fichiers objets (.o) generes automatiquement
OBJETS  = $(SOURCES:.c=.o)

# ============================================================
# Regle principale : compiler tout le projet
# ============================================================
all: $(PROG)

# Cible GEN demandee par le professeur (identique a all)
GEN: $(PROG)

# Lier les fichiers objets pour creer l'executable
$(PROG): $(OBJETS)
	$(CC) $(OBJETS) -o $(PROG) $(LIBS)
	@echo "Compilation reussie ! Lancez : ./$(PROG)"

# Compiler chaque fichier .c en .o
main.o: main.c jeu.h
	$(CC) $(CFLAGS) -c main.c -o main.o

jeu.o: jeu.c jeu.h
	$(CC) $(CFLAGS) -c jeu.c -o jeu.o

# ============================================================
# Nettoyer les fichiers generes
# ============================================================
clean:
	rm -f $(OBJETS) $(PROG)
	@echo "Nettoyage termine."

# Compiler et lancer directement
run: GEN
	./$(PROG)

# Indiquer que ces cibles ne sont pas des fichiers
.PHONY: all GEN clean run
