#include "definitions.h"

int main(void)
{
    InitGame(); // Initializes the game
    while (!WindowShouldClose())
    {
        BeginDrawing();
        switch (gameState) // Updates the screen depending on gameState
        {
        case BEFORE_PLAY:
            UpdateMainMenuScreen();
            break;
        case LEVEL_SELECTION:
            UpdateLevelSelectionScreen();
            break;
        case PLAYING:
            UpdateGameplayScreen();
            break;
        case AFTER_PLAY:
            UpdateAfterGameScreen();
            break;
        case GAME_COMPLETED:
            UpdateGameCompletedScreen();
            break;
        }
        EndDrawing();
    }
	DeinitGame(); // Deinitializes the game
    return 0;
}