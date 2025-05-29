#include "definitions.h"

const int TARGET_SCORE[5] = { 1000, 1250, 1500, 1750, 2000 }; // Target scores for each level
const int MAX_MOVES = 20; // Maximum moves allowed

// Game states
GameState gameState;
WinState winState;
MusicState musicState;
SoundState soundState;

Image window_icon; // Window icon
GameStats gameStats; // Game statistics
Audio_t audio_t; // Audio management

// UI elements
MainMenuUI mainMenuUI;
LevelUI levelUI;
GameUI gameUI;
AfterGameUI afterGameUI;
GameCompletedUI gameCompletedUI;

// Dragging and animation management
Drag drag;
SwapAnim swapAnim;
ExplosionAnim explosionAnim[MAX_EXPLOSIONS];

void LoadTextures()
{
    window_icon = LoadImage("assets/textures/window_icon.png");
    mainMenuUI.wallpaper = LoadTexture("assets/textures/main_menu_wallpaper.png");
    gameUI.candy_textures[0] = LoadTexture("assets/textures/texture1.png");
    gameUI.candy_textures[1] = LoadTexture("assets/textures/texture2.png");
    gameUI.candy_textures[2] = LoadTexture("assets/textures/texture3.png");
    gameUI.candy_textures[3] = LoadTexture("assets/textures/texture4.png");
    gameUI.candy_textures[4] = LoadTexture("assets/textures/texture5.png");
    gameCompletedUI.trophy_texture = LoadTexture("assets/textures/trophy_texture.png");
}

void UnloadTextures()
{
    UnloadImage(window_icon);
    UnloadTexture(mainMenuUI.wallpaper);
    for (int i = 0; i < CANDY_TYPES; i++)
    {
        UnloadTexture(gameUI.candy_textures[i]);
    }
    UnloadTexture(gameCompletedUI.trophy_texture);
}

void InitGame()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Underwater Dream");
    InitAudioDevice();

    LoadTextures(); // Loading textures
    LoadAudio(); // Loading audio files
    InitUI(); // Initializing UI elements

    // Setting the window icon
    SetWindowIcon(window_icon);

    gameState = BEFORE_PLAY; // Initial game state
    winState = LOSE; // Initial win state
    musicState = MUSIC_ON; // Initial music state
    soundState = SOUND_ON; // Initial sound state

    LoadGameProgress();
    InitGrid();

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

void DeinitGame()
{
    UnloadTextures(); // Unloading textures
    UnloadAudio(); // Unloading audio files
    CloseAudioDevice();
    CloseWindow();
}

void InitUI()
{
    mainMenuUI.play_button = (Rectangle){ 300,300,200,50 };
    mainMenuUI.music_button = (Rectangle){ 325,410,150,50 };
    mainMenuUI.sound_button = (Rectangle){ 325,490,150,50 };
    mainMenuUI.reset_button = (Rectangle){ 360,570,80,50 };

    levelUI.level_button[0] = (Rectangle){ 300, 275, 200, 50 };
    levelUI.level_button[1] = (Rectangle){ 300, 375, 200, 50 };
    levelUI.level_button[2] = (Rectangle){ 300, 475, 200, 50 };
    levelUI.level_button[3] = (Rectangle){ 300, 575, 200, 50 };
    levelUI.level_button[4] = (Rectangle){ 300, 675, 200, 50 };

    gameUI.level_rec = (Rectangle){ 4,4,194,52 };
    gameUI.score_rec = (Rectangle){ 202,4,296,52 };
    gameUI.moves_left_rec = (Rectangle){ 502,4,294,52 };
    gameUI.background = (Rectangle){ 0,0,800,60 };
    gameUI.menu_button = (Rectangle){ 5,5,390,50 };
    gameUI.music_button = (Rectangle){ 400,5,100,50 };
    gameUI.sound_button = (Rectangle){ 505,5,100,50 };

    afterGameUI.menu_button = (Rectangle){ 200,400,400,100 };
    afterGameUI.restart_button = (Rectangle){ 275,600,250,75 };
    afterGameUI.next_level_button = (Rectangle){ 275,600,250,75 };

    gameCompletedUI.menu_button = (Rectangle){ 250,650,300,50 };
}

