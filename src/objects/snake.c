#include "../snake.h"

void CreateSnake(
    Snake *const snake
)
{
    snake->segment = NULL;
    snake->killed = ALIVE;
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
        if (snakeSegment->x == BOARD_SECTION_WIDTH - 1)
        {
            snakeSegment->turn = 'l';
        }
        else
        {
            snakeSegment->turn = 'r';
        }
    }
    else if (snakeSegment->direction == 'd' && snakeSegment->y == BOARD_SECTION_HEIGHT - 1)
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
    else if (snakeSegment->direction == 'r' && snakeSegment->x == BOARD_SECTION_WIDTH - 1)
    {
        if (snakeSegment->y == BOARD_SECTION_HEIGHT - 1)
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
        KillSnake(snake);
        if (!snake->killed)
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
        else
        {
            break;
        }
    }
}

SnakeSegment *GetMovedSnakeSegment(
    SnakeSegment *snakeSegment
)
{
    SnakeSegment *movedSnakeSegment = (SnakeSegment *) malloc(sizeof(SnakeSegment));
    memcpy(movedSnakeSegment, snakeSegment, sizeof(SnakeSegment));
    MoveSnakeSegment(movedSnakeSegment);
}

void KillSnake(
    Snake *snake
)
{
    // The kill conditions are checked prospectively,
    // therefore move the snake segments indepedently from the rest of the snake
    SnakeSegment *firstSnakeSegmentMoved = GetMovedSnakeSegment(snake->segment);
    
    if (firstSnakeSegmentMoved->x < 0 || firstSnakeSegmentMoved->x > BOARD_SECTION_WIDTH - 1 ||
        firstSnakeSegmentMoved->y < 0 || firstSnakeSegmentMoved->y > BOARD_SECTION_HEIGHT - 1)
    {
        snake->killed = HIT_WALL;
    }
    else {
        SnakeSegment *snakeSegment = snake->segment->next;
        do
        {
            if (
                firstSnakeSegmentMoved->x == snakeSegment->x
                && firstSnakeSegmentMoved->y == snakeSegment->y
            )
            {
                // There is a chance that the head of the snake occupies the space just freed by its tails
                if (snakeSegment->next == snake->segment)
                {   
                    SnakeSegment *lastSnakeSegmentMoved = GetMovedSnakeSegment(snakeSegment);
                    if (
                        firstSnakeSegmentMoved->x == lastSnakeSegmentMoved->x
                        && firstSnakeSegmentMoved->y == lastSnakeSegmentMoved->y
                    )
                    {
                        snake->killed = HIT_ITSELF;
                    }
                    free(lastSnakeSegmentMoved);
                }
                else
                {
                    snake->killed = HIT_ITSELF;
                }
            }
            if (snake->killed)
            {
                break;
            }
            snakeSegment = snakeSegment->next;
        } while (snakeSegment != snake->segment);
    }

    free(firstSnakeSegmentMoved);
}

void RenderSnake(
    GameWindow *window,
    const Snake *const snake,
    const SDL_Rect *const boardRect
)

{
    SDL_Rect snakeSegmentRect;
    snakeSegmentRect.w = SNAKE_SEGMENT_SIZE;
    snakeSegmentRect.h = SNAKE_SEGMENT_SIZE;
    
    SnakeSegment *snakeSegment = snake->segment->previous;
    do
    {
        snakeSegmentRect.x = boardRect->x + snakeSegment->x * SNAKE_SEGMENT_SIZE;
        snakeSegmentRect.y = boardRect->y + snakeSegment->y * SNAKE_SEGMENT_SIZE;
        SDL_RenderCopy(window->renderer, window->snakeSkin, NULL, &snakeSegmentRect);
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
    snake->killed = ALIVE;
    snake->timeSinceLastMove = 0;
}
