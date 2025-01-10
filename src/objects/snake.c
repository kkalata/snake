#include "../snake.h"

void CreateSnake(
    Snake *const snake
)
{
    snake->segment = NULL;
    snake->timeSinceLastMove = 0;

    SnakeSegment *snakeSegment = NULL;
    SnakeSegment *previousSnakeSegment = NULL;

    for (int snakeSegmentI = 0; snakeSegmentI < SNAKE_INIT_LENGTH; snakeSegmentI++)
    {
        snakeSegment = (SnakeSegment *) malloc(sizeof(SnakeSegment));

        snakeSegment->next = snake->segment;
        if (snake->segment == NULL)
        {
            snake->segment = snakeSegment;
            snakeSegment->previous = snakeSegment;
        }
        else
        {
            previousSnakeSegment->next = snakeSegment;
            snake->segment->previous = snakeSegment;

            snakeSegment->previous = previousSnakeSegment;
        }

        snakeSegment->x = SNAKE_INIT_POS_X + snakeSegmentI;
        snakeSegment->y = SNAKE_INIT_POS_Y;
        snakeSegment->direction = SNAKE_INIT_DIRECTION;
        snakeSegment->turn = '\0';
        
        previousSnakeSegment = snakeSegment;
    }
}

void TurnSnake(
    Snake *snake,
    Uint32 direction
)
{
    char snakeTurn = '\0';
    switch (direction)
    {
        case SDLK_UP:
            snakeTurn = 'u';
            break;
        case SDLK_DOWN:
            snakeTurn = 'd';
            break;
        case SDLK_LEFT:
            snakeTurn = 'l';
            break;
        case SDLK_RIGHT:
            snakeTurn = 'r';
            break;
    }
    snake->segment->turn = snakeTurn;
}

void MoveSnake(
    Snake *snake,
    GameTimer timer
)
{
    snake->timeSinceLastMove += timer.timeDelta;
    while (snake->timeSinceLastMove >= SNAKE_COOLDOWN)
    {
        SnakeSegment *snakeSegment = snake->segment;
        do
        {   
            if (snakeSegment->turn)
            {
                snakeSegment->direction = snakeSegment->turn;
            }
            switch (snakeSegment->direction)
            {
                case 'u':
                    snakeSegment->y--;
                    break;
                case 'd':
                    snakeSegment->y++;
                    break;
                case 'l':
                    snakeSegment->x--;
                    break;
                case 'r':
                    snakeSegment->x++;
                    break;
            }
            snakeSegment = snakeSegment->next;
        } while (snakeSegment != snake->segment);

        snakeSegment = snake->segment->previous;
        while (snakeSegment != snake->segment)
        {
            snakeSegment->turn = snakeSegment->previous->turn;
            snakeSegment = snakeSegment->previous;
        }
        snake->segment->turn = '\0';
        
        snake->timeSinceLastMove -= SNAKE_COOLDOWN;
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

    SnakeSegment *snakeSegment = snake->segment;
    do
    {
        snakeSegmentRect.x = snakeSegment->x * SNAKE_SEGMENT_SIZE;
        snakeSegmentRect.y = snakeSegment->y * SNAKE_SEGMENT_SIZE;
        SDL_RenderFillRect(window->renderer, &snakeSegmentRect);

        snakeSegment = snakeSegment->next;
    } while (snakeSegment != snake->segment);    
}
