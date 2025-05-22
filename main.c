#include "raylib.h"
#include "stdio.h"
#include "time.h"
#include "stdlib.h"

// Tahta boyutu, hücre boyutlarý ve ekran boyutlarý
#define GRID_WIDTH 8
#define GRID_HEIGHT 8
#define CELL_SIZE 100
#define UI_HEIGHT 60
#define SCREEN_WIDTH (GRID_WIDTH * CELL_SIZE)
#define SCREEN_HEIGHT ((GRID_HEIGHT * CELL_SIZE) + UI_HEIGHT)

// Þeker türleri ve tanýmlanmasý
#define CANDY_TYPES 5
Texture2D candyTextures[CANDY_TYPES];

// Oyun tahtasý tanýmlanmasý
int grid[GRID_HEIGHT][GRID_WIDTH];

// Sürükleme baþlangýç koordinatlarý
int dragStartX = -1, dragStartY = -1;

// Oyun durumlarý ve ses ayarlarý
typedef enum { BEFOREPLAY, LEVELSELECTION, PLAYING, AFTERPLAY } GameState; GameState gameState = 0;
typedef enum { WIN, LOSE } WinState; WinState winState = LOSE;
typedef enum { MUSIC_ON, MUSIC_OFF } MusicState; MusicState musicState = MUSIC_ON;
typedef enum { SOUND_ON, SOUND_OFF } SoundState; SoundState soundState = SOUND_ON;

// Oyun istatistikleri bileþenleri
typedef struct
{
    int level;
    int score;
    int targetScore;
    int movesLeft;
} GameStats; GameStats gameStats;

// Anamenü bileþenleri
typedef struct
{
    Rectangle playButton;
    Rectangle musicButton;
    Rectangle soundButton;
    Texture2D wallpaper;
} MenuUI; MenuUI menuUI;

// Oyun sonrasý menüsü bileþenleri
typedef struct
{
    Rectangle restartButton;
    Rectangle menuButton;
    Rectangle nextLevelButton;
} AfterGameUI; AfterGameUI afterGameUI;

// Seviye seçimi bileþenleri
typedef struct
{
    Rectangle LevelButton[5];
    int TargetScore[5];
} Level; Level level;

// Ses efektleri bileþenleri
typedef struct
{
    Music music;
    Sound buttonClick;
    Sound swapSound;
    Sound explosionSound;
    Sound winSound;
    Sound loseSound;
} SoundEffects; SoundEffects soundEffects;

// Function prototypes
void InitAndLoad();
void UpdateMenu();
void UpdateLevelSelection();
void UpdateGameplay();
void CheckMatches();
void DropCandies();
void UpdateAfterGame();
void Unload();
void ButtonClick(Rectangle, int, bool, bool, int);
void Dragging();
void UpdateMusic();
void UpdateInGameUI();
void UpdateGrid();
void UpdateResult();

int main(void)
{
    SetTargetFPS(60);
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Underwater Dream");
	InitAudioDevice();

	// Initializing requirements and loading textures
    InitAndLoad();

    while (!WindowShouldClose())
    {
		BeginDrawing();

        // Updating the screen depending on gameState
        switch (gameState)
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
        }
        EndDrawing();
	}
	// Oyun döngüsü sona erdiðinde kaynaklarý serbest býrakma
	Unload();

	CloseAudioDevice();
    CloseWindow();
    return 0;
}

void InitAndLoad()
{
        // UI COMPONENTS
    // menuUI
    menuUI.playButton = (Rectangle){ 300,300,200,50 };
    menuUI.musicButton = (Rectangle){ 325,410,150,50 };
    menuUI.soundButton = (Rectangle){ 325,490,150,50 };
    menuUI.wallpaper = LoadTexture("assets/images/wallpaper.png");
    // levelUI
    level.LevelButton[0] = (Rectangle){ 300, 275, 200, 50 };
    level.LevelButton[1] = (Rectangle){ 300, 375, 200, 50 };
    level.LevelButton[2] = (Rectangle){ 300, 475, 200, 50 };
    level.LevelButton[3] = (Rectangle){ 300, 575, 200, 50 };
    level.LevelButton[4] = (Rectangle){ 300, 675, 200, 50 };
    // gameUI
    candyTextures[0] = LoadTexture("assets/images/image1.png");
    candyTextures[1] = LoadTexture("assets/images/image2.png");
    candyTextures[2] = LoadTexture("assets/images/image3.png");
    candyTextures[3] = LoadTexture("assets/images/image4.png");
    candyTextures[4] = LoadTexture("assets/images/image5.png");
    // afterGameUI
    afterGameUI.menuButton = (Rectangle){ 200,400,400,100 };
    afterGameUI.restartButton = (Rectangle){ 275,600,250,75 };
    afterGameUI.nextLevelButton = (Rectangle){ 275,600,250,75 };

        // MUSIC AND SOUND EFFECTS
    // Background music
    soundEffects.music = LoadMusicStream("assets/soundEffects/music.mp3");
    PlayMusicStream(soundEffects.music);
    SetMusicVolume(soundEffects.music, 0.2f);
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
        level.TargetScore[n] = tempScore;
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
                );
            grid[y][x] = candy;
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

    // Drawing buttons and their labels
    char levelNum[32] = {0};
    int tempLevel = 1;
    for (int i = 0; i <= 4; i++)
    {
        DrawRectangleRec(level.LevelButton[i], LIGHTGRAY);
        snprintf(levelNum, sizeof(levelNum), "Level %d", tempLevel);
        DrawText(levelNum, level.LevelButton[i].x + 60, level.LevelButton[i].y + 13, 25, DARKPURPLE);
        tempLevel += 1;
    }

    // Button controls
    for (int i = 0; i <= 4; i++)
    {
        ButtonClick(level.LevelButton[i], 2, 0, 0, i);
    }
}

