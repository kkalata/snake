#include "snake.h"

void DrawString(
    GameWindow *const window,
    const int x,
    const int y,
    const char *text
)
{
    int textWidth = strlen(text) * CHAR_SIZE; 

    SDL_Rect charSourceRect, charDestRect;
    
    charSourceRect.w = charSourceRect.h = CHAR_SIZE;
    charDestRect.x = x;
    charDestRect.y = y;
    charDestRect.w = charDestRect.h = CHAR_SIZE;

    while (*text) // copy char by char
    {
        int charCode = *text & 255; // ASCII has max char code of 255
        charSourceRect.x = (charCode % FONT_ROW_SIZE) * CHAR_SIZE; // find row of the char
        charSourceRect.y = (charCode / FONT_ROW_SIZE) * CHAR_SIZE; // find column of the char
        SDL_RenderCopy(window->renderer, window->font, &charSourceRect, &charDestRect);
        charDestRect.x += CHAR_SIZE;
        text++;
    }
}

void RenderGameWindow(
    Game *const game
)
{
    if (game->window.timeSinceLastRender > 1000 / FRAMES_PER_SECOND || game->window.timeSinceLastRender == 0)
    {
        SDL_SetRenderDrawColor(game->window.renderer, 0, 0, 0, 255); // fill with black color
        SDL_RenderFillRect(game->window.renderer, NULL);

        RenderBoard(&game->window);
        RenderBlueDot(&game->window, &game->blueDot, game->timer);
        if (game->redDot.visible)
        {
            RenderRedDot(&game->window, &game->redDot, game->timer);
        }
        RenderSnake(&game->window, &game->snake);

        RenderStatusSection(&game->window, &game->timer, game->pointsScored, game->snake.killed, &game->redDot, game->bestPlayers.listUpdated);
        if (game->snake.killed)
        {
            RenderLeaderboard(&game->window, &game->bestPlayers);
        }
        SDL_RenderPresent(game->window.renderer);
        
        game->window.timeSinceLastRender = 0;
    }
    game->window.timeSinceLastRender += game->timer.timeDelta;
}

void RenderBoard(
    GameWindow *const window
)
{
    SDL_SetRenderDrawColor(window->renderer, 46, 194, 126, 255); // fill with green color
    SDL_RenderFillRect(window->renderer, &window->rect.board);
}

void RenderSnake(
    GameWindow *const window,
    const Snake *const snake
)
{
    SDL_Rect snakeSegmentSrcRect;
    SDL_Rect snakeSegmentDestRect;
    int snakeSegmentI = 0;
    
    SnakeSegment *snakeSegment = snake->segment->previous;
    do
    {
        int isSmallSnakeSegment = (snakeSegmentI % 2 == 0) // isn't even
            && snakeSegment != snake->segment // isn't first (head)
            && snakeSegment != snake->segment->previous; // isn't last (tail)
        
        snakeSegmentSrcRect = GetSnakeSegmentCenterRect(snakeSegment, isSmallSnakeSegment);
        snakeSegmentDestRect = GetSnakeSegmentDestRect(snakeSegmentSrcRect, window->rect.board, snakeSegment->pos);

        if (snakeSegment == snake->segment || snakeSegment == snake->segment->previous)
        {
            SDL_RenderCopyEx(
                window->renderer,
                GetSnakeSkinFragment(window, snakeSegment, snake->segment),
                &snakeSegmentSrcRect,
                &snakeSegmentDestRect,
                90 * snakeSegment->direction,
                NULL,
                SDL_FLIP_NONE
            );
        }
        else
        {
            SDL_RenderCopy(window->renderer, window->snakeSkin.body, &snakeSegmentSrcRect, &snakeSegmentDestRect);
        }
        if (isSmallSnakeSegment)
        {
            snakeSegmentSrcRect = GetSnakeSmallSegmentFillRect(snakeSegment, 1);
            snakeSegmentDestRect = GetSnakeSegmentDestRect(snakeSegmentSrcRect, window->rect.board, snakeSegment->pos);
            SDL_RenderCopy(window->renderer, window->snakeSkin.body, &snakeSegmentSrcRect, &snakeSegmentDestRect);

            snakeSegmentSrcRect = GetSnakeSmallSegmentFillRect(snakeSegment->next, 0);
            snakeSegmentDestRect = GetSnakeSegmentDestRect(snakeSegmentSrcRect, window->rect.board, snakeSegment->pos);
            SDL_RenderCopy(window->renderer, window->snakeSkin.body, &snakeSegmentSrcRect, &snakeSegmentDestRect);
        }
        snakeSegmentI++;
        snakeSegment = snakeSegment->next;
    } while (snakeSegment->next != snake->segment);
}

