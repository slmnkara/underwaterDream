#include "raylib.h"
#include "stdio.h"
#include "time.h"
#include "stdlib.h"

// Initializing grid, cell, in game ui and screen sizes
#define GRID_WIDTH 8
#define GRID_HEIGHT 8
#define CELL_SIZE 100
#define IN_GAME_UI_HEIGHT 60
#define SCREEN_WIDTH (GRID_WIDTH * CELL_SIZE)
#define SCREEN_HEIGHT ((GRID_HEIGHT * CELL_SIZE) + IN_GAME_UI_HEIGHT)

// Initializing animation durations
#define EXPLOSION_ANIMATION_DURATION 1.0f
#define SWAP_ANIMATION_DURATION 0.5f
#define MAX_EXPLOSIONS 64

// Initializing candy types
#define CANDY_TYPES 5
Texture2D candyTextures[CANDY_TYPES];

// Initializing grid size
int grid[GRID_HEIGHT][GRID_WIDTH];

typedef struct
{
    int swapActive;
    int swapX1, swapY1, swapX2, swapY2;
    float swapProgress;
} SwapAnim; SwapAnim swapAnim;

typedef struct {
    int x, y;
    int active;
    float explosionProgress;
    int candyType;
} ExplosionAnim; ExplosionAnim explosionAnim[MAX_EXPLOSIONS];

float visualY[GRID_HEIGHT][GRID_WIDTH];
float visualX[GRID_HEIGHT][GRID_WIDTH];

typedef struct
{
    Rectangle playButton;
    Rectangle musicButton;
    Rectangle soundButton;
    Texture2D wallpaper;
} MenuUI; MenuUI menuUI;

typedef struct
{
    Rectangle LevelButton[5];
    int TargetScore[5];
} LevelUI; LevelUI levelUI;

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
} InGameUI; InGameUI inGameUI;

typedef struct
{
    Rectangle restartButton;
    Rectangle menuButton;
    Rectangle nextLevelButton;
} AfterGameUI; AfterGameUI afterGameUI;

typedef struct
{
    Rectangle menuButton;
    Texture2D final;
} CompletedUI; CompletedUI completedUI;

typedef struct
{
    int level;
    int score;
    int targetScore;
    int movesLeft;
} GameStats; GameStats gameStats;

typedef struct
{
    Music music;
    Sound buttonClick;
    Sound swapSound;
    Sound explosionSound;
    Sound winSound;
    Sound loseSound;
} SoundEffects; SoundEffects soundEffects;

typedef struct
{
    int dragStartX;
	int dragStartY;
} Drag; Drag drag;

// For game states and music/sound states
typedef enum { BEFOREPLAY, LEVELSELECTION, PLAYING, AFTERPLAY, COMPLETED } GameState; GameState gameState = 0;
typedef enum { WIN, LOSE } WinState; WinState winState = LOSE;
typedef enum { MUSIC_ON, MUSIC_OFF } MusicState; MusicState musicState = MUSIC_ON;
typedef enum { SOUND_ON, SOUND_OFF } SoundState; SoundState soundState = SOUND_ON;

// Function prototypes
void InitAndLoad();
void UpdateMusic();
void ButtonClick(Rectangle, int, bool, bool, int);
void UpdateMenu();
void UpdateLevelSelection();
void UpdateGameplay();
void UpdateInGameUI();
void UpdateGrid();
void Dragging();
void CheckMatches();
void DropCandies();
void UpdateResult();
void StartExplosion(int x, int y, int candyType);
void UpdateExplosion(float delta);
void UpdateSwap();
void UpdateAfterGame();
void UpdateCompleted();
void Unload(); // Kaynakları serbest bırakma

int main(void)
{
	InitAndLoad(); // Initializing requirements and loading textures
    while (!WindowShouldClose())
    {
		BeginDrawing();
        switch (gameState) // Updating the screen depending on gameState
        {
        case BEFOREPLAY:
            UpdateMenu();
            break;
		case LEVELSELECTION:
            UpdateLevelSelection();
			break;
        case PLAYING:
			UpdateGameplay();
            break;
        case AFTERPLAY:
			UpdateAfterGame();
            break;
        case COMPLETED:
            UpdateCompleted();
            break;
        }
        EndDrawing();
	}
	Unload(); // Releasing resources when the game ends
    return 0;
}

