#include "definitions.h"

int main(void)
{
    InitGame(); // Initializing game
    while (!WindowShouldClose())
    {
        BeginDrawing();
        switch (gameState) // Updating the screen depending on gameState
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
	DeinitGame(); // Deinitializing game
    return 0;
}