SDL_Texture *GetSnakeSkinFragment(
    const GameWindow *const window,
    const SnakeSegment *const snakeSegment,
    const SnakeSegment *const firstSnakeSegment
)
{
    if (snakeSegment == firstSnakeSegment)
    {
        return window->snakeSkin.head;
    }
    else if (snakeSegment == firstSnakeSegment->previous)
    {
        return window->snakeSkin.tail;
    }
    else
    {
        return window->snakeSkin.body;
    }
}

SDL_Rect GetSnakeSegmentCenterRect(
    const SnakeSegment *const snakeSegment,
    const int isSmallSnakeSegment
)
{
    SDL_Rect snakeSegmentRect;
    if (isSmallSnakeSegment)
    {
        snakeSegmentRect.x = snakeSegmentRect.y = SNAKE_SMALL_SEGMENT_MARGIN;
        snakeSegmentRect.w = snakeSegmentRect.h = SNAKE_SEGMENT_SIZE - 2 * SNAKE_SMALL_SEGMENT_MARGIN;
    }
    else
    {
        snakeSegmentRect.x = snakeSegmentRect.y = 0;
        snakeSegmentRect.w = snakeSegmentRect.h = SNAKE_SEGMENT_SIZE;
    }
    return snakeSegmentRect;
}

SDL_Rect GetSnakeSmallSegmentFillRect(
    const SnakeSegment *const snakeSegment,
    const int front
)
{
    SDL_Rect snakeSegmentRect;
    if ((front && snakeSegment->direction == SNAKE_UP) || (!front && snakeSegment->direction == SNAKE_DOWN))
    {
        snakeSegmentRect.x = SNAKE_SMALL_SEGMENT_MARGIN;
        snakeSegmentRect.y = 0;
        snakeSegmentRect.w = SNAKE_SEGMENT_SIZE - 2 * SNAKE_SMALL_SEGMENT_MARGIN;
        snakeSegmentRect.h = SNAKE_SMALL_SEGMENT_MARGIN;
    }
    else if ((front && snakeSegment->direction == SNAKE_DOWN) || (!front && snakeSegment->direction == SNAKE_UP))
    {
        snakeSegmentRect.x = SNAKE_SMALL_SEGMENT_MARGIN;
        snakeSegmentRect.y = SNAKE_SEGMENT_SIZE - SNAKE_SMALL_SEGMENT_MARGIN;
        snakeSegmentRect.w = SNAKE_SEGMENT_SIZE - 2 * SNAKE_SMALL_SEGMENT_MARGIN;
        snakeSegmentRect.h = SNAKE_SMALL_SEGMENT_MARGIN;
    }
    else if ((front && snakeSegment->direction == SNAKE_LEFT) || (!front && snakeSegment->direction == SNAKE_RIGHT))
    {
        snakeSegmentRect.x = 0;
        snakeSegmentRect.y = SNAKE_SMALL_SEGMENT_MARGIN;
        snakeSegmentRect.w = SNAKE_SMALL_SEGMENT_MARGIN;
        snakeSegmentRect.h = SNAKE_SEGMENT_SIZE - 2 * SNAKE_SMALL_SEGMENT_MARGIN;
    }
    else if ((front && snakeSegment->direction == SNAKE_RIGHT) || (!front && snakeSegment->direction == SNAKE_LEFT))
    {
        snakeSegmentRect.x = SNAKE_SEGMENT_SIZE - SNAKE_SMALL_SEGMENT_MARGIN;
        snakeSegmentRect.y = SNAKE_SMALL_SEGMENT_MARGIN;
        snakeSegmentRect.w = SNAKE_SMALL_SEGMENT_MARGIN;
        snakeSegmentRect.h = SNAKE_SEGMENT_SIZE - 2 * SNAKE_SMALL_SEGMENT_MARGIN;
    }
    return snakeSegmentRect;
}