void LoadAudio()
{
    // Background Music
    audio_t.background_music = LoadMusicStream("assets/audio/background_music.mp3");
    PlayMusicStream(audio_t.background_music);

    // Sound Effects
    audio_t.active_button_sound = LoadSound("assets/audio/active_button_sound.mp3");
    audio_t.deactive_button_sound = LoadSound("assets/audio/deactive_button_sound.mp3");
    audio_t.swap_sound = LoadSound("assets/audio/swap_sound.mp3");
    audio_t.explosion_sound = LoadSound("assets/audio/explosion_sound.mp3");
    audio_t.win_sound = LoadSound("assets/audio/win_sound.mp3");
    audio_t.lose_sound = LoadSound("assets/audio/lose_sound.mp3");

    // Setting volumes
    SetMusicVolume(audio_t.background_music, 0.1f);
    SetSoundVolume(audio_t.active_button_sound, 0.5f);
    SetSoundVolume(audio_t.deactive_button_sound, 0.5f);
    SetSoundVolume(audio_t.swap_sound, 0.2f);
    SetSoundVolume(audio_t.explosion_sound, 0.1f);
    SetSoundVolume(audio_t.win_sound, 0.8f);
    SetSoundVolume(audio_t.lose_sound, 0.5f);
}

void UnloadAudio()
{
    UnloadMusicStream(audio_t.background_music);
    UnloadSound(audio_t.active_button_sound);
    UnloadSound(audio_t.deactive_button_sound);
    UnloadSound(audio_t.swap_sound);
    UnloadSound(audio_t.explosion_sound);
    UnloadSound(audio_t.win_sound);
    UnloadSound(audio_t.lose_sound);
}

void PlayAudio(Audio audio)
{
    switch (audio)
    {
    case BACKGROUND_MUSIC:
        if (musicState == MUSIC_ON) UpdateMusicStream(audio_t.background_music);
        break;
    case ACTIVE_BUTTON_SOUND:
        if (soundState == SOUND_ON) PlaySound(audio_t.active_button_sound);
        break;
    case DEACTIVE_BUTTON_SOUND:
        if (soundState == SOUND_ON) PlaySound(audio_t.deactive_button_sound);
        break;
    case SWAP_SOUND:
        if (soundState == SOUND_ON) PlaySound(audio_t.swap_sound);
        break;
    case EXPLOSION_SOUND:
        if (soundState == SOUND_ON) PlaySound(audio_t.explosion_sound);
        break;
    case WIN_SOUND:
        if (soundState == SOUND_ON) PlaySound(audio_t.win_sound);
        break;
    case LOSE_SOUND:
        if (soundState == SOUND_ON) PlaySound(audio_t.lose_sound);
        break;
    }
}

void InitGrid()
{
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
}

void UpdateMainMenuScreen()
{
    PlayAudio(BACKGROUND_MUSIC);
    ClearBackground(RAYWHITE);

    // Drawing wallpaper
    DrawTexture(mainMenuUI.wallpaper, 0, 0, WHITE);

    // Drawing buttons
    DrawRectangleRec(mainMenuUI.play_button, DARKPURPLE);
    DrawRectangleRec(mainMenuUI.music_button, DARKBLUE);
    DrawRectangleRec(mainMenuUI.sound_button, DARKBLUE);
    DrawRectangleRec(mainMenuUI.reset_button, BLACK);

    // Drawing labels
    DrawText("Underwater Dream", 200, 120, 42, DARKPURPLE);
    DrawText("Similar to Candy Crush Saga. Coded with Raylib.", 150, 170, 21, BLACK);
    DrawText("Start!", mainMenuUI.play_button.x + 65, mainMenuUI.play_button.y + 15, 25, WHITE);
    DrawText("Music", mainMenuUI.music_button.x + 20, mainMenuUI.music_button.y + 17, 18, WHITE);
    DrawText("Sound", mainMenuUI.sound_button.x + 20, mainMenuUI.sound_button.y + 17, 18, WHITE);
    DrawText("Reset\nLevel", mainMenuUI.reset_button.x + 13, mainMenuUI.reset_button.y + 5, 20, WHITE);

    // Control of ON-OFF labels
    if (musicState == MUSIC_ON)
        DrawText("ON", mainMenuUI.music_button.x + 100, mainMenuUI.music_button.y + 17, 18, GREEN);
    else
        DrawText("OFF", mainMenuUI.music_button.x + 100, mainMenuUI.music_button.y + 17, 18, RED);

    if (soundState == SOUND_ON)
        DrawText("ON", mainMenuUI.sound_button.x + 100, mainMenuUI.sound_button.y + 17, 18, GREEN);
    else
        DrawText("OFF", mainMenuUI.sound_button.x + 100, mainMenuUI.sound_button.y + 17, 18, RED);

    // Drawing current level
    char info[128];
    snprintf(info, sizeof(info), "Current Level: %d", gameStats.current_level);
    DrawText(info, 15, 830, 20, BLACK);

    // Button controls
    IsButtonClicked(mainMenuUI.play_button, ACTIVE, LEVEL_SELECTION, CONTROL_NONE, -1);
    IsButtonClicked(mainMenuUI.music_button, ACTIVE, BEFORE_PLAY, SET_MUSIC_STATE, -1);
    IsButtonClicked(mainMenuUI.sound_button, ACTIVE, BEFORE_PLAY, SET_SOUND_STATE, -1);
    IsButtonClicked(mainMenuUI.reset_button, ACTIVE, BEFORE_PLAY, RESET_LEVEL, -1);
}

