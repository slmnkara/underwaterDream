#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include "raylib.h"
#include "stdio.h"



// Initializing grid, cell, in game ui and screen sizes
#define GRID_WIDTH 8
#define GRID_HEIGHT 8
#define CELL_SIZE 100
#define GAME_UI_HEIGHT 60
#define SCREEN_WIDTH (GRID_WIDTH * CELL_SIZE)
#define SCREEN_HEIGHT ((GRID_HEIGHT * CELL_SIZE) + GAME_UI_HEIGHT)

// Initializing candy types
#define CANDY_TYPES 5

// Initializing animation durations
#define EXPLOSION_ANIMATION_DURATION 1.0f
#define SWAP_ANIMATION_DURATION 0.5f
#define MAX_EXPLOSIONS 64

int grid[GRID_HEIGHT][GRID_WIDTH]; // Grid for candies
float visualY[GRID_HEIGHT][GRID_WIDTH]; // Visual Y positions for candies
float visualX[GRID_HEIGHT][GRID_WIDTH]; // Visual X positions for candies
extern const int TARGET_SCORE[5]; // Target scores for each level
extern const int MAX_MOVES; // Maximum moves allowed

// For game states and music/sound states
typedef enum { BEFORE_PLAY, LEVEL_SELECTION, PLAYING, AFTER_PLAY, GAME_COMPLETED } GameState; extern GameState gameState;
typedef enum { WIN, LOSE } WinState; 
typedef enum { MUSIC_ON, MUSIC_OFF } MusicState;
typedef enum { SOUND_ON, SOUND_OFF } SoundState;  

typedef enum { BACKGROUND_MUSIC, ACTIVE_BUTTON_SOUND, DEACTIVE_BUTTON_SOUND, SWAP_SOUND,
                EXPLOSION_SOUND, WIN_SOUND, LOSE_SOUND } Audio;

// For isButtonClicked() function
typedef enum { CONTROL_NONE, SET_MUSIC_STATE, SET_SOUND_STATE, SET_LEVEL, RESET_LEVEL } ControlType;
typedef enum { ACTIVE, DEACTIVE } ButtonState;

typedef struct
{
    Rectangle play_button;
    Rectangle music_button;
    Rectangle sound_button;
    Rectangle reset_button;
    Texture2D wallpaper;
} MainMenuUI;

typedef struct
{
    Rectangle level_button[5];
} LevelUI;

typedef struct
{
    Rectangle background;
    Rectangle level_rec;
    Rectangle score_rec;
    Rectangle moves_left_rec;
    Rectangle menu_button;
    Rectangle music_button;
    Rectangle sound_button;
    char char_level[32];
    char char_score[32];
    char char_moves_left[32];
    Texture2D candy_textures[CANDY_TYPES];
} GameUI;

typedef struct
{
    Rectangle restart_button;
    Rectangle menu_button;
    Rectangle next_level_button;
} AfterGameUI;

typedef struct
{
    Rectangle menu_button;
    Texture2D trophy_texture;
} GameCompletedUI;
typedef struct
{
    int current_level;
    int current_score;
    int current_moves_left;
    int target_score;
} GameStats;

typedef struct
{
    Music background_music;
    Sound active_button_sound;
    Sound deactive_button_sound;
    Sound swap_sound;
    Sound explosion_sound;
    Sound win_sound;
    Sound lose_sound;
} Audio_t;

typedef struct
{
    int dragStartX;
    int dragStartY;
} Drag;

typedef struct
{
    int swapActive;
    int swapX1, swapY1, swapX2, swapY2;
    float swapProgress;
} SwapAnim;

float swapStartX1, swapStartY1, swapStartX2, swapStartY2;

typedef struct {
    int x, y;
    int active;
    float explosionProgress;
    int candyType;
} ExplosionAnim;

void LoadTextures();
void UnloadTextures();
void InitGame();
void InitUI();
void LoadAudio();
void UnloadAudio();
void PlayAudio(Audio audio);
void InitGrid();
void UpdateMainMenuScreen();
void UpdateLevelSelectionScreen();
void UpdateGameplayScreen();
void UpdateAfterGameScreen();
void UpdateGameCompletedScreen();
void UpdateGameUI();
void UpdateGrid();
void HandleDragging();
int GetScoreForMatch(int length);
void MarkHorizontalMatches(bool toExplode[GRID_HEIGHT][GRID_WIDTH]);
void MarkVerticalMatches(bool toExplode[GRID_HEIGHT][GRID_WIDTH]);
void ProcessMatches();
void ProcessCandyFalls();
void CheckGameStatus();
void TriggerExplosion(int x, int y, int candyType);
void UpdateExplosion(float delta);
void UpdateSwap();
void IsButtonClicked(Rectangle button, ButtonState buttonState, GameState setGameState, ControlType controlType, int setLevel);
void LoadGameProgress();
void SaveGameProgress(int level);

#endif // DEFINITIONS_H