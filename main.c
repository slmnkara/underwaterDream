#include "audio.h"
#include "game.h"
#include "effects.h"
#include "game_logic.h"
#include "ui.h"





void UpdateGameplay();
void UpdateGrid();
void Dragging();
void CheckMatches();
void DropCandies();

void GetResult();

void Unload();
void IsButtonLeftClicked(Rectangle button, ButtonState buttonState, GameState gameState, ControlType controlType, int setLevel);


int main(void)
{
    InitAndLoad(); // Initializing requirements and loading textures
    while (!WindowShouldClose())
    {
        BeginDrawing();
        switch (gameState) // Updating the screen depending on gameState
        {
        case BEFORE_PLAY:
            UpdateMenu();
            break;
        case LEVEL_SELECTION:
            UpdateLevelSelection();
            break;
        case PLAYING:
            UpdateGameplay();
            break;
        case AFTER_PLAY:
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



void Unload() // Releasing resources and closing the window
{
    UnloadTexture(menuUI.wallpaper);
    UnloadTexture(candyTextures[0]);
    UnloadTexture(candyTextures[1]);
    UnloadTexture(candyTextures[2]);
    UnloadTexture(candyTextures[3]);
    UnloadTexture(candyTextures[4]);
    UnloadTexture(completedUI.final);
    UnloadImage(icon);
	UnloadAudio(); // Unloading audio resources

    CloseAudioDevice();
    CloseWindow();
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
        snprintf(info, sizeof(info), "Level: %d", gameStats.targetScore / 250 - 3);
        DrawText(info, 635, 15, 35, RAYWHITE);

        // Button controls
        IsButtonLeftClicked(inGameUI.menuButton, ACTIVE, BEFORE_PLAY, NONE, 0);
        IsButtonLeftClicked(inGameUI.musicButton, ACTIVE, PLAYING, MUSIC, 0);
        IsButtonLeftClicked(inGameUI.soundButton, ACTIVE, PLAYING, SOUND, 0);
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

void GetResult()
{
    // Sonuç kontrolü
    if (gameStats.score >= gameStats.targetScore)
    {
        winState = WIN;
        if (soundState == SOUND_ON) PlaySound(soundEffects.winSound);

        if (gameStats.targetScore == TARGET_SCORE[gameStats.level - 1] && gameStats.level < 5)
        {
            gameStats.level++;
            WriteDataToFile(gameStats.level); // Seviye güncelleme
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
        gameState = AFTER_PLAY;
        if (winState == WIN && gameStats.level == 5) gameState = COMPLETED;
    }
}

void IsButtonLeftClicked(Rectangle button, ButtonState buttonState, GameState setGameState, ControlType controlType, int setLevel)
{
    Vector2 mousePos = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        if (CheckCollisionPointRec(mousePos, button))
        {
            // Sound Effects
            if (soundState == SOUND_ON)
            {
                if (buttonState == ACTIVE)
                    PlaySound(soundEffects.buttonClick);
                else
                    PlaySound(soundEffects.loseSound);
            }

            // Game State Change
            gameState = setGameState;

            // Control Types
            switch (controlType)
            {
            case NONE:
                break;

            case MUSIC:
                if (musicState == MUSIC_ON)
                    musicState = MUSIC_OFF;
                else
                    musicState = MUSIC_ON;
                break;

            case SOUND:
                if (soundState == SOUND_ON)
                    soundState = SOUND_OFF;
                else
                    soundState = SOUND_ON;
                break;

            case SET_LEVEL:
                if (setLevel < gameStats.level)
                {
                    // Setting the target score for the selected level
                    gameStats.targetScore = TARGET_SCORE[setLevel];

                    // Resetting stats for the selected level
                    gameStats.score = 0;
                    gameStats.movesLeft = MAX_MOVES;
                }
                break;

            case RESET_LEVEL:
                WriteDataToFile(1);
                gameStats.level = 1;
                break;
            }
        }
    }
}