void UpdateLevelSelectionScreen()
{
    PlayAudio(BACKGROUND_MUSIC);
    ClearBackground(RAYWHITE);

    // Drawing labels
    DrawText("Level Selection", SCREEN_WIDTH / 8, SCREEN_HEIGHT / 10, 80, DARKPURPLE);
    DrawText("Select a level to play!", SCREEN_WIDTH / 4 + 30, SCREEN_HEIGHT / 5, 30, DARKPURPLE);

    // Drawing buttons
    for (int i = gameStats.current_level; i <= 4; i++)
    {
        DrawRectangleRec(levelUI.level_button[i], RED);
        DrawText("LOCKED!", levelUI.level_button[i].x + 50, levelUI.level_button[i].y + 13, 25, WHITE);
        IsButtonClicked(levelUI.level_button[i], DEACTIVE, LEVEL_SELECTION, CONTROL_NONE, -1);
    }

    // Drawing buttons and their labels
    char levelNum[32] = { 0 };
    int tempLevel = 1;
    for (int i = 0; i < gameStats.current_level; i++)
    {
        DrawRectangleRec(levelUI.level_button[i], DARKPURPLE);
        snprintf(levelNum, sizeof(levelNum), "Level %d", tempLevel);
        DrawText(levelNum, levelUI.level_button[i].x + 60, levelUI.level_button[i].y + 13, 25, WHITE);
        tempLevel += 1;
        IsButtonClicked(levelUI.level_button[i], ACTIVE, PLAYING, SET_LEVEL, i);
    }
}

void UpdateGameplayScreen()
{
    PlayAudio(BACKGROUND_MUSIC);
    ClearBackground(RAYWHITE);

    UpdateGameUI();
    UpdateGrid();
    if (!swapAnim.swapActive) HandleDragging();
    ProcessMatches();
    UpdateExplosion(GetFrameTime());
    UpdateSwap();

    CheckGameStatus();
}

