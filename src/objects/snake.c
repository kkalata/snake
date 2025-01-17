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
    const SnakeDirection direction
)
{
    SnakeSegment *snakeSegment = NULL;

    snakeSegment = (SnakeSegment *) malloc(sizeof(SnakeSegment));

    AttachSnakeSegment(snake, snakeSegment);
    snakeSegment->pos.x = x;
    snakeSegment->pos.y = y;
    snakeSegment->direction = direction;
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
            if (snake->segment->direction != SNAKE_DOWN)
            {
                snake->turn = SNAKE_UP;
            }
            break;
        case SDLK_DOWN:
            if (snake->segment->direction != SNAKE_UP)
            {
                snake->turn = SNAKE_DOWN;
            }
            break;
        case SDLK_LEFT:
            if (snake->segment->direction != SNAKE_RIGHT)
            {
                snake->turn = SNAKE_LEFT;
            }
            break;
        case SDLK_RIGHT:
            if (snake->segment->direction != SNAKE_LEFT)
            {
                snake->turn = SNAKE_RIGHT;
            }
            break;
    }
}

int IsSnakeHere(
    Snake *snake,
    Position pos
)
{
    SnakeSegment *snakeSegment = snake->segment;
    do
    {
        if (snakeSegment->pos.x == pos.x && snakeSegment->pos.y == pos.y)
        {
            return 1;
        }
        snakeSegment = snakeSegment->next;
    } while (snakeSegment != snake->segment);
    return 0;
}

void AutoTurnSnake(
    Snake *snake
)
{
    if (snake->turn)
    {
        return;
    }

    Position possibleSnakePos;
    possibleSnakePos.x = snake->segment->pos.x;
    possibleSnakePos.y = snake->segment->pos.y;
    if (snake->segment->direction == SNAKE_UP && snake->segment->pos.y == 0)
    {
        possibleSnakePos.x++;
        if (snake->segment->pos.x == BOARD_SECTION_WIDTH - 1 || IsSnakeHere(snake, possibleSnakePos))
        {
            snake->turn = SNAKE_LEFT;
        }
        else
        {
            snake->turn = SNAKE_RIGHT;
        }
    }
    else if (snake->segment->direction == SNAKE_DOWN && snake->segment->pos.y == BOARD_SECTION_HEIGHT - 1)
    {
        possibleSnakePos.x--;
        if (snake->segment->pos.x == 0  || IsSnakeHere(snake, possibleSnakePos))
        {
            snake->turn = SNAKE_RIGHT;
        }
        else
        {
            snake->turn = SNAKE_LEFT;
        }
    }
    else if (snake->segment->direction == SNAKE_LEFT && snake->segment->pos.x == 0)
    {
        possibleSnakePos.y--;
        if (snake->segment->pos.y == 0  || IsSnakeHere(snake, possibleSnakePos))
        {
            snake->turn = SNAKE_DOWN;
        }
        else
        {
            snake->turn = SNAKE_UP;
        }
    }
    else if (snake->segment->direction == SNAKE_RIGHT && snake->segment->pos.x == BOARD_SECTION_WIDTH - 1)
    {
        possibleSnakePos.y++;
        if (snake->segment->pos.y == BOARD_SECTION_HEIGHT - 1  || IsSnakeHere(snake, possibleSnakePos))
        {
            snake->turn = SNAKE_UP;
        }
        else
        {
            snake->turn = SNAKE_DOWN;
        }
    }
}

void MoveSnakeSegment(
    SnakeSegment *snakeSegment,
    SnakeDirection snakeTurn
)
{
    if (snakeTurn)
    {
        snakeSegment->direction = snakeTurn;
    }
    switch (snakeSegment->direction)
    {
        case SNAKE_UP:
            snakeSegment->pos.y--;
            break;
        case SNAKE_DOWN:
            snakeSegment->pos.y++;
            break;
        case SNAKE_LEFT:
            snakeSegment->pos.x--;
            break;
        case SNAKE_RIGHT:
            snakeSegment->pos.x++;
            break;
    }
}