SDL_Rect GetSnakeSegmentDestRect(
    const SDL_Rect snakeSegmentSrcRect,
    const SDL_Rect boardRect,
    const Position snakePosition
)
{
    SDL_Rect snakeSegmentDestRect;
    snakeSegmentDestRect = snakeSegmentSrcRect;
    snakeSegmentDestRect.x += boardRect.x + snakePosition.x * SNAKE_SEGMENT_SIZE;
    snakeSegmentDestRect.y += boardRect.y + snakePosition.y * SNAKE_SEGMENT_SIZE;
    return snakeSegmentDestRect;
}

void RenderDot(
    GameWindow *const window,
    const Position pos,
    const GameTimer timer
)
{
    int dotMargin = 0;
    if ((timer.timeElapsed / DOT_BLINK_INTERVAL) % 2)
    {
        dotMargin = SMALL_DOT_MARGIN;
    }
    SDL_Rect dotRect;
    dotRect.x = window->rect.board.x + pos.x * SNAKE_SEGMENT_SIZE + dotMargin;
    dotRect.y = window->rect.board.y + pos.y * SNAKE_SEGMENT_SIZE + dotMargin;
    dotRect.w = SNAKE_SEGMENT_SIZE - 2 * dotMargin;
    dotRect.h = SNAKE_SEGMENT_SIZE - 2 * dotMargin;
    SDL_RenderFillRect(window->renderer, &dotRect);
}

void RenderBlueDot(
    GameWindow *const window,
    const BlueDot *const blueDot,
    const GameTimer timer
)
{
    SDL_SetRenderDrawColor(window->renderer, 26, 95, 180, 255); // fill with blue color
    RenderDot(window, blueDot->pos, timer);
}

void RenderRedDot(
    GameWindow *const window,
    const RedDot *const redDot,
    const GameTimer timer
)
{
    SDL_SetRenderDrawColor(window->renderer, 192, 28, 40, 255); // fill with red color
    RenderDot(window, redDot->pos, timer);
}

void RenderStatusSection(
    GameWindow *const window,
    const GameTimer *const timer,
    const Uint32 pointsScored,
    const SnakeKillReason snakeKillReason,
    const RedDot *const redDot,
    const int bestPlayersListUpdated
)
{
    SDL_SetRenderDrawColor(window->renderer, 192, 28, 40, 255); // fill with red color
    SDL_RenderFillRect(window->renderer, &window->rect.statusSection);

    char statusSectionContent[SCREEN_WIDTH];
    
    sprintf(statusSectionContent, "%.1f s elapsed", timer->timeElapsed / 1000.0);
    RenderStatusSectionInfo(window, statusSectionContent, 0, ALIGN_LEFT);

    if (snakeKillReason != ALIVE || !redDot->visible)
    {
        sprintf(statusSectionContent, "Points: %u", pointsScored);
        RenderStatusSectionInfo(window, statusSectionContent, 1, ALIGN_LEFT);
    }

    if (snakeKillReason == ALIVE && !redDot->visible)
    {
        sprintf(statusSectionContent, IMPLEMENTED_REQUIREMENTS_HEADER);
        RenderStatusSectionInfo(window, statusSectionContent, 0, ALIGN_RIGHT);

        sprintf(statusSectionContent, IMPLEMENTED_REQUIREMENTS);
        RenderStatusSectionInfo(window, statusSectionContent, 1, ALIGN_RIGHT);
    }

    if (snakeKillReason == ALIVE && redDot->visible)
    {
        sprintf(statusSectionContent, RED_DOT_APPEAR_BAR_DESCRIPTION);
        RenderStatusSectionInfo(window, statusSectionContent, 1, ALIGN_LEFT);
        RenderRedDotAppearTimeBar(
            window,
            strlen(statusSectionContent) * CHAR_SIZE,
            (float)(redDot->appearTime + RED_DOT_DISPLAY_TIME - timer->timeElapsed) / RED_DOT_DISPLAY_TIME
        );
    } 

    if (snakeKillReason != ALIVE)
    {
        GetSnakeKillReasonInfo(statusSectionContent, snakeKillReason);
        RenderStatusSectionInfo(window, statusSectionContent, 0, ALIGN_RIGHT);

        GetGameKeyGuide(statusSectionContent, bestPlayersListUpdated);
        RenderStatusSectionInfo(window, statusSectionContent, 1, ALIGN_RIGHT);
    }   
}

