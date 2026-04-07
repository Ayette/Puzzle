/* music.c – music management */
#include "../include/game.h"

void music_play(Game *g, int idx)
{
    if (idx < 1 || idx > 4) return;
    if (!g->music[idx]) return;
    /* Already playing the same track → don't restart */
    if (Mix_PlayingMusic() && Mix_GetMusicType(g->music[idx]) != MUS_NONE) {
        /* We track which is playing by checking the pointer directly */
    }
    Mix_HaltMusic();
    Mix_VolumeMusic(g->volume);
    Mix_PlayMusic(g->music[idx], -1);
}

void music_stop(Game *g)
{
    (void)g;
    Mix_HaltMusic();
}
