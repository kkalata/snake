#include "../snake.h"

void CreateSnake(
    Snake *const snake
)
{
    snake->segment = NULL;
    snake->killed = 0;
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
    switch (direction)
    {
        case SDLK_UP:
            if (snake->segment->direction != 'd')
            {
                snake->segment->turn = 'u';
            }
            break;
        case SDLK_DOWN:
            if (snake->segment->direction != 'u')
            {
                snake->segment->turn = 'd';
            }
            break;
        case SDLK_LEFT:
            if (snake->segment->direction != 'r')
            {
                snake->segment->turn = 'l';
            }
            break;
        case SDLK_RIGHT:
            if (snake->segment->direction != 'l')
            {
                snake->segment->turn = 'r';
            }
            break;
    }
}

void AutoTurnSnakeSegment(
    SnakeSegment *snakeSegment
)
{
    if (snakeSegment->direction == 'u' && snakeSegment->y == 0)
    {
        if (snakeSegment->x == BOARD_SECTION_WIDTH)
        {
            snakeSegment->turn = 'l';
        }
        else
        {
            snakeSegment->turn = 'r';
        }
    }
    else if (snakeSegment->direction == 'd' && snakeSegment->y == BOARD_SECTION_HEIGHT)
    {
        if (snakeSegment->x == 0)
        {
            snakeSegment->turn = 'r';
        }
        else
        {
            snakeSegment->turn = 'l';
        }
    }
    else if (snakeSegment->direction == 'l' && snakeSegment->x == 0)
    {
        if (snakeSegment->y == 0)
        {
            snakeSegment->turn = 'd';
        }
        else
        {
            snakeSegment->turn = 'u';
        }
    }
    else if (snakeSegment->direction == 'r' && snakeSegment->x == BOARD_SECTION_WIDTH)
    {
        if (snakeSegment->y == BOARD_SECTION_HEIGHT)
        {
            snakeSegment->turn = 'u';
        }
        else
        {
            snakeSegment->turn = 'd';
        }
    }
}

void MoveSnakeSegment(
    SnakeSegment *snakeSegment
)
{
    if (!snakeSegment->turn)
    {
        AutoTurnSnakeSegment(snakeSegment);
    }
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
            MoveSnakeSegment(snakeSegment);
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

void KillSnake(
    Snake *snake
)
{
    SnakeSegment *firstSnakeSegment = snake->segment;
    SnakeSegment *snakeSegment = snake->segment->next;
    do
    {
        if (firstSnakeSegment->x == snakeSegment->x && firstSnakeSegment->y == snakeSegment->y)
        {
            snake->killed = 1;
        }
        snakeSegment = snakeSegment->next;
    } while (snakeSegment != firstSnakeSegment);
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
    
    SnakeSegment *snakeSegment = snake->segment->previous;
    do
    {
        snakeSegmentRect.x = snakeSegment->x * SNAKE_SEGMENT_SIZE;
        snakeSegmentRect.y = snakeSegment->y * SNAKE_SEGMENT_SIZE;
        SDL_RenderFillRect(window->renderer, &snakeSegmentRect);
        snakeSegment = snakeSegment->previous;
    } while (snakeSegment->next != snake->segment);
}

void DestroySnake(
    Snake *snake
)
{
    SnakeSegment *snakeSegment = snake->segment;
    SnakeSegment *nextSnakeSegment;
    do
    {
        nextSnakeSegment = snakeSegment->next;
        free(snakeSegment);
        snakeSegment = nextSnakeSegment;
    } while (snakeSegment != snake->segment);
    snake->segment = NULL;
    snake->killed = 0;
    snake->timeSinceLastMove = 0;
}
