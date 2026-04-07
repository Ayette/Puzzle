# Makefile – PuzzleQuest
CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -Iinclude \
          $(shell sdl2-config --cflags)

SDL_LIBS = $(shell sdl2-config --libs)
LDFLAGS  = $(SDL_LIBS) -lSDL2_image -lSDL2_mixer -lSDL2_ttf -lm

TARGET  = puzzlequest

SRCS    = main.c \
          src/init.c \
          src/assets.c \
          src/button.c \
          src/render.c \
          src/music.c \
          src/menu_main.c \
          src/menu_options.c \
          src/menu_saveload.c \
          src/menu_player.c \
          src/menu_scores.c \
          src/menu_enigma.c \
          src/quiz.c \
          src/puzzle.c

OBJS    = $(SRCS:.c=.o)

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)
	@echo "Build OK → ./$(TARGET)"

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

run: all
	./$(TARGET)
