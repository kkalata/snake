#include "snake.h"

GameTimer InitGameTimer()
{
    GameTimer timer;
    timer.lastTimeMeasure = SDL_GetTicks();
    timer.timeElapsed = 0;
    timer.timeDelta = 0;
    return timer;
}

Uint32 GetTimeDelta(
    GameTimer *const timer
)
{
    Uint32 currentTimeMeasure = SDL_GetTicks();
    timer->timeDelta = currentTimeMeasure - timer->lastTimeMeasure;
    timer->lastTimeMeasure = currentTimeMeasure;
    timer->timeElapsed += timer->timeDelta;
    return timer->timeDelta;
}