void InitAndLoad()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Underwater Dream");
    InitAudioDevice();
    Image icon = LoadImage("assets/images/icon.png");
    SetWindowIcon(icon);
        // UI COMPONENTS
    // menuUI
    menuUI.playButton = (Rectangle){ 300,300,200,50 };
    menuUI.musicButton = (Rectangle){ 325,410,150,50 };
    menuUI.soundButton = (Rectangle){ 325,490,150,50 };
    menuUI.wallpaper = LoadTexture("assets/images/wallpaper.png");
    // levelUI
    levelUI.LevelButton[0] = (Rectangle){ 300, 275, 200, 50 };
    levelUI.LevelButton[1] = (Rectangle){ 300, 375, 200, 50 };
    levelUI.LevelButton[2] = (Rectangle){ 300, 475, 200, 50 };
    levelUI.LevelButton[3] = (Rectangle){ 300, 575, 200, 50 };
    levelUI.LevelButton[4] = (Rectangle){ 300, 675, 200, 50 };
    // inGameUI
    candyTextures[0] = LoadTexture("assets/images/image1.png");
    candyTextures[1] = LoadTexture("assets/images/image2.png");
    candyTextures[2] = LoadTexture("assets/images/image3.png");
    candyTextures[3] = LoadTexture("assets/images/image4.png");
    candyTextures[4] = LoadTexture("assets/images/image5.png");
    inGameUI.levelRec = (Rectangle){ 4,4,194,52 };
    inGameUI.scoreRec = (Rectangle){ 202,4,296,52 };
    inGameUI.movesLeftRec = (Rectangle){ 502,4,294,52 };
    inGameUI.uiBackground = (Rectangle){ 0,0,800,60 };
    inGameUI.menuButton = (Rectangle){ 5,5,390,50 };
    inGameUI.musicButton = (Rectangle){ 400,5,100,50 };
    inGameUI.soundButton = (Rectangle){ 505,5,100,50 };
    // afterGameUI
    afterGameUI.menuButton = (Rectangle){ 200,400,400,100 };
    afterGameUI.restartButton = (Rectangle){ 275,600,250,75 };
    afterGameUI.nextLevelButton = (Rectangle){ 275,600,250,75 };
    // CompletedUI
    completedUI.menuButton = (Rectangle){ 250,650,300,50 };
    completedUI.final = LoadTexture("assets/images/final.png");

        // MUSIC AND SOUND EFFECTS
    // Background music
    soundEffects.music = LoadMusicStream("assets/soundEffects/music.mp3");
    PlayMusicStream(soundEffects.music);
    SetMusicVolume(soundEffects.music, 0.1f);
    
    // Sound Effects
    soundEffects.buttonClick = LoadSound("assets/soundEffects/button.mp3");
    soundEffects.swapSound = LoadSound("assets/soundEffects/swap.mp3");
    soundEffects.explosionSound = LoadSound("assets/soundEffects/explosion.mp3");
    soundEffects.winSound = LoadSound("assets/soundEffects/win.mp3");
    soundEffects.loseSound = LoadSound("assets/soundEffects/lose.mp3");
    SetSoundVolume(soundEffects.buttonClick, 0.5f);
    SetSoundVolume(soundEffects.swapSound, 0.2f);
    SetSoundVolume(soundEffects.explosionSound, 0.1f);
    SetSoundVolume(soundEffects.winSound, 0.8f);
    SetSoundVolume(soundEffects.loseSound, 0.5f);

        // GAMEDATA
    // If there are any data in the file
    // 
    // Initializing level, score and moves left
    gameStats.level = 1;
    gameStats.score = 0;
    gameStats.movesLeft = 20;
    // Initializing target scores for each level
    int tempScore = 1000;
    for (int n = 0; n <= 4; n++)
    {
        levelUI.TargetScore[n] = tempScore;
        tempScore += 250;
    }

	// Initializing first values for candies
    for (int y = 0; y < GRID_HEIGHT; y++)
    {
        for (int x = 0; x < GRID_WIDTH; x++)
        {
            int candy;
            do
            {
                candy = GetRandomValue(0, CANDY_TYPES - 1);
            } while (
                (x >= 2 && grid[y][x - 1] == candy && grid[y][x - 2] == candy) ||
                (y >= 2 && grid[y - 1][x] == candy && grid[y - 2][x] == candy)
                ); // This condition prevents the formation of candies that will explode.
            grid[y][x] = candy;
        }
    }

    // Dragging start values
    drag.dragStartX = -1;
    drag.dragStartY = -1;

    // Swap animation start values
    swapAnim.swapActive = 0;
    swapAnim.swapProgress = 0.0f;
    for (int y = 0; y < GRID_HEIGHT; y++)
    {
        for (int x = 0; x < GRID_WIDTH; x++)
        {
            visualY[y][x] = (float)y;
            visualX[y][x] = (float)x;
        }
    }

    
}

