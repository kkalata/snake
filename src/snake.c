#include "snake.h"

SDL_Rect GetTextRect(
    const int x,
    const int y,
    const int textWidth
)
{
    SDL_Rect textRect;
    textRect.x = x;
    textRect.y = y;
    textRect.w = textWidth;
    textRect.h = CHAR_SIZE;
    return textRect;
}

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

int CreateGameWindow(
    GameWindow *window
)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        return 0;
    }

    if (SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window->window, &window->renderer) != 0)
    {
        SDL_Quit();
        return 0;
    }
    window->timeSinceLastRender = 0;

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    SDL_RenderSetLogicalSize(window->renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
    SDL_SetRenderDrawColor(window->renderer, 0, 0, 0, 255);

    SDL_SetWindowTitle(window->window, GAME_TITLE);

    window->font = SDL_LoadBMP(FONT_FILEPATH);
    if (window->font == NULL)
    {
        return 0;
    }
    SDL_SetColorKey(window->font, 1, 0x000000);

    SDL_Surface *snakeSkinCPU = SDL_LoadBMP(SNAKE_SKIN_FILEPATH);
    if (window->font == NULL)
    {
        return 0;
    }
    window->snakeSkin = SDL_CreateTextureFromSurface(window->renderer, snakeSkinCPU);
    SDL_FreeSurface(snakeSkinCPU);

    SDL_StopTextInput(); // it appears that the text input is enabled by default
    window->textInputActive = 0;

    return 1;
}

void CreateGame(
    Game *game
)
{
    game->seed = time(NULL);
    srand(game->seed);
    game->timer = InitGameTimer();
    game->pointsScored = 0;
    CreateSnake(&game->snake);
    PlaceBlueDot(&game->blueDot, &game->snake, &game->redDot);
    SetRedDotParams(&game->redDot, game->timer);
    LoadBestPlayers(game->bestPlayers.list);
    game->bestPlayers.newBestPlayerI = BEST_PLAYER_COUNT;
    game->bestPlayers.listUpdated = 0;
}

int GameLoop(
    Game *game
)
{
    SDL_Event event;
    int quitRequested = 0;
    
    RenderGameWindow(game);

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_TEXTINPUT:
                if (game->window.textInputActive)
                {
                    AppendNewBestPlayerName(&game->bestPlayers, event.text.text);
                }
                break;
            case SDL_KEYDOWN:
                if (!game->window.textInputActive)
                {
                    quitRequested = ProcessGameKeydowns(game, event.key.keysym.sym);
                }
                else
                {
                    ConfirmNewBestPlayerName(&game->bestPlayers, event.key.keysym.sym);
                }
                break;
            case SDL_QUIT:
                quitRequested = 1;
                break;
        }
    }

    ProcessGameLogic(game);

    SDL_Delay(1);
    return !quitRequested;
}

int ProcessGameKeydowns(
    Game *game,
    SDL_Keycode pressedKey
)
{
    int quitRequested = 0;
    switch (pressedKey)
    {
        case SDLK_ESCAPE:
            quitRequested = 1;
            break;
        case SDLK_n:
            DestroyGame(game);
            CreateGame(game);
            break;
        case SDLK_s:
            if (!game->snake.killed)
            {
                SaveGame(game);
            }
            break;
        case SDLK_l:
            if (!game->snake.killed)
            {
                LoadGame(game);
            }
            break;
        case SDLK_UP:
        case SDLK_DOWN:
        case SDLK_LEFT:
        case SDLK_RIGHT:
            if (!game->snake.killed)
            {
                TurnSnake(&game->snake, pressedKey);
            }
            break;
    }
    return quitRequested;
}

void ConfirmNewBestPlayerName(
    BestPlayers *bestPlayers,
    SDL_Keycode pressedKey
)
{
    char *bestPlayerName = bestPlayers->list[bestPlayers->newBestPlayerI].playerName;
    switch (pressedKey)
    {
        case SDLK_ESCAPE:
        case SDLK_RETURN:
            if (pressedKey == SDLK_ESCAPE)
            {
                strcpy(bestPlayerName, "Player");
            }
            SaveBestPlayers(bestPlayers->list);
            bestPlayers->listUpdated = 1;
            break;
        case SDLK_BACKSPACE:
            if (strlen(bestPlayerName) > 0)
            {
                bestPlayerName[strlen(bestPlayerName) - 1] = '\0';
            }
            break;
    }
}

void ProcessGameLogic(
    Game *game
)
{
    if (!game->snake.killed)
    {
        GetTimeDelta(&game->timer);
        PlaceRedDot(&game->redDot, &game->snake, &game->blueDot, game->timer);
        game->pointsScored += AdvanceSnake(&game->snake, &game->blueDot, &game->redDot, game->timer);
    }
    else
    {
        if (game->pointsScored != 0)
        {
            if (!game->bestPlayers.listUpdated)
            {
                if (!game->window.textInputActive)
                {
                    PrepareNewBestPlayer(&game->bestPlayers, game->pointsScored);
                    if (game->bestPlayers.newBestPlayerI == BEST_PLAYER_COUNT)
                    {
                        game->bestPlayers.listUpdated = 1;
                    }
                    else
                    {
                        SDL_StartTextInput();
                        game->window.textInputActive = 1;
                    }
                }
            }
            else
            {
                SDL_StopTextInput();
                game->window.textInputActive = 0;
            }
        }
        else
        {
            game->bestPlayers.listUpdated = 1;
        }
    }
}

