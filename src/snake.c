#include "snake.h"

int create_window(
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

    return 1;
}

void close_window(
    GameWindow *window
)
{
    SDL_DestroyRenderer(window->renderer);
    SDL_DestroyWindow(window->window);
}

int game_loop(
    GameWindow *window
)
{
    SDL_Event event;
    int quit_requested = 0;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                    case SDLK_ESCAPE:
                        quit_requested = 1;
                        break;
                }
                break;
            case SDL_QUIT:
                quit_requested = 1;
                break;
        }
    }
    return !quit_requested;
}

int main(int argc, char **argv)
{
    GameWindow window;
    
    if (!create_window(&window))
    {
        return 1;
    }
    
    while (game_loop(&window));

    close_window(&window);

    return 0;
}