void UpdateMusic(void)
{
    if (musicState == MUSIC_ON) UpdateMusicStream(soundEffects.music);
}

void ButtonClick(Rectangle button, int state, bool musicControl, bool soundControl, int targetS)
{
    Vector2 mousePos = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        if (CheckCollisionPointRec(mousePos, button))
        {
            if (musicControl)
            {
                if (musicState == MUSIC_ON) musicState = MUSIC_OFF;
                else if (musicState == MUSIC_OFF) musicState = MUSIC_ON;
            }
            if (soundControl)
            {
                if (soundState == SOUND_ON) soundState = SOUND_OFF;
                else if (soundState == SOUND_OFF) soundState = SOUND_ON;
            }
            if (targetS < gameStats.level)
            {
                if (targetS != -1)
                {
                    gameStats.targetScore = levelUI.TargetScore[targetS];

                }
                gameState = state;
                if (soundState == SOUND_ON) PlaySound(soundEffects.buttonClick);

                // Resetting stats
                gameStats.score = 0;
                gameStats.movesLeft = 20;
            }
            else if (soundState == SOUND_ON) PlaySound(soundEffects.loseSound);
        }
    }
}

void UpdateMenu()
{
    UpdateMusic();
    // Drawing wallpaper, buttons and labels for main menu.
    DrawTexture(menuUI.wallpaper, 0, 0, WHITE);
    DrawRectangleRec(menuUI.playButton, DARKPURPLE);
    DrawRectangleRec(menuUI.musicButton, DARKBLUE);
    DrawRectangleRec(menuUI.soundButton, DARKBLUE);
    DrawText("Underwater Dream", 200, 120, 42, DARKPURPLE);
    DrawText("Similar to Candy Crush Saga. Coded with Raylib.", 150, 170, 21, BLACK);
    DrawText("Start!", menuUI.playButton.x+65, menuUI.playButton.y+15, 25, WHITE);
    DrawText("Music", menuUI.musicButton.x+20, menuUI.musicButton.y+17, 18, WHITE);
    DrawText("Sound", menuUI.soundButton.x+20, menuUI.soundButton.y+17, 18, WHITE);

    // Control of ON-OFF labels
    if (musicState == MUSIC_ON) DrawText("ON", menuUI.musicButton.x + 100, menuUI.musicButton.y + 17, 18, GREEN);
    else DrawText("OFF", menuUI.musicButton.x + 100, menuUI.musicButton.y + 17, 18, RED);
    if (soundState == SOUND_ON) DrawText("ON", menuUI.soundButton.x + 100, menuUI.soundButton.y + 17, 18, GREEN);
    else DrawText("OFF", menuUI.soundButton.x + 100, menuUI.soundButton.y + 17, 18, RED);

    // Control of buttons
    ButtonClick(menuUI.playButton, 1, 0, 0, -1);
    ButtonClick(menuUI.musicButton, 0, 1, 0, -1);
    ButtonClick(menuUI.soundButton, 0, 0, 1, -1);
}

