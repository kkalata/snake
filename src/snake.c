#include "snake.h"

SDL_Rect GetTextRect(
    const int x,
    const int y,
    const int textWidth
)
{
    SDL_Rect TextRect;
    TextRect.x = x;
    TextRect.y = y;
    TextRect.w = textWidth;
    TextRect.h = CHAR_SIZE;
    return TextRect;
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

    return 1;
}

void CreateGame(
    Game *game
)
{
    CreateSnake(&game->snake);
    game->timer = InitGameTimer();
}

int GameLoop(
    Game *game
)
{
    SDL_Event event;
    int quitRequested = 0;

    if (!game->snake.killed)
    {
        GetTimeDelta(&game->timer);
    }
    
    RenderGameWindow(game);

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                    case SDLK_ESCAPE:
                        quitRequested = 1;
                        break;
                    case SDLK_n:
                        DestroyGame(game);
                        CreateGame(game);
                        return !quitRequested;
                    case SDLK_UP:
                    case SDLK_DOWN:
                    case SDLK_LEFT:
                    case SDLK_RIGHT:
                        if (!game->snake.killed)
                        {
                            TurnSnake(&game->snake, event.key.keysym.sym);
                        }
                        break;
                }
                break;
            case SDL_QUIT:
                quitRequested = 1;
                break;
        }
    }
    if (!game->snake.killed)
    {
        MoveSnake(&game->snake, game->timer);
    }
    SDL_Delay(1);
    return !quitRequested;
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
        RenderSnake(&game->window, &game->snake, &game->boardRect);
        RenderStatusSection(&game->window, &game->timer, game->snake.killed);
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
    SnakeKillReason snakeKillReason
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
    DrawString(window, 16, SCREEN_HEIGHT - 40, statusSectionContent);

    sprintf(statusSectionContent, IMPLEMENDED_REQUIREMENTS);
    DrawString(
        window,
        SCREEN_WIDTH - strlen(statusSectionContent) * CHAR_SIZE - 16, // align to the right
        SCREEN_HEIGHT - 40,
        statusSectionContent
    );
    
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
        strcat(statusSectionContent, " Press N to retry or ESC to quit.");
        
        DrawString(window, 16, SCREEN_HEIGHT - 24, statusSectionContent);
    }
}

void DestroyGame(
    Game *game
)
{
    game->window.timeSinceLastRender = 0;
    DestroySnake(&game->snake);
}

void CloseGameWindow(
    GameWindow *window
)
{
    SDL_FreeSurface(window->font);
    SDL_DestroyTexture(window->snakeSkin);
    SDL_DestroyRenderer(window->renderer);
    SDL_DestroyWindow(window->window);
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