void UpdateAfterGameScreen()
{
    if (winState == WIN)
    {
        ClearBackground(DARKGREEN);

        // Drawing buttons
        DrawRectangleRec(afterGameUI.menu_button, LIGHTGRAY);

        // Son seviye de bittiyse, sýradaki seviye butonunu gösterme
        if (gameStats.current_level <= 5)
        {
            DrawRectangleRec(afterGameUI.next_level_button, DARKPURPLE);
            DrawText("Next Level!", afterGameUI.next_level_button.x + 53, afterGameUI.next_level_button.y + 22, 28, LIGHTGRAY);
        }

        // Drawing labels
        DrawText("YOU WIN!", SCREEN_WIDTH / 5 + 15, SCREEN_HEIGHT / 5, 100, RAYWHITE);
        DrawText("Return to the main menu", afterGameUI.menu_button.x + 17, afterGameUI.menu_button.y + 33, 30, DARKPURPLE);

        // Button controls
        IsButtonClicked(afterGameUI.next_level_button, ACTIVE, BEFORE_PLAY, CONTROL_NONE, -1);
        switch (gameStats.current_level)
        {
        case 1:
            IsButtonClicked(afterGameUI.next_level_button, ACTIVE, PLAYING, SET_LEVEL, 0);
            break;
        case 2:
            IsButtonClicked(afterGameUI.next_level_button, ACTIVE, PLAYING, SET_LEVEL, 1);
            break;
        case 3:
            IsButtonClicked(afterGameUI.next_level_button, ACTIVE, PLAYING, SET_LEVEL, 2);
            break;
        case 4:
            IsButtonClicked(afterGameUI.next_level_button, ACTIVE, PLAYING, SET_LEVEL, 3);
            break;
        case 5:
            IsButtonClicked(afterGameUI.next_level_button, ACTIVE, PLAYING, SET_LEVEL, 4);
            break;
        }
    }
    else if (winState == LOSE)
    {
        ClearBackground(RED);

        // Drawing buttons
        DrawRectangleRec(afterGameUI.menu_button, LIGHTGRAY);
        DrawRectangleRec(afterGameUI.restart_button, DARKPURPLE);

        // Drawing labels
        DrawText("GAME OVER!", SCREEN_WIDTH / 8 - 10, SCREEN_HEIGHT / 5, 100, RAYWHITE);
        DrawText("Return to the main menu", afterGameUI.menu_button.x + 17, afterGameUI.menu_button.y + 33, 30, DARKPURPLE);
        DrawText("Try again!", afterGameUI.restart_button.x + 57, afterGameUI.restart_button.y + 22, 28, LIGHTGRAY);

        // Button controls
        IsButtonClicked(afterGameUI.menu_button, ACTIVE, BEFORE_PLAY, CONTROL_NONE, -1);
        IsButtonClicked(afterGameUI.restart_button, ACTIVE, PLAYING, CONTROL_NONE, -1);
    }
}

void UpdateGameCompletedScreen()
{
    ClearBackground(DARKPURPLE);

    // Drawing buttons
    DrawRectangleRec(gameCompletedUI.menu_button, LIGHTGRAY);

    // Drawing trophy texture
    DrawTextureEx(gameCompletedUI.trophy_texture, (Vector2) { 180, 275 }, -13.0f, 0.4f, WHITE);

    // Drawing labels
    DrawText("Congratulations!", 160, 130, 60, WHITE);
    DrawText("The game is completed!", 230, 200, 30, WHITE);
    DrawText("Return to the main menu.", gameCompletedUI.menu_button.x + 20, gameCompletedUI.menu_button.y + 15, 20, DARKPURPLE);

    // Button controls
    IsButtonClicked(gameCompletedUI.menu_button, ACTIVE, BEFORE_PLAY, CONTROL_NONE, -1);
}

