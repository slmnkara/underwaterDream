#ifndef GAME_H
#define GAME_H

#include "stdio.h"
#include "raylib.h"
#include "game.h"
#include "ui.h"
#include "game_logic.h"

// Initializing grid, cell, in game ui and screen sizes
#define GRID_WIDTH 8
#define GRID_HEIGHT 8
#define CELL_SIZE 100
#define IN_GAME_UI_HEIGHT 60
#define SCREEN_WIDTH (GRID_WIDTH * CELL_SIZE)
#define SCREEN_HEIGHT ((GRID_HEIGHT * CELL_SIZE) + IN_GAME_UI_HEIGHT)

#define CANDY_TYPES 5
Texture2D candyTextures[CANDY_TYPES];
Image icon; // For window icon

typedef struct
{
    int level;
    int score;
    int targetScore;
    int movesLeft;
} GameStats;
GameStats gameStats;

void InitGame();
void LoadTextures();
void UnloadTextures();
void LoadGameProgress();
void SaveGameProgress();

void LoadAudio();
void PlayAudio(Audio audio);
void UnloadAudio();

#endif