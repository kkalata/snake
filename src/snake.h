#ifndef SNAKE
#define SNAKE

#include <stdio.h>
#include <string.h>
#include <time.h>

#include <SDL.h>

#include "constants.h"

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Event event;
    Uint32 timeSinceLastRender;
    SDL_Texture *font;
    struct {
        SDL_Texture *head;
        SDL_Texture *body;
        SDL_Texture *tail;
    } snakeSkin;
    struct {
        SDL_Rect board;
        SDL_Rect statusSection;
        SDL_Rect leaderboard;
    } rect;
    int textInputActive;
} GameWindow;

typedef enum {
    ALIGN_LEFT,
    ALIGN_RIGHT
} StatusSectionAlignment;

typedef struct {
    Uint32 lastTimeMeasure;
    Uint32 timeElapsed;
    Uint32 timeDelta;
} GameTimer;

typedef struct {
    Uint32 pointsScored;
    char *playerName;
} BestPlayer;

typedef struct {
    BestPlayer list[BEST_PLAYER_COUNT];
    int newBestPlayerI;
    int listUpdated;
} BestPlayers;

typedef struct {
    int x;
    int y;
} Position;

typedef enum {
    SNAKE_NO_TURN = 0,
    SNAKE_UP = 4,
    SNAKE_DOWN = 2,
    SNAKE_LEFT = 3,
    SNAKE_RIGHT = 1
} SnakeDirection;

typedef struct SnakeSegment {
    Position pos;
    SnakeDirection direction;
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
    SnakeDirection turn;
    SnakeKillReason killed;
} Snake;

typedef struct {
    Position pos;
} BlueDot;

typedef enum {
    SNAKE_SHORTENING,
    SNAKE_SLOWING_DOWN,
    BEHAVIOR_COUNT // this one is for easy counting of behaviors
} SnakeBehavior;

typedef struct {
    Position pos;
    Uint32 appearTime;
    Uint8 visible;
    SnakeBehavior snakeBehavior;
} RedDot;

typedef struct {
    GameWindow window;
    GameTimer timer;
    int seed;
    Uint32 pointsScored;
    BestPlayers bestPlayers;
    Snake snake;
    BlueDot blueDot;
    RedDot redDot;
} Game;

int CreateGameWindow(
    GameWindow *const window
);
int LoadBitmap(
    GameWindow *const window,
    SDL_Texture **texture,
    const char *const filePath,
    const int transparent
);
void SetSectionRects(
    GameWindow *const window
);
void CreateGame(
    Game *const game
);
int GameLoop(
    Game *const game
);
int ProcessGameKeydowns(
    Game *const game,
    const SDL_Keycode pressedKey
);
void ConfirmNewBestPlayerName(
    BestPlayers *const bestPlayers,
    const SDL_Keycode pressedKey
);
void ProcessGameLogic(
    Game *const game
);
void DestroyGame(
    Game *const game
);
void CloseGameWindow(
    GameWindow *const window
);

void DrawString(
    GameWindow *const window,
    const int x,
    const int y,
    const char *text
);
void RenderGameWindow(
    Game *const game
);
void RenderBoard(
    GameWindow *const window
);
void RenderSnake(
    GameWindow *const window,
    const Snake *const snake
);
SDL_Texture *GetSnakeSkinFragment(
    const GameWindow *const window,
    const SnakeSegment *const snakeSegment,
    const SnakeSegment *const firstSnakeSegment
);
SDL_Rect GetSnakeSegmentCenterRect(
    const SnakeSegment *const snakeSegment,
    const int isSmallSnakeSegment
);
SDL_Rect GetSnakeSmallSegmentFillRect(
    const SnakeSegment *const snakeSegment,
    const int front
);
SDL_Rect GetSnakeSegmentDestRect(
    const SDL_Rect snakeSegmentSrcRect,
    const SDL_Rect boardRect,
    const Position snakePosition
);
void RenderDot(
    GameWindow *const window,
    const Position pos,
    const GameTimer timer
);
void RenderBlueDot(
    GameWindow *const window,
    const BlueDot *const blueDot,
    const GameTimer timer
);
void RenderRedDot(
    GameWindow *const window,
    const RedDot *const redDot,
    const GameTimer timer
);
void RenderStatusSection(
    GameWindow *const window,
    const Game *const game
);
void RenderStatusSectionInfo(
    GameWindow *const window,
    const char *const content,
    const int line,
    const StatusSectionAlignment alignment
);
void GetSnakeKillReasonInfo(
    char statusSectionContent[],
    const SnakeKillReason snakeKillReason
);
void GetGameKeyGuide(
    char statusSectionContent[],
    const int bestPlayersListUpdated
);
void RenderRedDotAppearTimeBar(
    GameWindow *const window,
    const int descriptionWidth,
    const float timeLeft
);
void RenderLeaderboard(
    GameWindow *const window,
    const BestPlayers *const bestPlayers
);

GameTimer InitGameTimer();
Uint32 GetTimeDelta(
    GameTimer *const timer
);

void SaveGame(
    Game *const game
);
void LoadGame(
    Game *const game
);

void UnsetBestPlayers(
    BestPlayers *const bestPlayers
);
void LoadBestPlayers(
    BestPlayers *const bestPlayers
);
void SaveBestPlayers(
    const BestPlayer bestPlayers[]
);
void PrepareNewBestPlayer(
    BestPlayers *const bestPlayers,
    const Uint32 pointsScored
);
void AppendNewBestPlayerName(
    BestPlayers *const bestPlayers,
    const char *text
);
void DestroyBestPlayersList(
    BestPlayer bestPlayers[]
);

void CreateSnake(
    Snake *const snake
);
void CreateSnakeSegment(
    Snake *const snake,
    const int x,
    const int y,
    const SnakeDirection direction
);
void AttachSnakeSegment(
    Snake *const snake,
    SnakeSegment *snakeSegment
);
int IsSnakeHere(
    const Snake *const snake,
    const Position pos
);
SnakeSegment *GetSnakeSegment(
    const SnakeSegment *const snakeSegment,
    const int moveSnakeSegment,
    const SnakeDirection snakeTurn
);
void TurnSnake(
    Snake *const snake,
    const Uint32 direction
);
void AutoTurnSnake(
    Snake *const snake
);
void MoveSnakeSegment(
    SnakeSegment *const snakeSegment,
    const SnakeDirection snakeTurn
);
void MoveSnake(
    Snake *const snake
);
int EatBlueDot(
    Snake *const snake,
    const BlueDot *const blueDot
);
int EatRedDot(
    Snake *const snake,
    const RedDot *const redDot
);
Uint32 AdvanceSnake(
    Snake *const snake,
    BlueDot *const blueDot,
    RedDot *const redDot,
    const GameTimer timer
);
void KillSnake(
    Snake *const snake
);
void DetachLastSnakeSegment(
    Snake *const snake
);
void DestroySnake(
    Snake *const snake
);

void PlaceDot(
    Position *const dotPos,
    const Snake *const snake,
    const Position otherDotPos
);

void PlaceBlueDot(
    BlueDot *const blueDot,
    const Snake *const snake,
    const RedDot *const redDot
);

void PlaceRedDot(
    RedDot *const redDot,
    const Snake *const snake,
    const BlueDot *const blueDot,
    const GameTimer timer
);
void SetRedDotParams(
    RedDot *const redDot,
    const GameTimer timer
);

#endif
