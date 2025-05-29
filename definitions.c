#include "definitions.h"

// State structs
GameState gameState;
WinState winState;
MusicState musicState;
SoundState soundState;

// UI elements structs
MainMenuUI mainMenuUI;
LevelUI levelUI;
GameUI gameUI;
AfterGameUI afterGameUI;
GameCompletedUI gameCompletedUI;

// Dragging and animation management structs
Drag drag;
SwapAnim swapAnim;
ExplosionAnim explosionAnim[MAX_EXPLOSIONS];

GameStats gameStats; // Game statistics struct

Audio_t audio_t; // Audio management struct

Image window_icon; // Image for window icon

const int TARGET_SCORE[5] = { 1000, 1250, 1500, 1750, 2000 }; // Target scores for each level
const int MAX_MOVES = 20; // Maximum moves allowed

// Initializes all game elements
void InitGame()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Underwater Dream");
    SetWindowIcon(window_icon); // Setting the window icon
    InitAudioDevice();

    gameState = BEFORE_PLAY; // Initial game state
    winState = LOSE; // Initial win state
    musicState = MUSIC_ON; // Initial music state
    soundState = SOUND_ON; // Initial sound state

    LoadTextures(); // Loading textures
    LoadAudio(); // Loading audio files
    InitUI(); // Initializing UI elements
    LoadGameProgress(); // Loading game progress from file
    InitGrid(); // Initializing the grid with random candies
	InitDrag();  // Initializing dragging start values
	InitSwapAnim(); // Initializing swap animation start values
}

// Initializes the elements of UI
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

// Initializes first values for candies for the grid
void InitGrid()
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
                ); // This condition prevents the formation of candies that will explode.
            grid[y][x] = candy;
        }
    }
}

// Initializes dragging start values
void InitDrag()
{
    // Dragging start values
    drag.dragStartX = -1;
    drag.dragStartY = -1;
}

// Initializes swap animation start values
void InitSwapAnim()
{
    // Swap animation start values
    swapAnim.swapActive = 0;
    swapAnim.swapProgress = 0.0f;

	// Swap positions start values
    for (int y = 0; y < GRID_HEIGHT; y++)
    {
        for (int x = 0; x < GRID_WIDTH; x++)
        {
            visualY[y][x] = (float)y;
            visualX[y][x] = (float)x;
        }
    }
}

// Functions for loading and unloading textures
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

// Functions for loading and unloading audio files
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

// Function to play audio based on the audio type
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

// Functions for updating different game screens
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

	// Drawing current level at the bottom left corner
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

	// Drawing level buttons which are locked and their labels
	for (int i = gameStats.current_level; i <= 4; i++) // Starting from the next level
    {
        DrawRectangleRec(levelUI.level_button[i], RED);
        DrawText("LOCKED!", levelUI.level_button[i].x + 50, levelUI.level_button[i].y + 13, 25, WHITE);
        IsButtonClicked(levelUI.level_button[i], DEACTIVE, LEVEL_SELECTION, CONTROL_NONE, -1);
    }

    // Drawing active level buttons and their labels
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

    // Game logic
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

		// If player has completed all levels, do not show the next level button
        if (gameStats.current_level <= 5)
        {
            DrawRectangleRec(afterGameUI.next_level_button, DARKPURPLE);
            DrawText("Next Level!", afterGameUI.next_level_button.x + 53, afterGameUI.next_level_button.y + 22, 28, LIGHTGRAY);
        }

        // Drawing labels
        DrawText("YOU WIN!", SCREEN_WIDTH / 5 + 15, SCREEN_HEIGHT / 5, 100, RAYWHITE);
        DrawText("Return to the main menu", afterGameUI.menu_button.x + 17, afterGameUI.menu_button.y + 33, 30, DARKPURPLE);

		// Button control for the next level button
        IsButtonClicked(afterGameUI.next_level_button, ACTIVE, BEFORE_PLAY, CONTROL_NONE, -1);
		switch (gameStats.current_level) // Button controls based on the current level
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
		// Button control for the main menu
        IsButtonClicked(afterGameUI.menu_button, ACTIVE, BEFORE_PLAY, CONTROL_NONE, -1);
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

