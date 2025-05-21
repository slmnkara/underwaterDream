#include "raylib.h"
#include "stdio.h"
#include "time.h"
#include "stdlib.h"

// Tahta boyutu, h�cre boyutlar� ve ekran boyutlar�
#define GRID_WIDTH 8
#define GRID_HEIGHT 8
#define CELL_SIZE 100
#define UI_HEIGHT 60
#define SCREEN_WIDTH (GRID_WIDTH * CELL_SIZE)
#define SCREEN_HEIGHT ((GRID_HEIGHT * CELL_SIZE) + UI_HEIGHT)

// �eker t�rleri ve tan�mlanmas�
#define CANDY_TYPES 5
Texture2D candyTextures[CANDY_TYPES];

// Oyun tahtas� tan�mlanmas�
int grid[GRID_HEIGHT][GRID_WIDTH];

// S�r�kleme ba�lang�� koordinatlar�
int dragStartX = -1, dragStartY = -1;

// Oyun durumlar� ve ses ayarlar�
typedef enum { BEFOREPLAY, LEVELSELECTION, PLAYING, AFTERPLAY } GameState; GameState gameState = BEFOREPLAY;
typedef enum { WIN, LOSE } WinState; WinState winState = LOSE;
typedef enum { MUSIC_ON, MUSIC_OFF } MusicState; MusicState musicState = MUSIC_ON;
typedef enum { SOUND_ON, SOUND_OFF } SoundState; SoundState soundState = SOUND_ON;

// Oyun istatistikleri bile�enleri
typedef struct
{
    int score;
    int targetScore;
    int movesLeft;
} GameStats; GameStats gameStats;

// Anamen� bile�enleri
typedef struct
{
    Rectangle playButton;
    Rectangle musicButton;
    Rectangle soundButton;
    Texture2D wallpaper;
} MenuUI; MenuUI menuUI;

// Oyun sonras� men�s� bile�enleri
typedef struct
{
    Rectangle restartButton;
    Rectangle menuButton;
    Rectangle nextLevelButton;
} AfterGameUI; AfterGameUI afterGameUI;

// Seviye se�imi bile�enleri
typedef struct
{
	Rectangle level1Button;
	Rectangle level2Button;
	Rectangle level3Button;
	Rectangle level4Button;
	Rectangle level5Button;
    int level1TargetScore;
    int level2TargetScore;
    int level3TargetScore;
    int level4TargetScore;
    int level5TargetScore;
} LevelSelectionUI; LevelSelectionUI levelSelectionUI;

// Ses efektleri bile�enleri
typedef struct
{
    Music music;
    Sound buttonClick;
    Sound swapSound;
    Sound explosionSound;
    Sound winSound;
    Sound loseSound;
} SoundEffects; SoundEffects soundEffects;

// Oyun verileri bile�enleri
typedef struct {
    int level;
} GameData; GameData gameData;

// Fonksiyon Prototipleri
void InitAndLoad(); // Tan�mlama ve y�kleme fonksiyonu
void UpdateMenu(); // Anamen� fonksiyonu
void UpdateLevelSelection(); // Seviye se�imi fonksiyonu
void UpdateGameplay(); // Oyun fonksiyonu
void CheckMatches(); // E�le�meleri kontrol etme fonksiyonu
void DropCandies(); // �eker d���rme fonksiyonu
void UpdateAfterGame(); // Oyun sonras� fonksiyonu
void ResetGameStats(); // Oyun istatistiklerini s�f�rlama fonksiyonu
void Unload(); // Kaynaklar� serbest b�rakma fonksiyonu