void UpdateLevelSelection()
{
    UpdateMusic();
    ClearBackground(RAYWHITE);
    // Drawing labels
    DrawText("Level Selection", SCREEN_WIDTH / 8, SCREEN_HEIGHT / 10, 80, DARKPURPLE);
    DrawText("Select a level to play!", SCREEN_WIDTH / 4 + 30, SCREEN_HEIGHT / 5, 30, DARKPURPLE);

    for (int i = gameStats.level; i <= 4; i++)
    {
        DrawRectangleRec(levelUI.LevelButton[i], RED);
        DrawText("LOCKED!", levelUI.LevelButton[i].x + 50, levelUI.LevelButton[i].y + 13, 25, WHITE);
        ButtonClick(levelUI.LevelButton[i], 2, 0, 0, i);
    }

    // Drawing buttons and their labels
    char levelNum[32] = {0};
    int tempLevel = 1;
    for (int i = 0; i < gameStats.level; i++)
    {
        DrawRectangleRec(levelUI.LevelButton[i], DARKPURPLE);
        snprintf(levelNum, sizeof(levelNum), "Level %d", tempLevel);
        DrawText(levelNum, levelUI.LevelButton[i].x + 60, levelUI.LevelButton[i].y + 13, 25, WHITE);
        tempLevel += 1;
        ButtonClick(levelUI.LevelButton[i], 2, 0, 0, i);
    }
}

void UpdateGameplay()
{
    UpdateMusic();
    ClearBackground(RAYWHITE);
    UpdateInGameUI();
    UpdateGrid();
    if (!swapAnim.swapActive) Dragging();
    CheckMatches();
    UpdateResult();
    UpdateExplosion(GetFrameTime());
	UpdateSwap();
}

void UpdateInGameUI()
{
    Vector2 mousePos = GetMousePosition();
    if (mousePos.y <= IN_GAME_UI_HEIGHT)
    {
        DrawRectangleRec(inGameUI.uiBackground, DARKPURPLE);
        DrawRectangleRec(inGameUI.menuButton, RAYWHITE);
        DrawRectangleRec(inGameUI.musicButton, RAYWHITE);
        DrawRectangleRec(inGameUI.soundButton, RAYWHITE);
        DrawText("Return to the main menu", inGameUI.menuButton.x + 45, inGameUI.menuButton.y + 15, 25, DARKPURPLE);
        DrawText("Music", inGameUI.musicButton.x + 22, inGameUI.musicButton.y + 8, 20, DARKPURPLE);
        DrawText("Sound", inGameUI.soundButton.x + 20, inGameUI.soundButton.y + 8, 20, DARKPURPLE);

        // Control of ON-OFF labels
        if (musicState == MUSIC_ON) DrawText("ON", inGameUI.musicButton.x + 40, inGameUI.musicButton.y + 28, 18, GREEN);
        else DrawText("OFF", inGameUI.musicButton.x + 35, inGameUI.musicButton.y + 28, 18, RED);
        if (soundState == SOUND_ON) DrawText("ON", inGameUI.soundButton.x + 40, inGameUI.soundButton.y + 28, 18, GREEN);
        else DrawText("OFF", inGameUI.soundButton.x + 35, inGameUI.soundButton.y + 28, 18, RED);

        char info[128];
        snprintf(info, sizeof(info), "Level: %d", gameStats.targetScore/250-3);
        DrawText(info, 635, 15, 35, RAYWHITE);

        // Button controls
        ButtonClick(inGameUI.menuButton, 0, 0, 0, -1);
        ButtonClick(inGameUI.musicButton, 2, 1, 0, -1);
        ButtonClick(inGameUI.soundButton, 2, 0, 1, -1);
    }
    else
    {
        DrawRectangleRec(inGameUI.uiBackground, DARKPURPLE);
        DrawRectangleRec(inGameUI.levelRec, RAYWHITE);
        DrawRectangleRec(inGameUI.scoreRec, RAYWHITE);
        DrawRectangleRec(inGameUI.movesLeftRec, RAYWHITE);
        snprintf(inGameUI.charLevel, sizeof(inGameUI.charLevel), "Level: %d", gameStats.targetScore / 250 - 3);
        snprintf(inGameUI.charScore, sizeof(inGameUI.charScore), "Score: %d / %d", gameStats.score, gameStats.targetScore);
        snprintf(inGameUI.charMovesLeft, sizeof(inGameUI.charMovesLeft), "Moves Left: %d", gameStats.movesLeft);
        DrawText(inGameUI.charLevel, inGameUI.levelRec.x + 35, inGameUI.levelRec.y + 12, 32, VIOLET);
        DrawText(inGameUI.charScore, inGameUI.scoreRec.x + 20, inGameUI.scoreRec.y + 15, 29, DARKGREEN);
        DrawText(inGameUI.charMovesLeft, inGameUI.movesLeftRec.x + 33, inGameUI.movesLeftRec.y + 14, 30, DARKBLUE);
    }
}

