#include "../snake.h"

Uint32 RedDotAppearDelay()
{
    return rand() % (RED_DOT_MAX_APPEAR_DELAY - RED_DOT_MIN_APPEAR_DELAY) + RED_DOT_MIN_APPEAR_DELAY;
}

void PlaceRedDot(
    RedDot *const redDot,
    const Snake *const snake,
    const BlueDot *const blueDot,
    const GameTimer timer
)
{
    if (!redDot->visible)
    {
        if (timer.timeElapsed >= redDot->appearTime)
        {
            redDot->visible = 1;
            PlaceDot(&redDot->pos, snake, blueDot->pos);
        }
    }
    else
    {
        if (timer.timeElapsed >= redDot->appearTime + RED_DOT_DISPLAY_TIME)
        {
            SetRedDotParams(redDot, timer);
        }
    }
}

void SetRedDotParams(
    RedDot *const redDot,
    const GameTimer timer
)
{
    redDot->appearTime = timer.timeElapsed + RedDotAppearDelay();;
    redDot->visible = 0;
    redDot->snakeBehavior = (SnakeBehavior) (rand() % BEHAVIOR_COUNT);
}
