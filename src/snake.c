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

    SDL_SetRenderDrawColor(window->renderer, 0, 0, 0, 255); // fill with black color
    SDL_RenderFillRect(window->renderer, NULL);

    SDL_Rect status_rectangle;
    status_rectangle.x = STATUS_MARGIN;
    status_rectangle.y = SCREEN_HEIGHT - STATUS_HEIGHT - STATUS_MARGIN;
    status_rectangle.w = SCREEN_WIDTH - 2 * STATUS_MARGIN;
    status_rectangle.h = STATUS_HEIGHT;
    SDL_SetRenderDrawColor(window->renderer, 26, 95, 180, 255); // fill with blue color
    SDL_RenderFillRect(window->renderer, &status_rectangle);
    SDL_RenderPresent(window->renderer);

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