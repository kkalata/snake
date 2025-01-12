#include "../snake.h"

void CreateSnake(
    Snake *const snake
)
{
    snake->segment = NULL;
    snake->killed = ALIVE;
    snake->timeSinceLastMove = 0;
    snake->timeSinceLastSpeedup = 0;
    snake->cooldown = SNAKE_INIT_COOLDOWN;

    for (int snakeSegmentI = 0; snakeSegmentI < SNAKE_INIT_LENGTH; snakeSegmentI++)
    {
        CreateSnakeSegment(
            snake,
            SNAKE_INIT_POS_X + snakeSegmentI,
            SNAKE_INIT_POS_Y,
            SNAKE_INIT_DIRECTION
        );
    }
}

void CreateSnakeSegment(
    Snake *const snake,
    const int x,
    const int y,
    const int direction
)
{
    SnakeSegment *snakeSegment = NULL;

    snakeSegment = (SnakeSegment *) malloc(sizeof(SnakeSegment));

    AttachSnakeSegment(snake, snakeSegment);
    snakeSegment->pos.x = x;
    snakeSegment->pos.y = y;
    snakeSegment->direction = direction;
    snakeSegment->turn = '\0';
}

void AttachSnakeSegment(
    Snake *const snake,
    SnakeSegment *snakeSegment
)
{
    snakeSegment->next = snake->segment;
    if (snake->segment == NULL)
    {
        snake->segment = snakeSegment;
        snakeSegment->previous = snakeSegment;
    }
    else
    {
        snakeSegment->previous = snake->segment->previous;
        snake->segment->previous->next = snakeSegment;
        snake->segment->previous = snakeSegment;
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
    if (snakeSegment->direction == 'u' && snakeSegment->pos.y == 0)
    {
        if (snakeSegment->pos.x == BOARD_SECTION_WIDTH - 1)
        {
            snakeSegment->turn = 'l';
        }
        else
        {
            snakeSegment->turn = 'r';
        }
    }
    else if (snakeSegment->direction == 'd' && snakeSegment->pos.y == BOARD_SECTION_HEIGHT - 1)
    {
        if (snakeSegment->pos.x == 0)
        {
            snakeSegment->turn = 'r';
        }
        else
        {
            snakeSegment->turn = 'l';
        }
    }
    else if (snakeSegment->direction == 'l' && snakeSegment->pos.x == 0)
    {
        if (snakeSegment->pos.y == 0)
        {
            snakeSegment->turn = 'd';
        }
        else
        {
            snakeSegment->turn = 'u';
        }
    }
    else if (snakeSegment->direction == 'r' && snakeSegment->pos.x == BOARD_SECTION_WIDTH - 1)
    {
        if (snakeSegment->pos.y == BOARD_SECTION_HEIGHT - 1)
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
            snakeSegment->pos.y--;
            break;
        case 'd':
            snakeSegment->pos.y++;
            break;
        case 'l':
            snakeSegment->pos.x--;
            break;
        case 'r':
            snakeSegment->pos.x++;
            break;
    }
}

void MoveSnake(
    Snake *snake,
    BlueDot *blueDot,
    RedDot *redDot,
    GameTimer timer
)
{
    snake->timeSinceLastMove += timer.timeDelta;
    snake->timeSinceLastSpeedup += timer.timeDelta;
    while (snake->timeSinceLastMove >= snake->cooldown)
    {
        snake->timeSinceLastMove -= snake->cooldown;
        int blueDotEaten = EatBlueDot(snake, blueDot);
        if (blueDotEaten)
        {
            PlaceBlueDot(blueDot, snake, redDot);
        }
        int redDotEaten = EatRedDot(snake, redDot);
        if (redDotEaten)
        {
            PlaceRedDot(redDot, snake, blueDot, timer);
        }
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

            if (snake->timeSinceLastSpeedup >= SNAKE_SPEEDUP_INTERVAL)
            {
                snake->cooldown *= SNAKE_COOLDOWN_CHANGE_RATE;
                snake->timeSinceLastSpeedup -= SNAKE_SPEEDUP_INTERVAL;
            }
        }
        else
        {
            if (blueDotEaten)
            {
                DetachLastSnakeSegment(snake);
            }
            break;
        }
    }
}

int EatBlueDot(
    Snake *snake,
    BlueDot *blueDot
)
{
    if (snake->segment->pos.x == blueDot->pos.x && snake->segment->pos.y == blueDot->pos.y)
    {
        SnakeSegment *newSnakeSegment = GetSnakeSegment(snake->segment->previous, 0);
        switch (newSnakeSegment->direction)
        {
            case 'u':
                newSnakeSegment->pos.y++;
                break;
            case 'd':
                newSnakeSegment->pos.y--;
                break;
            case 'l':
                newSnakeSegment->pos.x++;
                break;
            case 'r':
                newSnakeSegment->pos.x--;
                break;
        }
        newSnakeSegment->turn = '\0';
        AttachSnakeSegment(snake, newSnakeSegment);
        return 1;
    }
    return 0;
}

