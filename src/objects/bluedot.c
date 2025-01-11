#include "../snake.h"

void PlaceBlueDot(
    BlueDot *blueDot,
    Snake *snake
)
{
    int dotOutsideOfSnake = 1;
    do
    {
        blueDot->x = rand() % BOARD_SECTION_WIDTH; // random number between 0 and board width
        blueDot->y = rand() % BOARD_SECTION_HEIGHT; // random number between 0 and board height
    } while (!dotOutsideOfSnake);
}

void RenderBlueDot(
    GameWindow *window,
    BlueDot *blueDot,
    const SDL_Rect *const boardRect
)
{
    SDL_Rect blueDotRect;
    blueDotRect.x = boardRect->x + blueDot->x * SNAKE_SEGMENT_SIZE;
    blueDotRect.y = boardRect->y + blueDot->y * SNAKE_SEGMENT_SIZE;
    blueDotRect.w = SNAKE_SEGMENT_SIZE;
    blueDotRect.h = SNAKE_SEGMENT_SIZE;

    SDL_SetRenderDrawColor(window->renderer, 26, 95, 180, 255); // fill with blue color
    SDL_RenderFillRect(window->renderer, &blueDotRect);
}