// Function to update the game UI
void UpdateGameUI()
{
	Vector2 mousePos = GetMousePosition(); // Getting the mouse position
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
        snprintf(gameUI.char_level, sizeof(gameUI.char_level), "Level: %d", (gameStats.target_score / 250 - 3) /* This expression detects the level that the player is currently playing, not the current level. */);
		DrawText(gameUI.char_level, 635, 15, 35, RAYWHITE);

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

		// Drawing game statistics
        snprintf(gameUI.char_level, sizeof(gameUI.char_level), "Level: %d", (gameStats.target_score / 250 - 3) /* This expression detects the level that the player is currently playing, not the current level. */);
        DrawText(gameUI.char_level, gameUI.level_rec.x + 35, gameUI.level_rec.y + 12, 32, VIOLET);

        snprintf(gameUI.char_score, sizeof(gameUI.char_score), "Score: %d / %d", gameStats.current_score, gameStats.target_score);
        DrawText(gameUI.char_score, gameUI.score_rec.x + 20, gameUI.score_rec.y + 15, 29, DARKGREEN);

        snprintf(gameUI.char_moves_left, sizeof(gameUI.char_moves_left), "Moves Left: %d", gameStats.current_moves_left);
        DrawText(gameUI.char_moves_left, gameUI.moves_left_rec.x + 33, gameUI.moves_left_rec.y + 14, 30, DARKBLUE);
    }
}

// Function to update the grid and draw candies
void UpdateGrid()
{
	float fallSpeed = 5.0f * GetFrameTime(); // We can adjust the fall speed here

    // Update visual positions for candies at every frame
    for (int y = 0; y < GRID_HEIGHT; y++)
    {
        for (int x = 0; x < GRID_WIDTH; x++)
        {
            if (visualY[y][x] < y)
            {
                visualY[y][x] += fallSpeed;
                if (visualY[y][x] > y) visualY[y][x] = (float)y;
            }
            else if (visualY[y][x] > y)
            {
				visualY[y][x] = (float)y; // Candy has reached its final position, it should not go above its grid position
            }
        }
    }

	// Draw candies for normal state
    for (int y = 0; y < GRID_HEIGHT; y++)
    {
        for (int x = 0; x < GRID_WIDTH; x++)
        {
			Rectangle cell = { x * CELL_SIZE, y * CELL_SIZE + GAME_UI_HEIGHT, CELL_SIZE, CELL_SIZE }; // Cell rectangle for drawing
			Texture2D tex = gameUI.candy_textures[grid[y][x]]; // Get the texture for the candy type
			Rectangle source = { 0, 0, (float)tex.width, (float)tex.height }; // Source rectangle for the texture
			Rectangle dest = { visualX[y][x] * CELL_SIZE, visualY[y][x] * CELL_SIZE + GAME_UI_HEIGHT, CELL_SIZE, CELL_SIZE }; // Destination rectangle for the texture
			Vector2 origin = { 0, 0 }; // Origin for the texture
			DrawTexturePro(tex, source, dest, origin, 0.0f, WHITE); // Draw the candy texture
			DrawRectangleLinesEx(cell, 1, DARKPURPLE); // Draw the cell border
        }
    }

    // Draw explosion animations
    for (int i = 0; i < MAX_EXPLOSIONS; i++)
    {
        if (explosionAnim[i].active)
        {
            float alpha = 1.0f - (explosionAnim[i].explosionProgress / EXPLOSION_ANIMATION_DURATION); // Fade out
			if (alpha < 0) alpha = 0; // Ensure alpha does not go below 0

			int x = explosionAnim[i].x; // Get the x position of the explosion
			int y = explosionAnim[i].y; // Get the y position of the explosion

			Texture2D tex = gameUI.candy_textures[explosionAnim[i].candyType]; // Get the texture for the candy type
			Rectangle source = { 0, 0, (float)tex.width, (float)tex.height }; // Source rectangle for the texture
			Rectangle dest = { x * CELL_SIZE, y * CELL_SIZE + GAME_UI_HEIGHT, CELL_SIZE, CELL_SIZE }; // Destination rectangle for the texture
			Vector2 origin = { 0, 0 }; // Origin for the texture
			Color fadeColor = WHITE; // Color for fading (white by default)
			fadeColor.a = (unsigned char)(255 * alpha); // Set the alpha value for fading color
			DrawTexturePro(tex, source, dest, origin, 0.0f, fadeColor); // Draw the explosion texture with fading
        }
    }
}

