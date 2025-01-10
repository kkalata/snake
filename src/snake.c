#include "snake.h"

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

    return 1;
}

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

void RenderStatusSection(
    GameWindow *window,
    GameTimer *timer,
    Uint8 snakeKilled
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
    if (snakeKilled)
    {
        sprintf(statusSectionContent, "Snake killed. Press N to retry or ESC to quit.");
        DrawString(window, 16, SCREEN_HEIGHT - 24, statusSectionContent);
    }
}

void CloseGameWindow(
    GameWindow *window
)
{
    SDL_DestroyRenderer(window->renderer);
    SDL_DestroyWindow(window->window);
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

    SDL_SetRenderDrawColor(game->window.renderer, 46, 194, 126, 255); // fill with green color
    SDL_RenderFillRect(game->window.renderer, NULL);
    RenderSnake(&game->window, &game->snake);
    RenderStatusSection(&game->window, &game->timer, game->snake.killed);
    SDL_RenderPresent(game->window.renderer);

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
        KillSnake(&game->snake);
    }
    return !quitRequested;
}

int main(int argc, char **argv)
{
    Game game;
    
    if (!CreateGameWindow(&game.window))
    {
        CloseGameWindow(&game.window);
        return 1;
    }

    CreateSnake(&game.snake);
    game.timer = InitGameTimer();    
    while (GameLoop(&game));

    CloseGameWindow(&game.window);

    return 0;
}