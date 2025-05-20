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
typedef enum { BEFOREPLAY, PLAYING, AFTERPLAY } GameState; GameState gameState = BEFOREPLAY;
typedef enum { WIN, LOSE } WinState; WinState winState = LOSE;
typedef enum { MUSIC_ON, MUSIC_OFF } MusicState; MusicState musicState = MUSIC_ON;
typedef enum { SOUND_ON, SOUND_OFF } SoundState; SoundState soundState = SOUND_ON;

// Oyun skorlarý bileþenleri
typedef struct
{
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
} AfterGameUI; AfterGameUI afterGameUI;

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

// Fonksiyon Prototipleri
void InitAndLoad(); // Tanýmlama ve yükleme fonksiyonu
void UpdateMenu(); // Anamenü fonksiyonu
void UpdateGameplay(); // Oyun fonksiyonu
void CheckMatches(); // Eþleþmeleri kontrol etme fonksiyonu
void DropCandies(); // Þeker düþürme fonksiyonu
void UpdateAfterGame(); // Oyun sonrasý fonksiyonu
void ResetGameStats(); // Oyun istatistiklerini sýfýrlama fonksiyonu
void Unload(); // Kaynaklarý serbest býrakma fonksiyonu

int main(void)
{
	// Rastgele sayý üreteci için tohumlama
    srand(time(NULL));

	// FPS sabitleme ayarý
    SetTargetFPS(60);

    // Pencere ve ses cihazýný tanýmlama
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Underwater Dream");
	InitAudioDevice();

	// Gerekli nesneleri tanýmlama ve yükleme
    InitAndLoad();

	// Oyun döngüsü
    while (!WindowShouldClose())
    {
		BeginDrawing();

        // Oyun durumuna göre ekraný güncelleme
        switch (gameState)
        {
        case BEFOREPLAY:
            UpdateMenu();
            break;
        case PLAYING:
			UpdateGameplay();
			// Skora göre oyunu bitirme kontrolü
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

	// Oyun döngüsü sona erdiðinde kaynaklarý serbest býrakma
	Unload();

	// Oyun döngüsü sona erdiðinde ses cihazýný kapatma ve pencereyi kapatma
	CloseAudioDevice();
    CloseWindow();
    return 0;
}

void InitAndLoad()
{
	// MenuUI bileþenlerinin tanýmlanmasý ve yüklenmesi
    menuUI.playButton = (Rectangle){ 300,300,200,50 };
    menuUI.musicButton = (Rectangle){ 325,410,150,50 };
    menuUI.soundButton = (Rectangle){ 325,490,150,50 };
    menuUI.wallpaper = LoadTexture("assets/images/wallpaper.png");

	// Arkaplan müziðinin yüklenmesi ve ayarlanmasý
    soundEffects.music = LoadMusicStream("assets/soundEffects/music.mp3");
    PlayMusicStream(soundEffects.music);
    SetMusicVolume(soundEffects.music, 0.2f);

	// Ses efektlerinin yüklenmesi ve ayarlanmasý
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

	// Þekerlerin yüklenmesi
    candyTextures[0] = LoadTexture("assets/images/image1.png");
    candyTextures[1] = LoadTexture("assets/images/image2.png");
    candyTextures[2] = LoadTexture("assets/images/image3.png");
    candyTextures[3] = LoadTexture("assets/images/image4.png");
    candyTextures[4] = LoadTexture("assets/images/image5.png");

	// Oyun skorlarýnýn tanýmlanmasý
    gameStats.score = 0;
    gameStats.targetScore = 1500;
    gameStats.movesLeft = 20;

	// Oyun tahtasýnýn baþlatýlmasý
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

	// Oyun sonrasý menüsü bileþenlerinin tanýmlanmasý
    afterGameUI.menuButton = (Rectangle){ 200,400,400,100 };
    afterGameUI.restartButton = (Rectangle){ 275,600,250,75 };
}

void UpdateMenu()
{
	// Arkaplan müziðinin çalýnmasý
    if (musicState == MUSIC_ON) UpdateMusicStream(soundEffects.music);

    // Arka planýn çizilmesi
    DrawTexture(menuUI.wallpaper, 0, 0, WHITE);
    
    // Butonlarýn çizilmesi
    DrawRectangleRec(menuUI.playButton, DARKPURPLE);
    DrawRectangleRec(menuUI.musicButton, DARKBLUE);
    DrawRectangleRec(menuUI.soundButton, DARKBLUE);

	// Baþlýklarýn çizilmesi
    DrawText("Underwater Dream", 200, 120, 42, DARKPURPLE);
    DrawText("Similar to Candy Crush Saga. Coded with Raylib.", 150, 170, 21, BLACK);
    DrawText("Start!", menuUI.playButton.x+65, menuUI.playButton.y+15, 25, WHITE);
    DrawText("Music", menuUI.musicButton.x+20, menuUI.musicButton.y+17, 18, WHITE);
    DrawText("Sound", menuUI.soundButton.x+20, menuUI.soundButton.y+17, 18, WHITE);

    if (musicState == MUSIC_ON) DrawText("ON", menuUI.musicButton.x + 100, menuUI.musicButton.y + 17, 18, GREEN);
    else if (musicState == MUSIC_OFF) DrawText("OFF", menuUI.musicButton.x + 100, menuUI.musicButton.y + 17, 18, RED);
    if (soundState == SOUND_ON) DrawText("ON", menuUI.soundButton.x + 100, menuUI.soundButton.y + 17, 18, GREEN);
    else if (soundState == SOUND_OFF) DrawText("OFF", menuUI.soundButton.x + 100, menuUI.soundButton.y + 17, 18, RED);

	// Buton týklama kontrolü
    Vector2 mousePos = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
		// Oynama butonuna týklama kontrolü
        if (CheckCollisionPointRec(mousePos, menuUI.playButton))
        {
            if (soundState == SOUND_ON)
            {
                PlaySound(soundEffects.buttonClick);
            }
            gameState = PLAYING;
        }
		// Müzik butonuna týklama kontrolü
        if (CheckCollisionPointRec(mousePos, menuUI.musicButton))
        {
            if (soundState == SOUND_ON) PlaySound(soundEffects.buttonClick);
            if (musicState == MUSIC_ON) musicState = MUSIC_OFF;
            else if (musicState == MUSIC_OFF) musicState = MUSIC_ON;
        }
		// Ses butonuna týklama kontrolü
        if (CheckCollisionPointRec(mousePos, menuUI.soundButton))
        {
            if (soundState == SOUND_ON) soundState = SOUND_OFF;
            else if (soundState == SOUND_OFF)
            {
                soundState = SOUND_ON;
				PlaySound(soundEffects.buttonClick);
            }
        }
    }
}

void UpdateGameplay()
{
	// Arkaplan müziðinin çalýnmasý
    if (musicState == MUSIC_ON) UpdateMusicStream(soundEffects.music);

	// Arka planýn çizilmesi
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
                if (soundState == SOUND_ON) PlaySound(soundEffects.swapSound);
            }

            // Temizle
            dragStartX = dragStartY = -1;
        }
    }


    // Üst bilgi çubuðu
    char info[128];
    snprintf(info, sizeof(info), "Points: %d / %d    Moves Left: %d", gameStats.score, gameStats.targetScore, gameStats.movesLeft);
    DrawText(info, 125, 20, 30, DARKPURPLE);

    // Oyun tahtasýnýn çizilmesi
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

	// Eþleþmeleri kontrol etme
	CheckMatches();

	// Sonuç kontrolü
    if (gameStats.score >= gameStats.targetScore)
    {
        winState = WIN;
		if (soundState == SOUND_ON) PlaySound(soundEffects.winSound);
    }
    else if (gameStats.movesLeft <= 0)
    {
        winState = LOSE;
        if (soundState == SOUND_ON) PlaySound(soundEffects.loseSound);
    }
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
    // Oyun kazanýldýysa
    if (winState == WIN)
    {
		// Arka planýn çizilmesi
        ClearBackground(DARKGREEN);

		// Butonun çizilmesi
        DrawRectangleRec(afterGameUI.menuButton, LIGHTGRAY);

		// Baþlýklarýn çizilmesi
        DrawText("YOU WIN!", SCREEN_WIDTH / 5+15, SCREEN_HEIGHT / 5, 100, RAYWHITE);
        DrawText(" Return to the main menu", afterGameUI.menuButton.x+5, afterGameUI.menuButton.y+33, 30, DARKPURPLE);
    }

	// Oyun kaybedildiyse
    else if (winState == LOSE)
    {
		// Arka planýn çizilmesi
		ClearBackground(RED);

		// Butonlarýn çizilmesi
        DrawRectangleRec(afterGameUI.menuButton, LIGHTGRAY);
        DrawRectangleRec(afterGameUI.restartButton, DARKPURPLE);

		// Baþlýklarýn çizilmesi
        DrawText("GAME OVER!", SCREEN_WIDTH / 8 - 10, SCREEN_HEIGHT / 5, 100, RAYWHITE);
        DrawText(" Return to the main menu", afterGameUI.menuButton.x+5, afterGameUI.menuButton.y+33, 30, DARKPURPLE);
        DrawText("Try again!", afterGameUI.restartButton.x+57, afterGameUI.restartButton.y+22, 28, LIGHTGRAY);
    }

	// Oyun sonrasý skorlarýn sýfýrlanmasý
    ResetGameStats();

    // Butonlara týklama kontrolü
    Vector2 mousePos = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        if (CheckCollisionPointRec(mousePos, afterGameUI.menuButton))
        {
			if (soundState == SOUND_ON) PlaySound(soundEffects.buttonClick);
            gameState = BEFOREPLAY;
        }
        if (CheckCollisionPointRec(mousePos, afterGameUI.restartButton))
        {
			if (soundState == SOUND_ON) PlaySound(soundEffects.buttonClick);
            gameState = PLAYING;
        }
    }
}

void ResetGameStats()
{
    gameStats.movesLeft = 20;
    gameStats.score = 0;
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