void UpdateGrid()
{
    // Her karede görsel konumu güncelle
    float fallSpeed = 5.0f * GetFrameTime(); // Hızı ayarlayabilirsiniz
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            if (visualY[y][x] < y) {
                visualY[y][x] += fallSpeed;
                if (visualY[y][x] > y) visualY[y][x] = (float)y;
            }
            else if (visualY[y][x] > y) {
                visualY[y][x] = (float)y; // yukarı çıkma olmasın
            }
        }
    }

    // Draw normal candies
    for (int y = 0; y < GRID_HEIGHT; y++)
    {
        for (int x = 0; x < GRID_WIDTH; x++)
        {
            Rectangle cell = { x * CELL_SIZE, y * CELL_SIZE + IN_GAME_UI_HEIGHT, CELL_SIZE, CELL_SIZE };
            Texture2D tex = candyTextures[grid[y][x]];
            Rectangle source = { 0, 0, (float)tex.width, (float)tex.height };
            Rectangle dest = { visualX[y][x] * CELL_SIZE, visualY[y][x] * CELL_SIZE + IN_GAME_UI_HEIGHT, CELL_SIZE, CELL_SIZE };
            Vector2 origin = { 0, 0 };
            DrawTexturePro(tex, source, dest, origin, 0.0f, WHITE);
            DrawRectangleLinesEx(cell, 1, DARKPURPLE);
        }
    }

    // Draw explosion animations
    for (int i = 0; i < MAX_EXPLOSIONS; i++) {
        if (explosionAnim[i].active) {
            float alpha = 1.0f - (explosionAnim[i].explosionProgress / EXPLOSION_ANIMATION_DURATION); // Fade out
            if (alpha < 0) alpha = 0;
            int x = explosionAnim[i].x;
            int y = explosionAnim[i].y;
            Texture2D tex = candyTextures[explosionAnim[i].candyType];
            Rectangle source = { 0, 0, (float)tex.width, (float)tex.height };
            Rectangle dest = { x * CELL_SIZE, y * CELL_SIZE + IN_GAME_UI_HEIGHT, CELL_SIZE, CELL_SIZE };
            Vector2 origin = { 0, 0 };
            Color fadeColor = WHITE;
            fadeColor.a = (unsigned char)(255 * alpha);
            DrawTexturePro(tex, source, dest, origin, 0.0f, fadeColor);
        }
    }
}

void Dragging()
{
    Vector2 mousePos = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (mousePos.y >= IN_GAME_UI_HEIGHT) {
            drag.dragStartX = mousePos.x / CELL_SIZE;
            drag.dragStartY = (mousePos.y - IN_GAME_UI_HEIGHT) / CELL_SIZE;
        }
    }
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        if (drag.dragStartX != -1 && drag.dragStartY != -1 && !swapAnim.swapActive) {
            int dropX = mousePos.x / CELL_SIZE;
            int dropY = (mousePos.y - IN_GAME_UI_HEIGHT) / CELL_SIZE;

            // Komşuluk kontrolü
            if ((abs(drag.dragStartX - dropX) == 1 && drag.dragStartY == dropY) ||
                (abs(drag.dragStartY - dropY) == 1 && drag.dragStartX == dropX)) {

                // Swap animasyonunu başlat
                swapAnim.swapActive = 1;
                swapAnim.swapX1 = drag.dragStartX;
                swapAnim.swapY1 = drag.dragStartY;
                swapAnim.swapX2 = dropX;
                swapAnim.swapY2 = dropY;
                swapAnim.swapProgress = 0.0f;
                gameStats.movesLeft--;
                if (soundState == SOUND_ON) PlaySound(soundEffects.swapSound);
            }

            drag.dragStartX = drag.dragStartY = -1;
        }
    }
}

