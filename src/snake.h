#include <stdio.h>
#include <string.h>

// maybe there is a better way to include SDL
#include"../SDL2-2.0.10/include/SDL.h"

#include "constants.h"

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Surface *font;
} GameWindow;

typedef struct {
    Uint32 lastTimeMeasure;
    Uint32 timeElapsed;
    Uint32 timeDelta;
} GameTimer;

typedef struct SnakeSegment {
    int x;
    int y;
    char direction;
    char turn;
    struct SnakeSegment *next;
    struct SnakeSegment *previous;
} SnakeSegment;
typedef struct {
    SnakeSegment *segment;
    Uint32 timeSinceLastMove;
} Snake;

typedef struct {
    GameWindow window;
    GameTimer timer;
    Snake snake;
} Game;

void CreateSnake(
    Snake *const snake
);
void RenderSnake(
    GameWindow *window,
    const Snake *const snake
);
void TurnSnake(
    Snake *snake,
    Uint32 direction
);
void MoveSnake(
    Snake *snake,
    GameTimer timer
);

GameTimer InitGameTimer();
Uint32 GetTimeDelta(
    GameTimer *timer
);
