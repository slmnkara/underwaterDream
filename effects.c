#include "effects.h"

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