// Function to handle dragging and swapping candies
void HandleDragging()
{
	Vector2 mousePos = GetMousePosition(); // Getting the mouse position
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        if (mousePos.y >= GAME_UI_HEIGHT) // Only allow dragging in the game area
        {
			drag.dragStartX = mousePos.x / CELL_SIZE; // Calculate the horizontal grid position
			drag.dragStartY = (mousePos.y - GAME_UI_HEIGHT) / CELL_SIZE; // Calculate the vertical grid position
        }
    }

    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
    {
		if (drag.dragStartX != -1 && drag.dragStartY != -1 && !swapAnim.swapActive) // Only allow swapping if not already in a swap animation
        {
			int dropX = mousePos.x / CELL_SIZE; // Calculate the horizontal grid position for dropping
			int dropY = (mousePos.y - GAME_UI_HEIGHT) / CELL_SIZE; // Calculate the vertical grid position for dropping

			// Cotrol if the drag start position is adjacent to the drop position
            if ((abs(drag.dragStartX - dropX) == 1 && drag.dragStartY == dropY) ||
                (abs(drag.dragStartY - dropY) == 1 && drag.dragStartX == dropX)) {

				// Start the swap animation
				swapAnim.swapActive = 1; // Set swap active
				swapAnim.swapX1 = drag.dragStartX; // Set the starting X position for the swap
				swapAnim.swapY1 = drag.dragStartY; // Set the starting Y position for the swap
				swapAnim.swapX2 = dropX; // Set the ending X position for the swap
				swapAnim.swapY2 = dropY; // Set the ending Y position for the swap
				swapAnim.swapProgress = 0.0f; // Reset the swap progress (animation progress)
				gameStats.current_moves_left--; // Decrease the moves left
                PlayAudio(SWAP_SOUND);
            }
			drag.dragStartX = drag.dragStartY = -1; // Reset dragging start values
        }
    }
}

// Function to get the score for a match based on its length
int GetScoreForMatch(int length)
{
    if (length >= 5) return 250;
    if (length == 4) return 175;
    return 75;
}

// Functions to mark horizontal and vertical matches
void MarkHorizontalMatches(bool toExplode[GRID_HEIGHT][GRID_WIDTH])
{
    for (int y = 0; y < GRID_HEIGHT; y++)
    {
        for (int x = 0; x < GRID_WIDTH - 2; x++)
        {
			int candy = grid[y][x]; // Get the candy type at the current position
			if (candy == -1) continue; // Skip if the candy is already exploded

			int matchLen = 1; // Initialize match length to 1
			while (x + matchLen < GRID_WIDTH && grid[y][x + matchLen] == candy) // Check for matches horizontally
            {
				matchLen++; // Increase match length if the next candy is the same type
            }

			if (matchLen >= 3) // If a match of length 3 or more is found
            {
				for (int i = 0; i < matchLen; i++) // Mark all candies in the match for explosion
					toExplode[y][x + i] = true; // Set the explosion flag for each candy in the match
				gameStats.current_score += GetScoreForMatch(matchLen); // Update the score based on the match length
				x += matchLen - 1; // Move the x position to the end of the match
            }
        }
    }
}

