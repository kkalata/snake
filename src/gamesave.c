#include "snake.h"

void SaveGame(
    Game *const game
)
{
    FILE *saveFile = fopen(SAVE_FILENAME, "w");
    if (saveFile == NULL)
    {
        return;
    }

    fprintf(saveFile, "%d\n", game->seed);
    fprintf(saveFile, "%u %u\n", game->timer.timeElapsed, game->pointsScored);
    fprintf(
        saveFile,
        "%f %u %u %d %u\n",
        game->snake.cooldown,
        game->snake.timeSinceLastSpeedup,
        game->snake.timeSinceLastMove,
        game->snake.turn,
        game->snake.killed
    );
    SaveSnake(saveFile, &game->snake);
    fprintf(saveFile, "%d %d\n", game->blueDot.pos.x, game->blueDot.pos.y);
    fprintf(
        saveFile,
        "%hhd %d %d\n",
        game->redDot.visible,
        game->redDot.appearTime,
        game->redDot.snakeBehavior
    );
    if (game->redDot.visible)
    {
        fprintf(
            saveFile,
            "%d %d\n",
            game->redDot.pos.x,
            game->redDot.pos.y
        );
    }
    fclose(saveFile);
}

void SaveSnake(
    FILE *const saveFile,
    const Snake *const snake
)
{
    SnakeSegment *snakeSegment = snake->segment;
    Uint32 snakeSegmentCount = 0;
    do
    {
        snakeSegmentCount++;
        snakeSegment = snakeSegment->next;
    } while (snakeSegment != snake->segment);
    fprintf(saveFile, "%u\n", snakeSegmentCount);
    do
    {
        
        fprintf(
            saveFile,
            "%d %d %d\n",
            snakeSegment->pos.x,
            snakeSegment->pos.y,
            snakeSegment->direction
        );
        snakeSegment = snakeSegment->next;
    } while (snakeSegment != snake->segment);
}

void LoadGame(
    Game *const game
)
{
    FILE *saveFile = fopen(SAVE_FILENAME, "r");
    if (saveFile == NULL)
    {
        return;
    }

    DestroyGame(game);
    game->window.timeSinceLastRender = 0; // force render

    fscanf(saveFile, "%d\n", &game->seed);
    srand(game->seed);
    fscanf(saveFile, "%u %u\n", &game->timer.timeElapsed, &game->pointsScored);
    fscanf(
        saveFile,
        "%f %u %u %d %u\n",
        &game->snake.cooldown,
        &game->snake.timeSinceLastSpeedup,
        &game->snake.timeSinceLastMove,
        (int *) &game->snake.turn,
        (int *) &game->snake.killed
    );
    LoadSnake(saveFile, &game->snake);
    fscanf(saveFile, "%d %d\n", &game->blueDot.pos.x, &game->blueDot.pos.y);
    fscanf(
        saveFile,
        "%hhd %d %d\n",
        &game->redDot.visible,
        &game->redDot.appearTime,
        (int *) &game->redDot.snakeBehavior
    );
    if (game->redDot.visible)
    {
        fscanf(
            saveFile,
            "%d %d\n",
            &game->redDot.pos.x,
            &game->redDot.pos.y
        );
    }
    fclose(saveFile);

    LoadBestPlayers(&game->bestPlayers);
}

void LoadSnake(
    FILE *const saveFile,
    Snake *const snake
)
{
    Uint32 snakeSegmentCount;
    fscanf(saveFile, "%u\n", &snakeSegmentCount);
    for (Uint32 snakeSegmentI = 0; snakeSegmentI < snakeSegmentCount; snakeSegmentI++)
    {
        CreateSnakeSegment(snake, 0, 0, SNAKE_NO_TURN); // the params will be overwritten soon
        SnakeSegment *snakeSegment = snake->segment->previous;
        fscanf(
            saveFile,
            "%d %d %d\n",
            &snakeSegment->pos.x,
            &snakeSegment->pos.y,
            (int *) &snakeSegment->direction
        );
    }
}