void RenderStatusSectionInfo(
    GameWindow *const window,
    const char *const content,
    const int line,
    const StatusSectionAlignment alignment
)
{
    int xOffset;
    int yOffset = window->rect.statusSection.y + STATUS_SECTION_MARGIN + line * (STATUS_SECTION_MARGIN + CHAR_SIZE);
    switch (alignment)
    {
        case ALIGN_LEFT:
            xOffset = 2 * STATUS_SECTION_MARGIN;
            break;
        case ALIGN_RIGHT:
            xOffset = SCREEN_WIDTH - strlen(content) * CHAR_SIZE - 2 * STATUS_SECTION_MARGIN;
            break;
    }
    DrawString(window, xOffset, yOffset, content);
}

void GetSnakeKillReasonInfo(
    char statusSectionContent[],
    const SnakeKillReason snakeKillReason
)
{
    switch (snakeKillReason)
    {
        case UNSPECIFIED:
            sprintf(statusSectionContent, SNAKE_DEATH_UNSPECIFIED_INFO);
            break;
        case HIT_ITSELF:
            sprintf(statusSectionContent, SNAKE_HIT_ITSELF_INFO);
            break;
        case HIT_WALL:
            sprintf(statusSectionContent, SNAKE_HIT_WALL_INFO);
            break;
    }
}

void GetGameKeyGuide(
    char statusSectionContent[],
    const int bestPlayersListUpdated
)
{
    if (bestPlayersListUpdated)
    {
        sprintf(statusSectionContent, SNAKE_DEATH_KEY_GUIDE);
    }
    else
    {
        sprintf(statusSectionContent, LEADERBOARD_KEY_GUIDE);
    }
}

void RenderRedDotAppearTimeBar(
    GameWindow *const window,
    const int descriptionWidth,
    const float timeLeft
)
{
    SDL_Rect redDotAppearTimeBar;
    redDotAppearTimeBar.x = 2 * STATUS_SECTION_MARGIN + descriptionWidth + CHAR_SIZE;
    redDotAppearTimeBar.y = SCREEN_HEIGHT - 2 * STATUS_SECTION_MARGIN - CHAR_SIZE;
    redDotAppearTimeBar.w = (SCREEN_WIDTH - 4 * STATUS_SECTION_MARGIN - descriptionWidth - CHAR_SIZE) * timeLeft;
    redDotAppearTimeBar.h = CHAR_SIZE;
    SDL_SetRenderDrawColor(window->renderer, 255, 255, 255, 255); // fill with white color
    SDL_RenderFillRect(window->renderer, &redDotAppearTimeBar);
}

void RenderLeaderboard(
    GameWindow *const window,
    const BestPlayers *const bestPlayers
)
{
    // render background
    SDL_SetRenderDrawColor(window->renderer, 192, 28, 40, 255); // fill with red color
    SDL_RenderFillRect(window->renderer, &window->rect.leaderboard);

    // render text
    int lineX = window->rect.leaderboard.x + STATUS_SECTION_MARGIN;
    int lineY = window->rect.leaderboard.y + STATUS_SECTION_MARGIN;
    char leaderboardContent[SCREEN_WIDTH];

    DrawString(window, lineX, lineY, LEADERBOARD_HEADER);
    for (int bestPlayerI = 0; bestPlayerI < BEST_PLAYER_COUNT; bestPlayerI++)
    {
        lineY += CHAR_SIZE + STATUS_SECTION_MARGIN;
        
        if (bestPlayers->list[bestPlayerI].playerName != NULL)
        {
            sprintf(
                leaderboardContent,
                "%d. %u - %s",
                bestPlayerI + 1,
                bestPlayers->list[bestPlayerI].pointsScored,
                bestPlayers->list[bestPlayerI].playerName
            );
            if (!bestPlayers->listUpdated && bestPlayerI == bestPlayers->newBestPlayerI)
            {
                strcat(leaderboardContent, TEXT_CURSOR);
            }
        }
        else
        {
            sprintf(leaderboardContent, "%d.", bestPlayerI + 1);
        }
        DrawString(window, lineX, lineY, leaderboardContent);
    }
}