void CheckMatches()
{
    bool matchFound;
    do {
        matchFound = false;

        // Yatay eşleşmelerin bulunması
        for (int y = 0; y < GRID_HEIGHT; y++) {
            for (int x = 0; x < GRID_WIDTH - 4; x++) {
                int candy = grid[y][x];
                if (candy != -1 &&
                    candy == grid[y][x + 1] &&
                    candy == grid[y][x + 2] &&
                    candy == grid[y][x + 3] &&
                    candy == grid[y][x + 4]) {
                    for (int i = 0; i < 5; i++) {
                        StartExplosion(x + i, y, grid[y][x + i]);
                        grid[y][x + i] = -1;
                    }
                    matchFound = true;
                    gameStats.score += 250;
                }
            }
            for (int x = 0; x < GRID_WIDTH - 3; x++) {
                int candy = grid[y][x];
                if (candy != -1 &&
                    candy == grid[y][x + 1] &&
                    candy == grid[y][x + 2] &&
                    candy == grid[y][x + 3]) {
                    for (int i = 0; i < 4; i++) {
                        StartExplosion(x + i, y, grid[y][x + i]);
                        grid[y][x + i] = -1;
                    }
                    matchFound = true;
                    gameStats.score += 150;
                }
            }
            for (int x = 0; x < GRID_WIDTH - 2; x++) {
                int candy = grid[y][x];
                if (candy != -1 &&
                    candy == grid[y][x + 1] &&
                    candy == grid[y][x + 2]) {
                    for (int i = 0; i < 3; i++) {
                        StartExplosion(x + i, y, grid[y][x + i]);
                        grid[y][x + i] = -1;
                    }
                    matchFound = true;
                    gameStats.score += 50;
                }
            }
        }

        // Dikey eşleşmelerin bulunması
        for (int x = 0; x < GRID_WIDTH; x++) {
            for (int y = 0; y < GRID_HEIGHT - 4; y++) {
                int candy = grid[y][x];
                if (candy != -1 &&
                    candy == grid[y + 1][x] &&
                    candy == grid[y + 2][x] &&
                    candy == grid[y + 3][x] &&
                    candy == grid[y + 4][x]) {
                    for (int i = 0; i < 5; i++) {
                        StartExplosion(x, y + i, grid[y + i][x]);
                        grid[y + i][x] = -1;
                    }
                    matchFound = true;
                    gameStats.score += 250;
                }
            }
            for (int y = 0; y < GRID_HEIGHT - 3; y++) {
                int candy = grid[y][x];
                if (candy != -1 &&
                    candy == grid[y + 1][x] &&
                    candy == grid[y + 2][x] &&
                    candy == grid[y + 3][x]) {
                    for (int i = 0; i < 4; i++) {
                        StartExplosion(x, y + i, grid[y + i][x]);
                        grid[y + i][x] = -1;
                    }
                    matchFound = true;
                    gameStats.score += 150;
                }
            }
            for (int y = 0; y < GRID_HEIGHT - 2; y++) {
                int candy = grid[y][x];
                if (candy != -1 &&
                    candy == grid[y + 1][x] &&
                    candy == grid[y + 2][x]) {
                    for (int i = 0; i < 3; i++) {
                        StartExplosion(x, y + i, grid[y + i][x]);
                        grid[y + i][x] = -1;
                    }
                    matchFound = true;
                    gameStats.score += 50;
                }
            }
        }
        // Eşleşme bulunduğunda şekerleri düşürme
        if (matchFound)
        {
            if (soundState == SOUND_ON) PlaySound(soundEffects.explosionSound);
            DropCandies();
        }
    } while (matchFound);
}

