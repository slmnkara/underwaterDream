#ifndef EFFECTS_H
#define EFFECTS_H

#include "stdio.h"
#include "raylib.h"
#include "game.h"

// Initializing animation durations
#define EXPLOSION_ANIMATION_DURATION 1.0f
#define SWAP_ANIMATION_DURATION 0.5f
#define MAX_EXPLOSIONS 64

typedef struct
{
    int swapActive;
    int swapX1, swapY1, swapX2, swapY2;
    float swapProgress;
} SwapAnim; SwapAnim swapAnim;

typedef struct {
    int x, y;
    int active;
    float explosionProgress;
    int candyType;
} ExplosionAnim; ExplosionAnim explosionAnim[MAX_EXPLOSIONS];

float visualY[GRID_HEIGHT][GRID_WIDTH];
float visualX[GRID_HEIGHT][GRID_WIDTH];

void TriggerExplosion(int x, int y, int candyType);
void UpdateExplosion(float delta);
void UpdateSwap();

#endif