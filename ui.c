
#include "ui.h"


void InitUI()
{
	// MenuUI
    menuUI.playButton = (Rectangle){ 300,300,200,50 };
    menuUI.musicButton = (Rectangle){ 325,410,150,50 };
    menuUI.soundButton = (Rectangle){ 325,490,150,50 };
    menuUI.resetButton = (Rectangle){ 360,570,80,50 };

    // levelUI
    levelUI.LevelButton[0] = (Rectangle){ 300, 275, 200, 50 };
    levelUI.LevelButton[1] = (Rectangle){ 300, 375, 200, 50 };
    levelUI.LevelButton[2] = (Rectangle){ 300, 475, 200, 50 };
    levelUI.LevelButton[3] = (Rectangle){ 300, 575, 200, 50 };
    levelUI.LevelButton[4] = (Rectangle){ 300, 675, 200, 50 };

    // GameUI
    inGameUI.levelRec = (Rectangle){ 4,4,194,52 };
    inGameUI.scoreRec = (Rectangle){ 202,4,296,52 };
    inGameUI.movesLeftRec = (Rectangle){ 502,4,294,52 };
    inGameUI.uiBackground = (Rectangle){ 0,0,800,60 };
    inGameUI.menuButton = (Rectangle){ 5,5,390,50 };
    inGameUI.musicButton = (Rectangle){ 400,5,100,50 };
    inGameUI.soundButton = (Rectangle){ 505,5,100,50 };

    // afterGameUI
    afterGameUI.menuButton = (Rectangle){ 200,400,400,100 };
    afterGameUI.restartButton = (Rectangle){ 275,600,250,75 };
    afterGameUI.nextLevelButton = (Rectangle){ 275,600,250,75 };

    // CompletedUI
    completedUI.menuButton = (Rectangle){ 250,650,300,50 };
}

void UpdateMenu()
{
    PlayAudio(BACKGROUND_MUSIC); // Updates background music stream

	ClearBackground(RAYWHITE); // Clear the screen with a white background
    
    // Drawing wallpaper, buttons and labels for main menu.
    DrawTexture(mainMenuUI.wallpaper, 0, 0, WHITE);
    DrawRectangleRec(mainMenuUI.playButton, DARKPURPLE);
    DrawRectangleRec(mainMenuUI.musicButton, DARKBLUE);
    DrawRectangleRec(mainMenuUI.soundButton, DARKBLUE);
    DrawRectangleRec(mainMenuUI.resetButton, BLACK);

    DrawText("Underwater Dream", 200, 120, 42, DARKPURPLE);
    DrawText("Similar to Candy Crush Saga. Coded with Raylib.", 150, 170, 21, BLACK);
    DrawText("Start!", mainMenuUI.playButton.x + 65, mainMenuUI.playButton.y + 15, 25, WHITE);
    DrawText("Music", mainMenuUI.musicButton.x + 20, mainMenuUI.musicButton.y + 17, 18, WHITE);
    DrawText("Sound", mainMenuUI.soundButton.x + 20, mainMenuUI.soundButton.y + 17, 18, WHITE);
    DrawText("Reset\nLevel", mainMenuUI.resetButton.x + 13, mainMenuUI.resetButton.y + 5, 20, WHITE);

	// Control of ON-OFF labels depending on the music and sound state
    if (musicState == MUSIC_ON)
        DrawText("ON", mainMenuUI.musicButton.x + 100, mainMenuUI.musicButton.y + 17, 18, GREEN);
    else
        DrawText("OFF", mainMenuUI.musicButton.x + 100, mainMenuUI.musicButton.y + 17, 18, RED);

    if (soundState == SOUND_ON)
        DrawText("ON", mainMenuUI.soundButton.x + 100, mainMenuUI.soundButton.y + 17, 18, GREEN);
    else
        DrawText("OFF", mainMenuUI.soundButton.x + 100, mainMenuUI.soundButton.y + 17, 18, RED);

	// Displaying the current level
    char info[128];
    snprintf(info, sizeof(info), "Current Level: %d", gameStats.level);
    DrawText(info, 15, 830, 20, BLACK);

    IsButtonLeftClicked(mainMenuUI.playButton, ACTIVE, LEVEL_SELECTION, NONE, 0);
    IsButtonLeftClicked(mainMenuUI.musicButton, ACTIVE, BEFORE_PLAY, MUSIC, 0);
    IsButtonLeftClicked(mainMenuUI.soundButton, ACTIVE, BEFORE_PLAY, SOUND, 0);
    IsButtonLeftClicked(mainMenuUI.resetButton, ACTIVE, BEFORE_PLAY, RESET_LEVEL, 0);
}

