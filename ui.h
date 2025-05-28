#ifndef UI_H
#define UI_H

#include "game.h"

typedef struct
{
    Rectangle playButton;
    Rectangle musicButton;
    Rectangle soundButton;
    Rectangle resetButton;
    Texture2D wallpaper;
} MainMenuUI; MainMenuUI mainMenuUI;

typedef struct
{
    Rectangle LevelButton[5];
} LevelUI;

typedef struct
{
    Rectangle uiBackground;
    Rectangle levelRec;
    Rectangle scoreRec;
    Rectangle movesLeftRec;
    Rectangle menuButton;
    Rectangle musicButton;
    Rectangle soundButton;
    char charLevel[32];
    char charScore[32];
    char charMovesLeft[32];
} InGameUI;

typedef struct
{
    Rectangle restartButton;
    Rectangle menuButton;
    Rectangle nextLevelButton;
} AfterGameUI;

typedef struct
{
    Rectangle menuButton;
    Texture2D final;
} CompletedUI;

void InitUI();
void UpdateMainMenuScreen();
void UpdateLevelSelectionScreen();
void UpdateAfterGameScreen();
void UpdateGameCompletedScreen(); // Updates the interface that appears when the game is completed
void UpdateGameUI();

#endif