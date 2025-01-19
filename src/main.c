#include "snake.h"

// Init, set and load all the things related to the window itself and rendering
int CreateGameWindow(
    GameWindow *const window
)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        fprintf(stderr, "%s\n", SDL_INIT_FAILED);
        return 0;
    }

    if (SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window->window, &window->renderer) != 0)
    {
        fprintf(stderr, "%s\n", SDL_CREATE_WINDOW_AND_RENDERER_FAILED);
        SDL_Quit();
        return 0;
    }
    window->timeSinceLastRender = 0;

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
    SDL_RenderSetLogicalSize(window->renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
    SDL_SetRenderDrawColor(window->renderer, 0, 0, 0, 255);
    SDL_ShowCursor(SDL_DISABLE);

    SDL_SetWindowTitle(window->window, GAME_TITLE);

    if (!LoadBitmap(window, &window->font, FONT_FILEPATH, 1)) return 0;
    if (!LoadBitmap(window, &window->snakeSkin.head, SNAKE_HEAD_FILEPATH, 1)) return 0;
    if (!LoadBitmap(window, &window->snakeSkin.body, SNAKE_SKIN_FILEPATH, 0)) return 0;
    if (!LoadBitmap(window, &window->snakeSkin.tail, SNAKE_TAIL_FILEPATH, 1)) return 0;

    SetSectionRects(window);

    SDL_StopTextInput(); // it appears that the text input is enabled by default
    window->textInputActive = 0;

    return 1;
}

int LoadBitmap(
    GameWindow *const window,
    SDL_Texture **texture,
    const char *const filePath,
    const int transparent
)
{
    SDL_Surface *tmpSurface;

    tmpSurface = SDL_LoadBMP(filePath);
    if (tmpSurface == NULL)
    {
        fprintf(stderr, "No bitmap: %s\n", filePath);
        SDL_FreeSurface(tmpSurface);
        return 0;
    }
    if (transparent)
    {
        SDL_SetColorKey(tmpSurface, 1, 0x000000);
    }
    *texture = SDL_CreateTextureFromSurface(window->renderer, tmpSurface);

    SDL_FreeSurface(tmpSurface);
    return 1;
}

// Set bounding boxes for window sections
void SetSectionRects(
    GameWindow *const window
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

// Set game params (related to its logic)
void CreateGame(
    Game *const game
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
    Game *const game
)
{
    int quitRequested = 0;
    
    RenderGameWindow(game);

    while (SDL_PollEvent(&game->window.event))
    {
        switch (game->window.event.type)
        {
            case SDL_TEXTINPUT:
                if (game->window.textInputActive)
                {
                    AppendNewBestPlayerName(&game->bestPlayers, game->window.event.text.text);
                }
                break;
            case SDL_KEYDOWN:
                if (!game->window.textInputActive)
                {
                    quitRequested = ProcessGameKeydowns(game, game->window.event.key.keysym.sym);
                }
                else
                {
                    ConfirmNewBestPlayerName(&game->bestPlayers, game->window.event.key.keysym.sym);
                }
                break;
            case SDL_QUIT:
                quitRequested = 1;
                break;
        }
    }

    ProcessGameLogic(game);

    SDL_Delay(1); // the game loop doesn't need to be executed as fast as CPU can
    return !quitRequested;
}

// Process pressed keys when game is pending
int ProcessGameKeydowns(
    Game *const game,
    const SDL_Keycode pressedKey
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

// Process Enter, Escape and Backspace in leaderboard edit mode
void ConfirmNewBestPlayerName(
    BestPlayers *const bestPlayers,
    const SDL_Keycode pressedKey
)
{
    char *bestPlayerName = bestPlayers->list[bestPlayers->newBestPlayerI].playerName;
    switch (pressedKey)
    {
        case SDLK_ESCAPE:
        case SDLK_RETURN:
            if (pressedKey == SDLK_ESCAPE)
            {
                strcpy(bestPlayerName, BEST_PLAYER_DEFAULT_NAME);
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

// Advance the game, enter and exit leaderboard edit mode
void ProcessGameLogic(
    Game *const game
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
    Game *const game
)
{
    game->window.timeSinceLastRender = 0;
    DestroySnake(&game->snake);
    DestroyBestPlayersList(game->bestPlayers.list);
}

void CloseGameWindow(
    GameWindow *const window
)
{
    SDL_DestroyTexture(window->font);
    SDL_DestroyTexture(window->snakeSkin.body);
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