int EatRedDot(
    Snake *snake,
    RedDot *redDot
)
{
    if (snake->segment->pos.x == redDot->pos.x && snake->segment->pos.y == redDot->pos.y)
    {
        switch (redDot->snakeBehavior)
        {
            case SNAKE_SLOWING_DOWN:
                snake->cooldown /= SNAKE_COOLDOWN_CHANGE_RATE;
                break;
            case SNAKE_SHORTENING:
                int snakeSegmentCount = 0;
                int snakeSegmentMinCount = SNAKE_INIT_LENGTH + RED_DOT_SNAKE_SEGMENT_DETACH_COUNT;
                SnakeSegment *snakeSegment = snake->segment;
                do
                {
                    snakeSegmentCount++;
                    snakeSegment = snakeSegment->next;
                } while (snakeSegment != snake->segment && snakeSegmentCount < snakeSegmentMinCount);
                if (snakeSegmentCount >= snakeSegmentMinCount)
                {
                    for (int snakeSegmentI = 0; snakeSegmentI < RED_DOT_SNAKE_SEGMENT_DETACH_COUNT; snakeSegmentI++)
                    {
                        DetachLastSnakeSegment(snake);
                    }
                }
                break;
        }
        return 1;
    }
    return 0;
}

SnakeSegment *GetSnakeSegment(
    SnakeSegment *snakeSegment,
    int moveSnakeSegment
)
{
    SnakeSegment *movedSnakeSegment = (SnakeSegment *) malloc(sizeof(SnakeSegment));
    memcpy(movedSnakeSegment, snakeSegment, sizeof(SnakeSegment));
    if (moveSnakeSegment)
    {
        MoveSnakeSegment(movedSnakeSegment);
    }
    return movedSnakeSegment;
}

void KillSnake(
    Snake *snake
)
{
    // The kill conditions are checked prospectively,
    // therefore move the snake segments indepedently from the rest of the snake
    SnakeSegment *firstSnakeSegmentMoved = GetSnakeSegment(snake->segment, 1);
    
    if (firstSnakeSegmentMoved->pos.x < 0 || firstSnakeSegmentMoved->pos.x > BOARD_SECTION_WIDTH - 1 ||
        firstSnakeSegmentMoved->pos.y < 0 || firstSnakeSegmentMoved->pos.y > BOARD_SECTION_HEIGHT - 1)
    {
        snake->killed = HIT_WALL;
    }
    else {
        SnakeSegment *snakeSegment = snake->segment->next;
        do
        {
            if (
                firstSnakeSegmentMoved->pos.x == snakeSegment->pos.x
                && firstSnakeSegmentMoved->pos.y == snakeSegment->pos.y
            )
            {
                // There is a chance that the head of the snake occupies the space just freed by its tails
                if (snakeSegment->next == snake->segment)
                {   
                    SnakeSegment *lastSnakeSegmentMoved = GetSnakeSegment(snakeSegment, 1);
                    if (
                        firstSnakeSegmentMoved->pos.x == lastSnakeSegmentMoved->pos.x
                        && firstSnakeSegmentMoved->pos.y == lastSnakeSegmentMoved->pos.y
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
        snakeSegmentRect.x = boardRect->x + snakeSegment->pos.x * SNAKE_SEGMENT_SIZE;
        snakeSegmentRect.y = boardRect->y + snakeSegment->pos.y * SNAKE_SEGMENT_SIZE;
        SDL_RenderCopy(window->renderer, window->snakeSkin, NULL, &snakeSegmentRect);
        snakeSegment = snakeSegment->previous;
    } while (snakeSegment->next != snake->segment);
}

void DetachLastSnakeSegment(
    Snake *snake
)
{
    SnakeSegment *lastSnakeSegment = snake->segment->previous;
    lastSnakeSegment->previous->next = snake->segment;
    snake->segment->previous = lastSnakeSegment->previous;
    free(lastSnakeSegment);
}

void DestroySnake(
    Snake *snake
)
{
    do
    {
        DetachLastSnakeSegment(snake);
    } while (snake->segment->next != snake->segment);
    free(snake->segment);
    snake->segment = NULL;
    snake->killed = ALIVE;
    snake->timeSinceLastMove = 0;
    snake->timeSinceLastSpeedup = 0;
    snake->cooldown = SNAKE_INIT_COOLDOWN;
}
