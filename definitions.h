#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include "stdio.h"
#include "raylib.h"

typedef struct
{
    int dragStartX;
    int dragStartY;
} Drag;

// For game states and music/sound states
typedef enum { BEFORE_PLAY, LEVEL_SELECTION, PLAYING, AFTER_PLAY, COMPLETED } GameState;
typedef enum { WIN, LOSE } WinState;
typedef enum { NONE, MUSIC, SOUND, SET_LEVEL, RESET_LEVEL } ControlType;
typedef enum { ACTIVE, DEACTIVE } ButtonState;

const int TARGET_SCORE[5] = { 1000, 1250, 1500, 1750, 2000 }; // Target scores for each level
const int MAX_MOVES = 20; // Maximum moves allowed per level
int grid[GRID_HEIGHT][GRID_WIDTH];

typedef struct
{
    Music backgroundMusic;
    Sound button;
    Sound swap;
    Sound explosion;
    Sound win;
    Sound lose;
} Audio_t;
Audio_t audio_t;

typedef enum { MUSIC_ON, MUSIC_OFF } MusicState;
MusicState musicState;

typedef enum { SOUND_ON, SOUND_OFF } SoundState;
SoundState soundState;

typedef enum { BACKGROUND_MUSIC, BUTTON_ACTIVE, BUTTON_DEACTIVE, SWAP, EXPLOSION, WIN, LOSE } Audio;


// GAme init 
GameState gameState = 0;
WinState winState = LOSE;
MusicState musicState = MUSIC_ON;
SoundState soundState = SOUND_ON;

#endif // DEFINITIONS_H