void MoveSnake(
    Snake *snake
)
{
    SnakeSegment *snakeSegment = snake->segment;
    do
    {   
        MoveSnakeSegment(snakeSegment, snake->turn);
        snake->turn = SNAKE_NO_TURN;
        snakeSegment = snakeSegment->next;
    } while (snakeSegment != snake->segment);

    snakeSegment = snake->segment->previous;
    while (snakeSegment != snake->segment)
    {
        snakeSegment->direction = snakeSegment->previous->direction;
        snakeSegment = snakeSegment->previous;
    }
}

Uint32 AdvanceSnake(
    Snake *snake,
    BlueDot *blueDot,
    RedDot *redDot,
    GameTimer timer
)
{
    Uint32 pointsScored = 0;

    snake->timeSinceLastMove += timer.timeDelta;
    snake->timeSinceLastSpeedup += timer.timeDelta;
    while (snake->timeSinceLastMove >= snake->cooldown)
    {
        snake->timeSinceLastMove -= snake->cooldown;
        int blueDotEaten = EatBlueDot(snake, blueDot);
        if (blueDotEaten)
        {
            pointsScored += BLUE_DOT_POINTS;
            PlaceBlueDot(blueDot, snake, redDot);
        }
        int redDotEaten = EatRedDot(snake, redDot);
        if (redDotEaten)
        {
            pointsScored += RED_DOT_POINTS;
            SetRedDotParams(redDot, timer);
        }
        AutoTurnSnake(snake);
        KillSnake(snake);
        if (!snake->killed)
        {
            MoveSnake(snake);

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
    return pointsScored;
}

int EatBlueDot(
    Snake *snake,
    BlueDot *blueDot
)
{
    if (snake->segment->pos.x == blueDot->pos.x && snake->segment->pos.y == blueDot->pos.y)
    {
        SnakeSegment *newSnakeSegment = GetSnakeSegment(snake->segment->previous, 0, SNAKE_NO_TURN);
        switch (newSnakeSegment->direction)
        {
            case SNAKE_UP:
                newSnakeSegment->pos.y++;
                break;
            case SNAKE_DOWN:
                newSnakeSegment->pos.y--;
                break;
            case SNAKE_LEFT:
                newSnakeSegment->pos.x++;
                break;
            case SNAKE_RIGHT:
                newSnakeSegment->pos.x--;
                break;
        }
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
    if (redDot->visible && snake->segment->pos.x == redDot->pos.x && snake->segment->pos.y == redDot->pos.y)
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
                int snakeSegmentDetachCount = RED_DOT_SNAKE_SEGMENT_DETACH_COUNT;
                if (snakeSegmentCount < snakeSegmentMinCount)
                {
                    snakeSegmentDetachCount = snakeSegmentCount - RED_DOT_SNAKE_SEGMENT_DETACH_COUNT;
                }
                for (int snakeSegmentI = 0; snakeSegmentI < snakeSegmentDetachCount; snakeSegmentI++)
                {
                    DetachLastSnakeSegment(snake);
                }
                
                break;
        }
        return 1;
    }
    return 0;
}

SnakeSegment *GetSnakeSegment(
    SnakeSegment *snakeSegment,
    int moveSnakeSegment,
    SnakeDirection snakeTurn
)
{
    SnakeSegment *movedSnakeSegment = (SnakeSegment *) malloc(sizeof(SnakeSegment));
    memcpy(movedSnakeSegment, snakeSegment, sizeof(SnakeSegment));
    if (moveSnakeSegment)
    {
        MoveSnakeSegment(movedSnakeSegment, snakeTurn);
    }
    return movedSnakeSegment;
}

void KillSnake(
    Snake *snake
)
{
    // The kill conditions are checked prospectively,
    // therefore move the snake segments indepedently from the rest of the snake
    SnakeSegment *firstSnakeSegmentMoved = GetSnakeSegment(snake->segment, 1, snake->turn);
    
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
                    SnakeSegment *lastSnakeSegmentMoved = GetSnakeSegment(snakeSegment, 1, snakeSegment->previous->direction);
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
}
