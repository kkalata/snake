#include "snake.h"

void DrawString(
    GameWindow *window,
    const int x,
    const int y,
    const char *text
)
{
    int textWidth = strlen(text) * CHAR_SIZE; 

    SDL_Rect charSourceRect, charDestRect;
    charDestRect.x = charDestRect.y = 0;
    charSourceRect.w = charDestRect.w = CHAR_SIZE;
    charSourceRect.h = charDestRect.w = CHAR_SIZE;

    SDL_Surface *textSubscreenCPU = SDL_CreateRGBSurface(
        0,
        textWidth, CHAR_SIZE,
        32,
        0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000
    );

    while (*text) // copy char by char
    {
        int charCode = *text & 255; // ASCII has max char code of 255
        charSourceRect.x = (charCode % FONT_ROW_SIZE) * CHAR_SIZE; // find row of the char
        charSourceRect.y = (charCode / FONT_ROW_SIZE) * CHAR_SIZE; // find column of the char
        SDL_BlitSurface(window->font, &charSourceRect, textSubscreenCPU, &charDestRect);
        charDestRect.x += CHAR_SIZE;
        text++;
    }

    SDL_Texture *textSubscreenGPU = SDL_CreateTextureFromSurface(window->renderer, textSubscreenCPU);
    SDL_Rect textDestRect = GetTextRect(x, y, textWidth);
    SDL_RenderCopy(window->renderer, textSubscreenGPU, NULL, &textDestRect);
    SDL_FreeSurface(textSubscreenCPU);
    SDL_DestroyTexture(textSubscreenGPU);
}

void RenderGameWindow(
    Game *game
)
{
    if (game->window.timeSinceLastRender > 1000 / FRAMES_PER_SECOND || game->window.timeSinceLastRender == 0)
    {
        SDL_SetRenderDrawColor(game->window.renderer, 0, 0, 0, 255); // fill with black color
        SDL_RenderFillRect(game->window.renderer, NULL);

        RenderBoard(&game->window);
        RenderBlueDot(&game->window, &game->blueDot);
        if (game->redDot.visible)
        {
            RenderRedDot(&game->window, &game->redDot);
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
    GameWindow *window
)
{
    SDL_SetRenderDrawColor(window->renderer, 46, 194, 126, 255); // fill with green color
    SDL_RenderFillRect(window->renderer, &window->rect.board);
}

void RenderSnake(
    GameWindow *window,
    const Snake *const snake
)

{
    SDL_Rect snakeSegmentSrcRect;
    SDL_Rect snakeSegmentDestRect;
    
    SnakeSegment *snakeSegment = snake->segment->previous;
    do
    {
        if (
            ((snakeSegment->pos.x % 2) ^ (snakeSegment->pos.y % 2)) // compare with checker pattern
            && snakeSegment != snake->segment // isn't first (head)
            && snakeSegment != snake->segment->previous // isn't last (tail)
        )
        {
            snakeSegmentSrcRect.x = snakeSegmentSrcRect.y = SNAKE_SMALL_SEGMENT_MARGIN;
            snakeSegmentSrcRect.w = snakeSegmentSrcRect.h = SNAKE_SEGMENT_SIZE - 2 * SNAKE_SMALL_SEGMENT_MARGIN;
        }
        else
        {
            snakeSegmentSrcRect.x = snakeSegmentSrcRect.y = 0;
            snakeSegmentSrcRect.w = snakeSegmentSrcRect.h = SNAKE_SEGMENT_SIZE;
        }
        snakeSegmentDestRect = snakeSegmentSrcRect;
        snakeSegmentDestRect.x += window->rect.board.x + snakeSegment->pos.x * SNAKE_SEGMENT_SIZE;
        snakeSegmentDestRect.y += window->rect.board.y + snakeSegment->pos.y * SNAKE_SEGMENT_SIZE;
        
        SDL_Texture *snakeSkinFragment;
        if (snakeSegment == snake->segment)
        {
            snakeSkinFragment = window->snakeSkin.head;
        }
        else if (snakeSegment == snake->segment->previous)
        {
            snakeSkinFragment = window->snakeSkin.tail;
        }
        else
        {
            snakeSkinFragment = window->snakeSkin.body;
        }
        SDL_RenderCopyEx(
            window->renderer,
            snakeSkinFragment,
            &snakeSegmentSrcRect,
            &snakeSegmentDestRect,
            90 * snakeSegment->direction,
            NULL,
            SDL_FLIP_NONE
        );
        snakeSegment = snakeSegment->previous;
    } while (snakeSegment->next != snake->segment);
}

void RenderDot(
    GameWindow *window,
    const Position pos
)
{
    SDL_Rect dotRect;
    dotRect.x = window->rect.board.x + pos.x * SNAKE_SEGMENT_SIZE;
    dotRect.y = window->rect.board.y + pos.y * SNAKE_SEGMENT_SIZE;
    dotRect.w = SNAKE_SEGMENT_SIZE;
    dotRect.h = SNAKE_SEGMENT_SIZE;
    SDL_RenderFillRect(window->renderer, &dotRect);
}

void RenderBlueDot(
    GameWindow *window,
    BlueDot *blueDot
)
{
    SDL_SetRenderDrawColor(window->renderer, 26, 95, 180, 255); // fill with blue color
    RenderDot(window, blueDot->pos);
}

void RenderRedDot(
    GameWindow *window,
    RedDot *redDot
)
{
    SDL_SetRenderDrawColor(window->renderer, 192, 28, 40, 255); // fill with red color
    RenderDot(window, redDot->pos);
}

void RenderStatusSection(
    GameWindow *window,
    GameTimer *timer,
    Uint32 pointsScored,
    SnakeKillReason snakeKillReason,
    const RedDot *redDot,
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
    GameWindow *window,
    char *content,
    int line,
    StatusSectionAlignment alignment
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
    SnakeKillReason snakeKillReason
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
    int bestPlayersListUpdated
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
    GameWindow *window,
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
    GameWindow *window,
    BestPlayers *bestPlayers
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