void RenderGameWindow(
    Game *game
)
{
    if (game->window.timeSinceLastRender > 1000 / FRAMES_PER_SECOND || game->window.timeSinceLastRender == 0)
    {
        SDL_SetRenderDrawColor(game->window.renderer, 0, 0, 0, 255); // fill with black color
        SDL_RenderFillRect(game->window.renderer, NULL);
        RenderBoard(&game->window, &game->boardRect);
        RenderBlueDot(&game->window, &game->blueDot, &game->boardRect);
        if (game->redDot.visible)
        {
            RenderRedDot(&game->window, &game->redDot, &game->boardRect);
        }
        RenderSnake(&game->window, &game->snake, &game->boardRect);
        RenderStatusSection(&game->window, &game->timer, game->pointsScored, game->snake.killed, &game->redDot, game->bestPlayers.listUpdated);
        if (game->snake.killed)
        {
            RenderLeaderboard(&game->window, &game->boardRect, &game->bestPlayers);
        }
        SDL_RenderPresent(game->window.renderer);
        game->window.timeSinceLastRender = 0;
    }
    game->window.timeSinceLastRender += game->timer.timeDelta;
}

void RenderBoard(
    GameWindow *window,
    SDL_Rect *boardRect
)
{
    SDL_SetRenderDrawColor(window->renderer, 46, 194, 126, 255); // fill with green color
    SDL_RenderFillRect(window->renderer, boardRect);
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
    SDL_Rect statusSectionRect;
    statusSectionRect.x = STATUS_MARGIN;
    statusSectionRect.y = SCREEN_HEIGHT - STATUS_HEIGHT - STATUS_MARGIN;
    statusSectionRect.w = SCREEN_WIDTH - 2 * STATUS_MARGIN;
    statusSectionRect.h = STATUS_HEIGHT;
    
    SDL_SetRenderDrawColor(window->renderer, 192, 28, 40, 255); // fill with red color
    SDL_RenderFillRect(window->renderer, &statusSectionRect);

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
        RenderStatusSectionInfo(window, statusSectionContent, 0, RIGHT);

        if (bestPlayersListUpdated)
        {
            sprintf(statusSectionContent, "Press N to retry or ESC to quit.");
        }
        else
        {
            sprintf(statusSectionContent, "Press ENTER to confirm the name or ESC to refuse.");
        }
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
    SDL_Rect *boardRect,
    BestPlayers *bestPlayers
)
{
    // render background
    SDL_Rect leaderboardSection;
    leaderboardSection.w = boardRect->w;
    leaderboardSection.h = (BEST_PLAYER_COUNT + 1) * (CHAR_SIZE + STATUS_MARGIN) + STATUS_MARGIN;
    leaderboardSection.x = boardRect->x;
    leaderboardSection.y = boardRect->y + (boardRect->h - leaderboardSection.h) / 2;
    SDL_SetRenderDrawColor(window->renderer, 192, 28, 40, 255); // fill with red color
    SDL_RenderFillRect(window->renderer, &leaderboardSection);

    // render text
    int lineX = leaderboardSection.x + STATUS_MARGIN;
    int lineY = leaderboardSection.y + STATUS_MARGIN;
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

void DestroyGame(
    Game *game
)
{
    game->window.timeSinceLastRender = 0;
    DestroySnake(&game->snake);

    for (int bestPlayerI = 0; bestPlayerI < BEST_PLAYER_COUNT; bestPlayerI++)
    {
        free(game->bestPlayers.list[bestPlayerI].playerName);
        game->bestPlayers.list[bestPlayerI].pointsScored = 0;
        game->bestPlayers.list[bestPlayerI].playerName = NULL;
    }
}

void CloseGameWindow(
    GameWindow *window
)
{
    SDL_FreeSurface(window->font);
    SDL_DestroyTexture(window->snakeSkin);
    SDL_DestroyRenderer(window->renderer);
    SDL_DestroyWindow(window->window);
    SDL_Quit();
}

int main(int argc, char **argv)
{
    Game game;
    if (!CreateGameWindow(&game.window))
    {
        CloseGameWindow(&game.window);
        return 1;
    }
    game.boardRect.w = BOARD_SECTION_WIDTH * SNAKE_SEGMENT_SIZE;
    game.boardRect.h = BOARD_SECTION_HEIGHT * SNAKE_SEGMENT_SIZE;
    game.boardRect.x = (SCREEN_WIDTH - game.boardRect.w) / 2;
    game.boardRect.y = (SCREEN_HEIGHT - game.boardRect.h - STATUS_HEIGHT - STATUS_MARGIN) / 2;
    CreateGame(&game);
    
    while (GameLoop(&game));
    
    DestroyGame(&game);
    CloseGameWindow(&game.window);

    return 0;
}