#include "raylib.h"
#include "stdio.h"
#include "time.h"
#include "stdlib.h"

#define GRID_WIDTH 8
#define GRID_HEIGHT 8
#define CELL_SIZE 100
#define UI_HEIGHT 60
#define SCREEN_WIDTH (GRID_WIDTH * CELL_SIZE)
#define SCREEN_HEIGHT ((GRID_HEIGHT * CELL_SIZE) + UI_HEIGHT)

#define CANDY_TYPES 5
Texture2D candyTextures[CANDY_TYPES];

int grid[GRID_HEIGHT][GRID_WIDTH];
int dragStartX = -1, dragStartY = -1;

typedef enum { BEFOREPLAY, PLAYING, AFTERPLAY } GameState; GameState gameState = BEFOREPLAY;
typedef enum { MUSIC_ON, MUSIC_OFF } MusicState; MusicState musicState = MUSIC_ON;
typedef enum { SOUND_ON, SOUND_OFF } SoundState; SoundState soundState = SOUND_ON;
typedef enum { WIN, LOSE } WinState; WinState winState = LOSE;

typedef struct
{
    int score;
    int targetScore;
    int movesLeft;
} GameStats; GameStats gameStats;
typedef struct
{
    Rectangle playButton;
    Rectangle musicButton;
    Rectangle soundButton;
    Texture2D wallpaper;
} MenuUI; MenuUI menuUI;
typedef struct
{
    Rectangle restartButton;
    Rectangle menuButton;
} AfterGameUI; AfterGameUI afterGameUI;

void DropCandies(void);

// Yeni fonksiyonlar
void LoadTextures(void);
void UpdateMenu(void);
void UpdateGameplay(void);
void UpdateAfterGame(void);
void resetGameStats(void);
void UnloadTextures(void);

void main(void)
{
    srand(time(NULL));
    SetTargetFPS(60);

    // Pencere ve ses cihazý
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Underwater Dream");
	InitAudioDevice();

    // Arka plan müziði
	Music music = LoadMusicStream("assets/music.mp3");
	PlayMusicStream(music);
	SetMusicVolume(music, 0.2f);

    LoadTextures();

    while (!WindowShouldClose())
    {
        if (musicState == MUSIC_ON) UpdateMusicStream(music);
		BeginDrawing();
        switch (gameState)
        {
        case BEFOREPLAY:
            UpdateMenu();
            break;
        case PLAYING:
			UpdateGameplay();
            if (gameStats.movesLeft <= 0 || gameStats.score >= gameStats.targetScore)
            {
                gameState = AFTERPLAY;
			}
            break;
        case AFTERPLAY:
			UpdateAfterGame();
            break;
        }
        EndDrawing();
	}
	UnloadTextures();
    CloseWindow();
}

void LoadTextures(void)
{
    // Menü
    menuUI.playButton = (Rectangle){ 300,300,200,50 };
    menuUI.musicButton = (Rectangle){ 325,410,150,50 };
    menuUI.soundButton = (Rectangle){ 325,490,150,50 };
    menuUI.wallpaper = LoadTexture("assets/wallpaper.png");

    // Oyun
    candyTextures[0] = LoadTexture("assets/1.png");
    candyTextures[1] = LoadTexture("assets/2.png");
    candyTextures[2] = LoadTexture("assets/3.png");
    candyTextures[3] = LoadTexture("assets/4.png");
    candyTextures[4] = LoadTexture("assets/5.png");

    gameStats.score = 0;
    gameStats.targetScore = 1500;
    gameStats.movesLeft = 20;

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
                );
            grid[y][x] = candy;
        }
    }

    // Oyun Sonrasý
    afterGameUI.menuButton = (Rectangle){ 200,400,400,100 };
    afterGameUI.restartButton = (Rectangle){ 275,600,250,75 };
}

