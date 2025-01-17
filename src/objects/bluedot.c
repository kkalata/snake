#include "../snake.h"

void PlaceBlueDot(
    BlueDot *blueDot,
    Snake *snake,
    RedDot *redDot
)
{
    PlaceDot(&blueDot->pos, snake, redDot->pos);
}

void RenderBlueDot(
    GameWindow *window,
    BlueDot *blueDot
)
{
    SDL_SetRenderDrawColor(window->renderer, 26, 95, 180, 255); // fill with blue color
    RenderDot(window, blueDot->pos);
}
