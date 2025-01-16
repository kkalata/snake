#include "../snake.h"

Uint32 RedDotAppearDelay()
{
    return rand() % (RED_DOT_MAX_APPEAR_DELAY - RED_DOT_MIN_APPEAR_DELAY) + RED_DOT_MIN_APPEAR_DELAY;
}

void PlaceRedDot(
    RedDot *redDot,
    Snake *snake,
    BlueDot *blueDot,
    GameTimer timer
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
    RedDot *redDot,
    GameTimer timer
)
{
    redDot->appearTime = timer.timeElapsed + RedDotAppearDelay();;
    redDot->visible = 0;
    redDot->snakeBehavior = (SnakeBehavior) (rand() % BEHAVIOR_COUNT);
}

void RenderRedDot(
    GameWindow *window,
    RedDot *redDot
)
{
    SDL_SetRenderDrawColor(window->renderer, 192, 28, 40, 255); // fill with red color
    RenderDot(window, redDot->pos);
}