void UpdateMenu(void)
{
    // Arka planýn çizilmesi
    DrawTexture(menuUI.wallpaper, 0, 0, WHITE);

    // Butonlara týklama kontrolü
    Vector2 mousePos = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        if (CheckCollisionPointRec(mousePos, menuUI.playButton))
        {
            gameState = PLAYING;
        }
    }
    // Baþlýk
    DrawText("Underwater Dream", 200, 120, 42, DARKPURPLE);
    DrawText("Similar to Candy Crush Saga. Coded with Raylib.", 150, 170, 21, BLACK);

    // Butonlarýn çizilmesi
    DrawRectangleRec(menuUI.playButton, DARKPURPLE);
    DrawRectangleRec(menuUI.musicButton, DARKBLUE);
    DrawRectangleRec(menuUI.soundButton, DARKBLUE);

    // Baþlýklarýn çizilmesi
    DrawText("Start!", menuUI.playButton.x+65, menuUI.playButton.y+15, 25, WHITE);
    DrawText("Music", menuUI.musicButton.x+20, menuUI.musicButton.y+17, 18, WHITE);
    DrawText("Sound", menuUI.soundButton.x+20, menuUI.soundButton.y+17, 18, WHITE);

    // Müzik ve ses durumlarýnýn kontrolü
    if (musicState == MUSIC_ON) DrawText("ON", menuUI.musicButton.x+100, menuUI.musicButton.y+17, 18, GREEN);
    else if (musicState == MUSIC_OFF) DrawText("OFF", menuUI.musicButton.x+100, menuUI.musicButton.y+17, 18, RED);
    if (soundState == SOUND_ON) DrawText("ON", menuUI.soundButton.x+100, menuUI.soundButton.y+17, 18, GREEN);
    else if (soundState == SOUND_OFF) DrawText("OFF", menuUI.soundButton.x+100, menuUI.soundButton.y+17, 18, RED);
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        if (CheckCollisionPointRec(mousePos, menuUI.musicButton))
        {
            if (musicState == MUSIC_ON) musicState = MUSIC_OFF;
            else if (musicState == MUSIC_OFF) musicState = MUSIC_ON;
        }
        if (CheckCollisionPointRec(mousePos, menuUI.soundButton))
        {
            if (soundState == SOUND_ON) soundState = SOUND_OFF;
            else if (soundState == SOUND_OFF) soundState = SOUND_ON;
        }
    }
}

void UpdateGameplay(void)
{
    ClearBackground(RAYWHITE);

    // Sürükleme mekanizmasý
    Vector2 mousePos = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (mousePos.y >= UI_HEIGHT) {
            dragStartX = mousePos.x / CELL_SIZE;
            dragStartY = (mousePos.y - UI_HEIGHT) / CELL_SIZE;
        }
    }
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        if (dragStartX != -1 && dragStartY != -1) {
            int dropX = mousePos.x / CELL_SIZE;
            int dropY = (mousePos.y - UI_HEIGHT) / CELL_SIZE;

            // Komþuluk kontrolü
            if ((abs(dragStartX - dropX) == 1 && dragStartY == dropY) ||
                (abs(dragStartY - dropY) == 1 && dragStartX == dropX)) {

                // Þekerleri deðiþtir
                int temp = grid[dragStartY][dragStartX];
                grid[dragStartY][dragStartX] = grid[dropY][dropX];
                grid[dropY][dropX] = temp;

                gameStats.movesLeft--;
            }

            // Temizle
            dragStartX = dragStartY = -1;
        }
    }


    // Üst bilgi çubuðu
    char info[128];
    snprintf(info, sizeof(info), "Points: %d / %d    Moves Left: %d", gameStats.score, gameStats.targetScore, gameStats.movesLeft);
    DrawText(info, 125, 20, 30, DARKPURPLE);

    // Oyun tahtasýný çiz
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            Rectangle cell = { x * CELL_SIZE, y * CELL_SIZE + UI_HEIGHT, CELL_SIZE, CELL_SIZE };
            Texture2D tex = candyTextures[grid[y][x]];
            Rectangle source = { 0, 0, (float)tex.width, (float)tex.height };
            Rectangle dest = { x * CELL_SIZE, y * CELL_SIZE + UI_HEIGHT, CELL_SIZE, CELL_SIZE };
            Vector2 origin = { 0, 0 };

            DrawTexturePro(tex, source, dest, origin, 0.0f, WHITE);
            DrawRectangleLinesEx(cell, 1, DARKPURPLE);
        }
    }

    bool matchFound;
    do {
        matchFound = false;

        // Yatay eþleþmeler
        for (int y = 0; y < GRID_HEIGHT; y++) {
            for (int x = 0; x < GRID_WIDTH - 4; x++) {
                int candy = grid[y][x];
                if (candy != -1 &&
                    candy == grid[y][x + 1] &&
                    candy == grid[y][x + 2] &&
                    candy == grid[y][x + 3] &&
                    candy == grid[y][x + 4]) {
                    grid[y][x] = grid[y][x + 1] = grid[y][x + 2] = grid[y][x + 3] = grid[y][x + 4] = -1;
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
                    grid[y][x] = grid[y][x + 1] = grid[y][x + 2] = grid[y][x + 3] = -1;
                    matchFound = true;
                    gameStats.score += 150;
                }
            }
            for (int x = 0; x < GRID_WIDTH - 2; x++) {
                int candy = grid[y][x];
                if (candy != -1 &&
                    candy == grid[y][x + 1] &&
                    candy == grid[y][x + 2]) {
                    grid[y][x] = grid[y][x + 1] = grid[y][x + 2] = -1;
                    matchFound = true;
                    gameStats.score += 50;
                }
            }
        }

        // Dikey eþleþmeler
        for (int x = 0; x < GRID_WIDTH; x++) {
            for (int y = 0; y < GRID_HEIGHT - 4; y++) {
                int candy = grid[y][x];
                if (candy != -1 &&
                    candy == grid[y + 1][x] &&
                    candy == grid[y + 2][x] &&
                    candy == grid[y + 3][x] &&
                    candy == grid[y + 4][x]) {
                    grid[y][x] = grid[y + 1][x] = grid[y + 2][x] = grid[y + 3][x] = grid[y + 4][x] = -1;
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
                    grid[y][x] = grid[y + 1][x] = grid[y + 2][x] = grid[y + 3][x] = -1;
                    matchFound = true;
                    gameStats.score += 150;
                }
            }
            for (int y = 0; y < GRID_HEIGHT - 2; y++) {
                int candy = grid[y][x];
                if (candy != -1 &&
                    candy == grid[y + 1][x] &&
                    candy == grid[y + 2][x]) {
                    grid[y][x] = grid[y + 1][x] = grid[y + 2][x] = -1;
                    matchFound = true;
                    gameStats.score += 50;
                }
            }
        }

        if (matchFound)
        {
            DropCandies();
        }
    } while (matchFound);

    // Sonucu
    if (gameStats.score >= gameStats.targetScore)
    {
        winState = WIN;
    }
}