void MarkVerticalMatches(bool toExplode[GRID_HEIGHT][GRID_WIDTH])
{
    for (int x = 0; x < GRID_WIDTH; x++)
    {
        for (int y = 0; y < GRID_HEIGHT - 2; y++)
        {
			int candy = grid[y][x]; // Get the candy type at the current position
			if (candy == -1) continue; // Skip if the candy is already exploded

			int matchLen = 1; // Initialize match length to 1
			while (y + matchLen < GRID_HEIGHT && grid[y + matchLen][x] == candy) // Check for matches vertically
            {
				matchLen++; // Increase match length if the next candy is the same type
            }

			if (matchLen >= 3) // If a match of length 3 or more is found
            {
				for (int i = 0; i < matchLen; i++) // Mark all candies in the match for explosion
					toExplode[y + i][x] = true; // Set the explosion flag for each candy in the match
				gameStats.current_score += GetScoreForMatch(matchLen); // Update the score based on the match length
				y += matchLen - 1; // Move the y position to the end of the match
            }
        }
    }
}

// Function to process matches and trigger explosions
void ProcessMatches()
{
	bool matchFound; // Flag to check if any matches were found
    do {
		bool toExplode[GRID_HEIGHT][GRID_WIDTH] = { false }; // Array to mark candies to explode

		matchFound = false; // Reset match found flag for each iteration

		// Check for horizontal and vertical matches
        MarkHorizontalMatches(toExplode);
        MarkVerticalMatches(toExplode);

		// Process the matches and trigger explosions
        for (int y = 0; y < GRID_HEIGHT; y++)
        {
            for (int x = 0; x < GRID_WIDTH; x++)
            {
				if (toExplode[y][x]) // If a match is marked for explosion
                {
					TriggerExplosion(x, y, grid[y][x]); // Trigger explosion animation
					grid[y][x] = -1; // Set the candy to -1 to mark it as exploded
					matchFound = true; // Set match found flag to true
                }
            }
        }

		if (matchFound) // If any matches were found
        {
            PlayAudio(EXPLOSION_SOUND);
			ProcessCandyFalls(); // Process the candy falls after matches
        }

    } while (matchFound);
}

