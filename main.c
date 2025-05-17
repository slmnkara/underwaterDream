#include "raylib.h"
#include "stdio.h"
#include "time.h"
#include "stdlib.h"

// Pencere boyutu ve gridin ayarlanmasý
#define GRID_WIDTH 10
#define GRID_HEIGHT 10
#define CELL_SIZE 60

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

// Fonksiyonlar
void mainMenu(void);
void playMenu(void);
void DropCandies(void);
void InitGrid(void);

int main(void)
{
    // Rastgelelik için
    srand(time(NULL));
    // Oyun penceresini baþlat
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Candy Crush Saga");
    SetTargetFPS(60);
    while (gameState == BEFOREPLAY) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        mainMenu();
        // Butonlara týklama kontrolü
        Vector2 mousePos = GetMousePosition();
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            if (CheckCollisionCircleRec(mousePos, 40, (Rectangle) { 210, 180, 80, 40 })) {
                gameState = PLAYING;
            }
            else if (CheckCollisionCircleRec(mousePos, 40, (Rectangle) { 210, 280, 80, 40 })) {
                gameState = PLAYING;
            }
        }
        EndDrawing();
    }
    while (gameState == PLAYING) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        playMenu();
        EndDrawing();
    }
    CloseWindow();
    return 0;
}

void mainMenu(void)
{
    ClearBackground(GRAY);
    // Ana menü butonlarýný oluþturmak için genel bir struct.
    typedef struct
    {
        int posX;
        int posY;
        float radius;
        Color color;
    } Circle;

    // Ana menü butonlarýnýn tanýmlanmasý
    Circle startButton = { 250,200,60,BLACK };
    Circle musicButton = { 400,450,20,WHITE };
    Circle soundButton = { 450,450,20,WHITE };
    Circle resetButton = { 50,450,30,DARKGRAY };

    // Baþlýk
    DrawText("Welcome to Candy Crush Saga!", 100, 100, 30, BLACK);

    // Butonlarýn çizilmesi ve Label eklenmesi
    DrawCircle(startButton.posX, startButton.posY, startButton.radius, startButton.color);
    DrawText("START", startButton.posX - 28, startButton.posY - 8, 18, WHITE);
    DrawCircle(musicButton.posX, musicButton.posY, musicButton.radius, musicButton.color);
    DrawCircle(soundButton.posX, soundButton.posY, soundButton.radius, soundButton.color);
    DrawCircle(resetButton.posX, resetButton.posY, resetButton.radius, resetButton.color);
}

void playMenu(void)
{
    // Grid fonksiyonunu çaðýr
    InitGrid();
    // Þekerlerin yüklenmesi
    Texture2D candyTextures[CANDY_TYPES];
    candyTextures[0] = LoadTexture("assets/blue.png");
    candyTextures[1] = LoadTexture("assets/red.png");
    candyTextures[2] = LoadTexture("assets/orange.png");
    candyTextures[3] = LoadTexture("assets/purple.png");
    candyTextures[4] = LoadTexture("assets/yellow.png");
    // Hamle, skor ve hedef puan
    int score = 0;
    int targetScore = 1500;
    int movesLeft = 20;

    // Oyun döngüsü
    while (!WindowShouldClose())
    {
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

        BeginDrawing();
        ClearBackground(RAYWHITE);

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

        // Hamle ve puan kontrolü
        if (movesLeft <= 0 && score < targetScore)
        {
            gameState = AFTERPLAY;
            break;
        }
        // Oyun bitiþ kontrolü
        if (gameState == AFTERPLAY) break;
        if (score >= targetScore)
        {
            DrawText("Tebrikler! Hedef puaný geçtiniz!", 100, SCREEN_HEIGHT / 2, 20, GREEN);
            gameState = AFTERPLAY;
            break;
        }
    }
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