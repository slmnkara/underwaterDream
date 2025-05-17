#include "raylib.h"
#include "stdio.h"
#include "time.h"
#include "stdlib.h"

// Pencere boyutu ve gridin ayarlanmasý
#define GRID_WIDTH 8
#define GRID_HEIGHT 8
#define CELL_SIZE 100

// Hamle hakký ve puan için üst bar yüksekliði
#define UI_HEIGHT 40
#define SCREEN_WIDTH (GRID_WIDTH * CELL_SIZE)
#define SCREEN_HEIGHT ((GRID_HEIGHT * CELL_SIZE) + UI_HEIGHT)
int grid[GRID_HEIGHT][GRID_WIDTH];

// Þeker çeþitleri
#define CANDY_TYPES 5

// Seçilen hücre için baþlangýç deðeri
Vector2 selected = { -1, -1 };

// Oyun durumu
typedef enum { BEFOREPLAY, PLAYING, AFTERPLAY } GameState;
GameState gameState = BEFOREPLAY;

// Animasyon durumu
typedef enum { ANIMATING, IDLE } AnimationState;
AnimationState animationState = IDLE;

// Müzik durumu
typedef enum { MUSIC_ON, MUSIC_OFF } MusicState;
MusicState musicState = MUSIC_ON;

// Ses durumu
typedef enum { SOUND_ON, SOUND_OFF } SoundState;
SoundState soundState = SOUND_ON;

// Fonksiyonlar
void mainMenu(void);
void playMenu(void);
void DropCandies(void);
void InitGrid(void);

void main(void)
{
    srand(time(NULL));
    SetTargetFPS(60);
    mainMenu();
}

void mainMenu(void)
{
    InitWindow(600, 640, "Underwater Dream");
    // Ana menü butonlarýný oluþturmak için genel bir struct.
    typedef struct
    {
        Vector2 position;
        float radius;
        Color color;
    } Circle;

    // Ana menü butonlarýnýn tanýmlanmasý
    int start_x = 300;
    int start_y = 275;
    float start_radius = 60;
    Circle startButton = { {300,275},start_radius,BLACK };
    Circle musicButton = { {300,390},40,BLACK };
    Circle soundButton = { {300,480},40,BLACK };

	// Duvar kaðýdý yüklenmesi
    Texture2D wallpaper = LoadTexture("assets/wallpaper.png");

    while (!WindowShouldClose())
    {
		BeginDrawing();
		// Arka planýn çizilmesi
        DrawTexture(wallpaper, 0, 0, WHITE);

        // Butonlara týklama kontrolü
        Vector2 mousePos = GetMousePosition();
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            if (CheckCollisionPointCircle(mousePos, (Vector2) { start_x, start_y }, start_radius))
            {
                gameState = PLAYING;
				break;
            }
        }
        // Baþlýk
        DrawText("Underwater Dream", 100, 100, 42, DARKPURPLE);
        DrawText("Similar to Candy Crush Saga. Coded with Raylib.", 50, 150, 21, BLACK);

        // Butonlarýn çizilmesi
        DrawCircleV(startButton.position, startButton.radius, startButton.color);
        DrawCircleV(musicButton.position, musicButton.radius, musicButton.color);
        DrawCircleV(soundButton.position, soundButton.radius, soundButton.color);

        // Baþlýklarýn çizilmesi
        DrawText("START", startButton.position.x - 42, startButton.position.y - 10, 25, WHITE);
        DrawText("Music", musicButton.position.x - 21, musicButton.position.y - 15, 18, WHITE);
        DrawText("Sound", soundButton.position.x - 25, soundButton.position.y - 15, 18, WHITE);

		// Müzik ve ses durumlarýnýn kontrolü
        if (musicState == MUSIC_ON) DrawText("ON", musicButton.position.x - 10, musicButton.position.y + 5, 18, GREEN);
        else if (musicState == MUSIC_OFF) DrawText("OFF", musicButton.position.x - 15, musicButton.position.y + 5, 18, RED);
        if (soundState == SOUND_ON) DrawText("ON", soundButton.position.x - 10, soundButton.position.y + 5, 18, GREEN);
        else if (soundState == SOUND_OFF) DrawText("OFF", soundButton.position.x - 15, soundButton.position.y + 5, 18, RED);
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            if (CheckCollisionPointCircle(mousePos, musicButton.position, musicButton.radius))
            {
                if (musicState == MUSIC_ON) musicState = MUSIC_OFF;
                else if (musicState == MUSIC_OFF) musicState = MUSIC_ON;
            }
            if (CheckCollisionPointCircle(mousePos, soundButton.position, soundButton.radius))
            {
                if (soundState == SOUND_ON) soundState = SOUND_OFF;
                else if (soundState == SOUND_OFF) soundState = SOUND_ON;
            }
        }
		EndDrawing();
    }
    UnloadTexture(wallpaper);
    CloseWindow();

    if (gameState == PLAYING) playMenu();
}

