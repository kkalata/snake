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

    window->font = SDL_LoadBMP(FONT_FILEPATH);
    if (window->font == NULL)
    {
        return 0;
    }
    SDL_SetColorKey(window->font, 1, 0x000000);

    return 1;
}

SDL_Rect get_text_rect(
    const int x,
    const int y,
    const int text_width
)
{
    SDL_Rect text_rect;
    text_rect.x = x;
    text_rect.y = y;
    text_rect.w = text_width;
    text_rect.h = CHAR_SIZE;
    return text_rect;
}

void draw_string(
    GameWindow *window,
    const int x,
    const int y,
    const char *text
)
{
    int text_width = strlen(text) * CHAR_SIZE; 

    SDL_Rect char_src_rect, char_dest_rect;
    char_dest_rect.x = char_dest_rect.y = 0;
    char_src_rect.w = char_dest_rect.w = CHAR_SIZE;
    char_src_rect.h = char_dest_rect.w = CHAR_SIZE;

    SDL_Surface *text_subscreen_cpu = SDL_CreateRGBSurface(
        0,
        text_width, CHAR_SIZE,
        32,
        0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000
    );

    while (*text) // copy char by char
    {
        int char_code = *text & 255; // ASCII has max char code of 255
        char_src_rect.x = (char_code % FONT_ROW_SIZE) * CHAR_SIZE; // find row of the char
        char_src_rect.y = (char_code / FONT_ROW_SIZE) * CHAR_SIZE; // find column of the char
        SDL_BlitSurface(window->font, &char_src_rect, text_subscreen_cpu, &char_dest_rect);
        char_dest_rect.x += CHAR_SIZE;
        text++;
    }

    SDL_Texture *text_subscreen_gpu = SDL_CreateTextureFromSurface(window->renderer, text_subscreen_cpu);
    SDL_Rect text_dest_rect = get_text_rect(x, y, text_width);
    SDL_RenderCopy(window->renderer, text_subscreen_gpu, NULL, &text_dest_rect);
    SDL_FreeSurface(text_subscreen_cpu);
    SDL_DestroyTexture(text_subscreen_gpu);
}

void render_status_section(
    GameWindow *window
)
{
    SDL_Rect status_rectangle;
    status_rectangle.x = STATUS_MARGIN;
    status_rectangle.y = SCREEN_HEIGHT - STATUS_HEIGHT - STATUS_MARGIN;
    status_rectangle.w = SCREEN_WIDTH - 2 * STATUS_MARGIN;
    status_rectangle.h = STATUS_HEIGHT;
    
    SDL_SetRenderDrawColor(window->renderer, 192, 28, 40, 255); // fill with red color
    SDL_RenderFillRect(window->renderer, &status_rectangle);
    draw_string(window, 16, SCREEN_HEIGHT - 32, GAME_TITLE);
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

    SDL_SetRenderDrawColor(window->renderer, 46, 194, 126, 255); // fill with green color
    SDL_RenderFillRect(window->renderer, NULL);
    render_status_section(window);
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
        close_window(&window);
        return 1;
    }
    
    while (game_loop(&window));

    close_window(&window);

    return 0;
}