// Function to process candy falls after matches
void ProcessCandyFalls()
{
    for (int x = 0; x < GRID_WIDTH; x++)
    {
        for (int y = GRID_HEIGHT - 1; y >= 0; y--)
        {
			if (grid[y][x] == -1) // If the current cell is empty (exploded candy)
            {
				for (int k = y - 1; k >= 0; k--) // Check above cells for candies
                {
					if (grid[k][x] != -1) // If a candy is found above
                    {
						grid[y][x] = grid[k][x]; // Move the candy down to the empty cell
						grid[k][x] = -1; // Mark the original cell as empty
						visualY[y][x] = visualY[k][x]; // Update visual position for the moved candy
						visualX[y][x] = (float)x; // Keep the x position the same
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

// Function to update the swap animation
void UpdateSwap()
{
    if (swapAnim.swapActive)
    {
		float dt = GetFrameTime(); // Get the delta time for the frame
		swapAnim.swapProgress += dt / SWAP_ANIMATION_DURATION; // Update the swap progress based on the delta time

		if (swapAnim.swapProgress >= 1.0f) // If the swap animation is complete
        {
			// Swap the candies in the grid
			int temp = grid[swapAnim.swapY1][swapAnim.swapX1]; // Temporary variable to hold the candy type
			grid[swapAnim.swapY1][swapAnim.swapX1] = grid[swapAnim.swapY2][swapAnim.swapX2]; // Swap the candies in the grid
			grid[swapAnim.swapY2][swapAnim.swapX2] = temp; // Set the candy type in the second position to the temporary variable

			// Swap the visual positions of the candies
			float tempX = visualX[swapAnim.swapY1][swapAnim.swapX1]; // Temporary variable to hold the visual x position
			float tempY = visualY[swapAnim.swapY1][swapAnim.swapX1]; // Temporary variable to hold the visual y position
			visualX[swapAnim.swapY1][swapAnim.swapX1] = visualX[swapAnim.swapY2][swapAnim.swapX2]; // Swap the visual x positions
			visualY[swapAnim.swapY1][swapAnim.swapX1] = visualY[swapAnim.swapY2][swapAnim.swapX2]; // Swap the visual y positions
			visualX[swapAnim.swapY2][swapAnim.swapX2] = tempX; // Set the visual x position in the second position to the temporary variable
			visualY[swapAnim.swapY2][swapAnim.swapX2] = tempY; // Set the visual y position in the second position to the temporary variable

			swapAnim.swapActive = 0; // Reset the swap active flag
			swapAnim.swapProgress = 0.0f; // Reset the swap progress
        }
        else
        {
			// Update the visual positions of the candies during the swap animation
			float t = swapAnim.swapProgress; // Get the progress of the swap animation (0 to 1)
			visualX[swapAnim.swapY1][swapAnim.swapX1] = (1 - t) * swapAnim.swapX1 + t * swapAnim.swapX2; // Interpolate the x position for the first candy
			visualY[swapAnim.swapY1][swapAnim.swapX1] = (1 - t) * swapAnim.swapY1 + t * swapAnim.swapY2; // Interpolate the y position for the first candy
			visualX[swapAnim.swapY2][swapAnim.swapX2] = (1 - t) * swapAnim.swapX2 + t * swapAnim.swapX1; // Interpolate the x position for the second candy
			visualY[swapAnim.swapY2][swapAnim.swapX2] = (1 - t) * swapAnim.swapY2 + t * swapAnim.swapY1; // Interpolate the y position for the second candy
        }
    }
}

// Function to trigger an explosion animation
void TriggerExplosion(int x, int y, int candyType)
{
	for (int i = 0; i < MAX_EXPLOSIONS; i++) // Check for an available explosion animation slot
    {
		if (!explosionAnim[i].active) // If the explosion animation slot is not active
        {
			explosionAnim[i].x = x; // Set the x position of the explosion
			explosionAnim[i].y = y; // Set the y position of the explosion
			explosionAnim[i].candyType = candyType; // Set the candy type for the explosion
			explosionAnim[i].explosionProgress = 0.0f; // Reset the explosion progress for the animation
			explosionAnim[i].active = 1; // Set the explosion animation as active
            break;
        }
    }
}

// Function to update explosion animations
void UpdateExplosion(float delta)
{
    for (int i = 0; i < MAX_EXPLOSIONS; i++)
    {
		if (explosionAnim[i].active) // If the explosion animation is active
        {
			explosionAnim[i].explosionProgress += delta; // Update the explosion progress based on the delta time
            if (explosionAnim[i].explosionProgress >= EXPLOSION_ANIMATION_DURATION) // If the explosion animation is complete
            {
				explosionAnim[i].active = 0; // Set the explosion animation as inactive
            }
        }
    }
}

// Function to check the game status and update the game state
void CheckGameStatus()
{
	if (gameStats.current_score >= gameStats.target_score) // If the current score is greater than or equal to the target score
    {
        winState = WIN;
        PlayAudio(WIN_SOUND);

        // Check for level completion
		for (int i = 1; i <= 5; i++) 
        {
            if (gameStats.current_level == i && gameStats.target_score == 750 + 250 * i) // Check if the current level matches the target score for that level
            {
				gameStats.current_level++; // Increment the current level
				SaveGameProgress(gameStats.current_level); // Save the game progress
            }
        }
    }
	else if (gameStats.current_moves_left <= 0) // If there are no moves left
    {
        winState = LOSE;
        PlayAudio(LOSE_SOUND);
    }

	// Check if the game is completed
	if (gameStats.current_moves_left <= 0 || gameStats.current_score >= gameStats.target_score) // If the game is over
    {
        gameState = AFTER_PLAY;
		if (winState == WIN && gameStats.current_level > 5) // If the player has completed all levels
            gameState = GAME_COMPLETED;
    }
}

// Function for button click handling
void IsButtonClicked(Rectangle button, ButtonState buttonState, GameState setGameState, ControlType controlType, int setLevel)
{
	Vector2 mousePos = GetMousePosition(); // Getting the mouse position
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
                    if (setLevel != -1) // If it is -1, it means that button should not change target score
                    {
                        gameStats.target_score = TARGET_SCORE[setLevel]; // Setting the target score for the selected level
                    }


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

// Functions for game progress saving and loading
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

// Function to deinitialize the game and free resources
void DeinitGame()
{
    UnloadTextures(); // Unloading textures
    UnloadAudio(); // Unloading audio files
    CloseAudioDevice();
    CloseWindow();
}