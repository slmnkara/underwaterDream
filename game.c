#include "game.h"
#include "definitions.h"

void InitGame()
{
	// Setting up window and audio
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Underwater Dream");
    InitAudioDevice();

	SetWindowIcon(icon); // Setting the icon for the window
    LoadGameProgress(); // Reading data from the file to initialize the game state

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

void LoadTextures()
{
    icon = LoadImage("assets/images/icon.png");
    menuUI.wallpaper = LoadTexture("assets/images/wallpaper.png");
    candyTextures[0] = LoadTexture("assets/images/image1.png");
    candyTextures[1] = LoadTexture("assets/images/image2.png");
    candyTextures[2] = LoadTexture("assets/images/image3.png");
    candyTextures[3] = LoadTexture("assets/images/image4.png");
    candyTextures[4] = LoadTexture("assets/images/image5.png");
    completedUI.final = LoadTexture("assets/images/final.png");
}

void UnloadTextures()
{

}

void LoadGameProgress()
{
    FILE* file = fopen("assets/data.txt", "r");
    if (file == NULL)
    {
        printf("Error opening file!\n");
        gameStats.level = 1;
        return;
    }
    fscanf_s(file, "%d", &gameStats.level);
    fclose(file);
}
void SaveGameProgress(int level)
{
    FILE* file = fopen("assets/save.txt", "w");
    if (file == NULL)
    {
        printf("Error opening file!\n");
        return;
    }
    fprintf(file, "%d", level);
    fclose(file);
}

void LoadAudio()
{
    // Background music
    audio_t.backgroundMusic = LoadMusicStream("assets/audio/music.mp3");
    PlayMusicStream(audio_t.backgroundMusic);

    // Sound effects
    audio_t.button = LoadSound("assets/audio/button.mp3");
    audio_t.swap = LoadSound("assets/audio/swap.mp3");
    audio_t.explosion = LoadSound("assets/audio/explosion.mp3");
    audio_t.win = LoadSound("assets/audio/win.mp3");
    audio_t.lose = LoadSound("assets/audio/lose.mp3");

    // Set volumes
    SetMusicVolume(audio_t.backgroundMusic, 0.1f);
    SetSoundVolume(audio_t.button, 0.5f);
    SetSoundVolume(audio_t.swap, 0.2f);
    SetSoundVolume(audio_t.explosion, 0.1f);
    SetSoundVolume(audio_t.win, 0.8f);
    SetSoundVolume(audio_t.lose, 0.5f);
}

void UnloadAudio()
{
    // Unload background music and sounds
    UnloadMusicStream(audio_t.backgroundMusic);
    UnloadSound(audio_t.button);
    UnloadSound(audio_t.swap);
    UnloadSound(audio_t.explosion);
    UnloadSound(audio_t.win);
    UnloadSound(audio_t.lose);
}

void PlayAudio(Audio audio)
{
    if (soundState == SOUND_ON)
    {
        switch (audio)
        {
        case BACKGROUND_MUSIC:
            if (musicState == MUSIC_ON)
            {
                UpdateMusicStream(audio_t.backgroundMusic);
            }
            break;
        case BUTTON_ACTIVE:
            PlaySound(audio_t.button);
            break;
        case BUTTON_DEACTIVE:
            // No sound for deactive button
            break;
        case SWAP:
            PlaySound(audio_t.swap);
            break;
        case EXPLOSION:
            PlaySound(audio_t.explosion);
            break;
        case WIN:
            PlaySound(audio_t.win);
            break;
        case LOSE:
            PlaySound(audio_t.lose);
            break;
        }
    }
}