void playMenu(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Underwater Dream");
    // Grid fonksiyonunu çaðýr
    InitGrid();

    // Þekerlerin yüklenmesi
    Texture2D candyTextures[CANDY_TYPES];
    candyTextures[0] = LoadTexture("assets/1.png");
    candyTextures[1] = LoadTexture("assets/2.png");
    candyTextures[2] = LoadTexture("assets/3.png");
    candyTextures[3] = LoadTexture("assets/4.png");
    candyTextures[4] = LoadTexture("assets/5.png");

    // Hamle, skor ve hedef puan
    int score = 0;
    int targetScore = 1500;
    int movesLeft = 20;

    // Oyun döngüsü
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Fare týklama kontrolü
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

                        movesLeft--;
                    }
                }
            }
        }
        

        // Üst bilgi çubuðu
        char info[128];
        snprintf(info, sizeof(info), "Hamle: %d   Puan: %d / %d", movesLeft, score, targetScore);
        DrawText(info, 10, 10, 20, DARKGRAY);

        // Oyun tahtasýný çiz
        for (int y = 0; y < GRID_HEIGHT; y++) {
            for (int x = 0; x < GRID_WIDTH; x++) {
                Rectangle cell = { x * CELL_SIZE, y * CELL_SIZE + UI_HEIGHT, CELL_SIZE, CELL_SIZE };
                Texture2D tex = candyTextures[grid[y][x]];
                Rectangle source = { 0, 0, (float)tex.width, (float)tex.height };
                Rectangle dest = { x * CELL_SIZE, y * CELL_SIZE + UI_HEIGHT, CELL_SIZE, CELL_SIZE };
                Vector2 origin = { 0, 0 };

                DrawTexturePro(tex, source, dest, origin, 0.0f, WHITE);
                DrawRectangleLinesEx(cell, 1, GRAY);

                if (selected.x == x && selected.y == y) {
                    DrawRectangleLinesEx(cell, 3, BLUE);
                }
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
                        score += 250;
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
                        score += 150;
                    }
                }
                for (int x = 0; x < GRID_WIDTH - 2; x++) {
                    int candy = grid[y][x];
                    if (candy != -1 &&
                        candy == grid[y][x + 1] &&
                        candy == grid[y][x + 2]) {
                        grid[y][x] = grid[y][x + 1] = grid[y][x + 2] = -1;
                        matchFound = true;
                        score += 50;
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
                        score += 250;
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
                        score += 150;
                    }
                }
                for (int y = 0; y < GRID_HEIGHT - 2; y++) {
                    int candy = grid[y][x];
                    if (candy != -1 &&
                        candy == grid[y + 1][x] &&
                        candy == grid[y + 2][x]) {
                        grid[y][x] = grid[y + 1][x] = grid[y + 2][x] = -1;
                        matchFound = true;
                        score += 50;
                    }
                }
            }

            if (matchFound)
            {
                DropCandies();
            }
        } while (matchFound);

        EndDrawing();
    }
    for (int i = 0; i < CANDY_TYPES; i++)
    {
        UnloadTexture(candyTextures[i]);
    }
    CloseWindow();
    if (gameState == 0) mainMenu();
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

void InitGrid(void)
{
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
}