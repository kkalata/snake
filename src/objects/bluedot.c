#include "../snake.h"

void PlaceBlueDot(
    BlueDot *blueDot,
    Snake *snake,
    RedDot *redDot
)
{
    PlaceDot(&blueDot->pos, snake, redDot->pos);
}
