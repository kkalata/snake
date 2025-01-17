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
    RenderStatusSectionInfo(window, statusSectionContent, 0, LEFT);

    if (snakeKillReason != ALIVE || !redDot->visible)
    {
        sprintf(statusSectionContent, "Points: %u", pointsScored);
        RenderStatusSectionInfo(window, statusSectionContent, 1, LEFT);
    }

    if (snakeKillReason == ALIVE && !redDot->visible)
    {
        sprintf(statusSectionContent, IMPLEMENTED_REQUIREMENTS_HEADER);
        RenderStatusSectionInfo(window, statusSectionContent, 0, RIGHT);

        sprintf(statusSectionContent, IMPLEMENTED_REQUIREMENTS);
        RenderStatusSectionInfo(window, statusSectionContent, 1, RIGHT);
    }

    if (snakeKillReason == ALIVE && redDot->visible)
    {
        sprintf(statusSectionContent, "RED DOT");
        RenderStatusSectionInfo(window, statusSectionContent, 1, LEFT);
        RenderRedDotAppearTimeBar(
            window,
            strlen(statusSectionContent) * CHAR_SIZE,
            (float)(redDot->appearTime + RED_DOT_DISPLAY_TIME - timer->timeElapsed) / RED_DOT_DISPLAY_TIME
        );
    } 

    if (snakeKillReason != ALIVE)
    {
        GetSnakeKillReasonInfo(statusSectionContent, snakeKillReason);
        RenderStatusSectionInfo(window, statusSectionContent, 0, RIGHT);

        GetGameKeyGuide(statusSectionContent, bestPlayersListUpdated);
        RenderStatusSectionInfo(window, statusSectionContent, 1, RIGHT);
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
    int yOffset = SCREEN_HEIGHT - (STATUS_LINES - line + 1) * STATUS_MARGIN - (STATUS_LINES - line) * CHAR_SIZE;
    switch (alignment)
    {
        case LEFT:
            xOffset = 2 * STATUS_MARGIN;
            break;
        case RIGHT:
            xOffset = SCREEN_WIDTH - strlen(content) * CHAR_SIZE - 2 * STATUS_MARGIN;
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
            sprintf(statusSectionContent, "Snake killed.");
            break;
        case HIT_ITSELF:
            sprintf(statusSectionContent, "Snake hit itself.");
            break;
        case HIT_WALL:
            sprintf(statusSectionContent, "Snake hit the wall.");
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
        sprintf(statusSectionContent, "Press N to retry or ESC to quit.");
    }
    else
    {
        sprintf(statusSectionContent, "Press ENTER to confirm the name or ESC to refuse.");
    }
}

void RenderRedDotAppearTimeBar(
    GameWindow *window,
    const int descriptionWidth,
    const float timeLeft
)
{
    SDL_Rect redDotAppearTimeBar;
    redDotAppearTimeBar.x = 2 * STATUS_MARGIN + descriptionWidth + CHAR_SIZE;
    redDotAppearTimeBar.y = SCREEN_HEIGHT - 2 * STATUS_MARGIN - CHAR_SIZE;
    redDotAppearTimeBar.w = (SCREEN_WIDTH - 4 * STATUS_MARGIN - descriptionWidth - CHAR_SIZE) * timeLeft;
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
    int lineX = window->rect.leaderboard.x + STATUS_MARGIN;
    int lineY = window->rect.leaderboard.y + STATUS_MARGIN;
    char leaderboardContent[SCREEN_WIDTH];

    DrawString(window, lineX, lineY, "LEADERBOARD");
    for (int bestPlayerI = 0; bestPlayerI < BEST_PLAYER_COUNT; bestPlayerI++)
    {
        lineY += CHAR_SIZE + STATUS_MARGIN;
        
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