int main(void)
{
	// Rastgele say� �reteci i�in tohumlama
    srand(time(NULL));

	// FPS sabitleme ayar�
    SetTargetFPS(60);

    // Pencere ve ses cihaz�n� tan�mlama
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Underwater Dream");
	InitAudioDevice();

	// Gerekli nesneleri tan�mlama ve y�kleme
    InitAndLoad();

	// Oyun d�ng�s�
    while (!WindowShouldClose())
    {
		BeginDrawing();

        // Oyun durumuna g�re ekran� g�ncelleme
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
			// Skora g�re oyunu bitirme kontrol�
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

	// Oyun d�ng�s� sona erdi�inde kaynaklar� serbest b�rakma
	Unload();

	// Oyun d�ng�s� sona erdi�inde ses cihaz�n� kapatma ve pencereyi kapatma
	CloseAudioDevice();
    CloseWindow();
    return 0;
}

void InitAndLoad()
{
    // Ba�lang�� seviyesinin belirlenmesi
    gameData.level = 1;
    printf("!!! Level is set to 1.\n");

    // Seviyelerin hedeflenen puanlar�n�n belirlenmesi
    levelSelectionUI.level1TargetScore = 1000;
    levelSelectionUI.level2TargetScore = 1250;
    levelSelectionUI.level3TargetScore = 1500;
    levelSelectionUI.level4TargetScore = 1750;
    levelSelectionUI.level5TargetScore = 2000;

    // Oyun skorlar�n�n belirlenmesi
    gameStats.score = 0;
    printf("!!! Score is set to 0.\n");
    gameStats.movesLeft = 20;
    printf("!!! Moves Left is set to 20.\n");

	// MenuUI bile�enlerinin tan�mlanmas� ve y�klenmesi
    menuUI.playButton = (Rectangle){ 300,300,200,50 };
    menuUI.musicButton = (Rectangle){ 325,410,150,50 };
    menuUI.soundButton = (Rectangle){ 325,490,150,50 };
    menuUI.wallpaper = LoadTexture("assets/images/wallpaper.png");

	// Arkaplan m�zi�inin y�klenmesi ve ayarlanmas�
    soundEffects.music = LoadMusicStream("assets/soundEffects/music.mp3");
    PlayMusicStream(soundEffects.music);
    SetMusicVolume(soundEffects.music, 0.2f);

	// Ses efektlerinin y�klenmesi ve ayarlanmas�
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

	// Seviye se�imi bile�enlerinin tan�mlanmas�
    levelSelectionUI.level1Button = (Rectangle){ 300, 275, 200, 50 };
    levelSelectionUI.level2Button = (Rectangle){ 300, 375, 200, 50 };
    levelSelectionUI.level3Button = (Rectangle){ 300, 475, 200, 50 };
    levelSelectionUI.level4Button = (Rectangle){ 300, 575, 200, 50 };
    levelSelectionUI.level5Button = (Rectangle){ 300, 675, 200, 50 };

	// �ekerlerin y�klenmesi
    candyTextures[0] = LoadTexture("assets/images/image1.png");
    candyTextures[1] = LoadTexture("assets/images/image2.png");
    candyTextures[2] = LoadTexture("assets/images/image3.png");
    candyTextures[3] = LoadTexture("assets/images/image4.png");
    candyTextures[4] = LoadTexture("assets/images/image5.png");

	// Oyun tahtas�n�n ba�lat�lmas�
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

	// Oyun sonras� men�s� bile�enlerinin tan�mlanmas�
    afterGameUI.menuButton = (Rectangle){ 200,400,400,100 };
    afterGameUI.restartButton = (Rectangle){ 275,600,250,75 };
    afterGameUI.nextLevelButton = (Rectangle){ 275,600,250,75 };
}

void UpdateMenu()
{
	// Arkaplan m�zi�inin �al�nmas�
    if (musicState == MUSIC_ON) UpdateMusicStream(soundEffects.music);

    // Arka plan�n �izilmesi
    DrawTexture(menuUI.wallpaper, 0, 0, WHITE);
    
    // Butonlar�n �izilmesi
    DrawRectangleRec(menuUI.playButton, DARKPURPLE);
    DrawRectangleRec(menuUI.musicButton, DARKBLUE);
    DrawRectangleRec(menuUI.soundButton, DARKBLUE);

	// Ba�l�klar�n �izilmesi
    DrawText("Underwater Dream", 200, 120, 42, DARKPURPLE);
    DrawText("Similar to Candy Crush Saga. Coded with Raylib.", 150, 170, 21, BLACK);
    DrawText("Start!", menuUI.playButton.x+65, menuUI.playButton.y+15, 25, WHITE);
    DrawText("Music", menuUI.musicButton.x+20, menuUI.musicButton.y+17, 18, WHITE);
    DrawText("Sound", menuUI.soundButton.x+20, menuUI.soundButton.y+17, 18, WHITE);

    if (musicState == MUSIC_ON) DrawText("ON", menuUI.musicButton.x + 100, menuUI.musicButton.y + 17, 18, GREEN);
    else if (musicState == MUSIC_OFF) DrawText("OFF", menuUI.musicButton.x + 100, menuUI.musicButton.y + 17, 18, RED);
    if (soundState == SOUND_ON) DrawText("ON", menuUI.soundButton.x + 100, menuUI.soundButton.y + 17, 18, GREEN);
    else if (soundState == SOUND_OFF) DrawText("OFF", menuUI.soundButton.x + 100, menuUI.soundButton.y + 17, 18, RED);

	// Buton t�klama kontrol�
    Vector2 mousePos = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
		// Oynama butonuna t�klama kontrol�
        if (CheckCollisionPointRec(mousePos, menuUI.playButton))
        {
            if (soundState == SOUND_ON)
            {
                PlaySound(soundEffects.buttonClick);
            }
            gameState = LEVELSELECTION;
        }
		// M�zik butonuna t�klama kontrol�
        if (CheckCollisionPointRec(mousePos, menuUI.musicButton))
        {
            if (soundState == SOUND_ON) PlaySound(soundEffects.buttonClick);
            if (musicState == MUSIC_ON) musicState = MUSIC_OFF;
            else if (musicState == MUSIC_OFF) musicState = MUSIC_ON;
        }
		// Ses butonuna t�klama kontrol�
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

void UpdateLevelSelection()
{
    // Arkaplan m�zi�inin �al�nmas�
    if (musicState == MUSIC_ON) UpdateMusicStream(soundEffects.music);

    // Arka plan�n �izilmesi
    ClearBackground(RAYWHITE);

    // Seviye butonlar� ve ba�l�klar�n�n �izilmesi
    DrawRectangleRec(levelSelectionUI.level1Button, LIGHTGRAY);
    DrawText("Level 1", levelSelectionUI.level1Button.x + 60, levelSelectionUI.level1Button.y + 13, 25, DARKPURPLE);
    if (gameData.level >= 2)
    {
        DrawRectangleRec(levelSelectionUI.level2Button, LIGHTGRAY);
        DrawText("Level 2", levelSelectionUI.level2Button.x + 60, levelSelectionUI.level2Button.y + 13, 25, DARKPURPLE);
	}
    if (gameData.level >= 3)
    {
        DrawRectangleRec(levelSelectionUI.level3Button, LIGHTGRAY);
        DrawText("Level 3", levelSelectionUI.level3Button.x + 60, levelSelectionUI.level3Button.y + 13, 25, DARKPURPLE);
    }
    if (gameData.level >= 4)
    {
        DrawRectangleRec(levelSelectionUI.level4Button, LIGHTGRAY);
        DrawText("Level 4", levelSelectionUI.level4Button.x + 60, levelSelectionUI.level4Button.y + 13, 25, DARKPURPLE);
    }
    if (gameData.level >= 5)
    {
        DrawRectangleRec(levelSelectionUI.level5Button, LIGHTGRAY);
        DrawText("Level 5", levelSelectionUI.level5Button.x + 60, levelSelectionUI.level5Button.y + 13, 25, DARKPURPLE);
	}

    // Ba�l�klar�n �izilmesi
    DrawText("Level Selection", SCREEN_WIDTH / 8, SCREEN_HEIGHT / 10, 80, DARKPURPLE);
    DrawText("Select a level to play!", SCREEN_WIDTH / 4+30, SCREEN_HEIGHT / 5, 30, DARKPURPLE);
	
    // Oyun seviyesinin se�ilmesi
    Vector2 mousePos = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        if (CheckCollisionPointRec(mousePos, levelSelectionUI.level1Button) && gameData.level >= 1)
        {
            if (soundState == SOUND_ON) PlaySound(soundEffects.buttonClick);
            gameStats.targetScore = levelSelectionUI.level1TargetScore;
            printf("!!! Target Score is set to Level 1's target score.\n");
            gameState = PLAYING;
		}
        if (CheckCollisionPointRec(mousePos, levelSelectionUI.level2Button) && gameData.level >= 2)
        {
            if (soundState == SOUND_ON) PlaySound(soundEffects.buttonClick);
            gameStats.targetScore = levelSelectionUI.level2TargetScore;
            printf("!!! Target Score is set to Level 2's target score.\n");
            gameState = PLAYING;
        }
        if (CheckCollisionPointRec(mousePos, levelSelectionUI.level3Button) && gameData.level >= 3)
        {
            if (soundState == SOUND_ON) PlaySound(soundEffects.buttonClick);
            gameStats.targetScore = levelSelectionUI.level3TargetScore;
            printf("!!! Target Score is set to Level 3's target score.\n");
            gameState = PLAYING;
        }
        if (CheckCollisionPointRec(mousePos, levelSelectionUI.level4Button) && gameData.level >= 4)
        {
            if (soundState == SOUND_ON) PlaySound(soundEffects.buttonClick);
            gameStats.targetScore = levelSelectionUI.level4TargetScore;
            printf("!!! Target Score is set to Level 4's target score.\n");
            gameState = PLAYING;
        }
        if (CheckCollisionPointRec(mousePos, levelSelectionUI.level5Button) && gameData.level >= 5)
        {
            if (soundState == SOUND_ON) PlaySound(soundEffects.buttonClick);
            gameStats.targetScore = levelSelectionUI.level5TargetScore;
            printf("!!! Target Score is set to Level 5's target score.\n");
            gameState = PLAYING;
        }
	}
}

void UpdateGameplay()
{
	// Arkaplan m�zi�inin �al�nmas�
    if (musicState == MUSIC_ON) UpdateMusicStream(soundEffects.music);

	// Arka plan�n �izilmesi
    ClearBackground(RAYWHITE);

    // S�r�kleme mekanizmas�
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

            // Kom�uluk kontrol�
            if ((abs(dragStartX - dropX) == 1 && dragStartY == dropY) ||
                (abs(dragStartY - dropY) == 1 && dragStartX == dropX)) {

                // �ekerleri de�i�tir
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


    // �st bilgi �ubu�u
    char info[128];
    snprintf(info, sizeof(info), "Points: %d / %d    Moves Left: %d", gameStats.score, gameStats.targetScore, gameStats.movesLeft);
    DrawText(info, 125, 20, 30, DARKPURPLE);

    // Oyun tahtas�n�n �izilmesi
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

	// E�le�meleri kontrol etme
	CheckMatches();

	// Sonu� kontrol�
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

        // Yatay e�le�melerin bulunmas�
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

        // Dikey e�le�melerin bulunmas�
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
        // E�le�me bulundu�unda �ekerleri d���rme
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
				// �stteki �ekerleri bul ve a�a�� kayd�r
                for (int k = y - 1; k >= 0; k--)
                {
                    if (grid[k][x] != -1)
                    {
						// �ekerin yeni konumunu ayarla
                        grid[y][x] = grid[k][x];
                        grid[k][x] = -1;
                        break;
                    }
                }
            }
        }
    }

	// Yeni �ekerleri rastgele olu�tur
    for (int y = 0; y < GRID_HEIGHT; y++)
    {
        for (int x = 0; x < GRID_WIDTH; x++)
        {
			// E�er h�cre bo�sa, yeni bir �eker olu�tur
            if (grid[y][x] == -1)
            {
                grid[y][x] = GetRandomValue(0, CANDY_TYPES - 1);
            }
        }
    }
}

