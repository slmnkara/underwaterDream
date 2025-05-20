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

// Se�ilen h�cre i�in ba�lang�� de�eri
Vector2 selected = { -1, -1 };

void DropCandies(void);

// Yeni fonksiyonlar
void LoadTextures(void);
void UpdateMenu(void);
void UpdateGameplay(void);
void UpdateAfterGame(void);
void UnloadTextures(void);

void main(void)
{
    srand(time(NULL));
    SetTargetFPS(60);
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Underwater Dream");
    LoadTextures();
    while (!WindowShouldClose())
    {
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
    // Men�
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

    // Oyun Sonras�
    afterGameUI.menuButton = (Rectangle){ 200,400,400,100 };
    afterGameUI.restartButton = (Rectangle){ 275,600,250,75 };
}

void UpdateMenu(void)
{
    // Arka plan�n �izilmesi
    DrawTexture(menuUI.wallpaper, 0, 0, WHITE);

    // Butonlara t�klama kontrol�
    Vector2 mousePos = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        if (CheckCollisionPointRec(mousePos, menuUI.playButton))
        {
            gameState = PLAYING;
        }
    }
    // Ba�l�k
    DrawText("Underwater Dream", 200, 120, 42, DARKPURPLE);
    DrawText("Similar to Candy Crush Saga. Coded with Raylib.", 150, 170, 21, BLACK);

    // Butonlar�n �izilmesi
    DrawRectangleRec(menuUI.playButton, DARKPURPLE);
    DrawRectangleRec(menuUI.musicButton, DARKBLUE);
    DrawRectangleRec(menuUI.soundButton, DARKBLUE);

    // Ba�l�klar�n �izilmesi
    DrawText("Start!", menuUI.playButton.x+65, menuUI.playButton.y+15, 25, WHITE);
    DrawText("Music", menuUI.musicButton.x+20, menuUI.musicButton.y+17, 18, WHITE);
    DrawText("Sound", menuUI.soundButton.x+20, menuUI.soundButton.y+17, 18, WHITE);

    // M�zik ve ses durumlar�n�n kontrol�
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
    // Fare t�klama kontrol�
    Vector2 mousePos = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        if (mousePos.y >= UI_HEIGHT) {
            int col = mousePos.x / CELL_SIZE;
            int row = (mousePos.y - UI_HEIGHT) / CELL_SIZE;

            if (selected.x == -1) {
                selected.x = col;
                selected.y = row;
            }
            else {
                if ((selected.x == col) && (selected.y == row)) {
                    selected.x = -1;
                    selected.y = -1;
                }
                else if ((selected.x == col && abs((int)row - (int)selected.y) == 1) ||
                    (selected.y == row && abs((int)col - (int)selected.x) == 1)) {
                    int temp = grid[row][col];
                    grid[row][col] = grid[(int)selected.y][(int)selected.x];
                    grid[(int)selected.y][(int)selected.x] = temp;

                    selected.x = -1;
                    selected.y = -1;

                    gameStats.movesLeft--;
                }
            }
        }
    }

    // �st bilgi �ubu�u
    char info[128];
    snprintf(info, sizeof(info), "Points: %d / %d    Moves Left: %d", gameStats.score, gameStats.targetScore, gameStats.movesLeft);
    DrawText(info, 125, 20, 30, DARKPURPLE);

    // Oyun tahtas�n� �iz
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            Rectangle cell = { x * CELL_SIZE, y * CELL_SIZE + UI_HEIGHT, CELL_SIZE, CELL_SIZE };
            Texture2D tex = candyTextures[grid[y][x]];
            Rectangle source = { 0, 0, (float)tex.width, (float)tex.height };
            Rectangle dest = { x * CELL_SIZE, y * CELL_SIZE + UI_HEIGHT, CELL_SIZE, CELL_SIZE };
            Vector2 origin = { 0, 0 };

            DrawTexturePro(tex, source, dest, origin, 0.0f, WHITE);
            DrawRectangleLinesEx(cell, 1, DARKPURPLE);

            if (selected.x == x && selected.y == y) {
                DrawRectangleLinesEx(cell, 3, BLUE);
            }
        }
    }

    bool matchFound;
    do {
        matchFound = false;

        // Yatay e�le�meler
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

        // Dikey e�le�meler
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
    if (gameStats.score >= gameStats.targetScore)
    {
        winState = WIN;
    }
    else if (gameStats.movesLeft <= 0)
    {
        winState = LOSE;
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

    // Butonlara t�klama kontrol�
    Vector2 mousePos = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        if (CheckCollisionPointRec(mousePos, afterGameUI.menuButton))
        {
            gameState = BEFOREPLAY;
        }
        if (CheckCollisionPointRec(mousePos, afterGameUI.restartButton))
        {
            gameState = PLAYING;
        }
    }
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