void DropCandies() {
    for (int x = 0; x < GRID_WIDTH; x++) {
        for (int y = GRID_HEIGHT - 1; y >= 0; y--) {
            if (grid[y][x] == -1) {
                for (int k = y - 1; k >= 0; k--) {
                    if (grid[k][x] != -1) {
                        grid[y][x] = grid[k][x];
                        grid[k][x] = -1;
                        visualY[y][x] = visualY[k][x]; // Görsel konumu da taşı
                        visualX[y][x] = (float)x;
                        break;
                    }
                }
            }
        }
    }
    // Yeni şekerler için
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            if (grid[y][x] == -1) {
                grid[y][x] = GetRandomValue(0, CANDY_TYPES - 1);
                visualY[y][x] = -1.5f; // Ekran dışından başlasın
                visualX[y][x] = (float)x;
            }
        }
    }
}

void UpdateResult()
{
    // Sonuç kontrolü
    if (gameStats.score >= gameStats.targetScore)
    {
        winState = WIN;
        if (soundState == SOUND_ON) PlaySound(soundEffects.winSound);
        for (int i = 1; i <= 5; i++)
        {
            if (gameStats.level == i && gameStats.targetScore == 750 + 250 * i) gameStats.level++;
        }
        
    }
    else if (gameStats.movesLeft <= 0)
    {
        winState = LOSE;
        if (soundState == SOUND_ON) PlaySound(soundEffects.loseSound);
    }
    // Skora göre oyunu bitirme kontrolü
    
    if (gameStats.movesLeft <= 0 || gameStats.score >= gameStats.targetScore)
    {
        gameState = AFTERPLAY;
        if (winState == WIN && gameStats.level > 5) gameState = COMPLETED;
    }
}

void StartExplosion(int x, int y, int candyType)
{
    for (int i = 0; i < MAX_EXPLOSIONS; i++)
    {
        if (!explosionAnim[i].active)
        {
            explosionAnim[i].x = x;
            explosionAnim[i].y = y;
            explosionAnim[i].candyType = candyType;
            explosionAnim[i].explosionProgress = 0.0f;
            explosionAnim[i].active = 1;
            break;
        }
    }
}

void UpdateExplosion(float delta)
{
    for (int i = 0; i < MAX_EXPLOSIONS; i++)
    {
        if (explosionAnim[i].active)
        {
            explosionAnim[i].explosionProgress += delta;
            if (explosionAnim[i].explosionProgress >= EXPLOSION_ANIMATION_DURATION)
            {
                explosionAnim[i].active = 0;
            }
        }
    }
}

void UpdateSwap()
{
    // Swap animasyonu güncelle
    if (swapAnim.swapActive) {
        float delta = GetFrameTime();
        swapAnim.swapProgress += delta / SWAP_ANIMATION_DURATION;
        if (swapAnim.swapProgress >= 1.0f) {
            // Swap işlemini grid'de uygula
            int temp = grid[swapAnim.swapY1][swapAnim.swapX1];
            grid[swapAnim.swapY1][swapAnim.swapX1] = grid[swapAnim.swapY2][swapAnim.swapX2];
            grid[swapAnim.swapY2][swapAnim.swapX2] = temp;

            // Görsel konumları da güncelle
            float tempX = visualX[swapAnim.swapY1][swapAnim.swapX1];
            float tempY = visualY[swapAnim.swapY1][swapAnim.swapX1];
            visualX[swapAnim.swapY1][swapAnim.swapX1] = visualX[swapAnim.swapY2][swapAnim.swapX2];
            visualY[swapAnim.swapY1][swapAnim.swapX1] = visualY[swapAnim.swapY2][swapAnim.swapX2];
            visualX[swapAnim.swapY2][swapAnim.swapX2] = tempX;
            visualY[swapAnim.swapY2][swapAnim.swapX2] = tempY;

            swapAnim.swapActive = 0;
            swapAnim.swapProgress = 0.0f;
        }
        else {
            // Görsel konumları ara pozisyona taşı
            float t = swapAnim.swapProgress;
            visualX[swapAnim.swapY1][swapAnim.swapX1] = (1 - t) * swapAnim.swapX1 + t * swapAnim.swapX2;
            visualY[swapAnim.swapY1][swapAnim.swapX1] = (1 - t) * swapAnim.swapY1 + t * swapAnim.swapY2;
            visualX[swapAnim.swapY2][swapAnim.swapX2] = (1 - t) * swapAnim.swapX2 + t * swapAnim.swapX1;
            visualY[swapAnim.swapY2][swapAnim.swapX2] = (1 - t) * swapAnim.swapY2 + t * swapAnim.swapY1;
        }
    }
}