void UpdateGameUI()
{
    Vector2 mousePos = GetMousePosition();
    if (mousePos.y <= GAME_UI_HEIGHT)
    {
        // Drawing background
        DrawRectangleRec(gameUI.background, DARKPURPLE);
        DrawRectangleRec(gameUI.menu_button, RAYWHITE);
        DrawRectangleRec(gameUI.music_button, RAYWHITE);
        DrawRectangleRec(gameUI.sound_button, RAYWHITE);

        // Drawing labels
        DrawText("Return to the main menu", gameUI.menu_button.x + 45, gameUI.menu_button.y + 15, 25, DARKPURPLE);
        DrawText("Music", gameUI.music_button.x + 22, gameUI.music_button.y + 8, 20, DARKPURPLE);
        DrawText("Sound", gameUI.sound_button.x + 20, gameUI.sound_button.y + 8, 20, DARKPURPLE);

        // Drawing ON-OFF labels
        if (musicState == MUSIC_ON)
            DrawText("ON", gameUI.music_button.x + 40, gameUI.music_button.y + 28, 18, GREEN);
        else
            DrawText("OFF", gameUI.music_button.x + 35, gameUI.music_button.y + 28, 18, RED);

        if (soundState == SOUND_ON)
            DrawText("ON", gameUI.sound_button.x + 40, gameUI.sound_button.y + 28, 18, GREEN);
        else
            DrawText("OFF", gameUI.sound_button.x + 35, gameUI.sound_button.y + 28, 18, RED);

        // Drawing level
        char info[128];
        snprintf(info, sizeof(info), "Level: %d", gameStats.target_score / 250 - 3); // Should be corrected
        DrawText(info, 635, 15, 35, RAYWHITE);

        // Button controls
        IsButtonClicked(gameUI.menu_button, ACTIVE, BEFORE_PLAY, CONTROL_NONE, -1);
        IsButtonClicked(gameUI.music_button, ACTIVE, PLAYING, SET_MUSIC_STATE, -1);
        IsButtonClicked(gameUI.sound_button, ACTIVE, PLAYING, SET_SOUND_STATE, -1);
    }
    else
    {
        // Drawing background and rectangles
        DrawRectangleRec(gameUI.background, DARKPURPLE);
        DrawRectangleRec(gameUI.level_rec, RAYWHITE);
        DrawRectangleRec(gameUI.score_rec, RAYWHITE);
        DrawRectangleRec(gameUI.moves_left_rec, RAYWHITE);

        // Drawing game stats
        snprintf(gameUI.char_level, sizeof(gameUI.char_level), "Level: %d", gameStats.target_score / 250 - 3); // Should be corrected
        snprintf(gameUI.char_score, sizeof(gameUI.char_score), "Score: %d / %d", gameStats.current_score, gameStats.target_score);
        snprintf(gameUI.char_moves_left, sizeof(gameUI.char_moves_left), "Moves Left: %d", gameStats.current_moves_left);
        DrawText(gameUI.char_level, gameUI.level_rec.x + 35, gameUI.level_rec.y + 12, 32, VIOLET);
        DrawText(gameUI.char_score, gameUI.score_rec.x + 20, gameUI.score_rec.y + 15, 29, DARKGREEN);
        DrawText(gameUI.char_moves_left, gameUI.moves_left_rec.x + 33, gameUI.moves_left_rec.y + 14, 30, DARKBLUE);
    }
}

void UpdateGrid()
{
    // Her karede görsel konumu güncelle
    float fallSpeed = 5.0f * GetFrameTime(); // Hýzý ayarlayabilirsiniz
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            if (visualY[y][x] < y) {
                visualY[y][x] += fallSpeed;
                if (visualY[y][x] > y) visualY[y][x] = (float)y;
            }
            else if (visualY[y][x] > y) {
                visualY[y][x] = (float)y; // yukarý çýkma olmasýn
            }
        }
    }

    // Draw normal candies
    for (int y = 0; y < GRID_HEIGHT; y++)
    {
        for (int x = 0; x < GRID_WIDTH; x++)
        {
            Rectangle cell = { x * CELL_SIZE, y * CELL_SIZE + GAME_UI_HEIGHT, CELL_SIZE, CELL_SIZE };
            Texture2D tex = gameUI.candy_textures[grid[y][x]];
            Rectangle source = { 0, 0, (float)tex.width, (float)tex.height };
            Rectangle dest = { visualX[y][x] * CELL_SIZE, visualY[y][x] * CELL_SIZE + GAME_UI_HEIGHT, CELL_SIZE, CELL_SIZE };
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
            Texture2D tex = gameUI.candy_textures[explosionAnim[i].candyType];
            Rectangle source = { 0, 0, (float)tex.width, (float)tex.height };
            Rectangle dest = { x * CELL_SIZE, y * CELL_SIZE + GAME_UI_HEIGHT, CELL_SIZE, CELL_SIZE };
            Vector2 origin = { 0, 0 };
            Color fadeColor = WHITE;
            fadeColor.a = (unsigned char)(255 * alpha);
            DrawTexturePro(tex, source, dest, origin, 0.0f, fadeColor);
        }
    }
}