void UpdateGameplay()
{
    UpdateMusic();
    ClearBackground(RAYWHITE);
    UpdateInGameUI();
    UpdateGrid();
    Dragging();
	CheckMatches();
    UpdateResult();
}

void CheckMatches()
{
    bool matchFound;
    do {
        matchFound = false;

        // Yatay eþleþmelerin bulunmasý
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

        // Dikey eþleþmelerin bulunmasý
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
        // Eþleþme bulunduðunda þekerleri düþürme
        if (matchFound)
        {
            if (soundState == SOUND_ON) PlaySound(soundEffects.explosionSound);
            DropCandies();
        }
    } while (matchFound);
}

void DropCandies() {
    for (int x = 0; x < GRID_WIDTH; x++)
    {
        for (int y = GRID_HEIGHT - 1; y >= 0; y--)
        {
            if (grid[y][x] == -1)
            {
				// Üstteki þekerleri bul ve aþaðý kaydýr
                for (int k = y - 1; k >= 0; k--)
                {
                    if (grid[k][x] != -1)
                    {
						// Þekerin yeni konumunu ayarla
                        grid[y][x] = grid[k][x];
                        grid[k][x] = -1;
                        break;
                    }
                }
            }
        }
    }

	// Yeni þekerleri rastgele oluþtur
    for (int y = 0; y < GRID_HEIGHT; y++)
    {
        for (int x = 0; x < GRID_WIDTH; x++)
        {
			// Eðer hücre boþsa, yeni bir þeker oluþtur
            if (grid[y][x] == -1)
            {
                grid[y][x] = GetRandomValue(0, CANDY_TYPES - 1);
            }
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

        // Son seviye de bittiyse, sýradaki seviye butonunu gösterme
        if (gameStats.level <= 5)
        {
            DrawRectangleRec(afterGameUI.nextLevelButton, DARKPURPLE);
            DrawText("Next Level!", afterGameUI.nextLevelButton.x + 57, afterGameUI.nextLevelButton.y + 22, 28, LIGHTGRAY);
        }

        // Drawing labels
        DrawText("YOU WIN!", SCREEN_WIDTH / 5 + 15, SCREEN_HEIGHT / 5, 100, RAYWHITE);
        DrawText(" Return to the main menu", afterGameUI.menuButton.x + 5, afterGameUI.menuButton.y + 33, 30, DARKPURPLE);

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
        DrawText("Return to the main menu", afterGameUI.menuButton.x+5, afterGameUI.menuButton.y+33, 30, DARKPURPLE);
        DrawText("Try again!", afterGameUI.restartButton.x+57, afterGameUI.restartButton.y+22, 28, LIGHTGRAY);

        // Button controls
        ButtonClick(afterGameUI.menuButton, 0, 0, 0, -1);
        ButtonClick(afterGameUI.restartButton, 2, 0, 0, -1);
    }
}

void Unload()
{
	// Kaynaklarý serbest býrakma
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
                    gameStats.targetScore = level.TargetScore[targetS];
                    
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

void UpdateMusic(void)
{
    if (musicState == MUSIC_ON) UpdateMusicStream(soundEffects.music);
}

void Dragging()
{
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
                if (soundState == SOUND_ON) PlaySound(soundEffects.swapSound);
            }

            // Temizle
            dragStartX = dragStartY = -1;
        }
    }
}

void UpdateInGameUI()
{
    char info[128];
    snprintf(info, sizeof(info), "Points: %d / %d    Moves Left: %d", gameStats.score, gameStats.targetScore, gameStats.movesLeft);
    DrawText(info, 125, 20, 30, DARKPURPLE);
}

void UpdateGrid()
{
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
}

void UpdateResult()
{
    // Sonuç kontrolü
    if (gameStats.score >= gameStats.targetScore)
    {
        winState = WIN;
        if (soundState == SOUND_ON) PlaySound(soundEffects.winSound);
        gameStats.level += 1;
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
    }
}