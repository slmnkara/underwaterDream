#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include "raylib.h"
#include "stdio.h"

// Initializing grid
#define GRID_WIDTH 8
#define GRID_HEIGHT 8

#define CELL_SIZE 100 // Size of each cell in pixels

#define GAME_UI_HEIGHT 60 // Height of the game UI in pixels

// Initializing screen dimensions
#define SCREEN_WIDTH (GRID_WIDTH * CELL_SIZE)
#define SCREEN_HEIGHT ((GRID_HEIGHT * CELL_SIZE) + GAME_UI_HEIGHT)

#define CANDY_TYPES 5 // Initializing candy types

// Initializing animation durations
#define EXPLOSION_ANIMATION_DURATION 1.0f
#define SWAP_ANIMATION_DURATION 0.5f
#define MAX_EXPLOSIONS 64

int grid[GRID_HEIGHT][GRID_WIDTH]; // Grid for candies

float visualY[GRID_HEIGHT][GRID_WIDTH]; // Visual Y positions for candies
float visualX[GRID_HEIGHT][GRID_WIDTH]; // Visual X positions for candies

extern const int TARGET_SCORE[5]; // Target scores for each level
extern const int MAX_MOVES; // Maximum moves allowed

typedef enum { BEFORE_PLAY, LEVEL_SELECTION, PLAYING, AFTER_PLAY, GAME_COMPLETED } GameState; // Game states for the game flow
extern GameState gameState; // External variable to use the current game state for main.c

typedef enum { WIN, LOSE } WinState; // Win states for the game
typedef enum { MUSIC_ON, MUSIC_OFF } MusicState; // Music states for the game
typedef enum { SOUND_ON, SOUND_OFF } SoundState; // Sound states for the game

typedef enum { BACKGROUND_MUSIC, ACTIVE_BUTTON_SOUND, DEACTIVE_BUTTON_SOUND, SWAP_SOUND,
EXPLOSION_SOUND, WIN_SOUND, LOSE_SOUND} Audio; // Audio play types for the game

typedef enum { CONTROL_NONE, SET_MUSIC_STATE, SET_SOUND_STATE, SET_LEVEL, RESET_LEVEL } ControlType; // Control types for button actions
typedef enum { ACTIVE, DEACTIVE } ButtonState; // Button states for the button sound effects

// UI structures for different screens
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

// Game statistics structure to keep track of the current game statistics
typedef struct
{
    int current_level;
    int current_score;
    int current_moves_left;
    int target_score;
} GameStats;

// Audio structure to manage all audio assets
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

// Dragging struct
typedef struct
{
    int dragStartX;
    int dragStartY;
} Drag;

// Animation structures for swap, explosion animations
typedef struct
{
    int swapActive;
    int swapX1, swapY1, swapX2, swapY2;
    float swapProgress;
} SwapAnim;

typedef struct {
    int x, y;
    int active;
    float explosionProgress;
    int candyType;
} ExplosionAnim;

// Function prototypes for initialization
void InitGame();
void InitUI();
void InitGrid();
void InitDrag();
void InitSwapAnim();

// Function prototypes for texture management
void LoadTextures();
void UnloadTextures();

// Function prototypes for audio management
void LoadAudio();
void UnloadAudio();
void PlayAudio(Audio audio);

// Function prototypes for updating different game screens
void UpdateMainMenuScreen();
void UpdateLevelSelectionScreen();
void UpdateGameplayScreen();
void UpdateAfterGameScreen();
void UpdateGameCompletedScreen();

// Function prototypes for game logic
void UpdateGameUI();
void UpdateGrid();
void HandleDragging();
int GetScoreForMatch(int length);
void MarkHorizontalMatches(bool toExplode[GRID_HEIGHT][GRID_WIDTH]);
void MarkVerticalMatches(bool toExplode[GRID_HEIGHT][GRID_WIDTH]);
void ProcessMatches();
void ProcessCandyFalls();
void UpdateSwap();
void TriggerExplosion(int x, int y, int candyType);
void UpdateExplosion(float delta);
void CheckGameStatus();

// Function prototype for button click handling
void IsButtonClicked(Rectangle button, ButtonState buttonState, GameState setGameState, ControlType controlType, int setLevel);

// Function prototypes for game progress saving and loading
void LoadGameProgress();
void SaveGameProgress(int level);

// Function prototype to deinitialize the game and free resources
void DeinitGame();

#endif // DEFINITIONS_H