void HandleDragging()
{
    Vector2 mousePos = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (mousePos.y >= GAME_UI_HEIGHT) {
            drag.dragStartX = mousePos.x / CELL_SIZE;
            drag.dragStartY = (mousePos.y - GAME_UI_HEIGHT) / CELL_SIZE;
        }
    }
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        if (drag.dragStartX != -1 && drag.dragStartY != -1 && !swapAnim.swapActive) {
            int dropX = mousePos.x / CELL_SIZE;
            int dropY = (mousePos.y - GAME_UI_HEIGHT) / CELL_SIZE;

            // Komþuluk kontrolü
            if ((abs(drag.dragStartX - dropX) == 1 && drag.dragStartY == dropY) ||
                (abs(drag.dragStartY - dropY) == 1 && drag.dragStartX == dropX)) {

                // Swap animasyonunu baþlat
                swapAnim.swapActive = 1;
                swapAnim.swapX1 = drag.dragStartX;
                swapAnim.swapY1 = drag.dragStartY;
                swapAnim.swapX2 = dropX;
                swapAnim.swapY2 = dropY;
                swapAnim.swapProgress = 0.0f;
                gameStats.current_moves_left--;
                PlayAudio(SWAP_SOUND);

                swapStartX1 = visualX[swapAnim.swapY1][swapAnim.swapX1];
                swapStartY1 = visualY[swapAnim.swapY1][swapAnim.swapX1];
                swapStartX2 = visualX[swapAnim.swapY2][swapAnim.swapX2];
                swapStartY2 = visualY[swapAnim.swapY2][swapAnim.swapX2];
            }

            drag.dragStartX = drag.dragStartY = -1;
        }
    }
}

int GetScoreForMatch(int length) {
    if (length >= 5) return 250;
    if (length == 4) return 150;
    return 50;
}

void MarkHorizontalMatches(bool toExplode[GRID_HEIGHT][GRID_WIDTH]) {
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH - 2; x++) {
            int candy = grid[y][x];
            if (candy == -1) continue;

            int matchLen = 1;
            while (x + matchLen < GRID_WIDTH && grid[y][x + matchLen] == candy) {
                matchLen++;
            }

            if (matchLen >= 3) {
                for (int i = 0; i < matchLen; i++)
                    toExplode[y][x + i] = true;
                gameStats.current_score += GetScoreForMatch(matchLen);
                x += matchLen - 1;
            }
        }
    }
}

void MarkVerticalMatches(bool toExplode[GRID_HEIGHT][GRID_WIDTH]) {
    for (int x = 0; x < GRID_WIDTH; x++) {
        for (int y = 0; y < GRID_HEIGHT - 2; y++) {
            int candy = grid[y][x];
            if (candy == -1) continue;

            int matchLen = 1;
            while (y + matchLen < GRID_HEIGHT && grid[y + matchLen][x] == candy) {
                matchLen++;
            }

            if (matchLen >= 3) {
                for (int i = 0; i < matchLen; i++)
                    toExplode[y + i][x] = true;
                gameStats.current_score += GetScoreForMatch(matchLen);
                y += matchLen - 1;
            }
        }
    }
}

void ProcessMatches() {
    bool matchFound;
    do {
        bool toExplode[GRID_HEIGHT][GRID_WIDTH] = { false };
        matchFound = false;

        MarkHorizontalMatches(toExplode);
        MarkVerticalMatches(toExplode);

        for (int y = 0; y < GRID_HEIGHT; y++) {
            for (int x = 0; x < GRID_WIDTH; x++) {
                if (toExplode[y][x]) {
                    TriggerExplosion(x, y, grid[y][x]);
                    grid[y][x] = -1;
                    matchFound = true;
                }
            }
        }

        if (matchFound) {
            PlayAudio(EXPLOSION_SOUND);
            ProcessCandyFalls();
        }

    } while (matchFound);
}

void ProcessCandyFalls() {
    for (int x = 0; x < GRID_WIDTH; x++) {
        for (int y = GRID_HEIGHT - 1; y >= 0; y--) {
            if (grid[y][x] == -1) {
                for (int k = y - 1; k >= 0; k--) {
                    if (grid[k][x] != -1) {
                        grid[y][x] = grid[k][x];
                        grid[k][x] = -1;
                        visualY[y][x] = visualY[k][x]; // Görsel konumu da taþý
                        visualX[y][x] = (float)x;
                        break;
                    }
                }
            }
        }
    }
    // Yeni þekerler için
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            if (grid[y][x] == -1) {
                grid[y][x] = GetRandomValue(0, CANDY_TYPES - 1);
                visualY[y][x] = -1.5f; // Ekran dýþýndan baþlasýn
                visualX[y][x] = (float)x;
            }
        }
    }
}

