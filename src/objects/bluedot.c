#include "../snake.h"

Position PlaceDot(
    Snake *snake,
    Position otherDotPos
)
{
    Position pos;

    int dotUnderSomeObject;
    do
    {   
        dotUnderSomeObject = 0;
        pos.x = rand() % BOARD_SECTION_WIDTH; // random number between 0 and board width
        pos.y = rand() % BOARD_SECTION_HEIGHT; // random number between 0 and board height
        
        SnakeSegment *snakeSegment = snake->segment;
        do
        {
            if (snakeSegment->pos.x == pos.x && snakeSegment->pos.y == pos.y)
            {
                dotUnderSomeObject = 1;
                break;
            }
            snakeSegment = snakeSegment->next;
        } while (snakeSegment != snake->segment);

        if (otherDotPos.x == pos.x && otherDotPos.y == pos.y)
        {
            dotUnderSomeObject = 1;
        }
    } while (dotUnderSomeObject);

    return pos;
}

void PlaceBlueDot(
    BlueDot *blueDot,
    Snake *snake,
    RedDot *redDot
)
{
    blueDot->pos = PlaceDot(snake, redDot->pos);
}

void RenderDot(
    GameWindow *window,
    const Position pos,
    const SDL_Rect *const boardRect
)
{
    SDL_Rect dotRect;
    dotRect.x = boardRect->x + pos.x * SNAKE_SEGMENT_SIZE;
    dotRect.y = boardRect->y + pos.y * SNAKE_SEGMENT_SIZE;
    dotRect.w = SNAKE_SEGMENT_SIZE;
    dotRect.h = SNAKE_SEGMENT_SIZE;
    SDL_RenderFillRect(window->renderer, &dotRect);
}

void RenderBlueDot(
    GameWindow *window,
    BlueDot *blueDot,
    const SDL_Rect *const boardRect
)
{
    SDL_SetRenderDrawColor(window->renderer, 26, 95, 180, 255); // fill with blue color
    RenderDot(window, blueDot->pos, boardRect);
}