void UpdateAfterGame(void)
{
    if (winState == WIN)
    {
        ClearBackground(DARKGREEN);
        DrawRectangleRec(afterGameUI.menuButton, LIGHTGRAY);

        DrawText("YOU WIN!", SCREEN_WIDTH / 5+15, SCREEN_HEIGHT / 5, 100, RAYWHITE);
        DrawText(" Return to the main menu", afterGameUI.menuButton.x+5, afterGameUI.menuButton.y+33, 30, DARKPURPLE);
    }

    else if (winState == LOSE)
    {
		ClearBackground(RED);
        DrawRectangleRec(afterGameUI.menuButton, LIGHTGRAY);
        DrawRectangleRec(afterGameUI.restartButton, DARKPURPLE);

        DrawText("GAME OVER!", SCREEN_WIDTH / 8 - 10, SCREEN_HEIGHT / 5, 100, RAYWHITE);
        DrawText(" Return to the main menu", afterGameUI.menuButton.x+5, afterGameUI.menuButton.y+33, 30, DARKPURPLE);
        DrawText("Try again!", afterGameUI.restartButton.x+57, afterGameUI.restartButton.y+22, 28, LIGHTGRAY);
    }

    // Butonlara týklama kontrolü
    Vector2 mousePos = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        if (CheckCollisionPointRec(mousePos, afterGameUI.menuButton))
        {
            gameState = BEFOREPLAY;
            resetGameStats();
        }
        if (CheckCollisionPointRec(mousePos, afterGameUI.restartButton))
        {
            gameState = PLAYING;
            resetGameStats();
        }
    }
}

void resetGameStats(void)
{
    gameStats.movesLeft = 20;
    gameStats.score = 0;
    winState = LOSE;
}

void UnloadTextures(void)
{
    UnloadTexture(menuUI.wallpaper);

    for (int i = 0; i < CANDY_TYPES; i++)
    {
        UnloadTexture(candyTextures[i]);
    }
}

void DropCandies(void) {
    for (int x = 0; x < GRID_WIDTH; x++)
    {
        for (int y = GRID_HEIGHT - 1; y >= 0; y--)
        {
            if (grid[y][x] == -1)
            {
                for (int k = y - 1; k >= 0; k--)
                {
                    if (grid[k][x] != -1)
                    {
                        grid[y][x] = grid[k][x];
                        grid[k][x] = -1;
                        break;
                    }
                }
            }
        }
    }

    for (int y = 0; y < GRID_HEIGHT; y++)
    {
        for (int x = 0; x < GRID_WIDTH; x++)
        {
            if (grid[y][x] == -1)
            {
                grid[y][x] = GetRandomValue(0, CANDY_TYPES - 1);
            }
        }
    }
}