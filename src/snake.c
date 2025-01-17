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

    SetSectionRects(window);

    SDL_StopTextInput(); // it appears that the text input is enabled by default
    window->textInputActive = 0;

    return 1;
}

void SetSectionRects(
    GameWindow *window
)
{
    window->rect.statusSection.w = SCREEN_WIDTH - 2 * STATUS_SECTION_MARGIN;
    window->rect.statusSection.h = (STATUS_SECTION_LINES + 1) * STATUS_SECTION_MARGIN + STATUS_SECTION_LINES * CHAR_SIZE;
    window->rect.statusSection.x = STATUS_SECTION_MARGIN;
    window->rect.statusSection.y = SCREEN_HEIGHT - window->rect.statusSection.h - STATUS_SECTION_MARGIN;

    window->rect.board.w = BOARD_SECTION_WIDTH * SNAKE_SEGMENT_SIZE;
    window->rect.board.h = BOARD_SECTION_HEIGHT * SNAKE_SEGMENT_SIZE;
    window->rect.board.x = (SCREEN_WIDTH - window->rect.board.w) / 2;
    window->rect.board.y = (SCREEN_HEIGHT - window->rect.board.h - window->rect.statusSection.h - STATUS_SECTION_MARGIN) / 2;

    window->rect.leaderboard.w = window->rect.board.w;
    window->rect.leaderboard.h = (BEST_PLAYER_COUNT + 1) * (CHAR_SIZE + STATUS_SECTION_MARGIN) + STATUS_SECTION_MARGIN;
    window->rect.leaderboard.x = window->rect.board.x;
    window->rect.leaderboard.y = window->rect.board.y + (window->rect.board.h - window->rect.leaderboard.h) / 2;
}

void CreateGame(
    Game *game
)
{
    game->seed = time(NULL);
    srand(game->seed);
    game->timer = InitGameTimer();

    game->pointsScored = 0;
    LoadBestPlayers(&game->bestPlayers);

    CreateSnake(&game->snake);
    PlaceBlueDot(&game->blueDot, &game->snake, &game->redDot);
    SetRedDotParams(&game->redDot, game->timer);
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

void DestroyGame(
    Game *game
)
{
    game->window.timeSinceLastRender = 0;
    DestroySnake(&game->snake);
    DestroyBestPlayersList(game->bestPlayers.list);
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
    CreateGame(&game);
    
    while (GameLoop(&game));
    
    DestroyGame(&game);
    CloseGameWindow(&game.window);

    return 0;
}
