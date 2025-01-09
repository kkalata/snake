#include "../snake.h"

void CreateSnake(
    Snake *const snake
)
{
    snake->segment = NULL;

    SnakeSegment *snakeSegment = NULL;
    SnakeSegment *previousSnakeSegment = NULL;

    for (int snakeSegmentI = 0; snakeSegmentI < SNAKE_INIT_LENGTH; snakeSegmentI++)
    {
        snakeSegment = (SnakeSegment *) malloc(sizeof(SnakeSegment));
        if (snake->segment == NULL)
        {
            snake->segment = snakeSegment;
        }

        snakeSegment->x = snakeSegmentI;
        snakeSegment->y = 0;

        snakeSegment->next = NULL;
        
        if (previousSnakeSegment != NULL)
        {
            previousSnakeSegment->next = snakeSegment;
        }
        previousSnakeSegment = snakeSegment;
    }
}

void RenderSnake(
    GameWindow *window,
    const Snake *const snake
)
{
    SDL_Rect snakeSegmentRect;
    snakeSegmentRect.w = SNAKE_SEGMENT_SIZE;
    snakeSegmentRect.h = SNAKE_SEGMENT_SIZE;
    SDL_SetRenderDrawColor(window->renderer, 229, 165, 10, 255); // fill with yellow color

    for (SnakeSegment *snakeSegment = snake->segment; snakeSegment != NULL; snakeSegment = snakeSegment->next)
    {
        snakeSegmentRect.x = snakeSegment->x * SNAKE_SEGMENT_SIZE;
        snakeSegmentRect.y = snakeSegment->y * SNAKE_SEGMENT_SIZE;
        SDL_RenderFillRect(window->renderer, &snakeSegmentRect);
    }
    
}