void UpdateAfterGame()
{
    Vector2 mousePos = GetMousePosition();
    // Oyun kazan�ld�ysa
    if (winState == WIN)
    {
		// Arka plan�n �izilmesi
        ClearBackground(DARKGREEN);

		// Butonun �izilmesi
        DrawRectangleRec(afterGameUI.menuButton, LIGHTGRAY);

		// Ba�l�klar�n �izilmesi
        DrawText("YOU WIN!", SCREEN_WIDTH / 5+15, SCREEN_HEIGHT / 5, 100, RAYWHITE);
        DrawText(" Return to the main menu", afterGameUI.menuButton.x+5, afterGameUI.menuButton.y+33, 30, DARKPURPLE);

        if (gameData.level < 5)
        {
            DrawRectangleRec(afterGameUI.nextLevelButton, DARKPURPLE);
            DrawText("Next Level!", afterGameUI.nextLevelButton.x + 57, afterGameUI.nextLevelButton.y + 22, 28, LIGHTGRAY);
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            if (CheckCollisionPointRec(mousePos, afterGameUI.menuButton))
            {
                if (soundState == SOUND_ON) PlaySound(soundEffects.buttonClick);
                // Seviyeyi artt�rma
                gameData.level += 1;
                printf("!!! Player reached level %d\n", gameData.level);
                gameState = BEFOREPLAY;
            }
            if (CheckCollisionPointRec(mousePos, afterGameUI.nextLevelButton))
            {
                if (soundState == SOUND_ON) PlaySound(soundEffects.buttonClick);

                // Seviyeyi artt�rma
                gameData.level += 1;
                printf("!!! Player reached level %d\n", gameData.level);
                switch (gameData.level)
                {
                case 1:
                    gameStats.targetScore = levelSelectionUI.level1TargetScore;
                    printf("!!! Target Score is set to Level 1's target score.\n");
                    break;
                case 2:
                    gameStats.targetScore = levelSelectionUI.level2TargetScore;
                    printf("!!! Target Score is set to Level 2's target score.\n");
                    break;
                case 3:
                    gameStats.targetScore = levelSelectionUI.level3TargetScore;
                    printf("!!! Target Score is set to Level 3's target score.\n");
                    break;
                case 4:
                    gameStats.targetScore = levelSelectionUI.level4TargetScore;
                    printf("!!! Target Score is set to Level 4's target score.\n");
                    break;
                case 5:
                    gameStats.targetScore = levelSelectionUI.level5TargetScore;
                    printf("!!! Target Score is set to Level 5's target score.\n");
                    break;
                }
                gameState = PLAYING;
            }
        }
    }

	// Oyun kaybedildiyse
    else if (winState == LOSE)
    {
		// Arka plan�n �izilmesi
		ClearBackground(RED);

		// Butonlar�n �izilmesi
        DrawRectangleRec(afterGameUI.menuButton, LIGHTGRAY);
        DrawRectangleRec(afterGameUI.restartButton, DARKPURPLE);

		// Ba�l�klar�n �izilmesi
        DrawText("GAME OVER!", SCREEN_WIDTH / 8 - 10, SCREEN_HEIGHT / 5, 100, RAYWHITE);
        DrawText(" Return to the main menu", afterGameUI.menuButton.x+5, afterGameUI.menuButton.y+33, 30, DARKPURPLE);
        DrawText("Try again!", afterGameUI.restartButton.x+57, afterGameUI.restartButton.y+22, 28, LIGHTGRAY);

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

	// Oyun sonras� skorlar�n s�f�rlanmas�
    ResetGameStats();    
    }

void ResetGameStats()
{
	gameStats.score = 0;
    gameStats.movesLeft = 20;;
}

void Unload()
{
	// Kaynaklar� serbest b�rakma
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