void CheckGameStatus()
{
    // Sonuç kontrolü
    if (gameStats.current_score >= gameStats.target_score)
    {
        winState = WIN;
        PlayAudio(WIN_SOUND);

        for (int i = 1; i <= 5; i++)
        {
            if (gameStats.current_level == i && gameStats.target_score == 750 + 250 * i)
            {
                gameStats.current_level++;
                SaveGameProgress(gameStats.current_level); // Seviye güncelleme
            }
        }

    }
    else if (gameStats.current_moves_left <= 0)
    {
        winState = LOSE;
        PlayAudio(LOSE_SOUND);
    }
    // Skora göre oyunu bitirme kontrolü

    if (gameStats.current_moves_left <= 0 || gameStats.current_score >= gameStats.target_score)
    {
        gameState = AFTER_PLAY;
        if (winState == WIN && gameStats.current_level > 5) gameState = GAME_COMPLETED;
    }
}

void TriggerExplosion(int x, int y, int candyType)
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
            // Swap iþlemini grid'de uygula
            int temp = grid[swapAnim.swapY1][swapAnim.swapX1];
            grid[swapAnim.swapY1][swapAnim.swapX1] = grid[swapAnim.swapY2][swapAnim.swapX2];
            grid[swapAnim.swapY2][swapAnim.swapX2] = temp;

            // Görsel konumlarý da güncelle
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
            // Görsel konumlarý ara pozisyona taþý
            float t = swapAnim.swapProgress;
            visualX[swapAnim.swapY1][swapAnim.swapX1] = (1 - t) * swapAnim.swapX1 + t * swapAnim.swapX2;
            visualY[swapAnim.swapY1][swapAnim.swapX1] = (1 - t) * swapAnim.swapY1 + t * swapAnim.swapY2;
            visualX[swapAnim.swapY2][swapAnim.swapX2] = (1 - t) * swapAnim.swapX2 + t * swapAnim.swapX1;
            visualY[swapAnim.swapY2][swapAnim.swapX2] = (1 - t) * swapAnim.swapY2 + t * swapAnim.swapY1;
        }
    }
}

void IsButtonClicked(Rectangle button, ButtonState buttonState, GameState setGameState, ControlType controlType, int setLevel)
{
    Vector2 mousePos = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        if (CheckCollisionPointRec(mousePos, button))
        {
            // Sound Effects
            if (buttonState == ACTIVE)
                PlayAudio(ACTIVE_BUTTON_SOUND);
            else
                PlayAudio(DEACTIVE_BUTTON_SOUND);

            // Game State Change
            gameState = setGameState;

            // Control Types
            switch (controlType)
            {
            case CONTROL_NONE:
                // Resetting stats for the selected level
                gameStats.current_score = 0;
                gameStats.current_moves_left = MAX_MOVES;
                break;

            case SET_MUSIC_STATE:
                if (musicState == MUSIC_ON)
                    musicState = MUSIC_OFF;
                else
                    musicState = MUSIC_ON;
                break;

            case SET_SOUND_STATE:
                if (soundState == SOUND_ON)
                    soundState = SOUND_OFF;
                else
                    soundState = SOUND_ON;
                break;

            case SET_LEVEL:
                if (setLevel < gameStats.current_level)
                {
                    // Setting the target score for the selected level
                    gameStats.target_score = TARGET_SCORE[setLevel];

                    // Resetting stats for the selected level
                    gameStats.current_score = 0;
                    gameStats.current_moves_left = MAX_MOVES;
                }
                break;

            case RESET_LEVEL:
                SaveGameProgress(1);
                gameStats.current_level = 1;
                break;
            }
        }
    }
}

void LoadGameProgress()
{
    FILE* file = fopen("assets/level_data.txt", "r");
    if (file == NULL)
    {
        printf("Error opening file!\n");
        gameStats.current_level = 1;
        return;
    }
    fscanf_s(file, "%d", &gameStats.current_level);
    fclose(file);
}

void SaveGameProgress(int level)
{
    FILE* file = fopen("assets/level_data.txt", "w");
    if (file == NULL)
    {
        printf("Error opening file!\n");
        return;
    }
    fprintf(file, "%d", level);
    fclose(file);
}