void UpdateLevelSelection()
{
    UpdateMusic();
    ClearBackground(RAYWHITE);
    // Drawing labels
    DrawText("Level Selection", SCREEN_WIDTH / 8, SCREEN_HEIGHT / 10, 80, DARKPURPLE);
    DrawText("Select a level to play!", SCREEN_WIDTH / 4 + 30, SCREEN_HEIGHT / 5, 30, DARKPURPLE);

    for (int i = gameStats.level; i <= 4; i++)
    {
        DrawRectangleRec(levelUI.LevelButton[i], RED);
        DrawText("LOCKED!", levelUI.LevelButton[i].x + 50, levelUI.LevelButton[i].y + 13, 25, WHITE);
        IsButtonLeftClicked(levelUI.LevelButton[i], DEACTIVE, LEVEL_SELECTION, NONE, 0);
    }

    // Drawing buttons and their labels
    char levelNum[32] = { 0 };
    int tempLevel = 1;
    for (int i = 0; i < gameStats.level; i++)
    {
        DrawRectangleRec(levelUI.LevelButton[i], DARKPURPLE);
        snprintf(levelNum, sizeof(levelNum), "Level %d", tempLevel);
        DrawText(levelNum, levelUI.LevelButton[i].x + 60, levelUI.LevelButton[i].y + 13, 25, WHITE);
        tempLevel += 1;
        IsButtonLeftClicked(levelUI.LevelButton[i], ACTIVE, PLAYING, SET_LEVEL, i);
    }
}

void UpdateGameplay()
{
    UpdateMusic();
    ClearBackground(RAYWHITE);
    UpdateInGameUI();
    UpdateGrid();
    if (!swapAnim.swapActive) Dragging();
    CheckMatches();
    UpdateExplosion(GetFrameTime());
    UpdateSwap();
    GetResult();
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
            DrawText("Next Level!", afterGameUI.nextLevelButton.x + 53, afterGameUI.nextLevelButton.y + 22, 28, LIGHTGRAY);
        }

        // Drawing labels
        DrawText("YOU WIN!", SCREEN_WIDTH / 5 + 15, SCREEN_HEIGHT / 5, 100, RAYWHITE);
        DrawText("Return to the main menu", afterGameUI.menuButton.x + 17, afterGameUI.menuButton.y + 33, 30, DARKPURPLE);

        // Next level button control
        switch (gameStats.level)
        {
        case 2:
            IsButtonLeftClicked(afterGameUI.nextLevelButton, ACTIVE, PLAYING, SET_LEVEL, 1);
            break;
        case 3:
            IsButtonLeftClicked(afterGameUI.nextLevelButton, ACTIVE, PLAYING, SET_LEVEL, 2);
            break;
        case 4:
            IsButtonLeftClicked(afterGameUI.nextLevelButton, ACTIVE, PLAYING, SET_LEVEL, 3);
            break;
        case 5:
            IsButtonLeftClicked(afterGameUI.nextLevelButton, ACTIVE, PLAYING, SET_LEVEL, 4);
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
        DrawText("Return to the main menu", afterGameUI.menuButton.x + 17, afterGameUI.menuButton.y + 33, 30, DARKPURPLE);
        DrawText("Try again!", afterGameUI.restartButton.x + 57, afterGameUI.restartButton.y + 22, 28, LIGHTGRAY);

        // Restart button control
        IsButtonLeftClicked(afterGameUI.restartButton, ACTIVE, PLAYING, SET_LEVEL, gameStats.level - 1);
    }

    // Main menu button control
    IsButtonLeftClicked(afterGameUI.menuButton, ACTIVE, BEFORE_PLAY, NONE, 0);
}

void UpdateCompleted()
{
    ClearBackground(DARKPURPLE);
    DrawRectangleRec(completedUI.menuButton, LIGHTGRAY);
    DrawTextureEx(completedUI.final, (Vector2) { 180, 275 }, -13.0f, 0.4f, WHITE);
    DrawText("Congratulations!", 160, 130, 60, WHITE);
    DrawText("The game is completed!", 230, 200, 30, WHITE);
    DrawText("Return to the main menu.", completedUI.menuButton.x + 20, completedUI.menuButton.y + 15, 20, DARKPURPLE);

    // 
    IsButtonLeftClicked(completedUI.menuButton, ACTIVE, BEFORE_PLAY, NONE, 0);
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