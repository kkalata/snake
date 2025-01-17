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
    struct {
        SDL_Rect board;
        SDL_Rect statusSection;
        SDL_Rect leaderboard;
    } rect;
    int textInputActive;
} GameWindow;

typedef struct {
    Uint32 lastTimeMeasure;
    Uint32 timeElapsed;
    Uint32 timeDelta;
} GameTimer;

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

typedef enum {
    ALIGN_LEFT,
    ALIGN_RIGHT
} StatusSectionAlignment;

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
    GameWindow window;
    GameTimer timer;
    int seed;
    Uint32 pointsScored;
    BestPlayers bestPlayers;
    Snake snake;
    BlueDot blueDot;
    RedDot redDot;
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
void SetSectionRects(
    GameWindow *window
);
void CreateGame(
    Game *game
);
int GameLoop(
    Game *game
);
int ProcessGameKeydowns(
    Game *game,
    SDL_Keycode pressedKey
);
void ConfirmNewBestPlayerName(
    BestPlayers *bestPlayers,
    SDL_Keycode pressedKey
);
void ProcessGameLogic(
    Game *game
);
void RenderGameWindow(
    Game *game
);
void RenderBoard(
    GameWindow *window
);
void RenderStatusSection(
    GameWindow *window,
    GameTimer *timer,
    Uint32 pointsScored,
    SnakeKillReason snakeKillReason,    
    const RedDot *redDot,
    const int bestPlayersListUpdated
);
void RenderStatusSectionInfo(
    GameWindow *window,
    char *content,
    int line,
    StatusSectionAlignment alignment
);
void GetSnakeKillReasonInfo(
    char statusSectionContent[],
    SnakeKillReason snakeKillReason
);
void GetGameKeyGuide(
    char statusSectionContent[],
    int bestPlayersListUpdated
);
void RenderRedDotAppearTimeBar(
    GameWindow *window,
    const int descriptionWidth,
    const float timeLeft
);
void RenderLeaderboard(
    GameWindow *window,
    BestPlayers *bestPlayers
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
    const SnakeDirection direction
);
void AttachSnakeSegment(
    Snake *const snake,
    SnakeSegment *snakeSegment
);
void RenderSnake(
    GameWindow *window,
    const Snake *const snake
);
void MoveSnake(
    Snake *snake
);
Uint32 AdvanceSnake(
    Snake *snake,
    BlueDot *blueDot,
    RedDot *redDot,
    GameTimer timer
);
void TurnSnake(
    Snake *snake,
    Uint32 direction
);
int IsSnakeHere(
    Snake *snake,
    Position pos
);
void AutoTurnSnake(
    Snake *snake
);
SnakeSegment *GetSnakeSegment(
    SnakeSegment *snakeSegment,
    int moveSnakeSegment,
    SnakeDirection snakeTurn
);
int EatBlueDot(
    Snake *snake,
    BlueDot *blueDot
);
int EatRedDot(
    Snake *snake,
    RedDot *redDot
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

void PlaceDot(
    Position *dotPos,
    Snake *snake,
    Position otherDotPos
);
void RenderDot(
    GameWindow *window,
    const Position pos
);

void PlaceBlueDot(
    BlueDot *blueDot,
    Snake *snake,
    RedDot *redDot
);
void RenderBlueDot(
    GameWindow *window,
    BlueDot *blueDot
);

void PlaceRedDot(
    RedDot *redDot,
    Snake *snake,
    BlueDot *blueDot,
    GameTimer timer
);
void SetRedDotParams(
    RedDot *redDot,
    GameTimer timer
);
void RenderRedDot(
    GameWindow *window,
    RedDot *redDot
);

GameTimer InitGameTimer();
Uint32 GetTimeDelta(
    GameTimer *timer
);

void SaveGame(
    Game *game
);
void SaveBestPlayers(
    BestPlayer bestPlayers[]
);
void LoadGame(
    Game *game
);
void UnsetBestPlayers(
    BestPlayers *bestPlayers
);
void LoadBestPlayers(
    BestPlayers *bestPlayers
);

void PrepareNewBestPlayer(
    BestPlayers *bestPlayers,
    Uint32 pointsScored
);
void AppendNewBestPlayerName(
    BestPlayers *bestPlayers,
    char *text
);
void DestroyBestPlayersList(
    BestPlayer bestPlayers[]
);
