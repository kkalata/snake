#include "../snake.h"

void PlaceBlueDot(
    BlueDot *const blueDot,
    const Snake *const snake,
    const RedDot *const redDot
)
{
    PlaceDot(&blueDot->pos, snake, redDot->pos);
}
