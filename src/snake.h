#include <stdio.h>
#include <string.h>
#include <time.h>

// maybe there is a better way to include SDL
#include"../SDL2-2.0.10/include/SDL.h"

#include "constants.h"

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    Uint32 timeSinceLastRender;
    SDL_Surface *font;
    SDL_Texture *snakeSkin;
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

typedef enum {
    ALIVE,
    UNSPECIFIED,
    HIT_ITSELF,
    HIT_WALL
} SnakeKillReason;
typedef struct {
    SnakeSegment *segment;
    Uint32 timeSinceLastMove;
    Uint32 timeSinceLastSpeedup;
    float cooldown;
    SnakeKillReason killed;
} Snake;

typedef struct {
    int x;
    int y;
} BlueDot;

typedef struct {
    GameWindow window;
    SDL_Rect boardRect;
    GameTimer timer;
    Snake snake;
    BlueDot blueDot;
} Game;

SDL_Rect GetTextRect(
    const int x,
    const int y,
    const int textWidth
);
void DrawString(
    GameWindow *window,
    const int x,
    const int y,
    const char *text
);
int CreateGameWindow(
    GameWindow *window
);
void CreateGame(
    Game *game
);
int GameLoop(
    Game *game
);
void RenderGameWindow(
    Game *game
);
void RenderBoard(
    GameWindow *window,
    SDL_Rect *boardRect
);
void RenderStatusSection(
    GameWindow *window,
    GameTimer *timer,
    SnakeKillReason snakeKillReason
);
void DestroyGame(
    Game *game
);
void CloseGameWindow(
    GameWindow *window
);

void CreateSnake(
    Snake *const snake
);
void CreateSnakeSegment(
    Snake *const snake,
    const int x,
    const int y,
    const int direction
);
void AttachSnakeSegment(
    Snake *const snake,
    SnakeSegment *snakeSegment
);
void RenderSnake(
    GameWindow *window,
    const Snake *const snake,
    const SDL_Rect *const boardRect
);
void MoveSnake(
    Snake *snake,
    BlueDot *blueDot,
    GameTimer timer
);
void TurnSnake(
    Snake *snake,
    Uint32 direction
);
void AutoTurnSnakeSegment(
    SnakeSegment *snakeSegment
);
SnakeSegment *GetSnakeSegment(
    SnakeSegment *snakeSegment,
    int moveSnakeSegment
);
int EatBlueDot(
    Snake *snake,
    BlueDot *blueDot
);
void KillSnake(
    Snake *snake
);
void DetachLastSnakeSegment(
    Snake *snake
);
void DestroySnake(
    Snake *snake
);

void PlaceBlueDot(
    BlueDot *blueDot,
    Snake *snake
);
void RenderBlueDot(
    GameWindow *window,
    BlueDot *blueDot,
    const SDL_Rect *const boardRect
);

GameTimer InitGameTimer();
Uint32 GetTimeDelta(
    GameTimer *timer
);