void UpdateAfterGame()
{
    if (winState == WIN)
    {
        ClearBackground(DARKGREEN);

        // Drawing buttons
        DrawRectangleRec(afterGameUI.menuButton, LIGHTGRAY);

        // Son seviye de bittiyse, sıradaki seviye butonunu gösterme
        if (gameStats.level <= 5)
        {
            DrawRectangleRec(afterGameUI.nextLevelButton, DARKPURPLE);
            DrawText("Next Level!", afterGameUI.nextLevelButton.x + 53, afterGameUI.nextLevelButton.y + 22, 28, LIGHTGRAY);
        }

        // Drawing labels
        DrawText("YOU WIN!", SCREEN_WIDTH / 5 + 15, SCREEN_HEIGHT / 5, 100, RAYWHITE);
        DrawText("Return to the main menu", afterGameUI.menuButton.x + 17, afterGameUI.menuButton.y + 33, 30, DARKPURPLE);

        // Button controls
        ButtonClick(afterGameUI.menuButton, 0, 0, 0, -1);
        switch (gameStats.level)
        {
        case 1:
            ButtonClick(afterGameUI.nextLevelButton, 2, 0, 0, 0);
            break;
        case 2:
            ButtonClick(afterGameUI.nextLevelButton, 2, 0, 0, 1);
            break;
        case 3:
            ButtonClick(afterGameUI.nextLevelButton, 2, 0, 0, 2);
            break;
        case 4:
            ButtonClick(afterGameUI.nextLevelButton, 2, 0, 0, 3);
            break;
        case 5:
            ButtonClick(afterGameUI.nextLevelButton, 2, 0, 0, 4);
            break;
        }
    }
    else if (winState == LOSE)
    {
		ClearBackground(RED);

		// Drawing buttons
        DrawRectangleRec(afterGameUI.menuButton, LIGHTGRAY);
        DrawRectangleRec(afterGameUI.restartButton, DARKPURPLE);

		// Drawing labels
        DrawText("GAME OVER!", SCREEN_WIDTH / 8 - 10, SCREEN_HEIGHT / 5, 100, RAYWHITE);
        DrawText("Return to the main menu", afterGameUI.menuButton.x+17, afterGameUI.menuButton.y+33, 30, DARKPURPLE);
        DrawText("Try again!", afterGameUI.restartButton.x+57, afterGameUI.restartButton.y+22, 28, LIGHTGRAY);

        // Button controls
        ButtonClick(afterGameUI.menuButton, 0, 0, 0, -1);
        ButtonClick(afterGameUI.restartButton, 2, 0, 0, -1);
    }
}

void UpdateCompleted()
{
    ClearBackground(DARKPURPLE);
    DrawRectangleRec(completedUI.menuButton, LIGHTGRAY);
    DrawTextureEx(completedUI.final, (Vector2) { 180, 275 }, -13.0f, 0.4f, WHITE);
    DrawText("Congratulations!", 160, 130, 60, WHITE);
    DrawText("The game is completed!", 230, 200, 30, WHITE);
    DrawText("Return to the main menu.", completedUI.menuButton.x+20, completedUI.menuButton.y+15, 20, DARKPURPLE);
    ButtonClick(completedUI.menuButton, 0, 0, 0, -1);
}

void Unload()
{
    UnloadTexture(menuUI.wallpaper);

    for (int i = 0; i < CANDY_TYPES; i++)
    {
        UnloadTexture(candyTextures[i]);
    }

	UnloadSound(soundEffects.buttonClick);
	UnloadSound(soundEffects.swapSound);
	UnloadSound(soundEffects.explosionSound);
	UnloadSound(soundEffects.winSound);
	UnloadSound(soundEffects.loseSound);
    UnloadMusicStream(soundEffects.music);
    CloseAudioDevice();
    CloseWindow();
}
