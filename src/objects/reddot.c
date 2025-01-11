#include "../snake.h"

Uint32 RedDotAppearDelay()
{
    return rand() % (RED_DOT_MAX_APPEAR_DELAY - RED_DOT_MIN_APPEAR_DELAY) + RED_DOT_MIN_APPEAR_DELAY;
}

void PlaceRedDot(
    RedDot *redDot,
    Snake *snake,
    GameTimer timer
)
{
    redDot->pos = PlaceDot(snake);
    redDot->appearTime = timer.timeElapsed + RedDotAppearDelay();
}

int RedDotVisible(
    Uint32 appearTime,
    const GameTimer timer
)
{
    return appearTime >= timer.timeElapsed && appearTime < timer.timeElapsed + RED_DOT_DISPLAY_TIME;
}

void RenderRedDot(
    GameWindow *window,
    RedDot *redDot,
    const SDL_Rect *const boardRect
)
{
    SDL_SetRenderDrawColor(window->renderer, 192, 28, 40, 255); // fill with red color
    RenderDot(window, redDot->pos, boardRect);
}
